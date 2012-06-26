#include "stdafx.h"
#include "JpegRtpUdpReceiver.h"
#include "Include/RegistryParamsSaver.h"
#include "RTP/RTP_Receiver.h"
#include "ImageProc/inc/JpegData.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

JpegRtpUdpReceiver::JpegRtpUdpReceiver():currentPic(0, 0), rtpReceiver(4096)
{
	started = false;
	RegistryParamsSaver saver;
	interlaced = saver.loadInt(_T("JpegRtpUdpReceiver"), _T("Interlace"), 1) == 0 ? false : true;

	int maxJpegSize = 1500000;
	pic = sh_ptr_uch(new unsigned char[maxJpegSize]);

	stab_thread = NULL;
	lock_image = ::CreateEvent(NULL, FALSE, TRUE, NULL);
	event_abort	= ::CreateEvent(NULL, TRUE, FALSE, NULL);

}

void JpegRtpUdpReceiver::init()
{
	started = false;
}

int JpegRtpUdpReceiver::getPort() const
{
	return rtpReceiver.getPort();
}

void JpegRtpUdpReceiver::setPort(int val)
{
	rtpReceiver.setPort((unsigned short) val);
}

void JpegRtpUdpReceiver::setInterlaced(bool val)
{
	interlaced = val;
	RegistryParamsSaver saver;
	saver.saveInt(_T("JpegRtpUdpReceiver"), _T("Interlace"), interlaced);
}

bool JpegRtpUdpReceiver::ifInterlaced() const
{
	return interlaced;
}



DWORD WINAPI JpegRtpUdpReceiver::mainProcStarter(LPVOID lpParam)
{
	((JpegRtpUdpReceiver*)lpParam)->mainProc();
	return 0;
}

void JpegRtpUdpReceiver::mainProc()
{
	started = true;
	while (true)
	{
		if (ifStop()) break;
		nextStep();
	}
}

bool JpegRtpUdpReceiver::ifStop()
{
	DWORD res = ::WaitForSingleObject( event_abort, 0);
	if (res == WAIT_OBJECT_0)
	{
		return true;
	}
	return false;
}


void JpegRtpUdpReceiver::lockImage()
{
	::WaitForSingleObject(lock_image, INFINITE);
}

void JpegRtpUdpReceiver::releaseImage()
{
	::SetEvent(lock_image);
}

bool JpegRtpUdpReceiver::start()
{
	init();
	::ResetEvent(event_abort);
	stab_thread = ::CreateThread(NULL, 0, mainProcStarter, this, 0, NULL);
	return true;

}

JpegWrap JpegRtpUdpReceiver::getNextFrame()
{
	//lockImage();
	//JpegWrap pic(currentPic.pic.get(), currentPic.length);
	//releaseImage();
	//return pic;
	return currentPic;
}

void JpegRtpUdpReceiver::nextStep()
{
	bool first = true;
	bool use = true;
	int picSize = 0;
	while (true)
	{
		if (ifStop()) return;
		RTP_packet packet = rtpReceiver.receive();

		if (packet.length == -123)
		{
			use = false;
		}
		if (packet.length <= 0) continue;
		int shift = 8;
		if (first)
		{
			first = false;
			picSize = initJpegHeader(pic, packet.data.get());
			shift += 132;
		}
		memcpy(pic.get() + picSize, packet.data.get() + shift, (packet.length - shift)  * sizeof(unsigned char));
		picSize += packet.length - shift;
		if (packet.marker) break;
	}
	if (use)
	{
		currentPic = JpegWrap(pic.get(), picSize);
	}
}

int JpegRtpUdpReceiver::initJpegHeader(sh_ptr_uch pic, unsigned char const* firstPacket) const
{
	int type = firstPacket[4];
	int width = firstPacket[6];
	int height = firstPacket[7];
	width *= 8;
	height *= 8;
	int place = 0;
	memcpy(pic.get() + place, jpegData, 25 * sizeof(unsigned char));
	place += 25;
	pic.get()[place++] = (unsigned char)(height / 256);
	pic.get()[place++] = (unsigned char)(height % 256);
	pic.get()[place++] = (unsigned char)(width / 256);
	pic.get()[place++] = (unsigned char)(width % 256);
	memcpy(pic.get() + place, jpegData1, 15 * sizeof(unsigned char));
	if (type == 1)
	{
		*(pic.get() + place + 2) = (unsigned char)0x22;
	}
	place += 15;
	memcpy(pic.get() + place, firstPacket + 12, 64 * sizeof(unsigned char));
	place += 64;
	memcpy(pic.get() + place, jpegData2, 5 * sizeof(unsigned char));
	place += 5;
	memcpy(pic.get() + place, firstPacket + 12 + 64, 64 * sizeof(unsigned char));
	place += 64;
	memcpy(pic.get() + place, jpegData3, 446 * sizeof(unsigned char));
	place += 446;
	return place;
}


void JpegRtpUdpReceiver::stop()
{
	if (!stab_thread) return;

	::SetEvent(event_abort);
	::WaitForSingleObject(stab_thread, INFINITE);
	::CloseHandle(stab_thread);
	stab_thread = NULL;
	started = false;
	rtpReceiver.disconnect();


}

JpegRtpUdpReceiver::~JpegRtpUdpReceiver()
{
	stop();
	lockImage();
	if (lock_image)			::CloseHandle(lock_image);
	if (event_abort)		::CloseHandle(event_abort);
}

bool JpegRtpUdpReceiver::isStarted() const
{
	return started;
}

Picture JpegRtpUdpReceiver::nextPic()
{
	createNextPicture();
	return curPic;
}

void JpegRtpUdpReceiver::createNextPicture()
{
	JpegWrap frame = getNextFrame();
	if (frame.length == 0) return;

	CxImage* image = new CxImage(frame.pic.get(), frame.length, CXIMAGE_FORMAT_JPG);
	Picture pic = Picture::createFromCxImage(image);
	delete image;
	if (interlaced)
	{
		curPic = pic.deinterlace();
	}
	else
	{
		curPic = pic.clone();
	}
}

