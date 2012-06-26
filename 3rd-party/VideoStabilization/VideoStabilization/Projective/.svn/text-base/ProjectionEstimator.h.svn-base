#pragma once

#include <boost/shared_ptr.hpp>

#include "projection.h"
#include "ImageProc/inc/Picture.h"
#include "Include/define.h"
#include "Geom/inc/Point.h"
#include "ProjectiveConverter.h"
#include "ImageProc/inc/PictureConverter.h"

class ProjectionEstimator
{
	PictureConverter converter;
	ProjectiveConverter projConv;
	static const int MAX_PYR_SIZE = 10;
	Picture im1[MAX_PYR_SIZE];
	Picture im2[MAX_PYR_SIZE];
	Projection proj_engine[MAX_PYR_SIZE];
	Picture stabilized, stabilizedSmall, stabUnion;

	Picture prevPic;
	
	bool initialized;
	int pyrSize;
	int gaussDepth;
	int defaultIterNum;

	Matrix prevMatr;
	Matrix medMatrix;

	double alpha, alpha1;

	int maxCornerDifference;
	int safeShift, shiftKoeff;

	void shiftIm();
	void createPyr(Picture pic1, Picture pic2);
	void createPyr(Picture pic, Picture* im, int pyramidSize);

//	void createStabPic(Picture pic);
	void createStabPicExp(Picture pic, sh_ptr_db projection);
	void createStabPicExpModify(Picture pic, sh_ptr_db projection);
	void createStabPicExpMult(Picture pic, sh_ptr_db projection);
	void createStabPicMedian(Picture pic, Matrix toStab, Matrix toPrev);

	int hugeProec(double const* proec, int width, int height) const;
	double convertX(double const* proec, double x, double y) const;
	double convertY(double const* proec, double x, double y) const;
	Point2D convertXY(double const* proec, Point2D p1) const;

	bool checkSmall(double const* proec, int width, int height) const;
	double estimateCornerDist(double const* proec, int width, int height) const;

	bool createPowerMt(Matrix& powerMt, Matrix& dopolnMt, sh_ptr_db proec, double alpha);
	void createSimpleStabPic(Picture pic);

public:
	ProjectionEstimator(int pyrSize, int iterNumber, double alpha, int gaussDepth = 2);
	void stabilize(Picture pic, sh_ptr_int pyrIter, int downscale = 1);
	
	void stabilizeStable(Picture pic, sh_ptr_int pyrIter);
	void stabilizeStableLight(Picture pic, sh_ptr_db projection);


	/************************************************************************/
	/* Estimates projection from pic2 to pic1								*/
	/************************************************************************/
	sh_ptr_db estimateProj(Picture pic1, Picture pic2, sh_ptr_int pyrIter);
	sh_ptr_db simpleProjection() const;
	Picture getStabilized() const;
	Picture getStabilizedUnion() const;

	void setShiftData(int safeShift, int shiftKoeff);

	void setMaxCornerDifference(int val);
};

typedef boost::shared_ptr<ProjectionEstimator> sh_ptr_PrEst;
