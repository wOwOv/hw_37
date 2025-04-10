#include <winsock2.h>
#include "SerialBuffer.h"
#include "System.h"
#include "Network.h"
#include "Contents.h"
#include "Protocol.h"
#include <unordered_map>

//std::unordered_map<>

int scretval;//startup,cleanup retval
int lscretval;//listensock retval
int nbretval;//nonblocking retval
int bdretval;//bind retval
int lnretval;//listen retval
int slretval;//select retval
int csretval;//closesocket retval
int atretval;//accept retval
int sdretval;//send retval
int rvretval;//recv retval
int lgretval;//linger retval
int ndretval;//nodelay retval

extern int sLogLevel;
extern WCHAR LogBuff[1024];

SOCKET ListenSocket = INVALID_SOCKET;
std::unordered_map<SOCKET, Session*> SessionMap;

void netStartUp()
{
	//윈속초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		scretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"WSAStartup error: %d\n", scretval);
	}

	//listensock 생성
	ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket == INVALID_SOCKET)
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
	bdretval = bind(ListenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (bdretval == SOCKET_ERROR)
	{
		bdretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"bind error: %d\n", bdretval);
	}

	//listen sock nonblocking
	u_long on = 1;
	nbretval = ioctlsocket(ListenSocket, FIONBIO, &on);
	if (nbretval == SOCKET_ERROR)
	{
		nbretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"nonblokcin error: %d\n", nbretval);
	}

	//SO_LINGER
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 0;
	lgretval = setsockopt(ListenSocket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));
	if (lgretval == SOCKET_ERROR)
	{
		lgretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"linger error: %d\n", lgretval);
	}

	//nodelay
	BOOL optval = TRUE;
	ndretval = setsockopt(ListenSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	if (ndretval == SOCKET_ERROR)
	{
		ndretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"nodelay error: %d\n", ndretval);
	}

	//listen()
	lnretval = listen(ListenSocket, SOMAXCONN);
	if (lnretval == SOCKET_ERROR)
	{
		lnretval = WSAGetLastError();
		_LOG(LOG_LEVEL_SYSTEM, L"listen error: %d\n", lnretval);
	}

}

bool PacketProc(Session* session, SBuffer* buf)
{
	HEADER header;
	buf->GetData((char*)&header, sizeof(HEADER));
	switch (header.type)
	{
	case dfPACKET_CS_MOVE_START:
	{
		return PacketProc_MoveStart(session, buf);
		break;
	}
	case dfPACKET_CS_MOVE_STOP:
	{
		return PacketProc_MoveStop(session, buf);
		break;
	}
	case dfPACKET_CS_ATTACK1:
	{
		return PacketProc_Attack1(session, buf);
		break;
	}
	case dfPACKET_CS_ATTACK2:
	{
		return PacketProc_Attack2(session, buf);
		break;
	}
	case dfPACKET_CS_ATTACK3:
	{
		return PacketProc_Attack3(session, buf);
		break;
	}
	case dfPACKET_CS_ECHO:
	{
		return PacketProc_Echo(session, buf);
	}

	return TRUE;
	}
}

//PACKET_SCCREATEME 생성
void mpSCCREATEME(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 10;
	header.type = dfPACKET_SC_CREATE_MY_CHARACTER;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y << hp;
}

//PACKET_SCCREATEOTHER 생성
void mpSCCREATEOTHER(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 10;
	header.type = dfPACKET_SC_CREATE_OTHER_CHARACTER;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y << hp;
}

//PACKET_SCDELETE 생성
void mpSCDELETE(unsigned int id, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 4;
	header.type = dfPACKET_SC_DELETE_CHARACTER;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id;
}


//PACKET_SCMOVESTART 생성
void mpSCMOVESTART(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 9;
	header.type = dfPACKET_SC_MOVE_START;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y;
}

//PACKET_SCMOVESTOP 생성
void mpSCMOVESTOP(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size =9;
	header.type = dfPACKET_SC_MOVE_STOP;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y;
}


//PACKET_SCATTACK1 생성
void mpSCATTACK1(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size =9;
	header.type = dfPACKET_SC_ATTACK1;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y;
}

//PACKET_SCATTACK2 생성
void mpSCATTACK2(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 9;
	header.type = dfPACKET_SC_ATTACK2;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y;
}

//PACKET_SCATTACK3 생성
void mpSCATTACK3(unsigned int id, unsigned char dir, unsigned short x, unsigned short y, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 9;
	header.type = dfPACKET_SC_ATTACK3;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << dir << x << y;
}

//PACKET_SCDAMAGE 생성
void mpSCDAMAGE(unsigned int atk, unsigned int tgt, unsigned char hp, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size =9;
	header.type = dfPACKET_SC_DAMAGE;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << atk << tgt << hp;
}

//PACKET_SCSYNC 생성
void mpSCSYNC(unsigned int id, unsigned short x, unsigned short y, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 8;
	header.type = dfPACKET_SC_SYNC;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << id << x << y;
}

//PACKET_SCSYNC 생성
void mpSCECHO(unsigned int time, SBuffer* buf)
{
	HEADER header;
	header.code = 0x89;
	header.size = 4;
	header.type = dfPACKET_SC_ECHO;

	buf->PutData((char*)&header, sizeof(HEADER));
	*buf << time;
}

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
		Player* player=PlayerMap.find(session->Sessionid)->second;
		Disconnect(player);
	}

}

//클라 기준 주변 섹터에 보내기
void SendAround(Session* session, SBuffer* buf, bool me = false)
{
	Player* player=PlayerMap.find(session->Sessionid)->second;
	//주변 세션 찾기 최대 9개
	SectorAround* sectoraround;
	GetSectorAround(player->CurSector.x, player->CurSector.y, sectoraround);
	for (int i = 0; i < sectoraround->count - 1; i++)
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
	std::list<Player*>::iterator it = Sector[sectoraround->around[sectoraround->count - 1].y][sectoraround->around[sectoraround->count - 1].x].begin();
	for (; it != Sector[sectoraround->around[sectoraround->count - 1].y][sectoraround->around[sectoraround->count - 1].x].end(); it++)
	{
		//날 제외하지 않는다면
		if (me == false)
		{
			Player* player = *it;
			int size = buf->GetDataSize();
			int enqret = player->session->SendQ.Enqueue(buf->GetBufferPtr(), size);
			if (enqret != size)
			{
				Disconnect(player);
			}
		}
		else
		{
			Player* player = *it;
			if (player->session != session)
			{
				int size = buf->GetDataSize();
				int enqret = player->session->SendQ.Enqueue(buf->GetBufferPtr(), size);
				if (enqret != size)
				{
					Disconnect(player);
				}
			}
		}
	}

}

//섹터리스트 받아서 섹터리스트에 들어있는 섹터들에 보내기
void SendSectorList(SectorAround* sectoraround, SBuffer* buf, bool me = false)
{
	for (int i = 0; i < sectoraround->count; i++)
	{
		std::list<Player*>::iterator it = Sector[sectoraround->around[i].y][sectoraround->around[i].x].begin();
		for (; it != Sector[sectoraround->around[i].y][sectoraround->around[i].x].end(); it++)
		{
			Player* player = *it;
			int size = buf->GetDataSize();
			int enqret=player->session->SendQ.Enqueue(buf->GetBufferPtr(), size);
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
	std::unordered_map<SOCKET, Session*>::iterator it=SessionMap.begin();
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
