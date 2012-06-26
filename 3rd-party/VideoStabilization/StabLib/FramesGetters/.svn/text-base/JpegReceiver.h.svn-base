#pragma once
#include <string.h>
#include <WTypes.h>
#include "JpegWrap.h"
#include "Include/FrameGetterInterface.h"


class TCPSocket;

class JpegReceiver : public IFrameGetter
{
	typedef boost::shared_ptr<TCPSocket> sh_ptr_sock;
	static const int headerPrediction = 300;
	std::string ip;
	std::string boundary;
	
	sh_ptr_uch buffer;
	int bufferCapacity;
	int bufferLength;
	int bufferStartPoint;
	void shiftBuffer(int place);
	void receivePacket();
	void getParams();
	void init();
	void nextStep();

	bool connectToCamera();

	void createNextPicture();

	sh_ptr_sock sock;

	JpegWrap currentPic;
	Picture curPic;

	bool started;

	HANDLE lock_image; 
	HANDLE event_abort;
	HANDLE stab_thread;

	JpegWrap getNextFrame();
	void mainProc();
	void lockImage();
	void releaseImage();

	static DWORD WINAPI mainProcStarter(LPVOID lpParam);

public:
	JpegReceiver();
	virtual ~JpegReceiver();
	bool start();
	void stop();

	// Only for three colored images.
	Picture nextPic();

	std::string const& getIp() const;
	void setIp(std::string const& camIp);

	bool isStarted() const;
};