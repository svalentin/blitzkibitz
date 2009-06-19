#ifndef MAGIC_H_
#define MAGIC_H_

typedef unsigned long long ull;

// pregenerated line/ column bitboards
extern const ull lbb[8], cbb[8];


// magic bitboards

extern ull magicmovesndb[64];
extern ull magicmoveskdb[64];

extern const ull magicmoves_r_magics[64];
extern const ull magicmoves_r_mask[64];
extern const ull magicmoves_b_magics[64];
extern const ull magicmoves_b_mask[64];

#define MINIMAL_B_BITS_SHIFT 55
#define MINIMAL_R_BITS_SHIFT 52

extern ull magicmovesbdb[64][1<<9];
extern ull magicmovesrdb[64][1<<12];

inline ull Nmagic(const unsigned int square)
{
    return magicmovesndb[square];
}

inline ull Kmagic(const unsigned int square)
{
    return magicmoveskdb[square];
}

inline ull Bmagic(const unsigned int square, const ull occupancy)
{
    return magicmovesbdb[square][((occupancy & magicmoves_b_mask[square]) * magicmoves_b_magics[square]) >> MINIMAL_B_BITS_SHIFT];
}
inline ull BmagicNOMASK(const unsigned int square, const ull occupancy)
{
    return magicmovesbdb[square][(occupancy * magicmoves_b_magics[square]) >> MINIMAL_B_BITS_SHIFT];
}

inline ull Rmagic(const unsigned int square, const ull occupancy)
{
    return magicmovesrdb[square][((occupancy & magicmoves_r_mask[square]) * magicmoves_r_magics[square]) >> MINIMAL_R_BITS_SHIFT];
}
inline ull RmagicNOMASK(const unsigned int square, const ull occupancy)
{
    return magicmovesrdb[square][(occupancy * magicmoves_r_magics[square]) >> MINIMAL_R_BITS_SHIFT];
}

inline ull Qmagic(const unsigned int square, const ull occupancy)
{
    return Bmagic(square, occupancy) | Rmagic(square, occupancy);
}
inline ull QmagicNOMASK(const unsigned int square, const ull occupancy)
{
    return BmagicNOMASK(square, occupancy) | RmagicNOMASK(square, occupancy);
}

void initmagicmoves(void);

#endif // MAGIC_H_
