#pragma once
#include "Homography/inc/homography.h"
#include "Picture.h"
#include "stereo_converter.h"

class PictureConverter
{
	int getPos(int w, int h) const;
public:
	Picture convertGrey(Picture  pic, int dim = 1) const;
	Picture convertColor(Picture  pic) const;
	Picture downscale(Picture  pic, int downscale) const;
	Picture stretch(Picture  pic, int stretch) const;

	/************************************************************************/
	/* for each point on the result picture homo is applied and the result point 
	color is taken from the src pic											*/
	/************************************************************************/
	Picture applyHomo(Picture  pic, Homography  homo) const;

	Picture unFixDistortions(StereoConverter& converter, Picture pic);
	Picture fixDistortions(StereoConverter& converter, Picture pic);
};