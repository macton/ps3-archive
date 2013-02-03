#include <stdint.h>
#include <stdio.h>

static inline uint16_t nlz_u16( uint16_t x )
{
  x = x | ( x >> 1 );
  x = x | ( x >> 2 );
  x = x | ( x >> 4 );
  x = x | ( x >> 8 );
  x = ~x;
  x = x - (( x >> 1 ) & 0x5555);
  x = ( x & 0x3333 ) + ((x >> 2) & 0x3333);
  x = ( x + (x >> 4)) & 0x0f0f;
  x = x + ( x >> 8 );

  return ( x & 0x001f );
}

uint16_t
half_add( uint16_t a, uint16_t b )
{
  uint16_t   a_s = a & 0x8000;
  uint16_t   a_e = a & 0x7c00;
  uint16_t   a_m = a & 0x03ff;

  uint16_t   b_s = b & 0x8000;
  uint16_t   b_e = b & 0x7c00;
  uint16_t   b_m = b & 0x03ff;

  if ( b_e > a_e )
  {
    uint16_t t_m = b_m;
    b_m = a_m;
    a_m = t_m;

    uint16_t t_e = b_e;
    b_e = a_e;
    a_e = t_e;

    uint16_t t_s = b_s;
    b_s = a_s;
    a_s = t_s;
  }

  a_m |= 0x400;   
  b_m |= 0x400;   

  // a > b 
  // grs (guard + round + sticky), 3 bits

  uint16_t m_sa        = ((a_e >> 10) - (b_e >> 10));

  a_m <<= 3;
  b_m <<= 3;

  uint16_t sh_m        = b_m >> m_sa;

  uint16_t sticky_mask = ( 1 << m_sa ) -1;
  uint16_t sticky = ( b_m & sticky_mask ) ? 0x0001 : 0x0000;

  b_m = sh_m | sticky;

  uint16_t z_m;

  uint16_t z_e = a_e;
  uint16_t z_s = a_s;

  if (a_s ^ b_s )
  {
    if (a_m > b_m)
    {
      z_m = a_m - b_m;
      z_s = a_s;
    }
    else
    {
      z_m = b_m - a_m;
      z_s = b_s;
    }

    int8_t sa = nlz_u16( z_m ) - 1;

    if ( sa == 15 )
    {
      return (0);
    }

    z_m = z_m << sa;
    z_e = ( ( z_e >> 10 ) - sa ) << 10;
  }
  else
  {
    z_m = a_m + b_m;
  }

  if ( z_m & 0x4000 )
  {
    z_m = ( z_m >> 1 );
    z_e = ( ( z_e >> 10 ) + 1 ) << 10;
  }

  z_m = ( z_m >> 3 );

  // remove implicit bit
  z_m = z_m & 0x3ff;

  uint16_t h = z_s | z_e | z_m;

  return (h);
}

uint16_t 
half_sub( uint16_t ha, uint16_t hb ) 
{
  // MJA - (a-b) is the same as (a+(-b))
  // b and -b are the same except the sign
  // bit is flipped.
  // Just flip the sign bit and call add.

  return half_add( ha, hb ^ 0x8000 );
}

uint16_t 
half_mul( uint16_t arg0, uint16_t arg1 ) 
{
  // MJA - TODO
  return (0);
}

uint16_t 
half_div( uint16_t arg0, uint16_t arg1 ) 
{
  // MJA - TODO
  return (0);
}
