#ifndef _MAGIC_H_
#define _MAGIC_H_

#define USE_FORCE_INLINE
#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef USE_FORCE_INLINE
	#ifdef _MSC_VER
		#define _FORCE_INLINE_ __forceinline
	#endif
	#ifdef __GNUC__
		#define _FORCE_INLINE_ __inline__
	#endif
	#ifndef _FORCE_INLINE_
		#define _FORCE_INLINE_ inline
	#endif
#endif

#define KSC_WHITE_MASK			((U64)0x06)
#define KSC_BLACK_MASK			(KSC_WHITE_MASK<<56)
#define QSC_WHITE_MASK			((U64)0x70)
#define QSC_BLACK_MASK			(QSC_WHITE_MASK<<56)

/* the shit that made me work like a fucking slave...MAGIC baby :> */
//#define MINIMUM_MAGIC
#define HASH_PERFECT_MAGIC unsigned short

extern U64 FileMasks[8]; /* Numbers representing the vertical files of the chess board */

extern U64 IsolaniMasks[8]; /* Numbers representing bit masks to locate isolani masks */

extern U64 MagicMoves_PawnEnPas_DB[2][64];
extern U64 MagicMoves_Pawns_DB[2][64];
extern U64 MagicMoves_Knight_DB[64];
extern U64 MagicMoves_King_DB[64];

extern const U64 MagicNumbers_Rook[64];
extern const U64 MagicMask_Rook[64];
extern const U64 MagicNumbers_Bishop[64];
extern const U64 MagicMask_Bishop[64];

extern const U64 CastleMasks[2][2];


#define GET_FILE_MASK(file) (FileMasks[file])

#ifndef MINIMUM_MAGIC//shift-uri minime pentru conversie
   #define SHIFT_MINIMAL_BISHOP		55
   #define SHIFT_MINIMAL_ROOK		52
#endif

#ifndef HASH_PERFECT_MAGIC
	#ifdef MINIMUM_MAGIC

	#ifndef USE_FORCE_INLINE
		#define Pmagic(square,color) MagicMoves_Pawns_DB[color][square]
		#define PAttackMagic(square,color) (MagicMoves_Pawns_DB[color][square]? MagicMoves_Pawns_DB[color][square]^(((U64)1)<<(square+(color?-8:8))):MagicMoves_Pawns_DB[color][square])
		#define PNormalMagic(square,color) (MagicMoves_Pawns_DB[color][square]? MagicMoves_Pawns_DB[color][square]&(((U64)1)<<(square+(color?-8:8))):MagicMoves_Pawns_DB[color][square])
		#define Kmagic(square) MagicMoves_King_DB[square]
		#define Nmagic(square) MagicMoves_Knight_DB[square]
        #define Bmagic(square, occupancy) *(MagicIndexesBishop[square]+((((occupancy)&MagicMask_Bishop[square])*MagicNumbers_Bishop[square])>>MagicShiftsBishop[square]))
        #define Rmagic(square, occupancy) *(MagicIndexesRook[square]+((((occupancy)&MagicMask_Rook[square])*MagicNumbers_Rook[square])>>MagicShiftsRook[square]))
	#endif //USE_FORCE_INLINE

    //extern U64 MagicMoves_Bishop_DB[5248];
    extern const unsigned int MagicShiftsBishop[64];
    extern const U64* MagicIndexesBishop[64];

    //extern U64 MagicMoves_Rook_DB[102400];
    extern const unsigned int MagicShiftsRook[64];
    extern const U64* MagicIndexesRook[64];

	#else //Nu minimizam marimea bazei de date
		#ifndef USE_FORCE_INLINE
			#define Pmagic(square,color) MagicMoves_Pawns_DB[color][square]
			#define PAttackMagic(square,color) (MagicMoves_Pawns_DB[color][square]? MagicMoves_Pawns_DB[color][square]^(((U64)1)<<(square+(color?-8:8))):MagicMoves_Pawns_DB[color][square])
			#define PNormalMagic(square,color) (MagicMoves_Pawns_DB[color][square]? MagicMoves_Pawns_DB[color][square]&(((U64)1)<<(square+(color?-8:8))):MagicMoves_Pawns_DB[color][square])
			#define Kmagic(square) MagicMoves_King_DB[square]
			#define Nmagic(square) MagicMoves_Knight_DB[square]
			#define Bmagic(square, occupancy) MagicMoves_Bishop_DB[square][(((occupancy)&MagicMask_Bishop[square])*MagicNumbers_Bishop[square])>>SHIFT_MINIMAL_BISHOP]
			#define Rmagic(square, occupancy) MagicMoves_Rook_DB[square][(((occupancy)&MagicMask_Rook[square])*MagicNumbers_Rook[square])>>SHIFT_MINIMAL_ROOK]
		#endif //USE_FORCE_INLINE
		extern U64 MagicMoves_Bishop_DB[64][1<<9];
		extern U64 MagicMoves_Rook_DB[64][1<<12];
   #endif //MINIMIZEAZA_MISCARI_MAGICE
#else //PERFCT_MAGIC_HASH definit...deci 
	#ifndef MINIMUM_MAGIC

	#ifndef USE_FORCE_INLINE
		#define Pmagic(square,color) MagicMoves_Pawns_DB[color][square]
		#define PAttackMagic(square,color) (MagicMoves_Pawns_DB[color][square]? MagicMoves_Pawns_DB[color][square]^(((U64)1)<<(square+(color?-8:8))):MagicMoves_Pawns_DB[color][square])
		#define PNormalMagic(square,color) (MagicMoves_Pawns_DB[color][square]? MagicMoves_Pawns_DB[color][square]&(((U64)1)<<(square+(color?-8:8))):MagicMoves_Pawns_DB[color][square])
		#define Kmagic(square) MagicMoves_King_DB[square]
		#define Nmagic(square) MagicMoves_Knight_DB[square]
        #define Bmagic(square, occupancy) MagicMoves_Bishop_DB[MagicIndexesBishop[square][(((occupancy)&MagicMask_Bishop[square])*MagicNumbers_Bishop[square])>>SHIFT_MINIMAL_BISHOP]]
        #define Rmagic(square, occupancy) MagicMoves_Rook_DB[MagicIndexesRook[square][(((occupancy)&MagicMask_Rook[square])*MagicNumbers_Rook[square])>>SHIFT_MINIMAL_ROOK]]
    #endif //USE_FORCE_INLINE

	extern U64 MagicMoves_Bishop_DB[1428];
    extern U64 MagicMoves_Rook_DB[4900];
    extern HASH_PERFECT_MAGIC MagicIndexesBishop[64][1<<9];
    extern HASH_PERFECT_MAGIC MagicIndexesRook[64][1<<12];
	#else
		#error magicmoves - MINIMIZED_MAGIC si HASH_PERFECT_MAGIC definie...nu putem avea ambele activate deoarece se contrazic
	#endif
#endif //PERFCT_MAGIC_HASH

#ifdef USE_FORCE_INLINE
	_FORCE_INLINE_ U64 QCastleMagic(const int color)
	{
		return CastleMasks[color][0];
	}
	_FORCE_INLINE_ U64 KCastleMagic(const int color)
	{
		return CastleMasks[color][1];
	}
	// Miscarile pionului,calului, turei, BISHOPului si regelui cu si fara masca
	_FORCE_INLINE_ U64 Pmagic(const unsigned int square,const unsigned int color)
	{
		return MagicMoves_Pawns_DB[color][square];
	}
	_FORCE_INLINE_ U64 PAttackMagic(const unsigned int square,const unsigned int color)
	{
		return (MagicMoves_Pawns_DB[color][square]? MagicMoves_Pawns_DB[color][square]&(~(((U64)0x101)<<(square+(color?-16+(((square>>3)&1)<<3):8)))):MagicMoves_Pawns_DB[color][square]);
	}
	_FORCE_INLINE_ U64 PNormalMagic(const unsigned int square,const unsigned int color)
	{
		return (MagicMoves_Pawns_DB[color][square]? MagicMoves_Pawns_DB[color][square]&(((U64)0x101)<<(square+(color?-16+(((square>>3)&1)<<3):8))):MagicMoves_Pawns_DB[color][square]);
	}
	_FORCE_INLINE_ U64 PEnpasMagic(const unsigned int square,const unsigned int color)
	{
		return (MagicMoves_PawnEnPas_DB[color][square]);
	}
	_FORCE_INLINE_ U64 Nmagic(const unsigned int square)
	{
		return MagicMoves_Knight_DB[square];
	}
	_FORCE_INLINE_ U64 Kmagic(const unsigned int square)
	{
		return MagicMoves_King_DB[square];
	}
	_FORCE_INLINE_ U64 Bmagic(const unsigned int square,const U64 occupancy)
	{
		#ifndef HASH_PERFECT_MAGIC
			#ifdef MINIMUM_MAGIC
				return *(MagicIndexesBishop[square]+(((occupancy&MagicMask_Bishop[square])*MagicNumbers_Bishop[square])>>MagicShiftsBishop[square]));
			#else
				return MagicMoves_Bishop_DB[square][(((occupancy)&MagicMask_Bishop[square])*MagicNumbers_Bishop[square])>>SHIFT_MINIMAL_BISHOP];
			#endif
		#else
			return MagicMoves_Bishop_DB[MagicIndexesBishop[square][(((occupancy)&MagicMask_Bishop[square])*MagicNumbers_Bishop[square])>>SHIFT_MINIMAL_BISHOP]];
		#endif
	}
	_FORCE_INLINE_ U64 Rmagic(const unsigned int square,const U64 occupancy)
	{
		#ifndef HASH_PERFECT_MAGIC
			#ifdef MINIMUM_MAGIC
				return *(MagicIndexesRook[square]+(((occupancy&MagicMask_Rook[square])*MagicNumbers_Rook[square])>>MagicShiftsRook[square]));
			#else
				return MagicMoves_Rook_DB[square][(((occupancy)&MagicMask_Rook[square])*MagicNumbers_Rook[square])>>SHIFT_MINIMAL_ROOK];
			#endif
		#else
			return MagicMoves_Rook_DB[MagicIndexesRook[square][(((occupancy)&MagicMask_Rook[square])*MagicNumbers_Rook[square])>>SHIFT_MINIMAL_ROOK]];
		#endif
	}
	// Miscarile damei cu si fara masca
	_FORCE_INLINE_ U64 Qmagic(const unsigned int square,const U64 occupancy)
	{
		return Bmagic(square,occupancy)|Rmagic(square,occupancy);
	}
#else

	#define Qmagic(square, occupancy) (Bmagic(square,occupancy)|Rmagic(square,occupancy))

#endif

const void InitMagicMoves();	// Functia care face initializarea primara. 

#endif