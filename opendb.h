#ifndef OPENDB_H_
#define OPENDB_H_

#include "pieces.h"
#include "board.h"
#include "magic.h"

#include<unordered_map>
#include<cstring>

using namespace std;

class Move;
class Board;

class Openings
{
	public:
		unordered_map<const string, const string, hash<string> > OpenDB;
		void InitOpenings(const char*);
		Move GetMoveFromDB(Board &b);
}; 

char *toFEN(Board &b);

#endif
