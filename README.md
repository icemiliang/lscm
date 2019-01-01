# LSCM
## Least squares conformal mapping

This package includes the prototype code for implementing least squares conformal maps.

![alt text](data/human.jpg?raw=true "Least squares conformal mapping")

## Build

[![Build status](https://ci.appveyor.com/api/projects/status/mxjqk9rqgiaec40d?svg=true)](https://ci.appveyor.com/project/icemiliang/lscm)

In the root directory, run:
```
$ make clean && make
```

The program has been tested on Ubuntu 16.04 with g++ 5.5.0. It requires Eigen 3 for solving linear systems, which was included, but if you want to use you own Eigen library, update the path in [config.mk](config.mk).

## Usage
```
./lscm input.obj output.obj [output.ply]
```
The last argument is an additional output in the ply format. MeshLab cannot load vertex normal from obj but can from ply, at least on my computer (Ubuntu 16.04 + MeshLab v1.3.2_64bit). You can ask for a ply output if needed.

LSCM requires two fixed vertices. Search in human.obj for "fix" and you will find two such fixed vertices having an additional trait "fix" after xyz coordinates. In human.obj, these two vertices are somewhere at the two eyes. These two vertices will be mapped to hard-coded positions -- (0,0) and (0,0.8). You can specify your own fixed vertices in the same way. Check out [LSCM::parameterize](src/lscm.cpp) to change the cooresponding uv coordinates.

## References
Gu, Xianfeng David. Computational conformal geometry. Edited by Shing-Tung Yau. Somerville, Mass, USA: International Press, 2008.

Lévy, Bruno, Sylvain Petitjean, Nicolas Ray, and Jérome Maillot. "Least squares conformal maps for automatic texture atlas generation." In ACM transactions on graphics (TOG), vol. 21, no. 3, pp. 362-371. ACM, 2002.

## Contact
Please contact Liang Mi icemiliang@gmail.com for any issues. 
