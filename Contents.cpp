#include "Contents.h"
#include "SerialBuffer.h"
#include "Protocol.h"
#include "System.h"
#include "SerialBuffer.h"
#include <stdlib.h>

bool PacketProc_MoveStart(Player* player, SBuffer* buf)
{
	unsigned char dir;
	unsigned short x;
	unsigned short y;
	*buf >> dir >> x >> y;

	if (abs(player->x - x) > dfERROR_RANGE || abs(player->y - y) > dfERROR_RANGE)
	{
		Disconnect(player);
	}
	else
	{
		//player 움직임 정보 변경
		switch (dir)
		{
		case dfPACKET_MOVE_DIR_LL:
		case dfPACKET_MOVE_DIR_LU:
		case dfPACKET_MOVE_DIR_LD:
			player->direction = dfPACKET_MOVE_DIR_LL;
			break;
		case dfPACKET_MOVE_DIR_RR:
		case dfPACKET_MOVE_DIR_RU:
		case dfPACKET_MOVE_DIR_RD:
			player->direction = dfPACKET_MOVE_DIR_RR;
			break;
		}
		player->move = dir;
		player->x = x;
		player->y = y;

		_LOG(LOG_LEVEL_DEBUG,L"Recv CSMOVESTART id:%d direction:%d x:%d y:%d \n", player->id, dir, x, y);

		//해당 플레이어에 대한 움직임 정보 본인 제외 전체에게 send
		buf->Clear();
		mpSCMOVESTART(player->id, player->move, player->x, player->y, buf);

		SendBroadcast(player, buf);
		buf->MoveReadPos(sizeof(HEADER));
		unsigned int mid = 0;
		unsigned char mdir;
		unsigned short mx;
		unsigned short my;
		*buf >> mid >> mdir >> mx >> my;
		_LOG(LOG_LEVEL_DEBUG, L"broadcast SCMOVESTART id:%d direction:%d x:%d y:%d \n", mid, mdir, mx, my);
	}
	return true;
}
bool PacketProc_MoveStop(Player* player, SBuffer* buf)
{
	unsigned char dir;
	unsigned short x;
	unsigned short y;
	*buf >> dir >> x >> y;

	if (abs(player->x - x) > dfERROR_RANGE || abs(player->y - y) > dfERROR_RANGE)
	{
		Disconnect(player);
	}
	else
	{
		//player 움직임 정보 변경
		switch (dir)
		{
		case dfPACKET_MOVE_DIR_LL:
		case dfPACKET_MOVE_DIR_LU:
		case dfPACKET_MOVE_DIR_LD:
			player->direction = dfPACKET_MOVE_DIR_LL;
			break;
		case dfPACKET_MOVE_DIR_RR:
		case dfPACKET_MOVE_DIR_RU:
		case dfPACKET_MOVE_DIR_RD:
			player->direction = dfPACKET_MOVE_DIR_RR;
			break;
		}
		player->move = -1;
		player->x = x;
		player->y = y;

		_LOG(LOG_LEVEL_DEBUG, L"Recv CSMOVESTOP id:%d direction:%d x:%d y:%d \n", player->id, dir, x, y);

		//해당 플레이어에 대한 움직임 정보 본인 제외 전체에게 send
		buf->Clear();
		mpSCMOVESTOP(player->id, dir, player->x, player->y, buf);

		SendBroadcast(player, buf);
		buf->MoveReadPos(sizeof(HEADER));
		unsigned int mid = 0;
		unsigned char mdir;
		unsigned short mx;
		unsigned short my;
		*buf >> mid >> mdir >> mx >> my;
		_LOG(LOG_LEVEL_DEBUG, L"broadcast SCMOVESTOP id:%d direction:%d x:%d y:%d \n", mid, mdir, mx, my);
	}
	return true;
}

bool PacketProc_Attack1(Player* player, SBuffer* buf)
{
	unsigned char dir;
	unsigned short x;
	unsigned short y;
	*buf >> dir >> x >> y;

	if (abs(player->x - x) > dfERROR_RANGE || abs(player->y - y) > dfERROR_RANGE)
	{
		Disconnect(player);
	}
	else
	{
		//해당 플레이어 공격 정보 처리
		player->direction = dir;
		player->x = x;
		player->y = y;

		//해당 플레이어 공격 정보 send()
		buf->Clear();

		mpSCATTACK1(player->id, player->direction, player->x, player->y, buf);

		SendBroadcast(player, buf);

		buf->MoveReadPos(sizeof(HEADER));
		unsigned int mid = 0;
		unsigned char mdir = 0;
		unsigned short mx = 0;
		unsigned short my = 0;
		_LOG(LOG_LEVEL_DEBUG, L"broadcast attack1 id: %d direction: %d x: %d y: %d\n", mid, mdir, mx, my);

		//데미지처리
		AttackPlayer(player, dfPACKET_CS_ATTACK1);

	}
	return true;
}
bool PacketProc_Attack2(Player* player, SBuffer* buf)
{
	unsigned char dir;
	unsigned short x;
	unsigned short y;
	*buf >> dir >> x >> y;

	if (abs(player->x - x) > dfERROR_RANGE || abs(player->y - y) > dfERROR_RANGE)
	{
		Disconnect(player);
	}
	else
	{
		//해당 플레이어 공격 정보 처리
		player->direction = dir;
		player->x = x;
		player->y = y;

		//해당 플레이어 공격 정보 send()
		buf->Clear();

		mpSCATTACK2(player->id, player->direction, player->x, player->y, buf);

		SendBroadcast(player, buf);

		buf->MoveReadPos(sizeof(HEADER));
		unsigned int mid = 0;
		unsigned char mdir = 0;
		unsigned short mx = 0;
		unsigned short my = 0;
		_LOG(LOG_LEVEL_DEBUG, L"broadcast attack2 id: %d direction: %d x: %d y: %d\n", mid, mdir, mx, my);

		//데미지처리
		AttackPlayer(player, dfPACKET_CS_ATTACK2);

	}
	return true;
}
bool PacketProc_Attack3(Player* player, SBuffer* buf)
{
	unsigned char dir;
	unsigned short x;
	unsigned short y;
	*buf >> dir >> x >> y;

	if (abs(player->x - x) > dfERROR_RANGE || abs(player->y - y) > dfERROR_RANGE)
	{
		Disconnect(player);
	}
	else
	{
		//해당 플레이어 공격 정보 처리
		player->direction = dir;
		player->x = x;
		player->y = y;

		//해당 플레이어 공격 정보 send()
		buf->Clear();

		mpSCATTACK3(player->id, player->direction, player->x, player->y, buf);

		SendBroadcast(player, buf);

		buf->MoveReadPos(sizeof(HEADER));
		unsigned int mid = 0;
		unsigned char mdir = 0;
		unsigned short mx = 0;
		unsigned short my = 0;
		_LOG(LOG_LEVEL_DEBUG, L"broadcast attack1 id: %d direction: %d x: %d y: %d\n", mid, mdir, mx, my);

		//데미지처리
		AttackPlayer(player, dfPACKET_CS_ATTACK3);

	}
	return true;
}

void Disconnect(Player* player)
{
	//list에서 삭제 예정 표시
	player->remove = true;

	//player 삭제 메시지 Broadcast
	SBuffer buf;
	mpSCDELETE(player->id, &buf);

	SendBroadcast(player, &buf);
}

//특정 섹터 1개에 보내기
void SendSectorOne(int SectorX, int SectorY, SBuffer* buf,Session* session){}
//특정 1명의 클라에 보내기
void SendUnicast(Session* session, SBuffer* buf, bool me = false) {}
//클라 기준 주변 섹터에 보내기
void SendAround(Session* session, SBuffer* buf) {}
//진짜 브로드캐스팅
void SendBroadcast(Session* session, SBuffer* buf) {}
