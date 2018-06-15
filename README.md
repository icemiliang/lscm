# LSCM
## Least squares conformal mapping

This package includes the prototype codes for implementing least squares conformal maps.

![alt text](data/human.jpg?raw=true "Least squares conformal mapping")

## Build

In the root directory, run:
```
$ make clean
$ make
```

The program requires Eigen for solving a linear system. Edit config.mk to change its directory.

## Usage
```
./subdiv input.obj output.obj [output.ply]
```
The last argument is an additional output in ply format. MeshLab cannot load vertex normal from obj but can from ply, at least on my computer (Ubuntu 16.04 + MeshLab v1.3.2_64bit).

## References
Gu, Xianfeng David. Computational conformal geometry. Edited by Shing-Tung Yau. Somerville, Mass, USA: International Press, 2008.

Lévy, Bruno, Sylvain Petitjean, Nicolas Ray, and Jérome Maillot. "Least squares conformal maps for automatic texture atlas generation." In ACM transactions on graphics (TOG), vol. 21, no. 3, pp. 362-371. ACM, 2002.

## Contact
Please contact Liang Mi icemiliang@gmail.com for any issues. 
