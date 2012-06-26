#pragma once

#include "Matrix/matrix.h"
#include "Point.h"
#include "vector3D.h"

class Rotation
{
	static const int dim = 3;
	Matrix matrix;
public:
	Matrix const& getMatrix() const;
	Rotation();
	Rotation(double* rot);
	Rotation(Matrix matr);
	Rotation operator+(Rotation const& rot) const;
	Rotation const& operator+=(Rotation const& rot);
	Rotation const& operator+=(double const* vec);

	Point3D rotate(Point3D const& point) const;
	bool isEqual(Rotation const& rot) const;

	void setRot(double* rot);

	Rotation obratn() const;

	// поворот вокруг вектора vec на угол angle.
	Rotation(Point3D const& vec, double angle);

	void fixDet();
	// all elements in matrix are multiplied by val.
	void simpleMult(double val);
	void simpleAdd(Rotation const& rot);
};

