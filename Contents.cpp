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
		//player ������ ���� ����
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

		//�ش� �÷��̾ ���� ������ ���� ���� ���� ��ü���� send
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
		//player ������ ���� ����
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

		//�ش� �÷��̾ ���� ������ ���� ���� ���� ��ü���� send
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
		//�ش� �÷��̾� ���� ���� ó��
		player->direction = dir;
		player->x = x;
		player->y = y;

		//�ش� �÷��̾� ���� ���� send()
		buf->Clear();

		mpSCATTACK1(player->id, player->direction, player->x, player->y, buf);

		SendBroadcast(player, buf);

		buf->MoveReadPos(sizeof(HEADER));
		unsigned int mid = 0;
		unsigned char mdir = 0;
		unsigned short mx = 0;
		unsigned short my = 0;
		_LOG(LOG_LEVEL_DEBUG, L"broadcast attack1 id: %d direction: %d x: %d y: %d\n", mid, mdir, mx, my);

		//������ó��
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
		//�ش� �÷��̾� ���� ���� ó��
		player->direction = dir;
		player->x = x;
		player->y = y;

		//�ش� �÷��̾� ���� ���� send()
		buf->Clear();

		mpSCATTACK2(player->id, player->direction, player->x, player->y, buf);

		SendBroadcast(player, buf);

		buf->MoveReadPos(sizeof(HEADER));
		unsigned int mid = 0;
		unsigned char mdir = 0;
		unsigned short mx = 0;
		unsigned short my = 0;
		_LOG(LOG_LEVEL_DEBUG, L"broadcast attack2 id: %d direction: %d x: %d y: %d\n", mid, mdir, mx, my);

		//������ó��
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
		//�ش� �÷��̾� ���� ���� ó��
		player->direction = dir;
		player->x = x;
		player->y = y;

		//�ش� �÷��̾� ���� ���� send()
		buf->Clear();

		mpSCATTACK3(player->id, player->direction, player->x, player->y, buf);

		SendBroadcast(player, buf);

		buf->MoveReadPos(sizeof(HEADER));
		unsigned int mid = 0;
		unsigned char mdir = 0;
		unsigned short mx = 0;
		unsigned short my = 0;
		_LOG(LOG_LEVEL_DEBUG, L"broadcast attack1 id: %d direction: %d x: %d y: %d\n", mid, mdir, mx, my);

		//������ó��
		AttackPlayer(player, dfPACKET_CS_ATTACK3);

	}
	return true;
}

void Disconnect(Player* player)
{
	//list���� ���� ���� ǥ��
	player->remove = true;

	//player ���� �޽��� Broadcast
	SBuffer buf;
	mpSCDELETE(player->id, &buf);

	SendBroadcast(player, &buf);
}

//Ư�� ���� 1���� ������
void SendSectorOne(int SectorX, int SectorY, SBuffer* buf,Session* session){}
//Ư�� 1���� Ŭ�� ������
void SendUnicast(Session* session, SBuffer* buf, bool me = false) {}
//Ŭ�� ���� �ֺ� ���Ϳ� ������
void SendAround(Session* session, SBuffer* buf) {}
//��¥ ��ε�ĳ����
void SendBroadcast(Session* session, SBuffer* buf) {}
