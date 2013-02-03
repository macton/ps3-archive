#ifndef UINT64_INSTR_H
#define UINT64_INSTR_H

#include <stdint.h>

static inline uint64_t uint64_li( uint64_t a )
{
    return (a);
}

static inline uint64_t uint64_and( uint64_t a, uint64_t b )
{
    return ( a & b );
}

static inline uint64_t uint64_andc( uint64_t a, uint64_t b )
{
    return ( a & ~b );
}

static inline uint64_t uint64_or( uint64_t a, uint64_t b )
{
    return ( a | b );
}

static inline uint64_t uint64_xor( uint64_t a, uint64_t b )
{
    return ( a ^ b );
}

static inline uint64_t uint64_sub( uint64_t a, uint64_t b )
{
    return ( a - b );
}

static inline uint64_t uint64_add( uint64_t a, uint64_t b )
{
    return ( a + b );
}

static inline uint64_t uint64_srl( uint64_t a, uint64_t sa )
{
    return ( a >> sa );
}

static inline uint64_t uint64_sll( uint64_t a, uint64_t sa )
{
    return ( a << sa );
}

static inline uint64_t uint64_sra( uint64_t a, uint64_t sa )
{
    return ( (int32_t)a >> sa );
}

static inline uint64_t uint64_not( uint64_t a )
{
    return (~a);
}

static inline uint64_t uint64_neg( uint64_t a )
{
    return (-a);
}

static inline uint64_t uint64_nand( uint64_t a, uint64_t b )
{
    return ~(a & b);
}

static inline uint64_t uint64_cmpeq( uint64_t a, uint64_t b )
{
    uint64_t diff_pos      = uint64_sub( a, b );
    uint64_t diff_neg      = uint64_neg( diff_pos );
    uint64_t diff_nez_msb  = uint64_or( diff_pos, diff_neg );
    uint64_t diff_eqz_msb  = uint64_not( diff_nez_msb );
    uint64_t diff_eqz_mask = uint64_sra( diff_eqz_msb, 31 );

    return (diff_eqz_mask);
}

static inline uint64_t uint64_cmpnez( uint64_t a )
{
    uint64_t a_neg      = uint64_neg( a );
    uint64_t a_nez_msb  = uint64_or( a, a_neg );
    uint64_t a_nez_mask = uint64_sra( a_nez_msb, 31 );

    return (a_nez_mask);
}


static inline uint64_t uint64_cmpeqz( uint64_t a )
{
    uint64_t a_nez_mask = uint64_cmpnez( a );
    uint64_t a_eqz_mask = uint64_not( a_nez_mask );

    return (a_eqz_mask);
}

static inline uint64_t uint64_cmplt( uint64_t a, uint64_t b )
{
    uint64_t diff          = uint64_sub( a, b );
    uint64_t diff_ltz_mask = uint64_sra( diff, 31 );

    return (diff_ltz_mask);
}

static inline uint64_t uint64_sel( uint64_t mask, uint64_t a, uint64_t b )
{
    return (( mask & a ) | ( ~mask & b ));
}

static inline uint64_t uint64_selneg( uint64_t mask, uint64_t a )
{
    return (a+mask)^mask;
}

#endif /* UINT64_INSTR_H */
