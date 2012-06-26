#ifndef _MATH__LINEAR_ALGEBRA_H_
#define _MATH__LINEAR_ALGEBRA_H_

typedef struct edge{
	double x1,y1,x2,y2;
} edge;

// find edge-edge intersection
// e1, e2	- edges
// x,y		- intersection coords
// return	- 0 - no intersection, 1 - intersection, 2 - coincide
int ee_intersect(edge * e1, edge * e2, double &x, double &y);

// check if point is at right to an edge (in right ref. frame)
// e		- edge
// x,y		- point coords
// return	- -1 - at left, 1 - at right, 0 - coincide in, -2 coincide out
int is_at_right(edge * e, double x, double y);


// This function returns the solution of Ax = b
//
// The function employs LU decomposition followed by forward/back substitution
//
// A is mxm, b is mx1
//
// The function returns 0 in case of error,
// 1 if successfull
//
// required buffer size: m * sizeof(int) + m * sizeof(double)
// matrix a is modified by the function

int la_solve_lu(double *a, double const*b, double *x, int m, void *buf);

#endif
