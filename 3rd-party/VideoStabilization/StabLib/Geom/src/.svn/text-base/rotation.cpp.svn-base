#include "stdafx.h"
#include <math.h>

#include "../inc/rotation.h"
//#include "Include/capture_log.h"
#include "Include/different_libs.h"


Rotation::Rotation(double * rot1):matrix(dim, dim, rot1)
{
}

void Rotation::setRot(double* rot)
{
	matrix = Matrix(dim, dim, rot);
}


Matrix const& Rotation::getMatrix() const
{
	return matrix;
}

Rotation::Rotation():matrix(dim, dim)
{
	for (int i = 0; i < dim; ++i)
	{
		matrix.set_elem(i, i, 1);
	}
}

Point3D Rotation::rotate(Point3D const& point) const
{
	double* data = new double[dim];
	data[0] = point.x;
	data[1] = point.y;
	data[2] = point.z;
	Matrix matr(dim, 1, data);
	Matrix mult = matrix.mult(matr);
	return Point3D (mult.get_elem(0, 0), mult.get_elem(1, 0), mult.get_elem(2, 0));
}

Rotation::Rotation(Point3D const& vec, double angle) : matrix(dim, dim)
{
	matrix.set_elem(0, 0, cos(angle) + (1 - cos(angle)) * sqr(vec.x));
	matrix.set_elem(1, 1, cos(angle) + (1 - cos(angle)) * sqr(vec.y));
	matrix.set_elem(2, 2, cos(angle) + (1 - cos(angle)) * sqr(vec.z));

	matrix.set_elem(0, 1, (1 - cos(angle)) * vec.x * vec.y - sin(angle) * vec.z);
	matrix.set_elem(0, 2, (1 - cos(angle)) * vec.x * vec.z - sin(angle) * vec.y);
	matrix.set_elem(1, 2, (1 - cos(angle)) * vec.z * vec.y - sin(angle) * vec.x);

	matrix.set_elem(1, 0, matrix.at(0, 1));
	matrix.set_elem(2, 0, matrix.at(0, 2));
	matrix.set_elem(2, 1, matrix.at(1, 2));
}


Rotation::Rotation(Matrix matr): matrix(matr)
{

}


Rotation Rotation::operator+(Rotation const& rot) const
{
	Matrix mult = rot.matrix.mult(matrix);
	return Rotation(mult);
}

Rotation const& Rotation::operator+=(Rotation const& rot)
{
	matrix = rot.matrix.mult(matrix);
	return *this;
}

Rotation const& Rotation::operator+=(double const* vec)
{
	double* data = new double[dim * dim];
	for (size_t i = 0; i < dim * dim; ++i)
	{
		data[i] = vec[i];
	}
	Matrix mt(dim, dim, data);
	matrix += mt;
	matrix.fixDet();
	return *this;
}

void Rotation::simpleMult(double val)
{
	matrix.mult(val);
}

void Rotation::fixDet()
{
	matrix.fixDet();
}

Rotation Rotation::obratn() const
{
	Rotation res(matrix.obratn());
	return res;
}

bool Rotation::isEqual(Rotation const& rot) const
{
	return matrix.isEqual(rot.matrix);
}

void Rotation::simpleAdd(Rotation const& rot)
{
	matrix += rot.getMatrix();
}

