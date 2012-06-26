#include "stdafx.h"
#include "mediator.h"
#include "Include/capture_log.h"
#include "Include/different_libs.h"
#include "Include/RegistryParamsSaver.h"

#include "ImageProc/inc/PicturePainter.h"
#include "ImageProc/inc/PictureUpgrade.h"

#include <fstream>

#include "Actions/inc/Analyzer.h"
#include "Actions/inc/BaseVideo.h"
#include "Actions/inc/Stabilizator.h"
#include "Actions/inc/MotionDetectorAction.h"
#include "Actions/inc/Tracker.h"
#include "Actions/inc/UnStabilizator.h"

Mediator::Mediator(MediatorParams const& params):
			convImage(0, 0), curPic(0, 0), converter(1.0)
{
	this->params = params;

	videoSaver = boost::shared_ptr<VideoSaver>(new VideoSaver());
	framesGetter = boost::shared_ptr<FramesGetter>(new FramesGetter());

	modeType = STABILIZE;
	paused = false;

	frame = 0;
	stab_thread = NULL;
	lock_conv_image = ::CreateEvent(NULL, FALSE, TRUE, NULL);
	event_abort		= ::CreateEvent(NULL, TRUE, FALSE, NULL);

	RegistryParamsSaver sf;
	videoSaver->setOutputPath(sf.loadString(_T("UNSTABLE_VIDEO"), _T("OutputPath"), "resVideo.avi"));
	int dist = sf.loadInt(_T("STABILIZATION"), _T("COMPENSATE_DISTORTION"), 1);
	compensateDistortions = dist == 0 ? false : true;

	initActions();
}

void Mediator::setFrame(IFrame* frame)
{
	this->frame = frame;
}

Picture Mediator::getFrame()
{
	lockConvImage();
	Picture framePic = convImage;//.clone();//getRectMiddle(params.bandWidth);
	releaseConvImage();
	return framePic;
}

int Mediator::step()
{
	//TODO
	DWORD tm1 = GetTickCount();
	static int stepAll1 = 0;
	Picture pic = framesGetter->readPic();//.getRectMiddle(params.bandWidth);
//	Capture_Log::getInstance()->log_write("Read time is %d\n", GetTickCount() - tm1);
	if (pic.getWidth() == 0) return 0;

//	Capture_Log::getInstance()->enable();
	static DWORD prevTime = GetTickCount();
//	Capture_Log::getInstance()->log_write("Time between is %d\n", GetTickCount() - prevTime);
	prevTime = GetTickCount();

	converter.setWidthHeight(pic.getWidth(), pic.getHeight());
	curPic = pic;
	if (compensateDistortions)
	{
		PictureConverter picConverter;
		static DWORD sumTm = 0;
		static int stepTm = 0;
		DWORD tm1 = GetTickCount();
		pic = picConverter.fixDistortions(converter, pic);
		DWORD estimateTM = GetTickCount() - tm1;
		sumTm += estimateTM;
		++stepTm;
		//Capture_Log::getInstance()->log_write("UnDistortion time is %d\n", GetTickCount() - tm1);
		//Capture_Log::getInstance()->log_write("UnDistortion time average is %d\n", sumTm / stepTm);
	}


	if (modeType >= 0 && modeType < MAX_MODE_TYPE)
	{
//		DWORD tm = GetTickCount();
		Picture res = actions[modeType]->nextStep(pic);
		//PicturePainter painter;
		//Color col;
		//col.init(255);
		//painter.drawRectangle(res, Point2D(10, 10), Point2D(20, 20), col);
		if (res.getWidth() > 0)
		{
			static DWORD tm = GetTickCount();
			lockConvImage();
			convImage = res;
			releaseConvImage();
			++stepAll1;
			Capture_Log::getInstance()->log_write("Average step time is %d\n", (GetTickCount() - tm) / stepAll1);
		}
		return 0;
	}
	return -2; // Unknown action;
}

DWORD WINAPI Mediator::mainProcStarter(LPVOID lpParam)
{
	((Mediator*)lpParam)->mainProc();
	return 0;
}

void Mediator::initActions()
{
	actions[0] = sh_ptr_IAct(new Stabilizator(params));
	actions[1] = sh_ptr_IAct(new UnStabilizator());
	actions[2] = sh_ptr_IAct(new Analyzer(params));
	actions[3] = sh_ptr_IAct(new BaseVideo());
	actions[4] = sh_ptr_IAct(new MotionDetectorAction());
	actions[5] = sh_ptr_IAct(new Tracker(params));
}


bool Mediator::start()
{
	::ResetEvent(event_abort);

	initActions();

	framesGetter->start();
	stab_thread = ::CreateThread(NULL, 0, mainProcStarter, this, 0, NULL);

	return TRUE;
}

bool Mediator::stop()
{
	if (!stab_thread) return TRUE;
	::SetEvent(event_abort);
	::WaitForSingleObject(stab_thread, INFINITE);
	::CloseHandle(stab_thread);
	framesGetter->stop();
	stab_thread = NULL;
	return TRUE;
}

void Mediator::restart()
{
	stop();
	start();
}


void Mediator::mainProc()
{
	while (true)
	{
		DWORD tm = GetTickCount();
		DWORD res = ::WaitForSingleObject( event_abort, 0);
		if (res == WAIT_OBJECT_0)
		{
			break;
		}
		else
		{
			if (!paused && frame)
			{
				frame->repaintFrame();
			}
		}

		if (!paused)
		{
			if (step() == -1) 
			{
				break;
			}
			videoSaver->saveNextPicture(getFrame());
		}
		DWORD estimatedTime = GetTickCount() - tm;
		DWORD stepTime = 0;
		if (estimatedTime < stepTime)
		{
			Sleep(stepTime - estimatedTime);
		}
	}
}

void Mediator::lockConvImage()
{
	::WaitForSingleObject(lock_conv_image, INFINITE);
}

void Mediator::releaseConvImage()
{
	::SetEvent(lock_conv_image);
}

Mediator::~Mediator()
{
	stop();
	lockConvImage();
	if (lock_conv_image)	::CloseHandle(lock_conv_image);
	if (event_abort)		::CloseHandle(event_abort);
	params.saveParams();

	RegistryParamsSaver sf;
	sf.saveInt(_T("STABILIZATION"), _T("COMPENSATE_DISTORTION"), (int)compensateDistortions);
	sf.saveString(_T("UNSTABLE_VIDEO"), _T("OutputPath"), videoSaver->getOutputPath());
}

ModeType Mediator::getModeType() const
{
	return modeType;
}

void Mediator::setModeType(ModeType modeType)
{
	this->modeType = modeType;
}

MediatorParams& Mediator::getParams()
{
	return params;
}

bool Mediator::pause()
{
	paused = !paused;
	return paused;
}

void Mediator::setDistortionParams(const double* params)
{
	converter.setDistortionParams(params);
}

sh_ptr_db Mediator::getDistortionParams() const
{
	return converter.getDistortionParams();
}

void Mediator::setCompensateDistortions(bool val)
{
	compensateDistortions = val;
}

bool Mediator::ifCompensateDistortions()
{
	return compensateDistortions;
}

UnStabilizator* Mediator::getUnstabilizator()
{
	return (UnStabilizator*)actions[1].get();
}

ProjectiveAnalyzer& Mediator::getProjectiveAnalyzer()
{
	return ((Analyzer*)actions[2].get())->getProjectiveAnalyzer();
}

FramesGetter& Mediator::getFramesGetter()
{
	return *(framesGetter.get());
}

VideoSaver& Mediator::getResVideoSaver()
{
	return *(videoSaver.get());
}

sh_ptr_mtdtc Mediator::getMotionDetector()
{
	return ((MotionDetectorAction*)actions[4].get())->getMotionDetector();
//	return ((Stabilizator*)actions[0].get())->getMotionDetector();
}

Picture Mediator::getOrigin()
{
	return curPic;
}


void Mediator::disableTracker()
{
	((Tracker*)actions[5].get())->disableTracker();
}

void Mediator::setTarget(Point2D const& p1, Point2D const& p2)
{
	((Tracker*)actions[5].get())->setTarget(p1, p2);
}
