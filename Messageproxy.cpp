
#include "Messageproxy.h"
#include "Network.h"
#include "SerialBuffer.h"

#include "Protocol.h"


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
void mpSCECHO(unsigned long time, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 4;
	header.type = dfPACKET_SC_ECHO;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << time;
}