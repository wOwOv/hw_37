#ifndef __MessagePROXY__
#define __MessagePROXY__

class SBuffer;


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

//PACKET_SCSYNC ����
void mpSCSYNC(unsigned int id, unsigned short x, unsigned short y, SBuffer* buf);

//PACKET_SCSYNC ����
void mpSCECHO(unsigned int time, SBuffer* buf);

#endif