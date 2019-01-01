# lscm makefile
#
# Author   : Mi, Liang 
# Email    : icemiliang@gmail.com
# Date     : Dec 31th 2018

include config.mk

all: objs main

objs:
	$(MAKE) -C src

main: main.cpp
	$(CXX) $(CFLAGS) $(E_INC) $(E_LIB) -o main main.cpp -lmesh++

clean:
	$(MAKE) -C src clean
	rm -f main
