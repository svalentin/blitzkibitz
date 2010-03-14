#include<stdio.h>
#include"random.h"

#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL /* Most significant 33 bits */
#define LM 0x7FFFFFFFULL /* Least significant 31 bits */

/* The array for the state vector */
static unsigned long long mt[NN]; 
/* mti==NN+1 means mt[NN] is not initialized */
static int mti=NN+1; 

/* initializes mt[NN] with a seed */
void init_genrand64(unsigned long long seed)
{
    mt[0] = seed;
    for(mti=1; mti<NN; mti++) 
        mt[mti] =  (6364136223846793005ULL * (mt[mti-1] ^ (mt[mti-1] >> 62)) + mti);
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
void InitByArray64(unsigned long long init_key[],unsigned long long key_length)
{
    unsigned long long i, j, k;
    init_genrand64(19650218ULL);
    i=1; j=0;
    k = (NN>key_length ? NN : key_length);
    for(; k; k--)
	{
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 62)) * 3935559000370003845ULL)) + init_key[j] + j; /* non linear */
        i++; j++;
		if(i>=NN)
		{
			mt[0] = mt[NN-1];
			i=1;
		}
        if(j>=key_length) j=0;
    }
    for(k=NN-1; k; k--)
	{
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 62)) * 2862933555777941757ULL)) - i; /* non linear */
        i++;
        if(i>=NN)
		{
			mt[0] = mt[NN-1];
			i=1;
		}
    }
    mt[0] = 1ULL << 63; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long GetRandInt64(void)
{
    int i;
    unsigned long long x;
    static unsigned long long mag01[2]={0ULL, MATRIX_A};

    if(mti >= NN) /* generate NN words at one time */
	{
        /* if init_genrand64() has not been called, */
        /* a default initial seed is used     */
        if(mti == NN+1) init_genrand64(5489ULL); 

        for(i=0;i<NN-MM;i++)
		{
            x = (mt[i]&UM)|(mt[i+1]&LM);
            mt[i] = mt[i+MM] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
        }
        for(;i<NN-1;i++)
		{
            x = (mt[i]&UM)|(mt[i+1]&LM);
            mt[i] = mt[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
        }
        x = (mt[NN-1]&UM)|(mt[0]&LM);
        mt[NN-1] = mt[MM-1] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
        mti = 0;
    }
    x = mt[mti++];
    x ^= (x >> 29) & 0x5555555555555555ULL;
    x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
    x ^= (x << 37) & 0xFFF7EEE000000000ULL;
    x ^= (x >> 43);
    return x;
}

/* generates a random number on [0, 2^63-1]-interval */
long long GetRandInt63(void)
{
    return (long long)(GetRandInt64() >> 1);
}

/* generates a random number on [0,1]-real-interval */
double GetRandReal1(void)
{
    return (GetRandInt64() >> 11) * (1.0/9007199254740991.0);
}

/* generates a random number on [0,1)-real-interval */
double GenRandReal2(void)
{
    return (GetRandInt64() >> 11) * (1.0/9007199254740992.0);
}

/* generates a random number on (0,1)-real-interval */
double GetRandReal3(void)
{
    return ((GetRandInt64() >> 12) + 0.5) * (1.0/4503599627370496.0);
}

/*
	stub procedure to initialize the PRNG with an array of length 4 of predefined "good" values...in case you don't have
	any bright ideas 
*/
void InitRandGenByArray(void)
{
	unsigned long long init[4]={0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL}, length=4;
    InitByArray64(init, length);
}
