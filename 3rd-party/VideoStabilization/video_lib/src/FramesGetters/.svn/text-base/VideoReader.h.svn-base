#pragma once
#include "Include/FrameGetterInterface.h"
#include "highgui.h"

class VideoReader : public IFrameGetter
{
	CvCapture* capture;
	void releaseCvCapture();
public:
	VideoReader();
	void openVideoFile(std::string const& fileName);
	Picture nextPic();
	virtual ~VideoReader();
};