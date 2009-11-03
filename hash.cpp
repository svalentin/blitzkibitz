#include "hash.h"

HElem HTable[HASH_SIZE];

ull zobristPieces[14][64], zobristPlayer, zobristCastling[4], zobristEnPassant[64];

// Get a random 64-bit number
ull GetRand64()
{
    static int i=256, cnt=0;
    if (i>254) {
        randinit(cnt++);
        isaac();
        i=0;
    }
    return (((ull) randrsl[i++] << 32ULL) | ((ull) randrsl[i++]));
}

// Initialize the hashing numbers and other hash initialization stuff
void InitHash()
{
    for (int i=0; i<14; ++i)
        for (int j=0; j<64; ++j)
            zobristPieces[i][j] = GetRand64();
    zobristPlayer = GetRand64();
    for (int i=0; i<4; ++i)
        zobristCastling[i] = GetRand64();
    for (int i=0; i<64; ++i)
        zobristEnPassant[i] = GetRand64();
}

// Calculat a Zorbist Key from a given board
// should not be used very often
// the key should be maintained in the Board class with MakeMove
ull GetZobristKey(const Board &board)
{
    ull key=0;
    
    // white pieces
    for (int i=PAWN_W; i<=KING_W; ++i) {
        ull aux = board.bb[i];
        for (int pp = LSBi(aux); pp != 64; pp = LSBi(aux))
            key ^= zobristPieces[i][pp];
    }
    
    // black pieces
    for (int i=PAWN_B; i<=KING_B; ++i) {
        ull aux = board.bb[i];
        for (int pp = LSBi(aux); pp != 64; pp = LSBi(aux))
            key ^= zobristPieces[i][pp];
    }

    // castling flags
    ull aux = board.castling;
    for (int pp = LSBi(aux); pp != 64; pp = LSBi(aux))
        key ^= zobristCastling[pp];
    
    // enPassant
    key ^= zobristEnPassant[board.enPassant];
    
    // who moves
    if (board.player) key ^= zobristPlayer;
    
    return key;
}

ull GetPawnZobristKey(const Board &board)
{
    ull aux, key=0;
    aux = board.bb[PAWN_W];
    for (int pp = LSBi(aux); pp != 64; pp = LSBi(aux))
        key ^= zobristPieces[13][pp];
    aux = board.bb[PAWN_B];
    for (int pp = LSBi(aux); pp != 64; pp = LSBi(aux))
        key ^= zobristPieces[13][pp];
    return key;
}

HElem SetHElem(const int index, const HElem elem)
{
    // Replace key if table position is empty OR
    // if number of pieces in stored table is greater or equal to new elem OR
    // if calculated depth in table is less than the new elem depth
    
    if (HTable[index].npiece == 0 ||
        (HTable[index].npiece && HTable[index].npiece >= elem.npiece) ||
        (HTable[index].depth < elem.depth)) {
        HTable[index] = elem;
    }
}

