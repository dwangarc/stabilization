/*
#pragma once
#include "IAction.h"
#include "StabLib/Include/MediatorParams.h"
#include "Projective/ProjectionEstimator.h"
#include "MotionDetector/inc/MotionDetector.h"

class Stabilizator : public IAction
{
	PictureConverter picCv;
	sh_ptr_PrEst stabilizator;
	sh_ptr_int pyrIter;
	int downscale;
	Picture prevPic;
	Picture motion;
public:
	Stabilizator(MediatorParams const&params);
	Picture nextStep(Picture pic);

};*/
