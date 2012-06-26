#include "stdafx.h"
#include "../inc/MotionDetector.h"
#include "ImageProc/inc/PictureAlgs.h"
#include "Include/RegistryParamsSaver.h"
#include "ImageProc/inc/gausian.h"
#include "ImageProc/inc/pictureConverter.h"
#include "Include/different_libs.h"

#include <omp.h>

MotionDetector::MotionDetector()
{
	lock_data = 0;
	RegistryParamsSaver saver;
	eps					= saver.loadInt(_T("MotionDetector"), _T("Eps"), 15);
	baseEps				= saver.loadInt(_T("MotionDetector"), _T("BaseEps"), 5);
	smoothParam			= saver.loadInt(_T("MotionDetector"), _T("Smooth"), 4);
	downscale			= saver.loadInt(_T("MotionDetector"), _T("Downscale"), 2);
	updateIter			= saver.loadInt(_T("MotionDetector"), _T("UpdateIter"), 10);
	minObjectSize		= saver.loadInt(_T("MotionDetector"), _T("MinObjectSize"), 100);
	minFreeSpaceSize	= saver.loadInt(_T("MotionDetector"), _T("MinFreeSpaceSize"), 100);
	newDownscale = downscale;
	init();
}

void MotionDetector::init()
{
	lockData();
	if (lock_data) ::CloseHandle(lock_data);
	lock_data = ::CreateEvent(NULL, FALSE, TRUE, NULL);
	stableDetector = StablePicDetector(10, downscale, baseEps);
	veryOldDetector = StablePicDetector(10, downscale, baseEps);
	iter = 0;
	moved = Picture();
	stayed = Picture();
	mode = MOTION_BASE;
}

void MotionDetector::reset()
{
	init();
}

lst_bfs MotionDetector::getSimpleMoved(Picture pic1, Picture pic2)
{
	if (newDownscale != downscale)
	{
		downscale = newDownscale;
		init();
	}
	PictureConverter conv;
	Picture small1 = conv.downscale(pic1, downscale);
	Picture small2 = conv.downscale(pic2, downscale);
	Picture smallMoved = small2 - small1;
	int band = 8;
	smallMoved.fill(0, 0, 0, band, smallMoved.getHeight());
	smallMoved.fill(0, 0, 0, smallMoved.getWidth(), band);
	smallMoved.fill(0, smallMoved.getWidth() - band, 0, smallMoved.getWidth(), smallMoved.getHeight());
	smallMoved.fill(0, 0, smallMoved.getHeight() - band, smallMoved.getWidth(), smallMoved.getHeight());
	
	lst_bfs objData = fixResult(smallMoved, small2);
	for (lst_bfs::iterator iter = objData.begin(); iter != objData.end(); ++iter)
	{
		iter->pMax *= downscale;
		iter->pMin *= downscale;
	}

	simpleMv = conv.fromSmallPic(smallMoved, pic2, downscale);
	return objData;
}

Picture MotionDetector::getSimpleMovedRes()
{
	return simpleMv;
}



lst_bfs MotionDetector::nextStep(Picture cur)
{
	if (cur.getWidth() == 0) 
	{
		return lst_bfs();
	}
	if (newDownscale != downscale)
	{
		downscale = newDownscale;
		init();
	}

	PictureConverter conv;
	Picture smallCur = conv.downscale(cur, downscale);
	Gaussian gs(smallCur.getWidth(), smallCur.getHeight(), 2.0,  3);
	smallCur = gs.apply(smallCur);

	lockData();
	stableDetector.nextStep(cur, smallCur);
	releaseData();

	if (iter % updateIter == 0)
	{
		lockData();
		veryOldDetector.nextStep(stableDetector.getBase(), stableDetector.getSmallBase());
		releaseData();
	}

//#pragma omp parallel
//	{
//		if (omp_get_thread_num())
//		{
			createMoved(cur, smallCur);
		//}
		//else
		//{
			createStayed();
	//	}
	//}
	++iter;
	return createObjects(smallCur);
}

Picture MotionDetector::getMoved()
{
	lockData();
	Picture picCopy = moved.clone();
	releaseData();
	return picCopy;
}

Picture MotionDetector::getBase()
{
	lockData();
	Picture picCopy = stableDetector.getBase().clone();
	releaseData();
	return picCopy;
}

Picture MotionDetector::getStayed()
{
	lockData();
	Picture picCopy = stayed.clone();
	picCopy.overlay(moved);
	releaseData();
	return picCopy;
}

Picture MotionDetector::getVeryOld()
{
	lockData();
	Picture picCopy = veryOldDetector.getBase().clone();
	releaseData();
	return picCopy;
}

Picture MotionDetector::getAims()
{
	PictureConverter converter;
	return converter.stretch(aims, downscale);
}


lst_bfs MotionDetector::createObjects(Picture smallPic)
{
	aims = smallStayed.clone();
	aims.overlay(smallMoved);
	lst_bfs res = fixResult(aims, smallPic);
	for (lst_bfs::iterator iter = res.begin(); iter != res.end(); ++iter)
	{
		iter->pMax *= downscale;
		iter->pMin *= downscale;
	}
	return res;
}


void MotionDetector::createMoved(Picture pic, Picture smallPic)
{
	Picture smallBase = stableDetector.getSmallBase();
	smallMoved = smallPic - smallBase;
	fixResult(smallMoved, smallPic);
	PictureConverter conv;
	lockData();
	moved = conv.fromSmallPic(smallMoved, pic, downscale);
	releaseData();
}

void MotionDetector::createStayed()
{
	Picture smallBase = stableDetector.getSmallBase();
	Picture smallOld = veryOldDetector.getSmallBase();
	smallStayed = smallBase - smallOld;
	fixResult(smallStayed, smallBase);
	PictureConverter conv;
	lockData();
	stayed = conv.fromSmallPic(smallStayed, stableDetector.getBase(), downscale);
	releaseData();
}


lst_bfs MotionDetector::fixResult(Picture& res, Picture basePic) const
{
	PictureAlgs cleaner;

	//	cleaner.zeroSame(moved, base, 25);
	cleaner.toBitMask(res, eps);

	lst_bfs objects = cleaner.excludeSmallSpaces(res, minObjectSize / (int)sqr(downscale), 1);
	cleaner.smooth(res, smoothParam / downscale);
	
	cleaner.excludeSmallIslands(res, minFreeSpaceSize / (int)sqr(downscale), 1);
	cleaner.fromBitMask(res, basePic);

	return objects;
}



void MotionDetector::setMode(int mode)
{
	this->mode = MotionDetectionModes(mode);
}

MotionDetectionModes MotionDetector::getMode() const
{
	return mode;
}


Picture MotionDetector::getPicByMode()
{
	switch (mode)
	{
	case MOTION_BASE: return getBase();
	case MOTION_STAYED: return getStayed();
	case MOTION_MOVED: return getMoved();
	}
	return getMoved();
}


void MotionDetector::setEPS(int val)
{
	eps = val;
}

void MotionDetector::setUpdateIter(int val)
{
	updateIter = val;
}

void MotionDetector::setMinObjectSize(int val)
{
	minObjectSize = val;
}

void MotionDetector::setMinFreeSpaceSize(int val)
{
	minFreeSpaceSize = val;
}

void MotionDetector::setBaseEps(int val)
{
	baseEps = val;
}

void MotionDetector::setDownscale(int val)
{
	newDownscale = val;
}

void MotionDetector::setSmoothParam(int val)
{
	smoothParam = val;
}

int MotionDetector::getEPS() const
{
	return eps;
}

int MotionDetector::getUpdateIter() const
{
	return updateIter;
}

int MotionDetector::getMinObjectSize() const
{
	return minObjectSize;
}

int MotionDetector::getMinFreeSpaceSize() const
{
	return minFreeSpaceSize;
}

int MotionDetector::getBaseEps() const
{
	return baseEps;
}

int MotionDetector::getDownscale() const
{
	return newDownscale;
}

int MotionDetector::getSmoothParam() const
{
	return smoothParam;
}


MotionDetector::~MotionDetector()
{
	if (lock_data)	::CloseHandle(lock_data);
	RegistryParamsSaver saver;
	saver.saveInt(_T("MotionDetector"), _T("Eps"), eps);
	saver.saveInt(_T("MotionDetector"), _T("BaseEps"), baseEps);
	saver.saveInt(_T("MotionDetector"), _T("Smooth"), smoothParam);
	saver.saveInt(_T("MotionDetector"), _T("Downscale"), newDownscale);
	saver.saveInt(_T("MotionDetector"), _T("UpdateIter"), updateIter);
	saver.saveInt(_T("MotionDetector"), _T("MinObjectSize"), minObjectSize);
	saver.saveInt(_T("MotionDetector"), _T("MinFreeSpaceSize"), minFreeSpaceSize);
}

void MotionDetector::lockData()
{
	::WaitForSingleObject(lock_data, INFINITE);
}

void MotionDetector::releaseData()
{
	::SetEvent(lock_data);
}
