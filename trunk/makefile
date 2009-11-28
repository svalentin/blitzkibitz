# BlitzKibitz
# makefile

MODEL = s
CC = g++

# 	`-s` 				option removes all symbol table and relocation information from the executable

# 	`-fomit-fame-pointer` 	makes programs faster at runtime, but makes debugging impossible. -O turns on -fomit-frame-pointer on machines 
#  					where doing so does not interfere with debugging. x86 processors need the frame pointer for debugging, so -fomit-frame-pointer is not turned on by default.
CFLAGS = -O3 -ffast-math -fomit-frame-pointer -s -msse

all: main.o board.o pieces.o magic.o opendb.o engine.o acn.o xboard.o rand.o evaluation.o rand.o hash.o
	$(CC) $(CFLAGS) main.o magic.o board.o pieces.o opendb.o engine.o acn.o xboard.o rand.o hash.o evaluation.o -s -o BlitzKibitz 

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp
	
magic.o: magic.cpp magic.h
	$(CC) $(CFLAGS) -c magic.cpp
	
hash.o: hash.cpp hash.h
	$(CC) $(CFLAGS) -c hash.cpp

evaluation.o: evaluation.cpp evaluation.h
	$(CC) $(CFLAGS) -c evaluation.cpp

rand.o: rand.cpp rand.h
	$(CC) $(CFLAGS) -c rand.cpp

acn.o: acn.cpp acn.h
	$(CC) $(CFLAGS) -c acn.cpp

board.o: board.cpp board.h
	$(CC) $(CFLAGS) -c board.cpp
	
xboard.o: xboard.cpp xboard.h
	$(CC) $(CFLAGS) -c xboard.cpp
	
opendb.o: opendb.cpp opendb.h
	$(CC) $(CFLAGS) -c opendb.cpp
	
pieces.o: pieces.cpp pieces.h
	$(CC) $(CFLAGS) -c pieces.cpp
	
engine.o: engine.cpp engine.h
	$(CC) $(CFLAGS) -c engine.cpp

run: sah openings.bk
	./sah

clean: sah
	rm sah
