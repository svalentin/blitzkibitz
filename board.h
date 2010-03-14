#ifndef _BOARD_H_
#define _BOARD_H_

#define INITIAL_POS_PAWNS_W			0x000000000000FF00LL
#define INITIAL_POS_PAWNS_B			(INITIAL_POS_PAWNS_W<<40)
#define INITIAL_POS_KNIGHTS_W		0x42LL
#define	INITIAL_POS_KNIGHTS_B		(INITIAL_POS_KNIGHTS_W<<56)
#define INITIAL_POS_BISHOPS_W		0x24LL
#define INITIAL_POS_BISHOPS_B		(INITIAL_POS_BISHOPS_W<<56)
#define INITIAL_POS_ROOKS_W			0x81LL
#define INITIAL_POS_ROOKS_B			(INITIAL_POS_ROOKS_W<<56)
#define INITIAL_POS_QUEEN_W			0x10LL
#define INITIAL_POS_QUEEN_B			(INITIAL_POS_QUEEN_W<<56)
#define INITIAL_POS_KING_W			0x8LL
#define INITIAL_POS_KING_B			(INITIAL_POS_KING_W<<56)
#define INITIAL_POS_WHITE_PIECES	(INITIAL_POS_PAWNS_W|INITIAL_POS_KNIGHTS_W|INITIAL_POS_BISHOPS_W|INITIAL_POS_ROOKS_W|INITIAL_POS_QUEEN_W|INITIAL_POS_KING_W)
#define INITIAL_POS_BLACK_PIECES	((INITIAL_POS_PAWNS_W|INITIAL_POS_KNIGHTS_W|INITIAL_POS_BISHOPS_W|INITIAL_POS_ROOKS_W|INITIAL_POS_QUEEN_W|INITIAL_POS_KING_W)<<48)

#define SQUARE_FREE(bbBoard,iIndex)						((~((U64)bbBoard))&((U64)1<<(iIndex)))

#define CHECK_ENPAS(shEnpas,iIndex,iColor)				((shEnpas)&(1<<((iIndex)+iColor*8)))

#define CHECK_KING_CASTLING(chCastlingRights,iColor)	(((chCastlingRights)&(0x03<<(4*iColor)))==0x00)
#define CHECK_QUEEN_CASTLING(chCastlingRights,iColor)	(((chCastlingRights)&(0x0A<<(4*iColor)))==0x00)

#define REVOKE_KING_CASTLING(chCastlingRights,iColor)	((chCastlingRights)|=(0x01<<(4*iColor)))
#define REVOKE_QUEEN_CASTLING(chCastlingRights,iColor)	((chCastlingRights)|=(0x08<<(4*iColor)))
#define REVOKE_CASTLING(chCastlingRights,iColor)		((chCastlingRights)|=(0x0F<<(4*iColor)))

#define KING_IN_CHECK(bbKing,bbEnemyAttacks)			((U64)((bbKing)&(bbEnemyAttacks)))

class Move;

class Board
{
	public:
		U64 bbPieceBoards[14];
		U64 ullBoardZKey;
		U64 ullPawnZKey;
		unsigned short shEnPassant;
		unsigned char chCastlingRights;
		// first 16 bits en pessant rights in white/black order
		// next 4 bits castling rights in white-black/king-queen-side order

		const int GetPieceType(const int pos) const;
		const int GetPieceCount() const;
		const U64 GetOccupancy() const;
		const U64 GetAttackBoard(const int iColor) const;
		const U64 GetSideOccupancy(const int iColor) const;
		const U64 GetPieceBoard(const int iType,const int iColor) const;

		const U64 GetBoardZKey(const int iColor) const;
		const U64 GetPawnZKey() const;

		const void AppendMoves(std::vector<Move>& vMoves,U64 bbMoveBoard,const int iType,const int iSrc,const int iColor) const;
		const void AppendPawnNormalMoves(std::vector<Move>&vMoves,U64 bbMoveBoard,const int iSrc,const int iColor) const;
		const void AppendPawnAttackMoves(std::vector<Move>&vMoves,U64 bbMoveBoard,const int iSrc,const int iColor) const;

		const std::vector<Move> GetMoves(const int iColor) const;

		const void MakeMove(Move const * const mv);
};

void InitChessboard(Board*board);
void PrintBoard(Board*board,FILE *fout = stdout);
void PrintBitBoard(const U64 bitboard,FILE *fout = stdout);

#endif
