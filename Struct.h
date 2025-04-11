#ifndef __STRUCTDEF__
#define __STRUCTDEF__

#include <winsock2.h>
#include "RingBuffer.h"

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

//---------------------------------------------------------------
#define dfPACKET_MOVE_DIR_LL					0
#define dfPACKET_MOVE_DIR_LU					1
#define dfPACKET_MOVE_DIR_UU					2
#define dfPACKET_MOVE_DIR_RU					3
#define dfPACKET_MOVE_DIR_RR					4
#define dfPACKET_MOVE_DIR_RD					5
#define dfPACKET_MOVE_DIR_DD					6
#define dfPACKET_MOVE_DIR_LD					7

#define	dfPACKET_SC_CREATE_MY_CHARACTER			0
#define	dfPACKET_SC_CREATE_OTHER_CHARACTER		1
#define	dfPACKET_SC_DELETE_CHARACTER			2
#define	dfPACKET_CS_MOVE_START					10
#define	dfPACKET_SC_MOVE_START					11
#define	dfPACKET_CS_MOVE_STOP					12
#define	dfPACKET_SC_MOVE_STOP					13
#define	dfPACKET_CS_ATTACK1						20
#define	dfPACKET_SC_ATTACK1						21
#define	dfPACKET_CS_ATTACK2						22
#define	dfPACKET_SC_ATTACK2						23
#define	dfPACKET_CS_ATTACK3						24
#define	dfPACKET_SC_ATTACK3						25
#define	dfPACKET_SC_DAMAGE						30










struct Player
{
	SOCKET sock;
	IN_ADDR ip;
	u_short port;
	RingBuffer RecvQ;
	RingBuffer SendQ;

	unsigned int id;
	unsigned char direction;
	unsigned short x;
	unsigned short y;
	signed char hp;

	signed char move;
	bool remove;
};

#endif