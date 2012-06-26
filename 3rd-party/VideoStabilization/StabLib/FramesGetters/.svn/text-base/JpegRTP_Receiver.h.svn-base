#pragma once

#include <string.h>
#include <WTypes.h>

#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"

#include "JpegWrap.h"
#include "Include/FrameGetterInterface.h"


class JpegRTPReceiver : public IFrameGetter
{

	std::string ip;
	int port;

	sh_ptr_uch buffer;
	int bufferCapacity;
	int bufferLength;
	int bufferStartPoint;

	JpegWrap currentPic;
	Picture curPic;

	bool started;

	HANDLE lock_image; 
	HANDLE event_abort;
	HANDLE stab_thread;

	boost::shared_ptr<RTPUDPv4TransmissionParams> transparams;
	boost::shared_ptr<RTPSession> sess;


	void shiftBuffer(int place);
	int receivePacket();
	void init();
	void nextStep();

	int initJpegHeader(sh_ptr_uch pic, unsigned char const* firstPacket) const;

	int checkerror(int rtperr);
	bool connectToCamera();

	void createNextPicture();

	JpegWrap getNextFrame();
	void mainProc();
	void lockImage();
	void releaseImage();

	static DWORD WINAPI mainProcStarter(LPVOID lpParam);

public:
	JpegRTPReceiver();
	virtual ~JpegRTPReceiver();

	bool start();
	void stop();

	// Only for three colored images.
	Picture nextPic();

	std::string const& getIp() const;
	void setIp(std::string const& camIp);

	void setPort(int port);
	int getPort() const;

	bool isStarted() const;
};
