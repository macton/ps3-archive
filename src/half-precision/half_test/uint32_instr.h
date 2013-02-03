#ifndef UINT32_INSTR_H
#define UINT32_INSTR_H

#include <stdint.h>

static inline uint32_t uint32_li( uint32_t a )
{
    return (a);
}

static inline uint32_t uint32_and( uint32_t a, uint32_t b )
{
    return ( a & b );
}

static inline uint32_t uint32_andc( uint32_t a, uint32_t b )
{
    return ( a & ~b );
}

static inline uint32_t uint32_or( uint32_t a, uint32_t b )
{
    return ( a | b );
}

static inline uint32_t uint32_xor( uint32_t a, uint32_t b )
{
    return ( a ^ b );
}

static inline uint32_t uint32_sub( uint32_t a, uint32_t b )
{
    return ( a - b );
}

static inline uint32_t uint32_add( uint32_t a, uint32_t b )
{
    return ( a + b );
}

static inline uint32_t uint32_srl( uint32_t a, uint32_t sa )
{
    return ( a >> sa );
}

static inline uint32_t uint32_sll( uint32_t a, uint32_t sa )
{
    return ( a << sa );
}

static inline uint32_t uint32_sra( uint32_t a, uint32_t sa )
{
    return ( (int32_t)a >> sa );
}

static inline uint32_t uint32_not( uint32_t a )
{
    return (~a);
}

static inline uint32_t uint32_neg( uint32_t a )
{
    return (-a);
}

static inline uint32_t uint32_nand( uint32_t a, uint32_t b )
{
    return ~(a & b);
}

static inline uint32_t uint32_cmpeq( uint32_t a, uint32_t b )
{
    uint32_t diff_pos      = uint32_sub( a, b );
    uint32_t diff_neg      = uint32_neg( diff_pos );
    uint32_t diff_nez_msb  = uint32_or( diff_pos, diff_neg );
    uint32_t diff_eqz_msb  = uint32_not( diff_nez_msb );
    uint32_t diff_eqz_mask = uint32_sra( diff_eqz_msb, 31 );

    return (diff_eqz_mask);
}

static inline uint32_t uint32_cmpnez( uint32_t a )
{
    uint32_t a_neg      = uint32_neg( a );
    uint32_t a_nez_msb  = uint32_or( a, a_neg );
    uint32_t a_nez_mask = uint32_sra( a_nez_msb, 31 );

    return (a_nez_mask);
}


static inline uint32_t uint32_cmpeqz( uint32_t a )
{
    uint32_t a_nez_mask = uint32_cmpnez( a );
    uint32_t a_eqz_mask = uint32_not( a_nez_mask );

    return (a_eqz_mask);
}

static inline uint32_t uint32_cmplt( uint32_t a, uint32_t b )
{
    uint32_t diff          = uint32_sub( a, b );
    uint32_t diff_ltz_mask = uint32_sra( diff, 31 );

    return (diff_ltz_mask);
}

static inline uint32_t uint32_sel( uint32_t mask, uint32_t a, uint32_t b )
{
    return (( mask & a ) | ( ~mask & b ));
}

static inline uint32_t uint32_selneg( uint32_t mask, uint32_t a )
{
    return (a+mask)^mask;
}

#endif /* UINT32_INSTR_H */
