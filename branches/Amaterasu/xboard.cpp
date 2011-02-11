#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<vector>
#include<algorithm>

#include"bitboards.h"
#include"board.h"
#include"moves.h"
#include"timecontrol.h"
#include"evaluate.h"
#include"search.h"
#include"xboard.h"
#include"warnings.h"

bool bRandom,bPost;
int iColor;
int iFirstNMoves;
int iTimeAvailable;
int iTimeMode;
int iTimeLeft,iOTimeLeft;

void xboard(Board * const GameBoard)
{
	char sInput[256]={0},sOutput[256]={0},aux[256]={0};
	int pingN=0,i,score,start=1,iNumMoves=0;
	Move mvEnemy,mv,mvBestMove;
	FILE *logxb = fopen("Amaterasu_xboard.log", "w");
	setvbuf(logxb, 0, _IONBF, 0);
	setvbuf(stdin, 0, _IONBF, 0);
	setvbuf(stdout, 0, _IONBF, 0);

	g_TimeControl.SetAllocatedTime(7.4f);
	do
	{
		memset(sInput,0,50);
		if(fgets(sInput,256, stdin))
		{
			fprintf(logxb,"%s\n",sInput);
			if(DecodeMove(sInput,GameBoard,!iColor,&mvEnemy)==-1 || mvEnemy.iSrc==-1) // it's a winboard command
			{
				if(strcmp(sInput,"go\n")==0)
				{
					start=1;
					goto start_game; // yes the infamous goto...makes things a lot easier sometimes
				}
				if(strcmp(sInput,"force\n")==0)
				{
					iNumMoves=start=0;
					InitChessboard(GameBoard);
				}
				else if(strncmp(sInput,"level ",6)==0)
				{
					sscanf(sInput,"level %d %d %d\n",&iFirstNMoves,&iTimeAvailable,&iTimeMode);
					sprintf(sOutput,"level %d %d %d\n",iFirstNMoves,iTimeAvailable,iTimeMode);
					fprintf(logxb,"%s",sOutput);
				}
				else if(strncmp(sInput,"ping",4)==0)
				{
					sscanf(sInput,"ping %d\n",&pingN);
					sprintf(sOutput,"pong %d\n",pingN);
					puts(sOutput);
				}
				else if(strcmp(sInput,"xboard\n")==0)
				{
					puts("feature ping=1 san=1\n");
					fprintf(logxb,"feature ping=1 san=1\n");
				}
				else if(strcmp(sInput,"new\n")==0)
				{
					iColor=BLACK;
				}
				else if(strcmp(sInput,"random\n")==0)	bRandom=true;
				else if(strcmp(sInput,"post\n")==0)		bPost=true;
				else if(strcmp(sInput,"nopost\n")==0)	bPost=false;
				else if(strcmp(sInput,"white\n")==0)
				{
					start=1;
					iColor=WHITE;
				}
				else if(strcmp(sInput,"black\n")==0)
				{
					start=1;
					iColor=BLACK;
				}
				else if(strcmp(sInput,"quit\n")==0) break;
			}
			else // it's a move
			{
				fprintf(logxb,"received move  %s\n",sInput);
				GameBoard->MakeMove(&mvEnemy);
				PrintBoard(GameBoard,logxb);

				if(start)
				{
					start_game: // we jump here when we receive the "go" command from winboard
					g_TimeControl.SetStartTime();
					for(i=4;i<=MAX_DEPTH;i++)
					{
						fprintf(logxb,"Start search at depth %d\n",i);
						iMaxDepth=i;
						score=RootNode(GameBoard,iColor,&mv);
						if(g_TimeControl.GetStatus()) break;	// time ran out during last search
						else mvBestMove=mv;

						int iAuxCol=!iColor;
						EncodeMove(GameBoard,&g_PVLine[0],iColor,aux);
						sprintf(sOutput,"%d %d %d %lld %s ",i,score,(int)(g_TimeControl.GetElapsedTime()*100),ullNodesSearched,aux);
						for(int j=1;j<=i;j++)
						{
							EncodeMove(GameBoard,&g_PVLine[j],iAuxCol,aux);
							sprintf(sOutput,"%s %s ",sOutput,aux);
							iAuxCol=!iAuxCol;
						}
						sprintf(sOutput,"%s\n",sOutput);
						puts(sOutput);
						mvBestMove=mv;
					}

					GameBoard->MakeMove(&mvBestMove);
					char mstr[100];
					EncodeMove(GameBoard,&mvBestMove,iColor,mstr);
					sprintf(sOutput,"move %s",mstr);
					puts(sOutput);
					fprintf(logxb,"%s\n",mstr);
					iNumMoves++;
				}
			}
		}
	}while(1);
}
