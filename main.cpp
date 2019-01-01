// Least squares conformal mapping
//
// Author   : Mi, Liang (Arizona State University)
// Email    : icemiliang@gmail.com
// Date     : June 13th 2018

#include <cmath>
#include "Mesh.h"
#include "FormTrait.h"
#include "lscm.h"
#include <iostream>

using namespace MeshLib;

int main(int argc, char * argv[]) {
	std::cout << "--> Reading mesh..." << std::endl;
	Mesh mesh;
	mesh.read_obj(argv[1]);

	FormTrait traits(&mesh);

	std::cout << "--> Computing conformal map..." << std::endl;
	LSCM lscm(&mesh);
	lscm.parameterize();

	std::cout << "--> Writing mesh..." << std::endl;
	mesh.write_obj(argv[2]);
	if (argc > 3)
		mesh.write_ply(argv[3]);
	return 0;
}
