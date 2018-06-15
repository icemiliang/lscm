#include "Vertex.h"
#include "HalfEdge.h"
#include "Edge.h"
#include "Face.h"
#include "Mesh.h"
#include <fstream>
#include <cstring>
#include <iostream>
#include <map>
#include "FormTrait.h"

using namespace MeshLib;

//access e->v
Vertex *Mesh::edge_vertex_1(Edge  *e)
{
	assert(e->halfedge(0) != NULL);
	return e->halfedge(0)->source();
}

//access e->v
Vertex *Mesh::edge_vertex_2(Edge  *e)
{
	assert(e->halfedge(0) != NULL);
	return e->halfedge(0)->target();
}

//access e->f
Face *Mesh::edge_face_1(Edge  *e)
{
	assert(e->halfedge(0) != NULL);
	return e->halfedge(0)->face();
}

//access e->f
Face *Mesh::edge_face_2(Edge  *e)
{
	assert(e->halfedge(1) != NULL);
	return e->halfedge(1)->face();
}

//access he->f
Face *Mesh::halfedge_face(HalfEdge  *he)
{
	return he->face();
}


//access he->v
Vertex  *Mesh::halfedge_vertex(HalfEdge  *he)
{
	return he->vertex();
}

bool  Mesh::is_boundary(Vertex * v)
{
	return v->boundary();
}

bool  Mesh::is_boundary(Edge  *e)
{
	if (e->halfedge(0) == NULL || e->halfedge(1) == NULL) return true;
	return false;
}

bool  Mesh::is_boundary(HalfEdge  *he)
{
	if (he->he_sym() == NULL) return true;
	return false;
}

int Mesh::numVertices()
{
	return (int)m_vertices.size();
}

int Mesh::numEdges()
{
	return (int)m_edges.size();
}

int Mesh::numFaces()
{
	return (int)m_faces.size();
}


//Euler operation

HalfEdge *Mesh::vertexMostClwOutHalfEdge(Vertex  *v)
{
	return v->most_clw_out_halfedge();
}

HalfEdge *Mesh::vertexMostCcwOutHalfEdge(Vertex  *v)
{
	return v->most_ccw_out_halfedge();
}

HalfEdge *Mesh::corner(Vertex *v, Face *f)
{
	HalfEdge *he = f->halfedge();
	do{
		if (he->vertex() == v)
			return he;
		he = he->he_next();
	} while (he != f->halfedge());
	return NULL;
}

HalfEdge *Mesh::vertexNextCcwOutHalfEdge(HalfEdge  *he)
{
	return he->ccw_rotate_about_source();
}

HalfEdge *Mesh::vertexNextClwOutHalfEdge(HalfEdge  *he)
{
	assert(he->he_sym() != NULL);
	return he->clw_rotate_about_source();
}

HalfEdge *Mesh::vertexMostClwInHalfEdge(Vertex  *v)
{
	return v->most_clw_in_halfedge();
}

HalfEdge *Mesh::vertexMostCcwInHalfEdge(Vertex  *v)
{
	return v->most_ccw_in_halfedge();
}

HalfEdge *Mesh::vertexNextCcwInHalfEdge(HalfEdge  *he)
{
	assert(he->he_sym() != NULL);
	return he->ccw_rotate_about_target();
}

HalfEdge *vertexNextClwInHalfEdge(HalfEdge  *he)
{
	return he->clw_rotate_about_target();
}

HalfEdge *Mesh::faceNextClwHalfEdge(HalfEdge  *he)
{
	return he->he_prev();
}

HalfEdge *Mesh::faceNextCcwHalfEdge(HalfEdge  *he)
{
	return he->he_next();
}

HalfEdge *Mesh::faceMostCcwHalfEdge(Face  *face)
{
	return face->halfedge();
}

HalfEdge *Mesh::faceMostClwHalfEdge(Face  *face) {
	return face->halfedge()->he_next();
}


Mesh::~Mesh() {
	//remove vertices

	for (std::list<Vertex*>::iterator viter = m_vertices.begin(); viter != m_vertices.end(); viter++)
	{
		Vertex * pV = *viter;
		delete pV;
	}
	m_vertices.clear();

	//remove faces

	for (std::list<Face*>::iterator fiter = m_faces.begin(); fiter != m_faces.end(); fiter++)
	{
		Face * pF = *fiter;

		HalfEdge *he = pF->halfedge();

		std::list<HalfEdge*> hes;
		do{
			he = he->he_next();
			hes.push_back(he);
		} while (he != pF->halfedge());

		for (std::list<HalfEdge*>::iterator hiter = hes.begin(); hiter != hes.end(); hiter++)
		{
			HalfEdge * pH = *hiter;
			delete pH;
		}
		hes.clear();

		delete pF;
	}
	m_faces.clear();

	//remove edges
	for (std::list<Edge*>::iterator eiter = m_edges.begin(); eiter != m_edges.end(); eiter++)
	{
		Edge * pE = *eiter;
		delete pE;
	}

	m_edges.clear();

	m_map_vertex.clear();
	m_map_face.clear();
	m_map_edge.clear();
}



double Mesh::edge_length(Edge *e)
{
	Vertex * v1 = edge_vertex_1(e);
	Vertex * v2 = edge_vertex_2(e);

	return (v1->point() - v2->point()).norm();
}





//create new gemetric simplexes

Vertex *Mesh::create_vertex(int id)
{
	Vertex *v = new Vertex();
	assert(v != NULL);
	v->id() = id;
	m_vertices.push_back(v);
	m_map_vertex.insert(std::pair<int, Vertex*>(id, v));
	return v;//insert a new vertex, with id as the key
}



int Mesh::read_obj(const char * filename)
{
	//	TRACE("load obj file %s\n",filename);

	FILE* f = fopen(filename, "r");
	if (f == NULL) return 0;

	char cmd[1024];
	char seps[] = " ,\t\n";
	int  vid = 1;
	int  fid = 1;
	int  nid = 1;


	while (true)
	{
		if (fgets(cmd, 1024, f) == NULL)
			break;

		char *token = strtok(cmd, seps);

		if (token == NULL)
			continue;

		if (strcmp(token, "v") == 0)
		{
			Point p;
			for (int i = 0; i < 3; i++)
			{
				token = strtok(NULL, seps);
				p[i] = atof(token);
			}

			Vertex * v = create_vertex(vid);
			v->point() = p;
			v->id() = vid++;

			// Add feature points
			token = strtok(NULL, seps);
			if (token != NULL) {
				v->string() = "fix";
			}
			continue;
		}

		// if (strcmp(token, "vt") == 0)
		// {
		// 	Point p;
		// 	for (int i = 0; i < 2; i++)
		// 	{
		// 		token = strtok(NULL, seps);
		// 		p[i] = atof(token);
		// 	}
		// 	continue;
		// }

		if (strcmp(token, "vn") == 0)
		{
			Point p;
			for (int i = 0; i < 3; i++)
			{
				token = strtok(NULL, seps);
				p[i] = atof(token);

			}
			Vertex* v = id_vertex(nid);
			v->normal() = p;
			nid++;
			continue;
		}

		if (strcmp(token, "f") == 0)
		{
			Vertex* v[3];
			for (int i = 0; i < 3; i++)
			{
				token = strtok(NULL, seps);
				// std::cout << (void *) token;
				// char* tmp = strchr(token, '/');
				int id = atoi(token);
				// std::cout << i << ": " << id << ", ";
				v[i] = m_map_vertex[id];
			}
			create_face(v, fid++);
			// std::cout << std::endl;
		}
	}
	fclose(f);

	//Label boundary edges
	for (std::list<Edge*>::iterator eiter = m_edges.begin(); eiter != m_edges.end(); ++eiter)
	{
		Edge     *edge = *eiter;
		HalfEdge *he[2];

		he[0] = edge->halfedge(0);
		he[1] = edge->halfedge(1);

		assert(he[0] != NULL);


		if (he[1] != NULL)
		{
			assert(he[0]->target() == he[1]->source() && he[0]->source() == he[1]->target());

			if (he[0]->target()->id() < he[0]->source()->id())
			{
				edge->halfedge(0) = he[1];
				edge->halfedge(1) = he[0];
			}

			assert(edge_vertex_1(edge)->id() < edge_vertex_2(edge)->id());
		}
		else
		{
			he[0]->vertex()->boundary() = true;
			he[0]->he_prev()->vertex()->boundary() = true;
		}

	}

	std::list<Vertex*> dangling_verts;
	//Label boundary vertices
	for (std::list<Vertex*>::iterator viter = m_vertices.begin(); viter != m_vertices.end(); ++viter)
	{
		Vertex     *v = *viter;
		if (v->halfedge() != NULL) continue;
		dangling_verts.push_back(v);
	}

	for (std::list<Vertex*>::iterator viter = dangling_verts.begin(); viter != dangling_verts.end(); ++viter)
	{
		Vertex *v = *viter;
		m_vertices.remove(v);
		delete v;
		v = NULL;
	}

	//Arrange the boundary half_edge of boundary vertices, to make its halfedge
	//to be the most ccw in half_edge

	for (std::list<Vertex*>::iterator viter = m_vertices.begin(); viter != m_vertices.end(); ++viter)
	{
		Vertex     *v = *viter;
		if (!v->boundary()) continue;

		HalfEdge * he = v->halfedge();
		while (he->he_sym() != NULL)
		{
			he = he->ccw_rotate_about_target();
		}

		v->halfedge() = he;
	}
	return 0;
}

Face *Mesh::create_face(Vertex * v[], int id)
{
	Face *f = new Face();
	assert(f != NULL);
	f->id() = id;
	m_faces.push_back(f);
	m_map_face.insert(std::pair<int, Face*>(id, f));

	//create halfedges
	HalfEdge *hes[3];

	for (int i = 0; i < 3; i++)
	{
		hes[i] = new HalfEdge;
		assert(hes[i]);
		Vertex * vert = v[i];
		hes[i]->vertex() = vert;
		vert->halfedge() = hes[i];
	}

	//linking to each other
	for (int i = 0; i < 3; i++)
	{
		hes[i]->he_next() = hes[(i + 1) % 3];
		hes[i]->he_prev() = hes[(i + 2) % 3];
	}

	//linking to face
	for (int i = 0; i < 3; i++)
	{
		hes[i]->face() = f;
		f->halfedge() = hes[i];
	}

	//connecting with edge
	for (int i = 0; i < 3; i++)
	{
		Edge *e = create_edge(v[i], v[(i + 2) % 3]);
		if (e->halfedge(0) == NULL)
		{
			e->halfedge(0) = hes[i];
		}
		else
		{
			assert(e->halfedge(1) == NULL);
			e->halfedge(1) = hes[i];
		}
		hes[i]->edge() = e;
	}

	return f;
}


//access id->v
Vertex *Mesh::id_vertex(int id)
{
	return m_map_vertex[id];
}

//access v->id
int Mesh::vertex_id(Vertex  *v)
{
	return v->id();
}

//access id->f
Face *Mesh::id_face(int id)
{
	return m_map_face[id];
}

//acess f->id
int Mesh::face_id(Face  *f)
{
	return f->id();
}

Edge *Mesh::create_edge(Vertex *v1, Vertex *v2)
{
	EdgeKey key(v1, v2);

	Edge *e = NULL;

	if (m_map_edge.find(key) != m_map_edge.end()) {
		e = m_map_edge[key];
		return e;
	}

	e = new Edge;

	assert(e != NULL);
	m_map_edge.insert(std::pair<EdgeKey, Edge*>(key, e));
	m_edges.push_back(e);

	return e;

}

//access vertex->edge
Edge *Mesh::vertex_edge(Vertex *v0, Vertex *v1)
{
	EdgeKey key(v0, v1);
	return m_map_edge[key];
}

//access vertex->edge
HalfEdge *Mesh::vertex_halfedge(Vertex *v0, Vertex *v1)
{
	Edge *e = vertex_edge(v0, v1);
	assert(e != NULL);
	HalfEdge *he = e->halfedge(0);
	if (he->vertex() == v1 && he->he_prev()->vertex() == v0) return he;
	he = e->halfedge(1);
	assert(he->vertex() == v1 && he->he_prev()->vertex() == v0);
	return he;
}

int Mesh::read_m(const char * input)
{
	FILE * is = fopen(input, "r");
	if (is == NULL)
	{
		fprintf(stderr, "Error is opening file %s\n", input);
		return 0;
	}

	char line[MAX_LINE];
	int id;

	while (!feof(is))
	{
		if (fgets(line, MAX_LINE, is) == NULL) break;
		if (strlen(line)< 4) break;

		char * str = strtok(line, " \r\n");
		if (strcmp(str, "Vertex") == 0)
		{

			str = strtok(NULL, " \r\n");
			id = atoi(str);
			Point p;
			for (int i = 0; i < 3; i++)
			{
				str = strtok(NULL, " \r\n");
				p[i] = atof(str);
			}

			Vertex *v = create_vertex(id);
			v->point() = p;
			v->id() = id;

			str = strtok(NULL, "\r\n");
			if (str == NULL) continue;

			std::string s(str);
			int sp = (int)s.find("{");
			int ep = (int)s.find("}");

			if (sp >= 0 && ep >= 0)
			{
				v->string() = s.substr(sp + 1, ep - sp - 1);
			}
			continue;
		}


		if (strcmp(str, "Face") == 0)
		{
			str = strtok(NULL, " \r\n");
			id = atoi(str);

			Vertex * v[3];
			for (int i = 0; i < 3; i++)
			{
				str = strtok(NULL, " \r\n");
				v[i] = id_vertex(atoi(str));
			}

			Face *f = create_face(v, id);

			str = strtok(NULL, "\r\n");
			if (str == NULL)       continue;

			std::string s(str);

			int sp = (int)s.find("{");
			int ep = (int)s.find("}");

			if (sp >= 0 && ep >= 0)
			{
				f->string() = s.substr(sp + 1, ep - sp - 1);
			}
			continue;
		}

		//read in edge attributes
		if (strcmp(str, "Edge") == 0)
		{
			str = strtok(NULL, " \r\n");
			int id0 = atoi(str);

			str = strtok(NULL, " \r\n");
			int id1 = atoi(str);

			Vertex * v0 = id_vertex(id0);
			Vertex * v1 = id_vertex(id1);

			Edge *edge = vertex_edge(v0, v1);

			str = strtok(NULL, "\r\n");

			std::string s(str);
			int sp = (int)s.find("{");
			int ep = (int)s.find("}");

			if (sp >= 0 && ep >= 0)
			{
				edge->string() = s.substr(sp + 1, ep - sp - 1);
			}
			continue;
		}

		//read in edge attributes
		if (strcmp(str, "Corner") == 0)
		{
			str = strtok(NULL, " \r\n");
			int id0 = atoi(str);

			str = strtok(NULL, " \r\n");
			int id1 = atoi(str);


			Vertex * v = id_vertex(id0);
			Face   * f = id_face(id1);
			HalfEdge *he = corner(v, f);

			str = strtok(NULL, "\r\n");
			std::string s(str);

			int sp = (int)s.find("{");
			int ep = (int)s.find("}");

			if (sp >= 0 && ep >= 0)
			{
				he->string() = s.substr(sp + 1, ep - sp - 1);
			}
			continue;
		}
	}

	//Label boundary edges
	for (std::list<Edge*>::iterator eiter = m_edges.begin(); eiter != m_edges.end(); ++eiter)
	{
		Edge     *edge = *eiter;
		HalfEdge *he[2];

		he[0] = edge->halfedge(0);
		he[1] = edge->halfedge(1);

		assert(he[0] != NULL);


		if (he[1] != NULL)
		{
			assert(he[0]->target() == he[1]->source() && he[0]->source() == he[1]->target());

			if (he[0]->target()->id() < he[0]->source()->id())
			{
				edge->halfedge(0) = he[1];
				edge->halfedge(1) = he[0];
			}

			assert(edge_vertex_1(edge)->id() < edge_vertex_2(edge)->id());
		}
		else
		{
			he[0]->vertex()->boundary() = true;
			he[0]->he_prev()->vertex()->boundary() = true;
		}

	}

	std::list<Vertex*> dangling_verts;
	//Label boundary edges
	for (std::list<Vertex*>::iterator viter = m_vertices.begin(); viter != m_vertices.end(); ++viter)
	{
		Vertex     *v = *viter;
		if (v->halfedge() != NULL) continue;
		dangling_verts.push_back(v);
	}

	for (std::list<Vertex*>::iterator viter = dangling_verts.begin(); viter != dangling_verts.end(); ++viter)
	{
		Vertex *v = *viter;
		m_vertices.remove(v);
		delete v;
		v = NULL;
	}

	//Arrange the boundary half_edge of boundary vertices, to make its halfedge
	//to be the most ccw in half_edge

	for (std::list<Vertex*>::iterator viter = m_vertices.begin(); viter != m_vertices.end(); ++viter)
	{
		Vertex     *v = *viter;
		if (!v->boundary()) continue;

		HalfEdge * he = v->halfedge();
		while (he->he_sym() != NULL)
		{
			he = he->ccw_rotate_about_target();
		}
		v->halfedge() = he;
	}

	return 0;
}

int Mesh::write_m(const char * output)
{
	FILE * _os = fopen(output, "w");
	assert(_os);

	//remove vertices
	for (std::list<Vertex*>::iterator viter = m_vertices.begin(); viter != m_vertices.end(); viter++)
	{
		Vertex *v = *viter;

		fprintf(_os, "Vertex %d ", v->id());

		for (int i = 0; i < 3; i++)
		{
			fprintf(_os, "%g ", v->point()[i]);
		}
		if (v->string().size() > 0)
		{
			fprintf(_os, "{%s}", v->string().c_str());
		}
		fprintf(_os, "\n");
	}

	for (std::list<Face*>::iterator fiter = m_faces.begin(); fiter != m_faces.end(); fiter++)
	{
		Face *f = *fiter;
		fprintf(_os, "Face %d", f->id());

		HalfEdge *he = f->halfedge();
		do{
			fprintf(_os, " %d", he->target()->id());
			he = he->he_next();
		} while (he != f->halfedge());

		if (f->string().size() > 0)
		{
			fprintf(_os, "{%s}", f->string().c_str());
		}

		fprintf(_os, "\n");
	}

	for (std::list<Edge*>::iterator eiter = m_edges.begin(); eiter != m_edges.end(); eiter++)
	{
		Edge *e = *eiter;
		if (e->string().size() > 0)
		{
			fprintf(_os, "Edge %d %d {%s} \n", edge_vertex_1(e)->id(), edge_vertex_2(e)->id(), e->string().c_str());
		}
	}

	for (std::list<Face*>::iterator fiter = m_faces.begin(); fiter != m_faces.end(); fiter++)
	{
		Face *f = *fiter;

		HalfEdge *he = f->halfedge();

		do{
			if (he->string().size() > 0)
			{
				fprintf(_os, "Corner %d %d {%s}\n", he->vertex()->id(), f->id(), he->string().c_str());
			}
			he = he->he_next();
		} while (he != f->halfedge());

	}

	fclose(_os);
	return 0;
}

int Mesh::write_obj(const char * output)
{
	FILE * _os = fopen(output, "w");
	assert(_os);

	//remove vertices
	for (std::list<Vertex*>::iterator viter = m_vertices.begin(); viter != m_vertices.end(); viter++)
	{
		Vertex *v = *viter;

		fprintf(_os, "v");

		for (int i = 0; i < 3; i++)
		{
			fprintf(_os, " %g", v->point()[i]);
		}
		fprintf(_os, "\n");
	}

	for (std::list<Vertex*>::iterator viter = m_vertices.begin(); viter != m_vertices.end(); viter++)
	{
		Vertex *v = *viter;

		fprintf(_os, "vn");

		for (int i = 0; i < 3; i++)
		{
			fprintf(_os, " %g", v->normal()[i]);
		}
		fprintf(_os, "\n");
	}

	for (std::list<Face*>::iterator fiter = m_faces.begin(); fiter != m_faces.end(); fiter++)
	{
		Face *f = *fiter;
		fprintf(_os, "f");

		HalfEdge *he = f->halfedge();
		do{
			fprintf(_os, " %d/%d", he->target()->id(),he->target()->id());
			he = he->he_next();
		} while (he != f->halfedge());


		fprintf(_os, "\n");
	}
	fclose(_os);
	return 0;
}

int Mesh::write_ply(const char * output)
{
	FILE * _os = fopen(output, "w");
	assert(_os);

	fprintf(_os, "ply\nformat ascii 1.0\n");
	fprintf(_os, "element vertex %d\n", (int)m_vertices.size());
	fprintf(_os, "property float x\nproperty float y\nproperty float z\n");
	fprintf(_os, "property float nx\nproperty float ny\nproperty float nz\n");
	fprintf(_os, "element face %d\n", (int)m_faces.size());
	fprintf(_os, "property list uchar int vertex_indices\nend_header\n");

	//remove vertices
	for (std::list<Vertex*>::iterator viter = m_vertices.begin(); viter != m_vertices.end(); viter++)
	{
		Vertex *v = *viter;

		for (int i = 0; i < 3; i++)
		{
			fprintf(_os, " %g", v->point()[i]);
		}
		for (int i = 0; i < 3; i++)
		{
			fprintf(_os, " %g", v->normal()[i]);
		}
		fprintf(_os, "\n");
	}

	for (std::list<Face*>::iterator fiter = m_faces.begin(); fiter != m_faces.end(); fiter++)
	{
		Face *f = *fiter;
		fprintf(_os, "3");

		HalfEdge *he = f->halfedge();
		do{
			fprintf(_os, " %d", he->target()->id()-1);
			he = he->he_next();
		} while (he != f->halfedge());


		fprintf(_os, "\n");
	}

	fclose(_os);
	return 0;
}
