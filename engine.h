#ifndef ENGINE_H_
#define ENGINE_H_

#include "board.h"
#include "evaluation.h"

#include <ctime>
#include <cstdlib>

#define INF 0x3f3f3f3f

typedef unsigned long long ull;

extern Move BestMove;
extern int hits, hd2, hd3;

int CalculateScore(const Board &cboard);

int NegaMax(Board &cboard, const int max_depth, const int depth=0);
int NegaMaxD(vector<Move> &moves, Board &cboard, const int max_depth, const int depth);
pair<int, int> AlphaBeta(const Board &cboard, const int max_depth, const int depth=0, int alpha=-INF, int beta=INF);
int AlphaBetaD(vector<Move> &moves, Board &cboard, const int max_depth, int depth=0, int alpha=-INF, int beta=INF);


class IDDFS_callback_class {
	public:
	virtual void operator()(int depth, int score, int nodes) const = 0;
};
class IDDFS_empty_callback : public IDDFS_callback_class {
	public:
	void operator()(int depth, int score, int nodes) const {};
};

int IDDFS(
	const Board &cboard,
	const int max_depth,
	const IDDFS_callback_class &callback = IDDFS_empty_callback()
);

#endif
