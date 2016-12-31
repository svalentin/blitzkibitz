#include "pieces.h"

map<char, int> PieceMap;
map<int, char> PieceIndexMap;

/// This function uses the existing knowledge of a pieces type, destination, and
/// maybe part of the source and tries to exactly reconstruct the source
/// considering it is a valid move.
/// If the source is incomplete, it presumes the column and/or line to be 64.
/// The equivalent of INDEX_TO_COORDS will return <72, 0> if we don't know
/// anything about the source.
/// When checking for failure please use index >= 64 !
bool Move::FindCoordinates(Board &board)
{
	if (source < 64) return true;  // we already know the source

	ull occ = board.GetOccupancy();
	int type = PieceMap[piece] + 7*player;
	ull attack = 0;

	if (piece == 'P') {

		// move one square
		if (!(flags & CAPTURE)) {
			if (player) {
				SET_BIT(attack, destination+8);
			}
			else {
				SET_BIT(attack, destination-8);
			}

			// move two squares
			if (player) {
				if (!IS_SET_BIT(board.bb[6] | board.bb[13], destination+8))
				if (31 < destination && destination < 40)
					SET_BIT(attack, destination+16);
			}
			else {
				if (!IS_SET_BIT(board.bb[6] | board.bb[13], destination-8))
				if (23 < destination && destination < 32)
					SET_BIT(attack, destination-16);
			}
		}

		// capture
		if (flags & CAPTURE || flags & ENPASS) {
			if (player) {
				if (destination+7 < 64) SET_BIT(attack, destination+7);
				if (destination+9 < 64) SET_BIT(attack, destination+9);
			}
			else {
				if (destination-7 >= 0) SET_BIT(attack, destination-7);
				if (destination-9 >= 0) SET_BIT(attack, destination-9);
			}
		}

	}
	else if (piece == 'N') {
		attack = Nmagic(destination);
	}
	else if (piece == 'B') {
		attack = Bmagic(destination, occ);
	}
	else if (piece == 'R') {
		attack = Rmagic(destination, occ);
	}
	else if (piece == 'Q') {
		attack = Qmagic(destination, occ);
	}
	else if (piece == 'K') {
		attack = Kmagic(destination);
	}

	attack &= board.bb[type];

	// NOTICE: the conditions work, no need to recheck :)
	if (source/8 < 64) {
		// we know the line
		attack &= lbb[(source-64)/8];
	}
	if (source/8 < 8 || (64 <= source/8 && source/8 < 72)) {
		// we know the column
		attack &= cbb[source%8];
	}

	int valid = 0;
	ull osrc;
	while (attack) {
		source = LSBi(attack);
		Board brd = board;
		brd.MakeMove(*this);
		if (!brd.VerifyChess(brd.bb[5 + 7*brd.player], !brd.player)) {
			valid++;
			osrc = source;
		}
	}
	if (valid != 1)
		return false;
	else
		source = osrc;

	return true;
}

/// Function that initialises pieces information.
/// Initialisation of a piece's character representation hasing takes place here
void initPieces()
{
	PieceMap['P'] = PAWN_W;
	PieceMap['N'] = KNIGHT_W;
	PieceMap['B'] = BISHOP_W;
	PieceMap['R'] = ROOK_W;
	PieceMap['Q'] = QUEEN_W;
	PieceMap['K'] = KING_W;

	PieceIndexMap[PAWN_W]	= 'P';
	PieceIndexMap[KNIGHT_W]	= 'N';
	PieceIndexMap[BISHOP_W]	= 'B';
	PieceIndexMap[ROOK_W]	= 'R';
	PieceIndexMap[QUEEN_W]	= 'Q';
	PieceIndexMap[KING_W]	= 'K';
	PieceIndexMap[PAWN_B]	= 'p';
	PieceIndexMap[KNIGHT_B]	= 'n';
	PieceIndexMap[BISHOP_B]	= 'b';
	PieceIndexMap[ROOK_B]	= 'r';
	PieceIndexMap[QUEEN_B]	= 'q';
	PieceIndexMap[KING_B]	= 'k';
}
