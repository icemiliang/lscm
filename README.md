# Vot++ 
## Least square conformal mapping

This package includes the propotype codes for computing variational optimal transportation (VOT) in the 3D Euclidean space.

## Build and run

Simply run:
```
$ make clean
$ make -B
```

Also, check Makefile, config.mk to change paths to requied libraries, e.g. Eigen.

A command-line program is built from vot.cpp in build/

A minimum command for running vot:
```
$ ./vot -e data/sample.em -d data/sample.dm 
```

## Dependences
Boost 1.58, Eigen 3 

The code has been tested on Ubuntu 16.04 with g++ 5.5.0

Boost is mainly for I/O and is not necessary for using vot++. You can skip it if you want to use vot++ as a library.

## Options
```
  -d [ --dirac ] arg                    Particles associated with Dirac measures
  
  -e [ --empirical ] arg                Test points associated with Empirical measures
  
  -p [ --iterP ] arg (=10)              Max iteration for updating positions of centroids/Dirac measures. 
                                        Default is 10.
  
  -h [ --iterH ] arg (=1000)            Max iteration for updating 'H', or size of cells. Default is 1000.
  
  -t [ --threshold ] arg (=1e-6)        Threshold for terminating the program. Default is 1e-06
  
  -s [ --scale ] arg (=1)               Scale of the output diagram. Default is 1.
  
  -r [ --rate ] arg (=0.2)              Learning rate for gradient descent. Default is 0.2. Not needed for
                                        Newtons's method.
  
  -o [ --outdir ] arg                   Output directory
  
  -h [ --help ]                         Print help messages
```

## Input and output files
Sample input and output files are provided in Demo.

### Inputs:

  *.dm specifies Dirac measures

  *.em specifies Empirical measures

  This program mainly deals with unstructured 3D point clouds. For 2D clouds, simply set 'z' values to zeros. It still regards the data as of 3D.

  All samples must be within the range of (-1,1) in each dimension. The current version only supports a cubic boundary. Spherical boundary is comming soon.

### Outputs:

  *.dm specifies the resulting Dirac measures.

  *.gnu specifies the resulting power Voronoi diagram.

Output files are compatible with Gnuplot. A sample Gnuplot script (gnuplotScripts.txt) is also provided in build.

## Code structure
The command-line program is built from build/vot.cpp

The main body of the vot++ code is in src/ot.cc and src/diagram.cc

We give credit to Voro++ (http://math.lbl.gov/voro++/) for computing power Voronoi diagrams. Many files in src/ are from Voro++, with our modification.

## Reference
If you find the code helpful, please cite the following article:

Mi, Liang, Wen Zhang, Junwei Zhang, Yonghui Fan, Dhruman Goradia, Kewei Chen, Eric M. Reiman, Xianfeng Gu, and Yalin Wang. "An Optimal Transportation Based Univariate Neuroimaging Index." In Proceedings of the IEEE Conference on Computer Vision and Pattern Recognition, pp. 182-191. 2017.

```
@inproceedings{mi2017optimal,
  title={An Optimal Transportation Based Univariate Neuroimaging Index},
  author={Mi, Liang and Zhang, Wen and Zhang, Junwei and Fan, Yonghui and Goradia, Dhruman and Chen, Kewei and Reiman, Eric M and Gu, Xianfeng and Wang, Yalin},
  booktitle={Proceedings of the IEEE Conference on Computer Vision and Pattern Recognition},
  pages={182--191},
  year={2017}
}
```

## Contact
Please contact Liang Mi icemiliang@gmail.com for any issues. 
