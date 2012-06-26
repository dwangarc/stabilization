#pragma once
#include <boost/shared_ptr.hpp>
#include "ImageProc/inc/Picture.h"
#include "Include/MotionTypes.h"
#include "ImageProc/inc/PictureAlgs.h"
#include "StablePicDetector.h"

#include <list>

class MotionDetector
{
	Picture stayed, moved;
	Picture smallMoved, smallStayed;
	Picture aims;
	StablePicDetector stableDetector, veryOldDetector;
	int iter;

	volatile int newDownscale;
	int eps, baseEps, updateIter, downscale;
	int minObjectSize, minFreeSpaceSize, smoothParam;

	void init();

	void createMoved(Picture pic, Picture smallPic);
	void createStayed();

	lst_bfs createObjects(Picture smallPic);

	lst_bfs fixResult(Picture& res, Picture basePic) const;

	MotionDetectionModes mode;

	void lockData();
	void releaseData();
	HANDLE lock_data;

	Picture simpleMv;

	
public:
	MotionDetector();
	~MotionDetector();
	lst_bfs nextStep(Picture cur);

	void reset();

	void setMode(int mode);
	MotionDetectionModes getMode() const;

	Picture getMoved();
	Picture getBase();
	Picture getStayed();
	Picture getVeryOld();

	lst_bfs getSimpleMoved(Picture pic1, Picture pic2);
	Picture getSimpleMovedRes();

	Picture getAims();

	Picture getPicByMode();

	void setEPS(int val);
	void setBaseEps(int val);
	void setDownscale(int val);
	void setSmoothParam(int val);
	void setUpdateIter(int val);
	void setMinObjectSize(int val);
	void setMinFreeSpaceSize(int val);

	int getEPS() const;
	int getBaseEps() const;
	int getDownscale() const;
	int getSmoothParam() const;
	int getUpdateIter() const;
	int getMinObjectSize() const;
	int getMinFreeSpaceSize() const;

};

typedef boost::shared_ptr<MotionDetector> sh_ptr_mtdtc;
