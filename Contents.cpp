#include "SerialBuffer.h"
#include "Contents.h"
#include "Protocol.h"
#include "System.h"
#include "SerialBuffer.h"
#include <stdlib.h>
#include <unordered_map>
#include <list>

//Player 객체 메인 관리
std::unordered_map<unsigned int, Player*> PlayerMap;

//월드맵 Player 섹터
std::list<Player*> Sector[SECTOR_MAX_Y][SECTOR_MAX_X];


bool PacketProc_MoveStart(Session* session, SBuffer* buf)
{
	unsigned char dir;
	unsigned short x;
	unsigned short y;
	*buf >> dir >> x >> y;

	_LOG(LOG_LEVEL_DEBUG, L"#MoveStart# SessionID:%d / Direction:%d / X:%d / Y:%d ", session->Sessionid, dir, x, y);

	//sessionid로 player 찾기
	Player* player = PlayerMap.find(session->Sessionid)->second;
	if (player == NULL)
	{
		_LOG(LOG_LEVEL_ERROR, L"#MoveStart#>SessionID: %d Player Not Found!", session->Sessionid);
		return false;
	}


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
bool PacketProc_MoveStop(Session* session, SBuffer* buf)
{
	unsigned char dir;
	unsigned short x;
	unsigned short y;
	*buf >> dir >> x >> y;

	_LOG(LOG_LEVEL_DEBUG, L"#MoveStop# SessionID:%d / Direction:%d / X:%d / Y:%d ", session->Sessionid, dir, x, y);

	//sessionid로 player 찾기
	Player* player = PlayerMap.find(session->Sessionid)->second;
	if (player == NULL)
	{
		_LOG(LOG_LEVEL_ERROR, L"#MoveStop#>SessionID: %d Player Not Found!", session->Sessionid);
		return false;
	}

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

bool PacketProc_Attack1(Session* session, SBuffer* buf)
{
	unsigned char dir;
	unsigned short x;
	unsigned short y;
	*buf >> dir >> x >> y;

	_LOG(LOG_LEVEL_DEBUG, L"#Attack1# SessionID:%d / Direction:%d / X:%d / Y:%d ", session->Sessionid, dir, x, y);

	//sessionid로 player 찾기
	Player* player = PlayerMap.find(session->Sessionid)->second;
	if (player == NULL)
	{
		_LOG(LOG_LEVEL_ERROR, L"#Attack1#>SessionID: %d Player Not Found!", session->Sessionid);
		return false;
	}

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
bool PacketProc_Attack2(Session* session, SBuffer* buf)
{
	unsigned char dir;
	unsigned short x;
	unsigned short y;
	*buf >> dir >> x >> y;

	_LOG(LOG_LEVEL_DEBUG, L"#Attack2# SessionID:%d / Direction:%d / X:%d / Y:%d ", session->Sessionid, dir, x, y);

	//sessionid로 player 찾기
	Player* player = PlayerMap.find(session->Sessionid)->second;
	if (player == NULL)
	{
		_LOG(LOG_LEVEL_ERROR, L"#Attack2#>SessionID: %d Player Not Found!", session->Sessionid);
		return false;
	}

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
bool PacketProc_Attack3(Session* session, SBuffer* buf)
{
	unsigned char dir;
	unsigned short x;
	unsigned short y;
	*buf >> dir >> x >> y;

	_LOG(LOG_LEVEL_DEBUG, L"#Attack3# SessionID:%d / Direction:%d / X:%d / Y:%d ", session->Sessionid, dir, x, y);

	//sessionid로 player 찾기
	Player* player = PlayerMap.find(session->Sessionid)->second;
	if (player == NULL)
	{
		_LOG(LOG_LEVEL_ERROR, L"#Attack3#>SessionID: %d Player Not Found!", session->Sessionid);
		return false;
	}

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

bool PacketProc_Echo(Session* session, SBuffer* buf)
{
	unsigned int time;
	*buf >> time;

	_LOG(LOG_LEVEL_DEBUG, L"#Echo# SessionID:%d / Time:%d", session->Sessionid, time);

	//sessionid로 player 찾기
	Player* player = PlayerMap.find(session->Sessionid)->second;
	if (player == NULL)
	{
		_LOG(LOG_LEVEL_ERROR, L"#Echo#>SessionID: %d Player Not Found!", session->Sessionid);
		return false;
	}


		mpSCECHO(time, buf);

		SendBroadcast(player, buf);

		buf->MoveReadPos(sizeof(HEADER));
		unsigned int mid = 0;
		unsigned char mdir = 0;
		unsigned short mx = 0;
		unsigned short my = 0;
		_LOG(LOG_LEVEL_DEBUG, L"broadcast echo id: %d direction: %d x: %d y: %d\n", mid, mdir, mx, my);

	return true;
}



void Disconnect(Player* player)
{
	//list에서 삭제 예정 표시
	player->remove = true;

	//player 삭제 메시지 Broadcast
	SBuffer buf;
	mpSCDELETE(player->id, &buf);

	Send
}

void GetSectorAround(int SectorX, int SectorY, SectorAround* sectoraround)
{
	sectoraround->count = 0;
	if (SectorX > 0)
	{
		//↖
		if (SectorY > 0)
		{
			sectoraround->around[sectoraround->count].x = SectorX - 1;
			sectoraround->around[sectoraround->count].y = SectorY - 1;
			sectoraround->count++;
		}

		//↙
		if (SectorY < SECTOR_MAX_Y-1)
		{
			sectoraround->around[sectoraround->count].x = SectorX - 1;
			sectoraround->around[sectoraround->count].y = SectorY + 1;
			sectoraround->count++;
		}

		//⭠
		sectoraround->around[sectoraround->count].x = SectorX - 1;
		sectoraround->around[sectoraround->count].y = SectorY;
		sectoraround->count++;
	}

	//⭡
	if (SectorY > 0)
	{
		sectoraround->around[sectoraround->count].x = SectorX;
		sectoraround->around[sectoraround->count].y = SectorY - 1;
		sectoraround->count++;
	}
	//⭣
	if (SectorY < SECTOR_MAX_Y - 1)
	{
		sectoraround->around[sectoraround->count].x = SectorX;
		sectoraround->around[sectoraround->count].y = SectorY + 1;
		sectoraround->count++;
	}

	if (SectorX < SECTOR_MAX_X-1)
	{
		//↗
		if (SectorY > 0)
		{
			sectoraround->around[sectoraround->count].x = SectorX + 1;
			sectoraround->around[sectoraround->count].y = SectorY - 1;
			sectoraround->count++;
		}

		//↘
		if (SectorY < SECTOR_MAX_Y - 1)
		{
			sectoraround->around[sectoraround->count].x = SectorX + 1;
			sectoraround->around[sectoraround->count].y = SectorY + 1;
			sectoraround->count++;
		}

		//⭢
		sectoraround->around[sectoraround->count].x = SectorX + 1;
		sectoraround->around[sectoraround->count].y = SectorY;
		sectoraround->count++;
	}

	//본인이 있는 섹터
	sectoraround->around[sectoraround->count].x = SectorX;
	sectoraround->around[sectoraround->count].y = SectorY;
	sectoraround->count++;


}

void GetUpdateSectorAround(Player* player, SectorAround* RemoveSec, SectorAround* AddSec)
{
	if (player->CurSector.x < player->OldSector.x)
	{
		//이동방향 ↖
		if (player->CurSector.y < player->OldSector.y)
		{
			RemoveSec->count = 0;
			if (player->OldSector.x < SECTOR_MAX_X - 1)
			{
				//↗
				if (player->OldSector.y > 0)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
					RemoveSec->count++;
				}

				//↘
				if (player->OldSector.y < SECTOR_MAX_Y - 1)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
					RemoveSec->count++;
				}

				//⭢
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y ;
				RemoveSec->count++;
			}
			//⭣
			if (player->OldSector.y < SECTOR_MAX_Y - 1)
			{
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
				RemoveSec->count++;
			}

			//↙
			if (player->OldSector.x > 0)
			{
				if (player->OldSector.y < SECTOR_MAX_Y - 1)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
					RemoveSec->count++;
				}
			}

			AddSec->count = 0;
			if (player->CurSector.x > 0)
			{
			//↖
				if (player->CurSector.y > 0)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x - 1;
					AddSec->around[AddSec->count].y = player->CurSector.y - 1;
					AddSec->count++;
				}
			//↙
				if (player->CurSector.y < SECTOR_MAX_Y - 1)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x - 1;
					AddSec->around[AddSec->count].y = player->CurSector.y + 1;
					AddSec->count++;
				}
			
			//⭠
				AddSec->around[AddSec->count].x = player->CurSector.x - 1;
				AddSec->around[AddSec->count].y = player->CurSector.y;
				AddSec->count++;
			}

			//⭡
			if (player->CurSector.y > 0)
			{
				AddSec->around[AddSec->count].x = player->CurSector.x;
				AddSec->around[AddSec->count].y = player->CurSector.y - 1;
				AddSec->count++;
			}

			//↗
			if (player->CurSector.x < SECTOR_MAX_X - 1)
			{
				if (player->CurSector.y > 0)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x + 1;
					AddSec->around[AddSec->count].y = player->CurSector.y - 1;
					AddSec->count++;
				}
			}


			return;
		}


		//이동방향 ↙
		if(player->CurSector.y>player->OldSector.y)
		{
			RemoveSec->count = 0;
			if (player->OldSector.x < SECTOR_MAX_X - 1)
			{
				//↗
				if (player->OldSector.y > 0)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
					RemoveSec->count++;
				}

				//↘
				if (player->OldSector.y < SECTOR_MAX_Y - 1)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
					RemoveSec->count++;
				}

				//⭢
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y;
				RemoveSec->count++;
			}
			//⭡
			if (player->OldSector.y >0)
			{
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
				RemoveSec->count++;
			}

			//↖
			if (player->OldSector.x > 0)
			{
				if (player->OldSector.y >0)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
					RemoveSec->count++;
				}
			}

			AddSec->count = 0;
			if (player->CurSector.x > 0)
			{
				//↖
				if (player->CurSector.y > 0)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x - 1;
					AddSec->around[AddSec->count].y = player->CurSector.y - 1;
					AddSec->count++;
				}
				//↙
				if (player->CurSector.y < SECTOR_MAX_Y - 1)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x - 1;
					AddSec->around[AddSec->count].y = player->CurSector.y + 1;
					AddSec->count++;
				}

				//⭠
				AddSec->around[AddSec->count].x = player->CurSector.x - 1;
				AddSec->around[AddSec->count].y = player->CurSector.y;
				AddSec->count++;
			}

			//⭣
			if (player->CurSector.y < SECTOR_MAX_Y-1)
			{
				AddSec->around[AddSec->count].x = player->CurSector.x;
				AddSec->around[AddSec->count].y = player->CurSector.y + 1;
				AddSec->count++;
			}

			//↘
			if (player->CurSector.x < SECTOR_MAX_X - 1)
			{
				if (player->CurSector.y < SECTOR_MAX_Y-1)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x + 1;
					AddSec->around[AddSec->count].y = player->CurSector.y + 1;
					AddSec->count++;
				}
			}

			return;
		}


		//이동방향 ⭠
		{
			RemoveSec->count = 0;
			if (player->OldSector.x < SECTOR_MAX_X - 1)
			{
			//↗
				if (player->OldSector.y > 0)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
					RemoveSec->count++;
				}
			//↘
				if (player->OldSector.y < SECTOR_MAX_Y - 1)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
					RemoveSec->count++;
				}
			//⭢
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y;
				RemoveSec->count++;

			}

			AddSec->count = 0;
			if (player->CurSector.x > 0)
			{
			//↖
			if(player->CurSector.y>0)
			{
				AddSec->around[AddSec->count].x = player->CurSector.x - 1;
				AddSec->around[AddSec->count].y = player->CurSector.y - 1;
				AddSec->count++;
			}
			//↙
			if (player->CurSector.y < SECTOR_MAX_Y - 1)
			{
				AddSec->around[AddSec->count].x = player->CurSector.x - 1;
				AddSec->around[AddSec->count].y = player->CurSector.y + 1;
				AddSec->count++;
			}
			//⭠
			AddSec->around[AddSec->count].x = player->CurSector.x - 1;
			AddSec->around[AddSec->count].y = player->CurSector.y;
			AddSec->count++;

			}
			return;
		}

	}
	//이동방향 ⭡
	if (player->CurSector.y < player->OldSector.y)
	{
		RemoveSec->count = 0;
		if (player->OldSector.y < SECTOR_MAX_Y - 1)
		{
			//↙
			if (player->OldSector.x > 0)
			{
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
				RemoveSec->count++;
			}
			//↘
			if (player->OldSector.x < SECTOR_MAX_X - 1)
			{
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
				RemoveSec->count++;
			}
			//⭣
			RemoveSec->around[RemoveSec->count].x = player->OldSector.x;
			RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
			RemoveSec->count++;

		}

		AddSec->count = 0;
		if (player->CurSector.y > 0)
		{
			//↖
			if (player->CurSector.x > 0)
			{
				AddSec->around[AddSec->count].x = player->CurSector.x - 1;
				AddSec->around[AddSec->count].y = player->CurSector.y - 1;
				AddSec->count++;
			}
			//↗
			if (player->CurSector.x < SECTOR_MAX_X - 1)
			{
				AddSec->around[AddSec->count].x = player->CurSector.x + 1;
				AddSec->around[AddSec->count].y = player->CurSector.y - 1;
				AddSec->count++;
			}
			//⭡
			AddSec->around[AddSec->count].x = player->CurSector.x;
			AddSec->around[AddSec->count].y = player->CurSector.y - 1;
			AddSec->count++;

		}
		return;
	}

	//이동방향 ⭣
	if (player->CurSector.y > player->OldSector.y)
	{
		RemoveSec->count = 0;
		if (player->OldSector.y > 0)
		{
			//↖
			if (player->OldSector.x > 0)
			{
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
				RemoveSec->count++;
			}
			//↗
			if (player->OldSector.x < SECTOR_MAX_X - 1)
			{
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
				RemoveSec->count++;
			}
			//⭡
			RemoveSec->around[RemoveSec->count].x = player->OldSector.x;
			RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
			RemoveSec->count++;

		}

		AddSec->count = 0;
		if (player->CurSector.y < SECTOR_MAX_Y - 1)
		{
			//↙
			if (player->CurSector.x > 0)
			{
				AddSec->around[AddSec->count].x = player->CurSector.x - 1;
				AddSec->around[AddSec->count].y = player->CurSector.y + 1;
				AddSec->count++;
			}
			//↘
			if (player->CurSector.x < SECTOR_MAX_X - 1)
			{
				AddSec->around[AddSec->count].x = player->OldSector.x + 1;
				AddSec->around[AddSec->count].y = player->OldSector.y + 1;
				AddSec->count++;
			}
			//⭣
			AddSec->around[AddSec->count].x = player->CurSector.x;
			AddSec->around[AddSec->count].y = player->CurSector.y + 1;
			AddSec->count++;

		}
		return;
	}


	if (player->CurSector.x > player->OldSector.x)
	{
		//이동방향 ↗
		if(player->CurSector.y<player->CurSector.y)
		{
			RemoveSec->count = 0;
			if (player->OldSector.x > 0)
			{
				//↖
				if (player->OldSector.y > 0)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
					RemoveSec->count++;
				}
				//↙
				if (player->OldSector.y < SECTOR_MAX_Y - 1)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
					RemoveSec->count++;
				}

				//⭠
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y;
				RemoveSec->count++;
			}

			//⭣
			if (player->OldSector.y < SECTOR_MAX_Y - 1)
			{
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
				RemoveSec->count++;
			}

			//↘
			if (player->OldSector.x < SECTOR_MAX_X - 1)
			{
				if (player->OldSector.y < SECTOR_MAX_Y - 1)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
					RemoveSec->count++;
				}
			}

			AddSec->count = 0;
			if (player->CurSector.x < SECTOR_MAX_X - 1)
			{
				//↗
				if (player->CurSector.y > 0)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x + 1;
					AddSec->around[AddSec->count].y = player->CurSector.y - 1;
					AddSec->count++;
				}

				//↘
				if (player->CurSector.y < SECTOR_MAX_Y - 1)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x + 1;
					AddSec->around[AddSec->count].y = player->CurSector.y + 1;
					AddSec->count++;
				}

				//⭢
				AddSec->around[AddSec->count].x = player->CurSector.x + 1;
				AddSec->around[AddSec->count].y = player->CurSector.y;
				AddSec->count++;
			}
			//⭡
			if (player->CurSector.y > 0)
			{
				AddSec->around[AddSec->count].x = player->CurSector.x;
				AddSec->around[AddSec->count].y = player->CurSector.y - 1;
				AddSec->count++;
			}

			//↖
			if (player->CurSector.x > 0)
			{
				if (player->CurSector.y > 0)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x - 1;
					AddSec->around[AddSec->count].y = player->CurSector.y - 1;
					AddSec->count++;
				}
			}

			return;
		}


		//이동방향 ↘
		if (player->CurSector.y > player->OldSector.y)
		{
			RemoveSec->count = 0;
			if (player->OldSector.x > 0)
			{
				//↖
				if (player->OldSector.y > 0)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
					RemoveSec->count++;
				}
				//↙
				if (player->OldSector.y < SECTOR_MAX_Y - 1)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
					RemoveSec->count++;
				}

				//⭠
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y;
				RemoveSec->count++;
			}

			//⭡
			if (player->OldSector.y > 0)
			{
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
				RemoveSec->count++;
			}

			//↗
			if (player->OldSector.x < SECTOR_MAX_X - 1)
			{
				if (player->OldSector.y > 0)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x + 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
					RemoveSec->count++;
				}
			}

			AddSec->count = 0;
			if (player->CurSector.x < SECTOR_MAX_X - 1)
			{
				//↗
				if (player->CurSector.y > 0)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x + 1;
					AddSec->around[AddSec->count].y = player->CurSector.y - 1;
					AddSec->count++;
				}

				//↘
				if (player->CurSector.y < SECTOR_MAX_Y - 1)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x + 1;
					AddSec->around[AddSec->count].y = player->CurSector.y + 1;
					AddSec->count++;
				}

				//⭢
				AddSec->around[AddSec->count].x = player->CurSector.x + 1;
				AddSec->around[AddSec->count].y = player->CurSector.y;
				AddSec->count++;
			}
			//⭣
			if (player->CurSector.y < SECTOR_MAX_Y - 1)
			{
				AddSec->around[AddSec->count].x = player->CurSector.x;
				AddSec->around[AddSec->count].y = player->CurSector.y + 1;
				AddSec->count++;
			}

			//↙
			if (player->CurSector.x > 0)
			{
				if (player->CurSector.y < SECTOR_MAX_Y - 1)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x - 1;
					AddSec->around[AddSec->count].y = player->CurSector.y + 1;
					AddSec->count++;
				}
			}


			return;
		}
		
		
		//이동방향 ⭢
		{
			RemoveSec->count = 0;
			if (player->OldSector.x > 0)
			{
				//↖
				if (player->OldSector.y > 0)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
					RemoveSec->count++;
				}
				//↙
				if (player->OldSector.y < SECTOR_MAX_Y - 1)
				{
					RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
					RemoveSec->around[RemoveSec->count].y = player->OldSector.y + 1;
					RemoveSec->count++;
				}
				//⭠
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x - 1;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y;
				RemoveSec->count++;
			}

			AddSec->count = 0;
			if (player->CurSector.x < SECTOR_MAX_X - 1)
			{
				//↗
				if (player->CurSector.y > 0)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x + 1;
					AddSec->around[AddSec->count].y = player->CurSector.y - 1;
					AddSec->count++;
				}
				//↘
				if (player->CurSector.y < SECTOR_MAX_Y - 1)
				{
					AddSec->around[AddSec->count].x = player->CurSector.x + 1;
					AddSec->around[AddSec->count].y = player->CurSector.y + 1;
					AddSec->count++;
				}
				//⭢
				AddSec->around[AddSec->count].x = player->CurSector.x + 1;
				AddSec->around[AddSec->count].y = player->CurSector.y;
				AddSec->count++;

			}


			return;
		}

	}
}