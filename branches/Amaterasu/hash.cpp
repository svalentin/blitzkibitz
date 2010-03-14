#include<stdio.h>
#include<vector>
#include"random.h"
#include"constants.h"
#include"bitboards.h"
#include"board.h"
#include"hash.h"

U64 zobristPieces[14][64], zobristSide, zobristCastling[4], zobristEnPassant[9];
HElem HTable[HASH_SIZE];

// Initialize the hashing numbers and other hash initialization stuff
void InitHash()
{
	for (int i=0; i<14; ++i)
		for (int j=0; j<64; ++j)
			zobristPieces[i][j] = GetRandInt64();
	zobristSide = GetRandInt64();
	for (int i=0; i<4; ++i)
		zobristCastling[i] = GetRandInt64();
	for (int i=0; i<9; ++i)
		zobristEnPassant[i] = GetRandInt64();
}

// Calculat a Zorbist Key from a given board and the color to move
// should not be used very often
// the key should be maintained in the Board class with MakeMove
U64 GetZobristKey(const Board *GameBoard,const int iColor)
{
	int sq;
	U64 key=0;
	
	// white pieces
	for (int i=PAWNS_W; i<=KING_W; ++i)
	{
		U64 aux = GameBoard->bbPieceBoards[i];
		FOR_BIT(aux,sq)
			key ^= zobristPieces[i][sq];
	}
	
	// black pieces
	for (int i=PAWNS_B; i<=KING_B; ++i)
	{
		U64 aux =GameBoard->bbPieceBoards[i];
		FOR_BIT(aux,sq)
			key ^= zobristPieces[i][sq];
	}

	// castling flags
	U64 aux = GameBoard->chCastlingRights;
	FOR_BIT(aux,sq)
		key ^= zobristCastling[sq];
	
	// enPassant
	//key ^= zobristEnPassant[board.enPassant];
	
	// who moves
	if (iColor) key ^= zobristSide;
	
	return key;
}

U64 GetPawnZobristKey(const Board *GameBoard)
{
	int sq;
	U64 aux, key=0;
	
	aux = GameBoard->bbPieceBoards[PAWNS_W];
	FOR_BIT(aux,sq)
		key ^= zobristPieces[13][sq];
	
	aux = GameBoard->bbPieceBoards[PAWNS_B];
	FOR_BIT(aux,sq)
		key ^= zobristPieces[13][sq];
	return key;
}

void SetHElem(const int index, const HElem elem)
{
	// Replace key if table position is empty OR
	// if number of pieces in stored table is greater or equal to new elem OR
	// if calculated depth in table is less than the new elem depth
	
	/*if (HTable[index].npiece == 0 ||
		(HTable[index].npiece && HTable[index].npiece >= elem.npiece) ||
		(HTable[index].depth < elem.depth))*/
	{
		
		HTable[index] = elem;
	}
}
