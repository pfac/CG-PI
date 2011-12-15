#ifndef ___LATK_H___
#define ___LATK_H___

/*
#include <boost\numeric\ublas\vector.hpp>

namespace ublas = boost::numeric::ublas;

double vector_norm (const ublas::vector<double>& input);

ublas::vector<double>& vector_normalize (ublas::vector<double>& input);

double vector_inner_product (const ublas::vector<double>& x, const ublas::vector<double>& y);
*/

double vector3d_norm (double *v);

double vector3d_angle (double *u, double *v);

double vector3d_inner_product (double *u, double *v);

void vector3d_cross_product (double *u, double *v, double *w);

#endif/*___LATK_H___*/