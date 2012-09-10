#pragma once

#include "Matrix/matrix.h"
#include "Geom/inc/Point.h"
#include "homography.h"
#include "ImageProc/inc/Picture.h"
#include "ImageProc/inc/PicDerivative.h"
#include "ImageProc/inc/pictureConverter.h"

class HomoEstimator
{
	static const int MAX_STEPS = 50;
	PictureConverter converter;

	double DefaultDerivative;

	double countDist(Picture const& pic1, Picture const& pic2, Point2D xy, Point2D xy1) const;
	int getPos(int width, int height) const;
	int getPos(Point2Di xy) const;
	int getPos(double width, double height) const;
public:
	HomoEstimator();
	Homography getHomography(Picture const& pic1, Picture const& pic2, int stepSize) const;
	Homography getHomographyDownscale(Picture const& pic1, Picture const& pic2) const;
	Homography homoStep(Picture const& pic1, Picture const& pic2, Homography const& curHomo, int step = 4) const;
};