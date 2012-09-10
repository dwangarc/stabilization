#pragma once
#include "Geom/inc/position.h"

struct SystemState
{
	static const size_t dim = 13;
	Position pos;
	double speed;

	SystemState():pos(), speed(0)
	{

	}

	SystemState operator+(double const* vec) const;
	void simpleAdd(SystemState state);
	SystemState operator*(double val) const;
};
