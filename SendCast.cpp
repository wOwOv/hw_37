#include <list>
#include "Struct.h"
#include "SerialBuffer.h"
#include "SendCast.h"
#include "Messageproxy.h"
#include "Messagestub.h"
#include "PacketDefine.h"

#define SERVERPORT 5000

using namespace std;

extern list<Player*> PlayerList;
SOCKET listensock;
int PlayerID = 0;

//retval



void networkStartUp()
{
	int scretval;//startup,cleanup retval
	int lscretval;//listensock retval
	int nbretval;//nonblocking retval
	int bdretval;//bind retval
	int lnretval;//listen retval
	int slretval;//select retval
	int atretval;//accept retval
	int sdretval;//send retval
	int rvretval;//recv retval
	int lgretval;//linger retval
	int ndretval;//nodelay retval



	//�����ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		scretval = WSAGetLastError();
		printf("WSAStartup error: %d\n", scretval);

	}

	//listensock todtjd
	listensock = socket(AF_INET, SOCK_STREAM, 0);
	if (listensock == INVALID_SOCKET)
	{
		lscretval = WSAGetLastError();
		printf("socket() error: %d\n", lscretval);

	}

	//listensock ���ε�
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	bdretval = bind(listensock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (bdretval == SOCKET_ERROR)
	{
		bdretval = WSAGetLastError();
		printf("bind error: %d\n", bdretval);

	}

	//listen sock nonblocking
	u_long on = 1;
	nbretval = ioctlsocket(listensock, FIONBIO, &on);
	if (nbretval == SOCKET_ERROR)
	{
		nbretval = WSAGetLastError();
		printf("nonblokcin error: %d\n", nbretval);

	}

	//SO_LINGER
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 0;
	lgretval = setsockopt(listensock, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));
	if (lgretval == SOCKET_ERROR)
	{
		lgretval = WSAGetLastError();
		printf("linger error: %d\n", lgretval);

	}

	//nodelay
	BOOL optval = TRUE;
	ndretval = setsockopt(listensock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	if (ndretval == SOCKET_ERROR)
	{
		ndretval = WSAGetLastError();
		printf("nodelay error: %d\n", ndretval);

	}

	//listen()
	lnretval = listen(listensock, SOMAXCONN);
	if (lnretval == SOCKET_ERROR)
	{
		lnretval = WSAGetLastError();
		printf("listen error: %d\n", lnretval);

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
		printf("closesocket error: %d\n", csretval);
	}
	//��������
	curetval = WSACleanup();
	if (curetval == SOCKET_ERROR)
	{
		curetval = WSAGetLastError();
		printf("cleanup error: %d\n", curetval);
	}



}

void NetworkSelect()
{
	int slretval;//select retval


	//��Ʈ��ũ
//���� �� �ʱ�ȭ
	fd_set readset;
	fd_set writeset;
	FD_ZERO(&readset);
	FD_ZERO(&writeset);
	FD_SET(listensock, &readset);
	for (list<Player*>::iterator it = PlayerList.begin(); it != PlayerList.end(); it++)
	{
		Player* tgtply = *it;
		FD_SET(tgtply->sock, &readset);
		if (tgtply->SendQ.GetUsedSize() > 0)
		{
			FD_SET(tgtply->sock, &writeset);
		}
	}

	//select() ������
	TIMEVAL timevalue;
	timevalue.tv_sec = 0;
	timevalue.tv_usec = 0;
	slretval = select(0, &readset, &writeset, NULL, &timevalue);
	if (slretval == SOCKET_ERROR)
	{
		slretval = WSAGetLastError();
		printf("select error: %d\n", slretval);
	}

	if (slretval > 0)
	{
		//readset�� listensock �ִ��� Ȯ��
		if (FD_ISSET(listensock, &readset))
		{
			AcceptProc();
			--slretval;
		}

		//readset�� writeset�� clientsock�ִ��� Ȯ��
		list<Player*>::iterator iter = PlayerList.begin();
		for (; iter != PlayerList.end(); iter++)
		{
			Player* tgtply = *iter;
			//readset�� �ִ��� Ȯ��
			if (FD_ISSET(tgtply->sock, &readset))
			{
				if (tgtply->remove == false)
				{
					--slretval;
					RecvProc(tgtply);
				}
			}
			//writeset�� �ִ��� Ȯ��
			if (FD_ISSET(tgtply->sock, &writeset))
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

//��α�ť���� �̾ƿ���
void AcceptProc()
{
	int atretval;//accept retval


	//clientsock ����
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);

	//���� ����
	Player* player = new Player;
	player->sock = accept(listensock, (SOCKADDR*)&clientaddr, &addrlen);
	if (player->sock == INVALID_SOCKET)
	{
		atretval = WSAGetLastError();
		printf("accept error: %d\n", atretval);
	}
	else
	{
		player->ip = clientaddr.sin_addr;
		player->port = clientaddr.sin_port;
		player->id = PlayerID;
		player->direction = dfPACKET_MOVE_DIR_RR;
		player->x = rand() % 500 + 50;//320;
		player->y = rand() % 300 + 100;//260;
		player->hp = 100;
		player->move = -1;
		player->remove = false;
		++PlayerID;

		//�ű������� ���� �����޽��� send
		UProcCreateMe(player, player->id, player->direction, player->x, player->y, player->hp);


		//�ű������ڿ��� ���� �����ڵ� �����޽��� send
		list<Player*>::iterator it = PlayerList.begin();
		for (; it != PlayerList.end(); it++)
		{
			Player* tgtply = *it;
			UProcCreateOther(player, tgtply->id, tgtply->direction, tgtply->x, tgtply->y, tgtply->hp);


			//�ű������ڿ��� ���� �����ڵ��� �����̰� �־��ٸ� �װſ� ���� MOVESTART�޽������� send
			if (tgtply->move != -1)
			{

				UProcMoveStart(player, tgtply->id, tgtply->move, tgtply->x, tgtply->y);

			}


		}





		//�ű������� PlayerList�� �ֱ�
		PlayerList.push_back(player);

		//���������ڿ��� �ű������� �����޽��� send
		BProcCreateOther(player, player->id, player->direction, player->x, player->y, player->hp);

	}
}



//client�� ������ �޽��� ó��
void RecvProc(Player* player)
{
	int rvretval;//recv retval

	if (player->remove == false)
	{
		//RecvQ�� �ִ� directsize��ŭ recv
		int directsize = player->RecvQ.DirectEnqueueSize();
		rvretval = recv(player->sock, player->RecvQ.GetRearBufferPtr(), directsize, 0);
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
			int movret = player->RecvQ.MoveRear(rvretval);
			if (movret != rvretval)
			{
				DebugBreak();
			}
			SBuffer msgbuf;
			while (1)
			{
				if (player->RecvQ.GetUsedSize() < sizeof(HEADER))
				{
					break;
				}
				msgbuf.Clear();
				HEADER header;
				int pkret = player->RecvQ.Peek((char*)&header, sizeof(HEADER));
				if (pkret != sizeof(HEADER))
				{
					DebugBreak();
				}
				if (header.code != 0x89)
				{
					Disconnect(player);
					break;
				}
				if (player->RecvQ.GetUsedSize() < header.size + sizeof(HEADER))
				{
					break;
				}

				int deqret = player->RecvQ.Dequeue(msgbuf.GetBufferPtr(), header.size + sizeof(HEADER));
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

		int usedsize = player->SendQ.GetUsedSize();
		if (usedsize >= 0)
		{
			int directsize = player->SendQ.DirectDequeueSize();
			sdretval = send(player->sock, player->SendQ.GetFrontBufferPtr(), directsize, 0);
			if (sdretval == SOCKET_ERROR)
			{
				Disconnect(player);
			}
			else
			{
				player->SendQ.MoveFront(sdretval);
			}

		}
	}
}








//player���Ը� sendQ�� Enqueue
void SendUnicast(Player* player, SBuffer* buf)
{
	if (player->remove == false)
	{
		int size = buf->GetDataSize();
		int enqret = player->SendQ.Enqueue(buf->GetBufferPtr(), size);
		if (enqret != size)
		{
			Disconnect(player);
		}
	}
}

//player�����ϰ� ��üsendQ�� Enqueue
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
				int enqret = (*it)->SendQ.Enqueue(buf->GetBufferPtr(), size);
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
					int enqret = (*it)->SendQ.Enqueue(buf->GetBufferPtr(), size);
					if (enqret != size)
					{
						Disconnect(player);
					}
				}
			}
		}

	}
}

void Disconnect(Player* player)
{
	//list���� ���� ���� ǥ��
	player->remove = true;

	//player ���� �޽��� Broadcast
	SBuffer buf;
	BProcDelete(player,player->id);

}

//������ ���� player ã�� damage��Ŷ ������
void AttackPlayer(Player* player, unsigned char type)
{
	if (player->remove == false)
	{
		switch (type)
		{
		case dfPACKET_CS_ATTACK1:
		{
			list<Player*>::iterator tgit = PlayerList.begin();
			if (player->direction == dfPACKET_MOVE_DIR_LL)
			{
				for (; tgit != PlayerList.end(); tgit++)
				{
					if ((*tgit)->id != player->id && (*tgit)->remove == false)
					{
						Player* tgtply = *tgit;
						if (tgtply->x <= player->x)
						{
							if ((player->x - tgtply->x) < dfATTACK1_RANGE_X && abs(player->y - tgtply->y) < dfATTACK1_RANGE_Y)
							{
								//hpó�� �� �޽��� ����� ��ü send
								tgtply->hp -= 1;
								if (tgtply->hp <= 0)
								{
									Disconnect(tgtply);
								}


								RBProcDamage(player->id, tgtply->id, tgtply->hp);

								break;
							}
						}
					}
				}
			}
			else if (player->direction == dfPACKET_MOVE_DIR_RR)
			{
				for (; tgit != PlayerList.end(); tgit++)
				{
					if ((*tgit)->id != player->id && (*tgit)->remove == false)
					{
						Player* tgtply = *tgit;
						if (tgtply->x >= player->x)
						{
							if ((tgtply->x - player->x) < dfATTACK1_RANGE_X && abs(tgtply->y - player->y) < dfATTACK1_RANGE_Y)
							{
								//hpó�� �� �޽��� ����� ��ü send
								tgtply->hp -= 1;

								if (tgtply->hp <= 0)
								{
									Disconnect(tgtply);
								}

								RBProcDamage(player->id, tgtply->id, tgtply->hp);

								break;
							}
						}
					}
				}
			}
			break;
		}
		case dfPACKET_CS_ATTACK2:
		{
			list<Player*>::iterator tgit = PlayerList.begin();
			if (player->direction == dfPACKET_MOVE_DIR_LL)
			{
				for (; tgit != PlayerList.end(); tgit++)
				{
					if ((*tgit)->id != player->id && (*tgit)->remove == false)
					{
						Player* tgtply = *tgit;
						if (tgtply->x <= player->x)
						{
							if ((player->x - tgtply->x) < dfATTACK2_RANGE_X && abs(player->y - tgtply->y) < dfATTACK2_RANGE_Y)
							{
								//hpó�� �� �޽��� ����� ��ü send
								tgtply->hp -= 2;

								if (tgtply->hp <= 0)
								{
									Disconnect(tgtply);
								}

								RBProcDamage(player->id, tgtply->id, tgtply->hp);

								break;
							}
						}
					}
				}
			}
			else if (player->direction == dfPACKET_MOVE_DIR_RR)
			{
				for (; tgit != PlayerList.end(); tgit++)
				{
					if ((*tgit)->id != player->id && (*tgit)->remove == false)
					{
						Player* tgtply = *tgit;
						if (tgtply->x >= player->x)
						{
							if ((tgtply->x - player->x) < dfATTACK2_RANGE_X && abs(tgtply->y - player->y) < dfATTACK2_RANGE_Y)
							{
								//hpó�� �� �޽��� ����� ��ü send
								tgtply->hp -= 2;

								if (tgtply->hp <= 0)
								{
									Disconnect(tgtply);
								}

								RBProcDamage( player->id, tgtply->id, tgtply->hp);

								break;
							}
						}
					}
				}
			}
			break;
		}
		case dfPACKET_CS_ATTACK3:
		{
			list<Player*>::iterator tgit = PlayerList.begin();
			if (player->direction == dfPACKET_MOVE_DIR_LL)
			{
				for (; tgit != PlayerList.end(); tgit++)
				{
					if ((*tgit)->id != player->id && (*tgit)->remove == false)
					{
						Player* tgtply = *tgit;
						if (tgtply->x <= player->x)
						{
							if ((player->x - tgtply->x) < dfATTACK3_RANGE_X && abs(player->y - tgtply->y) < dfATTACK3_RANGE_Y)
							{
								//hpó�� �� �޽��� ����� ��ü send
								tgtply->hp -= 3;

								if (tgtply->hp <= 0)
								{
									Disconnect(tgtply);
								}

								RBProcDamage( player->id, tgtply->id, tgtply->hp);

								break;
							}
						}
					}
				}
			}
			else if (player->direction == dfPACKET_MOVE_DIR_RR)
			{
				for (; tgit != PlayerList.end(); tgit++)
				{
					if ((*tgit)->id != player->id && (*tgit)->remove == false)
					{
						Player* tgtply = *tgit;
						if (tgtply->x >= player->x)
						{
							if ((tgtply->x - player->x) < dfATTACK3_RANGE_X && abs(tgtply->y - player->y) < dfATTACK3_RANGE_Y)
							{
								//hpó�� �� �޽��� ����� ��ü send
								tgtply->hp -= 3;

								if (tgtply->hp <= 0)
								{
									Disconnect(tgtply);
								}

								RBProcDamage(player->id, tgtply->id, tgtply->hp);

								break;
							}
						}
					}
				}
			}
			break;
		}
		}
	}
}