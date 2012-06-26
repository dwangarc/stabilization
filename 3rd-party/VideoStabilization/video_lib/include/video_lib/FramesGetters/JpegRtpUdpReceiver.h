#pragma once

#include <string.h>
#include <WTypes.h>

#include "JpegWrap.h"
#include "Include/FrameGetterInterface.h"
#include "rtp/RTP_Receiver.h"


class JpegRtpUdpReceiver : public IFrameGetter
{
	JpegWrap currentPic;
	Picture curPic;

	bool started;
	bool interlaced;

	HANDLE lock_image; 
	HANDLE event_abort;
	HANDLE stab_thread;

	RtpReceiver rtpReceiver;

	sh_ptr_uch pic;

	void init();
	void nextStep();

	int initJpegHeader(sh_ptr_uch pic, unsigned char const* firstPacket) const;

	void createNextPicture();

	JpegWrap getNextFrame();
	void mainProc();
	void lockImage();
	void releaseImage();

	bool ifStop();

	static DWORD WINAPI mainProcStarter(LPVOID lpParam);

public:
	JpegRtpUdpReceiver();
	virtual ~JpegRtpUdpReceiver();

	bool start();
	void stop();

	// Only for three colored images.
	Picture nextPic();

	void setPort(int port);
	int getPort() const;
	void setInterlaced(bool val);
	bool ifInterlaced() const;

	bool isStarted() const;
};
