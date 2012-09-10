#include "stdafx.h"
#include "../inc/homoEstimator.h"
#include "Include/different_libs.h"
#include <math.h>
#include <float.h>
#include <omp.h>

#define threadNum 8

typedef boost::shared_ptr<Point2D> sh_ptr_p2d;

HomoEstimator::HomoEstimator()
{
	DefaultDerivative = 0;
}

Homography HomoEstimator::getHomographyDownscale(Picture const& pic1, Picture const& pic2) const
{
	int downscale = 2;
	double EPS = 0.05;
	Picture small1 = converter.downscale(pic1, downscale);
	Picture small2 = converter.downscale(pic2, downscale);
	Homography homo = getHomography(small1, small2, 1);
	homo.applyDownscale(1.0 / (double)downscale);
	int step = 0;
	Homography homoShift;
	do{
		homoShift = homoStep(pic1, pic2, homo);
		homo += homoShift * (step == 0 ? 10 : 1);
		++step;
	}while (homoShift.dist() > EPS && step < 10);
	return homo;
}


Homography HomoEstimator::getHomography(Picture const& pic1, Picture const& pic2, int stepSize) const
{
	Homography curHomo, homoShift;

	double EPS = 0.05;
	int step = 0;
	DWORD time = GetTickCount();
	do{
		homoShift = homoStep(pic1, pic2, curHomo, stepSize);
		curHomo += homoShift;/* * (step == 0 ? 10 : 1);*/
		++step;
//		homoShift.printLog();
	} while (homoShift.dist() > EPS && step < MAX_STEPS);
	for (int i = 0; i < stepSize; ++i)
	{
		curHomo += homoStep(pic1, pic2, curHomo, 1);/* * (step == 0 ? 10 : 1);*/
	}
	Capture_Log::getInstance()->log_write("Base step time %d\n", GetTickCount() - time);
//	Capture_Log::getInstance()->log_write("Step time is %d\n", GetTickCount() - time);
//	curHomo.printLog();
	return /*step < MAX_STEPS ? curHomo : Homography()*/ curHomo;
}

Homography HomoEstimator::homoStep(Picture const& pic1, Picture const& pic2, Homography const& curHomo, int step) const
{
	Matrix M(6, 6);
	Matrix B(6, 1);

	int height = pic1.getHeight();
	int width  = pic2.getWidth();

	PictureDerivative derEst(&pic2);
	DWORD tm = GetTickCount();

	for (int i = 0; i < height; i += step)
	{
		for (int j = 0; j < width; j += step)
		{
			Point2D xy(i, j);
			Point2D xy1(curHomo.applyHomo(xy));
			if (xy1.outOfRange(1, 1, width - 2, height - 2)) continue;

			double dist = countDist(pic1, pic2, xy, xy1);
/*			double derx1 = derEst.derivativeX(xy1, 1);
			double dery1 = derEst.derivativeY(xy1, 1);*/
			double derx = derEst.derivativeXTwoDim(xy1);
			double dery = derEst.derivativeYTwoDim(xy1);
/*			Capture_Log::getInstance()->log_write("Der: %.2f %.2f %.2f %.2f\n", derx1, dery1, derx, dery);
			if (abs(derx) > 100) derx = derx1;
			if (abs(dery) > 100) dery = dery1;
*/
			if (dist > 1e50 || derx > 1e50 || dery > 1e50) continue;
			for (int k = 0; k < 6; ++k)
			{
				double weight = k < 3 ? derx : dery;
				if (k % 3 == 0) weight *= i;
				if (k % 3 == 1) weight *= j;

				B.at(k, 0) += weight * dist;

				M.at(k, 0) += weight * derx * xy.x;
				M.at(k, 1) += weight * derx * xy.y;
				M.at(k, 2) += weight * derx;

				M.at(k, 3) += weight * dery * xy.x;
				M.at(k, 4) += weight * dery * xy.y;
				M.at(k, 5) += weight * dery;
			}
		}
	}
//	Capture_Log::getInstance()->log_write("For time is %d\n", GetTickCount() - tm);

	tm = GetTickCount();
	Matrix Minv = M.obratn();
//	Capture_Log::getInstance()->log_write("Matrix time is %d\n", GetTickCount() - tm);
	Minv = Minv.mult(B);
	return Homography(Minv);
}

double HomoEstimator::countDist(Picture const& pic1, Picture const& pic2, Point2D xy, Point2D xy1) const
{
	int width = pic1.getWidth();
	int height = pic1.getHeight();
	if (xy1.outOfRange(0, 0, width - 1, height - 1)) return DBL_MAX;
	if (xy.outOfRange(0, 0, width - 1, height - 1)) return DBL_MAX;
	if (pic1.get(xy) < 1 || pic2.get(xy1) < 1) return 0;
	double dist = ((double)pic1.get(xy)) - (double)pic2.get(xy1);
	return dist;
}

