#include <stdint.h>

uint16_t
half_add( uint16_t ha, uint16_t hb )
{
  uint16_t   ha_s = ha & 0x8000;
  uint16_t   ha_e = ha & 0x7c00;
  uint16_t   ha_m = ha & 0x03ff;
  uint16_t   hb_s = hb & 0x8000;
  uint16_t   hb_e = hb & 0x7c00;
  uint16_t   hb_m = hb & 0x03ff;

  uint16_t e   = ha_e;
  uint16_t s   = ha_s;
  uint16_t m0  = ha_m;
  uint16_t m1  = hb_m;
  uint16_t s0  = ha_s;
  uint16_t s1  = hb_s;

  // Align radix points
  if ( ha_e > hb_e )
  {
    uint16_t m_sa  = ((ha_e >> 10) - (hb_e >> 10)) - 1;
    uint16_t adj_m = ( hb_m >> 1 ) | 0x0200;
    uint16_t sh_m  = adj_m >> m_sa;

    e    = ha_e;
    s    = ha_s;
    s0   = ha_s;
    s1   = hb_s;
    m0   = ha_m;
    m1   = sh_m;
  }

  else if ( ha_e < hb_e )
  {
    uint16_t m_sa  = ((hb_e >> 10) - (ha_e >> 10)) - 1;
    uint16_t adj_m = ( ha_m >> 1 ) | 0x0200;
    uint16_t sh_m  = adj_m >> m_sa;

    e    = hb_e;
    s    = hb_s;
    s0   = hb_s;
    s1   = ha_s;
    m0   = hb_m;
    m1   = sh_m;
  }

  if ( s0 ^ s1 )
  {
    if ( s0 )
    {
      m0 = (~m0)+1;
    }
    else
    {
      m1 = (~m1)+1;
    }
  }

  uint16_t h = 0x0 | s | e | (m0+m1);

  // TODO: Handle mantissa overflow
  // TODO: Handle INF
  // TODO: Handle NaN

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
