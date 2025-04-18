#ifndef __CONTENTS__
#define __CONTENTS__


#define dfPACKET_CODE		0x89


//-----------------------------------------------------------------
// 30�� �̻��� �ǵ��� �ƹ��� �޽��� ���ŵ� ���°�� ���� ����.
//-----------------------------------------------------------------
#define dfNETWORK_PACKET_RECV_TIMEOUT	30000


//-----------------------------------------------------------------
// ȭ�� �̵� ����.
//-----------------------------------------------------------------
#define dfRANGE_MOVE_TOP	0
#define dfRANGE_MOVE_LEFT	0
#define dfRANGE_MOVE_RIGHT	6400
#define dfRANGE_MOVE_BOTTOM	6400


//---------------------------------------------------------------
// ���ݹ���.
//---------------------------------------------------------------
#define dfATTACK1_RANGE_X		80
#define dfATTACK2_RANGE_X		90
#define dfATTACK3_RANGE_X		100
#define dfATTACK1_RANGE_Y		10
#define dfATTACK2_RANGE_Y		10
#define dfATTACK3_RANGE_Y		20


//---------------------------------------------------------------
// ���� ������.
//---------------------------------------------------------------
#define dfATTACK1_DAMAGE		1
#define dfATTACK2_DAMAGE		2
#define dfATTACK3_DAMAGE		3


//-----------------------------------------------------------------
// ĳ���� �̵� �ӵ�   // 25fps ���� �̵��ӵ�
//-----------------------------------------------------------------
#define dfSPEED_PLAYER_X	6	// 3   50fps
#define dfSPEED_PLAYER_Y	4	// 2   50fps


//-----------------------------------------------------------------
// �̵� ����üũ ����
//-----------------------------------------------------------------
#define dfERROR_RANGE		50

#define SECTOR_HEIGHT		64//100
#define SECTOR_WIDTH		64//100

#define SECTOR_MAX_Y		6400/SECTOR_HEIGHT
#define SECTOR_MAX_X		6400/SECTOR_WIDTH

struct SectorAround;
struct Session;

struct SectorPos
{
	int x;
	int y;

};

struct SectorAround
{
	int count;
	SectorPos around[9];
};

struct Player
{
	Session* session;
	unsigned int id;
	unsigned char direction;
	signed short x;
	signed short y;
	signed char hp;

	signed char move;
	bool remove;

	SectorPos CurSector;
	SectorPos OldSector;
};

void GetSectorAround(int SectorX, int SectorY, SectorAround* sectoraround);
void GetUpdateSectorAround(Player* player, SectorAround* RemoveSec, SectorAround* AddSec);
void GetAttackSectorAround(Player* player, SectorAround* sec);

void AttackPlayer(Player* player, unsigned char type);
void Disconnect(Player* player);

#endif
