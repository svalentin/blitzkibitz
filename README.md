# BlitzKibitz
BlitzKibitz - chess engine

BlitzKibitz is a C/C++ chess engine originally started as a university project.

BlitzKibitz doesn't have it's own interface but it is compatible with XBoard protocol.
Implementation uses an Alpha-Beta pruning search-tree algorithm with transposition tables and opening tables built over magic bitboard data structures.

Currently there is no time management. The only limiting factor is the maximum depth (or plise) the engine searches.
For 15 min games it goes to about 6-7 plies, while for 5 min games it's recommended to use 5-6 plies.
You can specify the maximum depth it should search with the "--depth N" argument.
