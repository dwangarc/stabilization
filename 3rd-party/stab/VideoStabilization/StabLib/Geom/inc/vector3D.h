#pragma once
#include "Point.h"

struct Vector3D
{
	Point3D p1, p2;

	Vector3D();

	Vector3D(Point3D const& p1, Point3D const& p2);

	Vector3D(Vector3D const& vec);

	Point3D intersect(Vector3D const& vec) const;
	Point3D intersectM(Vector3D const& vec) const;
	Point3D intersectIterative(Vector3D const& vec) const;

	static Vector3D random();

	void read();
	void print() const;

private:
	void shiftZX();
};