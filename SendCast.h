#ifndef __SENDCAST__
#define __SENDCAST__


//player���Ը� sendQ�� Enqueue
void SendUnicast(Player* player, SBuffer* buf);

//player�����ϰ� ��üsendQ�� Enqueue
void SendBroadcast(Player* player, SBuffer* buf);

//Disconnect
void Disconnect(Player* player);

void AttackPlayer(Player* player, unsigned char type);

void networkStartUp();
void networkCleanUp();

void NetworkSelect();

void AcceptProc();
void RecvProc(Player* player);
void SendProc(Player* player);
#endif
