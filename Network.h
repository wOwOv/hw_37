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

//PACKET_SCCREATEME ����
void mpSCCREATEME(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp, SBuffer* buf);

//PACKET_SCCREATEOTHER ����
void mpSCCREATEOTHER(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp, SBuffer* buf);

//PACKET_SCDELETE ����
void mpSCDELETE(unsigned int id, SBuffer* buf);


//PACKET_SCMOVESTART ����
void mpSCMOVESTART(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf);

//PACKET_SCMOVESTOP ����
void mpSCMOVESTOP(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf);

//PACKET_SCATTACK1 ����
void mpSCATTACK1(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf);
//PACKET_SCATTACK2 ����
void mpSCATTACK2(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf);

//PACKET_SCATTACK3 ����
void mpSCATTACK3(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf);

//PACKET_SCDAMAGE ����
void mpSCDAMAGE(unsigned int atk, unsigned int tgt, unsigned char hp, SBuffer* buf);

//PACKET_SCDAMAGE ����
void mpSCSYNC(unsigned int id, unsigned short x, unsigned short y, SBuffer* buf);

//PACKET_SCDAMAGE ����
void mpSCECHO(unsigned int time, SBuffer* buf);


void SendSectorOne(int SectorX, int SectorY, SBuffer* buf, Session* exceptsession);
void SendUnicast(Session* session, SBuffer* buf);
void SendAround(Session* session, SBuffer* buf, bool me = false);
void SendSectorList(SectorAround* sectoraround, SBuffer* buf, bool me = false);
void SendBroadcast(Session* session, SBuffer* buf);
#endif