
#include "Messageproxy.h"
#include "Network.h"
#include "SerialBuffer.h"

#include "Protocol.h"


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

void RBProcCreateMe(unsigned int id,unsigned char dir,unsigned short x,unsigned short y,unsigned char hp)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=10;
unsigned char type=0;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y<<hp;

SendBroadcast(nullptr,&buf);
}

void BProcCreateMe(Player* player,unsigned int id,unsigned char dir,unsigned short x,unsigned short y,unsigned char hp)
{
SBuffer buf;
unsigned char code = 0x89;
unsigned char size=10;
unsigned char type=0;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y<<hp;

SendBroadcast(player,&buf);
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

void RBProcCreateOther(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=10;
unsigned char type=1;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y<<hp;

SendBroadcast(nullptr,&buf);
}

void BProcCreateOther(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=10;
unsigned char type=1;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y<<hp;

SendBroadcast(player,&buf);
}

void UProcDelete(Player* player,unsigned int id)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=4;
unsigned char type=2;
buf<<code<<size<<type;
buf<<id;

SendUnicast(player,&buf);
}

void RBProcDelete(unsigned int id)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=4;
unsigned char type=2;
buf<<code<<size<<type;
buf<<id;

SendBroadcast(nullptr,&buf);
}

void BProcDelete(Player* player,unsigned int id)
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

void RBProcMoveStart(unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=11;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(nullptr,&buf);
}

void BProcMoveStart(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=11;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(player,&buf);
}

void UProcMoveStop(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=13;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendUnicast(player,&buf);
}

void RBProcMoveStop(unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=13;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(nullptr,&buf);
}

void BProcMoveStop(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=13;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(player,&buf);
}

void UProcAttack1(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=21;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendUnicast(player,&buf);
}

void RBProcAttack1(unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=21;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(nullptr,&buf);
}

void BProcAttack1(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=21;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(player,&buf);
}

void UProcAttack2(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=23;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendUnicast(player,&buf);
}

void RBProcAttack2(unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=23;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(nullptr,&buf);
}

void BProcAttack2(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=23;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(player,&buf);
}

void UProcAttack3(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=25;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendUnicast(player,&buf);
}

void RBProcAttack3(unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=25;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(nullptr,&buf);
}

void BProcAttack3(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=25;
buf<<code<<size<<type;
buf<<id<<dir<<x<<y;

SendBroadcast(player,&buf);
}

void UProcDamage(Player* player,unsigned int atk,unsigned int tgt,unsigned char hp)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=30;
buf<<code<<size<<type;
buf<<atk<<tgt<<hp;

SendUnicast(player,&buf);
}

void RBProcDamage(unsigned int atk,unsigned int tgt,unsigned char hp)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=30;
buf<<code<<size<<type;
buf<<atk<<tgt<<hp;

SendBroadcast(nullptr,&buf);
}

void BProcDamage(Player* player,unsigned int atk,unsigned int tgt,unsigned char hp)
{
SBuffer buf;
unsigned char code=0x89;
unsigned char size=9;
unsigned char type=30;
buf<<code<<size<<type;
buf<<atk<<tgt<<hp;

SendBroadcast(player,&buf);
}

//PACKET_SCCREATEME 持失
void mpSCCREATEME(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 10;
	header.type = dfPACKET_SC_CREATE_MY_CHARACTER;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y << hp;
}

//PACKET_SCCREATEOTHER 持失
void mpSCCREATEOTHER(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 10;
	header.type = dfPACKET_SC_CREATE_OTHER_CHARACTER;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y << hp;
}

//PACKET_SCDELETE 持失
void mpSCDELETE(unsigned int id, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 4;
	header.type = dfPACKET_SC_DELETE_CHARACTER;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id;
}


//PACKET_SCMOVESTART 持失
void mpSCMOVESTART(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 9;
	header.type = dfPACKET_SC_MOVE_START;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y;
}

//PACKET_SCMOVESTOP 持失
void mpSCMOVESTOP(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 9;
	header.type = dfPACKET_SC_MOVE_STOP;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y;
}


//PACKET_SCATTACK1 持失
void mpSCATTACK1(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 9;
	header.type = dfPACKET_SC_ATTACK1;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y;
}

//PACKET_SCATTACK2 持失
void mpSCATTACK2(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 9;
	header.type = dfPACKET_SC_ATTACK2;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y;
}

//PACKET_SCATTACK3 持失
void mpSCATTACK3(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 9;
	header.type = dfPACKET_SC_ATTACK3;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y;
}

//PACKET_SCDAMAGE 持失
void mpSCDAMAGE(unsigned int atk, unsigned int tgt, unsigned char hp, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 9;
	header.type = dfPACKET_SC_DAMAGE;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << atk << tgt << hp;
}

//PACKET_SCSYNC 持失
void mpSCSYNC(unsigned int id, unsigned short x, unsigned short y, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 8;
	header.type = dfPACKET_SC_SYNC;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << x << y;
}

//PACKET_SCSYNC 持失
void mpSCECHO(unsigned int time, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 4;
	header.type = dfPACKET_SC_ECHO;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << time;
}