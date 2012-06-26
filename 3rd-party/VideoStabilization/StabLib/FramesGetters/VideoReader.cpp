#include "stdafx.h"

#include "VideoReader.h"

VideoReader::VideoReader()
{
	capture = 0;
}

void VideoReader::openVideoFile(std::string const& fileName)
{
	releaseCvCapture();
	capture = cvCreateFileCapture( fileName.c_str());
}

void VideoReader::releaseCvCapture()
{
	if (capture)
	{
		cvReleaseCapture( &capture );	
		capture = 0;
	}
}

Picture VideoReader::nextPic()
{
	if (!capture) return Picture();
	IplImage* frame = cvQueryFrame( capture );
	return Picture::createFromIplImage(frame);
}

VideoReader::~VideoReader()
{
	releaseCvCapture();
}
