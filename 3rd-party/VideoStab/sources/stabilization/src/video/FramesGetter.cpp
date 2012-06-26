/*

#include "FramesGetter.h"

#include "FramesGetters/JpegReceiver.h"
#include "FramesGetters/frames_reader.h"
#include "FramesGetters/VideoReader.h"
#include "FramesGetters/JpegRtpUdpReceiver.h"
#include "Include/RegistryParamsSaver.h"


FramesGetter::FramesGetter()
{
	RegistryParamsSaver sf;
	videoSaver.setOutputPath(sf.loadString(_T("FRAMES_GETTER"), _T("OutputPath"), "baseVideo.avi"));
	usedDataType = LOCAL_PHOTOS;
	for (int i = INPUT_PHOTO_SRC_BEGIN; i < INPUT_PHOTO_SRC_END; ++i)
	{
		frameGetters[i] = 0;
	}
	frameGetters[IP_CAMERA] = new JpegReceiver();
	frameGetters[LOCAL_PHOTOS] = new FramesReader();
	frameGetters[LOCAL_VIDEO] = new VideoReader();
	frameGetters[RTP_CAMERA] = new JpegRtpUdpReceiver();
}

FramesGetter::~FramesGetter()
{
	RegistryParamsSaver sf;
	sf.saveString(_T("FRAMES_GETTER"), _T("OutputPath"), videoSaver.getOutputPath());
	for (int i = INPUT_PHOTO_SRC_BEGIN; i < INPUT_PHOTO_SRC_END; ++i)
	{
		if (frameGetters[i]) delete frameGetters[i];
	}
}

Picture FramesGetter::readPic()
{
	if (frameGetters[usedDataType])
	{
		Picture res = frameGetters[usedDataType]->nextPic();
		videoSaver.saveNextPicture(res);
		return res;
	}
	return Picture();
}

void FramesGetter::start()
{
	if (frameGetters[usedDataType] && !frameGetters[usedDataType]->isStarted())
	{
		frameGetters[usedDataType]->start();
	}
}

void FramesGetter::stop()
{
	if (frameGetters[usedDataType] && frameGetters[usedDataType]->isStarted())
	{
		frameGetters[usedDataType]->stop();
	}
}



void FramesGetter::useCamData()
{
	usedDataType = IP_CAMERA;
}

void FramesGetter::useFileData()
{
	usedDataType = LOCAL_PHOTOS;
}

void FramesGetter::useVideoData()
{
	usedDataType = LOCAL_VIDEO;
}

void FramesGetter::useRTPData()
{
	usedDataType = RTP_CAMERA;
}

bool FramesGetter::interlaced() const
{
	if (frameGetters[RTP_CAMERA])
	{
		return ((JpegRtpUdpReceiver*)frameGetters[RTP_CAMERA])->ifInterlaced();
	}
	return false;
}

void FramesGetter::setInterlaced(bool val)
{
	if (frameGetters[RTP_CAMERA])
	{
		((JpegRtpUdpReceiver*)frameGetters[RTP_CAMERA])->setInterlaced(val);
	}
}



std::string FramesGetter::getCamIp() const
{
	if (frameGetters[IP_CAMERA])
	{
		return ((JpegReceiver*)frameGetters[IP_CAMERA])->getIp();
	}
	return "";
}

void FramesGetter::setCamIp(std::string const& ip)
{
	if (frameGetters[IP_CAMERA])
	{
		((JpegReceiver*)frameGetters[IP_CAMERA])->setIp(ip);
	}
}

int FramesGetter::getCamRTPPort() const
{
	if (frameGetters[RTP_CAMERA])
	{
		return ((JpegRtpUdpReceiver*)frameGetters[RTP_CAMERA])->getPort();
	}
	return 0;
}

void FramesGetter::setCamRTPPort(int val)
{
	if (frameGetters[RTP_CAMERA])
	{
		((JpegRtpUdpReceiver*)frameGetters[RTP_CAMERA])->setPort(val);
	}
}


void FramesGetter::clearSelectedFrameNames()
{
	if (frameGetters[LOCAL_PHOTOS])
	{
		((FramesReader*)frameGetters[LOCAL_PHOTOS])->clearSelectedFrames();
	}
}

void FramesGetter::addSelectedFrame(std::string const& frameName)
{
	if (frameGetters[LOCAL_PHOTOS])
	{
		((FramesReader*)frameGetters[LOCAL_PHOTOS])->addSelectedFrame(frameName);
	}
}

void FramesGetter::openVideoFile(std::string const& videoFileName)
{
	if (frameGetters[LOCAL_VIDEO])
	{
		((VideoReader*)frameGetters[LOCAL_VIDEO])->openVideoFile(videoFileName);
	}
}

VideoSaver& FramesGetter::getVideoSaver()
{
	return videoSaver;
}
*/
