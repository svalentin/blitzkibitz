#ifndef ACN_H_
#define ACN_H_

#include <cstdio>
#include <string>

Move DecodeACN(const char *sMove, Board &board);
string EncodeACN(Move &mv, Board &board);

#endif
