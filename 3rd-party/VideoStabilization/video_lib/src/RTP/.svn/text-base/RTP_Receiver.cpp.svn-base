#include "stdafx.h"
#include "RTP_Receiver.h"

//#include "Include/capture_log.h"

#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include "Include/RegistryParamsSaver.h"

RtpReceiver::RtpReceiver(int bufferSize1) 
{
	RegistryParamsSaver saver;
	port = (unsigned short)saver.loadInt(_T("JpegRtpUdpReceiver"), _T("CameraPort"), 3334);
	tmpBuffer = sh_ptr_uch(new unsigned char[bufferSize1]);
	bufferSize = bufferSize1;
	connected = false;
}

RtpReceiver::~RtpReceiver()
{
	disconnect();
}

unsigned short RtpReceiver::getPort() const
{
	return port;
}


void RtpReceiver::setPort(unsigned short port1)
{
	port = port1;
	RegistryParamsSaver saver;
	saver.saveInt(_T("JpegRtpUdpReceiver"), _T("CameraPort"), port);
	if (connected)
	{
		disconnect();
	}
}

bool RtpReceiver::connect()
{
	struct hostent *hp;					/* Information about this computer */
	char host_name[256];				/* Name of the server */
	WSADATA w;							/* Used to open windows connection */

	if (WSAStartup(0x0101, &w) != 0)
	{
//		fprintf(stderr, "Could not open Windows connection.\n");
//		Capture_Log::getInstance()->log_write("Connect: fail\n");
		return false;
	}

	/* Open a datagram socket */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd == INVALID_SOCKET)
	{
//		fprintf(stderr, "Could not create socket.\n");
//		Capture_Log::getInstance()->log_write("Connect: fail\n");
		WSACleanup();
		return false;
	}

	memset((void *)&server, '\0', sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	gethostname(host_name, sizeof(host_name));
	hp = gethostbyname(host_name);

	if (hp == NULL)
	{
//		fprintf(stderr, "Could not get host name.\n");
//		Capture_Log::getInstance()->log_write("Connect: fail\n");
		closesocket(sd);
		WSACleanup();
		return false;
	}

	server.sin_addr.S_un.S_un_b.s_b1 = hp->h_addr_list[0][0];
	server.sin_addr.S_un.S_un_b.s_b2 = hp->h_addr_list[0][1];
	server.sin_addr.S_un.S_un_b.s_b3 = hp->h_addr_list[0][2];
	server.sin_addr.S_un.S_un_b.s_b4 = hp->h_addr_list[0][3];

	if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1)
	{
//		fprintf(stderr, "Could not bind name to socket.\n");
//		Capture_Log::getInstance()->log_write("Connect: fail\n");
		closesocket(sd);
		WSACleanup();
		return false;
	}
//	Capture_Log::getInstance()->log_write("Connect: ok\n");
	return true;
}

RTP_packet RtpReceiver::receive()
{
	RTP_packet res;
	if (!connected)
	{
		connected = connect();
		if (!connected)
		{
			res.length = -1;
			res.marker = false;
			return res;
		}
	}
//	}

	int client_length = (int)sizeof(struct sockaddr_in);
	struct sockaddr_in client;			/* Information about the client */

	bool flag = true;

	/* Receive bytes from client */
	int bytes_received = recvfrom(sd, (char*)tmpBuffer.get(), bufferSize, 0, (struct sockaddr *)&client, &client_length);
	if (bytes_received < 0)
	{
		res.length = bytes_received;
		closesocket(sd);
		WSACleanup();
	}
	else
	{
		bytes_received -= 12;
		res.marker = (tmpBuffer.get()[1] & 0x80) == 0 ? false : true;
		static int prevStam = 0;
		int stamp = ((int)tmpBuffer.get()[2]) * 256 + tmpBuffer.get()[3];
		if (prevStam != 0 && stamp != prevStam + 1)
		{
			flag = false;
			//Capture_Log::getInstance()->log_write("Fail: lost packet.\n");
			//Capture_Log::getInstance()->log_write("Stamp is %d\n", stamp);
		}
		prevStam = stamp;
		res.length = bytes_received;
		res.data = sh_ptr_uch(new unsigned char[bytes_received]);
		memcpy(res.data.get(), tmpBuffer.get() + 12, bytes_received);
	}
	if (!flag)
	{
		res.length = -123;
	}
	return res;
}

void RtpReceiver::disconnect()
{
	if (connected)
	{
		closesocket(sd);
		WSACleanup();
		connected = false;
	}
}
