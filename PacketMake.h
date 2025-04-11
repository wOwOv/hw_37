#ifndef __PACKETMAKE__
#define __PACKETMAKE__


struct Player;
struct SBuffer;

//PACKET_SCCREATEME 持失
void mpSCCREATEME( unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp,SBuffer* buf);

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




#endif