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

		//sector 계산
		player->OldSector.x = player->CurSector.x;
		player->OldSector.y = player->CurSector.y;

		player->CurSector.x = player->x / SECTOR_WIDTH;
		player->CurSector.y = player->y / SECTOR_HEIGHT;

		//sector변화가 있다면
		if (player->OldSector.x != player->CurSector.x && player->OldSector.y != player->CurSector.y)
		{
			//섹터옮기기
			std::list<Player*>::iterator it = Sector[player->OldSector.y][player->OldSector.x].begin();
			for (; it != Sector[player->OldSector.y][player->OldSector.x].end(); it++)
			{
				Player* tgt = *it;
				if (tgt->id == player->id)
				{
					Sector[player->OldSector.y][player->OldSector.x].erase(it);
					break;
				}
			}

			Sector[player->CurSector.y][player->CurSector.x].push_back(player);



			SectorAround removesec;
			SectorAround addsec;
			GetUpdateSectorAround(player, &removesec, &addsec);

			//제거해야할 sector들에 delete메시지 보내기
			SBuffer buf;
			mpSCDELETE(player->id, &buf);
			SendSectorList(player->session,&removesec, &buf, false);

			//추가해야할 sector들에 create메시지 보내기
			buf.Clear();
			mpSCCREATEOTHER(player->id, player->direction, player->x, player->y, player->hp, &buf);
			SendSectorList(player->session,&addsec, &buf, false);
			//movestop메시지 까지 보내주기
			buf.Clear();
			mpSCMOVESTOP(player->id, player->direction, player->x, player->y, &buf);
			SendSectorList(player->session,&addsec, &buf, false);
		}


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
	_LOG(LOG_LEVEL_DEBUG, L"#ATTACK2# id:%d / direction:%d / x:%d / y:%d \n", player->id, dir, x, y);

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
	mpSCATTACK2(player->id, player->direction, player->x, player->y, &buf);
	SendAround(player->session, &buf, false);

	//데미지처리
	AttackPlayer(player, dfPACKET_CS_ATTACK2);


	return true;
}

bool ProcAttack3(Player* player, unsigned char dir, unsigned short x, unsigned short y)
{
	_LOG(LOG_LEVEL_DEBUG, L"#ATTACK3# id:%d / direction:%d / x:%d / y:%d \n", player->id, dir, x, y);

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
	mpSCATTACK3(player->id, player->direction, player->x, player->y, &buf);
	SendAround(player->session, &buf, false);


	//데미지처리
	AttackPlayer(player, dfPACKET_CS_ATTACK3);


	return true;
}

//데미지 입힐 player 찾고 damage패킷 보내기
void AttackPlayer(Player* player, unsigned char type)
{
	if (player->remove == false)
	{
		SectorAround sec;
		GetAttackSectorAround(player, &sec);
		SBuffer buf;
		switch (type)
		{
		case dfPACKET_CS_ATTACK1:
		{
			if (player->direction == dfPACKET_MOVE_DIR_LL)
			{
				for (int i = 0; i < sec.count; i++)
				{
					std::list<Player*>::iterator tgtit = Sector[sec.around[i].y][sec.around[i].x].begin();
					for (; tgtit != Sector[sec.around[i].y][sec.around[i].x].end(); tgtit++)
					{
						Player* tgtply = *tgtit;
						if (tgtply->id != player->id && tgtply->remove == false)
						{
							if (tgtply->x <= player->x)
							{
								if ((player->x - tgtply->x) < dfATTACK1_RANGE_X && abs(player->y - tgtply->y) < dfATTACK1_RANGE_Y)
								{
									//hp처리 후 메시지 만들어 전체 send
									tgtply->hp -= dfATTACK1_DAMAGE;
									if (tgtply->hp <= 0)
									{
										Disconnect(tgtply);
									}
									buf.Clear();
									mpSCDAMAGE(player->id, tgtply->id, tgtply->hp, &buf);

									//맞는 player근처 sector에 send
									SendAround(tgtply->session, &buf, true);

									return;
								}
							}
						}
					}

				}
			}
			if (player->direction == dfPACKET_MOVE_DIR_RR)
			{
				for (int i = 0; i < sec.count; i++)
				{
					std::list<Player*>::iterator tgtit = Sector[sec.around[i].y][sec.around[i].x].begin();
					for (; tgtit != Sector[sec.around[i].y][sec.around[i].x].end(); tgtit++)
					{
						Player* tgtply = *tgtit;
						if (tgtply->id != player->id && tgtply->remove == false)
						{
							if (tgtply->x >= player->x)
							{
								if ((player->x - tgtply->x) < dfATTACK1_RANGE_X && abs(player->y - tgtply->y) < dfATTACK1_RANGE_Y)
								{
									//hp처리 후 메시지 만들어 전체 send
									tgtply->hp -= dfATTACK1_DAMAGE;
									if (tgtply->hp <= 0)
									{
										Disconnect(tgtply);
									}
									buf.Clear();
									mpSCDAMAGE(player->id, tgtply->id, tgtply->hp, &buf);

									//맞는 target player근처 sector에 send
									SendAround(tgtply->session, &buf, true);

									return;
								}
							}
						}
					}
				}
			}
		}
		case dfPACKET_CS_ATTACK2:
		{
			if (player->direction == dfPACKET_MOVE_DIR_LL)
			{
				for (int i = 0; i < sec.count; i++)
				{
					std::list<Player*>::iterator tgtit = Sector[sec.around[i].y][sec.around[i].x].begin();
					for (; tgtit != Sector[sec.around[i].y][sec.around[i].x].end(); tgtit++)
					{
						Player* tgtply = *tgtit;
						if (tgtply->id != player->id && tgtply->remove == false)
						{
							if (tgtply->x <= player->x)
							{
								if ((player->x - tgtply->x) < dfATTACK2_RANGE_X && abs(player->y - tgtply->y) < dfATTACK2_RANGE_Y)
								{
									//hp처리 후 메시지 만들어 전체 send
									tgtply->hp -= dfATTACK2_DAMAGE;
									if (tgtply->hp <= 0)
									{
										Disconnect(tgtply);
									}
									buf.Clear();
									mpSCDAMAGE(player->id, tgtply->id, tgtply->hp, &buf);

									//맞는 player근처 sector에 send
									SendAround(tgtply->session, &buf, true);

									return;
								}
							}
						}
					}

				}
			}
			if (player->direction == dfPACKET_MOVE_DIR_RR)
			{
				for (int i = 0; i < sec.count; i++)
				{
					std::list<Player*>::iterator tgtit = Sector[sec.around[i].y][sec.around[i].x].begin();
					for (; tgtit != Sector[sec.around[i].y][sec.around[i].x].end(); tgtit++)
					{
						Player* tgtply = *tgtit;
						if (tgtply->id != player->id && tgtply->remove == false)
						{
							if (tgtply->x >= player->x)
							{
								if ((player->x - tgtply->x) < dfATTACK2_RANGE_X && abs(player->y - tgtply->y) < dfATTACK2_RANGE_Y)
								{
									//hp처리 후 메시지 만들어 전체 send
									tgtply->hp -= dfATTACK2_DAMAGE;
									if (tgtply->hp <= 0)
									{
										Disconnect(tgtply);
									}
									buf.Clear();
									mpSCDAMAGE(player->id, tgtply->id, tgtply->hp, &buf);

									//맞는 target player근처 sector에 send
									SendAround(tgtply->session, &buf, true);

									return;
								}
							}
						}
					}
				}
			}
		}
		case dfPACKET_CS_ATTACK3:
		{
			if (player->direction == dfPACKET_MOVE_DIR_LL)
			{
				for (int i = 0; i < sec.count; i++)
				{
					std::list<Player*>::iterator tgtit = Sector[sec.around[i].y][sec.around[i].x].begin();
					for (; tgtit != Sector[sec.around[i].y][sec.around[i].x].end(); tgtit++)
					{
						Player* tgtply = *tgtit;
						if (tgtply->id != player->id && tgtply->remove == false)
						{
							if (tgtply->x <= player->x)
							{
								if ((player->x - tgtply->x) < dfATTACK3_RANGE_X && abs(player->y - tgtply->y) < dfATTACK3_RANGE_Y)
								{
									//hp처리 후 메시지 만들어 전체 send
									tgtply->hp -= dfATTACK3_DAMAGE;
									if (tgtply->hp <= 0)
									{
										Disconnect(tgtply);
									}
									buf.Clear();
									mpSCDAMAGE(player->id, tgtply->id, tgtply->hp, &buf);

									//맞는 player근처 sector에 send
									SendAround(tgtply->session, &buf, true);

									return;
								}
							}
						}
					}

				}
			}
			if (player->direction == dfPACKET_MOVE_DIR_RR)
			{
				for (int i = 0; i < sec.count; i++)
				{
					std::list<Player*>::iterator tgtit = Sector[sec.around[i].y][sec.around[i].x].begin();
					for (; tgtit != Sector[sec.around[i].y][sec.around[i].x].end(); tgtit++)
					{
						Player* tgtply = *tgtit;
						if (tgtply->id != player->id && tgtply->remove == false)
						{
							if (tgtply->x >= player->x)
							{
								if ((player->x - tgtply->x) < dfATTACK3_RANGE_X && abs(player->y - tgtply->y) < dfATTACK3_RANGE_Y)
								{
									//hp처리 후 메시지 만들어 전체 send
									tgtply->hp -= dfATTACK3_DAMAGE;
									if (tgtply->hp <= 0)
									{
										Disconnect(tgtply);
									}
									buf.Clear();
									mpSCDAMAGE(player->id, tgtply->id, tgtply->hp, &buf);

									//맞는 target player근처 sector에 send
									SendAround(tgtply->session, &buf, true);

									return;
								}
							}
						}
					}
				}
			}
		}
		}
	}
}


bool ProcDamage(Player* player, unsigned int tgt, unsigned char hp) { return true; }

bool ProcEcho(Player* player, unsigned int time)
{
	SBuffer buf;
	mpSCECHO(time, &buf);
	SendUnicast(player->session, &buf);

	return true;
}


void Disconnect(Player* player)
{
	//list에서 삭제 예정 표시
	player->remove = true;

	//player 삭제 메시지 Broadcast
	SBuffer buf;
	mpSCDELETE(player->id, &buf);
	SendAround(player->session, &buf, true);
	//BProcDelete(player, player->id);

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

void GetAttackSectorAround(Player* player, SectorAround* sec)
{
		sec->count = 0;
		//자기가 속한 섹터
		sec->around[sec->count].x = player->CurSector.x;
		sec->around[sec->count].y = player->CurSector.y;
		sec->count++;

		

	if (player->direction == dfPACKET_MOVE_DIR_LL)
	{
		if (player->CurSector.x > 0)
		{
			//⭠
			sec->around[sec->count].x = player->CurSector.x - 1;
			sec->around[sec->count].y = player->CurSector.y;
			sec->count++;

			//↖
			if (player->CurSector.y > 0)
			{
				sec->around[sec->count].x = player->CurSector.x - 1;
				sec->around[sec->count].y = player->CurSector.y - 1;
				sec->count++;
			}
			//↙
			if (player->CurSector.y < SECTOR_MAX_Y-1)
			{
				sec->around[sec->count].x = player->CurSector.x - 1;
				sec->around[sec->count].y = player->CurSector.y + 1;
				sec->count++;
			}


		}

	}

	if(player->direction == dfPACKET_MOVE_DIR_RR)
	{
		if (player->CurSector.x < SECTOR_MAX_X-1)
		{
			//⭢
			sec->around[sec->count].x = player->CurSector.x + 1;
			sec->around[sec->count].y = player->CurSector.y;
			sec->count++;

			//↗
			if (player->CurSector.y > 0)
			{
				sec->around[sec->count].x = player->CurSector.x + 1;
				sec->around[sec->count].y = player->CurSector.y - 1;
				sec->count++;
			}
			//↘
			if (player->CurSector.y < SECTOR_MAX_Y - 1)
			{
				sec->around[sec->count].x = player->CurSector.x + 1;
				sec->around[sec->count].y = player->CurSector.y + 1;
				sec->count++;
			}


		}

	}
	//⭡
	if (player->CurSector.y > 0)
	{
		sec->around[sec->count].x = player->CurSector.x;
		sec->around[sec->count].y = player->CurSector.y - 1;
		sec->count++;
	}

	//⭣
	if (player->CurSector.y < SECTOR_MAX_Y - 1)
	{
		sec->around[sec->count].x = player->CurSector.x;
		sec->around[sec->count].y = player->CurSector.y + 1;
		sec->count++;
	}

}