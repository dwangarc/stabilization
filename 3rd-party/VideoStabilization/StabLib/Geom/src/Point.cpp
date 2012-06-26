#include "stdafx.h"
#include "../inc/Point.h"
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <limits>
#include "Include/different_libs.h"

//#undef max

Point3D::Point3D(double x_, double y_, double z_):x(x_), y(y_), z(z_)
{

}

Point3D::Point3D(Point3D const& point): x(point.x), y(point.y), z(point.z)
{

}


Point3D Point3D::operator+(Point3D const&p) const
{
	return Point3D(p.x + x, p.y + y, p.z + z);
}

Point3D Point3D::operator-(Point3D const&p) const
{
	return Point3D(x - p.x, y - p.y, z - p.z);
}


Point3D const& Point3D::operator+=(Point3D const&p)
{
	x += p.x;
	y += p.y;
	z += p.z;
	return *this;
}

Point3D const& Point3D::operator-=(Point3D const&p)
{
	x -= p.x;
	y -= p.y;
	z -= p.z;
	return *this;
}

Point3D const& Point3D::operator *=(double val)
{
	x *= val;
	y *= val;
	z *= val;
	return *this;
}

Point3D Point3D::operator *(double val) const
{
	Point3D p1 = *this;
	p1 *= val;
	return p1;
}



Point3D const& Point3D::operator=(Point3D const&p)
{
	x = p.x;
	y = p.y;
	z = p.z;
	return *this;
}

void Point3D::set3D(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Point3D::set3D(Point3D const& p1)
{
	this->x = p1.x;
	this->y = p1.y;
	this->z = p1.z;
}

Point3D const& Point3D::normalize(double length)
{
	double dst = dist(Point3D(0));
	if (dst > 1e-6)
	{
		x /= dst;
		y /= dst;
		z /= dst;
	}
	return (*this) *= length;
}


double Point3D::dist(Point3D const& p1) const
{
	return sqrt(sqr(p1.x - x) + sqr(p1.y - y) + sqr(p1.z - z));
}

bool Point3D::isEqual(Point3D const& p1) const
{
	return dist(p1) < 1e-6 ? true : false;
}



void Point3D::export3D(double* data) const
{
	data[0] = x;
	data[1] = y;
	data[2] = z;
}

void Point3D::import3D(double const* data)
{
	x = data[0];
	y = data[1];
	z = data[2];
}

void Point3D::read()
{
	std::cin >> x >> y >> z;
}

void Point3D::print() const
{
	printf("(%.2f; %.2f; %.2f)", x, y, z);
}

Point3D Point3D::random()
{
	double sz = 10.0;
	return Point3D(dif_lib::randomDoubleSigned(sz), dif_lib::randomDoubleSigned(sz), dif_lib::randomDoubleSigned(sz));
}


Point3D Point3D::infinity()
{
	double inf = 1e100;//std::numeric_limits<double>::max();
	inf /= 10;
	return Point3D(inf, inf, inf);
}

bool Point3D::isInfinite() const
{
	double inf = 1e100 / 11;//std::numeric_limits<double>::max() / 11;
	if (x > inf || y > inf || z > inf) return true;
	return false;
}

void Point3D::shiftZX()
{
	double tmp = z;
	z = x;
	x = tmp;
}

Matrix Point3D::toMatrix() const
{
	Matrix res(3, 1);
	res.at(0, 0) = x;
	res.at(1, 0) = y;
	res.at(2, 0) = z;
	return res;
}

Point3D::Point3D(Matrix const& matr)
{
	if (matr.get_height() < 3) throw PointException("To little matrix size");
	x = matr.get_elem(0, 0);
	y = matr.get_elem(1, 0);
	z = matr.get_elem(2, 0);
}


