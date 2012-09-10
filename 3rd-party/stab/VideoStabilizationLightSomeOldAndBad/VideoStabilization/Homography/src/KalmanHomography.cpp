#include "stdafx.h"
#include "../inc/KalmanHomography.h"

KalmanHomography::KalmanHomography(double dispAngle, double dispShift, double dispAngleMeas, double dispShiftMeas, double koeff, double f1)
{
	unsigned int dim = Homography::getHomoDim();
	Matrix H = Matrix::identity(dim);
	Matrix B(dim, dim);
	Matrix F = Matrix::identity(dim);
	F.mult(f1);
	Matrix R(dim, dim);
	Matrix Q(dim, dim);
	Matrix P(dim, dim);
	for (unsigned int i = 0; i < dim; ++i)
	{
		if (i % 3 == 2)
		{
			R.at(i, i) = dispShiftMeas;
			Q.at(i, i) = dispShift;
			P.at(i, i) = dispShiftMeas / koeff;
		}
		else
		{
			R.at(i, i) = dispAngleMeas;
			Q.at(i, i) = dispAngle;
			P.at(i, i) = dispAngleMeas / koeff;
		}
	}
	Matrix x(dim, 1);
/*	x.at(2, 0) -= 1;
	x.at(5, 0) -= 1;*/
	filter.setInitState(P, x);
	filter.setParams(F, B, H, R, Q);
}

Homography KalmanHomography::observe(Homography const& curHomo)
{
	unsigned int dim = Homography::getHomoDim();
	Matrix homo(dim, 1, curHomo.getHomo());
	homo.at(0, 0) -= 1;
	homo.at(4, 0) -= 1;
	Matrix u(dim, 1);
	Matrix state = filter.observe(homo, u);
	state.at(0, 0) += 1;
	state.at(4, 0) += 1;
	return Homography(state);
}
