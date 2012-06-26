#include "stdafx.h"

#include <math.h>

#include "tester.h"
#include "Include/different_libs.h"

double Tester::imgDistace(Picture pic1, Picture pic2) const
{
	if (pic1.getWidth() == 0 || pic2.getWidth() == 0) return -1;
	double dist = 0;
	int count = 0;
	int width = pic1.getWidth();
	int height = pic1.getHeight();
	int colors = pic1.getColors();
	if (width != pic2.getWidth() || height != pic2.getHeight() || colors != pic2.getColors()) return -1;
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			for (int c = 0; c < colors; ++c)
			{
				unsigned char i1 = pic1.get(i, j, c);
				unsigned char i2 = pic2.get(i, j, c);
				if (i1 > 0 && i2 > 0)
				{
					dist += sqr((int)i1 - (int)i2);
					++count;
				}
			}
		}
	}
	dist /= count;
	dist = sqrt(dist);
	return dist;
}
