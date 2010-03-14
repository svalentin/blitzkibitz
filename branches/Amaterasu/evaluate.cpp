#include<vector>
#include"constants.h"
#include"bitboards.h"
#include"magic.h"
#include"moves.h"
#include"board.h"
#include"evaluate.h"

inline void evalKing(const Board *GameBoard, const int gameStage, int &wscore, int &bscore)
{
	int pos = LSBi(GameBoard->bbPieceBoards[KING_W]);
	if (gameStage > 1)
		wscore += PiecePositionScore[KING_W+1][63-pos];
	else
		wscore += PiecePositionScore[KING_W][63-pos];
		
	pos = LSBi(GameBoard->bbPieceBoards[KING_B]);
	if (gameStage > 1)
		bscore += PiecePositionScore[KING_W+1][63 - (pos%8 + (7-(pos/8))*8)];
	else
		bscore += PiecePositionScore[KING_W][63 - (pos%8 + (7-(pos/8))*8)];
}

inline void evalPawn(const Board *GameBoard, const int gameStage, int &wscore, int &bscore)
{
	int sq;
	U64 bbPawnBoard;
	bbPawnBoard=GameBoard->GetPieceBoard(PAWN,WHITE);
	FOR_BIT(bbPawnBoard,sq)
	{
		const int iCol=sq%8;
		if((bbPawnBoard&IsolaniMasks[iCol])==0) wscore -= IsolatedPawnPenalty[iCol];
	}

	bbPawnBoard=GameBoard->GetPieceBoard(PAWN,BLACK);
	FOR_BIT(bbPawnBoard,sq)
	{
		const int iCol=sq%8;
		if((bbPawnBoard&IsolaniMasks[iCol])==0) bscore -= IsolatedPawnPenalty[iCol];
	}
}

/*inline void evalPawn(const Board &cboard, const int gameStage, int &wscore, int &bscore)
{
	float pawnPieceScale = 0, pawnPosScale = 1;

	if (gameStage == 1) pawnPieceScale = 0.25, pawnPosScale = 1.3;
	if (gameStage == 2) pawnPieceScale = 0.40, pawnPosScale = 1.7;
	if (gameStage == 3) pawnPieceScale = 0.70, pawnPosScale = 2.0;
	
	// white
	ull aux = cboard.bb[0];
	for (int pp = LSBi(aux); pp != 64; pp = LSBi(aux))
	{
		wscore += (int) (PieceScore[0] * pawnPieceScale);
		wscore += (int) (PiecePositionScore[0][63-pp] * pawnPosScale);
		
		// check if it's on an empty file or a half-empty file
		int col = pp % 8;
		if (MSB(cbb[col] & (cboard.bb[6] | cboard.bb[13])) == pp)
		{
			// the pawn has no pieces in front of it, add half it's value
			wscore += (int) (PieceScore[0] * (pawnPieceScale)/2);
		}
		else if (MSB(cbb[col] & cboard.bb[0]) != pp)
		{
			// there is another pawn on the same file
			wscore -= (int) (12 * pawnPosScale);
		}
		
		// isolated pawns get a penalty
		if ((col==0 || (col>0 && LSB(cbb[col-1] & cboard.bb[0])) == 64) && 
			(col==7 || (col<7 && LSB(cbb[col+1] & cboard.bb[0])) == 64)) {
			wscore -= IsolatedPawnPenalty[col];
		}
	}
	
	// black
	aux = cboard.bb[0+7];
	for (int pp = LSBi(aux); pp != 64; pp = LSBi(aux))
	{
		bscore += (int) (PieceScore[0] * pawnPieceScale);
		bscore += (int) (PiecePositionScore[0][63 - (pp%8 + (7-(pp/8))*8)] * pawnPosScale);
		
		// check if it's on an empty file or a half-empty file
		int col = pp % 8;
		if (LSB(cbb[col] & (cboard.bb[6] | cboard.bb[13])) == pp)
		{
			// the pawn has no pieces in front of it, add half it's value
			bscore += (int) (PieceScore[0] * (pawnPieceScale)/2);
		}
		else if (LSB(cbb[col] & cboard.bb[7]) != pp)
		{
			// there is another pawn on the same file
			bscore -= (int) (12 * pawnPosScale);
		}

		// isolated pawns get a penalty
		if ((col==0 || (col>0 && LSB(cbb[col-1] & cboard.bb[7])) == 64) && 
			(col==7 || (col<7 && LSB(cbb[col+1] & cboard.bb[7])) == 64)) {
			bscore -= IsolatedPawnPenalty[col];
		}
	}
}*/

int EvaluateBoard(Board const*const GameBoard,const int iColor)
{
	int scoreW=0,scoreB=0,i;
	U64 bbPieceBoard,bbMoveBoard,bbAttackBoard,aux;
	int pos,gameStage=0;

	for(i=PAWN;i<KING;i++)
	{
		bbPieceBoard=GameBoard->GetPieceBoard(i,WHITE);
		scoreW+=PieceValue[i]*Count1s(bbPieceBoard);

		bbPieceBoard=GameBoard->GetPieceBoard(i,BLACK);
		scoreB+=PieceValue[i]*Count1s(bbPieceBoard);
	}

	//if (scoreW < PieceValue[KING]+2000 || scoreB < PieceValue[KING]+2000) gameStage = 1;
	//if (scoreW < PieceValue[KING]+1500 || scoreB < PieceValue[KING]+1500) gameStage = 2;
	//if (scoreW < PieceValue[KING]+1000 || scoreB < PieceValue[KING]+1000) gameStage = 3;

	//evalPawn(GameBoard,gameStage,scoreW,scoreB);
	evalKing(GameBoard,gameStage,scoreW,scoreB);

	bbMoveBoard=GameBoard->GetPieceBoard(KING,BLACK);
	bbMoveBoard&=GameBoard->GetAttackBoard(WHITE);
	//scoreW+=(5*Count1s(bbMoveBoard));

	bbMoveBoard=GameBoard->GetPieceBoard(KING,WHITE);
	bbMoveBoard&=GameBoard->GetAttackBoard(BLACK);
	//scoreB+=(5*Count1s(bbMoveBoard));

	if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,WHITE),GameBoard->GetAttackBoard(BLACK))) scoreB+=5;
	if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,BLACK),GameBoard->GetAttackBoard(WHITE))) scoreW+=5;

	bbAttackBoard=GameBoard->GetAttackBoard(WHITE);
	FOR_BIT(bbAttackBoard,pos)
	{
		scoreW += AttackScore[pos];
	}
	for (int i=PAWNS_W; i<=KING_W; ++i)
	{
		aux = GameBoard->GetPieceBoard(i,WHITE);
		FOR_BIT(aux,pos)
		{
			// piece position
			scoreW += PiecePositionScore[i][63-pos];
		}
	}

	bbAttackBoard=GameBoard->GetAttackBoard(BLACK);
	FOR_BIT(bbAttackBoard,pos)
	{
		scoreB += AttackScore[pos];
	}
	for (int i=PAWNS_B; i<=KING_B; ++i)
	{
		aux = GameBoard->GetPieceBoard(i,BLACK);
		FOR_BIT(aux,pos)
		{
			// piece position
			scoreB += PiecePositionScore[i-7][63 - (pos%8 + (7-(pos/8))*8)];
		}
	}

	if ((scoreW - scoreB) > 1200 || (scoreW - scoreB) < -1200)
		return ColorScore[iColor]*(scoreW - scoreB + (rand()%201 - 75));
	if ((scoreW - scoreB) > 600 || (scoreW - scoreB) < -600)
		return ColorScore[iColor]*(scoreW - scoreB + (rand()%101 - 50));
	if ((scoreW - scoreB) > 400 || (scoreW - scoreB) < -400)
		return ColorScore[iColor]*(scoreW - scoreB + (rand()%51 - 25));
	return ColorScore[iColor]*(scoreW-scoreB);
}
