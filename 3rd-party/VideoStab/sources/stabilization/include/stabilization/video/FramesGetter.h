/*
#pragma once

#include <string.h>
#include "Include/FrameGetterInterface.h"
#include "VideoSavers/VideoSaver.h"

class FramesGetter
{
	enum InputPhotoSrc
	{
		IP_CAMERA,
		LOCAL_PHOTOS,
		LOCAL_VIDEO,
		RTP_CAMERA,
		INPUT_PHOTO_SRC_BEGIN = IP_CAMERA,		
		INPUT_PHOTO_SRC_END = RTP_CAMERA + 1		
	};

	typedef IFrameGetter* IFrameGetterPtr;
	IFrameGetterPtr frameGetters[INPUT_PHOTO_SRC_END];

	InputPhotoSrc usedDataType; 
	
	
	VideoSaver videoSaver;
	bool saveIncomeVideo;

public:

	FramesGetter();
	
	Picture readPic();

	void start();
	void stop();

	std::string getCamIp() const;
	void setCamIp(std::string const& ip);

	int getCamRTPPort() const;
	void setCamRTPPort(int port);

	void clearSelectedFrameNames();
	void addSelectedFrame(std::string const& frameName);
	void openVideoFile(std::string const& videoFileName);

	void useCamData();
	void useFileData();
	void useVideoData();
	void useRTPData();

	bool interlaced() const;
	void setInterlaced(bool val);

	VideoSaver& getVideoSaver();

	~FramesGetter();
};*/
