#pragma once

#include "Filters/inc/KalmanFilter.h"
#include "homography.h"

class KalmanHomography
{
	KalmansFilter filter;
public:
	KalmanHomography(double dispAngle, double dispShift,double dispAngleMeas, double dispShiftMeas, double koeff, double f1);
	Homography observe(Homography const& curHomo);
};