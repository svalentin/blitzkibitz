#include<stdio.h>
#include<string.h>
#include<vector>
#include<algorithm>
#include"constants.h"
#include"bitboards.h"
#include"board.h"
#include"magic.h"
#include"moves.h"
#include"hash.h"
using namespace std;

/// Function that initialises the board.
/// Places pieces at starting location and other initialisation stuff.
void InitChessboard(Board*board)
{
	memset(board,0,sizeof(Board));
	board->bbPieceBoards[0]	|= INITIAL_POS_PAWNS_W;
	board->bbPieceBoards[7]	|= INITIAL_POS_PAWNS_B;
	board->bbPieceBoards[1]	|= INITIAL_POS_KNIGHTS_W;
	board->bbPieceBoards[8]	|= INITIAL_POS_KNIGHTS_B;
	board->bbPieceBoards[2]	|= INITIAL_POS_BISHOPS_W;
	board->bbPieceBoards[9]	|= INITIAL_POS_BISHOPS_B;
	board->bbPieceBoards[3]	|= INITIAL_POS_ROOKS_W;
	board->bbPieceBoards[10]|= INITIAL_POS_ROOKS_B;
	board->bbPieceBoards[4]	|= INITIAL_POS_QUEEN_W;
	board->bbPieceBoards[11]|= INITIAL_POS_QUEEN_B;
	board->bbPieceBoards[5]	|= INITIAL_POS_KING_W;
	board->bbPieceBoards[12]|= INITIAL_POS_KING_B;
	board->bbPieceBoards[6]	|= INITIAL_POS_WHITE_PIECES;
	board->bbPieceBoards[13]|= INITIAL_POS_BLACK_PIECES;
	board->shEnPassant	= 0;
	board->chCastlingRights	= 0x0;

	int sq;
	for (int i=PAWNS_W; i<=KING_W; ++i)
	{
		U64 aux = board->bbPieceBoards[i];
		FOR_BIT(aux,sq)
			board->ullBoardZKey ^= zobristPieces[i][sq];
	}
	for (int i=PAWNS_B; i<=KING_B; ++i)
	{
		U64 aux =board->bbPieceBoards[i];
		FOR_BIT(aux,sq)
			board->ullBoardZKey ^= zobristPieces[i][sq];
	}
	//board->ullBoardZKey=GetZobristKey(board,WHITE);
	board->ullPawnZKey=GetPawnZobristKey(board);
}

void PrintBoard(Board*board,FILE *fout)
{
	U64 aux=1;
	int i,type,k=7;
	char ch;
	fprintf(fout,"   abcdefgh\n +---------\n8| ");
	for(i=63;i>=0;i--)
	{
		type=board->GetPieceType(i);
		if(type==0) ch='P';
		else if(type==1) ch='N';
		else if(type==2) ch='B';
		else if(type==3) ch='R';
		else if(type==4) ch='Q';
		else if(type==5) ch='K';
		else if(type==7) ch='p';
		else if(type==8) ch='n';
		else if(type==9) ch='b';
		else if(type==10) ch='r';
		else if(type==11) ch='q';
		else if(type==12) ch='k';
		else ch='.';
		fprintf(fout,"%c",ch);
		if(i%8==0 && i) fprintf(fout,"\n%d| ",k--);
	}
	fprintf(fout,"\n");
}

void PrintBitBoard(const U64 bitboard,FILE *fout)
{   
	U64 bit = 1LL<<63;
	for (int i=0; i<8; i++)
	{
		//printf("  "); // whitespace to align with the ShowPieces board
		//fprintf(fout,"  ");
		for (int j=0; j<8; ++j, bit>>=1)
			//printf("%d", (bitboard & bit) ? 1 : 0);
			fprintf(fout,"%d", (bitboard & bit) ? 1 : 0);
		//printf("\n");
		fprintf(fout,"\n");
	}
	fprintf(fout,"\n");
}

const U64 Board::GetBoardZKey(const int iColor) const
{
	int sq;
	U64 aux = chCastlingRights,key=ullBoardZKey;
	FOR_BIT(aux,sq)
		key ^= zobristCastling[sq];
	if(iColor) key^=zobristSide;
	return key;
}

const U64 Board::GetPawnZKey() const
{
	return ullPawnZKey;
}

/// Returns the bitboard with positions occupied by all pieces.
const U64 Board::GetOccupancy() const
{
	return bbPieceBoards[WHITE_PIECES] | bbPieceBoards[BLACK_PIECES];
}

const U64 Board::GetSideOccupancy(const int iColor) const
{
	return bbPieceBoards[WHITE_PIECES+7*iColor];
}

const U64 Board::GetPieceBoard(const int iType,const int iColor) const
{
	return bbPieceBoards[iType+7*iColor];
}

const U64 Board::GetAttackBoard(const int iColor) const
{
	int iSq;
	U64 bbAttackBoard=0,bbAux,bbOcc=GetOccupancy();

	bbAux=bbPieceBoards[PAWN+7*iColor]; // pawn attacks
	FOR_BIT(bbAux,iSq)
	{
		bbAttackBoard|=PAttackMagic(iSq,iColor);
	}

	bbAux=bbPieceBoards[KNIGHT+7*iColor]; // knight attacks
	FOR_BIT(bbAux,iSq)
	{
		bbAttackBoard|=Nmagic(iSq);
	}

	bbAux=bbPieceBoards[BISHOP+7*iColor]; // bishop attacks
	FOR_BIT(bbAux,iSq)
	{
		bbAttackBoard|=Bmagic(iSq,bbOcc);
	}

	bbAux=bbPieceBoards[ROOK+7*iColor]; // rook attacks
	FOR_BIT(bbAux,iSq)
	{
		bbAttackBoard|=Rmagic(iSq,bbOcc);
	}

	bbAux=bbPieceBoards[QUEEN+7*iColor]; // queen attacks
	FOR_BIT(bbAux,iSq)
	{
		bbAttackBoard|=Qmagic(iSq,bbOcc);
	}

	bbAux=bbPieceBoards[KING+7*iColor]; // king attacks
	FOR_BIT(bbAux,iSq)
	{
		bbAttackBoard|=Kmagic(iSq);
	}

	return bbAttackBoard;
}

/// Function that returns the type (an integer) of the piece on the position pos
/// pos is an index to the bit on the board.
const int Board::GetPieceType(const int pos) const
{
	U64 bbPieceBoard = 0;
	bbPieceBoard=PLACE_PIECE(bbPieceBoard,pos);

	// white pieces
	for(int i=0; i<6; ++i)
		if (bbPieceBoard & bbPieceBoards[i])
			return i;

	// black pieces
	for(int i=7; i<13; ++i)
		if (bbPieceBoard & bbPieceBoards[i])
			return i;
	return -1;
}

const int Board::GetPieceCount() const
{
	return Count1s(bbPieceBoards[WHITE_PIECES] | bbPieceBoards[BLACK_PIECES]);
}

const void Board::AppendMoves(vector<Move>& vMoves,U64 bbMoveBoard,const int iType,const int iSrc,const int iColor) const
{
	int iSq,iDestType=21;
	Move mv;
	mv.iSrc=iSrc;
	SET_BIT(mv.iType,iType);
	bbMoveBoard^=(bbPieceBoards[WHITE_PIECES+7*iColor]&bbMoveBoard);

	FOR_BIT(bbMoveBoard,iSq)
	{
		int iFlags=iColor;
		CLEAR_BIT(mv.iType,iDestType+iColor*7);

		iDestType=GetPieceType(iSq);
		mv.iDest=iSq;
		if(iDestType!=-1)
		{
			SET_BIT(mv.iType,iDestType+iColor*7); // it's good...leave it like that
			SET_BITS(iFlags,CAP_INDEX);
		}
		mv.iFlags=iFlags;

		Board board=*this; // Is move legal?
		board.MakeMove(&mv);
		if(KING_IN_CHECK(board.bbPieceBoards[KING_W+7*iColor],board.GetAttackBoard(!iColor))) continue;
		else if(KING_IN_CHECK(board.bbPieceBoards[KING_W+7*(!iColor)],board.GetAttackBoard(iColor)))
		{
			SET_BITS(iFlags,CHECK_INDEX);
			mv.iFlags=iFlags;
		}
		
		vMoves.push_back(mv);
	}
}

const void Board::AppendPawnNormalMoves(vector<Move>&vMoves,U64 bbMoveBoard,const int iSrc,const int iColor) const
{
	int iSq;
	Move mv;
	mv.iSrc=iSrc;
	SET_BIT(mv.iType,PAWN);
	
	bbMoveBoard^=((bbPieceBoards[WHITE_PIECES]|bbPieceBoards[BLACK_PIECES])&bbMoveBoard);
	const int num1s=Count1s(bbMoveBoard);
	if(num1s==1 && abs(LSBi(bbMoveBoard)-iSrc)>8) return;
	FOR_BIT(bbMoveBoard,iSq)
	{
		int iFlags=iColor;

		mv.iDest=iSq;
		if(ROW(iSq)==0 || ROW(iSq)==7) SET_BITS(iFlags,PROM_INDEX);
		if(abs(LSBi(bbMoveBoard)-iSrc)>8) SET_BITS(iFlags,P2SM_INDEX);
		mv.iFlags=iFlags;

		Board board=*this; // Is move legal?
		board.MakeMove(&mv);
		if(KING_IN_CHECK(board.bbPieceBoards[KING_W+7*iColor],board.GetAttackBoard(!iColor))) continue;
		else if(KING_IN_CHECK(board.bbPieceBoards[KING_W+7*(!iColor)],board.GetAttackBoard(iColor)))
		{
			SET_BITS(iFlags,CHECK_INDEX);
			mv.iFlags=iFlags;
		}

		vMoves.push_back(mv);
	}
}

const void Board::AppendPawnAttackMoves(vector<Move>&vMoves,U64 bbMoveBoard,const int iSrc,const int iColor) const
{
	int iSq,iDestType=30;
	U64 bbAttEnpas=0;
	Move mv;
	mv.iSrc=iSrc;
	SET_BIT(mv.iType,PAWN);
	
	// get the normal pawn attacks...if any
	bbMoveBoard&=(bbPieceBoards[WHITE_PIECES+(!iColor)*7]);
	FOR_BIT(bbMoveBoard,iSq)
	{
		CLEAR_BIT(mv.iType,iDestType+iColor*7);
		int iFlags=iColor|CAP_INDEX;

		mv.iDest=iSq;
		iDestType=GetPieceType(iSq);
		if(ROW(iSq)==0 || ROW(iSq)==7) SET_BITS(iFlags,PROM_INDEX);
		SET_BIT(mv.iType,iDestType+iColor*7); // it's good...leave it like that
		mv.iFlags=iFlags;

		Board board=*this; // Is move legal?
		board.MakeMove(&mv);
		if(KING_IN_CHECK(board.bbPieceBoards[KING_W+7*iColor],board.GetAttackBoard(!iColor))) continue;
		else if(KING_IN_CHECK(board.bbPieceBoards[KING_W+7*(!iColor)],board.GetAttackBoard(iColor)))
		{
			SET_BITS(iFlags,CHECK_INDEX);
			mv.iFlags=iFlags;
		}
		
		vMoves.push_back(mv);
	}
	// now get en passant attacks...if any
	bbAttEnpas=PEnpasMagic(iSrc,iColor);
	bbAttEnpas&=(bbPieceBoards[PAWNS_W+(!iColor)*7]);
	SET_BITS(mv.iFlags,EPAS_INDEX);
	FOR_BIT(bbAttEnpas,iSq)
	{
		mv.iDest=iSq+(iColor?-8:8);
		if(CHECK_ENPAS(shEnPassant,iSq%8,!iColor)) // if the pawn on this column is suseptible to en passant
		{
			int iFlags=iColor|CAP_INDEX;
			SET_BIT(mv.iType,PAWNS_B); // set 7th bit so as to indicate a captured pawn (does not imply it's a black pawn)

			Board board=*this; // Is move legal?
			board.MakeMove(&mv);
			if(KING_IN_CHECK(board.bbPieceBoards[KING_W+7*iColor],board.GetAttackBoard(!iColor))) continue;
			else if(KING_IN_CHECK(board.bbPieceBoards[KING_W+7*(!iColor)],board.GetAttackBoard(iColor)))
			{
				SET_BITS(iFlags,CHECK_INDEX);
			}
			
			mv.iFlags=iFlags;
			vMoves.push_back(mv);
		}
	}
}

/// Returns a vector with all valid Moves on the current board.
/// It takes all the information from the internal state of the Board.
const vector<Move> Board::GetMoves(const int iColor) const
{
	Move mv;
	vector<Move> vMoves;
	vMoves.reserve(32);	// reserve memory for 32 moves, so we can avoid ~4 reallocations
	const U64 bbEnemyAttacks=GetAttackBoard(!iColor);
	const U64 bbOcc=GetOccupancy();
	U64 bbMoveBoard=0;
	int iSq;

	mv.iFlags|=iColor;	//+--- sets the corresponding color bit (this way we avoid an if :) )

	bbMoveBoard=bbPieceBoards[PAWN+7*iColor];	// Pawns
	FOR_BIT(bbMoveBoard,iSq)
	{
		AppendPawnAttackMoves(vMoves,PAttackMagic(iSq,iColor),iSq,iColor);
		AppendPawnNormalMoves(vMoves,PNormalMagic(iSq,iColor),iSq,iColor);
	}

	bbMoveBoard=bbPieceBoards[KNIGHT+7*iColor];	// Knights
	FOR_BIT(bbMoveBoard,iSq)
	{
		AppendMoves(vMoves,Nmagic(iSq),KNIGHT,iSq,iColor);
	}

	bbMoveBoard=bbPieceBoards[BISHOP+7*iColor];	// Bishops
	FOR_BIT(bbMoveBoard,iSq)
	{
		AppendMoves(vMoves,Bmagic(iSq,bbOcc),BISHOP,iSq,iColor);
	}

	bbMoveBoard=bbPieceBoards[ROOK+7*iColor];	// Rooks
	FOR_BIT(bbMoveBoard,iSq)
	{
		AppendMoves(vMoves,Rmagic(iSq,bbOcc),ROOK,iSq,iColor);
	}

	bbMoveBoard=bbPieceBoards[QUEEN+7*iColor];	// Queens
	FOR_BIT(bbMoveBoard,iSq)
	{
		AppendMoves(vMoves,Qmagic(iSq,bbOcc),QUEEN,iSq,iColor);
	}

	bbMoveBoard=bbPieceBoards[KING+7*iColor];	// King
	FOR_BIT(bbMoveBoard,iSq)
	{
		AppendMoves(vMoves,Kmagic(iSq),KING,iSq,iColor);
	}

	if(!KING_IN_CHECK(GetPieceBoard(KING,iColor),bbEnemyAttacks))
	{
		if(CHECK_KING_CASTLING(chCastlingRights,iColor)) // king side castle
		{
			if(!(KCastleMagic(iColor)&bbEnemyAttacks) && !(KCastleMagic(iColor)&bbOcc) && (GetPieceBoard(ROOK,iColor)&(((U64)1)<<(iColor*56))))
			{
				SET_BITS(mv.iFlags,KSC_INDEX);
				vMoves.push_back(mv);
				CLEAR_BITS(mv.iFlags,KSC_INDEX);
			}
		}
		if(CHECK_QUEEN_CASTLING(chCastlingRights,iColor)) // queen side castle
		{
			if(!(QCastleMagic(iColor)&bbEnemyAttacks) && !(QCastleMagic(iColor)&bbOcc) && (GetPieceBoard(ROOK,iColor)&(((U64)1)<<(7+iColor*56))))
			{
				SET_BITS(mv.iFlags,QSC_INDEX);
				vMoves.push_back(mv);
				CLEAR_BITS(mv.iFlags,QSC_INDEX);
			}
		}
	}

	return vMoves;
}

const void Board::MakeMove(Move const * const mv)
{
	const int iColor=mv->iFlags&1;
	const int iType=LSBi(mv->iType);

	if(IsKSCastle(mv->iFlags)) // apply king side castle
	{
		REVOKE_CASTLING(chCastlingRights,iColor);

		bbPieceBoards[KING_W+7*iColor]		=REMOVE_PIECE(bbPieceBoards[KING_W+7*iColor],3+iColor*56);
		bbPieceBoards[WHITE_PIECES+7*iColor]=REMOVE_PIECE(bbPieceBoards[WHITE_PIECES+7*iColor],3+iColor*56);
		bbPieceBoards[ROOKS_W+7*iColor]		=REMOVE_PIECE(bbPieceBoards[ROOKS_W+7*iColor],iColor*56);
		bbPieceBoards[WHITE_PIECES+7*iColor]=REMOVE_PIECE(bbPieceBoards[WHITE_PIECES+7*iColor],iColor*56);

		bbPieceBoards[KING_W+7*iColor]		=PLACE_PIECE(bbPieceBoards[KING_W+7*iColor],1+iColor*56);
		bbPieceBoards[WHITE_PIECES+7*iColor]=PLACE_PIECE(bbPieceBoards[WHITE_PIECES+7*iColor],1+iColor*56);
		bbPieceBoards[ROOKS_W+7*iColor]		=PLACE_PIECE(bbPieceBoards[ROOKS_W+7*iColor],2+iColor*56);
		bbPieceBoards[WHITE_PIECES+7*iColor]=PLACE_PIECE(bbPieceBoards[WHITE_PIECES+7*iColor],2+iColor*56);

		ullBoardZKey^=zobristPieces[KING_W+7*iColor][3+iColor*56];
		ullBoardZKey^=zobristPieces[ROOKS_W+7*iColor][iColor*56];

		ullBoardZKey^=zobristPieces[KING_W+7*iColor][1+iColor*56];
		ullBoardZKey^=zobristPieces[ROOKS_W+7*iColor][2+iColor*56];
	}
	else if(IsQSCastle(mv->iFlags)) // apply queen side castle
	{
		REVOKE_CASTLING(chCastlingRights,iColor);

		bbPieceBoards[KING_W+7*iColor]		=REMOVE_PIECE(bbPieceBoards[KING_W+7*iColor],3+iColor*56);
		bbPieceBoards[WHITE_PIECES+7*iColor]=REMOVE_PIECE(bbPieceBoards[WHITE_PIECES+7*iColor],3+iColor*56);
		bbPieceBoards[ROOKS_W+7*iColor]		=REMOVE_PIECE(bbPieceBoards[ROOKS_W+7*iColor],7+iColor*56);
		bbPieceBoards[WHITE_PIECES+7*iColor]=REMOVE_PIECE(bbPieceBoards[WHITE_PIECES+7*iColor],7+iColor*56);

		bbPieceBoards[KING_W+7*iColor]		=PLACE_PIECE(bbPieceBoards[KING_W+7*iColor],5+iColor*56);
		bbPieceBoards[WHITE_PIECES+7*iColor]=PLACE_PIECE(bbPieceBoards[WHITE_PIECES+7*iColor],5+iColor*56);
		bbPieceBoards[ROOKS_W+7*iColor]		=PLACE_PIECE(bbPieceBoards[ROOKS_W+7*iColor],4+iColor*56);
		bbPieceBoards[WHITE_PIECES+7*iColor]=PLACE_PIECE(bbPieceBoards[WHITE_PIECES+7*iColor],4+iColor*56);

		ullBoardZKey^=zobristPieces[KING_W+7*iColor][3+iColor*56];
		ullBoardZKey^=zobristPieces[ROOKS_W+7*iColor][7+iColor*56];

		ullBoardZKey^=zobristPieces[KING_W+7*iColor][5+iColor*56];
		ullBoardZKey^=zobristPieces[ROOKS_W+7*iColor][4+iColor*56];
	}
	else
	{
		if(iType==KING) REVOKE_CASTLING(chCastlingRights,iColor); // if king moved revoke all castling
		else if(iType==ROOK)	// else if a rook moved revoke castling based on rook type ( K or Q rook )
		{
			if(mv->iSrc==iColor*56) REVOKE_KING_CASTLING(chCastlingRights,iColor);
			else if(mv->iSrc==(7+iColor*56)) REVOKE_QUEEN_CASTLING(chCastlingRights,iColor);
		}
		else if(iType==PAWN) ullPawnZKey^=zobristPieces[13][mv->iSrc];

		bbPieceBoards[WHITE_PIECES+7*iColor]=REMOVE_PIECE(bbPieceBoards[WHITE_PIECES+7*iColor],mv->iSrc);
		bbPieceBoards[WHITE_PIECES+7*iColor]=PLACE_PIECE(bbPieceBoards[WHITE_PIECES+7*iColor],mv->iDest);

		bbPieceBoards[iType+7*iColor]=REMOVE_PIECE(bbPieceBoards[iType+7*iColor],mv->iSrc);
		ullBoardZKey^=zobristPieces[iType+7*iColor][mv->iSrc];
		
		if(IsPromotion(mv->iFlags))
		{
			bbPieceBoards[PAWN+7*iColor]=REMOVE_PIECE(bbPieceBoards[PAWN+7*iColor],mv->iDest);
			bbPieceBoards[QUEEN+7*iColor]=PLACE_PIECE(bbPieceBoards[QUEEN+7*iColor],mv->iDest);

			//ullBoardZKey^=zobristPieces[PAWN+7*iColor][mv->iDest];
			//ullPawnZKey^=zobristPieces[PAWN+7*iColor][mv->iDest];
			ullBoardZKey^=zobristPieces[QUEEN+7*iColor][mv->iDest];
		}
		else
		{
			bbPieceBoards[iType+7*iColor]=PLACE_PIECE(bbPieceBoards[iType+7*iColor],mv->iDest);
			ullBoardZKey^=zobristPieces[iType+7*iColor][mv->iDest];
			if(iType==PAWN) ullPawnZKey^=zobristPieces[13][mv->iDest];
		}

		if(IsEnPassant(mv->iFlags)) // it's en passant type capture
		{
			const int iOffset=(iColor?8:(-8));
			bbPieceBoards[WHITE_PIECES+7*(!iColor)]	=REMOVE_PIECE(bbPieceBoards[WHITE_PIECES+7*(!iColor)],mv->iDest+iOffset);
			bbPieceBoards[PAWN+7*(!iColor)]			=REMOVE_PIECE(bbPieceBoards[PAWN+7*(!iColor)],mv->iDest+iOffset);

			ullBoardZKey^=zobristPieces[PAWN+7*(!iColor)][mv->iDest+iOffset];
			ullPawnZKey^=zobristPieces[13][mv->iDest+iOffset];
		}
		else if(IsCapture(mv->iFlags)) // it's a normal capture
		{
			const int iTypeEnemy=LSBi(REMOVE_PIECE(mv->iType,LSBi(mv->iType)))-7;
			bbPieceBoards[WHITE_PIECES+7*(!iColor)]	=REMOVE_PIECE(bbPieceBoards[WHITE_PIECES+7*(!iColor)],mv->iDest);
			bbPieceBoards[iTypeEnemy+7*(!iColor)]	=REMOVE_PIECE(bbPieceBoards[iTypeEnemy+7*(!iColor)],mv->iDest);

			ullBoardZKey^=zobristPieces[iTypeEnemy+7*(!iColor)][mv->iDest];
			if(iTypeEnemy==PAWN) ullPawnZKey^=zobristPieces[13][mv->iDest];
		}
		else if(IsPawn2SqMove(mv->iFlags)) 
		{
			shEnPassant=0; // all en passant rights are forfeit against the enemy
			SET_BIT(shEnPassant,(COL(mv->iDest)+8*iColor)); //is en passant passible
		}

		//CLEAR_BITS(shEnPassant,(0xFF<<(8*iColor)));
	}
}
