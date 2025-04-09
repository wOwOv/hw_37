#ifndef __NETWORK__
#define __NETWORK__

#include "SerialBuffer.h"

#define SERVERPORT 20000

struct Session
{
	SOCKET sock;
	SBuffer SendQ;
	SBuffer RecvQ;
	unsigned int LastRecvTime;

	unsigned int setnum;
};




#endif