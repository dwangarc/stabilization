#pragma once
#include "ImageFeatures/inc/kdTreePoints.h"
#include "Painter/inc/painter.h"

interface KdPointsGenI
{
	virtual void setParams(const double* H_, const double* ic_, bool downscale, size_t width, size_t height, double base = 0.1) = 0;
	virtual KdPointsContainer getKdPoints(unsigned char* pic1, unsigned char* pic2) const = 0;
	virtual void setPainters(Painter* painter_l, Painter* painter_r) = 0;
	virtual ~KdPointsGenI(){}
};