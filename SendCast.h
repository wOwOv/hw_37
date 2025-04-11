#ifndef __SENDCAST__
#define __SENDCAST__


//player에게만 sendQ에 Enqueue
void SendUnicast(Player* player, SBuffer* buf);

//player제외하고 전체sendQ에 Enqueue
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
