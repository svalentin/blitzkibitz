#include "board.h"
#include "pieces.h"
#include "magic.h"

///////////////////////////////////////////////////////////////
// Board class

/// Function that initialises the board.
/// Places pieces at starting location and other initialisation stuff.
void Board::InitChessboard()
{
    player = enPassant = sah = 0;

    memset(bb, 0, sizeof(bb));
    bb[0]   |= INITIAL_POS_PAWN_W;
    bb[7]   |= INITIAL_POS_PAWN_B;
    bb[1]   |= INITIAL_POS_KNIGHT_W;
    bb[8]   |= INITIAL_POS_KNIGHT_B;
    bb[2]   |= INITIAL_POS_BISHOP_W;
    bb[9]   |= INITIAL_POS_BISHOP_B;
    bb[3]   |= INITIAL_POS_ROOK_W;
    bb[10]  |= INITIAL_POS_ROOK_B;
    bb[4]   |= INITIAL_POS_QUEEN_W;
    bb[11]  |= INITIAL_POS_QUEEN_B;
    bb[5]   |= INITIAL_POS_KING_W;
    bb[12]  |= INITIAL_POS_KING_B;
    bb[6]   |= INITIAL_POS_WHITE_PIECES;
    bb[13]  |= INITIAL_POS_BLACK_PIECES;
    enPassant   = 0;
    castling    = 0x0F;
}

/// Function that returns the type (an integer) of the piece on the position pos
/// pos is an index to the bit on the board.
int Board::GetPieceType(const int pos) const
{
    ull pieceBoard = 0;
    SET_BIT(pieceBoard, pos);

    // white pieces
    for (int i=0; i<6; ++i)
        if (pieceBoard & bb[i])
            return i;

    // black pieces
    for (int i=7; i<13; ++i)
        if (pieceBoard & bb[i])
            return i;

    return -1;
}

int Board::GetPieceCount() const
{
    ull aux = bb[WHITE_PIECE] | bb[BLACK_PIECE];
    int con=0;
    for (int pp = LSBi(aux); pp != 64; pp = LSBi(aux)) con++;
    return con;
}

/// Function that prints a human-readable board with line/column marks
void Board::PrintBoard(FILE *fout) const
{
    char ch;
    fprintf(fout, "  abcdefgh\n  --------\n8|");
    for (int i=63, k=7; i>=0; --i)
    {
        int type = GetPieceType(i);
        if (type >= 0) ch = PieceIndexMap[type];
        else ch = '.';
        fprintf(fout, "%c", ch);
        if ((i%8 == 0) && i) fprintf(fout, "\n%d|", k--);
    }
    fprintf(fout, "\n");
    fprintf(fout, "  --------\n  abcdefgh\n");
    if (player) fprintf(fout, "Black to move\n");
    else fprintf(fout, "White to move\n");
}

/// Function that prints a bit board on screen using 1 and 0
void Board::PrintBitBoard(const ull bitboard) const
{   
    ull bit = 1LL<<63;
    for (int i=0; i<8; ++i) {
        printf("  "); // whitespace to align with the ShowPieces board
//        fprintf(fout,"  ");
        for (int j=0; j<8; ++j, bit>>=1)
            printf("%d", (bitboard & bit) ? 1 : 0);
        //  fprintf(fout,"%d", (bitboard & bit) ? 1 : 0);
        printf("\n");
        //    fprintf(fout,"\n");
    }
    printf("\n");
}

/// Apply castling on a board given the position of the rook and king.
/// It does not check for validity!
void Board::applyCastling(pair<int, int> R, pair<int, int> K)
{
    int p1 = player * 7;
    int p2 = player * 56;

#ifdef DEBUG
    Board old = *this;
#endif
    
    // pierde dreptul la rocada pe viitor
    CLEAR_BIT_UCHAR(castling, player*2);
    CLEAR_BIT_UCHAR(castling, player*2+1);


    // rock
    SET_BIT(bb[3 + p1], R.second + p2);
    SET_BIT(bb[6 + p1], R.second + p2);
    CLEAR_BIT(bb[3 + p1], R.first + p2);
    CLEAR_BIT(bb[6 + p1], R.first + p2);
    
    // king
    SET_BIT(bb[5 + p1], K.second + p2);
    SET_BIT(bb[6 + p1], K.second + p2);
    CLEAR_BIT(bb[5 + p1], K.first + p2);
    CLEAR_BIT(bb[6 + p1], K.first + p2);
    
#ifdef DEBUG
    ull aux = bb[5];
    int x = LSBi(aux);
        x = LSBi(aux);
    if (x!=64) {
        // avem 2 regi pe tabla!
        printf("2 regi pe tabla! in functia Board::applyCastling\n");
        PrintBitBoard(bb[5]);
        PrintBitBoard(old.bb[5]);
        old.ShowPieces();
        printf("%d %d + %d %d + %d\n", R.first, R.second, K.first, K.second, player);
        printf("%d %d + %d %d\n", R.first + p2, R.second + p2, K.first + p2, K.second + p2);
        
        exit(0);
    }
#endif
}

/// Given a Move, it applies that move on the board.
/// It does not check for validity! It presumes the move is valid on the board
int Board::MakeMove(const Move mv)
{    
    // presupunand ca a facut o mutare valida, 
    // jucatorul curent iese din sah; daca a fost :P
    if (sah != -1)
        sah = 0;

    // chess    
    if (mv.sah == SAH)
        sah |= SAH;
    
    // fatality
    if (mv.sah == MAT)
        sah |= MAT;
 
    if (mv.flags == ROCADA_MICA) {
        applyCastling(make_pair(0,2), make_pair(3,1));
        return 0;
    }

    if (mv.flags == ROCADA_MARE) {
        applyCastling(make_pair(7,4), make_pair(3,5));
        return 0;
    }
 
    int dest = mv.destination;
    int src  = mv.source;
    int DestPieceType = GetPieceType(dest);
    int SrcPieceType  = GetPieceType(src);
    int DestPieceColor, SrcPieceColor;
    
    // daca regele sau tura se misca pierdem dreptul la rocada
    if (mv.piece == 'K') {
        CLEAR_BIT_UCHAR(castling, player*2);
        CLEAR_BIT_UCHAR(castling, player*2+1);
    }
    
    if (mv.piece == 'R') {
        if (src%8 == 0) CLEAR_BIT_UCHAR(castling, player*2);
        if (src%8 == 7) CLEAR_BIT_UCHAR(castling, player*2+1);
    }
        
    // dreptul de enpass se pierde (indiferent de mutare)
    enPassant = 0;
    // un pion ce se deplaseaza 2 patratele
    // ofera dreptul de enPassant adversarului
    if (mv.piece == 'P' && abs(dest - src) == 16) {
        enPassant = dest + (player ? 8 : -8);
    }
    
    if (SrcPieceType <= 5)
        SrcPieceColor = 6;
    else
        SrcPieceColor = 13;

    if (DestPieceType <= 5)
        DestPieceColor = 6;
    else
        DestPieceColor = 13;
    

    SET_BIT(bb[SrcPieceType], dest);
    CLEAR_BIT(bb[SrcPieceType], src);

    SET_BIT(bb[SrcPieceColor], dest);
    CLEAR_BIT(bb[SrcPieceColor], src);

    //enpass
    if (mv.flags == ENPASS) {
        //se elimina pionul aflat in "fata" destinatiei
        if (player) {
            CLEAR_BIT(bb[0], dest + 8);
            CLEAR_BIT(bb[6], dest + 8);
        }
        else {
            CLEAR_BIT(bb[7], dest - 8);
            CLEAR_BIT(bb[13], dest - 8);
        }
    }
    
    if (DestPieceType != -1) {
        CLEAR_BIT(bb[DestPieceType], dest);
        CLEAR_BIT(bb[DestPieceColor], dest);
        // daca se captureaza o tura
        if (toupper(PieceIndexMap[DestPieceType]) == 'R') {
            if (dest%8 == 0) CLEAR_BIT_UCHAR(castling, (!player)*2);
            if (dest%8 == 7) CLEAR_BIT_UCHAR(castling, (!player)*2+1);
        }
    }
    
    //promote
    if (mv.promote_to) {
        int type = PieceMap[mv.promote_to] + 7*player;
        
        CLEAR_BIT(bb[SrcPieceType], dest);
        SET_BIT(bb[type], dest);
    }

    return 0;
}

/// Returns the bitboard with positions occupied by all pieces.
ull Board::GetOccupancy() const
{
    return bb[6] | bb[13];
}

/// Verifies if given squares are controled by the enemy
bool Board::VerifyChess(const ull pos, const int side) const
{
    ull bb_cpy;
    int piece;
    int piece_pos;
    
    ull occ = GetOccupancy();
    
    // KNIGHT
    piece = 1 + 7*side;
    bb_cpy = bb[piece];
    for (piece_pos = LSBi(bb_cpy); piece_pos != 64; piece_pos = LSBi(bb_cpy)) 
        if (pos & Nmagic(piece_pos))
            return true;

    // BISHOP
    piece = 2 + 7*side;
    bb_cpy = bb[piece];
    for (piece_pos = LSBi(bb_cpy); piece_pos != 64; piece_pos = LSBi(bb_cpy))
        if (pos & Bmagic(piece_pos, occ))
            return true;

    // ROCK
    piece = 3 + 7*side;
    bb_cpy = bb[piece];
    for (piece_pos = LSBi(bb_cpy); piece_pos != 64; piece_pos = LSBi(bb_cpy))
        if (pos & Rmagic(piece_pos, occ))
            return true;
  
    // QUEEN
    piece = 4 + 7*side;
    bb_cpy = bb[piece];
    for (piece_pos = LSBi(bb_cpy); piece_pos != 64; piece_pos = LSBi(bb_cpy))
        if (pos & Qmagic(piece_pos, occ))
            return true;
    
    // KING
    piece = 5 + 7*side;
    bb_cpy = bb[piece];
    for (piece_pos = LSBi(bb_cpy); piece_pos != 64; piece_pos = LSBi(bb_cpy))
        if (pos & Kmagic(piece_pos))
            return true;
    
    // PAWNS
    int pl = side ? -1 : 1;
    piece = 0 + 7*side;
    bb_cpy = bb[piece];
    
    for (piece_pos = LSBi(bb_cpy); piece_pos != 64; piece_pos = LSBi(bb_cpy)) {
        int col = piece_pos%8;
        ull pw = 0;

        // attack right corner
        if (!(side && col == 7) && !(!side && col == 0))
            SET_BIT(pw, piece_pos + 7 * pl);
        
        // attack left corner
        if (!(!side && col == 7) && !(side && col == 0))
            SET_BIT(pw, piece_pos + 9 * pl);
        
        if (pos & pw)
            return true;
    }

    return false;
}

/// Validates castlings
bool Board::validCastling(const int side) const
{
    ull chess_field, occ_field, pawns_att;
    
    if (!side) {
        chess_field = 0xEULL<<(56*player);
        occ_field = 0x6ULL<<(56*player);
        pawns_att = 0x1F00ULL<<(40*player);
    }
    else {
        chess_field = 0x38ULL<<(56*player);
        occ_field = 0x70ULL<<(56*player);
        pawns_att = 0x7C00ULL<<(40*player);
    }
        
    if (occ_field & (bb[WHITE_PIECE] | bb[BLACK_PIECE]))
        return false;
    
    if (pawns_att & (bb[0 + 7*(!player)]))
        return false;
    
    if (VerifyChess(chess_field, !player))
        return false;
    
    return true;
}


int Board::WeGiveCheckOrMate(const Move mv) const
{
    int check = 0;
    Board brdinf;
    
    SaveBoard(brdinf);
    brdinf.MakeMove(mv);
    
    // daca dam sah
    if (brdinf.sah != -1 && brdinf.VerifyChess(brdinf.bb[5 + 7*(!brdinf.player)], brdinf.player)) {
        check = SAH;
        // daca dam mat
        brdinf.player = !brdinf.player;
        brdinf.sah = -1;
        vector<Move> tstm = brdinf.GetMoves();
        if (!tstm.size())
            check = MAT;
    }

    return check;
}

/// Given a piece, its source and valid locations as a bitboard
/// append the moves in Move-class format to a vector of possible moves.
bool Board::appendMoves(vector<Move> &m, ull att, const int piece, const int source) const
{
    Move mv;
    mv.source = source;
    mv.promote_to = mv.sah = mv.flags = 0;
    mv.player = player;
    mv.piece = toupper(PieceIndexMap[piece]);
    
    att ^= (bb[6 + 7*player] & att); // do not move a piece over one of the same color
    for (int dest=LSBi(att); dest!=64; dest = LSBi(att)) {
        mv.promote_to = 0;
        if (mv.sah != -1)
            mv.sah = 0;
        mv.destination = dest;
#ifdef DEBUG_MOVES
        printf("[%c|(%c%d)]", mv.piece, 'h' - dest%8, dest/8 + 1);
#endif
        if (GetPieceType(dest) != -1)
            mv.flags = CAPTURA;
        else
            mv.flags = 0;
                
        if (mv.piece == 'P') {
            if (dest == enPassant && enPassant != 0)
                mv.flags = ENPASS;
            if (dest >= 56*(!player) && dest <= 7 + 56*(!player))
                mv.promote_to = 'Q';
        }

        // daca dam sah sau mat
        if (sah != -1)
            mv.sah = WeGiveCheckOrMate(mv);
        
        // daca ramanem in sah
        Board brdinf;
        SaveBoard(brdinf);
        brdinf.MakeMove(mv);
        
        bool ch = brdinf.VerifyChess(brdinf.bb[5 + 7*brdinf.player], !brdinf.player);
        if ((brdinf.sah && !ch) || !ch) {
            m.push_back(mv);
            if (sah == -1)
                return true;
#ifdef DEBUG_MOVES
            printf("v  ");
#endif
        }
#ifdef DEBUG_MOVES
        if (ch)
            printf("Ramanem in sah!  ");
#endif
    }
    
    return false;
}

/// Returns a vector with all valid Moves on the current board.
/// It takes all the information from the internal state of the Board.
vector<Move> Board::GetMoves() const
{
    vector<Move> M;
    M.reserve(32);  // reserve memory for 32 moves, so we can avoid ~4 reallocations
    ull occ, bb_cpy;
    int piece_pos;
    int piece;
    
    occ = GetOccupancy();
    
    // kNight
    piece = 1 + 7*player;
    bb_cpy = bb[piece];
    for (piece_pos = LSBi(bb_cpy); piece_pos != 64; piece_pos = LSBi(bb_cpy)) {
        if (appendMoves(M, Nmagic(piece_pos), piece, piece_pos))
            return M;
    }
    
    // Bishop
    piece = 2 + 7*player;
    bb_cpy = bb[piece];
    for (piece_pos = LSBi(bb_cpy); piece_pos != 64; piece_pos = LSBi(bb_cpy)) {
        if (appendMoves(M, Bmagic(piece_pos, occ), piece, piece_pos))
            return M;
    }
    
    // Rock
    piece = 3 + 7*player;
    bb_cpy = bb[piece];
    for (piece_pos = LSBi(bb_cpy); piece_pos != 64; piece_pos = LSBi(bb_cpy)) {
        if (appendMoves(M, Rmagic(piece_pos, occ), piece, piece_pos))
            return M;
    }
        
    // Queen
    piece = 4 + 7*player;
    bb_cpy = bb[piece];
    for (piece_pos = LSBi(bb_cpy); piece_pos != 64; piece_pos = LSBi(bb_cpy)) {
        if (appendMoves(M, Qmagic(piece_pos, occ), piece, piece_pos))
            return M;
    }
    
    // King
    piece = 5 + 7*player;
    bb_cpy = bb[piece];
    for (piece_pos = LSBi(bb_cpy); piece_pos != 64; piece_pos = LSBi(bb_cpy)) {
        if (appendMoves(M, Kmagic(piece_pos), piece, piece_pos))
            return M;
    }
    
    Move m;

    // CASTLING KING SIDE
    if (castling & (1 << (player<<1)) && validCastling(0)) {
        m.player = player;
        m.flags = ROCADA_MICA;
        
        // daca dam sah sau mat
        m.sah = 0;
        if (sah != -1)
            m.sah = WeGiveCheckOrMate(m);
        
        M.push_back(m);
        if (sah == -1)
            return M;
#ifdef DEBUG_MOVES
        printf("O-O  ");
#endif
    }
    
    // CASTLING QUEEN SIDE
    if (castling & (2 << (player<<1)) && validCastling(1)) {
        m.player = player;
        m.flags = ROCADA_MARE;
        
        // daca dam sah sau mat
        m.sah = 0;
        if (sah != -1)
            m.sah = WeGiveCheckOrMate(m);
        
        M.push_back(m);
        if (sah == -1)
            return M;
#ifdef DEBUG_MOVES
        printf("O-O-O  ");
#endif
    }
    
    // PAWNS
    ull p_att = 0, pl;
    int poss;
    
    pl = player ? -1 : 1;
    piece = 0 + 7*player;
    bb_cpy = bb[piece];
    

    for (piece_pos = LSBi(bb_cpy); piece_pos != 64; piece_pos = LSBi(bb_cpy)) {
        int col = piece_pos%8;
        p_att = 0;

        // move 1 square ahead
        poss = piece_pos + 8 * pl;
        if (GetPieceType(poss) == -1)
            SET_BIT(p_att, poss);

        // move 2 squares ahead
        poss = piece_pos + 16 * pl;
        if ((poss < 32) ^ player)
            if (GetPieceType(poss) == -1 && GetPieceType(poss - 8*pl) == -1)
                SET_BIT(p_att, poss);

        // attack right corner
        if (!(player && col == 7) && !(!player && col == 0)) {
            poss = piece_pos + 7 * pl;
            int DestPieceType = GetPieceType(poss);
            int DestPieceColor = !(DestPieceType <= 5);
            if (DestPieceType != -1 && DestPieceColor == !player  || enPassant == poss)
                SET_BIT(p_att, poss);
        }
            
        // attack left corner
        if (!(!player && col == 7) && !(player && col == 0)) {
            poss = piece_pos + 9 * pl;
            int DestPieceType = GetPieceType(poss);
            int DestPieceColor = !(DestPieceType <= 5);
            if (DestPieceType != -1 && DestPieceColor == !player  || (enPassant == poss && enPassant != 0))
                SET_BIT(p_att, poss);
        }
        if (appendMoves(M, p_att, piece, piece_pos))
            return M;
    }
    
    sort(M.begin(), M.end());
    
    return M;
}

/// Saves board status
void Board::SaveBoard(Board &brd) const
{
    brd.player = player;
    brd.castling = castling;
    brd.sah = sah;
    brd.enPassant = enPassant;
    for (int i = 0; i < 14; i++)
        brd.bb[i] = bb[i];
}

/// Loads a given board
void Board::LoadBoard(const Board &brd)
{
    player = brd.player;
    castling = brd.castling;
    sah = brd.sah;
    enPassant = brd.enPassant;
    for (int i = 0; i < 14; i++)
        bb[i] = brd.bb[i];
}

/// Operator = overloaded to load a given board
Board& Board::operator=(const Board &brd)
{
    LoadBoard(brd);
    return *this;
}


// end of Board class
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
// bit manipulation functions

/// Returns the index of the Least Semnificative Bit.
/// Returns 64 in case of failure!
int LSB(const ull b)
{
    int ra, rb, rc;
	#ifdef _MSC_VER
		__asm 
		{
			xor eax, eax
			bsf eax, dword ptr b
			jnz end
			bsf eax, dword ptr b+4
			jz fail
			add eax, 32d
			jmp end
			fail: mov eax,64
			end:
		}
	#else
		asm(
			"   bsf     %2, %0"     "\n\t" 
			"   jnz     2f"         "\n\t" 
			"   bsf     %1, %0"     "\n\t" 
			"   jnz     1f"         "\n\t" 
			"   movl    $64, %0"    "\n\t" 
			"   jmp     2f"         "\n\t" 
			"1: addl    $32,%0"     "\n\t" 
			"2:"
			:   "=&q"(ra), "=&q"(rb), "=&q"(rc)
			:   "1"((int) (b >> 32)), "2"((int) b)
			:   "cc"
			);
		return ra;
	#endif
}

/// Returns the index of the Most Semnificative Bit.
/// Returns 64 in case of failure!
int MSB(const ull b)
{
    int ra, rb, rc;
	#ifdef _MSC_VER
		__asm 
		{
			xor eax, eax
			bsr eax, dword ptr b
			jnz end
			bsr eax, dword ptr b+4
			jz fail
			add eax, 32d
			jmp end
			fail: mov eax,64
			end:
		}
	#else
		asm(
			"   bsr     %1, %0"     "\n\t"
			"   jnz     1f"         "\n\t" 
			"   bsr     %2, %0"     "\n\t" 
			"   jnz     2f"         "\n\t" 
			"   movl    $64, %0"    "\n\t" 
			"   jmp     2f"         "\n\t" 
			"1: addl    $32,%0"     "\n\t" 
			"2:"
			:   "=&q"(ra), "=&q"(rb), "=&q"(rc)
			:   "1"((int) (b >> 32)), "2"((int) b)
			:   "cc"
			);
		return ra;
	#endif
}
