# LSCM
## Least squares conformal mapping

This package includes the prototype code for implementing least squares conformal maps.

![alt text](data/human.jpg?raw=true "Least squares conformal mapping")

## Build

[![Build status](https://ci.appveyor.com/api/projects/status/mxjqk9rqgiaec40d?svg=true)](https://ci.appveyor.com/project/icemiliang/lscm)

In the root directory, run:
```
$ rm -r build
$ mkdir build
$ cd build
$ cmake ..
$ make
```

The program has been tested on Ubuntu 16.04 with g++ 5.4.0. It requires Eigen 3 for solving linear systems, which was included, but if you want to use you own Eigen library, please modify [CMakeLists.txt](CMakeLists.txt).

## Usage
```
./main ../data/human.obj output.obj
```

LSCM requires at least two fixed vertices. Search in human.obj for "fix" and you will find two such fixed vertices having an additional trait "fix u v" after the xyz coordinates. In human.obj, these two vertices are somewhere at the two eyes. These two vertices will be mapped to (u,v). You can specify your own fixed vertices in the same way. 

## References
Gu, Xianfeng David. Computational conformal geometry. Edited by Shing-Tung Yau. Somerville, Mass, USA: International Press, 2008.

Lévy, Bruno, Sylvain Petitjean, Nicolas Ray, and Jérome Maillot. "Least squares conformal maps for automatic texture atlas generation." In ACM transactions on graphics (TOG), vol. 21, no. 3, pp. 362-371. ACM, 2002.

## Contact
Please contact Liang Mi icemiliang@gmail.com for any issues. 

Note: There were a few questions about texture mapping using LSCM. I honestly don't have experience with texture mapping, but I think Lévy's paper (referenced above) introduces a pipeline to do that. Below is what I understand.

1. Segment the mesh into smaller pieces
2. Map every piece onto the same uv plane (by giving each piece different fixed uv points)
3. Arranage the flattened meshes on the plane to avoid overlap and make them fit into a rectangular as tight as possible
4. Apply texture on the uv plane 
5. Use the correspondence between the 2D and 3D positions of each vertex to apply the texture to the original mesh

LSCM only serves step 2. This pipeline should work on either meshes with boundaries or water-tight meshes. However, non-genus zero meshes could be a problem.
