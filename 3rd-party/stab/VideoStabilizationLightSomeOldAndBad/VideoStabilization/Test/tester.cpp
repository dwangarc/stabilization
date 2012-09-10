#include "stdafx.h"

#include <math.h>

#include "tester.h"
#include "Include/different_libs.h"

void Tester::setWH(int width, int height)
{
	this->height = height;
	this->width = width;
}

double Tester::imgDistace(unsigned char const* pic1, unsigned char const* pic2, int colors) const
{
	if (!pic1 || !pic2) return 0;
	double dist = 0;
	int count = 0;
	for (int i = 0; i < width * height * colors; ++i)
	{
		if (pic1[i] > 1 && pic2[i] > 1)
		{
			int val1 = pic1[i];
			int val2 = pic2[i];
			dist += sqr(val1 - val2);
			++count;
		}
	}
	dist /= count;
	dist = sqrt(dist);
	return dist;
}
