#include "stdafx.h"
#include "VideoSaver.h"
#include "ImageProc/inc/IplImageWrapper.h"
#include "Include/capture_log.h"

VideoSaver::VideoSaver()
{
	saveVideo = false;
	outPath = "out.avi";
	writer = 0;
}

void VideoSaver::release()
{
	if (writer)
	{
		cvReleaseVideoWriter( &writer);
		writer = 0;
	}
}

VideoSaver::~VideoSaver()
{
	release();
}

void VideoSaver::init()
{
	if (writer) release();
	LogInterface* log = Capture_Log::getInstance();
	try
	{
		writer = cvCreateVideoWriter(outPath.c_str(), CV_FOURCC('M', 'J', 'P', 'G'), 25, size );
	}
	//catch (CMemoryException* e)
	//{
	//	log->log_write("CMemoryException\n");		
	//}
	//catch (CFileException* e)
	//{
	//	log->log_write("CFileException\n");		
	//}
	//catch (CException* e)
	//{
	//	log->log_write("CException\n");		
	//}
	catch (...)
	{
		log->log_write("Any Exception\n");		
	}
}

void VideoSaver::setOutputPath(std::string const& path)
{
	newOutPath = path;
	if (!writer)
	{
		outPath = newOutPath;
	}
}

std::string const& VideoSaver::getOutputPath() const
{
	return outPath;
}


void VideoSaver::restart()
{
	release();
}

void VideoSaver::saveNextPicture(Picture pic)
{
	if (!saveVideo)
	{
		release();
		return;
	}
	if (pic.getWidth() == 0) return;
	if (newOutPath != outPath)
	{
		outPath = newOutPath;
		release();
	}
	if (!writer)
	{
		size.height = pic.getHeight();
		size.width = pic.getWidth();
		init();
	}
	IplImageWrapper image(pic);
	IplImage const* frame = image.getImage();
	cvWriteFrame(writer, frame);
}

void VideoSaver::setSaveVideo(bool saveVideo)
{
	this->saveVideo = saveVideo;
}

bool VideoSaver::ifSaveVideo() const
{
	return saveVideo;
}
