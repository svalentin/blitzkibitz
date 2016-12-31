#ifndef _EVALUATION_H_
#define _EVALUATION_H_

#include "pieces.h"
#include "magic.h"
#include "hash.h"

#define NORAND

///////////////////////////////////////////////////////////////
// Scoring magic

#define PAWN_SCORE		100
#define KNIGHT_SCORE	310
#define BISHOP_SCORE	305
#define ROOK_SCORE		500
#define QUEEN_SCORE		850
#define KING_SCORE		64000

const int PieceScore[6] =
{
	PAWN_SCORE, KNIGHT_SCORE, BISHOP_SCORE,
	ROOK_SCORE, QUEEN_SCORE, KING_SCORE
};

const int AttackScore[64] =
{
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  1,  1,  1,  1,  1,  1,  0,
	0,  1,  8,  8,  8,  8,  1,  0,
	0,  1,  8, 25, 25,  8,  1,  0,
	0,  1,  8, 25, 25,  8,  1,  0,
	0,  1,  8,  8,  8,  8,  1,  0,
	0,  1,  1,  1,  1,  1,  1,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
};

const int IsolatedPawnPenalty[8] = {10, 12, 14, 18, 18, 14, 12, 10};

const int PiecePositionScore[7][64] =
{
	// Pawn scores White
	{
		 0,  0,  0,  0,  0,  0,  0,  0,
		20, 26, 26, 28, 28, 26, 26, 20,
		12, 14, 16, 21, 21, 16, 14, 12,
		 8, 10, 12, 18, 18, 12, 10,  8,
		 4,  6,  8, 16, 16,  8,  6,  4,
		 2,  2,  4,  6,  6,  4,  2,  2,
		 0,  0,  0, -4, -4,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0
	},
	// Knight scores White
	{
		-40, -10, - 5, - 5, - 5, - 5, -10, -40,
		- 5,   5,   5,   5,   5,   5,   5, - 5,
		- 5,   5,  10,  15,  15,  10,   5, - 5,
		- 5,   5,  10,  15,  15,  10,   5, - 5,
		- 5,   5,  10,  15,  15,  10,   5, - 5,
		- 5,   5,   8,   8,   8,   8,   5, - 5,
		- 5,   0,   5,   5,   5,   5,   0, - 5,
		-50, -20, -10, -10, -10, -10, -20, -50,
	},
	// Bishop scores White
	{
		-40, -20, -15, -15, -15, -15, -20, -40,
		  0,   5,   5,   5,   5,   5,   5,   0,
		  0,  10,  10,  18,  18,  10,  10,   0,
		  0,  10,  10,  18,  18,  10,  10,   0,
		  0,   5,  10,  18,  18,  10,   5,   0,
		  0,   0,   5,   5,   5,   5,   0,   0,
		  0,   5,   0,   0,   0,   0,   5,   0,
		-50, -20, -10, -20, -20, -10, -20, -50
	},
	// Rook scores White
	{
		10, 10, 10, 10, 10, 10, 10, 10,
		 5,  5,  5, 10, 10,  5,  5,  5,
		 0,  0,  5, 10, 10,  5,  0,  0,
		 0,  0,  5, 10, 10,  5,  0,  0,
		 0,  0,  5, 10, 10,  5,  0,  0,
		 0,  0,  5, 10, 10,  5,  0,  0,
		 0,  0,  5, 10, 10,  5,  0,  0,
		 0,  0,  5, 10, 10,  5,  0,  0,
	},
	// Queen scores White
	{
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0, 10, 10, 10, 10,  0,  0,
		0,  0, 10, 15, 15, 10,  0,  0,
		0,  0, 10, 15, 15, 10,  0,  0,
		0,  0, 10, 10, 10, 10,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0
	},
	// King scores White
	{
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		12,  8,  4,  0,  0,  4,  8, 12,
		16, 12,  8,  4,  4,  8, 12, 16,
		24, 20, 16, 12, 12, 16, 20, 24,
		24, 24, 24, 16, 16,  6, 32, 32
	},
	// King end-game scores White
	{
		-30, -5,  0,  0,  0,  0,  -5, -30,
		 -5,  0,  0,  0,  0,  0,   0,  -5,
		  0,  0,  0,  0,  0,  0,   0,   0,
		  0,  0,  0,  5,  5,  0,   0,   0,
		  0,  0,  0,  5,  5,  0,   0,   0,
		  0,  0,  0,  0,  0,  0,   0,   0,
		-10,  0,  0,  0,  0,  0,   0, -10,
		-40,-10, -5, -5, -5, -5, -10, -40
	}
};

inline void evalPieces(const Board &cboard, int &wscore, int &bscore);
inline void evalPawn(const Board &cboard, const int gameStage, int &wscore, int &bscore);
inline void evalKing(const Board &cboard, const bool gameStage, int &wscore, int &bscore);
int CalculateScore(const Board &cboard);
int SCalculateScore(const Board &cboard);

#endif
