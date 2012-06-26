#pragma once


#include "Include/define.h"
#include "ImageProc/inc/Picture.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>

struct GaussData
{
	int width, height, rows;
	sh_ptr_db data;

	GaussData(int width, int height, int dist)
	{
		rows = (dist + 1) * (dist + 2) / 2;
		this->width = width;
		this->height = height;
		data = sh_ptr_db(new double[rows * width * height]);
	}

	int getRow(int dx, int dy)
	{
		int adx = std::max(abs(dx), abs(dy));
		int ady = std::min(abs(dx), abs(dy));
		return ady + adx * (adx + 1) / 2;
	}

	double& at(int x, int y, int dx, int dy)
	{
		int row = getRow(dx, dy);
		return data.get()[row * width * height + y * width + x];
	}
};

class Gaussian
{
	int width, height;
	double sigma;
	int dist;
	static const int maxDist = 10;

	double computeGaussian(double x, double y, double sigma) const;
	double getGaussian(double x, double y) const;

	double sqr(double val) const;

	int getPos(int width, int height) const;

	double weights[2 * maxDist + 1][2 * maxDist + 1];

	GaussData preProcData(unsigned char const* pic) const;

public:
	Gaussian(int width, int height, double sigma, int dist);


	unsigned char* applyFast(unsigned char const* pic) const;
	Picture apply(Picture pic) const;
};