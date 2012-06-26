#include "stdafx.h"
#include "../inc/StablePicDetector.h"
#include "ImageProc/inc/pictureConverter.h"
#include "ImageProc/inc/gausian.h"
#include <omp.h>

StablePicDetector::StablePicDetector() // Some default values.
{
	this->downscale = 1;
	this->memSize = 5;
	this->eps = 5;
}

StablePicDetector::StablePicDetector(int memSize, int downscale, int eps):base(),smallBase()
{
	this->downscale = downscale;
	this->memSize = memSize;
	this->eps = eps;
}

void StablePicDetector::updateLatest(Picture pic)
{
	latest.push_back(pic);
	if (latest.size() > (unsigned int)memSize)
	{
		latest.pop_front();
	}
}


void StablePicDetector::nextStep(Picture cur_pic, Picture downscaled)
{
	if (base.getWidth() == 0)
	{
		base = cur_pic.clone();
	}
	if (downscaled.getWidth() == 0)
	{
		PictureConverter conv;
		downscaled = conv.downscale(cur_pic, downscale);
	}

	updateLatest(downscaled);	

#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < downscaled.getWidth(); ++i)
		{
			for (int j = 0; j < downscaled.getHeight(); ++j)
			{
				bool flag = true;
				for (std::list<Picture>::const_iterator iter = latest.begin(); (iter != latest.end()) && flag; ++iter)
				{
					for (int c = 0; c < base.getColors(); ++c)
					{
						int col = latest.rbegin()->get(i, j, c);
						int cCheck = iter->get(i, j, c);
						if (abs(col - cCheck) > eps)
						{
							flag = false;
							break;
						}						
					}
				}
				if (flag)
				{
					int width = i * downscale;
					int height = j * downscale;
					for (int c = 0; c < base.getColors(); ++c)
					{
						for (int k = 0; k < downscale; ++k)
						{
							for (int m = 0; m < downscale; ++m)
							{
								base.at(width + k, height + m, c) = cur_pic.at(width + k, height + m, c);
							}
						}
					}
				}
			}
		}
	}

	createSmallBase();
}

void StablePicDetector::createSmallBase()
{
	PictureConverter conv;
	smallBase = conv.downscale(base, downscale);
	Gaussian gs(smallBase.getWidth(), smallBase.getHeight(), 2.0,  3);
	smallBase = gs.apply(smallBase);
}


Picture StablePicDetector::getBase() const
{
	return base;
}

Picture StablePicDetector::getSmallBase() const
{
	return smallBase;
}

