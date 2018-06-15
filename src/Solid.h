#ifndef _MESHLIB_SOLID_H_
#define _MESHLIB_SOLID_H_

#define MAX_LINE 2048

#include <math.h>
#include "Edge.h"
#include "Face.h"
#include "HalfEdge.h"
#include "Vertex.h"
#include "Point.h"
#include "Trait.h"
#include "Avltree.h"
#include "SList.h"


namespace MeshLib{

class Vertex;
class HalfEdge;
class Edge;
class Face;

class SolidVertexIterator;
class SolidEdgeIterator;
class SolidFaceIterator;
class Configure;

//!  Solid.
/*!
  This class define solid(mesh) structure.
*/
class Solid 
{

public:

	// has to be gong someday!

	// pointer to Vertices, Halfedges, Edges, Face and Solid
	//!  Vertex pointer.
	typedef Vertex   * tVertex;
	//!  Halfedge pointer.
	typedef HalfEdge * tHalfEdge;
	//!  Edge pointer.
	typedef Edge     * tEdge;
	//!  Face pointer.
	typedef Face     * tFace;
	//!  Solid pointer.
	typedef Solid    * tSolid;

	//constructor and destructor
	//!  Constructor.
	Solid(){};
	//!  Destructor.
	virtual ~Solid();

	//copy operator
	//!  Solid copy operator.
	/*!
	  \param solid an Solid which is the copying target
	*/
	void copy( Solid & solid );

	// copy always causes confusion.  Change name to 
	// reflect its nature.
	void copyto( Solid & solid )
	{		copy( solid ); 		}

	//
	//!  Add more geometry to the mesh.
	/*!
	  \param solid an Solid which is the copying source..
	  \param uv the UV coordinates
	*/
	void add( Solid & solid );

	//file io
	//!  Read solid from istream.
	/*!
	  \param is an istream to get information from.
	*/
	void read(  std::istream & is );
	//!  Write solid to isstream.
	/*!
	  \param os an ostream to put information to.
	*/
	void write( std::ostream & os );

	//!  Read solid from input file.
	/*!
	  \param input the char pointor to indicate input file name.
	*/
	void read(  const char * input );
	//!  Write solid to output file.
	/*!
	  \param output the char pointor to indicate output file name.
	*/
	void write( const char * output);
	
	//number of vertices, faces, edges
	//!  Get # of vertices.
    /*!
      \return an int which is the # of vertices.
    */
	int  numVertices();
	//!  Get # of edges.
    /*!
      \return an int which is the # of edges.
    */
	int  numEdges();
	//!  Get # of face.
    /*!
      \return an int which is the # of faces.
    */
	int  numFaces();

	//is boundary
	//!  Is boundary for vertex.
    /*!      
      \param v a tVertex to be test it is boundary or not.
      \return the boolean result that the vertex is boundary or not.
    */
	bool    isBoundary( tVertex  v );
	//!  Is boundary for edge.
    /*!      
      \param e a tEdge to be test it is boundary or not.
      \return the boolean result that the edge is boundary or not.
    */
	bool    isBoundary( tEdge    e );
	//!  Is boundary for halfedge.
    /*!      
      \param he a tHalfEdge to be test it is boundary or not.
      \return the boolean result that the halfedge is boundary or not.
    */
	bool    isBoundary( tHalfEdge  he );

	//acess vertex - id
	//!  Acess vertex from id.
    /*!      
      \param id an int which is the ID of the vertex which needs to be access.
      \return the tVertex which is the vertex with its ID = given id.
    */
	tVertex idVertex( int id );
	//!  Get vertex ID from given vertex.
    /*!      
      \param v a tVertex that needs to find its id.
      \return a int which is the id of the given vertex.
    */
	int     vertexId( tVertex  v );

	//access face - id
	//!  Acess face from id.
    /*!      
      \param id an int which is the ID of the face which needs to be access.
      \return the tFace which is the face with its ID = given id.
    */
	tFace   idFace( int id );
	//!  Get face ID from given face.
    /*!      
      \param v a tFace that needs to find its id.
      \return a int which is the id of the given face.
    */
	int     faceId( tFace  f );

	//access edge - edge key, vertex
	//!  Acess edge from ids.
    /*!      
      \param id0 an int which is the source vertex ID of edge.
	  \param id1 an int which is the target vertex ID of edge.
      \return the tEdge which is the edge with its IDs = given ids.
    */
	tEdge   idEdge( int id0, int id1 );
	//!  Acess edge from vertices.
    /*!      
      \param id0 a tVertex which is the source vertex of the edge.
	  \param id1 a tVertex which is the target vertex of the edge.
      \return the tEdge which is the edge with its source/target vertex = given verticess.
    */
	tEdge   vertexEdge( tVertex v0, tVertex v1 );

	//access halfedge - halfedge key, vertex
	//!  Acess halfedge from ids.
    /*!      
      \param id0 an int which is the source vertex ID of the halfedge.
	  \param id1 an int which is the target vertex ID of the halfedge.
      \return the tHalfEdge which is the halfedge with its IDs = given ids.
    */
	tHalfEdge   idHalfedge( int id0, int id1 );
	//!  Acess halfedge from vertices.
    /*!      
      \param id0 a tVertex which is the source vertex of the halfedge.
	  \param id1 a tVertex which is the target vertex of the halfedge.
      \return the tHalfEdge which is the halfedge with its source/target vertex = given verticess.
    */
	tHalfEdge   vertexHalfedge( tVertex v0, tVertex v1 );
	//!  Acess halfedge from corner.
    /*!      
      \param v a tVertex which is on the oppsite side of the halfedge.
	  \param f a tFace which is the face thich is associated with the halfedge.
      \return the tHalfEdge which is the halfedge from its face and corner information.
    */
	tHalfEdge   corner( tVertex v, tFace f);

	//halfedge->face
	//!  Acess face from halfedge.
    /*!      
      \param he a tHalfEdge.
      \return the tFace which is associated with the given halfedge.
    */
	tFace   halfedgeFace( tHalfEdge he );
	//halfedge->vertex
	//!  Acess vertex from halfedge.
    /*!      
      \param he a tHalfEdge.
      \return the tVertex which is associated with the given halfedge (target vertex).
    */
	tVertex halfedgeVertex( tHalfEdge he );

	//edge->vertex
	//!  Acess vertex from edge.
    /*!      
      \param e a tEdge.
      \return the tVertex which is associated with the given edge (smaller vertex id).
	  \return For an unboundary edge, condition edgeVertex1(edge).id() < edgeVertex(edge).id()
	  \return is strictly enforced.
	  \return For a boundary edge, no such condition exists.
    */
	tVertex edgeVertex1( tEdge  e );
	//!  Acess vertex from edge.
	/*!      
      \param e a tEdge.
      \return the tVertex which is associated with the given edge (larger vertex id).
    */
	tVertex edgeVertex2( tEdge  e );

	//edge->face
	//!  Acess face from edge.
	/*!      
      \param e a tEdge.
      \return the tFace which is the 1st associated with halfedge which belongs to the given edge.
    */
	tFace edgeFace1( tEdge  e );
	//!  Acess face from edge.
	/*!      
      \param e a tEdge.
      \return the tFace which is the snd associated with halfedge which belongs to the given edge.
    */
	tFace edgeFace2( tEdge  e );

	//Euler operations
	//!  Access most clw out halfedge from a vertex.
	/*!      
      \param v a tVertex which is the base center for clw access.
      \return the tHalfEdge which is moust clw out halfedge from the given vertex.
    */
	tHalfEdge vertexMostClwOutHalfEdge( tVertex  v );
	//!  Access next ccw out halfedge from a halfedge.
	/*!      
      \param he a tHalfEdge which is the base center for ccw access.
      \return the tHalfEdge which is next ccw out halfedge from the given halfedge.
    */
	tHalfEdge vertexNextCcwOutHalfEdge( tHalfEdge  he );

	//!  Access most ccw out halfedge from a vertex.
	/*!      
      \param v a tVertex which is the base center for clw access.
      \return the tHalfEdge which is most ccw out halfedge from the given vertex.
    */
	tHalfEdge vertexMostCcwOutHalfEdge( tVertex  v );
	//!  Access next clw out halfedge from a halfedge.
	/*!
      \param he a tHalfEdge which is the base center for clw access.
      \return the tHalfEdge which is next clw out halfedge from the given halfedge.
    */
	tHalfEdge vertexNextClwOutHalfEdge( tHalfEdge  he );

	//!  Access most clw in halfedge from a vertex.
	/*!      
      \param v a tVertex which is the base center for clw access.
      \return the tHalfEdge which is most clw in halfedge from the given vertex.
    */
	tHalfEdge vertexMostClwInHalfEdge( tVertex  v );
	//!  Access next ccw in halfedge from a halfedge.
	/*!
      \param he a tHalfEdge which is the base center for ccw access.
      \return the tHalfEdge which is next ccw in halfedge from the given halfedge.
    */
	tHalfEdge vertexNextCcwInHalfEdge( tHalfEdge  he );

	//!  Access most ccw in halfedge from a vertex.
	/*!      
      \param v a tVertex which is the base center for ccw access.
      \return the tHalfEdge which is most ccw in halfedge from the given vertex.
    */
	tHalfEdge vertexMostCcwInHalfEdge( tVertex  v );
	//!  Access next ccw in halfedge from a halfedge.
	/*!
      \param he a tHalfEdge which is the base center for clw access.
      \return the tHalfEdge which is next ccw in halfedge from the given halfedge.
    */
	tHalfEdge vertexNextClwInHalfEdge( tHalfEdge  he );

	//!  Access most clw in halfedge from a face.
	/*!      
      \param f a tFace which is the base center for clw access.
      \return the tHalfEdge which is most clw in halfedge from the given face.
    */
	tHalfEdge faceMostClwHalfEdge( tFace  f );
	//!  Access most ccw in halfedge from a face.
	/*!      
      \param f a tFace which is the base center for ccw access.
      \return the tHalfEdge which is most ccw in halfedge from the given face.
    */
	tHalfEdge faceMostCcwHalfEdge( tFace  f );
	//!  Access next ccw in halfedge from a face.
	/*!      
      \param f a tFace which is the base center for ccw access.
      \return the tHalfEdge which is next ccw in halfedge from the given face.
    */
	tHalfEdge faceNextCcwHalfEdge( tHalfEdge  he );
	//!  Access next ccw in halfedge from a halfedge.
	/*!      
      \param f a tHalfEdge which is the base center for ccw access.
      \return the tHalfEdge which is next ccw in halfedge from the given halfedge.
    */
	tHalfEdge faceNextClwHalfEdge( tHalfEdge  he );

	//!  Get edge length.
	/*!      
      \param e a tEdge to be get length.
      \return a double which is the length of this edge.
    */
	double edgeLength( tEdge e );
	//Dynamic part

	//!  Split an edge.
	/*!      
      \param e a tEdge to be split.
      \return a tVertex which is the new vertex after edge split.
    */
	tVertex   edgeSplit( tEdge e );


	//!	Label boundary edges.
	/*!
		\param void
		\return void
	*/
	void labelBoundaryEdges();


	//! removeDanglingVertices.
	/*!
		\param void
		\return void
	*/
	void removeDanglingVertices();

	void UpdateNormals(void);

	void _FaceNormal(MeshLib::Solid *pMesh);
	void _VertexNormal(MeshLib::Solid *pMesh);

	void readBYU(  std::istream & is );
	void writeBYU( std::ostream & os );


protected:

	//std::map<EdgeKey, tEdge, ltedgekey >              m_edges;
	
	//!  AVL tree contains edges.
	AVL::Tree<Edge>                                   m_edges;
	//!  AVL tree contains vertices.
	AVL::Tree<Vertex>                                 m_verts;
	//!  AVL tree contains faces.
	AVL::Tree<Face>									  m_faces;

protected:

	friend class SolidVertexIterator;
	friend class SolidEdgeIterator;
	friend class SolidFaceIterator;
	friend class SolidHalfEdgeIterator;
	friend class DSolid;
	friend class WSolid;
	friend class TDSolid;
	friend class Configure;
	friend class SolidDelegate;

	//!  Create a new vertex.
	/*!      
      \param id a int which is the id of the new vertex (default = 0).
      \return a tVertex which is the new vertex.
    */
	tVertex   createVertex(   int id = 0 );
	//!  Destroy a vertex
	/*!
	  \param vertex a vertex which is to be destroyed
	*/
	void destroyVertex(Vertex * vertex);
	//! Destroy an Edge
	/*!
	  \param edge an edge which is to be destroyed
	*/
	void destroyEdge(Edge * edge);
	//! Destroy a HalfEdge
	/*!
	  \param he a halfedge which is to be destroyed
	*/
	void destroyHalfEdge(HalfEdge * he);
	//! Destroy a Face
	/*!
	  \param face a face which is to be destroyed
    */
	void destroyFace(Face * face);
	//!  Create a new edge.
	/*!      
      \param start a int which is the id of the souce vertex of the edge.
	  \param end a int which is the id of the target vertex of the edge.
      \return a tEdge which is the new edge.
    */
	tEdge     createEdge( int start, int end );
	//!  Create a new edge.
	/*!      
      \param id a int which is the id of the new face (default = 0).
      \return a tFace which is the new face.
    */
	tFace     createFace( int id = 0 );
	//!  Create a new face.
	/*!      
      \param v a int pointer which contains 3 vertices position information.
	  \param id a int which is the id of the new face.
      \return a tFace which is the new face.
    */
	tFace     createFace( int * v, int id ); //create a triangle
	//! Collapse Edge
	/*!
	  \param edge an edge to be collapsed
	*/
	void	collapseEdge(Edge * edge);
	//! Collapse Edge according to vertex
	/*!
	  \param edge an edge to be collapsed
	  \param vertex the vertex on the edge which should be saved
	*/
	void	collapseEdgeVertex(Edge * edge, Vertex * vertex);
	bool	collapsable(HalfEdge * halfedge);

};


}//name space MeshLib

#endif //_MESHLIB_SOLID_H_ defined
