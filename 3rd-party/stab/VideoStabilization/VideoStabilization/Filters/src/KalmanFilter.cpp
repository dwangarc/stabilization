#include "stdafx.h"
#include "../inc/KalmanFilter.h"

Matrix KalmansFilter::observe(Matrix obs, Matrix u)
{
	if (!isInit) return Matrix();
	// Extrapolation
	Matrix xk1 = F * x + B * u;
	Matrix Pk1 = F * P * (F.transpose()) + Q;

	// Correction
	Matrix yk = obs - H * xk1;
	Matrix S1 = H * Pk1 * (H.transpose()) + R;
	Matrix K = Pk1 * H * (S1.obratn());
	
	
	x = xk1 + K * yk;
	P = (Pk1.identity() - K.mult(H)).mult(Pk1);

	return x;
}

Matrix KalmansFilter::observeStatic(Matrix obs)
{
	if (!isInit) return Matrix();
	// Extrapolation
	Matrix xk1 = x;
	Matrix Pk1 = P + Q;

	// Correction
	Matrix yk = obs - xk1;
	Matrix S1 = Pk1 + R;
	Matrix K = Pk1 * (S1.obratn());

	x = xk1 + K * yk;
	P = (Pk1.identity() - K).mult(Pk1);
	return x;
}


void KalmansFilter::setInitState(Matrix _P, Matrix _x)
{
	isInit = true;
	x = _x;
	P = _P;
}

void KalmansFilter::setParams(Matrix F, Matrix B, Matrix H, Matrix R, Matrix Q)
{
	this->F = F;
	this->B = B;
	this->H = H;
	this->R = R;
	this->Q = Q;
}

void KalmansFilter::setPointParams(double r, double	q)
{
	F = Matrix::identity(3);
	B = Matrix(3, 3);
	H = Matrix::identity(3);
	R = Matrix::identity(3).mult(r);
	Q = Matrix::identity(3).mult(q);
}


Matrix const& KalmansFilter::getState() const
{
	return x;
}

bool KalmansFilter::isInited() const
{
	return isInit;
}

KalmansFilter::KalmansFilter()
{
	isInit = false;
}



