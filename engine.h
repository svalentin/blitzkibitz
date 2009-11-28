#ifndef ENGINE_H_
#define ENGINE_H_

#include "board.h"
#include "evaluation.h"

#include <ctime>
#include <cstdlib>

#define INF 0x3f3f3f3f

typedef unsigned long long ull;

extern Move BestMove;
extern int MAX_DEPTH;
extern int DEPTH_LIMIT;
extern int hits, hd2, hd3;

int CalculateScore(const Board &cboard);

int NegaMax(Board &cboard, int moveNr, int depth=0);
int NegaMaxD(vector<Move> &moves, int moveNr, Board &cboard, int depth);
int AlphaBeta(const Board &cboard, const int moveNr, const int max_depth, const int depth=0, int alpha=-INF, int beta=INF);
int AlphaBetaD(vector<Move> &moves, Board &cboard, int moveNr, int depth=0, int alpha=-INF, int beta=INF);

int IDDFS(const Board &cboard, const int moveNr, const int max_depth);

#endif
