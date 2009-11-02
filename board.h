#ifndef BOARD_H_
#define BOARD_H_

#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cctype>

typedef unsigned long long ull;

#define INITIAL_POS_PAWN_W 0x000000000000FF00LL
#define INITIAL_POS_PAWN_B (INITIAL_POS_PAWN_W<<40)
#define INITIAL_POS_KNIGHT_W 0x42LL
#define	INITIAL_POS_KNIGHT_B (INITIAL_POS_KNIGHT_W<<56)
#define INITIAL_POS_BISHOP_W 0x24LL
#define INITIAL_POS_BISHOP_B (INITIAL_POS_BISHOP_W<<56)
#define INITIAL_POS_ROOK_W 0x81LL
#define INITIAL_POS_ROOK_B (INITIAL_POS_ROOK_W<<56)
#define INITIAL_POS_QUEEN_W 0x10LL
#define INITIAL_POS_QUEEN_B (INITIAL_POS_QUEEN_W<<56)
#define INITIAL_POS_KING_W 0x8LL
#define INITIAL_POS_KING_B (INITIAL_POS_KING_W<<56)

#define INITIAL_POS_WHITE_PIECES  \
    (INITIAL_POS_PAWN_W | INITIAL_POS_KNIGHT_W | INITIAL_POS_BISHOP_W | \
     INITIAL_POS_ROOK_W | INITIAL_POS_QUEEN_W  | INITIAL_POS_KING_W)
#define INITIAL_POS_BLACK_PIECES \
    ((INITIAL_POS_PAWN_W | INITIAL_POS_KNIGHT_W | INITIAL_POS_BISHOP_W | \
      INITIAL_POS_ROOK_W | INITIAL_POS_QUEEN_W  | INITIAL_POS_KING_W) << 48)

#include<map>
#include<vector>
#include<algorithm>
using namespace std;

class Move;

class Board
{
    public:
		ull bb[14];
		int player;
		short int enPassant;
		int check;
		unsigned char castling;
    // bit1 : rocada pe partea regelui pt alb
    // bit2 : rocada pe partea reginei pt alb
    // bit3 : rocada pe partea regelui pt negru
    // bit4 : rocada pe partea reginei pt negru
    
		void InitChessboard();
		void PrintBoard(FILE *fout = stdout) const;
		void PrintBitBoard(const ull bitboard) const;
		int GetPieceType(const int pos) const;
		int GetPieceCount() const;
		int MakeMove(const Move mv);
		vector<Move> GetMoves() const;
		ull GetOccupancy() const;
		int WeGiveCheckOrMate(const Move mv) const;
		bool VerifyChess(const ull pos, const int side) const;
		void SaveBoard(Board &brd) const;
		void LoadBoard(const Board &brd);
    
    Board& operator=(const Board &brd);
    
    private:
		void applyCastling(pair<int, int>, pair<int, int>);
		bool appendMoves(vector<Move> &m, ull att, const int piece, const int source) const;
		bool validCastling(const int side) const;
};


///////////////////////////////////////////////////////////////
// functii si defineuri pentru lucru pe biti
// functii pentru a afla Least Semnificative Bit si Most Semnificative Bit
// LSBb intoarce un ull doar cu bitul cel mai nesemnificativ setat

#define CLEAR_BIT_UCHAR(x, index) ((x) &= ~((unsigned char) 1 << ((unsigned char) index)))

#define CLEAR_BIT(bb, index)    ((bb) &= ~((ull) 1 << ((ull) index)))
#define SET_BIT(bb, index)      ((bb) |= ((ull) 1 << ((ull) index)))

#define IS_SET_BIT(bb, index)       ((bb) & ((ull) 1 << ((ull) index)))

#define LSBb(x) (((x)^((x)-1)) & (x))
int LSB(const ull b);
int MSB(const ull b);

// bit functions used for iterating through bits
// NOTE: if all bits are 0, LSB/MSB returns 64 and CLEAR_BIT sets an existing 0 bit to 0.
//       1<<64 couses overflow, but who cares?
inline int LSBi(ull &b) {int ret = LSB(b); if (ret!=64) CLEAR_BIT(b, ret); return ret;}
inline int MSBi(ull &b) {int ret = MSB(b); if (ret!=64) CLEAR_BIT(b, ret); return ret;}

// conversie coordonate linie+coloana <--> index bitboard
inline ull COORDS_TO_INDEX(const int x, const int y) {return ((x) << 3) + (y);}
inline pair<int, int> INDEX_TO_COORDS(const ull pos) {return make_pair(pos/8, pos%8);}

#endif
