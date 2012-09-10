#include "stdafx.h"
#include "linear_algebra.h"
#include <cmath>
#include <cfloat>

#define		EPSILON			1.0e-7

#ifndef		MIN
#define		MIN(a,b)		( ((a) < (b)) ? (a) : (b) )
#endif

#ifndef		MAX
#define		MAX(a,b)		( ((a) > (b)) ? (a) : (b) )
#endif

int is_at_right(edge * e, double x, double y) {
	if (!e)	return 0;

	double	vx	= e->x2 - e->x1; // v - direction vector
	double	vy	= e->y2 - e->y1;

	double	nx	= vy;			// right normal to v
	double	ny	= -vx;

	double	b	= nx*e->x1 + ny*e->y1;	// distance from line to frame origin
	double	d;					// distance from line to point, if > 0 than at right side, else at left

	d	= (x*nx + y*ny) - b;

	if (abs(d) < EPSILON)	{
		if ( (x <= (MAX(e->x1, e->x2))) && (x >= (MIN(e->x1, e->x2))) )
			return 0;			// coincide in
		return -2;				// coincide out
	}
	if (d > 0)	return 1;		// at right
	return -1;					// at left
}

int ee_intersect(edge * e1, edge * e2, double &x, double &y)
{
	x = y = 0;
	if ((!e1) || (!e2))		return 0;

	int ret11	= is_at_right( e1, e2->x1, e2->y1 );
	int ret12	= is_at_right( e1, e2->x2, e2->y2 );
	int ret21	= is_at_right( e2, e1->x1, e1->y1 );
	int ret22	= is_at_right( e2, e1->x2, e1->y2 );

	if ( (ret11*ret12 == -1) && (ret21*ret22 == -1) )	{				// one intersection
		// find intersection
		double dx1	= e1->x2 - e1->x1;
		double dy1	= e1->y2 - e1->y1;
		double dx2	= e2->x2 - e2->x1;
		double dy2	= e2->y2 - e2->y1;

		double p1, q1, p2, q2;
		p1 = dy1/(double)dx1;
		p2 = dy2/(double)dx2;

		q1 = e1->y1 - e1->x1 * p1;
		q2 = e2->y1 - e2->x1 * p2;

		x = - (e2->y1*dx1*dx2 - e2->x1*dy2*dx1 - e1->y1*dx1*dx2 + e1->x1*dy1*dx2) / (dy2*dx1 - dy1*dx2);
		y = p1*x + q1;

		return 1;														// one intersection
	}

	if (((ret11 == -2) || (ret11 == 0)) && ((ret12 == 0) || (ret12 == -2)))
		return 2;		// coincide two points

	if (ret11 == 0)	{ x = e2->x1; y = e2->y1;			return 1;}		// one point coincide
	if (ret12 == 0)	{ x = e2->x2; y = e2->y2;			return 1;}
	if (ret21 == 0)	{ x = e1->x1; y = e1->y1;			return 1;}
	if (ret22 == 0)	{ x = e1->x2; y = e1->y2;			return 1;}

	return 0;			// no intersections
}

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

int la_solve_lu(double *a, double const*b, double *x, int m, void *buf)
{
	if (!a || !b || !x) return 0;

	double max, sum, tmp;
	int i, j, k;
	int maxi = -1;

	int *		idx		= (int *)buf;
	double *	work	= (double *)(idx + m);

	// copy b to x
	for (i = 0; i < m; ++i) x[i] = b[i];

	// compute the LU decomposition of a row permutation of matrix a; the permutation itself is saved in idx[]
	for (i = 0; i < m; ++i) {
		max = 0;
		for (j = 0; j < m; ++j)
			if ((tmp = fabs(a[i*m+j])) > max)
				max = tmp;
		if (max == 0) return 0;	// Singular matrix A
		work[i] = 1 / max;
	}

	for (j = 0; j < m; ++j) {
		for (i = 0; i < j; ++i) {
			sum = a[i*m+j];
			for (k = 0; k < i; ++k)
				sum -= a[i*m+k] * a[k*m+j];
			a[i*m+j] = sum;
		}
		max = 0.0;
		for (i = j; i < m; ++i) {
			sum = a[i*m+j];
			for (k = 0; k < j; ++k)
				sum -= a[i*m+k] * a[k*m+j];
			a[i*m+j] = sum;
			if ((tmp = work[i]*fabs(sum)) >= max) {
				max = tmp;
				maxi = i;
			}
		}
		if (j != maxi) {
			for (k = 0; k < m; ++k) {
				tmp = a[maxi*m+k];
				a[maxi*m+k] = a[j*m+k];
				a[j*m+k] = tmp;
			}
			work[maxi] = work[j];
		}
		idx[j] = maxi;
		if (a[j*m+j] == 0.0)
			a[j*m+j] = DBL_EPSILON;
		if (j != m-1) {
			tmp = 1 / (a[j*m+j]);
			for (i = j+1; i < m; ++i)
				a[i*m+j] *= tmp;
		}
	}

	// The decomposition has now replaced a. Solve the linear system using
	// forward and back substitution
	for (i = k = 0; i < m; ++i) {
		j = idx[i];
		sum = x[j];
		x[j] = x[i];
		if (k != 0)
			for (j = k-1; j < i; ++j)
				sum -= a[i*m+j] * x[j];
		else
			if (sum != 0.0)
				k = i+1;
		x[i] = sum;
	}

	for (i = m - 1; i >= 0; --i) {
		sum = x[i];
		for (j = i + 1; j < m; ++j)
			sum -= a[i*m+j] * x[j];
		x[i] = sum / a[i*m+i];
	}

	return 1;
}
