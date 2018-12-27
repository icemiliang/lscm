#include "Point.h"

using namespace MeshLib;

std::ostream & operator<<( std::ostream & os, const Point & p) {
	os << "Point: " << p(0) << " " << p(1) << " " << p(2) << std::endl;
	return os;
}

Point Point::rotate(double theta, Point vector) {
	Point result;
	double cos_t = cos(theta);
	double sin_t = sin(theta);
	result = vector * (v[0] * vector[0] + v[1] * vector[1] + v[2] * vector[2]) * (1-cos_t);
	result[0] += v[0] * cos_t;
	result[1] += v[1] * cos_t;
	result[2] += v[2] * cos_t;
	result[0] += (v[1] * vector[2] - v[2] * vector[1]) * sin_t;
	result[1] += (v[2] * vector[0] - v[0] * vector[2]) * sin_t;
	result[2] += (v[0] * vector[1] - v[1] * vector[0]) * sin_t;
	return result;
}
