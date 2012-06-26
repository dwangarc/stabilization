#pragma once

#include "Picture.h"
#include "Matrix/matrix.h"

class PictureDerivative
{

	double countDist(Point2D const& xy, Point2D const& xy1) const;
	double derivativeBase(Point2D const& xy, Point2D const& xy1) const;
	double matrixSum(Matrix const& matrix, Point2D const& p) const;
	Matrix derivativeMatrix3x3() const;

	Picture pic;
	double DefaultDerivative;

	Matrix derY3x3, derX3x3;
	double matrDerNorm;
	int width, height;

public:
	PictureDerivative(Picture pic);

	double derivativeX(Point2D xy, int dist = 1) const;
	double derivativeY(Point2D xy, int dist = 1) const;

	double derivativeXTwoDim(Point2D const& xy) const;
	double derivativeYTwoDim(Point2D const& xy) const;

	double derivativeXTwoDimSimple(Point2D xy) const;
	double derivativeYTwoDimSimple(Point2D xy) const;

	Picture gradient() const;

};