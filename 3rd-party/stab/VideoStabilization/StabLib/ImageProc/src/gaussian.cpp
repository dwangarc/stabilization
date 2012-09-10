#include "stdafx.h"
#include "../inc/gausian.h"
#include <omp.h>

double Gaussian::sqr(double val) const
{
	return val * val;
}


double Gaussian::computeGaussian(double x, double y, double sigma_) const
{
	double res = 1 / (2 * M_PI * sqr(sigma_));
	res *= pow(M_E, -(sqr(x) + sqr(y)) / (2 * sqr(sigma_)) );
	return res;
}

double Gaussian::getGaussian(double x, double y) const
{
	return weights[maxDist + (int)x][maxDist + (int)y];
}

Gaussian::Gaussian(int width, int height, double sigma, int dist)
{
	this->width = width;
	this->height = height;
	this->sigma = sigma;
	this->dist = dist;
	for (int i = 0; i < maxDist * 2 + 1; ++i)
	{
		for (int j = 0; j < maxDist * 2 + 1; ++j)
		{
			weights[i][j] = computeGaussian(i - maxDist, j - maxDist, sigma);
		}
	}
}

int Gaussian::getPos(int w, int h) const
{
	return h * width + w;
}

Picture Gaussian::apply(Picture pic) const
{
	int width = pic.getWidth();
	int height = pic.getHeight();
	int color = pic.getColors();
	Picture res(width, height, color);
#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				for (int c = 0; c < color; ++c)
				{
					double val = 0;
					for (int g = -dist; g < dist + 1; ++g )
					{
						if (i + g >= height || i + g < 0) continue;
						for (int k = -dist; k < dist + 1; ++k)
						{
							if (j + k >= width || j + k < 0) continue;
							double gs = getGaussian(g, k);
							val += gs * (double)pic.get(j + k, i + g, c);
						}
					}
					res.at(j, i, c) = (unsigned char)val;
				}
			}
		}
	}
	return res;
}

GaussData Gaussian::preProcData(unsigned char const* pic) const
{
	GaussData res(width, height, dist);
	for (int g = 0; g < dist + 1; ++g)
	{
		for (int h = 0; h < g + 1; ++h)
		{
			res.at(0, 0, g, h) = getGaussian(g, h) * pic[getPos(0, 0)];
		}
	}
	for (int j = 1; j < height; ++j)
	{
		for (int g = 0; g < dist + 1; ++g)
		{
			for (int h = 0; h < g + 1; ++h)
			{
				res.at(0, j, g, h) = getGaussian(g, h) * pic[getPos(0, j)] + res.at(0, j - 1, g, h);
			}
		}
	}
	for (int i = 1; i < width; ++i)
	{
		for (int g = 0; g < dist + 1; ++g)
		{
			for (int h = 0; h < g + 1; ++h)
			{
				res.at(i, 0, g, h) = getGaussian(g, h) * pic[getPos(i, 0)] + res.at(i - 1, 0, g, h);
			}
		}
	}

	for (int i = 1; i < width; ++i)
	{
		for (int j = 1; j < height; ++j)
		{
			for (int g = 0; g < dist + 1; ++g)
			{
				for (int h = 0; h < g + 1; ++h)
				{
					res.at(i, j, g, h) = getGaussian(g, h) * pic[getPos(i, j)] + res.at(i - 1, j, g, h) + res.at(i, j - 1, g, h);
				}
			}
		}
	}
	return res;
}


unsigned char* Gaussian::applyFast(unsigned char const* pic) const
{
	GaussData data = preProcData(pic);
	unsigned char* res = new unsigned char[width * height];
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			
		}
	}
	return res;
}

