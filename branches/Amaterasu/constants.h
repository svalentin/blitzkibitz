#ifndef _GAME_CONSTANTS_H_
#define _GAME_CONSTANTS_H_

#ifndef __64_BIT_INTEGER_DEFINED__
	#define __64_BIT_INTEGER_DEFINED__
	#if defined(_MSC_VER)
		typedef unsigned __int64 U64;
	#else
		typedef unsigned long long  U64;
	#endif
#endif

#define WHITE 0
#define BLACK 1

#define MAX_DEPTH				16
#define	DEPTH_REDUCTION			2

#define PAWN					0
#define KNIGHT					1
#define BISHOP					2
#define ROOK					3
#define QUEEN					4
#define KING					5

#define PAWNS_W					0
#define KNIGHTS_W				1
#define BISHOPS_W				2
#define ROOKS_W					3
#define QUEEN_W					4
#define KING_W					5
#define WHITE_PIECES			6

#define PAWNS_B					7
#define KNIGHTS_B				8
#define BISHOPS_B				9
#define ROOKS_B					10
#define QUEEN_B					11
#define KING_B					12
#define BLACK_PIECES			13

#define	INFINITY	65000
#define WIN			INFINITY
#define LOSS		-INFINITY
#define	DRAW		0

extern int PieceValue[6];
extern int ColorScore[2];

#endif
