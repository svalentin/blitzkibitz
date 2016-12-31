#include "xboard.h"

void XPlay(int normal_max_depth, Board &board)
{
	int opponent = 0;
	char cbuffer[128];
	string buffer;
	Openings Op;
	Op.InitOpenings("openings.bk");

	FILE *logf = fopen("BlitzKibitz-xboard.log", "wt");
	FILE *loga = fopen("BlitzKibitz-oponent.log", "wt");
	setvbuf(logf, 0, _IONBF, 0);
	setvbuf(loga, 0, _IONBF, 0);

	// disable buffer use
	setvbuf(stdin, 0, _IONBF, 0);
	setvbuf(stdout, 0, _IONBF, 0);

	fprintf(logf, "buffers set to 0\n");
	int moveNr = 1;
	fprintf(logf, "Begin - max depth %d\n", normal_max_depth);
	printf("Begin\n");
	while (1) {
		Move m;

		if (board.player == opponent || opponent == -1) {
			fgets(cbuffer, 128, stdin); cbuffer[strlen(cbuffer)-1] = 0;
			buffer = cbuffer;
			fprintf(logf, "cbuffer: `%s`\n", cbuffer);

			m.flags = 0;
			m = DecodeACN(cbuffer, board);
//			fprintf(logf, "DecodeACN finished with m.flags=%d\n", m.flags);
			if (m.check == MATE) {
				printf("resign\n");
			}
			else if (m.flags & ERROR) {
				if (buffer == "quit") {
					fgets(cbuffer, 128, stdin); // make sure there is nothing more to get
					break;
				}
				else if (buffer == "new") {
					// new game
					// TODO: test it some more!
					board.InitChessboard();
					opponent = 0;
				}
				else if (buffer == "xboard") {
					// ok..
				}
				else if (buffer == "force") {
					opponent = -1;
				}
				else if (buffer == "go") {
					//opponent = !board.player;
				}
				else if (buffer == "white") {
					opponent = 1;
				}
				else if (buffer == "black") {
					opponent = 0;
				}
				else if (buffer == "go") {
					opponent = !board.player;
				}
				// make sure we don't consider valid xboard commands as errors
				else if (buffer.find("accepted")!=string::npos
								 || buffer.find("random")!=string::npos) {
				}
				else if (buffer.find("level")!=string::npos
								 || buffer.find("hard")!=string::npos) {
				}
				else if (buffer.find("time")!=string::npos
								 || buffer.find("otim")!=string::npos) {
				}
				else if (buffer == "post") {
					// Turn on thinking/pondering output.
					// We don't do that so ignore it.
				}
				else {
					fprintf(logf, "error :|\n");
					board.PrintBoard(logf);
					fflush(logf);
				}
			}
			else {
				fprintf(loga, "%s\n", cbuffer);
				fflush(loga);
			}
		}
		else {
			fprintf(logf, "-->My turn!\n");
			moveNr += 2;
			m = Op.GetMoveFromDB(board);
			if (m.flags == ERROR) {
				int score = 0;
				int max_depth = normal_max_depth;
				if (moveNr < 8) {
					max_depth = max_depth + 1;
				}
				if (board.GetPieceCount() < 11) {
					max_depth = max_depth + 1;
				}
				fprintf(logf, "Playing at %d plies\n", max_depth);
				score = IDDFS(board, moveNr, max_depth);
				m = BestMove;
				fprintf(logf, "Move #%d\n", moveNr);
				fprintf(logf, "BestMove has score %d\n", score);
			}
			else {
				fprintf(logf, "Move #%d found in DB!\n", moveNr);
			}

			if (m.flags & DRAW) {
				// I can't make any move
				printf("1/2-1/2 {Stalemate}");
				fprintf(logf, "Stalemate\n");
				board.player = !board.player;
			}
			else {
				string enc = EncodeACN(m, board);
				fprintf(logf, "%s\n", enc.c_str());
				printf("move %s\n", enc.c_str());

				if (m.check == MATE) {
					printf("checkmate");
					board.MakeMove(m);
					board.player = !board.player;
					board.PrintBoard(logf);
				}
			}
		}

		if (!((m.flags & ERROR) || m.check == MATE || (m.flags & DRAW))) {
			board.MakeMove(m);
			board.player = !board.player;
			board.PrintBoard(logf);
			fprintf(logf, "Current board has score %d\n", CalculateScore(board));
			fprintf(logf, "FEN: %s\n", toFEN(board));
			fprintf(logf, "Total hash hits: %d\n", hits);
			fprintf(logf, "Total hash hits with depth diffference >=2 : %d\n", hd2);
			fprintf(logf, "Total hash hits with depth diffference >=3 : %d\n", hd3);
		}

		fprintf(logf, "\n");
	}
	fprintf(logf, "I'm done!\n");

	fclose(logf);
	fclose(loga);
}
