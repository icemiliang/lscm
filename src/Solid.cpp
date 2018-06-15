#include <string.h>

#include "Vertex.h"
#include "HalfEdge.h"
#include "Edge.h"
#include "Face.h"
#include "Solid.h"
#include <fstream>
#include <assert.h>
#include <vector>
#include <algorithm>
//#include "map.h"
#include "Iterators.h"

#include "TopologyException.h"
#include "FException.h"

#include "FaceNormalTrait.h"

using namespace MeshLib;

//!  Solid.
/*!
  This class define solid(mesh) structure.
*/
Solid::tFace Solid::createFace( int * v , int id )
{
	tFace f = createFace( id );		
	//create halfedges
	tHalfEdge hes[3];


	int i;
	for( i = 0; i < 3; i ++ )
	{
		hes[i] = new HalfEdge;

		Vertex * vert =  idVertex( v[i] );
		hes[i]->vertex() = vert;
		vert->halfedge() = hes[i];
	}

	//linking to each other
	for( i = 0; i < 3; i ++ )
	{
		hes[i]->he_next() = hes[(i+1)%3];
		hes[i]->he_prev() = hes[(i+2)%3];
	}

	//linking to face
	for( i = 0; i < 3; i ++ )
	{
		hes[i]->face()   = f;
		f->halfedge()    = hes[i];
	}

	//connecting with edge
	//this part is modified: fix the constraint " the target vertex id is 
	//greater than the source vertex id of halfedge(0) with interior edge "
	for( i = 0; i < 3; i ++ )
	{
		tEdge e = createEdge( v[i], v[(i+2)%3] );
		if( e->halfedge(0)  == NULL )
		{
			e->halfedge(0) = hes[i];
			hes[i]->edge() = e;
		}
		else
		{
			if( e->halfedge(1) != NULL ) throw TopologyException();
			e->halfedge(1) = hes[i];
			hes[i]->edge() = e;
			
			if( e->halfedge(0)->target()->id() < e->halfedge(0)->source()->id() )
			{
				HalfEdge * temp = e->halfedge(0);
				e->halfedge(0 ) = e->halfedge(1);
				e->halfedge(1 ) = temp;
			}
			
		}
	}

	return f;
}
//this function should always be the last to execute
//the vertex should always be isolated before destroy
void Solid::destroyVertex(Vertex * vertex)
{
    assert(vertex->halfedge () == NULL);
	assert(vertex->trait () == NULL);

	m_verts.remove (vertex);
    delete vertex;
}

//note this detroy will generate isolated vertex
void Solid::destroyEdge (Edge * edge)
{
	assert(edge->halfedge (0) == NULL && edge->halfedge (1) == NULL);
	assert(edge->trait () == NULL);
	m_edges.remove (edge);
	delete edge;
}

//note this function also can not manipulate the generation of isolated vertex
//this function assume no vertex has link to the halfedge to be destroyed
void Solid::destroyHalfEdge(HalfEdge * he)
{
	assert(he->target ()->halfedge () != he);
	assert(he->trait () == NULL);
	HalfEdge * hes = he->he_sym ();
	if(hes != NULL){
		hes->he_sym () = NULL;
		if(hes == hes->edge ()->halfedge (1)){
			Edge * e = hes->edge ();
			e->halfedge (0) = hes;
			e->halfedge (1) = NULL;
		}
	}
	else{
		Edge * e = he->edge ();
		e->halfedge (0) = NULL;
		e->halfedge (1) = NULL;
		destroyEdge(e);
	}
	HalfEdge * hep = he->he_prev ();
	HalfEdge * hen = he->he_next ();
	if(hep!= NULL) hep->he_next () = NULL;
	if(hen!= NULL) hen->he_prev () = NULL;
	if(he->face ()->halfedge () == he)
		he->face ()->halfedge () = he->he_next ();
	delete he;
}

//this will happen only after all the halfedge on the face is deleted
void Solid::destroyFace (Face * face)
{
	assert(face->trait () == NULL);
	HalfEdge * he = face->halfedge ();
	for(int i=0; i<3; i++){
		HalfEdge * hen = he->he_next ();
		assert(he->target ()->halfedge () == NULL || he->target ()->halfedge ()->face () != face);
		destroyHalfEdge(he);
		he = hen;
	}
	assert(face->halfedge () == NULL);
	m_faces.remove (face);
	delete face;
}


void Solid::collapseEdge (Edge * edge)
{
	collapseEdgeVertex(edge,edge->halfedge (0)->source () );
}

bool Solid::collapsable (HalfEdge * halfedge)
{
	Edge * edge = halfedge->edge ();
	Vertex * source = halfedge->source ();
	Vertex * target = halfedge->target ();
	Face * fL = halfedge->face ();
	Face * fR = halfedge->he_sym ()!= NULL?halfedge->he_sym()->face ():NULL;
	Vertex * vL = halfedge->he_next ()->target ();
	Vertex * vR = halfedge->he_sym ()!= NULL? halfedge->he_sym ()->he_next ()->target ():NULL;
	std::vector <HalfEdge *> thlist;
	for( VertexInHalfedgeIterator vhiter( this, target );!vhiter.end(); ++ vhiter )
	{
		HalfEdge * he = *vhiter;

		if( he->face() == fL ) continue;
		if( he->face() == fR ) continue;

		thlist.push_back ( he );
	}

	for( VertexInHalfedgeIterator svhiter( this, source );!svhiter.end(); ++ svhiter )
	{
		HalfEdge * he = *svhiter;

		if( he->face() == fL ) continue;
		if( he->face() == fR ) continue;

		thlist.push_back ( he );
	}
	if(thlist.size () == 0) return false;

	thlist.clear ();
	for( VertexInHalfedgeIterator lvhiter( this, vL );!lvhiter.end(); ++ lvhiter )
	{
		HalfEdge * he = *lvhiter;

		if( he->face() == fL ) continue;
		if( he->face() == fR ) continue;

		thlist.push_back ( he );
	}
	if(thlist.size () == 0) return false;
	if( vR != NULL )
	{
		thlist.clear ();
		for( VertexInHalfedgeIterator rvhiter( this, vR );!rvhiter.end(); ++ rvhiter )
		{
			HalfEdge * he = *rvhiter;

			if( he->face() == fL ) continue;
			if( he->face() == fR ) continue;

			thlist.push_back ( he );
		}
		if(thlist.size () == 0) return false;
	}

	if( source->boundary() && target->boundary() && !edge->boundary() )
		return false;

	List<Vertex> verts;

	for( VertexVertexIterator vviter( source ); !vviter.end(); ++ vviter )
	{
		Vertex * v = *vviter;
		if( v == vL ) continue;
		if( v == vR ) continue;
		verts.Append( v );
	}

	for( VertexVertexIterator viter(  target ); !viter.end(); ++ viter )
	{
		Vertex * v = *viter;
		if( verts.contains( v ) ) return false;
	}

	return true;
}

void Solid::collapseEdgeVertex (Edge * edge, Vertex * vertex)
{
	List<Edge> changekey;
	Vertex * vt = edge->other_vertex (vertex);
	for(VertexEdgeIterator eiter(vt); !eiter.end (); ++eiter){
		Edge * e = *eiter;
		if(edge->halfedge (0)->face ()->include_edge (e))
			continue;
		if(edge->halfedge (1)!= NULL && edge->halfedge (1)->face ()->include_edge (e))
			continue;
		changekey.Append (e);
	}
	List<Face> del_f;
	del_f.Append (edge->halfedge (0)->face ());
	if(edge->halfedge (1) != NULL)
		del_f.Append (edge->halfedge (1)->face ());
	List<HalfEdge> retain_he;
	List<HalfEdge> sym_he;
	for(int i=0; i<2; i++){
		HalfEdge * he = edge->halfedge (i);
		if(he!= NULL){
			if(he->target () == vertex){
				retain_he.Append (he->he_next ()->he_sym ());
				sym_he.Append (he->he_prev ()->he_sym ());
			}
			else{
				retain_he.Append (he->he_prev ()->he_sym ());
				sym_he.Append (he->he_next ()->he_sym ());
			}
		}
	}
	std::vector <Vertex * > iso_v;
	for(ListIterator<Face> fiter(del_f); !fiter.end (); ++fiter){
		Face * face = *fiter;
		HalfEdge * he = face->halfedge ();
		for(int i=0; i<3; i++){
			if(he->target ()->halfedge () == he){
				Vertex * v = he->target ();
				HalfEdge * ohe = NULL;
				for(VertexInHalfedgeIterator hiter(this, v);!hiter.end (); ++hiter){
					HalfEdge * heo = *hiter;
					if(!del_f.contains (heo->face ())){
						ohe = heo;
						break;
					}
				}
				if(ohe == NULL){
					v->halfedge () = NULL;
					iso_v.push_back (v);
				}
				else{
					v->halfedge () = ohe;
				}
			}
			he = he->he_next ();
		}
	}
	for(ListIterator<Face> fiter(del_f); !fiter.end (); ++fiter){
		Face * f = *fiter;
		destroyFace(f);
	}
	ListIterator<HalfEdge> siter(sym_he);
	for(ListIterator<HalfEdge> hiter(retain_he); !hiter.end () && !siter.end (); ++hiter, ++siter){
		HalfEdge * he = *hiter;
		HalfEdge * se = *siter;
		if(he == NULL && se == NULL) continue;
		if(he != NULL && se != NULL){
			he->he_sym () = se;
			Edge * e = se->edge ();
			for(int i=0; i<2; i++){
				HalfEdge * hes = e->halfedge (i);
				if(hes!=NULL){
					if(hes->target () == vt){
						hes->target () = vertex;
					}
					else if(hes->source () == vt)
						hes->source () = vertex;
					if(hes->target () == vertex && vertex->halfedge () == NULL) vertex->halfedge () = hes;
					hes->edge () = he->edge ();
				}
			}
			e->halfedge (0) = NULL;
			e->halfedge (1) = NULL;
			destroyEdge(e);
		}
		else if(he!= NULL && se == NULL){
			he->he_sym () = NULL;
		}
		else{
			assert(he == NULL && se != NULL);
			changekey.Append (se->edge ());
		}
	}

	for(ListIterator<Edge> eiter(changekey); !eiter.end (); ++eiter){
		Edge * e = *eiter;
		m_edges.remove (e);
		if(e->ekey ().s () == vt->id ()){
			e->ekey () = EdgeKey(vertex->id (),e->ekey ().t ());
		}
		else if(e->ekey ().t () == vt->id ())
			e->ekey () = EdgeKey(e->ekey ().s (),vertex->id ());
		assert(m_edges.find (e) == NULL);
		m_edges.insert (e);
		for(int i=0; i<2; i++){
			HalfEdge * he = e->halfedge (i);
			if(he != NULL){
				if(he->target () == vt)
					he->target () = vertex;
				else if(he->source () == vt)
					he->source () = vertex;
				if(he->target () == vertex && vertex->halfedge () == NULL)
					vertex->halfedge () = he;
			}
		}
	}
	std::vector<Vertex *>::iterator iter = std::find(iso_v.begin (),iso_v.end (),vt);
	if( iter == iso_v.end ()){
		vt->halfedge () = NULL;
		destroyVertex(vt);
	}

	for(int i=0; i<(int)iso_v.size (); i++){
		Vertex * v = iso_v[i];
		if(v!= vertex)
			destroyVertex(v);
	}
}

int Solid::numVertices() 
{
	/*
	
	int sum = 0;

	AVL::TreeIterator<Vertex> viter( m_verts );
	for( ; !viter.end(); ++viter  )
	{
		sum ++;
	}
	return sum;
	
	*/
	return m_verts.getSize();
}

int Solid::numEdges() 
{
/*
	int sum = 0;

	AVL::TreeIterator<Edge> eiter( m_edges );
	for( ; !eiter.end(); ++eiter )
	{
		sum ++;
	}
	return sum;
*/
	
	return m_edges.getSize();
}

int Solid::numFaces() 
{
	/*
	int sum = 0;

	AVL::TreeIterator<Face> fiter( m_faces );
	for( ; !fiter.end(); ++fiter )
	{
		sum ++;
	}
	return sum;
	*/

	return m_faces.getSize();
}


Solid::~Solid()
{
	//remove vertices
	AVL::TreeIterator<Vertex> viter( m_verts );
	for( ; !viter.end(); ++viter )
	{
		tVertex v = *viter;
		if( v->trait() != NULL )
		{
			v->trait()->clear( v->trait() );
		}
		delete v;
	}

	//remove faces
	AVL::TreeIterator<Face> fiter( m_faces );
	for( ; !fiter.end(); ++fiter )
	{
		tFace f = *fiter;

		tHalfEdge he = f->halfedge();

		List<HalfEdge> hes;

		do{
			he = he->he_next();
			hes.Append(he);
		}while( he != f->halfedge() );

		ListNode<HalfEdge> * node = hes.head();
		do{
			if( node->data()->trait() != NULL )
			{
				node->data()->trait()->clear( node->data()->trait() );
			}
			delete node->data();
			node = node->next();
		}while( node != NULL );
		
		if( f->trait() != NULL )
		{
			f->trait()->clear( f->trait() );
		}

		delete f;
	}
	
	//remove edges
	AVL::TreeIterator<Edge> eiter( m_edges );
	for( ; !eiter.end(); ++ eiter )
	{
		tEdge e = *eiter;
		if( e->trait() != NULL )
		{
			e->trait()->clear( e->trait() );
		}
		delete e;
	}
}

void Solid::write( std::ostream & os )
{

	//remove vertices
	AVL::TreeIterator<Vertex> viter( m_verts );
	for( ; !viter.end() ; ++ viter )
	{
		tVertex v = *viter;

		os << "Vertex " << v->id();
		
		for( int i = 0; i < 3; i ++ )
		{
			os << " " << v->point()[i];  
		}


		if( v->string().size() > 0 )
		{
			os << " {" << v->string() << "}";
		}

		os << std::endl;
	}

	for( AVL::TreeIterator<Face> fiter(m_faces); !fiter.end(); ++ fiter )
	{
		tFace f = *fiter;
		os << "Face " << f->id();
		tHalfEdge he = f->halfedge();
		do{
			os << " " << he->target()->id();
			he = he->he_next();
		}while( he != f->halfedge() );

		if( f->string().size() > 0 )
		{
			os << " {" << f->string() << "}";
		}

		os << std::endl;
	}


	for( AVL::TreeIterator<Edge> eiter(m_edges); !eiter.end(); ++ eiter )
	{
		tEdge e = *eiter;
		if( e->string().size() > 0 )
		{
			os << "Edge " << edgeVertex1(e)->id()<< " " << edgeVertex2(e)->id() << " ";
			os << "{" << e->string() << "}" << std::endl;
		}
	}

	for( AVL::TreeIterator<Face> pfiter(m_faces); !pfiter.end(); ++ pfiter )
	{
		tFace f = *pfiter;
		tHalfEdge he = f->halfedge();
		do{
			if( he->string().size() > 0 )
			{
				os << "Corner " << he->vertex()->id() << " " << he->face()->id() << " ";
				os <<"{" << he->string() << "}" << std::endl;
			}
			he = he->he_next();
		}while( he != f->halfedge() );
	}
}

void Solid::read( std::istream & is )
{
	char line[MAX_LINE];
	int id;

	while( is && !is.eof() && is.getline(line, MAX_LINE) )
	{		
		if( strlen( line ) == 0 ) continue;

		std::string s(line);

		string_token_iterator iter(s, " \n"); 
		
		std::string str = *iter;

		if( str == "Vertex" ) 
		{
			
			str = *(++iter);

			id = atoi( str.c_str() );


			Point p;
			for( int i = 0 ; i < 3; i ++ )
			{
				str = *(++iter);
				p[i] = atof( str.c_str() );
			}
		
			tVertex v  = createVertex( id );
			
			v->point() = p;
			v->id()    = id;

			int sp = (int)s.find("{");
			int ep = (int)s.find("}");

			if( sp >= 0 && ep >= 0 )
			{
				v->string() = s.substr( sp+1, ep-sp-1 );
			}

			continue;

		}

		if( str == "Face" ) 
		{
			str = *(++iter);

			id = atoi( str.c_str() );
	
			int v[3];
			for( int i = 0; i < 3; i ++ )
			{
				str = *(++iter);
				v[i] = atoi( str.c_str() );
			}

			tFace f = createFace( v, id );
			

			int sp = (int)s.find("{");
			int ep = (int)s.find("}");

			if( sp >= 0 && ep >= 0 )
			{
				f->string() = s.substr( sp+1, ep-sp-1 );
			}
			continue;
		}

		//read in edge attributes
		if( str == "Edge" )
		{
			str = *(++iter);
			int id0 = atoi( str.c_str() );

			str = *(++iter);
			int id1 = atoi( str.c_str() );

			tEdge edge = idEdge( id0, id1 );
			
			str = *(++iter);

			int sp = (int)s.find("{");
			int ep = (int)s.find("}");

			if( sp >= 0 && ep >= 0 )
			{
				edge->string() = s.substr( sp+1, ep-sp-1 );
			}
			
			continue;

		}

		//read in edge attributes
		if( str == "Corner" ) 
		{
			str = *(++iter);
			int id0 = atoi( str.c_str() );
				
			str = *(++iter);
			int id1 = atoi( str.c_str() );


			Vertex * v = idVertex( id0 );
			Face   * f = idFace( id1 );
			tHalfEdge he = corner( v, f );

			str = *(++iter);

			int sp = (int)s.find("{");
			int ep = (int)s.find("}");

			if( sp >= 0 && ep >= 0 )
			{
				he->string() = s.substr( sp+1, ep-sp-1 );
			}
			continue;
		}


	}

	labelBoundaryEdges();

	removeDanglingVertices();

}


void Solid::read( const char * input )
{
	FILE * is = fopen( input, "r" );
	if( is == NULL )
	{
		fprintf(stderr,"Error is opening file %s\n", input );
		return;
	}

	char line[MAX_LINE];
	int id;

	while( !feof(is)  )
	{		
		if (fgets( line, MAX_LINE, is ) != NULL) {
		if(  feof(is) || strlen( line ) == 0  ) break;

		char * str = strtok( line, " \r\n\t");
		if( strcmp(str, "Vertex" ) == 0 ) 
		{
			
			str = strtok(NULL," \r\n\t");

			id = atoi( str );


			Point p;
			for( int i = 0 ; i < 3; i ++ )
			{
				str = strtok(NULL," \r\n\t");
				p[i] = atof( str );
			}
		
			tVertex v  = createVertex( id );
			
			v->point() = p;
			v->id()    = id;

			str = strtok( NULL, "\r\n\t");

			if( str == NULL ) continue;

			std::string s(str);
			int sp = (int)s.find("{");
			int ep = (int)s.find("}");

			if( sp >= 0 && ep >= 0 )
			{
				v->string() = s.substr( sp+1, ep-sp-1 );
			}

			continue;

		}
		

		if( strcmp(str,"Face") == 0 ) 
		{
			str = strtok(NULL, " \r\n\t");

			id = atoi( str );
			
	
			int v[3];
			for( int i = 0; i < 3; i ++ )
			{
				str = strtok(NULL," \r\n\t");
				v[i] = atoi( str );
			}

			tFace f = createFace( v, id );
			

			str = strtok( NULL, "\r\n\t");
			if(  str== NULL || strlen( str ) == 0 ) continue;

			std::string s(str);

			int sp = (int)s.find("{");
			int ep = (int)s.find("}");

			if( sp >= 0 && ep >= 0 )
			{
				f->string() = s.substr( sp+1, ep-sp-1 );
			}
			continue;
		}

		//read in edge attributes
		if( strcmp(str,"Edge")==0 )
		{
			str = strtok(NULL, " \r\n\t");
			int id0 = atoi( str );

			str = strtok(NULL, " \r\n\t");
			int id1 = atoi( str );

			tEdge edge = idEdge( id0, id1 );
			

			str = strtok(NULL, "\r\n\t");

			std::string s(str);
			int sp = (int)s.find("{");
			int ep = (int)s.find("}");

			if( sp >= 0 && ep >= 0 )
			{
				edge->string() = s.substr( sp+1, ep-sp-1 );
			}
			
			continue;

		}

		//read in edge attributes
		if( strcmp(str,"Corner")==0 ) 
		{
			str = strtok(NULL," \r\n\t");
			int id0 = atoi( str );
				
			str = strtok(NULL," \r\n\t");
			int id1 = atoi( str );


			Vertex * v = idVertex( id0 );
			Face   * f = idFace( id1 );
			tHalfEdge he = corner( v, f );

			str = strtok(NULL,"\r\n\t");
			std::string s(str);

			int sp = (int)s.find("{");
			int ep = (int)s.find("}");

			if( sp >= 0 && ep >= 0 )
			{
				he->string() = s.substr( sp+1, ep-sp-1 );
			}
			continue;
		}
	}

	}

	
	labelBoundaryEdges();
	removeDanglingVertices();
}


//Euler operation

Solid::tHalfEdge Solid::vertexMostClwOutHalfEdge( Solid::tVertex  v )
{
	return v->most_clw_out_halfedge();
}

Solid::tHalfEdge Solid::vertexMostCcwOutHalfEdge( Solid::tVertex  v )
{
	return v->most_ccw_out_halfedge();
}

Solid::tHalfEdge  Solid::corner( tVertex v, tFace f)
{
	Solid::tHalfEdge he = f->halfedge();
	do{
		if( he->vertex() == v )
			return he;
		he = he->he_next();
	}while( he != f->halfedge() );
	return NULL;
}

Solid::tHalfEdge Solid::vertexNextCcwOutHalfEdge( Solid::tHalfEdge  he )
{
	return he->ccw_rotate_about_source();
}

Solid::tHalfEdge Solid::vertexNextClwOutHalfEdge( Solid::tHalfEdge  he )
{
	if( he->he_sym() == NULL ) throw TopologyException();
	return he->clw_rotate_about_source();
}

Solid::tHalfEdge Solid::vertexMostClwInHalfEdge( Solid::tVertex  v )
{
	return v->most_clw_in_halfedge();
}

Solid::tHalfEdge Solid::vertexMostCcwInHalfEdge( Solid::tVertex  v )
{
	return v->most_ccw_in_halfedge();
}

Solid::tHalfEdge Solid::vertexNextCcwInHalfEdge( Solid::tHalfEdge  he )
{
	if( he->he_sym() == NULL ) throw TopologyException();;
	return he->ccw_rotate_about_target();
}

Solid::tHalfEdge Solid::vertexNextClwInHalfEdge( Solid::tHalfEdge  he )
{
	return he->clw_rotate_about_target();
}

Solid::tHalfEdge Solid::faceNextClwHalfEdge( Solid::tHalfEdge  he )
{
	return he->he_prev();
}

Solid::tHalfEdge Solid::faceNextCcwHalfEdge( Solid::tHalfEdge  he )
{
	return he->he_next();
}

Solid::tHalfEdge Solid::faceMostCcwHalfEdge( Solid::tFace  face )
{
	return face->halfedge();
}

Solid::tHalfEdge Solid::faceMostClwHalfEdge( Solid::tFace  face )
{
	return face->halfedge()->he_next();
}


//access id->v
Solid::tVertex Solid::idVertex( int id ) 
{
	Vertex v;
	v.id() = id;
	return m_verts.find( &v );
}

//access v->id
int Solid::vertexId( Solid::tVertex  v )
{
	return v->id();
}

//access id->f
Solid::tFace Solid::idFace( int id )
{
	Face f;
	f.id() = id;

	return m_faces.find( &f );
}

//acess f->id
int Solid::faceId( Solid::tFace  f )
{
	return f->id();
}


//access id->edge
Solid::tEdge Solid::idEdge( int id0, int id1 )
{
	Edge e(id0,id1);
	return  m_edges.find( &e );//insert a new vertex, with id as the key
}

//access vertex->edge
Solid::tEdge Solid::vertexEdge( tVertex v0, tVertex v1 )
{
	int id0 = v0->id();
	int id1 = v1->id();
	Edge e(id0,id1);
	return m_edges.find( &e );//insert a new vertex, with id as the key
}


//access id->edge
//specified by source->target order(id0->id1)
Solid::tHalfEdge Solid::idHalfedge( int id0, int id1 )
{
	tEdge e = idEdge( id0, id1 );
	if(! e ) throw TopologyException();;
	tHalfEdge he = e->halfedge(0);
	if( he->source()->id() == id0 && he->target()->id() == id1 ) return he;
	he = e->halfedge(1);
	if( he->source()->id() != id0 || he->target()->id() != id1 ) throw TopologyException();
	return he;
}

//access vertex->edge
//specified by source->target order(v0->v1)
Solid::tHalfEdge Solid::vertexHalfedge( tVertex v0, tVertex v1 )
{
	tEdge e = vertexEdge( v0, v1 );
	tHalfEdge he = e->halfedge(0);
	if( he->vertex() == v1 && he->he_prev()->vertex() == v0 ) return he;
	he = e->halfedge(1);
	if( he->vertex() != v1 || he->he_prev()->vertex() != v0 ) throw TopologyException();
	return he;
}


//access e->v
Solid::tVertex Solid::edgeVertex1( Solid::tEdge  e )
{
	if( e->halfedge(0 ) == NULL ) throw TopologyException();
	return e->halfedge(0)->source();
}

//access e->v
Solid::tVertex Solid::edgeVertex2( Solid::tEdge  e )
{
	if( e->halfedge(0 ) == NULL ) throw TopologyException();
	return e->halfedge(0)->target();
}

//access e->f
Solid::tFace Solid::edgeFace1( Solid::tEdge  e )
{
	if( e->halfedge(0) == NULL ) throw TopologyException();
	return e->halfedge(0)->face();
}

//access e->f
Solid::tFace Solid::edgeFace2( Solid::tEdge  e )
{
	if( e->halfedge(1) == NULL ) throw TopologyException();
	return e->halfedge(1)->face();
}

//access he->f
Solid::tFace Solid::halfedgeFace( Solid::tHalfEdge  he )
{
	return he->face();
}


//access he->v
Solid::tVertex Solid::halfedgeVertex( Solid::tHalfEdge  he )
{
	return he->vertex();
}

bool Solid::isBoundary( Solid::tVertex  v )
{
	return v->boundary();
}

bool Solid::isBoundary( Solid::tEdge  e )
{
	if( e->halfedge(0) == NULL || e->halfedge(1) == NULL ) return true;
	return false;
}

bool Solid::isBoundary( Solid::tHalfEdge  he )
{
	if( he->he_sym() == NULL ) return true;
	return false;
}



//create new gemetric simplexes

Solid::tVertex Solid::createVertex( int id )
{
	tVertex v = new Vertex();
	
	v->id() = id;
	m_verts.insert( v );
	return v;//insert a new vertex, with id as the key
}


Solid::tEdge Solid::createEdge( int start, int end )
{
	Edge edge(start,end);

	tEdge e = m_edges.find( & edge );

	if( e != NULL ) return e;

	e = new Edge( start, end );
	
	m_edges.insert( e );

	return e;
}

Solid::tFace Solid::createFace( int id )
{

	tFace f = new Face();
	
	f->id() = id;
	m_faces.insert( f );

	return f;//insert a new face, with id as the key
}

void Solid::copy( Solid & solid )
{	
	for(AVL::TreeIterator<Vertex> viter(m_verts); !viter.end(); ++viter)
	{
		Vertex * v = *viter;
		Vertex * w = solid.createVertex( v->id() );

		w->point() = v->point();
		w->string()= v->string();
		w->boundary() = v->boundary();
	}

	for(AVL::TreeIterator<Face> fiter(m_faces); !fiter.end(); ++fiter)
	{
		Face * f = *fiter;

		HalfEdge * he = f->halfedge();
		int v[3];

		for( int i = 0; i < 3; i ++ )
		{
			v[i] = he->vertex()->id();
			he = he->he_next();
		}

		Face * F = solid.createFace(v, f->id() );
		F->string() = f->string();
	}

	solid.labelBoundaryEdges();
}

void Solid::add( Solid & solid )
{	
	for(AVL::TreeIterator<Vertex> viter(solid.m_verts); !viter.end(); ++viter)
	{
		Vertex * v = *viter;
		Vertex * w = createVertex( v->id() );

		w->point() = v->point();
		w->string()= v->string();
		w->boundary() = v->boundary();
	}

	for(AVL::TreeIterator<Face> fiter(solid.m_faces); !fiter.end(); ++fiter)
	{
		Face * f = *fiter;

		HalfEdge * he = f->halfedge();
		int v[3];

		for( int i = 0; i < 3; i ++ )
		{
			v[i] = he->vertex()->id();
			he = he->he_next();
		}

		Face * F = createFace(v, f->id() );
		F->string() = f->string();
	}

	labelBoundaryEdges();
}

Vertex * Solid::edgeSplit( Edge * e )
{
	//find the max_id for vertices;

	AVL::TreeIterator<Vertex> viter(m_verts);

	int max_vid = -1;

	for( ; !viter.end(); ++viter )
	{
		max_vid = ( max_vid > (*viter)->id() )? max_vid: (*viter)->id();
	}

	AVL::TreeIterator<Face> fiter(m_faces);

	int max_fid = -1;

	for( ; !fiter.end(); ++fiter )
	{
		max_fid = ( max_fid > (*fiter)->id() )? max_fid: (*fiter)->id();
	}

	//create a new vertex

	tVertex nv = createVertex( ++max_vid );

	tHalfEdge hes[2];
	hes[0] = e->halfedge(0);
	hes[1] = e->halfedge(1);

	nv->boundary() = (hes[1]==NULL);

	m_edges.remove( e );


	List<Face> new_faces;

	for( int j = 0; j < 2; j ++ )
	{
		tVertex v[3];
		tHalfEdge he = hes[j];

		if( he == NULL ) continue;

		tFace  f = he->face();
		m_faces.remove( f );
		delete f;

		for( int i = 0; i < 3; i ++ )
		{
			v[i] = he->target();
			
			tEdge te = he->edge();
			if( te->halfedge(0) == he )
			{
				te->halfedge(0) = te->halfedge(1);
			}
			te->halfedge(1) = NULL;

			he = he->he_next();
		}

		int k;
		for( k = 0; k < 3; k ++ )
		{
			tHalfEdge ne = he->he_next();
			delete he;
			he = ne;
		}

		int vid[3];
		tVertex w[3];
		w[0] = nv; w[1] = v[0]; w[2] = v[1];
		for( k = 0; k < 3; k ++ )
		{
			vid[k] = w[k]->id(); 
		}		

		Face * nf = createFace( vid, ++max_fid );
		new_faces.Append( nf );

		w[0] = nv; w[1] = v[1]; w[2] = v[2];
		for( k = 0; k < 3; k ++ )
		{
			vid[k] = w[k]->id(); 
		}	
		nf = createFace( vid, ++ max_fid );
		new_faces.Append( nf );
	}	


	for( ListIterator<Face> iter( new_faces ); !iter.end(); ++ iter )
	{
		Face * f = *iter;
		HalfEdge * he = f->halfedge();

		do{
			Edge     * e = he->edge();
			if( e->halfedge(1) != NULL )
			{
				HalfEdge * h = e->halfedge(0);
				if( h->target()->id() < h->source()->id() )
				{
					e->halfedge(0) = e->halfedge(1);
					e->halfedge(1) = h;
				}
			}
			he = he->he_next();
		}while( he != f->halfedge() );
	}

	delete e;
	return nv;	
}


void Solid::write( const char * output )
{
	FILE * _os = fopen( output,"w");
	if(  ! _os ) throw FException();

	//remove vertices
	AVL::TreeIterator<Vertex> viter( m_verts );
	for( ; !viter.end() ; ++ viter )
	{
		tVertex v = *viter;

		fprintf(_os,"Vertex %d ", v->id() );
		
		for( int i = 0; i < 3; i ++ )
		{
			fprintf(_os,"%g ",v->point()[i]);
		}
		if( v->string().size() > 0 )
		{
			fprintf(_os,"{%s}", v->string().c_str() );
		}
		fprintf(_os,"\n");

	}

	for( AVL::TreeIterator<Face> fiter(m_faces); !fiter.end(); ++ fiter )
	{
		tFace f = *fiter;
		fprintf(_os,"Face %d",f->id());

		tHalfEdge he = f->halfedge();
		do{
			fprintf(_os," %d", he->target()->id() );
			he = he->he_next();
		}while( he != f->halfedge() );

		if( f->string().size() > 0 )
		{
			fprintf(_os,"{%s}", f->string().c_str() );
		}

		fprintf(_os,"\n");
	}

	for( AVL::TreeIterator<Edge> eiter(m_edges); !eiter.end(); ++ eiter )
	{
		tEdge e = *eiter;
		if( e->string().size() > 0 )
		{
			fprintf(_os,"Edge %d %d {%s} \n", edgeVertex1(e)->id(), edgeVertex2(e)->id(), e->string().c_str() );
		}

	}

	for( AVL::TreeIterator<Face> pfiter(m_faces); !pfiter.end(); ++ pfiter )
	{
		tFace f = *pfiter;

		tHalfEdge he = f->halfedge();
		do{
			if( he->string().size() > 0 )
			{
				fprintf(_os,"Corner %d %d {%s}\n", he->vertex()->id(), f->id(), he->string().c_str() );
			}
			he = he->he_next();
		}while( he != f->halfedge() );
	}

	fclose(_os);
}


//method edge length
double Solid::edgeLength( Solid::tEdge e )
{
	Vertex * v1 = edgeVertex1(e);
	Vertex * v2 = edgeVertex2(e);

	return ( v1->point() - v2->point() ).norm();
}


void
Solid:: labelBoundaryEdges()
{
	for( AVL::TreeIterator<Edge> eiter( m_edges ); ! eiter.end() ; ++ eiter )
	{
		Edge  *   edge = *eiter;
		HalfEdge* he[2];

		he[0] = edge->halfedge(0);
		he[1] = edge->halfedge(1);
		
		//label boundary
		if( he[1] == NULL )
		{
			he[0]->vertex()->boundary() = true;
			he[0]->he_prev()->vertex()->boundary()  = true;
		}

	}
}


void 
Solid::removeDanglingVertices()
{
	List<Vertex> dangling_verts;
	//Label boundary edges
	for( AVL::TreeIterator<Vertex> viter( m_verts ); ! viter.end() ; ++ viter )
	{
		Solid::tVertex     v = *viter;
		if( v->halfedge() != NULL ) continue;
		dangling_verts.Append( v );
	}

	for( ListIterator<Vertex> iter( dangling_verts ); !iter.end(); ++ iter )
	{
		Solid::tVertex v = *iter;
		m_verts.remove( v );
		delete v;
		v = NULL;
	}
}

void Solid::readBYU( std::istream & is )
{
	char line[MAX_LINE];

	is.getline(line, MAX_LINE);
	std::string s(line );
	string_token_iterator iter(s, " \n");
	std::string str = *(++iter);
	int vnum = atoi( str.c_str() );
	str = *(++iter);
//	int fnum = atoi( str.c_str() );
	is.getline(line, MAX_LINE);
	int vind, find;
	vind = find = 1;

	while( is && !is.eof() && is.getline(line, MAX_LINE) )
	{		
		if( strlen( line ) == 0 ) continue;

		std::string s(line);

		string_token_iterator iter(s, " \n"); 
		std::string str;

		if( vind <= vnum )
		{
			Point p;
			for( int i=0; i<3; i++ )
			{
				str = *iter;
				p[i] = atof( str.c_str() );
				iter++;
			}

			tVertex v = createVertex( vind );
			v->point() = p;
			v->id() = vind;
			vind ++;
		}
		else
		{
			int v[3];
			for( int i = 0; i < 3; i ++ )
			{
				str = *iter;
				v[i] = atoi( str.c_str() );
				iter++;
			}
			v[2] = -v[2];

			createFace( v, find );
			find ++;
		}
	}

	labelBoundaryEdges();

	removeDanglingVertices();
}

void Solid::writeBYU( std::ostream & os )
{
	// first two line
	os << "1 " << numVertices() << " " << numFaces() << " " << numVertices() + numFaces() - 1 << std::endl;
	os << "1 " << numFaces() << std::endl;

	AVL::TreeIterator<Vertex> viter( m_verts );
	for( ; !viter.end() ; ++ viter )
	{
		tVertex v = *viter;
	
		for( int i = 0; i < 3; i ++ )
		{
			os << "    " << std::fixed << v->point()[i];  
		}

		os << std::endl;
	}

	for( AVL::TreeIterator<Face> fiter(m_faces); !fiter.end(); ++ fiter )
	{
		tFace f = *fiter;
		tHalfEdge he = f->halfedge();
		for( int i=0; i<3; i ++ )
		{
			if( i!= 2 )
			{
				os << "    ";
			}
			else
			{
				os << "    -"; 
			}
			os << std::right << he->target()->id();
			he = he->he_next();
		}

		os << std::endl;
	}
}

//Based very strongly on code found in CT.
void Solid::UpdateNormals(void)
{
	_FaceNormal(this);
	_VertexNormal(this);
}


void Solid::_FaceNormal(MeshLib::Solid *pMesh)
{
	for( MeshLib::SolidFaceIterator fiter( pMesh ); !fiter.end(); ++ fiter )
	{
		MeshLib::Face * f = *fiter;
		if( f->trait() == NULL )
		{
			MeshLib::FaceNormalTrait * ft = new MeshLib::FaceNormalTrait;
			if( ft == NULL ) return;
			MeshLib::add_trait<MeshLib::FaceNormalTrait,MeshLib::Face>(f,ft);
		}

		MeshLib::Point p[3];
		int i = 0;

		for( MeshLib::FaceVertexIterator fviter(f); !fviter.end(); ++ fviter )
		{
			MeshLib::Vertex * v = *fviter;
			p[i++] = v->point();
		}
		MeshLib::Point n = (p[1]-p[0])^(p[2]-p[0]);
		n /= n.norm();
		
		f_normal(f) = n;
	}
}

void Solid::_VertexNormal(MeshLib::Solid *pMesh)
{
#if 0
	for( MeshLib::SolidVertexIterator viter( pMesh ); !viter.end(); ++ viter )
	{
		MeshLib::Vertex * v = *viter;

		MeshLib::Point n(0,0,0);
		int count = 0;

		for(MeshLib::VertexFaceIterator vfiter(v); !vfiter.end(); ++ vfiter )
		{
			MeshLib::Face * f = *vfiter;
			n += f_normal(f);
			count ++;
		}
		n /= count;

		if( n.norm() > 1e-5 )
		{
			n /= n.norm();
		}

		v->normal() = n;
	}

	for( MeshLib::SolidFaceIterator fiter( pMesh ); !fiter.end(); ++ fiter )
	{
		MeshLib::Face * f = *fiter;
		MeshLib::FaceNormalTrait * pfnt = pTrait<MeshLib::FaceNormalTrait,MeshLib::Face>( f );
		del_trait<MeshLib::FaceNormalTrait,MeshLib::Face>(f, pfnt);
	}
#else
    // using edge lengthes as weight on calculating vertex normal
    for (SolidFaceIterator fIter(this); !fIter.end(); ++fIter)
    {
        Face *pFace = *fIter;
        HalfEdge *pHE[3] = {pFace->halfedge(),
            pFace->halfedge()->he_next(), pFace->halfedge()->he_prev()};
        Point e[3];
        for (int i = 0; i < 3; ++i) {
            e[i] = pHE[i]->target()->point() - pHE[i]->source()->point();
        }
        Point faceNormal = e[0] ^ e[1];
        for (int i = 0; i < 3; ++i) {
            pHE[i]->target()->normal() += faceNormal /
                (e[(i + 1) % 3].norm2() * e[i].norm2());
        }
    }

    for (SolidVertexIterator vIter(this); !vIter.end(); ++vIter) {
        Vertex *pVer = *vIter;
        pVer->normal() /= pVer->normal().norm();
    }
#endif
}



