#pragma once
#include "Matrix/matrix.h"

/*
x = F*xk1 + B*u + w;
w ~ N(0, Q)
z = H*x + v;
v ~ N(0, R);
H - матрица измерений
F - матрица эволюции процесса
B - матрица управления
*/

class KalmansFilter
{
	Matrix x, P, F, B, H, R, Q;
	bool isInit;
public:
	KalmansFilter();
	Matrix observe(Matrix obs, Matrix u);
	Matrix observeStatic(Matrix obs);
	void setInitState(Matrix P, Matrix x);
	void setParams(Matrix F, Matrix B, Matrix H, Matrix R, Matrix Q);
	void setPointParams(double r, double q);
	Matrix const& getState() const;
	bool isInited() const;
};

/*
class SimpleKalmanFilter
{
	Matrix x, P, F, B, H, R, Q;
public:
	Matrix observe(Matrix obs, Matrix u);
	void setInitState(Matrix P, Matrix x);
	void setParams(Matrix F, Matrix B, Matrix H, Matrix R, Matrix Q);
	Matrix const& getState() const;
};

*/