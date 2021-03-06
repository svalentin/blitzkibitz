#ifndef XBOARD_H_
#define XBOARD_H_

#include "board.h"
#include "magic.h"
#include "pieces.h"
#include "opendb.h"
#include "engine.h"
#include "acn.h"

void XPlay(int normal_max_depth, Board &board);

#endif
