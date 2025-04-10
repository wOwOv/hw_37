#ifndef __NETWORK__
#define __NETWORK__

#include "RingBuffer.h"
#include <winsock2.h>

#define SERVERPORT 20000

class SBuffer;

struct Session
{
	SOCKET sock;
	unsigned int Sessionid;
	RingBuffer SendQ;
	RingBuffer RecvQ;
	unsigned int LastRecvTime;

	unsigned int setnum;
};

void netStartUp();

bool PacketProc(Session* session, SBuffer* buf);

//PACKET_SCCREATEME 持失
void mpSCCREATEME(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp, SBuffer* buf);

//PACKET_SCCREATEOTHER 持失
void mpSCCREATEOTHER(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp, SBuffer* buf);

//PACKET_SCDELETE 持失
void mpSCDELETE(unsigned int id, SBuffer* buf);


//PACKET_SCMOVESTART 持失
void mpSCMOVESTART(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf);

//PACKET_SCMOVESTOP 持失
void mpSCMOVESTOP(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf);

//PACKET_SCATTACK1 持失
void mpSCATTACK1(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf);
//PACKET_SCATTACK2 持失
void mpSCATTACK2(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf);

//PACKET_SCATTACK3 持失
void mpSCATTACK3(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf);

//PACKET_SCDAMAGE 持失
void mpSCDAMAGE(unsigned int atk, unsigned int tgt, unsigned char hp, SBuffer* buf);

//PACKET_SCDAMAGE 持失
void mpSCSYNC(unsigned int id, unsigned short x, unsigned short y, SBuffer* buf);

//PACKET_SCDAMAGE 持失
void mpSCECHO(unsigned int time, SBuffer* buf);

#endif