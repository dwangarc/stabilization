#include "stdafx.h"
#include "../inc/SystemState.h"

SystemState SystemState::operator+(double const* vec) const
{
	SystemState res = *this;
	res.speed += vec[12];
	res.pos += vec;
	return res;
}

void SystemState::simpleAdd(SystemState state)
{
	speed += state.speed;
	Matrix mt = state.pos.toVector();
	mt += pos.toVector();
	pos = Position(mt);
}


SystemState SystemState::operator*(double val) const
{
	SystemState res = *this;
	res.speed *= val;
	Matrix mt = res.pos.toVector();
	mt.mult(val);
	res.pos = Position(mt);
	return res;
}
