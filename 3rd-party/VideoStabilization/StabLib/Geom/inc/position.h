#pragma once

#include "rotation.h"

struct Position
{
	Rotation rot;
	Point3D pos;
	Position():rot(),  pos()
	{
	}

	Position(Matrix const& matr);
	Position const& operator +=(Position const& pos);
	Position const& operator +=(double const* vec);
	Position operator -(Position const& pos) const;
	Position(Point3D const& p, Rotation rt): pos(p), rot(rt)
	{
	}

	Point3D apply(Point3D const& p1) const;
	Vector3D apply(Vector3D const& v1) const;

	Matrix toVector() const;

	void print() const;

	void simpleMult(double val);
	void simpleAdd(Position const& ps);

	bool isEqual(Position const& ps) const;
};