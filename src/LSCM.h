#ifndef _LSCM_H_
#define _LSCM_H_

#include <vector>
#include <list>

#include "Mesh.h"
#include "Iterators.h"
#include "FormTrait.h"


namespace MeshLib {

class LSCM {
public:
    LSCM(Mesh *mesh);
    ~LSCM();

    void project();
protected:
    Mesh *m_mesh;
    std::vector<Vertex*> m_fix_vertices;

    void set_coefficients();
};

}

#endif
