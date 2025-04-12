#include <list>
#include "Network.h"
#include "SerialBuffer.h"
#include "Messageproxy.h"
#include "Messagestub.h"
#include "Protocol.h"
#include "Contents.h"
#include "System.h"
#include <unordered_map>

#define SERVERPORT 20000

using namespace std;

extern list<Player*> PlayerList;
extern unordered_map<unsigned int, Player*> PlayerMap;
SOCKET listensock;

int PlayerID = 0;
std::unordered_map<unsigned int, Session*> SessionMap;
extern list<Player*> Sector[SECTOR_MAX_Y][SECTOR_MAX_X];
extern int sLogLevel;
extern WCHAR LogBuff[1024];



void networkStartUp()
{
	int scretval;//startup retval
	int lscretval;//listensock retval
	int nbretval;//nonblocking retval
	int bdretval;//bind retval
	int lnretval;//listen retval
	int lgretval;//linger retval
	int ndretval;//nodelay retval



	//윈속초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		scretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM,L"WSAStartup error: %d\n", scretval);

	}

	//listensock todtjd
	listensock = socket(AF_INET, SOCK_STREAM, 0);
	if (listensock == INVALID_SOCKET)
	{
		lscretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"socket() error: %d\n", lscretval);

	}

	//listensock 바인딩
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	bdretval = bind(listensock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (bdretval == SOCKET_ERROR)
	{
		bdretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"bind error: %d\n", bdretval);

	}

	//listen sock nonblocking
	u_long on = 1;
	nbretval = ioctlsocket(listensock, FIONBIO, &on);
	if (nbretval == SOCKET_ERROR)
	{
		nbretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"nonblokcin error: %d\n", nbretval);

	}

	//SO_LINGER
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 0;
	lgretval = setsockopt(listensock, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));
	if (lgretval == SOCKET_ERROR)
	{
		lgretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"linger error: %d\n", lgretval);

	}

	//nodelay
	BOOL optval = TRUE;
	ndretval = setsockopt(listensock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	if (ndretval == SOCKET_ERROR)
	{
		ndretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"nodelay error: %d\n", ndretval);

	}

	//listen()
	lnretval = listen(listensock, SOMAXCONN);
	if (lnretval == SOCKET_ERROR)
	{
		lnretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"listen error: %d\n", lnretval);

	}
}

void networkCleanUp()
{
	int curetval;//cleanup retval
	int csretval;//closesocket retval

	//closesocket()
	csretval = closesocket(listensock);
	if (csretval == SOCKET_ERROR)
	{
		csretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"closesocket error: %d\n", csretval);
	}
	//윈속종료
	curetval = WSACleanup();
	if (curetval == SOCKET_ERROR)
	{
		curetval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"cleanup error: %d\n", curetval);
	}



}

void NetworkSelect()
{
	int slretval;//select retval


	//네트워크
//소켓 셋 초기화
	fd_set readset;
	fd_set writeset;
	FD_ZERO(&readset);
	FD_ZERO(&writeset);
	FD_SET(listensock, &readset);

	
	for (unordered_map<unsigned int, Player*>::iterator it = PlayerMap.begin(); it != PlayerMap.end(); it++)
	{
		Player* tgtply = it->second;
		FD_SET(tgtply->session->sock, &readset);
		if (tgtply->session->SendQ.GetUsedSize() > 0)
		{
			FD_SET(tgtply->session->sock, &writeset);
		}
	}

	//select() 대기없음
	TIMEVAL timevalue;
	timevalue.tv_sec = 0;
	timevalue.tv_usec = 0;
	slretval = select(0, &readset, &writeset, NULL, &timevalue);
	if (slretval == SOCKET_ERROR)
	{
		slretval = WSAGetLastError();
		_LOG(LOG_LEVEL_DEBUG, L"select error: %d\n", slretval);
	}

	if (slretval > 0)
	{
		//readset에 listensock 있는지 확인
		if (FD_ISSET(listensock, &readset))
		{
			AcceptProc();
			--slretval;
		}

		//readset과 writeset에 clientsock있는지 확인
		unordered_map<unsigned int, Player*>::iterator iter = PlayerMap.begin();
		for (; iter != PlayerMap.end(); iter++)
		{
			Player* tgtply = iter->second;
			//readset에 있는지 확인
			if (FD_ISSET(tgtply->session->sock, &readset))
			{
				if (tgtply->remove == false)
				{
					--slretval;
					RecvProc(tgtply);
				}
			}
			//writeset에 있는지 확인
			if (FD_ISSET(tgtply->session->sock, &writeset))
			{
				--slretval;
				SendProc(tgtply);

			}
			if (slretval == 0)
			{
				break;
			}
		}
	}

}

//백로그큐에서 뽑아오기
void AcceptProc()
{
	int atretval;//accept retval
	SBuffer buf;

	//clientsock 생성
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);

	//세션 세팅
	Player* player = new Player;
	player->session = new Session;
	player->session->sock = accept(listensock, (SOCKADDR*)&clientaddr, &addrlen);
	if (player->session->sock == INVALID_SOCKET)
	{
		atretval = WSAGetLastError();
		printf("accept error: %d\n", atretval);
	}
	else
	{
		player->id = PlayerID;
		player->session->Sessionid = PlayerID;
		player->direction = dfPACKET_MOVE_DIR_RR;
		player->x = rand() % 500 + 50;//320;
		player->y = rand() % 300 + 100;//260;
		player->hp = 100;
		player->move = -1;
		player->remove = false;

		player->CurSector.x = player->x / SECTOR_WIDTH;
		player->CurSector.y = player->y / SECTOR_HEIGHT;
		++PlayerID;

		//신규접속자 본인 생성메시지 send
		mpSCCREATEME(player->id, player->direction, player->x, player->y, player->hp, &buf);
		SendUnicast(player->session, &buf);
		//UProcCreateMe(player, player->id, player->direction, player->x, player->y, player->hp);


		//신규접속자에게 근처 섹터의 기존 접속자들 생성메시지 send
		SectorAround sectoraround;
		GetSectorAround(player->CurSector.x, player->CurSector.y, &sectoraround);
		for (int i = 0; i < sectoraround.count; i++)
		{
			list<Player*>::iterator it = Sector[sectoraround.around[i].y][sectoraround.around[i].x].begin();
			for (; it != Sector[sectoraround.around[i].y][sectoraround.around[i].x].end(); it++)
			{
				Player* exist = *it;
				buf.Clear();
				mpSCCREATEOTHER(exist->id, exist->direction, exist->x, exist->y, exist->hp, &buf);
				SendUnicast(player->session, &buf);
			}
		}

		/*
		unordered_map<unsigned int, Player*>::iterator it = PlayerMap.begin();
		for (; it != PlayerMap.end(); it++)
		{
			Player* tgtply = it->second;
			
			buf.Clear();
			mpSCCREATEOTHER(tgtply->id, tgtply->direction, tgtply->x, tgtply->y, tgtply->hp, &buf);
			SendUnicast(tgtply->session, &buf);

			//UProcCreateOther(player, tgtply->id, tgtply->direction, tgtply->x, tgtply->y, tgtply->hp);


			//신규접속자에게 기존 접속자들이 움직이고 있었다면 그거에 대한 MOVESTART메시지까지 send
			if (tgtply->move != -1)
			{

				UProcMoveStart(player, tgtply->id, tgtply->move, tgtply->x, tgtply->y);

			}


		}
*/


		

		//신규접속자 PlayerMap, SectorList,SessionMap에 넣기
		PlayerMap.insert(make_pair(player->id, player));
		SessionMap.insert(make_pair(player->id, player->session));
		Sector[player->CurSector.y][player->CurSector.x].push_back(player);

		//기존접속자에게 신규접속자 생성메시지 send
		buf.Clear();
		mpSCCREATEOTHER(player->id, player->direction, player->x, player->y, player->hp, &buf);
		SendAround(player->session, &buf, false);

	}
}



//client가 보내온 메시지 처리
void RecvProc(Player* player)
{
	int rvretval;//recv retval

	if (player->remove == false)
	{
		//RecvQ에 최대 directsize만큼 recv
		int directsize = player->session->RecvQ.DirectEnqueueSize();
		rvretval = recv(player->session->sock, player->session->RecvQ.GetRearBufferPtr(), directsize, 0);
		if (rvretval == SOCKET_ERROR)
		{
			rvretval = WSAGetLastError();
			printf("recv error: %d", rvretval);
			Disconnect(player);
		}
		else if (rvretval == 0)
		{
			Disconnect(player);
		}
		else
		{
			int movret = player->session->RecvQ.MoveRear(rvretval);
			if (movret != rvretval)
			{
				DebugBreak();
			}
			SBuffer msgbuf;
			while (1)
			{
				if (player->session->RecvQ.GetUsedSize() < sizeof(HEADER))
				{
					break;
				}
				msgbuf.Clear();
				HEADER header;
				int pkret = player->session->RecvQ.Peek((char*)&header, sizeof(HEADER));
				if (pkret != sizeof(HEADER))
				{
					DebugBreak();
				}
				if (header.code != 0x89)
				{
					Disconnect(player);
					break;
				}
				if (player->session->RecvQ.GetUsedSize() < header.size + sizeof(HEADER))
				{
					break;
				}

				int deqret = player->session->RecvQ.Dequeue(msgbuf.GetBufferPtr(), header.size + sizeof(HEADER));
				if (deqret != header.size + sizeof(HEADER))
				{
					DebugBreak();
				}
				int moveret = msgbuf.MoveWritePos(deqret);
				if (moveret != deqret)
				{
					DebugBreak();
				}
				MessageProc(player, &msgbuf);

			}
		}
	}
}

void SendProc(Player* player)
{
	if (player->remove == false)
	{
		int sdretval;//send retval

		int usedsize = player->session->SendQ.GetUsedSize();
		if (usedsize >= 0)
		{
			int directsize = player->session->SendQ.DirectDequeueSize();
			sdretval = send(player->session->sock, player->session->SendQ.GetFrontBufferPtr(), directsize, 0);
			if (sdretval == SOCKET_ERROR)
			{
				Disconnect(player);
			}
			else
			{
				player->session->SendQ.MoveFront(sdretval);
			}

		}
	}
}







/*
//player에게만 sendQ에 Enqueue
void SendUnicast(Player* player, SBuffer* buf)
{
	if (player->remove == false)
	{
		int size = buf->GetDataSize();
		int enqret = player->session->SendQ.Enqueue(buf->GetBufferPtr(), size);
		if (enqret != size)
		{
			Disconnect(player);
		}
	}
}

//player제외하고 전체sendQ에 Enqueue
void SendBroadcast(Player* player, SBuffer* buf)
{
	list<Player*>::iterator it = PlayerList.begin();
	for (; it != PlayerList.end(); it++)
	{
		if ((*it)->remove == false)
		{
			if (player == nullptr)
			{
				int size = buf->GetDataSize();
				int enqret = (*it)->session->SendQ.Enqueue(buf->GetBufferPtr(), size);
				if (enqret != size)
				{
					Disconnect(player);
				}
			}
			else
			{
				if ((*it)->id != player->id)
				{
					int size = buf->GetDataSize();
					int enqret = (*it)->session->SendQ.Enqueue(buf->GetBufferPtr(), size);
					if (enqret != size)
					{
						Disconnect(player);
					}
				}
			}
		}

	}
}
*/



extern std::list<Player*> Sector[SECTOR_MAX_Y][SECTOR_MAX_X];
extern std::unordered_map<unsigned int, Player*> PlayerMap;

//특정 섹터 1개에 보내기
void SendSectorOne(int SectorX, int SectorY, SBuffer* buf, Session* exceptsession)
{
	std::list<Player*>::iterator it = Sector[SectorY][SectorX].begin();
	for (; it != Sector[SectorY][SectorX].end(); it++)
	{
		if ((*it)->session != exceptsession)
		{
			int size = buf->GetDataSize();
			Player* player = *it;
			int enqret = player->session->SendQ.Enqueue(buf->GetBufferPtr(), size);
			if (enqret != size)
			{
				Disconnect(player);
			}
		}
	}
}

//특정 1명의 클라에 보내기
void SendUnicast(Session* session, SBuffer* buf)
{
	int size = buf->GetDataSize();
	int enqret = session->SendQ.Enqueue(buf->GetBufferPtr(), size);
	if (enqret != size)
	{
		Player* player = PlayerMap.find(session->Sessionid)->second;
		Disconnect(player);
	}

}

//클라 기준 주변 섹터에 보내기
//만약 참이라면 본인도 보내기
void SendAround(Session* session, SBuffer* buf, bool me)
{
	Player* player = PlayerMap.find(session->Sessionid)->second;
	//주변 세션 찾기 최대 9개
	SectorAround sectoraround;
	GetSectorAround(player->CurSector.x, player->CurSector.y, &sectoraround);
	for (int i = 0; i < sectoraround.count - 1; i++)
	{
		std::list<Player*>::iterator it = Sector[sectoraround.around[i].y][sectoraround.around[i].x].begin();
		for (; it != Sector[sectoraround.around[i].y][sectoraround.around[i].x].end(); it++)
		{
			//날 제외하지 않는다면
			if (me != false)
			{
				Player* tgtplayer = *it;
				int size = buf->GetDataSize();
				int enqret = tgtplayer->session->SendQ.Enqueue(buf->GetBufferPtr(), size);
				if (enqret != size)
				{
					Disconnect(player);
				}
			}
			else
			{
				Player* tgtplayer = *it;
				if (tgtplayer->session != session)
				{
					int size = buf->GetDataSize();
					int enqret = tgtplayer->session->SendQ.Enqueue(buf->GetBufferPtr(), size);
					if (enqret != size)
					{
						Disconnect(player);
					}
				}
			}
		}
		
	}
	
}

//섹터리스트 받아서 섹터리스트에 들어있는 섹터들에 보내기
//만약 참이라면 본인도 보내기
void SendSectorList(SectorAround* sectoraround, SBuffer* buf, bool me)
{
	for (int i = 0; i < sectoraround->count; i++)
	{
		std::list<Player*>::iterator it = Sector[sectoraround->around[i].y][sectoraround->around[i].x].begin();
		for (; it != Sector[sectoraround->around[i].y][sectoraround->around[i].x].end(); it++)
		{
			Player* player = *it;
			int size = buf->GetDataSize();
			int enqret = player->session->SendQ.Enqueue(buf->GetBufferPtr(), size);
			if (enqret != size)
			{
				Disconnect(player);
			}
		}
	}
}

//진짜 브로드캐스팅
void SendBroadcast(Session* session, SBuffer* buf)
{
	std::unordered_map<unsigned int, Session*>::iterator it = SessionMap.begin();
	for (; it != SessionMap.end(); it++)
	{
		Session* session = it->second;
		int size = buf->GetDataSize();
		int enqret = session->SendQ.Enqueue(buf->GetBufferPtr(), size);
		if (enqret != size)
		{
			Player* player = PlayerMap.find(session->Sessionid)->second;
			Disconnect(player);
		}
	}
}
