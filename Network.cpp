#include <winsock2.h>
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

bool PacketProc(Player* player, SBuffer* buf)
{
	HEADER header;
	buf->GetData((char*)&header, sizeof(HEADER));
	switch (header.type)
	{
	case dfPACKET_CS_MOVE_START:
	{
		return PacketProc_MoveStart(player, buf);
		break;
	}
	case dfPACKET_CS_MOVE_STOP:
	{
		return PacketProc_MoveStop(player, buf);
		break;
	}
	case dfPACKET_CS_ATTACK1:
	{
		return PacketProc_Attack1(player, buf);
		break;
	}
	case dfPACKET_CS_ATTACK2:
	{
		return PacketProc_Attack2(player, buf);
		break;
	}
	case dfPACKET_CS_ATTACK3:
	{
		return PacketProc_Attack3(player, buf);
		break;
	}

	return TRUE;
	}
}

