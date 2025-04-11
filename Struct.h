#ifndef __STRUCTDEF__
#define __STRUCTDEF__

#include <winsock2.h>
#include "RingBuffer.h"





struct Player
{
	SOCKET sock;
	IN_ADDR ip;
	u_short port;
	RingBuffer RecvQ;
	RingBuffer SendQ;

	unsigned int id;
	unsigned char direction;
	unsigned short x;
	unsigned short y;
	signed char hp;

	signed char move;
	bool remove;
};

#endif