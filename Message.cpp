#include "Message.h"
#include "SerialBuffer.h"
#include "SendCast.h"


void UProcCreateMe(Player* player,unsigned int id,unsigned char dir,unsigned short x,unsigned short y,unsigned char hp)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=10;
unsigned char type=0;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y<<hp;

SendUnicast(player,&buf);
}

void UProcCreateOther(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=10;
unsigned char type=1;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y<<hp;

SendUnicast(player,&buf);
}

void ProcCreateOther(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=10;
unsigned char type=1;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y<<hp;

SendBroadcast(player,&buf);
}

void ProcDelete(Player* player,unsigned int id)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=4;
unsigned char type=2;
buf<<code<<size<<type;
buf<<id;

SendBroadcast(player,&buf);
}

void UProcMoveStart(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=11;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendUnicast(player,&buf);
}

void ProcMoveStart(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=11;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(player,&buf);
}

void ProcMoveStop(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=13;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(player,&buf);
}

void ProcAttack1(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=21;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(player,&buf);
}

void ProcAttack2(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=23;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(player,&buf);
}

void ProcAttack3(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=25;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(player,&buf);
}

void ProcDamage(Player* player,unsigned int atk,unsigned int tgt,unsigned char hp)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=30;
buf<<code<<size<<type;
buf<<atk<<tgt<<hp;

SendBroadcast(nullptr,&buf);
}

