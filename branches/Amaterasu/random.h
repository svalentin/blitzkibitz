#ifndef _RANDOM_H_
#define _RANDOM_H_

/* initializes mt[NN] with a seed */
void init_genrand64(unsigned long long seed);

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
void InitByArray64(unsigned long long init_key[], unsigned long long key_length);

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long GetRandInt64(void);


/* generates a random number on [0, 2^63-1]-interval */
long long GetRandInt63(void);

/* generates a random number on [0,1]-real-interval */
double GetRandReal1(void);

/* generates a random number on [0,1)-real-interval */
double GenRandReal2(void);

/* generates a random number on (0,1)-real-interval */
double GetRandReal3(void);

void InitRandGenByArray(void);

#endif
