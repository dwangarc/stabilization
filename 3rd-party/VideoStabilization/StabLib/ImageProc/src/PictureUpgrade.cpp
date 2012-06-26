#include "stdafx.h"
#include "../inc/PictureUpgrade.h"

sh_ptr_int PictureUpgrader::gistogramEstimator(Picture pic, int bins) const
{
	if (bins < 1) bins = 1;
	sh_ptr_int res(new int[bins]);
	for (int i = 0; i < bins; ++i)
	{
		res.get()[i] = 0;
	}
	for (int i = 0; i < pic.getWidth(); ++i)
	{
		for (int j = 0; j < pic.getHeight(); ++j)
		{
			double intensity = 0;
			for (int c = 0; c < pic.getColors(); ++c)
			{
				intensity += pic.get(i, j, c);
			}
			intensity *= bins;
			intensity /= 256 * pic.getColors();
			res.get()[(int)intensity]++;
		}
	}
	return res;
}

Picture PictureUpgrader::gistogramAlign(Picture pic) const
{
	int tableLength = 255 * pic.getColors() + 1;
	sh_ptr_int gist = gistogramEstimator(pic, tableLength);
	sh_ptr_int convTable(new int[tableLength]);

	int place = 0;
	double q = pic.getWidth() * pic.getHeight() / (double)tableLength;
	int sum = 0;
	for (int i = 0; i < tableLength; ++i)
	{
		while (place < tableLength && sum < q * i)
		{
			convTable.get()[place] = i;
			sum += gist.get()[place++];
		}
	}
	for (int i = place; i < tableLength; ++i)
	{
		convTable.get()[i] = 255 * pic.getColors();
	}

	Picture res(pic.getWidth(), pic.getHeight(), pic.getColors());
	for (int i = 0; i < pic.getWidth(); ++i)
	{
		for (int j = 0; j < pic.getHeight(); ++j)
		{
			double intensity = 0;
			for (int c = 0; c < pic.getColors(); ++c)
			{
				intensity += pic.get(i, j, c);
			}
			double newIntensity = convTable.get()[(int)intensity];
			double koeff = newIntensity / intensity;
			for (int c = 0; c < pic.getColors(); ++c)
			{
				int intense = (int)(koeff * pic.get(i, j, c));
				if (intense > 255) intense = 255;
				if (intense < 0) intense = 0;
				res.at(i, j, c) = (unsigned char)(intense);
			}
		}
	}

	return res;
}
