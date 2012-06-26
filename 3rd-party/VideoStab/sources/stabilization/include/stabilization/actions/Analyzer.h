/*
#pragma once
#include "IAction.h"
#include "StabLib/Include/MediatorParams.h"
#include "Projective/ProjectionEstimator.h"
#include "ImageProc/inc/ProjectiveAnalyzer.h"

class Analyzer : public IAction
{
	Picture prevPic;
	ProjectiveAnalyzer projectiveAnalyzer;
	sh_ptr_PrEst stabilizator;
	sh_ptr_int pyrIter;
public:
	Analyzer(MediatorParams const&params);
	ProjectiveAnalyzer& getProjectiveAnalyzer();
	
	Picture nextStep(Picture pic);
};*/
