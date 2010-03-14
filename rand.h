#ifndef RAND_H_
#define RAND_H_

#include <cstdio>
#include <cstddef>

// a ub4 is an unsigned 4-byte quantity
typedef unsigned long int ub4;
typedef unsigned long long ull;

extern ub4 randrsl[256];

void randinit(int);
void isaac();

ull GetRand64();

#endif
