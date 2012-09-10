#include "stdafx.h"
#include "KalmanProjective.h"

KalmanProjective::KalmanProjective(double dispAngle, double dispShift, double dispAngleMeas, double dispShiftMeas):
kalmHomo(dispAngle, dispShift, dispAngleMeas, dispShiftMeas, 10, 1)
{
	Matrix x(2, 1);
	Matrix P(2, 2);
	Matrix F = Matrix::identity(2);
	Matrix B(2, 2);
	Matrix H = Matrix::identity(2);
	Matrix R = Matrix::identity(2).mult(dispAngleMeas);
	Matrix Q = Matrix::identity(2).mult(dispAngle);
	filter.setInitState(P, x);
	filter.setParams(F, B, H, R, Q);
}

double* KalmanProjective::observe(double* proj)
{
	double* data = new double[6];
	for (int i = 0; i < 6; ++i)
	{
		data[i] = proj[i];
	}
	Matrix hMatr(6, 1, data);
	Homography homo1(hMatr);
	Homography homoRes = kalmHomo.observe(homo1);

	Matrix matr(2, 1);
	Matrix empt(2, 1);
	matr.at(0, 0) = proj[6];
	matr.at(1, 0) = proj[7];
	Matrix par = filter.observe(matr, empt);

	double* res = new double[9];
	for (int i = 0; i < 6; ++i)
	{
		res[i] = homoRes.getHomo().get()[i];
	}
	res[6] = par.get_elem(0, 0);
	res[7] = par.get_elem(1, 0);
	res[8] = proj[8];
	return res;
}
