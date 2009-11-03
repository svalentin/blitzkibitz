# BlitzKibitz
# makefile

all: main.o board.o pieces.o magic.o opendb.o engine.o acn.o xboard.o rand.o evaluation.o rand.o hash.o
	g++ main.o magic.o board.o pieces.o opendb.o engine.o acn.o xboard.o rand.o hash.o evaluation.o -s -O3 -o BlitzKibitz 

main.o: main.cpp
	g++ -c -O3 -ffast-math -fomit-frame-pointer main.cpp
	
magic.o: magic.cpp magic.h
	g++ -c -O3 -ffast-math -fomit-frame-pointer magic.cpp
	
hash.o: hash.cpp hash.h
	g++ -c -O3 -ffast-math -fomit-frame-pointer hash.cpp

evaluation.o: evaluation.cpp evaluation.h
	g++ -c -O3 -ffast-math -fomit-frame-pointer evaluation.cpp

rand.o: rand.cpp rand.h
	g++ -c -O3 -ffast-math -fomit-frame-pointer rand.cpp

acn.o: acn.cpp acn.h
	g++ -c -O3 -ffast-math -fomit-frame-pointer acn.cpp

board.o: board.cpp board.h
	g++ -c -O3 -ffast-math -fomit-frame-pointer board.cpp
	
xboard.o: xboard.cpp xboard.h
	g++ -c -O3 -ffast-math -fomit-frame-pointer xboard.cpp
	
opendb.o: opendb.cpp opendb.h
	g++ -c -O3 -ffast-math -fomit-frame-pointer opendb.cpp
	
pieces.o: pieces.cpp pieces.h
	g++ -c -O3 -ffast-math -fomit-frame-pointer pieces.cpp
	
engine.o: engine.cpp engine.h
	g++ -c -O3 -ffast-math -fomit-frame-pointer engine.cpp

run: sah openings.bk
	./sah

clean: sah
	rm sah
