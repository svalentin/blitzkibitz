#include "acn.h"

FILE *flog = fopen("BlitzKibitz-ACN.log", "w");

Move DecodeACN(const char *sMove, Board &board)
{
	setvbuf(flog, 0, _IONBF, 0);
	fprintf(flog, "\nDecode %s\n", sMove);

	Move m;
	m.flags = m.piece = m.promote_to = m.check = 0;
	m.source = m.destination = 64;
	m.player = board.player;

	int i;
	int xi=64, yi=64, xf=64, yf=64;

	i = strlen(sMove)-1;

	if (sMove[i] == '+') {				// check
		m.check |= CHECK;
		--i;
	}
	if (sMove[i] == '#') {				// check mate
		m.check |= MATE;
		--i;
	}

	if (strcmp(sMove, "O-O-O") == 0) {	// queen side castle
		m.flags |= QUEEN_SIDE_CASTLE;
		return m;
	}
	if (strcmp(sMove, "O-O") == 0) {	// king side castle
		m.flags |= KING_SIDE_CASTLE;
		return m;
	}

	// if this moves leads to a promotion
	char promovare = 0;
	if (IsPiece(sMove[i])) {
		promovare = sMove[i];
		m.flags |= PROMOTION;
		--i;
		if (sMove[i] == '=') --i;
	}

	// destination place

	// line and column
	if (sMove[i]<'1' || sMove[i]>'8') {
		fprintf(flog, "ACN format error!(1)\n");
		m.flags = ERROR;
		return m;
	}
	else {
		xf = sMove[i--]-'1';
	}

	if (sMove[i]<'a' || sMove[i]>'h') {
		fprintf(flog, "ACN format error!(2)\n");
		m.flags = ERROR;
		return m;
	}
	else {
		yf = 'h'-sMove[i--];
	}

	// Check capture
	if (sMove[i] == 'x') {
		m.flags |= CAPTURE;
		--i;
	}

	// next:
	// 1. nothing => pawn
	// 2. piece type
	// 3. line or column => indicates initial position

	// NOTICE:
	// needs more testing in all possible combinations!
	// try to vary piece type, line, column and capture

	if (i<0) {
		// 1. nothing => pawn
		m.piece = 'P';	// pion
	}
	else {
		if (IsPiece(sMove[i])) {
			// 2. piece type
			m.piece = sMove[i];
		}
		else {
			// 3. line and/or column
			if (sMove[i]>='a' && sMove[i]<='h') {
				yi = 'h'-sMove[i--];
			}
			else {
				if (sMove[i]>='0' && sMove[i]<='8') {
					xi = sMove[i--]-'1';

					// avem si linie?
					if (sMove[i]>='a' && sMove[i]<='h') {
						yi = 'h'-sMove[i--];
					}
				}
				else {
					fprintf(flog, "ACN format error!(3)\n");
					m.flags = ERROR;
					return m;
				}
			}

			// next: piece type or nothing
			if (i<0)
				m.piece = 'P';
			else if (IsPiece(sMove[i]))
				m.piece = sMove[i];

			if (m.piece == 0) {
				fprintf(flog, "ACN format error!(4)\n");
				m.flags = ERROR;
				return m;
			}
		}
	}

	m.source		= COORDS_TO_INDEX(xi, yi);
	m.destination	= COORDS_TO_INDEX(xf, yf);
	if (promovare)
		m.promote_to = promovare;

	// check "en passant" flag (pawn piece + capture)
	if (m.piece == 'P' && m.flags == CAPTURE)
		if (board.GetPieceType(m.destination) == -1)
			m.flags = ENPASS;

	if (xi==64 || yi==64) {
		if (!m.FindCoordinates(board)) {
			fprintf(flog, "Error while searching for piece!\n");
			fprintf(flog, "Searched for a piece that gets to %d %d\n", xf, yf);
			fprintf(flog, "What do we know about the starting position: %d %d\n", xi, yi);
			fprintf(flog, "Capture: %d\n", (m.flags & CAPTURE)!=0);
			fprintf(flog, "Piece: %c\n", m.piece);
			m.flags = ERROR;
			return m;
		}
	}

	return m;
}


string EncodeACN(Move &mv, Board &board)
{
	setvbuf(flog, 0, _IONBF, 0);
	pair<int, int> src = INDEX_TO_COORDS(mv.source);
	pair<int, int> dst = INDEX_TO_COORDS(mv.destination);
	fprintf(flog, "\nEncode %c %d %d -> %d %d\n",
		mv.piece,
		src.first, src.second,
		dst.first, dst.second);

	string acn = "";
	acn.reserve(16);	// preallocate 16 bytes to prevent reallocation

	if (mv.flags == KING_SIDE_CASTLE) {
		acn = "O-O";
		if (mv.check == CHECK)
			acn += "+";
		if (mv.check == MATE)
			acn += "#";
		fprintf(flog, "%s\n", acn.c_str());
		return acn;
	}

	if (mv.flags == QUEEN_SIDE_CASTLE) {
		acn = "O-O-O";
		if (mv.check == CHECK)
			acn += "+";
		if (mv.check == MATE)
			acn += "#";
		fprintf(flog, "%s\n", acn.c_str());
		return acn;
	}


	int lin = 64, col = 64;
	Move mt;
	mt.promote_to = mt.check = 0;

	if (toupper(mv.piece) != 'P')
		acn += mv.piece;

	// set source
	col = mv.source % 8;
	lin = mv.source / 8;

	mt.piece = toupper(mv.piece);
	mt.source = COORDS_TO_INDEX(64, 64);
	mt.destination = mv.destination;
	mt.flags = mv.flags;
	mt.player = mv.player;
	mt.check = mv.check;
	mt.promote_to = mv.promote_to;

	if ((toupper(mv.piece) == 'P' && (mv.flags & ENPASS || mv.flags & CAPTURE)) || !mt.FindCoordinates(board)) {
		// column desabiguazation
		mt.source = COORDS_TO_INDEX(64, col);
		if (mt.FindCoordinates(board)) {
			acn += ('h' - col);
		}
		else {
			// line desabiguazation
			mt.source = COORDS_TO_INDEX(lin, 64);
			if (mt.FindCoordinates(board)) {
				acn += (char)('1'+lin);
			}
			else {
				// line & column desabiguazation
				mt.source = COORDS_TO_INDEX(lin, col);
				acn += (char)('h' - col);
				acn += (char)('1' + lin);
			}
		}
	}

	// capture
	if (mv.flags & CAPTURE || mv.flags & ENPASS)
		acn += 'x';

	// set destination
	col = mv.destination % 8;
	lin = mv.destination / 8;

	acn += ('h' - col);
	acn += ('1' + lin);

	// promote
	if (mv.promote_to) {
		acn += '=';
		acn += mv.promote_to;
	}

	// check/mate
	if (mv.check == CHECK)
		acn += '+';
	if (mv.check == MATE)
		acn += '#';

	fprintf(flog, "%s\n", acn.c_str());
	return acn;
}
