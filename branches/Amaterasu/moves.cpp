#include"warnings.h"
#include<stdio.h>
#include<string.h>
#include<vector>
#include"bitboards.h"
#include"board.h"
#include"moves.h"
#include"magic.h"
using namespace std;

void PrintMoveInfo(Move m,FILE*fout)
{
	if(IsKSCastle(m.iFlags)) fprintf(fout,"Is King Side Castle\n");
	else if(IsQSCastle(m.iFlags)) fprintf(fout,"Is Queen Side Castle\n");
	{
		fprintf(fout,"Color: %d\nPiece type: %d\nSrc:		  %d\nDest:		  %d\n",m.iFlags&1,LSBi(m.iType),m.iSrc,m.iDest);
		if(IsCapture(m.iFlags)) fprintf(fout,"Is Capture (captured piece type: %d)\n",LSBi(REMOVE_PIECE(m.iType,LSBi(m.iType))));
		if(IsEnPassant(m.iFlags)) fprintf(fout,"Is En Passant capture\n");
		if(IsPromotion(m.iFlags)) fprintf(fout,"Is Promotion\n");
		if(IsPawn2SqMove(m.iFlags)) fprintf(fout,"Is Pawn 2 square move\n");
		if(IsCheckMate(m.iFlags)) fprintf(fout,"Is Check Mate\n");
		else if(IsCheck(m.iFlags)) fprintf(fout,"Is Check\n");
	}
}

void ResolveAmbiguity(U64 bbPieceBoard,const U64 bbAllPiecesBoard,const int iType,const int iSrc,const int iDest,int&iRow,int&iColumn)
{
	int sq;
	U64 bbAttackBoard=0;
	iColumn=iRow=-1;
	FOR_BIT(bbPieceBoard,sq)
	{
		bbPieceBoard=CLEAR_BIT(bbPieceBoard,sq);
		if(sq!=iSrc)
		{
			switch(iType)
			{
				case KNIGHT:	bbAttackBoard=Nmagic(sq);					break;
				case BISHOP:	bbAttackBoard=Bmagic(sq,bbAllPiecesBoard);	break;
				case ROOK:		bbAttackBoard=Rmagic(sq,bbAllPiecesBoard);	break;
				case QUEEN:		bbAttackBoard=Qmagic(sq,bbAllPiecesBoard);	break;
				case KING:		bbAttackBoard=Kmagic(sq);					break;
			}
			if(BIT_IS_SET(bbAttackBoard,iDest))
			{
				if(sq>>3==iSrc>>3) iColumn=iSrc%8;
				if(sq%8==iSrc%8) iRow=iSrc>>3;
				if(sq>>3!=iSrc>>3 && sq%8!=iSrc%8) iColumn=iSrc%8;
			}
		}
	}
}

int GetPieceByDest(const U64 bbBoard,U64 bbPiece,const int iDest,const int iType,const int iColor,const int iSrcRow,const int iSrcColumn,const int iCapture)
{
	int i=0,sq;
	U64 bbMoveBoard;
	FOR_BIT(bbPiece,sq)
	{
		if(iType==PAWN)			bbMoveBoard=((!iCapture)*PNormalMagic(sq,iColor))|(iCapture*PAttackMagic(sq,iColor));
		else if(iType==KNIGHT)	bbMoveBoard=Nmagic(sq);
		else if(iType==BISHOP)	bbMoveBoard=Bmagic(sq,bbBoard);
		else if(iType==ROOK)	bbMoveBoard=Rmagic(sq,bbBoard);
		else if(iType==QUEEN)	bbMoveBoard=Qmagic(sq,bbBoard);
		else if(iType==KING)	bbMoveBoard=Kmagic(sq);

		if(BIT_IS_SET(bbMoveBoard,iDest))
		{
			/* See if we have a column desambiguazation and if so see if we also have a line one. If we do it means
			   we found the piece. If not see if we have a row disambiguazation. */
			if(iSrcColumn>=0)
			{
				if(iSrcColumn==sq%8)
				{
					if(iSrcRow>=0)
					{
						if(iSrcRow==(sq>>3)) break;
						else continue;
					}
					else break;
				}
				else continue;
			}
			else if(iSrcRow>=0)
			{
				if(iSrcRow==(sq>>3)) break;
				else continue;
			}
			else break;
		}
	}
	return sq;
}

int EncodeMove(Board const*const GameBoard,Move const*const mv,const int iColor,char*str)
{
	int i=0,iColumn=-1,iRow=-1;
	const int iType=LSBi((mv->iType)&0x3F);
	
	if(IsKSCastle(mv->iFlags)) strcpy(str,"O-O");
	else if(IsQSCastle(mv->iFlags)) strcpy(str,"O-O-O");
	else
	{
		if(iType==PAWN)
		{
			if(IsCapture(mv->iFlags))
			{
				str[i++]='h'-mv->iSrc%8;
				str[i++]='x';
			}
		}
		else
		{
			switch(iType)
			{
				case KNIGHT: str[i++]='N'; break;
				case BISHOP: str[i++]='B'; break;
				case ROOK:	 str[i++]='R'; break;
				case QUEEN:	 str[i++]='Q'; break;
				case KING:	 str[i++]='K'; break;
			}
			ResolveAmbiguity(GameBoard->GetPieceBoard(iType,iColor),GameBoard->GetOccupancy(),iType,mv->iSrc,mv->iDest,iRow,iColumn);
			if(iColumn>-1)				str[i++]='h'-iColumn;
			if(iRow>-1)					str[i++]='1'+iRow;
			if(IsCapture(mv->iFlags))	str[i++]='x';
		}
		str[i++]='h'-mv->iDest%8;
		str[i++]='1'+(mv->iDest>>3);
		if(IsPromotion(mv->iFlags))
		{
			str[i++]='=';
			str[i++]='Q'; /* treb pus in move.iType ( desi ala tot 'Q' va fi tot timpul d'oh 8-| ) */
		}
		if(IsCheckMate(mv->iFlags)) str[i++]='#';
		else if(IsCheck(mv->iFlags)) str[i++]='+';
		str[i]=0;
	}
	return i;
}

int DecodeMove(char *const str,Board const*const GameBoard,const int iColor,Move *mv)
{
	char *p = str;	//this way any piece of information may or may not be there, as long as they come in this specific order.
	char piece_type[2]={0,0},start_column[2]={0,0},start_row[2]={0,0},capture[2]={0,0};
	char end_column[2]={0,0},end_row[2]={0,0},special[2]={0,0};
	int iType=0,iDestType;

	mv->iDest=mv->iSrc=mv->iType=0;
	mv->iFlags=iColor;
	if(IsQueenSideCastling(str))
	{
		SET_BITS(mv->iFlags,QSC_INDEX);
		return 0;
	}
	else if(IsKingSideCastling(str))
	{
		SET_BITS(mv->iFlags,KSC_INDEX);
		return 0;
	}
	piece_type[0] = 'P'; // piece_type defaults to pawn;
	if(p && strpbrk(p,"KQNBRP") == p)
	{
		piece_type[0] = (*p);
		p++;
	}
	switch(piece_type[0])
	{
		case 'N': SET_BIT(mv->iType,KNIGHT); iType=1;	break;
		case 'B': SET_BIT(mv->iType,BISHOP); iType=2;	break;
		case 'R': SET_BIT(mv->iType,ROOK);   iType=3;	break;
		case 'Q': SET_BIT(mv->iType,QUEEN);	 iType=4;	break;
		case 'K': SET_BIT(mv->iType,KING);	 iType=5;	break;
		case 'P': SET_BIT(mv->iType,PAWN);	 iType=0;	break;
		default: return -1;
	}

	if(p && strpbrk(p,"abcdefgh") == p)
	{
		start_column[0] = (*p);
		p++;
	}
	if(p && strpbrk(p,"12345678") == p)
	{
		start_row[0] = (*p);
		p++;
	}
	if(p && (*p)=='x')
	{
		capture[0] = 1;
		p++;
	}
	if(p && strpbrk(p,"abcdefgh") == p)
	{
		end_column[0] = (*p);
		p++;
	}
	if(p && strpbrk(p,"12345678") == p)
	{
		end_row[0] = (*p);
		p++;
	}
	if(p && (*p)=='=')
	{
		p++;
		piece_type[0]=(*p);
		SET_BITS(mv->iFlags,PROM_INDEX);
	}
	if(p && strpbrk(p,"+#") == p)
	{
		special[0] = (*p);
		p++;
	}
	if((!end_column[0] && !end_row[0]) && (start_column[0] && start_row[0]))
	{
		end_column[0] ^= start_column[0];
		start_column[0] ^= end_column[0];
		end_column[0] ^= start_column[0];

		end_row[0] ^= start_row[0];
		start_row[0] ^= end_row[0];
		end_row[0] ^= start_row[0];
	}

	mv->iDest = end_row[0]&&end_column[0] ? COORDS_2_INDEX(end_row[0]-'1',7-(end_column[0]-'a')) : 0;
	if(capture[0])
	{
		iDestType=GameBoard->GetPieceType(mv->iDest);
		if(iType==0 && iDestType==-1)
		{
			if(BIT_IS_SET(GameBoard->GetPieceBoard(iType,!iColor),(mv->iDest+(iColor?8:-8))))
			{
				SET_BIT(mv->iType,PAWNS_B); // it's good...leave it like that
				SET_BITS(mv->iFlags,EPAS_INDEX);
			}
		}
		else SET_BIT(mv->iType,iDestType+iColor*7); // it's good...leave it like that
	}
	
	const int iSrcRow = start_row[0] ? start_row[0]-'1': -1;
	const int iSrcCol = start_column[0] ? 7-(start_column[0]-'a') : -1;

	mv->iSrc=GetPieceByDest(GameBoard->GetOccupancy(),GameBoard->GetPieceBoard(iType,iColor),mv->iDest,iType,iColor,iSrcRow,iSrcCol,capture[0]);
	if(mv->iSrc==-1) return -1;
	
	SET_BITS(mv->iFlags,(CAP_INDEX*capture[0])); // set capture flag...if necessary
	
	if(special[0]=='+') SET_BITS(mv->iFlags,CHECK_INDEX);
	if(special[0]=='#') SET_BITS(mv->iFlags,CHECK_MATE_INDEX);

	return 0;
}
