#ifndef _LEASTSQUARE_H_
#define _LEASTSQUARE_H_

#include <vector>
#include <list>

#include "Mesh.h"
#include "Iterators.h"
#include "FormTrait.h"

namespace MeshLib {
class LeastSquare {
public:
	LeastSquare(Mesh *mesh);
	~LeastSquare();

	void parameterize();
protected:
	Mesh *m_mesh;
	std::vector<Vertex*> m_fix_vertices;

	void set_coefficients();
	void compute_vertex_valence();
};
}

#endif
