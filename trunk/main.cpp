#include <cstdio>
#include <cstdlib> 
#include <vector>
#include <algorithm>
using namespace std;

#include "board.h"
#include "magic.h"
#include "pieces.h"
#include "opendb.h"
#include "evaluation.h"
#include "engine.h"
#include "acn.h"
#include "xboard.h"

void clrscr(void)
{
#ifndef  __linux__
	system("CLS");
#else
	system("clear");
#endif
}

void terminal_or_xboard()
{
	Board board;
	board.InitChessboard();

	int opponent = 0;
	char com[128];
	
	printf("Choose a color (0 - white, 1 - black)\n"); // '\n' must exist for xboard
	gets(com);
	
	// if we don't recieve a color, then turn to xboard mode!
	if (!sscanf(com, "%d", &opponent)) {
		printf("feature san=1\n");
		printf("feature sigint=0\n");
		printf("feature sigterm=0\n");
		printf("feature name=0\n");
		printf("feature time=0\n");
		printf("feature draw=0\n");
		printf("feature reuse=0\n");
		printf("feature analyze=0\n");
		printf("feature myname=\"BlitzKibitz\"\n");
		printf("feature done=1\n");
		XPlay(board);
		return;
	}
	
	printf("Enter the move in ACN format:\n");
	printf("You can exit the program with \"quit\"\n");
	board.PrintBoard();
	
	Openings Op;
	Op.InitOpenings("openings.bk");

	int moveNr = 1;
	int startMoves = 0;
	int score = 0;
	while (board.check != MATE) {
		Move m;
		
		if (board.player == opponent) {
			gets(com);
			
			if (strcmp(com, "quit") == 0) break;
			if (strcmp(com, "q") == 0) break;
			
			m.flags = 0;
			m = DecodeACN(com, board);
			clrscr();
		}
		else {
			moveNr += 2;
			m = Op.GetMoveFromDB(board);
			if (m.flags != ERROR) {
#ifdef DEBUG
				printf("Move found in opening database:\n");
				printf("%c%d -> %c%d\n", 'h'-m.source%8, m.source/8+1, 'h'-m.destination%8, m.destination/8+1);
#endif
			}
			else {
				if (board.GetPieceCount() < 11) {
					DEPTH_LIMIT = 8;
					if (board.GetPieceCount() < 7)
						DEPTH_LIMIT = 9;
					MAX_DEPTH = 7;
				}
				if (startMoves < 4) {
					startMoves++;
					MAX_DEPTH++;
					score = AlphaBeta(board, moveNr);
					MAX_DEPTH--;
				}
				else {
					// vector<Move> moves;
					// score = NegaMaxD(moves, moveNr, board, 0);
					// score = NegaMax(board, moveNr);
					score = AlphaBeta(board, moveNr);
					// score = AlphaBetaD(moves, moveNr, board);
				}
				printf("BestMove has score %d\n", score);
				m = BestMove;
//				for (int i=0; i<moves.size(); ++i)
//					printf("%c%d -> %c%d\n", 'h'-moves[i].source%8, moves[i].source/8+1, 'h'-moves[i].destination%8, moves[i].destination/8+1);
			}

			string enc = EncodeACN(m, board);
			printf("move %s\n", enc.c_str());
		}
		
		if (strcmp(com, "quit") == 0) break;
		if (strcmp(com, "q") == 0) break;
		

		if (!(m.flags & ERROR)) {
			board.MakeMove(m);
			board.player = !board.player;
		}
		
		board.PrintBoard();
	}

}

int main(int argc, char* argv[])
{
#ifndef NORAND
	srand(time(NULL));
#endif

	initmagicmoves();
	initPieces();
	InitHash();
	
	terminal_or_xboard();

	return 0;
}