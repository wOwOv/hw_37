#ifndef __MessagePROXY__
#define __MessagePROXY__

struct Player;
class SBuffer;

void UProcCreateMe(Player* player,unsigned int id,unsigned char dir,unsigned short x,unsigned short y,unsigned char hp);
void RBProcCreateMe(unsigned int id,unsigned char dir,unsigned short x,unsigned short y,unsigned char hp);
void BProcCreateMe(Player* player,unsigned int id,unsigned char dir,unsigned short x,unsigned short y,unsigned char hp);
void UProcCreateOther(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp);
void RBProcCreateOther(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp);
void BProcCreateOther(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp);
void UProcDelete(Player* player,unsigned int id);
void RBProcDelete(unsigned int id);
void BProcDelete(Player* player,unsigned int id);
void UProcMoveStart(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void RBProcMoveStart(unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void BProcMoveStart(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void UProcMoveStop(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void RBProcMoveStop(unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void BProcMoveStop(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void UProcAttack1(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void RBProcAttack1(unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void BProcAttack1(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void UProcAttack2(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void RBProcAttack2(unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void BProcAttack2(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void UProcAttack3(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void RBProcAttack3(unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void BProcAttack3(Player* player,unsigned int id, unsigned char dir, unsigned short x, unsigned short y);
void UProcDamage(Player* player,unsigned int atk,unsigned int tgt,unsigned char hp);
void RBProcDamage(unsigned int atk,unsigned int tgt,unsigned char hp);
void BProcDamage(Player* player,unsigned int atk,unsigned int tgt,unsigned char hp);

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

//PACKET_SCSYNC 持失
void mpSCSYNC(unsigned int id, unsigned short x, unsigned short y, SBuffer* buf);

//PACKET_SCSYNC 持失
void mpSCECHO(unsigned int time, SBuffer* buf);

#endif