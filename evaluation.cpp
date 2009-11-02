#include "evaluation.h"inline void evalPieces(const Board &cboard, int &wscore, int &bscore)
{
    // white score
    for (int i=0; i<6; ++i) {
        ull aux = cboard.bb[i];
        for (int pp = LSBi(aux); pp != 64; pp = LSBi(aux)) 
            wscore += PieceScore[i];
    }
    
    // black score
    for (int i=0; i<6; ++i) {
        ull aux = cboard.bb[i+7];
        for (int pp = LSBi(aux); pp != 64; pp = LSBi(aux)) 
            bscore += PieceScore[i];
    }
}

inline void evalPawn(const Board &cboard, const int gameStage, int &wscore, int &bscore)
{
    float pawnPieceScale = 0, pawnPosScale = 1;

    if (gameStage == 1) pawnPieceScale = 0.25, pawnPosScale = 1.3;
    if (gameStage == 2) pawnPieceScale = 0.40, pawnPosScale = 1.7;
    if (gameStage == 3) pawnPieceScale = 0.70, pawnPosScale = 2.0;
    
    // white
    ull aux = cboard.bb[0];
    for (int pp = LSBi(aux); pp != 64; pp = LSBi(aux)) {
        wscore += (int) (PieceScore[0] * pawnPieceScale);
        wscore += (int) (PiecePositionScore[0][63-pp] * pawnPosScale);
        
        // check if it's on an empty file or a half-empty file
        int col = pp % 8;
        if (MSB(cbb[col] & (cboard.bb[6] | cboard.bb[13])) == pp) {
            // the pawn has no pieces in front of it, add half it's value
            wscore += (int) (PieceScore[0] * (pawnPieceScale)/2);
        }
        else if (MSB(cbb[col] & cboard.bb[0]) != pp) {
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
    for (int pp = LSBi(aux); pp != 64; pp = LSBi(aux)) {
        bscore += (int) (PieceScore[0] * pawnPieceScale);
        bscore += (int) (PiecePositionScore[0][63 - (pp%8 + (7-(pp/8))*8)] * pawnPosScale);
        
        // check if it's on an empty file or a half-empty file
        int col = pp % 8;
        if (LSB(cbb[col] & (cboard.bb[6] | cboard.bb[13])) == pp) {
            // the pawn has no pieces in front of it, add half it's value
            bscore += (int) (PieceScore[0] * (pawnPieceScale)/2);
        }
        else if (LSB(cbb[col] & cboard.bb[7]) != pp) {
            // there is another pawn on the same file
            bscore -= (int) (12 * pawnPosScale);
        }

        // isolated pawns get a penalty
        if ((col==0 || (col>0 && LSB(cbb[col-1] & cboard.bb[7])) == 64) && 
            (col==7 || (col<7 && LSB(cbb[col+1] & cboard.bb[7])) == 64)) {
            bscore -= IsolatedPawnPenalty[col];
        }
    }

}

inline void evalKing(const Board &cboard, const bool gameStage, int &wscore, int &bscore)
{
    int pos = LSB(cboard.bb[KING_W]);
    if (gameStage > 1)
        wscore += PiecePositionScore[KING_W+1][63-pos];
    else
        wscore += PiecePositionScore[KING_W][63-pos];
        
    pos = LSB(cboard.bb[KING_B]);
    if (gameStage > 1)
        bscore += PiecePositionScore[KING_W+1][63 - (pos%8 + (7-(pos/8))*8)];
    else
        bscore += PiecePositionScore[KING_W][63 - (pos%8 + (7-(pos/8))*8)];
}

int CalculateScore(const Board &cboard)
{
    int wscore=0, bscore=0;
	int gameStage=0;
    
	evalPieces(cboard, wscore, bscore);

    if (wscore < KING_SCORE+2000 || bscore < KING_SCORE+2000) gameStage = 1;
    if (wscore < KING_SCORE+1500 || bscore < KING_SCORE+1500) gameStage = 2;
    if (wscore < KING_SCORE+1000 || bscore < KING_SCORE+1000) gameStage = 3;
	
    evalPawn(cboard, gameStage, wscore, bscore);
	evalKing(cboard, gameStage, wscore, bscore);
    
    //////
    // Position score
    
    ull attack = 0;
    ull occ = cboard.GetOccupancy();

    // white
    for (int i=PAWN_W; i<=KING_W; ++i) {
        ull aux = cboard.bb[i];
        for (int pos = LSBi(aux); pos != 64; pos = LSBi(aux)) {
            
            // attack squares
            if (i == PAWN_W) {
                if (pos+7 < 64) SET_BIT(attack, pos+7);
                if (pos+9 < 64) SET_BIT(attack, pos+9);
                continue;
            }
            else if (i == KNIGHT_W) {
                attack |= Nmagic(pos);
            }
            else if (i == BISHOP_W) {
                attack |= Bmagic(pos, occ);
            }
            else if (i == ROOK_W) {
                attack |= Rmagic(pos, occ);
            }
            else if (i == QUEEN_W) {
                attack |= Qmagic(pos, occ);
            }
            else if (i == KING_W) {
                attack |= Kmagic(pos);
				continue;
            }
            
            // piece position
            wscore += PiecePositionScore[i][63-pos];
        }
    }
    
    for (int i=LSBi(attack); i != 64; i = LSBi(attack)) {
        wscore += AttackScore[i];
    }
    
    // black
    attack = 0;
    for (int i=PAWN_B; i<=KING_B; ++i) {
        ull aux = cboard.bb[i];
        for (int pos = LSBi(aux); pos != 64; pos = LSBi(aux)) {
        
            // attack squares
            if (i == PAWN_B) {
                if (pos-7 >= 0) SET_BIT(attack, pos-7);
                if (pos-9 >= 0) SET_BIT(attack, pos-9);
                continue;
            }
            else if (i == KNIGHT_B) {
                attack |= Nmagic(pos);
            }
            else if (i == BISHOP_B) {
                attack |= Bmagic(pos, occ);
            }
            else if (i == ROOK_B) {
                attack |= Rmagic(pos, occ);
            }
            else if (i == QUEEN_B) {
                attack |= Qmagic(pos, occ);
            }
            else if (i == KING_B) {
                attack |= Kmagic(pos);
				continue;
            }
            
            // piece position
            bscore += PiecePositionScore[i-7][63 - (pos%8 + (7-(pos/8))*8)];
        }
    }

    for (int i=LSBi(attack); i != 64; i = LSBi(attack)) {
        bscore += AttackScore[i];
    }

#ifndef NORAND
    // a little bit of random if we are winning
    if ((wscore - bscore) > 1200 || (wscore - bscore) < -1200)
        return wscore - bscore + (rand()%201 - 100);
    if ((wscore - bscore) > 600 || (wscore - bscore) < -600)
        return wscore - bscore + (rand()%101 - 50);
    if ((wscore - bscore) > 400 || (wscore - bscore) < -400)
        return wscore - bscore + (rand()%51 - 25);
#endif

    return wscore - bscore;
}

int SCalculateScore(const Board &cboard)
{
    return (cboard.player==0) ? CalculateScore(cboard) : -CalculateScore(cboard);
}

//
///////////////////////////////////////////////////////////////