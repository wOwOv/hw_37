#ifndef __MessageSTUB__
#define __MessageSTUB__

struct Player;
struct SBuffer;

bool ProcCreateMe(Player* player,unsigned char dir,unsigned short x,unsigned short y,unsigned char hp);
bool ProcCreateOther(Player* player, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp);
bool ProcDelete(Player* player);
bool ProcMoveStart(Player* player, unsigned char dir, unsigned short x, unsigned short y);
bool ProcMoveStop(Player* player, unsigned char dir, unsigned short x, unsigned short y);
bool ProcAttack1(Player* player, unsigned char dir, unsigned short x, unsigned short y);
bool ProcAttack2(Player* player, unsigned char dir, unsigned short x, unsigned short y);
bool ProcAttack3(Player* player, unsigned char dir, unsigned short x, unsigned short y);
bool ProcDamage(Player* player,unsigned int tgt,unsigned char hp);
bool ProcEcho(Player* player, unsigned int time);
bool MessageProc(Player* player,SBuffer* buf);


#endif