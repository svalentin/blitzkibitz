#include "pieces.h"
#include "board.h"
#include "magic.h"
#include "opendb.h" 

/// Initialise the opening moves database
void Openings::InitOpenings(const char *file_name)
{
    // printf("Loading openings...\n");

    FILE *f = fopen(file_name, "r"); 
    if (f == NULL) {
        // printf("File %s not found, no opening database!\n", file_name);
        return;
    }

    op_desc *nou, *p;
    
    DB = new op_desc;
    DB->next = NULL;
    p = DB;
    fgets(DB->FEN, 100, f);
    fgets(DB->move, 50, f);
    
    int i = 0;
    while (DB->move[i] != '{') i++;        
    
    memset(&DB->move[i], 0, (int)strlen(DB->move) - i);
    DB->FEN[(int)strlen(DB->FEN) - 1] = 0;
    OpenDB[DB->FEN] = DB->move;
    
    while (1) {
        nou = new op_desc;
        nou->next = NULL;

        if (!(fgets(nou->FEN, 100, f))) break;
        if (strstr(nou->FEN, "#END#"))  break;
        if (!(fgets(nou->move, 50, f))) break;
                
        i = 0;
        while (nou->move[i] != '{') i++;
        
        memset(&nou->move[i], 0, (int)strlen(nou->move) - i);
        nou->FEN[(int)strlen(nou->FEN) - 1] = 0;
        OpenDB[nou->FEN] = nou->move;
        
        p->next = nou;
        p = nou;
    }

    fclose(f);

    // printf("Done\n");
}

/// Given a Board, get the Move from the opening database.
Move Openings::GetMoveFromDB(Board &board)
{
    Move mv;
 
    mv.flags = mv.piece = mv.promote_to = mv.sah = 0;
    mv.source = mv.destination = 64;
    mv.player = board.player;
    
    char *mvch = (char *)calloc(50, sizeof(char ));
    
    char *FEN = toFEN(board);
    
    if (OpenDB[FEN] != NULL) {
        strcpy(mvch, OpenDB[FEN]);
    }
    
    if (!mvch[0]) {
        mv.flags = ERROR; // NO_MOVE
        return mv;
    }
    
    mv.source = COORDS_TO_INDEX(mvch[1] - '1', 'h' - mvch[0]);
    mv.destination = COORDS_TO_INDEX(mvch[3] - '1', 'h' - mvch[2]);
    
    // promovare
    if (mvch[5] != 0) {
        mv.flags |= PROMOVARE;
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
        mv.flags |= ROCADA_MICA;
    }
    else if (piecetype == 5 && mv.source == 3 && mv.destination == 1) {
        mv.flags |= ROCADA_MICA;
    }
    else if (piecetype == 5 && mv.source == 59 && mv.destination == 61) {
        mv.flags |= ROCADA_MARE;
    }
    else if (piecetype == 5 && mv.source == 3 && mv.destination == 5) {
        mv.flags |= ROCADA_MARE;
    }
    
    piecetype = board.GetPieceType(mv.destination);
    if (piecetype != -1)
        mv.flags |= CAPTURA;

    // daca dam sah sau mat
    mv.sah = board.WeGiveCheckOrMate(mv);
    
    return mv;
}



/// Conversion from an internal representation of a Board to a FEN string
char *toFEN(Board &board)
{
    char *fen = (char *) calloc(100, sizeof(char));
    
    for (int i=7, t=0; i >= 0; i--) {
        int g = 0;
        for (int j = 7; j >= 0; j--)
        {
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

    if (!board.enPassant) strcat(fen, " -");
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

/// Destructor that frees up the memory allocated for the openings
Openings::~Openings()
{
    op_desc *p, *fr;
    
    fr = p = DB;
    while (p != NULL) {
        fr = p;
        p = p->next;
        delete fr;
    }
}
