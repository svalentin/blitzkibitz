#ifndef _MOVES_H_
#define _MOVES_H_

#include "constants.h"

class Board;
class Move;

#define CAP_INDEX 2
#define KSC_INDEX 4
#define QSC_INDEX 8
#define P2SM_INDEX 16
#define EPAS_INDEX 32
#define PROM_INDEX 64
#define CHECK_INDEX 128
#define CHECK_MATE_INDEX 256

#define IsCapture(iFlags)		((iFlags)&(CAP_INDEX))
#define IsKSCastle(iFlags)		((iFlags)&(KSC_INDEX))
#define IsQSCastle(iFlags)		((iFlags)&(QSC_INDEX))
#define IsPawn2SqMove(iFlags)	((iFlags)&(P2SM_INDEX))
#define IsEnPassant(iFlags)		((iFlags)&(EPAS_INDEX))
#define IsPromotion(iFlags)		((iFlags)&(PROM_INDEX))
#define IsCheck(iFlags)			((iFlags)&(CHECK_INDEX))
#define IsCheckMate(iFlags)		((iFlags)&(CHECK_MATE_INDEX))

#define IsKingSideCastling(str) (str[0]=='O' && str[1]=='-' && str[2]=='O')
#define IsQueenSideCastling(str) (str[0]=='O' && str[1]=='-' && str[2]=='O'  && str[3]=='-'  && str[4]=='O')

/*

iType description

bit...
0 - 5 - Piece Type
7 - 12 - Captured Piece Type

iFlags description:
bit...
0 - Color ( 0 White, 1 Black )
1 - Capture
2 - K Castling
3 - Q Castling
4 - 2 square move
5 - En Passant
6 - Promotion (default will be queen)
7 - Check
8 - Check Mate

*/

extern Move g_PVLine[MAX_DEPTH+1];

class Move
{
	public:
		Move()
		{
			iSrc=iDest=iType=iFlags=0;
		}
		int iSrc;
		int iDest;
		int iType;
		int iFlags;

		const bool operator<(const Move &mv) const
		{
			if(g_PVLine[0].iDest==iDest && g_PVLine[0].iSrc==iSrc && g_PVLine[0].iType==iType && g_PVLine[0].iFlags==iFlags)
				return false;
			const int iEnemyTypeIndex1=LSBi(iType&0x1F80)-7,iTypeIndex1=LSBi(iType&0x3F);
			const int iEnemyTypeIndex2=LSBi(mv.iType&0x1F80)-7,iTypeIndex2=LSBi(mv.iType&0x3F);
			/*if(g_MoveSeries[0].iType!=-1)
			{
				if(iType==g_MoveSeries[0].iType && iDest==g_MoveSeries[0].iDest && iSrc==g_MoveSeries[0].iSrc && iFlags==g_MoveSeries[0].iFlags) return true;
			}*/
			if(IsCapture(iFlags) && IsCapture(mv.iFlags))
			{
				if( (PieceValue[iEnemyTypeIndex1]-PieceValue[iTypeIndex1]) > (PieceValue[iEnemyTypeIndex2]-PieceValue[iTypeIndex2]) )
				{
					return true;
				}
				if( (PieceValue[iEnemyTypeIndex1]-PieceValue[iTypeIndex1]) == (PieceValue[iEnemyTypeIndex2]-PieceValue[iTypeIndex2]) )
				{
					if(PieceValue[iEnemyTypeIndex1]>PieceValue[iEnemyTypeIndex2]) return true;
					else if(PieceValue[iEnemyTypeIndex1]==PieceValue[iEnemyTypeIndex2])
					{
						if(IsPromotion(iFlags) && !IsPromotion(mv.iFlags)) return true;
						return false;
					}
				}
				return false;
			}
			else if(IsCapture(iFlags) && !IsCapture(mv.iFlags)) return true;
			if(!IsCapture(iFlags) && !IsCapture(mv.iFlags))
			{
				if(IsPromotion(iFlags) && !IsPromotion(mv.iFlags)) return true;
				if(IsCheck(iFlags) && !IsCheck(mv.iFlags)) return true;
				
				if(iTypeIndex1==PAWN && iTypeIndex2==PAWN)
				{
					if(IsPawn2SqMove(iFlags) && !IsPawn2SqMove(mv.iFlags)) return true;
				}
			}
			return false;
		}
};


void PrintMoveInfo(Move m,FILE *fout = stdout);
void ResolveAmbiguity(U64 bbPieceBoard,const U64 bbAllPiecesBoard,const int iType,const int iSrc,const int iDest,int&iRow,int&iColumn);
int GetPieceByDest(const U64 bbBoard,U64 bbPiece,const int iDest,const int iType,const int iColor,const int iSrcRow,const int iSrcColumn,const int iCapture);
int DecodeMove(char *const str,Board const*const GameBoard,const int iColor,Move *mv);
int EncodeMove(Board const*const GameBoard,Move const*const mv,const int iColor,char*str);

#endif
