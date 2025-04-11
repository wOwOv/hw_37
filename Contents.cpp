//#include "Struct.h"

#include "Network.h"
#include "SerialBuffer.h"

#include "Messagestub.h"
#include "Messageproxy.h"
#include "Protocol.h"
#include "Contents.h"
#include "System.h"
#include <iostream>
#include <list>
#include <unordered_map>


std::list<Player*> Sector[SECTOR_MAX_Y][SECTOR_MAX_X];
std::unordered_map<unsigned int, Player*> PlayerMap;


bool ProcCreateMe(Player* player, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp) { return true; }
bool ProcCreateOther(Player* player, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp) { return true; }
bool ProcDelete(Player* player) { return true; }


bool ProcMoveStart(Player* player, unsigned char dir, unsigned short x, unsigned short y)
{
	_LOG(LOG_LEVEL_DEBUG, L"#MOVESTART# id:%d / direction:%d / x:%d / y:%d \n", player->id, dir, x, y);

	SBuffer buf;

	if (abs(player->x - x) > dfERROR_RANGE || abs(player->y - y) > dfERROR_RANGE)
	{
		mpSCSYNC(player->id, player->x, player->y, &buf);
		SendAround(player->session, &buf, true);

		x = player->x;
		y = player->y;

	}
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

		
		
		//해당 플레이어에 대한 움직임 정보 본인 제외 근처sector에 send
		buf.Clear();
		mpSCMOVESTART(player->id, player->move, player->x, player->y, &buf);
		SendAround(player->session, &buf, false);

	return true;
}


bool ProcMoveStop(Player* player, unsigned char dir, unsigned short x, unsigned short y)
{
	_LOG(LOG_LEVEL_DEBUG, L"#MOVESTOP# id:%d / direction:%d / x:%d / y:%d \n", player->id, dir, x, y);

	SBuffer buf;

	if (abs(player->x - x) > dfERROR_RANGE || abs(player->y - y) > dfERROR_RANGE)
	{
		mpSCSYNC(player->id, player->x, player->y, &buf);
		SendAround(player->session, &buf, true);

		x = player->x;
		y = player->y;
	}
	
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


		//해당 플레이어에 대한 움직임 정보 본인 제외 전체에게 send
		buf.Clear();
		mpSCMOVESTOP(player->id, dir, player->x, player->y, &buf);
		SendAround(player->session, &buf, false);
		//BProcMoveStop(player, player->id, dir, player->x, player->y);

	
	return true;
}

bool ProcAttack1(Player* player, unsigned char dir, unsigned short x, unsigned short y)
{
	_LOG(LOG_LEVEL_DEBUG, L"#ATTACK1# id:%d / direction:%d / x:%d / y:%d \n", player->id, dir, x, y);

	SBuffer buf;

	if (abs(player->x - x) > dfERROR_RANGE || abs(player->y - y) > dfERROR_RANGE)
	{
		mpSCSYNC(player->id, player->x, player->y, &buf);
		SendAround(player->session, &buf, true);

		x = player->x;
		y = player->y;
	}

		//해당 플레이어 공격 정보 처리
		player->direction = dir;
		player->x = x;
		player->y = y;

		//해당 플레이어 공격 정보 send()
		buf.Clear();
		mpSCATTACK1(player->id, player->direction, player->x, player->y, &buf);
		SendAround(player->session, &buf, false);
		//BProcAttack1(player, player->id, player->direction, player->x, player->y);

		//데미지처리
		AttackPlayer(player, dfPACKET_CS_ATTACK1);


	return true;
}

bool ProcAttack2(Player* player, unsigned char dir, unsigned short x, unsigned short y)
{
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
		BProcAttack2(player, player->id, player->direction, player->x, player->y);

		//데미지처리
		AttackPlayer(player, dfPACKET_CS_ATTACK2);

	}
	return true;
}

bool ProcAttack3(Player* player, unsigned char dir, unsigned short x, unsigned short y)
{

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
		BProcAttack3(player, player->id, player->direction, player->x, player->y);

		//데미지처리
		AttackPlayer(player, dfPACKET_CS_ATTACK3);

	}
	return true;
}

bool ProcDamage(Player* player, unsigned int tgt, unsigned char hp) { return true; }

bool ProcEcho(Player* player, unsigned int time)
{
	SBuffer buf;
	mpSCECHO(time, &buf);
	SendUnicast(player, &buf);

	return true;
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
		if (SectorY < SECTOR_MAX_Y - 1)
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

	if (SectorX < SECTOR_MAX_X - 1)
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
		if (player->CurSector.y > player->OldSector.y)
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
			if (player->OldSector.y > 0)
			{
				RemoveSec->around[RemoveSec->count].x = player->OldSector.x;
				RemoveSec->around[RemoveSec->count].y = player->OldSector.y - 1;
				RemoveSec->count++;
			}

			//↖
			if (player->OldSector.x > 0)
			{
				if (player->OldSector.y > 0)
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
			if (player->CurSector.y < SECTOR_MAX_Y - 1)
			{
				AddSec->around[AddSec->count].x = player->CurSector.x;
				AddSec->around[AddSec->count].y = player->CurSector.y + 1;
				AddSec->count++;
			}

			//↘
			if (player->CurSector.x < SECTOR_MAX_X - 1)
			{
				if (player->CurSector.y < SECTOR_MAX_Y - 1)
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
		if (player->CurSector.y < player->CurSector.y)
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