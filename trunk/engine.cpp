#include "engine.h"

///////////////////////////////////////////////////////////////
// Scoring magic

#define PAWN_SCORE      100
#define KNIGHT_SCORE    310
#define BISHOP_SCORE    305
#define ROOK_SCORE      500
#define QUEEN_SCORE     850
#define KING_SCORE      64000

const int PieceScore[6] = {
        PAWN_SCORE, KNIGHT_SCORE, BISHOP_SCORE,
        ROOK_SCORE, QUEEN_SCORE, KING_SCORE
    };

const int AttackScore[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  1,  1,  1,  1,  1,  1,  0,
        0,  1,  8,  8,  8,  8,  1,  0,
        0,  1,  8, 25, 25,  8,  1,  0,
        0,  1,  8, 25, 25,  8,  1,  0,
        0,  1,  8,  8,  8,  8,  1,  0,
        0,  1,  1,  1,  1,  1,  1,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
    };

const int IsolatedPawnPenalty[8] = {10, 12, 14, 18, 18, 14, 12, 10};
    
const int PiecePositionScore[7][64] = {
	// Pawn scores White
	{
		 0,  0,  0,  0,  0,  0,  0,  0,
		20, 26, 26, 28, 28, 26, 26, 20,
        12, 14, 16, 21, 21, 16, 14, 12,
         8, 10, 12, 18, 18, 12, 10,  8,
         4,  6,  8, 16, 16,  8,  6,  4,
         2,  2,  4,  6,  6,  4,  2,  2,
         0,  0,  0, -4, -4,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0
    },
    // Knight scores White
    {
        -40, -10,  - 5,  - 5,  - 5,  - 5, -10, -40,
        - 5,   5,    5,    5,    5,    5,   5, - 5,
        - 5,   5,   10,   15,   15,   10,   5, - 5,
        - 5,   5,   10,   15,   15,   10,   5, - 5,
        - 5,   5,   10,   15,   15,   10,   5, - 5,
        - 5,   5,    8,    8,    8,    8,   5, - 5,
        - 5,   0,    5,    5,    5,    5,   0, - 5,
        -50, -20,  -10,  -10,  -10,  -10, -20, -50,
    },
    // Bishop scores White
    {
        -40, -20, -15, -15, -15, -15, -20, -40,
          0,   5,   5,   5,   5,   5,   5,   0,
          0,  10,  10,  18,  18,  10,  10,   0,
          0,  10,  10,  18,  18,  10,  10,   0,
          0,   5,  10,  18,  18,  10,   5,   0,
          0,   0,   5,   5,   5,   5,   0,   0,
          0,   5,   0,   0,   0,   0,   5,   0,
        -50, -20, -10, -20, -20, -10, -20, -50
    },
    // Rook scores White
    {
        10, 10, 10, 10, 10, 10, 10, 10,
         5,  5,  5, 10, 10,  5,  5,  5,
         0,  0,  5, 10, 10,  5,  0,  0,
         0,  0,  5, 10, 10,  5,  0,  0,
         0,  0,  5, 10, 10,  5,  0,  0,
         0,  0,  5, 10, 10,  5,  0,  0,
         0,  0,  5, 10, 10,  5,  0,  0,
         0,  0,  5, 10, 10,  5,  0,  0,
    },
    // Queen scores White
    {
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 10, 10, 10, 10,  0,  0,
         0,  0, 10, 15, 15, 10,  0,  0,
         0,  0, 10, 15, 15, 10,  0,  0,
         0,  0, 10, 10, 10, 10,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0
    },
    // King scores White
    {
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
        12,  8,  4,  0,  0,  4,  8, 12,
        16, 12,  8,  4,  4,  8, 12, 16,
        24, 20, 16, 12, 12, 16, 20, 24,
        24, 24, 24, 16, 16,  6, 32, 32
    },
	// King end-game scores White
    {
        -30, -5,  0,  0,  0,  0, -5,-30,
         -5,  0,  0,  0,  0,  0,  0, -5,
          0,  0,  0,  0,  0,  0,  0,  0,
          0,  0,  0,  5,  5,  0,  0,  0,
          0,  0,  0,  5,  5,  0,  0,  0,
          0,  0,  0,  0,  0,  0,  0,  0,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -40,-10, -5, -5, -5, -5,-10,-40
    }
};

inline void evalPieces(const Board &cboard, int &wscore, int &bscore)
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

inline int SCalculateScore(const Board &cboard)
{
    return (cboard.player==0) ? CalculateScore(cboard) : -CalculateScore(cboard);
}

//
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
// BestMove search Algorithm

Move BestMove;
int MAX_DEPTH = 5;
int DEPTH_LIMIT = 5;

// NOTE: Only use this for debug! It is not up to date (no transposition tables)
int NegaMax(Board &cboard, int moveNr, int depth)
{
    if (depth == MAX_DEPTH) {
        return SCalculateScore(cboard);
    }
    
    int bestScore = -INF;
    vector<Move> mvs = cboard.GetMoves();
    Board newboard;
    for (int i=0; i<mvs.size(); ++i) {
        newboard = cboard;
        newboard.MakeMove(mvs[i]);
        newboard.player = !newboard.player;
        
        int score = -NegaMax(newboard, moveNr+1, depth+1);
        
        if (score > bestScore) {
            bestScore = score;
            if (depth == 0) {
                BestMove = mvs[i];
            }
        }
    }
    return bestScore;
}

// NOTE: This function is intended for debugging purposes only!
//       It acts like NegaMax, but also returns the best move-sequesnce
//       It is not up to date! (no transposition tables)
int NegaMaxD(vector<Move> &moves, int moveNr, Board &cboard, int depth)
{
    if (depth == MAX_DEPTH) {
        return SCalculateScore(cboard);
    }
    
    int bestScore = -INF;
    vector<Move> mvs = cboard.GetMoves();
    vector<Move> bm;
    Board newboard;
    
    for (int i=0; i<mvs.size(); ++i) {
        newboard = cboard;
        newboard.MakeMove(mvs[i]);
        newboard.player = !newboard.player;
        
        vector <Move> newmoves;
        
        int score = -NegaMaxD(newmoves, moveNr+1, newboard, depth+1);
        
        if (score > bestScore) {
            bestScore = score;
            bm = newmoves;
            bm.insert(bm.begin(), mvs[i]);
            if (depth == 0) {
                BestMove = mvs[i];
            }
        }
    }
    
    for (vector<Move>::reverse_iterator i=bm.rbegin(); i!=bm.rend(); i++)
        moves.insert(moves.begin(), *i);
    return bestScore;
}

inline void recordHash(ull zkey, ull pzkey, bool exact, bool lbound, int score,
                       unsigned char depth, unsigned char npiece)
{
    HElem ht;
    ht.zkey = zkey;
    ht.pzkey = pzkey;
    ht.exact = exact;
    ht.lbound = lbound;
    ht.score = score;
    ht.depth = depth;
    ht.npiece = npiece;
    SetHElem(GetHashIndex(zkey), ht);
}


int hits = 0, hd2 = 0, hd3 = 0;
// The main function used to search for the best move
int AlphaBeta(const Board &cboard, const int moveNr, const int depth, int alpha, int beta)
{
    if (depth == MAX_DEPTH) {
        return SCalculateScore(cboard);
    }

    // Transposition table stuff
    
    ull zkey = GetZobristKey(cboard);
    ull pzkey = GetPawnZobristKey(cboard);
    HElem ht = GetHElem(GetHashIndex(zkey));
    
    if (depth != 0) {
        // did we calculated this already?
        if (zkey == ht.zkey && pzkey == ht.pzkey &&
            ht.npiece == cboard.GetPieceCount() &&
            ht.depth >= MAX_DEPTH-depth) {

            if (ht.exact) {
                ++hits;
                if (ht.depth - (MAX_DEPTH-depth) >= 2) ++hd2;
                if (ht.depth - (MAX_DEPTH-depth) >= 3) ++hd3;
                return ht.score;
            }
            else {
                if (ht.lbound && ht.score <= alpha)
                    return alpha;
                else if (!ht.lbound && ht.score >= beta)
                    return beta;
            }
        }
    }
    
    // Get moves and check them
    
    int tstart = clock();
    
    bool hexact = false;
    vector<Move> mvs = cboard.GetMoves();
    Board newboard;
    
    if (mvs.empty()) {
        if (depth == 0) {
            BestMove.flags = EGAL;
        }
        if (cboard.sah == MAT || cboard.sah == SAH) {
            return -64000+depth;    // mate (the sooner the better)
        }
        else {
            // stalemate
            // it's ok only if we're loosing
            // and the sooner, the better
            int score = SCalculateScore(cboard);
            if (score > 500)
                return -32000+depth;
            else if (score < -500)
                return 32000-depth;
            else return score;
        }
    }
    
    for (int i=0; i<mvs.size(); ++i) {
        newboard = cboard;
        newboard.MakeMove(mvs[i]);
        newboard.player = !newboard.player;

        int score = -AlphaBeta(newboard, moveNr+1, depth+1, -beta, -alpha);
        
        if (score >= beta) {
            recordHash(zkey, pzkey, false, false, beta, MAX_DEPTH-depth, cboard.GetPieceCount());
            return beta;
        }
        if (score > alpha) {
            hexact = true;
            alpha = score;
            if (depth == 0) {
                BestMove = mvs[i];
            }
        }
    }
    
    int extra_depth=0;
    if (depth==0 && MAX_DEPTH < DEPTH_LIMIT && (double)(clock() - tstart) / CLOCKS_PER_SEC < 2) {
        extra_depth = 1;
        MAX_DEPTH += extra_depth;
        alpha = AlphaBeta(cboard, moveNr);
    }
    
    // store the key
    recordHash(zkey, pzkey, hexact, true, alpha, MAX_DEPTH-depth, cboard.GetPieceCount());
    
    MAX_DEPTH -= extra_depth;
    
    return alpha;
}

// NOTE: This function is intended for debugging purposes only!
//       It acts like AlphaBeta, but also returns the best move-sequesnce
//       It is not up to date! (no transposition tables)
int AlphaBetaD(vector<Move> &moves, int moveNr, Board &cboard, int depth, int alpha, int beta)
{
    if (depth == MAX_DEPTH) {
        return SCalculateScore(cboard);
    }
    
    int tstart = clock();
    
    int bestScore = -INF;
    vector<Move> mvs = cboard.GetMoves();
    vector<Move> bm;
    Board newboard;
    
    for (int i=0; i<mvs.size(); ++i) {
        newboard = cboard;
        newboard.MakeMove(mvs[i]);
        newboard.player = !newboard.player;
        
        vector <Move> newmoves;

        int score = -AlphaBetaD(newmoves, moveNr, newboard, depth+1, -beta, -alpha);

        if (score > alpha) alpha = score;
        if (score > bestScore) {
            bestScore = score;
            bm = newmoves;
            bm.insert(bm.begin(), mvs[i]);
            if (depth == 0) {
                BestMove = mvs[i];
            }
        }
        
        if (alpha >= beta) break;
    }
    
    /*
    if (depth==0 && (double)(clock() - tstart) / CLOCKS_PER_SEC < 1) {
        MAX_DEPTH+=1;
        int score = AlphaBeta(cboard);
        MAX_DEPTH-=1;
        return score;
    }
    */

    for (vector<Move>::reverse_iterator i=bm.rbegin(); i!=bm.rend(); i++)
        moves.insert(moves.begin(), *i);
    return bestScore;
}
//
///////////////////////////////////////////////////////////////
