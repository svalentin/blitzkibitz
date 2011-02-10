# BlitzKibitz
# makefile

@OS = $(shell echo %OS%)
# on Windows this is 'Windows_NT', on Unix it's empty

CC = g++
CFLAGS = -s -O3 -ffast-math -fomit-frame-pointer -msse -mfpmath=sse -static-libgcc -Wno-deprecated -Wno-unused-result
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


all: main.o bitboards.o board.o evaluate.o hash.o magic.o moves.o random.o search.o util.o xboard.o
	$(CC) $(CFLAGS) -o Amaterasu main.o bitboards.o board.o evaluate.o hash.o magic.o moves.o random.o search.o util.o xboard.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp
	
bitboards.o: bitboards.cpp bitboards.h
	$(CC) $(CFLAGS) -c bitboards.cpp
	
board.o: board.cpp board.h
	$(CC) $(CFLAGS) -c board.cpp

evaluate.o: evaluate.cpp evaluate.h
	$(CC) $(CFLAGS) -c evaluate.cpp

hash.o: hash.cpp hash.h
	$(CC) $(CFLAGS) -c hash.cpp

magic.o: magic.cpp magic.h
	$(CC) $(CFLAGS) -c magic.cpp

moves.o: moves.cpp moves.h
	$(CC) $(CFLAGS) -c moves.cpp

random.o: random.cpp random.h
	$(CC) $(CFLAGS) -c random.cpp

search.o: search.cpp search.h
	$(CC) $(CFLAGS) -c search.cpp

util.o: util.cpp util.h
	$(CC) $(CFLAGS) -c util.cpp

xboard.o: xboard.cpp xboard.h
	$(CC) $(CFLAGS) -c xboard.cpp

	
run: Amaterasu
	./Amaterasu

clean:
ifeq ($(OS),Windows_NT)
	del Amaterasu.exe
	del *.o
else
	rm Amaterasu
	rm *.o
endif