#include"warnings.h"
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<vector>
#include<algorithm>
#include"random.h"
#include"constants.h"
#include"bitboards.h"
#include"magic.h"
#include"moves.h"
#include"board.h"
#include"hash.h"
#include"evaluate.h"
#include"timecontrol.h"
#include"search.h"
#include"xboard.h"
#include"util.h"
using namespace std;

int PieceValue[6]={100,300,300,500,900,32000};
int ColorScore[2]={1,-1};

U64 ullNodesSearched;
TCtrl g_TimeControl;


int main(int argn,char**args)
{
	int iCol=WHITE;
	char str[100]={0};
	Board GameBoard={0};
	Move mv1,mv2,mv;

	InitRandGenByArray();
	InitHash();
	InitMagicMoves();
	InitChessboard(&GameBoard);

	xboard(&GameBoard);
	//LoadProblem(&GameBoard,"mate5_6.txt");
	PrintBoard(&GameBoard);
	
	int src=33,i=0,score=0,start=1;
	vector<Move> vM;

	g_TimeControl.SetAllocatedTime(20.5f);
	do
	{
		getchar();
		printf("Start thinking as %s\n",iCol?"Black":"White");

		int i;
		Move mvBestMove;
		g_TimeControl.SetStartTime();
		printf("Time status %d\n",g_TimeControl.GetStatus());
		for(i=4;i<=MAX_DEPTH;i++)
		{
			iMaxDepth=i;
			score=RootNode(&GameBoard,iCol,&mv);
			if(g_TimeControl.GetStatus()) break;	// time ran out during last search
			else 
			mvBestMove=mv;
		}
		if(g_TimeControl.GetStatus())
		{
			printf("Time ran out during search at ply %d\nReverting to search results at ply %d\n",i,i-1);
		}
		printf("Done thinking\nTime %f\nNodes searched %lld\n",g_TimeControl.GetElapsedTime(),ullNodesSearched);
		char mstr[100]={0};

		EncodeMove(&GameBoard,&mvBestMove,iCol,mstr);

		printf("\nMove %s scored %d\n",mstr,score);
		GameBoard.MakeMove(&mvBestMove);
		PrintBoard(&GameBoard);
		mv.iDest=mv.iSrc=mv.iFlags=mv.iType=0;
		iCol=!iCol;
	}while(1);

	
	getchar();

	printf("Begin Game\n");
	do
	{
		//scanf("%s",str);
		fgets(str,100,stdin);
		if(strcmp(str,"quit")==0) break;
		DecodeMove(str,&GameBoard,iCol,&mv);
		PrintMoveInfo(mv);
		getchar();
		getchar();
		GameBoard.MakeMove(&mv);
		clrscr();
		PrintBoard(&GameBoard);
		printf("Start thinking\n");
		Move mvBestMove;
		/*if(start==1) 
		{
			strcpy(str,"e5");
			DecodeMove(str,&GameBoard,!iCol,&mv);
			start=0;
		}
		else*/
		{
			g_TimeControl.SetStartTime();
			int i;
			for(i=4;i<=MAX_DEPTH;i++)
			{
				iMaxDepth=i;
				score=RootNode(&GameBoard,!iCol,&mv);
				if(g_TimeControl.GetStatus()) break;	// time ran out during last search
				else 
					mvBestMove=mv;
			}
			if(g_TimeControl.GetStatus())
			{
				printf("Time ran out during search at ply %d\nReverting to search results at ply %d\n",i,i-1);
			}
		}
		printf("Done thinking\nTime %f\nNodes searched %lld\n",g_TimeControl.GetElapsedTime(),ullNodesSearched);
		if(score>LOSS)
		{
			char mstr[100];
			EncodeMove(&GameBoard,&mvBestMove,!iCol,mstr);
			printf("\nMove %s scored %d\nPress any key...\n",mstr,score);
			getchar();
			GameBoard.MakeMove(&mv);
			clrscr();
			PrintBoard(&GameBoard);
		}
	}while(1);

	PrintBitBoard( Nmagic(44) );
	//printf("%d",59>>3);
	getchar();
	return 0;
}
