#ifndef _HASH_H_
#define _HASH_H_

const int HASH_SIZE = 3500017;
extern U64 zobristPieces[14][64], zobristSide, zobristCastling[4], zobristEnPassant[9];

class HElem
{
	public:
	U64 zkey;				// zobrist key
	U64 pzkey;				// pawn zobrist key
	bool exact;				// true = exact value   |   false = check lbound
	bool lbound;			// true = lowerbound	|   false = upperbound
	int score;				// evaluated score
	unsigned char depth;	// depth calculated from this point
	unsigned char npiece;	// number of pieces left on the board
	
	// Constructors
	HElem()
	{
		zkey = score = depth = npiece = 0;
		exact = lbound = false;
	}
	HElem(U64 zkey, bool exact, bool lbound, int score,unsigned char depth, unsigned char npiece)
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
U64 GetZobristKey(const Board *GameBoard,const int iColor);
U64 GetPawnZobristKey(const Board *board);

inline int GetHashIndex(const U64 key)
{
	return (int)(key % HASH_SIZE);
}

inline int GetHashIndexFromBoard(const Board *GameBoard,const int iColor)
{
	return GetHashIndex(GetZobristKey(GameBoard,iColor));
}

inline HElem GetHElem(const int index)
{
	return HTable[index];
}

void SetHElem(const int index, const HElem elem);

#endif // _HASH_H_