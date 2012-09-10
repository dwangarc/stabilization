#include "stdafx.h"
#include "../inc/Tracker.h"
#include "ImageProc/inc/PicturePainter.h"
#include "Include/different_libs.h"
#include "Include/capture_log.h"
#include "Projective/ProjectiveConverter.h"

#include <Windows.h>

Picture Tracker::nextStep(Picture basePic)
{
	if (basePic.getWidth() == 0) return Picture();

	//stabilizator->stabilize(basePic, pyrIter, downscale);
	//Picture pic = stabilizator->getStabilizedUnion().clone();
	Picture picStabOld = shiftBasePic(basePic);
	Picture pic = basePic.clone();

//	Picture pic = basePic;

	objectTrackerStep(pic);

	processMove(pic, picStabOld);

	//lst_bfs objects = motionDetector->nextStep(pic);

	markTarget(pic);

	int step = 15;
	if (++iterStep % step == 0)
	{
		old = basePic.clone();
	}

	return pic;
}

void Tracker::processMove(Picture& pic, Picture picStabOld)
{
	lst_bfs obj = motionDetector->getSimpleMoved(pic, picStabOld);

	int totalSize = 0;
	for (lst_bfs::iterator iter = obj.begin(); iter != obj.end(); ++iter)
	{
		totalSize += iter->size;
	}

	double maxPercent = 0.2;
	size_t maxObjects = 6;
	if (obj.size() > maxObjects || totalSize > pic.getHeight() * pic.getWidth() * maxPercent / sqr(motionDetector->getDownscale()))
	{
		motionDetector->reset();
	}
	else
	{
		markMove(pic, obj);
	}

}


void Tracker::markTarget(Picture& pic) const
{
	if (useObjectTracker)
	{
		Point2D p1 = objectTracker.getP1();
		Point2D p2 = objectTracker.getP2();
		PicturePainter painter;
		Color color(0);
		color.r() = 255;
		painter.drawRectangleCross(pic, p1, p2, color);
	}
}

void Tracker::objectTrackerStep(Picture basePic)
{
	if (useObjectTracker)
	{
		if (!objectTracker.initialized())
		{
			objectTracker.init(basePic, targetP1, targetP2);
		}
		else
		{
			objectTracker.step(basePic);
		}
	}
}


Picture Tracker::shiftBasePic(Picture basePic)
{
	if (old.getWidth() != basePic.getWidth())
	{
		old = basePic.clone();
	}
	sh_ptr_db proj = stabilizator->estimateProj(basePic, old, pyrIter);
	ProjectiveConverter conv;
	return conv.applyProj(old, proj.get());
}


void Tracker::markMove(Picture& pic, lst_bfs const& obj)
{
	PicturePainter painter;

	Picture diff = motionDetector->getSimpleMovedRes();
	Color slightRed(0);
	slightRed.r() = 100;
	painter.markNonNull(pic, diff, slightRed);

	Color color(50);
	color.b() = 200;
	int gain = 8;
	for (lst_bfs::const_iterator iter = obj.begin(); iter != obj.end(); ++iter)
	{
		Point2D p1(iter->pMin.y - gain, iter->pMin.x - gain);
		Point2D p2(iter->pMax.y + gain, iter->pMax.x + gain);
		painter.drawRectangle(pic, p1, p2, color);
	}
}


Tracker::Tracker(MediatorParams const&params):motionDetector(new MotionDetector()), old()
{
	iterStep = 0;
	stabilizator = sh_ptr_PrEst(new ProjectionEstimator(params.pyrSize, params.defaultIterNum, params.alpha));
	stabilizator->setMaxCornerDifference(params.maxCornerDistance / params.downscale);
	pyrIter = params.pyrIter;
	downscale = params.downscale;
	useObjectTracker = false;	
}

void Tracker::disableTracker()
{
	useObjectTracker = false;
	objectTracker.dispose();
}

void Tracker::setTarget(Point2D const& p1, Point2D const& p2)
{
	useObjectTracker = true;
	targetP1 = p1;
	targetP2 = p2;
}
