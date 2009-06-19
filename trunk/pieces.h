#ifndef PIECES_H_
#define PIECES_H_

#define PAWN_W      0
#define PAWN_B      7
#define KNIGHT_W    1
#define KNIGHT_B    8
#define BISHOP_W    2
#define BISHOP_B    9
#define ROOK_W      3
#define ROOK_B      10
#define QUEEN_W     4
#define QUEEN_B     11
#define KING_W      5
#define KING_B      12
#define WHITE_PIECE 6
#define BLACK_PIECE 13


// Move Flags
#define ROCADA_MARE 1
#define ROCADA_MICA 2
#define ENPASS      4
#define SAH         8
#define MAT         16
#define EGAL        32
#define CAPTURA     64
#define PROMOVARE   128
#define NOTROCMARE  256
#define NOTROCMICA  512
#define ERROR       1024


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
    int source;         // source bit index on a 64-bit board
    int destination;    // destination bit index on a 64-bit board
    char piece;         // character to mark the piece type // FIXME: should we make int?
    int flags;
    int sah;
    char promote_to;    // character to mark the pawn promotion piece
    int player;         // 0 = WHITE  and  1 = BLACK
    bool FindCoordinates(Board &b);
    
    Move()
    {
        source = destination = piece = flags = sah = promote_to = player = 0;
    }
    
    const bool operator<(const Move &b) const
    {
        if (b.sah == MAT) return false;
        if (sah == MAT) return true;

        if (b.sah == SAH) return false;
        if (sah == SAH) return true;

        if (flags == CAPTURA && b.flags == CAPTURA) {
            return PieceMap[piece] < PieceMap[b.piece];
        }
        else {
            if (flags != CAPTURA && b.flags != CAPTURA) {
                if (promote_to == 0 && b.promote_to == 0) {
                    return PieceMap[piece] < PieceMap[b.piece];
                }
                else return promote_to > b.promote_to;
            }
            else return (flags == CAPTURA && b.flags != CAPTURA);
        }
    }
};

/// Checks if a character is a valid notation for a piece
inline bool IsPiece(const char c) {return (PieceMap.find(c) != PieceMap.end());}

void initPieces();

#endif
