#include "Homology.h"
#include <iostream>

using namespace MeshLib;

Homology::Homology(Mesh *mesh) {
	m_mesh = mesh;
	m_graph = new Graph(m_mesh);
}

Homology::~Homology() {
	delete m_graph;

	for (std::list<std::list<HalfEdge*>*>::iterator liter = m_loops.begin(); liter != m_loops.end(); liter++) {
		std::list<HalfEdge*>* l = *liter;
		delete l;
	}
	m_loops.clear();
}

void Homology::compute_basis() {
	m_graph->propagation();
	for (MeshEdgeIterator me(m_mesh); !me.end(); me++) {
		Edge *e = *me;
		if (!e_sharp(e)) continue;
		m_graph->insert(e);
	}
	m_graph->breadth_first_search();
	m_graph->locate_loops();

	std::list<std::list<HalfEdge*>*> &loops = m_graph->loops();

	for (std::list<std::list<HalfEdge*>*>::iterator llh = loops.begin(); llh != loops.end(); llh++) {
		std::list<HalfEdge*>* l = *llh;
		std::list<HalfEdge*>* loop = new std::list < HalfEdge* > ;
		assert(loop);
		*loop = *l;
		m_loops.push_back(loop);
	}
}

void Homology::output(const char *prefix){
	int id = 1;
	for (std::list<std::list<HalfEdge*>*>::iterator llh = m_loops.begin(); llh != m_loops.end(); llh++) {
		std::list<HalfEdge*>* l = *llh;
		for (MeshEdgeIterator me(m_mesh); !me.end(); me++) {
			Edge *e = *me;
			e->string().clear();
		}
		for (std::list<HalfEdge*>::iterator lh = l->begin(); lh != l->end(); lh++) {
			HalfEdge *he = *lh;
			Edge *e = he->edge();
			e->string() = "sharp";
		}
		char name_m[1024];
		sprintf(name_m, "%s_basis_%d.m", prefix, id++);
		m_mesh->write_m(name_m);
	}

	for (MeshEdgeIterator me(m_mesh); !me.end(); me++) {
		Edge *e = *me;
		e->string().clear();
	}
	for (std::list<std::list<HalfEdge*>*>::iterator llh = m_loops.begin(); llh != m_loops.end(); llh++) {
		std::list<HalfEdge*>* l = *llh;
		for (std::list<HalfEdge*>::iterator lh = l->begin(); lh != l->end(); lh++) {
			HalfEdge *he = *lh;
			Edge *e = he->edge();
			e->string() = "sharp";
		}
	}

	// Write cut_graph
	char name_m[1024];
	sprintf(name_m, "%s_boundaries.m", prefix);
	m_mesh->write_m(name_m);


}
