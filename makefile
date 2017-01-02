# BlitzKibitz
# makefile

@TERM = $(shell echo %TERM%)
# on Windows this is empty

CC = g++
CFLAGS = -s -O3 -Wall -ffast-math -fomit-frame-pointer -msse -mfpmath=sse -static -static-libgcc -static-libstdc++ -Wno-unused-result
#CFLAGS = -g -msse -DNORAND

# CFLAGS explanation
#	-s
#		Option removes all symbol table and relocation information from the executable
#	-fomit-fame-pointer
#		Makes programs faster at runtime, but makes debugging impossible.
#		x86 processors need the frame pointer for debugging.
#	-mfpmath=sse
#		Use scalar floating point instructions present in the SSE instruction set.
#		This instruction set is supported by Pentium3 and newer chips,
#		in the AMD line by Athlon-4, Athlon-xp and Athlon-mp chips.
#		Later version, present only in Pentium4 and the future AMD x86-64 chips
#		supports double precision arithmetics too.
#		The resulting code should be considerably faster in the majority of cases
#		and avoid the numerical instability problems of 387 code,
#		but may break some existing code that expects temporaries to be 80bit.
#	-msse
#		Needed to activate sse instructions used by `-mfpmath=sse`
#	-ffast-math
#		Enable unsafe math optimizations. We don't rely on ISO specifications.
#	-static-libgcc
#		Mingw dynamically links libgcc, we need this static so it will run everywhere


all: main.o board.o pieces.o magic.o opendb.o engine.o acn.o xboard.o rand.o evaluation.o rand.o hash.o
	$(CC) $(CFLAGS) -o BlitzKibitz main.o magic.o board.o pieces.o opendb.o engine.o acn.o xboard.o rand.o hash.o evaluation.o

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

run: BlitzKibitz openings.bk
	./BlitzKibitz

clean:
ifndef TERM
	del BlitzKibitz.exe
	del *.o
else
	rm -f BlitzKibitz BlitzKibitz.exe
	rm -f *.o
endif
