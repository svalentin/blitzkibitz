#include "acn.h"

FILE *flog = fopen("BlitzKibitz-ACN.log", "w");

Move DecodeACN(const char *mutare, Board &board)
{
    setvbuf(flog, 0, _IONBF, 0);
    fprintf(flog, "\nDecode %s\n", mutare);
    
    Move m;
    m.flags = m.piece = m.promote_to = m.sah = 0;
    m.source = m.destination = 64;
    m.player = board.player;
 
    int i;
    int xi=64, yi=64, xf=64, yf=64;

    i = strlen(mutare)-1;

    if (mutare[i] == '+') {                 // sah
        m.sah |= SAH;
        --i;
    }
    if (mutare[i] == '#') {                 // sah mat
        m.sah |= MAT;
        --i;
    }

    if (strcmp(mutare, "O-O-O") == 0) {     // rocada mare
        m.flags |= ROCADA_MARE;
        return m;
    }
    if (strcmp(mutare, "O-O") == 0) {       // rocada mica
        m.flags |= ROCADA_MICA;
        return m;
    }

    // daca mutarea promoveaza
    char promovare = 0;
    if (IsPiece(mutare[i])) {
        promovare = mutare[i];
        m.flags |= PROMOVARE;
        --i;
        if (mutare[i] == '=') --i;
    }

    // locatie destinatie

    // linia si coloana
    if (mutare[i]<'1' || mutare[i]>'8') {
        fprintf(flog, "Eroare format ACN!(1)\n");
        m.flags = ERROR;
        return m;
    }
    else
        xf = mutare[i--]-'1';
    
    if (mutare[i]<'a' || mutare[i]>'h') {
        fprintf(flog, "Eroare format ACN!(2)\n");
        m.flags = ERROR;
        return m;
    }
    else
        yf = 'h'-mutare[i--];

    // Check capture
    if (mutare[i] == 'x') {
        m.flags |= CAPTURA;
        --i;
    }

    // next:
    // 1. nimic => pion
    // 2. nume piesa
    // 3. linie sau coloana => indica poz initiala
    
    // NOTICE: needs more testing in all possible combinations!
    //         try to vary piece type, line, column and capture
    
    if (i<0) {
        // 1. nimic => pion
        m.piece = 'P';    // pion
    }
    else {
        if (IsPiece(mutare[i])) {
            // 2. nume de piesa
            m.piece = mutare[i];
        }
        else {
            // 3. linie si/sau coloana
            if (mutare[i]>='a' && mutare[i]<='h') {
                yi = 'h'-mutare[i--];
            }
            else {
                if (mutare[i]>='0' && mutare[i]<='8') {
                    xi = mutare[i--]-'1';
                    
                    // avem si linie?
                    if (mutare[i]>='a' && mutare[i]<='h') {
                        yi = 'h'-mutare[i--];
                    }
                }
                else {
                    fprintf(flog, "Eroare format ACN!(3)\n");
                    m.flags = ERROR;
                    return m;
                }
            }
            
            // next: nume piesa sau nimic
            if (i<0)
                m.piece = 'P';
            else if (IsPiece(mutare[i]))
                m.piece = mutare[i];
            
            if (m.piece == 0) {
                fprintf(flog, "Eroare format ACN!(4)\n");
                m.flags = ERROR;
                return m;
            }
        }
    }

    m.source        = COORDS_TO_INDEX(xi, yi);
    m.destination   = COORDS_TO_INDEX(xf, yf);
    if (promovare)
        m.promote_to = promovare;
    
    // check "en passant" flag (pawn piece + capture)
    if (m.piece == 'P' && m.flags == CAPTURA)
        if (board.GetPieceType(m.destination) == -1)
            m.flags = ENPASS;
    
    if (xi==64 || yi==64) {
        if (!m.FindCoordinates(board)) {
            fprintf(flog, "Eroare cautare piesa!\n");
            fprintf(flog, "Am cautat o piesa care ajunge pe pozitia %d %d\n", xf, yf);
            fprintf(flog, "Ce stiu despre pozitia sursei: %d %d\n", xi, yi);
            fprintf(flog, "Captura: %d\n", (m.flags & CAPTURA)!=0);
            fprintf(flog, "Piesa: %c\n", m.piece);
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
    acn.reserve(16);    // preallocate 16 bytes to prevent reallocation

    if (mv.flags == ROCADA_MICA) {
        acn = "O-O";
        if (mv.sah == SAH)
            acn += "+";
        if (mv.sah == MAT)
            acn += "#";
        fprintf(flog, "\n%s\n", acn.c_str());
        return acn;
    }
    
    if (mv.flags == ROCADA_MARE) {
        acn = "O-O-O";
        if (mv.sah == SAH)
            acn += "+";
        if (mv.sah == MAT)
            acn += "#";
        fprintf(flog, "\n%s\n", acn.c_str());
        return acn;
    }
    
   
    int lin = 64, col = 64;
    Move mt;
    mt.promote_to = mt.sah = 0; 
    
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
    mt.sah = mv.sah;
    mt.promote_to = mv.promote_to;
    
    if ((toupper(mv.piece) == 'P' && (mv.flags & ENPASS || mv.flags & CAPTURA)) || !mt.FindCoordinates(board)) {
        // dezamb de coloana
        mt.source = COORDS_TO_INDEX(64, col);
        if (mt.FindCoordinates(board)) {
            acn += ('h' - col);
        }
        else {
            // dezamb de coloana
            mt.source = COORDS_TO_INDEX(lin, 64);
            if (mt.FindCoordinates(board)) {
                acn += (char)('1'+lin);
            }
            else {
                // dezamb de linie + coloana
                mt.source = COORDS_TO_INDEX(lin, col);
                acn += (char)('h' - col);
                acn += (char)('1' + lin);
            }
        }
    }

    // capture
    if (mv.flags & CAPTURA || mv.flags & ENPASS)
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
    
    // sah/mat
    if (mv.sah == SAH)
        acn += '+';
    if (mv.sah == MAT)
        acn += '#';
        
    fprintf(flog, "%s\n", acn.c_str());
    return acn;
}
