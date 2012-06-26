#include "stdafx.h"
#include <math.h>
#include <iostream>

#include "Include/different_libs.h"
#include "../inc/vector3D.h"

#define EPSV 1e-6


Vector3D::Vector3D():p1(), p2()
{

}

Vector3D::Vector3D(Point3D const& p_1, Point3D const& p_2) : p1(p_1), p2(p_2)
{

}

Vector3D Vector3D::random()
{
	return Vector3D(Point3D::random(), Point3D::random());
}


Vector3D::Vector3D(Vector3D const& vec):p1(vec.p1), p2(vec.p2)
{

}

Point3D Vector3D::intersectM(Vector3D const& vec) const
{
	Point3D inter = intersect(vec);
	if (!inter.isInfinite()) return inter;
	Vector3D vec1 = *this;
	Vector3D vec2 = vec;
	vec1.shiftZX();
	vec2.shiftZX();
	inter = vec1.intersect(vec2);
	inter.shiftZX();
	return inter;
}

void Vector3D::shiftZX()
{
	p1.shiftZX();
	p2.shiftZX();
}

Point3D Vector3D::intersectIterative(Vector3D const& vec) const
{
	double maxKoeff = 30;
	double maxIter = 1000000;
	double minDist = 1e6;
	Point3D res;
	srand(GetTickCount() % 10000);
	for (int i = 0; i < maxIter; ++i)
	{
		double koeff1 = dif_lib::randomDoubleSigned(maxKoeff);
		double koeff2 = dif_lib::randomDoubleSigned(maxKoeff);
		Point3D p3 = p1 + (p2 - p1) * koeff1;
		Point3D p4 = vec.p1 + (vec.p2 - vec.p1) * koeff2;
		double dist = p3.dist(p4);
		if (dist < minDist)
		{
			minDist = dist;
			res = (p3 + p4) * 0.5;
		}
	}
	return res;
}



Point3D Vector3D::intersect(Vector3D const& vec) const
{
	Point3D v = p2 - p1;
	Point3D u = vec.p2 - vec.p1;
	Point3D ba = p1 - vec.p1;

	double znam = (u.y * v.x - u.x * v.y);
	if (abs(znam) < EPSV || abs(v.x) < EPSV) return Point3D::infinity();
	double k[3];
	k[1] = (v.z * u.x - v.x * u.z) / znam;
	k[0] = -(v.z + v.y * k[1]) / v.x;
	k[2] = 1;
	Matrix matr(3, 3);
	for (size_t i = 0; i < 3; ++i)
	{
		matr.at(0, i) = k[i];
	}
	u.export3D(matr.get_mem() + 3);
	v *= -1;
	v.export3D(matr.get_mem() + 6);
	matr = matr.transpose();
	/*
	k1 u1 -v1
	k2 u2 -v2
	k3 u3 -v3	
	*/
	Matrix b(1, 3);
	ba.export3D(b.get_mem());
	b = b.transpose();
	Matrix x = matr.solve(b);
	if (x.get_width() == 0) return Point3D::infinity();
	Point3D res;
	res.z = x.at(0, 0);
	res.y = res.z * k[1];
	res.x = res.z * k[0]; // res - minimal vector between vec1 and vec2.
	res *= 0.5;
	Point3D inter = vec.p1 + u * x.at(1, 0);
	inter += res;
	return inter;
}


void Vector3D::read()
{
	p1.read();
	p2.read();
}

void Vector3D::print() const
{
	std::cout << "( ";
	p1.print();
	std::cout << "; ";
	p2.print();
	std::cout << ")";
}
