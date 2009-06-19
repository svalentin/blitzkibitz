# BlitzKibitz
# makefile

all: openings.bk main.cpp board.h board.cpp pieces.h pieces.cpp magic.h magic.cpp opendb.h opendb.cpp engine.cpp acn.cpp xboard.cpp
	g++ -s -O3 -ffast-math -fomit-frame-pointer main.cpp magic.cpp board.cpp pieces.cpp opendb.cpp engine.cpp acn.cpp xboard.cpp rand.c hash.cpp -o BlitzKibitz
    
openings.bk:
	tar xjvf openings.bk.tar.bz2    

run: sah openings.bk
	./sah

clean: sah
	rm sah
