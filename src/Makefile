# loop-subdivision
#
# Author   : Mi, Liang (Arizona State University)
# Email    : icemiliang@gmail.com
# Date     : June 13th 2018

# Load the configuration file
include ../config.mk

# List of the source files
objs = Boundary.o LeastSquare.o Edge.o Face.o FormTrait.o HalfEdge.o\
       Homology.o Mesh.o Vertex.o Graph.o
src=$(patsubst %.o,%.cpp,$(objs))

# Makefile rules
all: $(objs) libmesh++.a

depend:
	$(CXX) $(E_INC) -MM $(src) >Makefile.dep

include Makefile.dep

libmesh++.a: $(objs)
	rm -f libmesh++.a
	ar rs libmesh++.a $^

%.o: %.cpp
	$(CXX) $(CFLAGS) $(E_INC) -c $<


clean:
	rm -f $(objs)
	rm -f libmesh++.a
