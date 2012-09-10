#pragma once
#include <string>

#include "ImageProc/inc/gausian.h"
#include "ImageProc/inc/pictureConverter.h"
#include "Test/tester.h"
#include "Projective/ProjectionEstimator.h"
#include "Include/MediatorParams.h"
#include "Include/ModeTypes.h"
#include "Include/IFrame.h"

#include "FramesGetter/FramesGetter.h"
#include "ImageProc/inc/ProjectiveAnalyzer.h"
#include "VideoSavers/VideoSaver.h"

#include "MotionDetector/inc/MotionDetector.h"
#include "ObjectTracker/inc/ObjectTracker.h"
#include "Actions/inc/IAction.h"
#include "Actions/inc/UnStabilizator.h"

class Mediator
{
	/*
		0 - Stabilizator
		1 - UnStabilizator
		2 - Analyzer
		3 - BaseVideo
		4 - MotionDetector
		5 - Tracker
	*/

	typedef boost::shared_ptr<IAction> sh_ptr_IAct;
	sh_ptr_IAct actions[6];

	Picture convImage, curPic;
	StereoConverter converter;

	MediatorParams params;

	HANDLE lock_conv_image; // convImage...
	HANDLE event_abort;		// termination event
	HANDLE stab_thread;

	int step();

	void mainProc();
	static DWORD WINAPI mainProcStarter(LPVOID lpParam);		// processing thread main routine

	void lockConvImage();
	void releaseConvImage();


	ModeType modeType;

	IFrame* frame;

	bool paused;
	bool compensateDistortions;

	boost::shared_ptr<FramesGetter> framesGetter;	
	boost::shared_ptr<VideoSaver> videoSaver;

	void initActions();

public:
	Mediator(MediatorParams const& params);
	void setFrame(IFrame* frame);
	~Mediator();
	Picture getFrame();
	Picture getOrigin();

	bool start();
	bool stop();
	void restart();
	bool pause();

	ModeType getModeType() const;
	void setModeType(ModeType modeType);

	MediatorParams& getParams();

	void setDistortionParams(const double* params);
	sh_ptr_db getDistortionParams() const;
	void setCompensateDistortions(bool val);
	bool ifCompensateDistortions();


	FramesGetter& getFramesGetter();
	ProjectiveAnalyzer& getProjectiveAnalyzer();
	VideoSaver& getResVideoSaver();

	sh_ptr_mtdtc getMotionDetector();

	UnStabilizator* getUnstabilizator();

	void disableTracker();
	void setTarget(Point2D const& p1, Point2D const& p2);
};