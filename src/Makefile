# lscm
#
# Author   : Mi, Liang
# Email    : icemiliang@gmail.com
# Date     : Nov 28th 2018

include ../config.mk

objs = Boundary.o lscm.o Edge.o Face.o FormTrait.o HalfEdge.o\
       Homology.o Mesh.o Vertex.o Graph.o
src=$(patsubst %.o,%.cpp,$(objs))

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
