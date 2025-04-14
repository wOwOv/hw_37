#ifndef __NETWORK__
#define __NETWORK__

#include "RingBuffer.h"
#include "winsock2.h"

struct Player;
struct SBuffer;
struct Session;
struct SectorAround;


//player에게만 sendQ에 Enqueue
//void SendUnicast(Player* player, SBuffer* buf);

//player제외하고 전체sendQ에 Enqueue
//void SendBroadcast(Player* player, SBuffer* buf);



void networkStartUp();
void networkCleanUp();

void NetworkSelect();

void AcceptProc();
void RecvProc(Player* player);
void SendProc(Player* player);

void SendUnicast(Session* session, SBuffer* buf);
void SendAround(Session* session, SBuffer* buf, bool me = false);
void SendSectorList(Session* session, SectorAround* sectoraround, SBuffer* buf, bool me);
void SendBroadcast(Session* session, SBuffer* buf);

struct Session
{
	SOCKET sock;
	unsigned int Sessionid;
	RingBuffer SendQ;
	RingBuffer RecvQ;
	unsigned int LastRecvTime;
};


#endif
