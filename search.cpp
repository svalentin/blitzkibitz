#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<vector>
#include<algorithm>
#include"constants.h"
#include"bitboards.h"
#include"board.h"
#include"moves.h"
#include"evaluate.h"
#include"search.h"
#include"hash.h"
#include"timecontrol.h"
using namespace std;

Move g_PVLine[MAX_DEPTH+1];
int iMaxDepth;

int Negamax(Board const*const GameBoard,int iDepth,const int iColor)
{
	int score,bestscore=LOSS*(iMaxDepth+1),i;
	ullNodesSearched++;
	vector<Move> vMoves;
	vMoves=GameBoard->GetMoves(iColor);
	if(!vMoves.size())
	{
		if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,iColor),GameBoard->GetAttackBoard(!iColor))) return LOSS*(iMaxDepth-iDepth);
		if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,!iColor),GameBoard->GetAttackBoard(iColor))) return WIN*(iMaxDepth-iDepth);
		return DRAW;
	}
	if(iDepth>=iMaxDepth) return EvaluateBoard(GameBoard,iColor);
	else sort(vMoves.begin(),vMoves.end());
	for(i=0;i<(int)vMoves.size();i++)
	{
		Board AuxBoard=(*GameBoard);
		AuxBoard.MakeMove(&vMoves[i]);
		score=-Negamax(&AuxBoard,iDepth+1,!iColor);
		if(score>bestscore) bestscore=score;
	}
	return bestscore;
}

int AlphaBeta(Board const*const GameBoard,int iDepth,int iAlpha,int iBeta,const int iColor)
{
	int score,bestscore=LOSS*(iMaxDepth+1),i;
	ullNodesSearched++;
	vector<Move> vMoves;
	vMoves=GameBoard->GetMoves(iColor);
	if(!vMoves.size())
	{
		if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,iColor),GameBoard->GetAttackBoard(!iColor))) return LOSS*(iMaxDepth-iDepth);
		if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,!iColor),GameBoard->GetAttackBoard(iColor))) return WIN*(iMaxDepth-iDepth);
		return DRAW;
	}
	if(iDepth>=iMaxDepth) return EvaluateBoard(GameBoard,iColor);
	else sort(vMoves.begin(),vMoves.end());
	for(i=0;i<(int)vMoves.size();i++)
	{
		Board AuxBoard=(*GameBoard);
		AuxBoard.MakeMove(&vMoves[i]);
		score=-AlphaBeta(&AuxBoard,iDepth+1,-iBeta,-iAlpha,!iColor);
		if(score>iBeta) return score; // beta cutoff
		if(score>bestscore)
		{
			bestscore=score;
			if(score>iAlpha) iAlpha=score;
		}
	}
	return bestscore;
}

inline void recordHash(U64 zkey, U64 pzkey, bool exact, bool lbound, int score,
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
int AlphaBetaTT(Board const*const GameBoard,int iDepth,int iAlpha,int iBeta,const int iColor)
{
	int score,bestscore=LOSS*(iMaxDepth+1),i;
	ullNodesSearched++;
	vector<Move> vMoves;
	//U64 zkey = GameBoard->GetBoardZKey(iColor);
	//U64 pzkey = GameBoard->GetPawnZKey();
	U64 zkey = GetZobristKey(GameBoard,iColor);
	U64 pzkey = GetPawnZobristKey(GameBoard);

	vMoves=GameBoard->GetMoves(iColor);
	if(!vMoves.size())
	{
		if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,iColor),GameBoard->GetAttackBoard(!iColor))) score=LOSS*(iMaxDepth-iDepth);
		else if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,!iColor),GameBoard->GetAttackBoard(iColor))) score=WIN*(iMaxDepth-iDepth);
		else score=DRAW;
		recordHash(zkey, pzkey,true, true, score, MAX_DEPTH-iDepth, GameBoard->GetPieceCount());
		return score;
	}

	HElem ht = GetHElem(GetHashIndex(zkey));
	if(zkey == ht.zkey && pzkey == ht.pzkey && ht.npiece == GameBoard->GetPieceCount() && ht.depth > MAX_DEPTH-iDepth)
	{
		if (ht.exact)
		{
			++hits;
			if (ht.depth - (MAX_DEPTH-iDepth) >= 2) ++hd2;
			if (ht.depth - (MAX_DEPTH-iDepth) >= 3) ++hd3;
			return ht.score;
		}
		else
		{
			if (ht.lbound && ht.score <= iAlpha)
				return iAlpha;
			else if (!ht.lbound && ht.score >= iBeta)
				return iBeta;
		}
	}

	if(iDepth>=iMaxDepth)
	{
		score=EvaluateBoard(GameBoard,iColor);
		recordHash(zkey, pzkey,true, true, score, MAX_DEPTH-iDepth, GameBoard->GetPieceCount());
		return score;
	}

	// Time has run out?
	if(g_TimeControl.GetStatus()) return 0; // yes
	g_TimeControl.UpdateTime();				// no...update time and go on

	sort(vMoves.begin(),vMoves.end());

	bool hexact = false;
	
	for(i=0;i<(int)vMoves.size();i++)
	{
		Board AuxBoard=(*GameBoard);
		AuxBoard.MakeMove(&vMoves[i]);
		score=-AlphaBetaTT(&AuxBoard,iDepth+1,-iBeta,-iAlpha,!iColor);

		g_TimeControl.UpdateTime();				// update time
		if(g_TimeControl.GetStatus()) return 0;	// time has run out
		
		if(score>iBeta)
		{
			recordHash(zkey, pzkey, false, false, iBeta, MAX_DEPTH-iDepth, GameBoard->GetPieceCount());
			return score; // beta cutoff
		}
		if(score>bestscore)
		{
			bestscore=score;
			if(score>iAlpha)
			{
				hexact = true;
				iAlpha=score;
			}
		}
	}
	recordHash(zkey, pzkey, hexact, true, bestscore, MAX_DEPTH-iDepth, GameBoard->GetPieceCount());
	return bestscore;
}

int NegaScoutTT(Board const*const GameBoard,int iDepth,int iAlpha,int iBeta,const int iColor)
{
	int b=iBeta,score,i,betavalue,bestscore=LOSS*(iMaxDepth+1);
	ullNodesSearched++;
	vector<Move> vMoves;
	//U64 zkey = GameBoard->GetBoardZKey(iColor);
	//U64 pzkey = GameBoard->GetPawnZKey();
	U64 zkey = GetZobristKey(GameBoard,iColor);
	U64 pzkey = GetPawnZobristKey(GameBoard);

	vMoves=GameBoard->GetMoves(iColor);
	if(!vMoves.size())
	{
		if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,iColor),GameBoard->GetAttackBoard(!iColor))) return LOSS*(iMaxDepth-iDepth);
		if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,!iColor),GameBoard->GetAttackBoard(iColor))) return WIN*(iMaxDepth-iDepth);
		return DRAW;
	}

	HElem ht = GetHElem(GetHashIndex(zkey));
	if(zkey == ht.zkey && pzkey == ht.pzkey && ht.npiece == GameBoard->GetPieceCount() && ht.depth > MAX_DEPTH-iDepth)
	{
		if (ht.exact)
		{
			++hits;
			if (ht.depth - (MAX_DEPTH-iDepth) >= 2) ++hd2;
			if (ht.depth - (MAX_DEPTH-iDepth) >= 3) ++hd3;
			return ht.score;
		}
		else
		{
			if (ht.lbound && ht.score <= iAlpha)
				return iAlpha;
			else if (!ht.lbound && ht.score >= iBeta)
				return iBeta;
		}
	}

	if(iDepth>=iMaxDepth)
	{
		score=EvaluateBoard(GameBoard,iColor);
		recordHash(zkey, pzkey,true, true, score, MAX_DEPTH-iDepth, GameBoard->GetPieceCount());
		return score;
	}

	// Time has run out?
	if(g_TimeControl.GetStatus()) return 0; // yes
	g_TimeControl.UpdateTime();				// no...update time and go on
	
	sort(vMoves.begin(),vMoves.end());

	bool hexact = false;
	for(i=0;i<(int)vMoves.size();i++)
	{
		Board AuxBoard=(*GameBoard);
		AuxBoard.MakeMove(&vMoves[i]);

		betavalue=b;
		score=-NegaScoutTT(&AuxBoard,iDepth+1,-b,-iAlpha,!iColor);
		if(score>iAlpha && score<iBeta && i>1)
		{
			betavalue=iBeta;
			iAlpha=-NegaScoutTT(&AuxBoard,iDepth+1,-iBeta,-iAlpha,!iColor);
		}

		if(score>iAlpha)
		{
			hexact = true;
			iAlpha=score;
		}
		if(iAlpha>iBeta)
		{
			recordHash(zkey, pzkey, false, false, betavalue, MAX_DEPTH-iDepth, GameBoard->GetPieceCount());
			return iAlpha;
		}
		b=iAlpha+1;

		if(g_TimeControl.GetStatus()) return 0; // yes
		g_TimeControl.UpdateTime();				// no...update time and go on
	}
	recordHash(zkey, pzkey, hexact, true, iAlpha, MAX_DEPTH-iDepth, GameBoard->GetPieceCount());
	return iAlpha;
}

int NegaScout(Board const*const GameBoard,int iDepth,int iAlpha,int iBeta,const int iColor,bool bVerify)
{
	int b=iBeta,score,i;
	bool bFailHigh=false;
	ullNodesSearched++;
	vector<Move> vMoves;
	vMoves=GameBoard->GetMoves(iColor);
	if(!vMoves.size())
	{
		if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,iColor),GameBoard->GetAttackBoard(!iColor))) return LOSS*(iMaxDepth-iDepth);
		if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,!iColor),GameBoard->GetAttackBoard(iColor))) return WIN*(iMaxDepth-iDepth);
		return DRAW;
	}
	if(iDepth>=iMaxDepth) return EvaluateBoard(GameBoard,iColor);
	else sort(vMoves.begin(),vMoves.end());

	// Null move heuristic
	if(!KING_IN_CHECK(GameBoard->GetPieceBoard(KING,iColor),GameBoard->GetAttackBoard(!iColor)))
	{
		score=-NegaScout(GameBoard,iDepth+DEPTH_REDUCTION+1,-b,-iBeta+1,!iColor);
		if(score>=iBeta)
		{
			return iBeta;
		}
	}

	// did time run out?
	if(g_TimeControl.GetStatus()) return 0; // yes
	g_TimeControl.UpdateTime();				// no...update time and go on

	// regular negascout search
	for(i=0;i<(int)vMoves.size();i++)
	{
		Board AuxBoard=(*GameBoard);
		AuxBoard.MakeMove(&vMoves[i]);

		score=-NegaScout(&AuxBoard,iDepth+1,-b,-iAlpha,!iColor);
		if(score>iAlpha && score<iBeta && i>1) 
			iAlpha=-NegaScout(&AuxBoard,iDepth+1,-iBeta,-iAlpha,!iColor);
		if(score>iAlpha)
		{
			g_PVLine[iDepth]=vMoves[i];
			iAlpha=score;
		}
		if(iAlpha>=iBeta)
			return iAlpha;
		b=iAlpha+1;

		if(g_TimeControl.GetStatus()) return 0; // yes
		g_TimeControl.UpdateTime();				// no...update time and go on
	}
	return iAlpha;
}

int RootNode(Board const*const GameBoard,const int iColor,Move*mv)
{
	int score,bestscore=LOSS*(iMaxDepth+1),iAlpha=LOSS*(iMaxDepth+1),iBeta=WIN*(iMaxDepth+1),b=WIN*(iMaxDepth+1),i;
	ullNodesSearched=1;
	vector<Move> vMoves;
	vMoves=GameBoard->GetMoves(iColor);
	if(!vMoves.size())
	{
		if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,iColor),GameBoard->GetAttackBoard(!iColor))) return LOSS*iMaxDepth;
		if(KING_IN_CHECK(GameBoard->GetPieceBoard(KING,!iColor),GameBoard->GetAttackBoard(iColor))) return WIN*iMaxDepth;
		return DRAW;
	}
	else sort(vMoves.begin(),vMoves.end());

	if(g_TimeControl.GetStatus()) return 0; // time ran out

	hits=0;
#ifdef USE_NS
	for(i=0;i<(int)vMoves.size();i++)
	{
		Board AuxBoard=(*GameBoard);
		AuxBoard.MakeMove(&vMoves[i]);

		#ifdef USE_TT
			score=-NegaScoutTT(&AuxBoard,1,-b,-iAlpha,!iColor);
			if(score>iAlpha && score<iBeta && i>1) 
				score=-NegaScoutTT(&AuxBoard,1,-iBeta,-iAlpha,!iColor);
		#else
			score=-NegaScout(&AuxBoard,1,-b,-iAlpha,!iColor);
			if(score>iAlpha && score<iBeta && i>1) 
				score=-NegaScout(&AuxBoard,1,-iBeta,-iAlpha,!iColor);
		#endif

		if(score>iAlpha)
		{
			iAlpha=score;
			g_PVLine[0]=*mv=vMoves[i];	// save our best move so far
		}
		if(iAlpha>iBeta)
			return iAlpha;
		b=iAlpha+1;

		g_TimeControl.UpdateTime();
		if(g_TimeControl.GetStatus()) return 0; // time ran out on us
	}
	//printf("Number of hash hits %d\n",hits);
	return iAlpha;
#else
	for(i=0;i<(int)vMoves.size();i++)
	{
		Board AuxBoard=(*GameBoard);
		AuxBoard.MakeMove(&vMoves[i]);

	#ifdef USE_AB
	
		#ifdef USE_TT
			score=-AlphaBetaTT(&AuxBoard,1,-iBeta,-iAlpha,!iColor);
		#else
			score=-AlphaBeta(&AuxBoard,1,-iBeta,-iAlpha,!iColor);
		#endif
		if(score>iBeta) return score; // beta cutoff
		if(score>bestscore)
		{
			bestscore=score;
			*mv=vMoves[i];	// save our best move so far
			if(score>iAlpha) iAlpha=score;
		}
	#else
		score=-Negamax(&AuxBoard,1,!iColor);
		if(score>bestscore)
		{
			bestscore=score;
			*mv=vMoves[i];
		}
	#endif
		g_TimeControl.UpdateTime();
		if(g_TimeControl.GetStatus()) return 0; // time ran out on us
	}
	printf("Number of hash hits %d\n",hits);
	return bestscore;
#endif
}
