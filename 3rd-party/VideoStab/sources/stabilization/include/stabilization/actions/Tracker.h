/*
#pragma once
#include "IAction.h"
#include "StabLib/Include/MediatorParams.h"
#include "Projective/ProjectionEstimator.h"
#include "MotionDetector/inc/MotionDetector.h"
#include "ObjectTracker/inc/ObjectTracker.h"

class Tracker : public IAction
{
	sh_ptr_mtdtc motionDetector;

	sh_ptr_PrEst stabilizator;
	sh_ptr_int pyrIter;
	int downscale;
	Picture old;

	ObjectTracker objectTracker;
	Point2D targetP1, targetP2;
	bool useObjectTracker;
	int iterStep;

	void markMove(Picture& pic, lst_bfs const& obj);
	void markTarget(Picture& pic) const;

	void processMove(Picture& pic, Picture picStabOld);

	Picture shiftBasePic(Picture basePic);
	void objectTrackerStep(Picture basePic);


public:
	Tracker(MediatorParams const&params);
	Picture nextStep(Picture pic);

	void disableTracker();
	void setTarget(Point2D const& p1, Point2D const& p2);
};*/
