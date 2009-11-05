#ifndef ACN_H_
#define ACN_H_

#include "board.h"
#include "magic.h"
#include "pieces.h"

#include <cstdio>
#include <string>

Move DecodeACN(const char *sMove, Board &board);
string EncodeACN(Move &mv, Board &board);

#endif
