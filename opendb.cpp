#include "opendb.h"

/// Initialise the opening moves database
void Openings::InitOpenings(const char *file_name)
{
	FILE *f = fopen(file_name, "r");
	if (f == NULL) {
		return;
	}

	while (1) {
		char buf[100];

		if (!(fgets(buf, 100, f))) break;
		if (strstr(buf, "#END#"))  break;
		buf[strlen(buf)-1] = 0;
		const string sFEN = buf;

		if (!(fgets(buf, 100, f))) break;
		*strchr(buf, '{') = 0;
		OpenDB.emplace(sFEN, buf);
	}

	fclose(f);
}


/// Given a Board, get the Move from the opening database.
Move Openings::GetMoveFromDB(Board &board)
{
	Move mv;

	mv.flags = mv.piece = mv.promote_to = mv.check = 0;
	mv.source = mv.destination = 64;
	mv.player = board.player;

	const string sFEN = toFEN(board);

	if (OpenDB.find(sFEN) == OpenDB.end()) {
		mv.flags = ERROR; // NO_MOVE
		return mv;
	}

	const string mvch = OpenDB.at(sFEN);

	mv.source = COORDS_TO_INDEX(mvch[1] - '1', 'h' - mvch[0]);
	mv.destination = COORDS_TO_INDEX(mvch[3] - '1', 'h' - mvch[2]);

	// promotion
	if (mvch[5] != 0) {
		mv.flags |= PROMOTION;
		switch (mvch[5]) {
			case 'q': mv.promote_to = 'Q'; break;
			case 'r': mv.promote_to = 'R'; break;
			case 'b': mv.promote_to = 'B'; break;
			case 'n': mv.promote_to = 'N'; break;
		}
	}

	// piece
	int piecetype = board.GetPieceType(mv.source);
	if (piecetype > 5) piecetype -= 7;
	mv.piece = PieceIndexMap[piecetype];

	if (piecetype == 5 && mv.source == 59 && mv.destination == 57) {
		mv.flags |= KING_SIDE_CASTLE;
	}
	else if (piecetype == 5 && mv.source == 3 && mv.destination == 1) {
		mv.flags |= KING_SIDE_CASTLE;
	}
	else if (piecetype == 5 && mv.source == 59 && mv.destination == 61) {
		mv.flags |= QUEEN_SIDE_CASTLE;
	}
	else if (piecetype == 5 && mv.source == 3 && mv.destination == 5) {
		mv.flags |= QUEEN_SIDE_CASTLE;
	}

	piecetype = board.GetPieceType(mv.destination);
	if (piecetype != -1)
		mv.flags |= CAPTURE;

	// if the move gives check or mate
	mv.check = board.WeGiveCheckOrMate(mv);

	return mv;
}


/// Conversion from an internal representation of a Board to a FEN string
char *toFEN(Board &board)
{
	char *fen = (char *) calloc(100, sizeof(char));

	for (int i=7, t=0; i >= 0; i--) {
		int g = 0;
		for (int j = 7; j >= 0; j--) {
			int p_type = board.GetPieceType(i * 8 + j);

			if (p_type != -1 && g) {
				fen[t++] = g + 48;
				g = 0;
			}

			if (p_type == 6 || p_type < 0 || 12 < p_type) ++g;
			else fen[t++] = PieceIndexMap[p_type];
		}
		if (g) fen[t++] = g + 48;

		if (i) fen[t++] = '/';
	}

	if (!board.player) strcat(fen, " w ");
	else strcat(fen, " b ");

	if (board.castling & 1) strcat(fen, "K");
	if (board.castling & 2) strcat(fen, "Q");
	if (board.castling & 4) strcat(fen, "k");
	if (board.castling & 8) strcat(fen, "q");
	if (!board.castling) strcat(fen, "-");

	if (!board.enPassant) {
		strcat(fen, " -");
	}
	else {
		char c[4];

		c[0] = ' ';
		c[1] = 'h' - board.enPassant % 8;
		c[2] = '1' + board.enPassant / 8;
		c[3] = 0;
		strcat(fen, c);
	}

	return fen;
}
