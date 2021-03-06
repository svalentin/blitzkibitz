#ifndef HASH_H_
#define HASH_H_

#include "rand.h"
#include "pieces.h"
#include "board.h"

#include <ctime>

typedef unsigned long long ull;

const int HASH_SIZE = 3500017;

class HElem
{
	public:
	ull zkey;				// zobrist key
	ull pzkey;				// pawn zobrist key
	bool exact;				// true = exact value	|	false = check lbound
	bool lbound;			// true = lowerbound	|	false = upperbound
	int score;				// evaluated score
	unsigned char depth;	// depth calculated from this point
	unsigned char npiece;	// number of pieces left on the board

	// Constructors
	HElem() : zkey(0), exact(0), lbound(0), score(0), depth(0), npiece(0) { }
	HElem(ull zkey, bool exact, bool lbound, int score,
		unsigned char depth, unsigned char npiece)
	{
		this->zkey = zkey;
		this->exact = exact;
		this->lbound = lbound;
		this->score = score;
		this->depth = depth;
		this->npiece = npiece;
	}
};

extern HElem HTable[HASH_SIZE];

void InitHash();
ull GetZobristKey(const Board &board);
ull GetPawnZobristKey(const Board &board);

inline int GetHashIndex(const ull key)
{
	return key % HASH_SIZE;
}

inline int GetHashIndexFromBoard(const Board &board)
{
	return GetHashIndex(GetZobristKey(board));
}


inline HElem GetHElem(const int index)
{
	return HTable[index];
}

void SetHElem(const int index, const HElem elem);

#endif
