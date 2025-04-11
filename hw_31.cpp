#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"ws2_32.lib")

//#define WINDOWS_IGNORE_PACKING_MISMATCH
//#pragma pack(1)

#include "Struct.h"
#include "RingBuffer.h"
#include <winsock2.h>
#include <list>
#include "PacketDefine.h"
#include "PacketMake.h"
#include "SerialBuffer.h"
#include "SendCast.h"
#include "Messageproxy.h"
#include "Messagestub.h"

/*
게임프레임 50fps
*/


#define BUF 2048

//-----------------------------------------------------------------
// 화면 이동영역
//-----------------------------------------------------------------
#define dfRANGE_MOVE_TOP	50
#define dfRANGE_MOVE_LEFT	10
#define dfRANGE_MOVE_RIGHT	630
#define dfRANGE_MOVE_BOTTOM	470

//-----------------------------------------------------------------
// 이동 오류체크 범위
//-----------------------------------------------------------------
#define dfERROR_RANGE		50

//---------------------------------------------------------------
// 공격범위.
//---------------------------------------------------------------
#define dfATTACK1_RANGE_X		80
#define dfATTACK2_RANGE_X		90
#define dfATTACK3_RANGE_X		100
#define dfATTACK1_RANGE_Y		10
#define dfATTACK2_RANGE_Y		10
#define dfATTACK3_RANGE_Y		20

using namespace std;

void NetworkSelect();

void AcceptProc();
void RecvProc(Player* player);
void SendProc(Player* player);

void SendUnicast(Player* player, SBuffer* buf);
void SendBroadcast(Player* player, SBuffer* buf);
void Disconnect(Player* player);


void Update(WORD T);
void AttackPlayer(Player* player, unsigned char type);

bool FrameControl();



list<Player*> PlayerList;

extern SOCKET listensock;
DWORD OldTick;

DWORD tttt;
int frameCount=0;

int main()
{
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

		//끊긴 player지우기
		list<Player*>::iterator dlit = PlayerList.begin();
		for (; dlit != PlayerList.end();)
		{
			Player* tgtply = *dlit;
			if (tgtply->remove != false)
			{
				closesocket(tgtply->sock);
				delete tgtply;
				dlit = PlayerList.erase(dlit);
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
			printf("\n\nFPS : %d\n\n", frameCount);
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
		list<Player*>::iterator moveit = PlayerList.begin();
		for (; moveit != PlayerList.end(); moveit++)
		{
			Player* player = *moveit;
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
					if (player->x <= dfRANGE_MOVE_LEFT+3 || player->y <= dfRANGE_MOVE_TOP)
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
					if (player->y > dfRANGE_MOVE_TOP)
					{
						player->y -= 2*T;
					}
					break;
				}
				case dfPACKET_MOVE_DIR_RU:
				{
					//오른쪽에 닿거나 위에 닿았을때
					if (player->x >= dfRANGE_MOVE_RIGHT-3 || player->y <= dfRANGE_MOVE_TOP)
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
					if (player->x >= dfRANGE_MOVE_RIGHT-3 || player->y >= dfRANGE_MOVE_BOTTOM)
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
					if (player->y < dfRANGE_MOVE_BOTTOM)
					{
						player->y += 2*T;
					}
					break;
				}
				case dfPACKET_MOVE_DIR_LD:
				{
					//왼쪽에 닿거나 아래에 닿았을때
					if (player->x <= dfRANGE_MOVE_LEFT+3 || player->y >= dfRANGE_MOVE_BOTTOM)
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

				printf("move id: %d dir: %d x: %d y: %d\n", player->id, player->direction, player->x, player->y);
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