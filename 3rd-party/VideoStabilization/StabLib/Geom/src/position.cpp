#include "stdafx.h"
#include "../inc/position.h"

//#include "Include/capture_log.h"
//#include "Include/different_libs.h"


void Position::simpleAdd(Position const& ps)
{
	pos += ps.pos;
	rot.simpleAdd(ps.rot);
}

bool Position::isEqual(Position const& ps) const
{
	return rot.isEqual(ps.rot) && pos.isEqual(ps.pos);
}

Position const& Position::operator+=(Position const& ps)
{
	pos += ps.pos;
	rot = Rotation(ps.rot.getMatrix().mult(rot.getMatrix()));
	return *this;
}

Position const& Position::operator +=(double const* vec)
{
	Point3D point(0);
	point.import3D(vec);
	pos += point;
	rot += (vec + 3);
	return *this;
}

void Position::simpleMult(double val)
{
	pos *= val;
	rot.simpleMult(val);
}


Point3D Position::apply(Point3D const& p1) const
{
	Point3D pt = rot.rotate(p1);
	pt += pos;
	return pt;
}

Vector3D Position::apply(Vector3D const& v1) const
{
	Vector3D vt(apply(v1.p1), apply(v1.p2));
	return vt;
}

Matrix Position::toVector() const
{
	Matrix matr(12, 1);
	matr.at(0, 0) = pos.x;
	matr.at(1, 0) = pos.y;
	matr.at(2, 0) = pos.y;
	for (size_t i = 0; i < 3; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			matr.at(3 + i * 3 + j, 0) = rot.getMatrix().get_elem(i, j);
		}
	}
	return matr;
}

Position::Position(Matrix const& matr)
{
	pos.x = matr.get_elem(0, 0);
	pos.y = matr.get_elem(1, 0);
	pos.z = matr.get_elem(2, 0);
	Matrix mt(3, 3);
	for (size_t i = 0; i < 3; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			mt.at(i, j) = matr.get_elem(3 + i * 3 + j, 0);
		}
	}
	rot = Rotation(mt);
}

Position Position::operator -(Position const& ps) const
{
	Position res;
	res.pos = pos - ps.pos;
	res.rot = rot + ps.rot.obratn();

	Position check = ps;
	check += res;
	if (!check.isEqual(*this))
	{
//		Capture_Log::getInstance()->log_write("Failed - position\n");
	}
	return res;
}
