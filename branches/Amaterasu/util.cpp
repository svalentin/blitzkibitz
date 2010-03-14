#include"warnings.h"
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include"constants.h"
#include"bitboards.h"
#include"moves.h"
#include"board.h"
#include"util.h"

void clrscr(void)
{
#ifdef __linux__
	system("clear");
#else
	system("CLS");
#endif
}

void LoadProblem(Board*GameBoard,char*name)
{
	FILE*fil=fopen(name,"r");
	char str[10];
	int j,i,n,iType,iCol,iRow;

	for(i=0;i<2;i++)
	{
		fscanf(fil,"%d",&n);
		for(j=0;j<n;j++)
		{
			fscanf(fil,"%s",str);
			switch(str[0])
			{
				case 'N': iType=1;	break;
				case 'B': iType=2;	break;
				case 'R': iType=3;	break;
				case 'Q': iType=4;	break;
				case 'K': iType=5;	break;
				case 'P': iType=0;	break;
			}
			iCol=7-str[1]+'a';
			iRow=str[2]-'1';
			GameBoard->bbPieceBoards[iType+i*7]=PLACE_PIECE(GameBoard->bbPieceBoards[iType+i*7],COORDS_2_INDEX(iRow,iCol));
			GameBoard->bbPieceBoards[WHITE_PIECES+i*7]=PLACE_PIECE(GameBoard->bbPieceBoards[WHITE_PIECES+i*7],COORDS_2_INDEX(iRow,iCol));
		}
	}
	GameBoard->chCastlingRights=0xFF;
}
