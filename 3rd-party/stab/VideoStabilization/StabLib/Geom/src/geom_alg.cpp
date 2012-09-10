#include "stdafx.h"
#include "../inc/geom_alg.h"
#include "Libs/abacus.h"
//#include "Include/capture_log.h"
#include "Include/different_libs.h"

#include <algorithm>
#include <math.h>

Point3D GeomAlg::getGlobal(Point3D const& posit, Rotation rot, Point3D const& local) const
{
	Point3D global(posit);
	global += rot.rotate(local);
	return global;
}


double GeomAlg::shiftDistance(Point3D const** p1, Point3D const** p2, int size, Position const& shift, bool* use) const
{
	double dist = 0;
	int useCount = 0;
	for (int i = 0; i < size; ++i)
	{
		if (!use || use[i])
		{
			Point3D pt = shift.apply(*p2[i]);
			dist += p1[i]->dist(pt);
			++useCount;
		}
	}
	if (useCount == 0)
	{
		return 1e6;
	}
	dist /= useCount;
	return dist;
}

Position GeomAlg::getShift6D(Point3D const** p1, Point3D const** p2, int size, bool* use, int useCount) const
{
	//	Capture_Log::getInstance()->log_write("Points:\n");
	double* shift = new double[16];
	if (useCount == 0) useCount = size;
	double* m1 = new double[6 * useCount];
	double* m2 = new double[6 * useCount];
	int place = 0;
	for (int i = 0; i < size; ++i)
	{
		if (!use || use[i])
		{
			p1[i]->export3D(m1 + place * 6);
			p2[i]->export3D(m2 + place * 6);
			for (int j = 3; j < 6; ++j)
			{
				m1[place * 6 + j] = 0;
				m2[place * 6 + j] = 0;
			}
			++place;
		}
	}

	double scale = 1;
	int res = similarity_6d_estimate(shift, m1, m2, useCount, scale, 0);
	Point3D pos = Point3D(shift[3], shift[7], shift[11]);
	double* rt = new double[9];
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			rt[i * 3 + j] = shift[i * 4 + j];
		}
	}
	Rotation rot(rt); // Do not need to delete rt!
	rot.fixDet();
	delete[] m1;
	delete[] m2;
	delete[] shift;
	return Position(pos, rot);
}

Position GeomAlg::getShift(Point3D const** p1, Point3D const** p2, int size, bool* use, int useCount) const
{
	//	Capture_Log::getInstance()->log_write("Points:\n");
	double* shift = new double[16];
	if (useCount == 0) useCount = size;
	double* m1 = new double[3 * useCount];
	double* m2 = new double[3 * useCount];
	int place = 0;
	for (int i = 0; i < size; ++i)
	{
		if (!use || use[i])
		{
			p1[i]->export3D(m1 + place * 3);
			p2[i]->export3D(m2 + place * 3);
			++place;
		}
	}

	double scale = 1;
	int res = similarity_3d_estimate(shift, m1, m2, useCount, 0);
	Point3D pos = Point3D(shift[3], shift[7], shift[11]);
	double* rt = new double[9];
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			rt[i * 3 + j] = shift[i * 4 + j];
		}
	}
	Rotation rot(rt); // Do not need to delete rt!
	rot.fixDet();
	delete[] m1;
	delete[] m2;
	delete[] shift;
	return Position(pos, rot);
}

Position GeomAlg::getShiftHull( Point3D const** p1, Point3D const** p2, int size, bool* use, int useCount) const
{
	Point3D** point1;
	Point3D** point2;
	size_t sz;
	int koeff = 10;
	int place = 0;
	if (use)
	{
		sz = koeff * useCount;
	}
	else
	{
		sz = koeff * size;
	}
	point1 = new Point3D*[sz];
	point2 = new Point3D*[sz];
	for (int i = 0; i < size; ++i)
	{
		if (!use || use[i])
		{
			point1[place] = new Point3D(*p1[i]);
			point2[place] = new Point3D(*p2[i]);
			place++;
		}
	}
	hull(point1, point2, place, sz);
	Position ps = getShift((Point3D const**)point1, (Point3D const**)point2, sz);

	for (size_t i = 0; i < sz; ++i)
	{
		delete point1[i];
		delete point2[i];
	}
	delete[] point1;
	delete[] point2;
	return ps;
}

void GeomAlg::hull(Point3D** p1, Point3D** p2, size_t sz, size_t maxSize) const
{
	size_t place = sz;
	srand(GetTickCount() % 10000);
	while (place < maxSize)
	{
		double weight = dif_lib::randomDouble(1);
		int place1 = dif_lib::randomInt(sz);
		int place2 = dif_lib::randomInt(sz);
		Point3D* p1New = new Point3D((*p1[place1]) * weight);
		(*p1New) += (*p1[place2]) * (1.0 - weight);
		Point3D* p2New = new Point3D((*p2[place1]) * weight);
		(*p2New) += (*p2[place2]) * (1.0 - weight);
		p1[place] = p1New;
		p2[place] = p2New;
		place++;
	}
}



/*
mask - valid points - only these will be used. mask == 0 - using of all points
p2 = shift * p1;
*/
int GeomAlg::similarity_6d_estimate(double *shift, const double *m0, const double *m1, int count, double &scale, bool *mask) const
{
	double cmx0, cmy0, cmz0, cmx1, cmy1, cmz1, sq0, sq1, s;
	double M[9], buffer[9], W[3], U[9], V[9];
	int i;

	int nvalid = count;
	if (mask) 
	{
		nvalid = 0;
		for (i = 0; i < count; ++i)
			if (mask[i]) ++nvalid;
	}

	if (nvalid < 3) return 0;

	// calculate centroids of point sets
	cmx0 = cmy0 = cmz0 = 0;
	cmx1 = cmy1 = cmz1 = 0;
	for (i = 0; i < count; ++i)
	{
		if (!mask || mask[i]) 
		{
			cmx0 += m0[6*i + 0], cmy0 += m0[6*i + 1], cmz0 += m0[6*i + 2];
			cmx1 += m1[6*i + 0], cmy1 += m1[6*i + 1], cmz1 += m1[6*i + 2];
		}
	}
	cmx0 /= nvalid; cmy0 /= nvalid; cmz0 /= nvalid;
	cmx1 /= nvalid; cmy1 /= nvalid; cmz1 /= nvalid;

	// calculate correlation matrix and scale coefficients
	sq0 = sq1 = 0.0;
	memset(M, 0, 9 * sizeof(double));
	for (i = 0; i < count; i++, m0 += 6, m1 += 6)
	{
		if (!mask || mask[i]) 
		{
			M[0] += (m0[0] - cmx0) * (m1[0] - cmx1);	M[1] += (m0[0] - cmx0) * (m1[1] - cmy1);	M[2] += (m0[0] - cmx0) * (m1[2] - cmz1);
			M[3] += (m0[1] - cmy0) * (m1[0] - cmx1);	M[4] += (m0[1] - cmy0) * (m1[1] - cmy1);	M[5] += (m0[1] - cmy0) * (m1[2] - cmz1);
			M[6] += (m0[2] - cmz0) * (m1[0] - cmx1);	M[7] += (m0[2] - cmz0) * (m1[1] - cmy1);	M[8] += (m0[2] - cmz0) * (m1[2] - cmz1);

			M[0] += m0[3+0]*m1[3+0];	M[1] += m0[3+0]*m1[3+1];	M[2] += m0[3+0]*m1[3+2];
			M[3] += m0[3+1]*m1[3+0];	M[4] += m0[3+1]*m1[3+1];	M[5] += m0[3+1]*m1[3+2];
			M[6] += m0[3+2]*m1[3+0];	M[7] += m0[3+2]*m1[3+1];	M[8] += m0[3+2]*m1[3+2];

			sq0 += (m0[0] - cmx0) * (m0[0] - cmx0) + (m0[1] - cmy0) * (m0[1] - cmy0) + (m0[2] - cmz0) * (m0[2] - cmz0);
			sq1 += (m1[0] - cmx1) * (m1[0] - cmx1) + (m1[1] - cmy1) * (m1[1] - cmy1) + (m1[2] - cmz1) * (m1[2] - cmz1);
		}
	}

	s = sqrt(sq1 / sq0);
	m0 -= 6 * count;
	m1 -= 6 * count;

	SVD(M, 3, 3, 3, W, U, 3, V, 3, buffer);

	// M = V*Ut
	M[0] = V[0] * U[0] + V[3] * U[3] + V[6] * U[6];
	M[1] = V[0] * U[1] + V[3] * U[4] + V[6] * U[7];
	M[2] = V[0] * U[2] + V[3] * U[5] + V[6] * U[8];

	M[3] = V[1] * U[0] + V[4] * U[3] + V[7] * U[6];
	M[4] = V[1] * U[1] + V[4] * U[4] + V[7] * U[7];
	M[5] = V[1] * U[2] + V[4] * U[5] + V[7] * U[8];

	M[6] = V[2] * U[0] + V[5] * U[3] + V[8] * U[6];
	M[7] = V[2] * U[1] + V[5] * U[4] + V[8] * U[7];
	M[8] = V[2] * U[2] + V[5] * U[5] + V[8] * U[8];

	double det = mat3x3_det(M);
	if ( det < 0 )
	{
		// M = V*Q*Ut, Q = { 1 0 0
		//					 0 1 0 
		//					 0 0 -1 }

		U[6] *= -1.0; U[7] *= -1.0; U[8] *= -1.0;
		M[0] = V[0] * U[0] + V[3] * U[3] + V[6] * U[6];
		M[1] = V[0] * U[1] + V[3] * U[4] + V[6] * U[7];
		M[2] = V[0] * U[2] + V[3] * U[5] + V[6] * U[8];

		M[3] = V[1] * U[0] + V[4] * U[3] + V[7] * U[6];
		M[4] = V[1] * U[1] + V[4] * U[4] + V[7] * U[7];
		M[5] = V[1] * U[2] + V[4] * U[5] + V[7] * U[8];

		M[6] = V[2] * U[0] + V[5] * U[3] + V[8] * U[6];
		M[7] = V[2] * U[1] + V[5] * U[4] + V[8] * U[7];
		M[8] = V[2] * U[2] + V[5] * U[5] + V[8] * U[8];
	}

	for (i = 0; i < 9; i++)
	{
		M[i] *= s;
	}

	scale = s;

	shift[0]  = M[0];	shift[1]  = M[1];	shift[2]  = M[2];	shift[3]  = cmx1 - M[0] * cmx0 - M[1] * cmy0 - M[2] * cmz0;
	shift[4]  = M[3];	shift[5]  = M[4];	shift[6]  = M[5];	shift[7]  = cmy1 - M[3] * cmx0 - M[4] * cmy0 - M[5] * cmz0;
	shift[8]  = M[6];	shift[9]  = M[7];	shift[10] = M[8];	shift[11] = cmz1 - M[6] * cmx0 - M[7] * cmy0 - M[8] * cmz0;
	shift[12] = 0;		shift[13] = 0;		shift[14] = 0;		shift[15] = 1;

	if ( M[0] < 0 || M[4] < 0 || M[8] < 0 ) return 0;
	return 1;
}

int	GeomAlg::similarity_3d_estimate(double *H, const double *m0, const double *m1, int count, bool *mask) const
{
	double cmx0, cmy0, cmz0, cmx1, cmy1, cmz1, sq0, sq1, s;
	double M[9], buffer[9], W[3], U[9], V[9];
	int i;

	int nvalid = count;
	if (mask) {
		nvalid = 0;
		for (i = 0; i < count; i++)
			if (mask[i]) nvalid++;
	}

	if (nvalid < 3) return 0;

	// calculate centroids of point sets
	cmx0 = cmy0 = cmz0 = 0;
	cmx1 = cmy1 = cmz1 = 0;
	for (i = 0; i < count; i++)
	{
		if (!mask || mask[i]) 
		{
			cmx0 += m0[3*i + 0], cmy0 += m0[3*i + 1], cmz0 += m0[3*i + 2];
			cmx1 += m1[3*i + 0], cmy1 += m1[3*i + 1], cmz1 += m1[3*i + 2];
		}
	}
	cmx0 /= nvalid; cmy0 /= nvalid; cmz0 /= nvalid;
	cmx1 /= nvalid; cmy1 /= nvalid; cmz1 /= nvalid;

	// calculate correlation matrix and scale coefficients
	sq0 = sq1 = 0.0;
	memset(M, 0, 9 * sizeof(double));
	for (i = 0; i < count; i++, m0 += 3, m1 += 3)
	{
		if (!mask || mask[i]) 
		{
			M[0] += (m0[0] - cmx0) * (m1[0] - cmx1);	M[1] += (m0[0] - cmx0) * (m1[1] - cmy1);	M[2] += (m0[0] - cmx0) * (m1[2] - cmz1);
			M[3] += (m0[1] - cmy0) * (m1[0] - cmx1);	M[4] += (m0[1] - cmy0) * (m1[1] - cmy1);	M[5] += (m0[1] - cmy0) * (m1[2] - cmz1);
			M[6] += (m0[2] - cmz0) * (m1[0] - cmx1);	M[7] += (m0[2] - cmz0) * (m1[1] - cmy1);	M[8] += (m0[2] - cmz0) * (m1[2] - cmz1);
			sq0 += (m0[0] - cmx0) * (m0[0] - cmx0) + (m0[1] - cmy0) * (m0[1] - cmy0) + (m0[2] - cmz0) * (m0[2] - cmz0);
			sq1 += (m1[0] - cmx1) * (m1[0] - cmx1) + (m1[1] - cmy1) * (m1[1] - cmy1) + (m1[2] - cmz1) * (m1[2] - cmz1);
		}
	}
	s = sqrt(sq1 / sq0);
	m0 -= 3 * count;
	m1 -= 3 * count;

	SVD(M, 3, 3, 3, W, U, 3, V, 3, buffer);

	// M = V*Ut
	M[0] = V[0] * U[0] + V[3] * U[3] + V[6] * U[6];
	M[1] = V[0] * U[1] + V[3] * U[4] + V[6] * U[7];
	M[2] = V[0] * U[2] + V[3] * U[5] + V[6] * U[8];

	M[3] = V[1] * U[0] + V[4] * U[3] + V[7] * U[6];
	M[4] = V[1] * U[1] + V[4] * U[4] + V[7] * U[7];
	M[5] = V[1] * U[2] + V[4] * U[5] + V[7] * U[8];

	M[6] = V[2] * U[0] + V[5] * U[3] + V[8] * U[6];
	M[7] = V[2] * U[1] + V[5] * U[4] + V[8] * U[7];
	M[8] = V[2] * U[2] + V[5] * U[5] + V[8] * U[8];

	double det = mat3x3_det(M);
	if ( det < 0 )
	{
		// M = V*Q*Ut, Q = { 1 0 0
		//					 0 1 0 
		//					 0 0 -1 }

		U[6] *= -1.0; U[7] *= -1.0; U[8] *= -1.0;
		M[0] = V[0] * U[0] + V[3] * U[3] + V[6] * U[6];
		M[1] = V[0] * U[1] + V[3] * U[4] + V[6] * U[7];
		M[2] = V[0] * U[2] + V[3] * U[5] + V[6] * U[8];

		M[3] = V[1] * U[0] + V[4] * U[3] + V[7] * U[6];
		M[4] = V[1] * U[1] + V[4] * U[4] + V[7] * U[7];
		M[5] = V[1] * U[2] + V[4] * U[5] + V[7] * U[8];

		M[6] = V[2] * U[0] + V[5] * U[3] + V[8] * U[6];
		M[7] = V[2] * U[1] + V[5] * U[4] + V[8] * U[7];
		M[8] = V[2] * U[2] + V[5] * U[5] + V[8] * U[8];
//		Capture_Log::getInstance()->log_write("Det was < 0\n");
	}

	/*			for (i = 0; i < 9; i++)
	M[i] *= s;*/

	H[0]  = M[0];	H[1]  = M[1];	H[2]  = M[2];	H[3]  = cmx1 - M[0] * cmx0 - M[1] * cmy0 - M[2] * cmz0;
	H[4]  = M[3];	H[5]  = M[4];	H[6]  = M[5];	H[7]  = cmy1 - M[3] * cmx0 - M[4] * cmy0 - M[5] * cmz0;
	H[8]  = M[6];	H[9]  = M[7];	H[10] = M[8];	H[11] = cmz1 - M[6] * cmx0 - M[7] * cmy0 - M[8] * cmz0;
	H[12] = 0;		H[13] = 0;		H[14] = 0;		H[15] = 1;

	return 1;
}

Point3D GeomAlg::massCenterEstimate(Point3D const** p1, size_t size, bool* use) const
{
	Point3D pc1(0);
	double count = 0;
	for (size_t i = 0; i < size; ++i)
	{
		if (!use || use[i])
		{
			count += 1;
			pc1 += *p1[i];
		}
	}
	pc1 *= 1.0 / count;
	return pc1;
}


Position GeomAlg::massCenterRotation(Point3D const** p1, Point3D const** p2, int size, bool* use, Position curPos) const
{
	Point3D pc1 = massCenterEstimate(p1, size, use);
	Point3D pc2 = massCenterEstimate(p2, size, use);

	Point3D pcn1 = (pc1 - curPos.pos).normalize();
	Point3D pcn2 = (pc2 - curPos.pos).normalize();
	Point3D vecM = vectorMult(pcn1, pcn2);
	double angle = asin(vecM.dist(Point3D(0)));
	vecM.normalize();
	Rotation rot(vecM, -angle);
	Position add1(Point3D(0), rot);

	for (size_t i = 0; i < (size_t)size; ++i)
	{
		Point3D pt = add1.apply(*p2[i]);
		((Point3D)*p2[i]) = pt;
	}
	return add1;
}



Point3D GeomAlg::vectorMult(Point3D const& p1, Point3D const& p2) const
{
	Point3D res(0);
	res.x = p1.y * p2.z - p1.z * p2.y;
	res.y = p1.z * p2.x - p1.x * p2.z;
	res.z = p1.x * p2.y - p1.y * p2.x;
	return res;
}

