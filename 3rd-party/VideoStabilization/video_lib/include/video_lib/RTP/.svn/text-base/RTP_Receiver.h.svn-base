#pragma once

#include <winsock.h>
#include "Include/define.h"

struct RTP_packet
{
	sh_ptr_uch data;
	int length;
	bool marker;
};

class RtpReceiver
{
	SOCKET sd;							/* Socket descriptor of server */
	struct sockaddr_in server;			/* Information about the server */
	int bufferSize;
	sh_ptr_uch tmpBuffer;
	unsigned short port;
	bool connected;

	bool connect();
public:
	RtpReceiver(int bufferSize = 4096);
	~RtpReceiver();
	RTP_packet receive();
	void setPort(unsigned short port);
	unsigned short getPort() const;
	void disconnect();
};