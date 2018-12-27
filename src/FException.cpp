#include <iostream>
#include "FException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace MeshLib;

FException::FException() {
	msg = "failed to open file";
}
FException::FException(char *strg) {
	msg = strg;
}

FException::~FException(){}

void FException::what() {
	std::cout << msg << std::endl;
}
