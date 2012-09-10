#include "stdafx.h"
#include <float.h>

#include "../inc/PicDerivative.h"

PictureDerivative::PictureDerivative(Picture const* pic)
{
	this->pic = pic;
	DefaultDerivative = 0;
	derX3x3 = derivativeMatrix3x3();
	derY3x3 = derX3x3.transpose();
	matrDerNorm = derX3x3.elemSumAbs();
	width = pic->getWidth();
	height= pic->getHeight();
}


double PictureDerivative::derivativeBase(Point2D const& xy, Point2D const& xy1) const
{
	int width = pic->getWidth();
	int height = pic->getHeight();
	if (xy1.outOfRange(0, 0, width - 1, height - 1))
	{
		return countDist(xy, xy * 2 - xy1);
	}
	return countDist(xy1, xy);
}


double PictureDerivative::derivativeX(Point2D xy, int dist) const
{
	return derivativeBase(xy, Point2D(xy.x + dist, xy.y)) / dist;
}

double PictureDerivative::derivativeY(Point2D xy, int dist) const
{
	return derivativeBase(xy, Point2D(xy.x, xy.y + dist)) / dist;
}

double PictureDerivative::matrixSum(Matrix const& matrix, Point2D const& p) const
{
/*	int mH = matrix.get_height();
	int mW = matrix.get_width();
	if (mH != mW || mW % 2 != 1 || mH % 2 != 1) return DefaultDerivative;
	if (p.outOfRange(mW / 2, mH / 2, width - mW / 2 - 1, height - mH / 2 - 1)) return DefaultDerivative;*/

	double sum = 0;
	for (int i = 0; i < 3 /*mW*/; ++i)
	{
		for (int j = 0; j < 3/*mH*/; ++j)
		{
			sum += matrix.get_elem(j, i) * pic->get((int)p.x + i - 1, (int)p.y + j - 1);
		}
	}
	return sum;
}

Matrix PictureDerivative::derivativeMatrix3x3() const
{
	Matrix matr(3, 3);
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			matr.at(i, j) = j - 1;
		}
	}
	matr.at(1, 0) -= 1;
	matr.at(1, 2) += 1;
	return matr;
}

double PictureDerivative::derivativeXTwoDim(Point2D const& xy) const
{
	return matrixSum(derX3x3, xy) / matrDerNorm;
}

double PictureDerivative::derivativeYTwoDim(Point2D const& xy) const
{
	return matrixSum(derY3x3, xy) / matrDerNorm;
}


double PictureDerivative::countDist(Point2D const& xy, Point2D const& xy1) const
{
	int width = pic->getWidth();
	int height = pic->getHeight();
	if (xy1.outOfRange(0, 0, width - 1, height - 1)) return DBL_MAX;
	if (xy.outOfRange(0, 0, width - 1, height - 1)) return DBL_MAX;
	if (pic->get(xy) < 1 || pic->get(xy1) < 1) return 0;
	double dist = ((double)pic->get(xy)) - (double)pic->get(xy1);
	return dist;
}
/*
double PictureDerivative::derivativeXTwoDimSimple(Point2D xy) const
{

}

double PictureDerivative::derivativeYTwoDimSimple(Point2D xy) const
{

}*/

