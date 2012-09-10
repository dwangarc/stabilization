#pragma once

#include "opencv/highgui.h"
#include <string.h>
#include "ImageProc/inc/Picture.h"

class VideoSaver
{
	CvVideoWriter* writer;
	std::string newOutPath, outPath;
	CvSize size;
	volatile bool saveVideo;
	void release();
	void init();
public:
	VideoSaver();
	
	void setOutputPath(std::string const& path);
	std::string const& getOutputPath() const;

	void saveNextPicture(Picture pic);
	void restart();

	void setSaveVideo(bool saveVideo);
	bool ifSaveVideo() const;

	~VideoSaver();
};