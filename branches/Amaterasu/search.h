#ifndef _SEARCH_H_
#define _SEARCH_H_

#define USE_NS
//#define USE_AB
//#define USE_TT

extern int iMaxDepth;
extern Move g_PVLine[MAX_DEPTH+1];

int Negamax(Board const*const GameBoard,int iDepth,const int iColor);

int AlphaBeta(Board const*const GameBoard,int iDepth,int iAlpha,int iBeta,const int iColor);
int AlphaBetaTT(Board const*const GameBoard,int iDepth,int iAlpha,int iBeta,const int iColor);

int NegaScout(Board const*const GameBoard,int iDepth,int iAlpha,int iBeta,const int iColor,bool bVerify=true);

int RootNode(Board const*const GameBoard,const int iColor,Move*mv);

#endif
