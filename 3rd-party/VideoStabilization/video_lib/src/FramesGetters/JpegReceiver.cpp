#include "stdafx.h"
#include "JpegReceiver.h"
#include "Socket/PracticalSocket.h"
#include "Libs/CxImage/ximage.h"
#include "Include/capture_log.h"
#include "Include/different_libs.h"
#include "Include/RegistryParamsSaver.h"

#include <iostream>

JpegReceiver::JpegReceiver():currentPic(0, 0)
{
	started = false;
	RegistryParamsSaver saver;
	this->ip = saver.loadString(_T("JpegReceiver"), _T("CameraIP"), "10.10.76.50");
	bufferCapacity = 3000000;
	buffer = sh_ptr_uch(new unsigned char[bufferCapacity]);
	stab_thread = NULL;
	lock_image = ::CreateEvent(NULL, FALSE, TRUE, NULL);
	event_abort		= ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

void JpegReceiver::receivePacket()
{
	int startPoint = bufferLength + bufferStartPoint;
	int bytesReceived = sock.get()->recv((char*)(buffer.get() + startPoint), bufferCapacity - startPoint);
//	std::string str((char*)(buffer.get() + startPoint), bytesReceived);
	bufferLength += bytesReceived;
}

void JpegReceiver::init()
{
	started = false;
	bufferLength = 0;
	bufferStartPoint = 0;
	sock = sh_ptr_sock(new TCPSocket(ip, 80));
}

std::string const& JpegReceiver::getIp() const
{
	return ip;
}

void JpegReceiver::setIp(std::string const& camIp)
{
	ip = camIp;
}


void JpegReceiver::getParams()
{
	while (bufferLength < headerPrediction)
	{
		receivePacket();
	}
	std::string str((char*)(buffer.get() + bufferStartPoint), headerPrediction);
	std::string boundaryBord = "boundary=";
	int startBoundary = str.find(boundaryBord);
	startBoundary += boundaryBord.length();
	int endBoundary = min(str.find("\n", startBoundary), str.find("\r", startBoundary));
	boundary = str.substr(startBoundary, endBoundary - startBoundary);
}

DWORD WINAPI JpegReceiver::mainProcStarter(LPVOID lpParam)
{
	((JpegReceiver*)lpParam)->mainProc();
	return 0;
}

void JpegReceiver::mainProc()
{
	Capture_Log::getInstance()->enable();
	while (true)
	{
		DWORD res = ::WaitForSingleObject( event_abort, 0);
//		DWORD tm = GetTickCount();
		if (res == WAIT_OBJECT_0)
		{
			break;
		}
		if (!started)
		{
			connectToCamera();
		}
		if (started)
		{
			nextStep();
//			Sleep(20);
		}
//		Capture_Log::getInstance()->log_write("TCP time is %d\n", GetTickCount() - tm);
	}
}

void JpegReceiver::lockImage()
{
	::WaitForSingleObject(lock_image, INFINITE);
}

void JpegReceiver::releaseImage()
{
	::SetEvent(lock_image);
}

bool JpegReceiver::connectToCamera()
{
	char *echoString = "GET /mjpg/video.mjpg HTTP/1.1\r\nHost: 192.168.0.90\r\nUser-Agent: VLC media player - version 1.0.5 Goldeneye - (c) 1996-2010 the VideoLAN team\r\nX-REMOVED: Range\r\nIcy-MetaData: 1\r\nRange: bytes=0-\r\n\r\n";
	int echoStringLen = strlen(echoString);   // Determine input length
	try 
	{
		sock.get()->send(echoString, echoStringLen);
		getParams();
	}
	catch(SocketException &e) 
	{
		std::cerr << e.what() << endl;
		started = false;
		return started;
	}
	started = true;
	return started;
}


bool JpegReceiver::start()
{
	init();
	::ResetEvent(event_abort);
	stab_thread = ::CreateThread(NULL, 0, mainProcStarter, this, 0, NULL);
	return true;

}

JpegWrap JpegReceiver::getNextFrame()
{
	lockImage();
	JpegWrap pic(currentPic.pic.get(), currentPic.length);
	releaseImage();
	return pic;
}

void JpegReceiver::nextStep()
{
	while (bufferLength < headerPrediction)
	{
		receivePacket();
	}
	std::string firstPart((char*)(buffer.get() + bufferStartPoint), headerPrediction);
	std::string lengthBorder = "Content-Length: ";
	int startLen = firstPart.find(lengthBorder) + lengthBorder.length();
	int endLen = startLen;
	while (firstPart.at(endLen) >= '0' && firstPart.at(endLen) <= '9')
	{
		endLen++;
	}
	int jpegLength = atoi(firstPart.substr(startLen, endLen - startLen).c_str());
	while (firstPart.at(endLen) == ' ' || firstPart.at(endLen) == '\n' || firstPart.at(endLen) == '\r')
	{
		++endLen;
	}
	while (bufferLength - endLen < jpegLength)
	{
		receivePacket();
	}
	lockImage();
	currentPic = JpegWrap(buffer.get() + (bufferStartPoint + endLen) * sizeof(unsigned char), jpegLength);
//	createNextBmp();
	releaseImage();
//	createNextPicture();

	//jpegWrap.pic = sh_ptr_uch(new unsigned char[jpegWrap.length]);
	//memcpy(jpegWrap.pic.get(), buffer.get() + (bufferStartPoint + endLen) * sizeof(unsigned char), jpegWrap.length * sizeof(unsigned char));
/*	for (int i = 0; i < jpegLength; ++i)
	{
		data.get()[i] = buffer.get()[bufferStartPoint + endLen + i];
	}*/
	shiftBuffer(bufferStartPoint + endLen + jpegLength);
}

void JpegReceiver::stop()
{
	if (!stab_thread) return;

	::SetEvent(event_abort);
	::WaitForSingleObject(stab_thread, INFINITE);
	::CloseHandle(stab_thread);
	stab_thread = NULL;
	started = false;
}

void JpegReceiver::shiftBuffer(int place)
{
	for (int i = 0; i < bufferLength - place; ++i)
	{
		buffer.get()[i] = buffer.get()[i + place];
	}
	bufferStartPoint = 0;
	bufferLength -= place;
}

JpegReceiver::~JpegReceiver()
{
	stop();
	lockImage();
	if (lock_image)			::CloseHandle(lock_image);
	if (event_abort)		::CloseHandle(event_abort);
	RegistryParamsSaver saver;
	saver.saveString(_T("JpegReceiver"), _T("CameraIP"), ip);
}

bool JpegReceiver::isStarted() const
{
	return started;
}

Picture JpegReceiver::nextPic()
{
	createNextPicture();
	lockImage();
	Picture nextPicture = curPic.clone();
	releaseImage();
	return nextPicture;
}


void JpegReceiver::createNextPicture()
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

