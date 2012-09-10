#pragma once
#include "Homography/inc/KalmanHomography.h"

class KalmanProjective
{
	KalmanHomography kalmHomo;
	KalmansFilter filter;
	static const int dim = 8;
public:
	KalmanProjective(double dispAngle, double dispShift, double dispAngleMeas, double dispShiftMeas);
	double* observe(double* proj);
};