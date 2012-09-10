#pragma once
#include "Matrix/matrix.h"

struct EKFfunc
{
	Matrix transition(Matrix x, Matrix u) const;
	Matrix observation(Matrix x) const;
};

class EKF
{
	EKFfunc func;
public:
	EKF(EKFfunc* func);


};