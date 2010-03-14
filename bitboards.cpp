#include<stdio.h>
#include"constants.h"
#include"bitboards.h"

int LookupBit[64] = 
{

	63, 0, 58, 1, 59, 47, 53, 2,
	60, 39, 48, 27, 54, 33, 42, 3,
	61, 51, 37, 40, 49, 18, 28, 20,
	55, 30, 34, 11, 43, 14, 22, 4,
	62, 57, 46, 52, 38, 26, 32, 41,
	50, 36, 17, 19, 29, 10, 13, 21,
	56, 45, 25, 31, 35, 16, 9, 12,
	44, 24, 15, 8, 23, 7, 6, 5
};

/* Calculeaza indicele primului bit=1 dintr-o tabla */
int LSBi(U64 n)
{
#ifdef USE_ASM
	#ifdef _MSC_VER
		__asm 
		{
			xor eax, eax
			bsf eax, dword ptr n
			jnz end
			bsf eax, dword ptr n+4
			jz fail
			add eax, 32d
			jmp end
			fail: mov eax,-1
			end:
		}
	#else
		int i;
	    asm("bsf %%eax, %0 \n\t"
			"jnz end \n\t"
			"bsf %%edx, %0 \n\t"
			"jz fail \n\t"
			"addl $32, %0 \n\t"
			"jmp end \n\t"
			"fail: movl $-1, %0\n\t"
			"end:"
			:"=r"(i)
	        :"r"(n)
	       );
	    return i;
	#endif
#else
	if(n)
	{
		return LookupBit[((n&(-n)) * 0x7EDD5E59A4E28C2ULL) >> 58];
	}
	return -1;
#endif
}
/* Calculeaza numarul de biti=1 dintr-o tabla */
int Count1s(U64 n)
{
	int i=0;
	U64 bit;
	while(n)
	{
		bit=n&(-n);
		n^=bit;
		i++;
	}
	return i;
}
