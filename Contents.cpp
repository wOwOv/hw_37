#include "Messagestub.h"
#include "Struct.h"
#include "SerialBuffer.h"
#include "SendCast.h"
#include "Messageproxy.h"
#include <iostream>



bool ProcCreateMe(Player* player, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp) { return true; }
bool ProcCreateOther(Player* player, unsigned char dir, unsigned short x, unsigned short y, unsigned char hp) { return true; }
bool ProcDelete(Player* player) { return true; }


bool ProcMoveStart(Player* player, unsigned char dir, unsigned short x, unsigned short y)
{

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

		printf("Recv CSMOVESTART id:%d direction:%d x:%d y:%d \n", player->id, dir, x, y);

		//�ش� �÷��̾ ���� ������ ���� ���� ���� ��ü���� send
		BProcMoveStart(player, player->id, player->move, player->x, player->y);

	}
	return true;
}


bool ProcMoveStop(Player* player, unsigned char dir, unsigned short x, unsigned short y)
{
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

		printf("Recv CSMOVESTOP id:%d direction:%d x:%d y:%d \n", player->id, dir, x, y);

		//�ش� �÷��̾ ���� ������ ���� ���� ���� ��ü���� send

		BProcMoveStop(player, player->id, dir, player->x, player->y);

	}
	return true;
}

bool ProcAttack1(Player* player, unsigned char dir, unsigned short x, unsigned short y)
{

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

		BProcAttack1(player, player->id, player->direction, player->x, player->y);

		//������ó��
		AttackPlayer(player, dfPACKET_CS_ATTACK1);

	}
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
		//�ش� �÷��̾� ���� ���� ó��
		player->direction = dir;
		player->x = x;
		player->y = y;

		//�ش� �÷��̾� ���� ���� send()
		BProcAttack2(player, player->id, player->direction, player->x, player->y);

		//������ó��
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
		//�ش� �÷��̾� ���� ���� ó��
		player->direction = dir;
		player->x = x;
		player->y = y;

		//�ش� �÷��̾� ���� ���� send()
		BProcAttack3(player, player->id, player->direction, player->x, player->y);

		//������ó��
		AttackPlayer(player, dfPACKET_CS_ATTACK3);

	}
	return true;
}

bool ProcDamage(Player* player, unsigned int tgt, unsigned char hp) { return true; }

bool ProcEcho(Player* player, unsigned int time)
{


	return true;
}

