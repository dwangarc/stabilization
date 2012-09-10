#include "stdafx.h"
#include "../inc/BaseVideo.h"
#include "ImageProc/inc/PictureConverter.h"

Picture BaseVideo::nextStep(Picture pic)
{
	PictureConverter pconv;
	return pconv.downscale(pic, 2);
}
