#include "engine.h"

///////////////////////////////////////////////////////////////
// BestMove search Algorithm

Move BestMove;
int MAX_DEPTH = 7;
int DEPTH_LIMIT = 7;

// NOTE: Only use this for debug! It is not up to date (no transposition tables)
int NegaMax(Board &cboard, int moveNr, int depth)
{
	if (depth == MAX_DEPTH) {
		return SCalculateScore(cboard);
	}
	
	int bestScore = -INF;
	const vector<Move> mvs = cboard.GetMoves();
	Board newboard;
	for (unsigned int i=0; i<mvs.size(); ++i) {
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

// NOTE:
// This function is intended for debugging purposes only!
// It acts like NegaMax, but also returns the best move-sequesnce
// It is not up to date! (no transposition tables)
int NegaMaxD(vector<Move> &moves, int moveNr, Board &cboard, int depth)
{
	if (depth == MAX_DEPTH) {
		return SCalculateScore(cboard);
	}
	
	int bestScore = -INF;
	const vector<Move> mvs = cboard.GetMoves();
	vector<Move> bm;
	Board newboard;
	
	for (unsigned int i=0; i<mvs.size(); ++i) {
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
int AlphaBeta(const Board &cboard, const int moveNr, const int max_depth, const int depth, int alpha, int beta)
{
	if (depth == max_depth) {
		return SCalculateScore(cboard);
	}

	// Transposition table stuff
	
	// TODO: maintain hash key with MakeMove and not recalculate it here
	ull zkey = GetZobristKey(cboard);
	ull pzkey = GetPawnZobristKey(cboard);
	HElem ht = GetHElem(GetHashIndex(zkey));
	
	if (depth != 0) {
		// did we calculate this already?
		if (zkey == ht.zkey && pzkey == ht.pzkey &&
			ht.npiece == cboard.GetPieceCount() &&
			ht.depth >= max_depth-depth) {

			if (ht.exact) {
				++hits;
				if (ht.depth - (max_depth-depth) >= 2) ++hd2;
				if (ht.depth - (max_depth-depth) >= 3) ++hd3;
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
	
	bool hexact = false;
	const vector<Move> mvs = cboard.GetMoves();
	Board newboard;
	
	if (mvs.empty()) {
		if (depth == 0) {
			BestMove.flags = DRAW;
		}
		if (cboard.check == MATE || cboard.check == CHECK) {
			return -64000+depth;	// mate (the sooner the better)
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
	
	for (unsigned int i=0; i<mvs.size(); ++i) {
		newboard = cboard;
		newboard.MakeMove(mvs[i]);
		newboard.player = !newboard.player;

		int score = -AlphaBeta(newboard, moveNr+1, max_depth, depth+1, -beta, -alpha);
		
		if (score >= beta) {
			recordHash(zkey, pzkey, false, false, beta, max_depth-depth, cboard.GetPieceCount());
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
	
	// store the key
	recordHash(zkey, pzkey, hexact, true, alpha, max_depth-depth, cboard.GetPieceCount());
	
	return alpha;
}

int IDDFS(const Board &cboard, const int moveNr, const int max_depth)
{
	int score = 0;
	for (int cdepth=1; cboard.check != MATE && cdepth<max_depth; ++cdepth)
		score = AlphaBeta(cboard, moveNr, cdepth);
	return score;
}

// NOTE:
// This function is intended for debugging purposes only!
// It acts like AlphaBeta, but also returns the best move-sequesnce
// It is not up to date! (no transposition tables)
int AlphaBetaD(vector<Move> &moves, int moveNr, Board &cboard, int depth, int alpha, int beta)
{
	if (depth == MAX_DEPTH) {
		return SCalculateScore(cboard);
	}
	
	//int tstart = clock();
	
	int bestScore = -INF;
	const vector<Move> mvs = cboard.GetMoves();
	vector<Move> bm;
	Board newboard;
	
	for (unsigned int i=0; i<mvs.size(); ++i) {
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
