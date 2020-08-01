cmake_minimum_required(VERSION 3.1)
project(LSCM)
set(CMAKE_CXX_STANDARD 11)

include_directories(src)

# Include the Eigen library
include_directories(eigen3)
#find_package (Eigen3 3.3 REQUIRED NO_MODULE)
#include_directories(${EIGEN3_INCLUDE_DIR})

set(SOURCES src/Boundary.cpp src/Edge.cpp src/Face.cpp src/FormTrait.cpp src/LSCM.cpp
            src/HalfEdge.cpp src/Homology.cpp src/Mesh.cpp src/Point.cpp src/Vertex.cpp src/Graph.cpp)

add_executable(main main.cpp ${SOURCES})
