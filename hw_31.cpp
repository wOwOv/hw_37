#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"ws2_32.lib")



#include "Contents.h"
#include "RingBuffer.h"
#include <winsock2.h>
#include <list>
#include "Protocol.h"
#include "SerialBuffer.h"
#include "System.h"
#include "Network.h"
#include "Messageproxy.h"
#include "Messagestub.h"
#include <unordered_map>


/*
게임프레임 50fps
*/


#define BUF 2048


using namespace std;


void Update(WORD T);

bool FrameControl();

extern int sLogLevel;
extern WCHAR LogBuff[1024];

extern std::list<Player*> Sector[SECTOR_MAX_Y][SECTOR_MAX_X];
extern std::unordered_map<unsigned int, Player*> PlayerMap;
extern std::unordered_map<unsigned int, Session*> SessionMap;

list<Player*> PlayerList;

extern SOCKET listensock;
DWORD OldTick;

DWORD tttt;
int frameCount=0;

int main()
{
	sLogLevel = 0;
	//타이머해상도 설정
	timeBeginPeriod(1);
	networkStartUp();
	NetworkSelect();

	OldTick = timeGetTime();
	tttt = timeGetTime();
	while (1)
	{
		//네트워크 송수신 처리
		NetworkSelect();


		//타임아웃 지난애들도 연결끊기
		DWORD CurrentTick = timeGetTime();
		unordered_map<unsigned int, Player*>::iterator tmit = PlayerMap.begin();
		for (; tmit != PlayerMap.end();tmit++)
		{
			Player* tgtply = tmit->second;
			if (tgtply->remove == false)
			{
				if (CurrentTick - tgtply->session->LastRecvTime > dfNETWORK_PACKET_RECV_TIMEOUT)
				{
					Disconnect(tgtply);
				}
			}
		}



		//끊긴 player지우기
		unordered_map<unsigned int,Player*>::iterator dlit = PlayerMap.begin();
		for (; dlit != PlayerMap.end();)
		{
			Player* tgtply = dlit->second;
			if (tgtply->remove != false)
			{
				closesocket(tgtply->session->sock);
				//Sessionmap에서 지우기
				SessionMap.erase(tgtply->id);
				//Sector리스트에서 지우기
				list<Player*>::iterator it = Sector[tgtply->CurSector.y][tgtply->CurSector.x].begin();
				for (; it != Sector[tgtply->CurSector.y][tgtply->CurSector.x].end(); it++)
				{
					if (*it == tgtply)
					{
						Sector[tgtply->CurSector.y][tgtply->CurSector.x].erase(it);
						break;
					}
				}
				delete tgtply->session;
				delete tgtply;
				dlit = PlayerMap.erase(dlit);
			}
			else
			{
				++dlit;
			}
		}


		//Player 움직임 로직처리
		//프레임간 시간 단위 처리..20ms
		if (FrameControl())
		{
			DWORD deltatime = timeGetTime() - OldTick;
			DWORD frame = deltatime / 20;
			DWORD remain = deltatime % 20;
			OldTick += (deltatime - remain);
			Update((WORD)frame);
			//frameCount++;
		}
		if (timeGetTime() - tttt >= 1000)
		{
			_LOG(LOG_LEVEL_SYSTEM,L"\n\n##################################################FPS : %d##################################################\n\n", frameCount);
			tttt += 1000;
			frameCount = 0;
		}
	}


	networkCleanUp();

	//타이머해상도 해제
	timeEndPeriod(1);
}













void Update(WORD T)
{
		//움직임 로직처리
	unordered_map<unsigned int, Player*>::iterator moveit = PlayerMap.begin();
		for (; moveit != PlayerMap.end(); moveit++)
		{
			Player* player = moveit->second;
			if (player->move != -1)
			{
				switch (player->move)
				{
				case  dfPACKET_MOVE_DIR_LL:
				{
					if (player->x > dfRANGE_MOVE_LEFT+3)
					{
						player->x -= 3*T;
					}
					break;
				}
				case dfPACKET_MOVE_DIR_LU:
				{
					//왼쪽에 닿거나 위에 닿았을때
					if (player->x <= dfRANGE_MOVE_LEFT+3 || player->y <= dfRANGE_MOVE_TOP+2)
					{
						//이동하면 안됨
					}
					else
					{
						player->x -= 3*T;
						player->y -= 2*T;
					}
					break;
				}
				case dfPACKET_MOVE_DIR_UU:
				{
					if (player->y > dfRANGE_MOVE_TOP+2)
					{
						player->y -= 2*T;
					}
					break;
				}
				case dfPACKET_MOVE_DIR_RU:
				{
					//오른쪽에 닿거나 위에 닿았을때
					if (player->x >= dfRANGE_MOVE_RIGHT-3 || player->y <= dfRANGE_MOVE_TOP+2)
					{
						//이동하면 안됨
					}
					else
					{
						player->x += 3*T;
						player->y -= 2*T;
					}
					break;
				}
				case dfPACKET_MOVE_DIR_RR:
				{
					if (player->x < dfRANGE_MOVE_RIGHT-3)
					{
						player->x += 3*T;
					}
					break;
				}
				case dfPACKET_MOVE_DIR_RD:
				{
					//오른쪽에 닿거나 아래에 닿았을때
					if (player->x >= dfRANGE_MOVE_RIGHT-3 || player->y >= dfRANGE_MOVE_BOTTOM-2)
					{
						//이동하면 안됨
					}
					else
					{
						player->x += 3*T;
						player->y += 2*T;
					}
					break;
				}
				case dfPACKET_MOVE_DIR_DD:
				{
					if (player->y < dfRANGE_MOVE_BOTTOM-2)
					{
						player->y += 2*T;
					}
					break;
				}
				case dfPACKET_MOVE_DIR_LD:
				{
					//왼쪽에 닿거나 아래에 닿았을때
					if (player->x <= dfRANGE_MOVE_LEFT+3 || player->y >= dfRANGE_MOVE_BOTTOM-2)
					{
						//이동하면 안됨
					}
					else
					{
						player->x -= 3*T;
						player->y += 2*T;
					}
					break;
				}
				}

				_LOG(LOG_LEVEL_DEBUG,L"Move id: %d dir: %d x: %d y: %d\n", player->id, player->direction, player->x, player->y);
			}

			//sector 계산
			player->OldSector.x = player->CurSector.x;
			player->OldSector.y = player->CurSector.y;

			player->CurSector.x = player->x / SECTOR_WIDTH;
			player->CurSector.y = player->y / SECTOR_HEIGHT;

			//sector변화가 있다면
			if (player->OldSector.x != player->CurSector.x || player->OldSector.y != player->CurSector.y)
			{
				//섹터옮기기
				list<Player*>::iterator it = Sector[player->OldSector.y][player->OldSector.x].begin();
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

				//섹터가 변경된 것이니 그에 따라 player한테 removesec에 있는 객체들 삭제메시지 모내기
				for (int i = 0; i < removesec.count; i++)
				{
					list<Player*>::iterator rmvit = Sector[removesec.around[i].y][removesec.around[i].x].begin();
					for (; rmvit != Sector[removesec.around[i].y][removesec.around[i].x].end(); rmvit++)
					{
						Player* tgtply = *rmvit;
						if (tgtply->remove == false)
						{
							SBuffer delbuf;
							mpSCDELETE(tgtply->id, &delbuf);
							SendUnicast(player->session, &delbuf);
						}
					}
				}

				//섹터가 변경된 것이니 그에 따라 player한테 addsec에 있는 객체들 생성메시지 모내기
				for (int i = 0; i < addsec.count; i++)
				{
					list<Player*>::iterator addit = Sector[addsec.around[i].y][addsec.around[i].x].begin();
					for (; addit != Sector[addsec.around[i].y][addsec.around[i].x].end(); addit++)
					{
						Player* tgtply = *addit;
						if (tgtply->remove == false&&tgtply!=player)
						{
							SBuffer crtbuf;
							mpSCCREATEOTHER(tgtply->id,tgtply->direction,tgtply->x,tgtply->y,tgtply->hp, &crtbuf);
							SendUnicast(player->session, &crtbuf);

							//tgtply가 이동중이었다면 movestart메시지 까지..
							if (tgtply->move != -1)
							{
								SBuffer movbuf;
								mpSCMOVESTART(tgtply->id, tgtply->move, tgtply->x, tgtply->y, &movbuf);
								SendUnicast(player->session, &crtbuf);
							}
						}
					}
				}



				//제거해야할 sector들에 delete메시지 보내기
				SBuffer buf;
				mpSCDELETE(player->id, &buf);
				SendSectorList(player->session,&removesec, &buf, false);

				//추가해야할 sector들에 create메시지 보내기
				buf.Clear();
				mpSCCREATEOTHER(player->id, player->direction, player->x, player->y, player->hp, &buf);
				SendSectorList(player->session, &addsec, &buf, false);
				//움직이고 있다면 movestart메시지 까지 보내주기
				buf.Clear();
				mpSCMOVESTART(player->id, player->move, player->x, player->y, &buf);
				SendSectorList(player->session, &addsec, &buf, false);
			}

		}
}




//50fps -1프레임이 20ms 20ms가 지나면 update 안 지났다면 그냥 패스
bool FrameControl()
{
	DWORD T = timeGetTime() - OldTick;
	if (T < 20)
	{
		return FALSE;
	}
	else
	{
		//OldTick += 20;
		frameCount++;
		return TRUE;
	}

}