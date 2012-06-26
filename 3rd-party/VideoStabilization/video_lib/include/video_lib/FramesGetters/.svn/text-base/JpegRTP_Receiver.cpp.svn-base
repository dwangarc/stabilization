#include "stdafx.h"
#include "JpegRTP_Receiver.h"
#include "Include/capture_log.h"
#include "Include/RegistryParamsSaver.h"
#include "ImageProc/inc/JpegData.h"

#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#ifndef WIN32
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif // WIN32
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

int JpegRTPReceiver::checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		Capture_Log::getInstance()->log_write("ERROR: %s\n", RTPGetErrorString(rtperr));
		return -1;
	}
	return 0;
}

JpegRTPReceiver::JpegRTPReceiver():currentPic(0, 0)
{
	started = false;
	bufferCapacity = 3000000;
	buffer = sh_ptr_uch(new unsigned char[bufferCapacity]);
	RegistryParamsSaver saver;
	ip = saver.loadString(_T("JpegRTPReceiver"), _T("CameraIP"), "127.16.0.1");
	port = saver.loadInt(_T("JpegRTPReceiver"), _T("CameraPort"), 5004);

	stab_thread = NULL;
	lock_image = ::CreateEvent(NULL, FALSE, TRUE, NULL);
	event_abort	= ::CreateEvent(NULL, TRUE, FALSE, NULL);

	transparams = boost::shared_ptr<RTPUDPv4TransmissionParams>(new RTPUDPv4TransmissionParams());
	sess = boost::shared_ptr<RTPSession>(new RTPSession());
}


int JpegRTPReceiver::receivePacket()
{
	int startPoint = bufferLength + bufferStartPoint;
	RTPPacket *pack = NULL;
	LogInterface* log = Capture_Log::getInstance();

	while (pack == NULL)
	{
		while (!started)
		{
			RTPTime::Wait(RTPTime(1,0));
			connectToCamera();
		}
		while ((pack = sess->GetNextPacket()) == NULL)
		{
			if (!sess->GotoNextSourceWithData())
			{
				sess->EndDataAccess();
				started = false;
				break;
			}
		}
	}

	int bytesReceived = pack->GetPayloadLength(); 
	//log->log_write("Bytes recv %d\n", bytesReceived);
	//log->log_write("Buffer length %d\n", bufferLength);
	//log->log_write("Buffer start point %d\n", bufferStartPoint);
	//log->log_write("Packet marker%d\n", pack->HasMarker());
	memcpy(buffer.get() + startPoint, pack->GetPayloadData(), bytesReceived * sizeof(uint8_t));
	bufferLength += bytesReceived;
	int mark = pack->HasMarker() ? 1 : 0;
//	Capture_Log::getInstance()->log_write("Got packet! length is %d, marker is %d\n", bytesReceived, mark);
	sess->DeletePacket(pack);
	return mark;
}

void JpegRTPReceiver::init()
{
#ifdef WIN32
	WSADATA dat;
	WSAStartup(MAKEWORD(2,2),&dat);
#endif // WIN32
	started = false;
	bufferLength = 0;
	bufferStartPoint = 0;
	RTPSessionParams sessparams;
	sessparams.SetOwnTimestampUnit(1.0 / 8000.0);		
	sessparams.SetAcceptOwnPackets(true);
	transparams->SetPortbase((uint16_t)port);
	int status = sess->Create(sessparams, transparams.get());	
	checkerror(status);

}

std::string const& JpegRTPReceiver::getIp() const
{
	return ip;
}

int JpegRTPReceiver::getPort() const
{
	return port;
}

void JpegRTPReceiver::setIp(std::string const& camIp)
{
	ip = camIp;
}

void JpegRTPReceiver::setPort(int val)
{
	port = val;
}


DWORD WINAPI JpegRTPReceiver::mainProcStarter(LPVOID lpParam)
{
	((JpegRTPReceiver*)lpParam)->mainProc();
	return 0;
}

void JpegRTPReceiver::mainProc()
{
	while (true)
	{
		DWORD res = ::WaitForSingleObject( event_abort, 0);
		if (res == WAIT_OBJECT_0)
		{
			break;
		}
		nextStep();
		//if (!started)
		//{
		//	connectToCamera();
		//}
		//if (started)
		//{
		//	nextStep();
		//}
		//else
		//{
		//	RTPTime::Wait(RTPTime(1,0));
		//}
	}
}

void JpegRTPReceiver::lockImage()
{
	::WaitForSingleObject(lock_image, INFINITE);
}

void JpegRTPReceiver::releaseImage()
{
	::SetEvent(lock_image);
}

bool JpegRTPReceiver::connectToCamera()
{
	sess->BeginDataAccess();
	started = sess->GotoFirstSourceWithData();
	if (!started)
	{
		sess->EndDataAccess();
	}
	return started;
}


bool JpegRTPReceiver::start()
{
	init();
	::ResetEvent(event_abort);
	stab_thread = ::CreateThread(NULL, 0, mainProcStarter, this, 0, NULL);
	return true;

}

JpegWrap JpegRTPReceiver::getNextFrame()
{
	lockImage();
	JpegWrap pic(currentPic.pic.get(), currentPic.length);
	releaseImage();
	return pic;
}

void JpegRTPReceiver::nextStep()
{
	bool first = true;
	int maxJpegSize = 150000;
	sh_ptr_uch pic = sh_ptr_uch(new unsigned char[maxJpegSize]);
	int picSize = 0;
	while (true)
	{
		DWORD rs = ::WaitForSingleObject( event_abort, 0);
		if (rs == WAIT_OBJECT_0)
		{
			return;
		}
		int res = receivePacket();
		if (res < 0) continue;
		if (first)
		{
			first = false;
			picSize = initJpegHeader(pic, buffer.get() + bufferStartPoint * sizeof(unsigned char));
			bufferStartPoint += 132;
		}
		bufferStartPoint += 8;
		bufferLength -= bufferStartPoint;
		memcpy(pic.get() + picSize, buffer.get() + bufferStartPoint * sizeof(unsigned char), bufferLength);
		picSize += bufferLength;
		if (res == 1) break;
	}
	lockImage();
	currentPic = JpegWrap(pic.get(), picSize);
	//	createNextBmp();
	releaseImage();
	createNextPicture();
	shiftBuffer(bufferStartPoint + bufferLength);
}

int JpegRTPReceiver::initJpegHeader(sh_ptr_uch pic, unsigned char const* firstPacket) const
{
	int width = firstPacket[6];
	int height = firstPacket[7];
	width *= 16;
	height *= 16;
	int place = 0;
	memcpy(pic.get() + place, jpegData, 25 * sizeof(unsigned char));
	place += 25;
	pic.get()[place++] = (unsigned char)(height / 256);
	pic.get()[place++] = (unsigned char)(height % 256);
	pic.get()[place++] = (unsigned char)(width / 256);
	pic.get()[place++] = (unsigned char)(width % 256);
	memcpy(pic.get() + place, jpegData1, 15 * sizeof(unsigned char));
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


void JpegRTPReceiver::stop()
{
	if (!stab_thread) return;

	::SetEvent(event_abort);
	::WaitForSingleObject(stab_thread, INFINITE);
	::CloseHandle(stab_thread);
	stab_thread = NULL;
	started = false;

	sess->EndDataAccess();
	sess->BYEDestroy(RTPTime(10,0),0,0);
#ifdef WIN32
	WSACleanup();
#endif // WIN32

}

void JpegRTPReceiver::shiftBuffer(int place)
{
	for (int i = 0; i < bufferLength - place; ++i)
	{
		buffer.get()[i] = buffer.get()[i + place];
	}
	bufferStartPoint = 0;
	bufferLength -= place;
}

JpegRTPReceiver::~JpegRTPReceiver()
{
	stop();
	lockImage();
	if (lock_image)			::CloseHandle(lock_image);
	if (event_abort)		::CloseHandle(event_abort);
	RegistryParamsSaver saver;
	saver.saveString(_T("JpegRTPReceiver"), _T("CameraIP"), ip);
	saver.saveInt(_T("JpegRTPReceiver"), _T("CameraPort"), port);
}

bool JpegRTPReceiver::isStarted() const
{
	return started;
}

Picture JpegRTPReceiver::nextPic()
{
	lockImage();
	Picture nextPicture = curPic.clone();
	releaseImage();
	return nextPicture;
}


void JpegRTPReceiver::createNextPicture()
{
	JpegWrap frame = getNextFrame();
	if (frame.length == 0) return;
	DWORD tm = GetTickCount();
	CxImage* image = new CxImage(frame.pic.get(), frame.length, CXIMAGE_FORMAT_JPG);
	Picture pic = Picture::createFromCxImage(image);
	delete image;
	//	Capture_Log::getInstance()->log_write("Time is %d\n", GetTickCount() - tm);
	lockImage();
	curPic = pic;
	releaseImage();
}

