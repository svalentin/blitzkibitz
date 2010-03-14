#ifndef _BITBOARDS_H_
#define _BITBOARDS_H_

#ifndef __64_BIT_INTEGER_DEFINED__
	#define __64_BIT_INTEGER_DEFINED__
	#if defined(_MSC_VER) && _MSC_VER<1300
		typedef unsigned __int64 U64;
	#else
		typedef unsigned long long  U64;
	#endif //defined(_MSC_VER) && _MSC_VER<1300
#endif //__64_BIT_INTEGER_DEFINED__

//#define USE_ASM

#define EMPTY_BOARD ((U64)0)

#define BIT_IS_SET(bbBoard,iIndex)	(((U64)(bbBoard))&(((U64)1)<<iIndex))
#define SET_BIT(bbBoard,iIndex)		((bbBoard)|=((U64)1<<(iIndex)))
#define SET_BITS(bbBoard,bbMask)	((bbBoard)|=((U64)bbMask))

#define CLEAR_BIT(bbBoard,iIndex)	((bbBoard)&=(~((U64)1<<((U64)iIndex))))
#define CLEAR_BITS(bbBoard,bbMask)	((bbBoard)&=(~(bbMask)))

#define GET_BIT(bbBoard,iIndex)		(((U64)bbBoard)&((U64)1<<((U64)(iIndex))))
#define INVERT_BIT(bbBoard,iIndex)	((bbBoard)^=((U64)1<<((U64)(iIndex))))

#define PLACE_PIECE(iBoard,iIndex)	(((U64)iBoard)|((U64)1<<((U64)iIndex)))
#define REMOVE_PIECE(iBoard,iIndex)	(((U64)iBoard)&(~((U64)1<<((U64)iIndex))))

#define ROW(iSq) ((iSq)>>3)
#define COL(iSq) ((iSq)%8)
#define COORDS_2_INDEX(iR,iC)		(((iR)<<3)+iC)

#define FOR_BIT(bbBoard,iIndex) for(iIndex=LSBi(bbBoard);iIndex!=-1;CLEAR_BIT(bbBoard,iIndex),iIndex=LSBi(bbBoard))

extern int LookupBit[64];

int LSBi(U64 n);
int Count1s(U64 n);

#endif
