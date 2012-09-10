#pragma once
#include "opencv/highgui.h"
#include "Picture.h"

class IplImageWrapper
{
	IplImage* image;
public:
	IplImageWrapper(Picture pic);
	IplImage const* getImage() const;
	~IplImageWrapper();
};