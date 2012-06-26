#pragma once

#include "ImageProc/inc/Picture.h"
#include <list>

class  StablePicDetector
{
	Picture base, smallBase;
	std::list<Picture> latest;
	int memSize, downscale, eps;

	void updateLatest(Picture pic);
	void createSmallBase();

public:
	StablePicDetector();
	StablePicDetector(int memSize, int downscale, int eps);
	void nextStep(Picture cur_pic, Picture downscaled = Picture());
	Picture getBase() const;
	Picture getSmallBase() const;
};