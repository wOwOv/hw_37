#include "Messagestub.h"
#include "Struct.h"
#include "SerialBuffer.h"
#include "Protocol.h"

bool MessageProc(Player* player, SBuffer* buf)
{
	unsigned char code;
	unsigned char size;
	unsigned char type;
	*buf >> code >> size >> type;
	switch (type)
	{
	case dfPACKET_CS_MOVE_START:
	{
		unsigned char dir = 0;
		unsigned short x = 0;
		unsigned short y = 0;
		*buf >> dir >> x >> y;
		return ProcMoveStart(player, dir, x, y);
		break;
	}
	case dfPACKET_CS_MOVE_STOP:
	{
		unsigned char dir = 0;
		unsigned short x = 0;
		unsigned short y = 0;
		*buf >> dir >> x >> y;
		return ProcMoveStop(player, dir, x, y);
		break;
	}
	case dfPACKET_CS_ATTACK1:
	{
		unsigned char dir = 0;
		unsigned short x = 0;
		unsigned short y = 0;
		*buf >> dir >> x >> y;
		return ProcAttack1(player, dir, x, y);
		break;
	}
	case dfPACKET_CS_ATTACK2:
	{
		unsigned char dir = 0;
		unsigned short x = 0;
		unsigned short y = 0;
		*buf >> dir >> x >> y;
		return ProcAttack2(player, dir, x, y);
		break;
	}
	case dfPACKET_CS_ATTACK3:
	{
		unsigned char dir = 0;
		unsigned short x = 0;
		unsigned short y = 0;
		*buf >> dir >> x >> y;
		return ProcAttack3(player, dir, x, y);
		break;
	}
	case dfPACKET_CS_ECHO:
	{

	}
	default:
	{
		return true;
		break;
	}
	}
}
