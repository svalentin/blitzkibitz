#ifndef RAND_H_
#define RAND_H_

typedef unsigned long int ub4;

extern ub4 randrsl[256];

void randinit(int);
void isaac();

#endif
