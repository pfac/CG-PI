#include "latk.h"

#include <cmath>

/*
using ublas::vector;

double vector_norm (const vector<double>& input)
{
	double accumulator = 0;
	vector<double>::const_iterator it;
	for (it = input.begin(); it < input.end(); ++it)
		accumulator += ( (*it) * (*it) );
	return sqrt(accumulator);
}

vector<double>& vector_normalize (vector<double>& input)
{
	double norm;
	norm = vector_norm(input);
	input /= norm;
	return input;
}

double vector_inner_product (const vector<double>& x, const vector<double>& y)
{
	int i;
	double accumulator = 0;
	for (i = 0; i < (int) x.size(); ++i)
		accumulator += ( x(i) * y(i) );
	return accumulator;
}
*/

double vector3d_norm (double *v)
{
	return sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
}

double vector3d_angle (double *u, double *v)
{
	double nu, nv, p;
	nu = vector3d_norm(u);
	nv = vector3d_norm(v);
	p = vector3d_inner_product(u,v);
	return acos( p / (nu * nv) );
}

double vector3d_inner_product (double *u, double *v)
{
	return ( u[0]*v[0] + u[1]*v[1] + u[2]*v[2] );
}

void vector3d_cross_product (double *u, double *v, double *w)
{
	w[0] = u[1]*v[2] - u[2]*v[1];
	w[1] = - ( u[0]*v[2] - u[2]*v[0] );
	w[2] = u[0]*v[1] - u[1]*v[0];
}



/*

double vector_norm (double *v, unsigned size)
{
	return sqrt( vector_inner_product(v,v,size) );
}

double vector_inner_product (double *u, double *v, unsigned size)
{
	unsigned i;
	double sum = 0;
	for (i = 0; i < size; ++i)
		sum += ( u[i]*v[i] );
	return sum;
}

double vector_angle (double *u, double *v)
{
	double nu, nv, p;
	nu = vector3d_norm(u);
	nv = vector3d_norm(v);
	p = vector3d_inner_product(u,v);
	return acos( p / (nu * nv) );
}



void vector_cross_product (double *u, double *v, double *w)
{
	w[0] = u[1]*v[2] - u[2]*v[1];
	w[1] = - ( u[0]*v[2] - u[2]*v[0] );
	w[2] = u[0]*v[1] - u[1]*v[0];
}
*/