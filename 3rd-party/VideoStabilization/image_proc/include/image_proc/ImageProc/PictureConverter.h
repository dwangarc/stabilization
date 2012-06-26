#pragma once
#include "Homography/inc/homography.h"
#include "ImageProc/inc/Picture.h"
#include "ImageProc/inc/stereo_converter.h"

class PictureConverter
{
	int getPos(int w, int h) const;
	
	sh_ptr_int divide4;
	bool init;
	int *multWC, *multC;

	void initDivide();


	Picture downscaleSimple(Picture  pic, int downscale);
	Picture downscale2(Picture  pic);
	Picture downscale2Fast(Picture  pic);

public:
	PictureConverter();
	~PictureConverter();
	Picture convertGrey(Picture  pic, int dim = 1) const;
	Picture convertColor(Picture  pic) const;
	Picture downscale(Picture  pic, int downscale);
	Picture stretch(Picture  pic, int stretch) const;
	Picture stretchNearest2(Picture  pic) const;
	Picture normallize(Picture pic, int mediumInt) const;
	Picture toBGR(Picture pic) const;

	/************************************************************************/
	/* for each point on the result picture homo is applied and the result point 
	color is taken from the src pic											*/
	/************************************************************************/
	Picture applyHomo(Picture  pic, Homography  homo) const;

	Picture unFixDistortions(StereoConverter const& converter, Picture pic);
	Picture fixDistortions(StereoConverter const& converter, Picture pic);
	sh_prt_uch createBitMapData(Picture pic, int alignIm = 4) const;
	//	bitmap* createBitMap(Picture pic) const;

	Picture fromSmallPic(Picture smallPic, Picture realPic, int downscale) const;
};