#ifndef ACN_H_
#define ACN_H_

#include <cstdio>
#include <string>

#include "board.h"
#include "magic.h"
#include "pieces.h"

Move DecodeACN(const char *mutare, Board &board);
string EncodeACN(Move &mv, Board &board);
#endif
