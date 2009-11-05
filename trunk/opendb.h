#ifndef OPENDB_H_
#define OPENDB_H_

#include "pieces.h"
#include "board.h"
#include "magic.h"

#include<ext/hash_map>
#include<cstring>

using namespace __gnu_cxx;
using namespace std;

class Move;
class Board;
		
struct op_eqstr
{
	bool operator()(const char* s1, const char* s2) const
	{
		return strcmp(s1, s2) == 0;
	}
};

struct op_desc
{
	char FEN[100];
	char move[16];
	struct op_desc *next;
};


class Openings
{
	public:
		~Openings();
		hash_map<const char*, char*, hash<const char*>, op_eqstr> OpenDB;
		void InitOpenings(const char*);
		Move GetMoveFromDB(Board &b);
	private:
		op_desc *DB;

}; 

char *toFEN(Board &b);

#endif
