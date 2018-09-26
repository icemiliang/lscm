#include "LeastSquare.h"
#include <assert.h>
#include <math.h>
#include <float.h>
#include <Eigen/Sparse>
#include <iostream>
#include <Eigen/IterativeLinearSolvers>

using namespace MeshLib;

typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::VectorXd VectorXd;

LeastSquare::LeastSquare(Mesh * mesh)
{
	m_mesh = mesh;
}

LeastSquare::~LeastSquare()
{
}

void LeastSquare::set_coefficients()
{
	for (MeshEdgeIterator eiter(m_mesh); !eiter.end(); ++eiter) {
		Edge * e = *eiter;
		e_l(e) = m_mesh->edge_length(e);
	}

	for (MeshFaceIterator fiter(m_mesh); !fiter.end(); ++fiter) {
		Point  p[3];

		Face * f = *fiter;

		double l[3];
		HalfEdge * he = f->halfedge();
		for (int j = 0; j < 3; j++) {
			Edge * e = he->edge();
			l[j] = e_l(e);
			he = he->he_next();
		}

		double a = acos((l[0] * l[0] + l[2] * l[2] - l[1] * l[1]) / (2 * l[0] * l[2]));

		p[0] = Point(0, 0, 0);
		p[1] = Point(l[0], 0, 0);
		p[2] = Point(l[2] * cos(a), l[2] * sin(a), 0);

		Point n = (p[1] - p[0]) ^ (p[2] - p[0]);
		double   area = n.norm() / 2.0;
		n = n / area;

		he = f->halfedge();
		for (int j = 0; j < 3; j++) {
			Point   s = (n ^ (p[(j + 1) % 3] - p[j])) / sqrt(area);
			c_s(he) = s;
			he = he->he_next();
		}
	}
}

void LeastSquare::parameterize()
{
	set_coefficients();
	compute_vertex_valence();

	std::vector<Vertex*> vertices;
	std::vector<Face*>   faces;

	for (MeshVertexIterator viter(m_mesh); !viter.end(); ++viter){
		Vertex * v = *viter;
		if (v_valence(v) > 1) {
			m_fix_vertices.push_back(v);
		}
		else {
			vertices.push_back(v);
		}
	}
	assert(m_fix_vertices.size()>=2);

	for (int k = 0; k < (int)vertices.size(); k++ ){
		v_idx(vertices[k]) = k;
	}

	for (int k = 0; k < (int)m_fix_vertices.size(); k++) {
		v_idx(m_fix_vertices[k]) = k;

	}
	Vertex * v0 = m_fix_vertices[0];
	Vertex * v1 = m_fix_vertices[1];

	v_uv(v0) = Point2(0, 0);
	v_uv(v1) = Point2(0, 0.8);
	for (MeshFaceIterator fiter(m_mesh); !fiter.end(); ++fiter) {
		// Face * f = *fiter;
		faces.push_back(*fiter);
	}
	int fn = m_mesh->numFaces();
	int	vfn = 2;
	int	vn = m_mesh->numVertices() - vfn;
	typedef Eigen::Triplet<double> T;
	std::vector<T> tripletList1;
	std::vector<T> tripletList2;
	tripletList1.reserve((int)faces.size());
	tripletList2.reserve((int)faces.size());
	VectorXd b(vfn * 2);


	for (int fid = 0; fid < (int)faces.size(); fid++) {
		Face * f = faces[fid];

		HalfEdge * he = f->halfedge();

		for (int j = 0; j < 3; j++) {
			Point s = c_s(he);

			Vertex * v = he->he_next()->target();

			int vid = v_idx(v);

			if (v_valence(v) < 2) {
				tripletList1.push_back(T(fid,vid,s[0]));
				tripletList1.push_back(T(fn + fid, vn + vid, s[0]));
				tripletList1.push_back(T(fid, vn + vid, -s[1]));
				tripletList1.push_back(T(fn + fid, vid, s[1]));
			}
			else {
				Point2 uv = v_uv(v);

				tripletList2.push_back(T(fid, vid, s[0]));
				tripletList2.push_back(T(fn + fid, vfn + vid, s[0]));
				tripletList2.push_back(T(fid, vfn + vid, -s[1]));
				tripletList2.push_back(T(fn + fid, vid, s[1]));

				b[vid] = uv[0];
				b[vfn + vid] = uv[1];
			}
			he = he->he_next();
		}
	}
	SpMat A(2 * fn, 2 * vn);
	SpMat B(2 * fn, 2 * vfn);

	A.setFromTriplets(tripletList1.begin(), tripletList1.end());
	B.setFromTriplets(tripletList2.begin(), tripletList2.end());

	VectorXd r, x;
	r = B * b;
	r = r * -1;

	Eigen::LeastSquaresConjugateGradient<Eigen::SparseMatrix<double>> lscg;
	lscg.compute(A);
	x = lscg.solve(r);

	for (int i = 0; i < vn; i++) {
		Vertex * v = vertices[i];
		v_uv(v) = Point2(x[i], x[i + vn]);
	}
	for (MeshVertexIterator viter(m_mesh); !viter.end(); viter++) {
		Vertex * v = *viter;
		//char uv_line[1024];
		Point2 p = v_uv(v);
		Point p3(p[0], p[1], 0);
		v->point() = p3;

		//if (v->string().size() > 0) {
		//	sprintf(uv_line, " uv=( %g %g )", p[0], p[1]);
		//}
		//else {
		//	sprintf(uv_line, "uv=( %g %g )", p[0], p[1]);
		//}

		//v->string() += uv_line;
		
	}
}

// void LeastSquare::compute_vertex_valence()
// {
// 	for (MeshEdgeIterator eiter(m_mesh); !eiter.end(); ++eiter) {
// 		Edge * e = *eiter;
// 		e_string(e) = e->string();
// 		EdgeTrait * pT = (EdgeTrait*)e->trait();
// 		pT->read();
// 	}

// 	for (MeshVertexIterator viter(m_mesh); !viter.end(); ++viter) {
// 		Vertex * v = *viter;
// 		v_valence(v) = 0;
// 		for (VertexEdgeIterator veiter(v); !veiter.end(); ++veiter) {
// 			Edge * e = *veiter;
// 			if (e_sharp(e)) v_valence(v)++;
// 		}
// 	}
// }

void LeastSquare::compute_vertex_valence()
{
	for (MeshVertexIterator viter(m_mesh); !viter.end(); ++viter) {
		Vertex * v = *viter;
		v_valence(v) = 0;
		if (v->string() == "fix") {
		 	v_valence(v) = 2;
		}
	}
}