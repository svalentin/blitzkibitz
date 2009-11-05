#ifndef PIECES_H_
#define PIECES_H_

#include "board.h"
#include "magic.h"

#define PAWN_W				0
#define PAWN_B				7
#define KNIGHT_W			1
#define KNIGHT_B			8
#define BISHOP_W			2
#define BISHOP_B			9
#define ROOK_W				3
#define ROOK_B				10
#define QUEEN_W				4
#define QUEEN_B				11
#define KING_W				5
#define KING_B				12
#define WHITE_PIECE			6
#define BLACK_PIECE			13


// Move Flags
#define QUEEN_SIDE_CASTLE	1
#define KING_SIDE_CASTLE	2
#define ENPASS				4
#define CHECK				8
#define MATE				16
#define DRAW				32
#define CAPTURE				64
#define PROMOTION			128
#define NO_Q_CASTLE			256
#define NO_K_CASTLE			512
#define ERROR				1024


#include<map>
#include<algorithm>
using namespace std;

typedef unsigned long long ull;

class Board;

extern map<char, int> PieceMap;
extern map<int, char> PieceIndexMap;

class Move
{
	public:
	int source;			// source bit index on a 64-bit board
	int destination;	// destination bit index on a 64-bit board
	char piece;			// character to mark the piece type // FIXME: should we make int?
	int flags;
	int check;
	char promote_to;	// character to mark the pawn promotion piece
	int player;			// 0 = WHITE  and  1 = BLACK
	bool FindCoordinates(Board &b);
	
	Move()
	{
		source = destination = piece = flags = check = promote_to = player = 0;
	}
	
	const bool operator<(const Move &b) const
	{
		if (b.check == MATE) return false;
		if (check == MATE) return true;

		if (b.check == CHECK) return false;
		if (check == CHECK) return true;

		if (flags == CAPTURE && b.flags == CAPTURE) {
			return PieceMap[piece] < PieceMap[b.piece];
		}
		else {
			if (flags != CAPTURE && b.flags != CAPTURE) {
				if (promote_to == 0 && b.promote_to == 0) {
					return PieceMap[piece] < PieceMap[b.piece];
				}
				else return promote_to > b.promote_to;
			}
			else return (flags == CAPTURE && b.flags != CAPTURE);
		}
	}
};

/// Checks if a character is a valid notation for a piece
inline bool IsPiece(const char c) {return (PieceMap.find(c) != PieceMap.end());}

void initPieces();

#endif
