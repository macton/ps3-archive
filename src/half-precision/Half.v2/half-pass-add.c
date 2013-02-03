#include "half.h"
#include <stdio.h>

static inline uint32_t mask_u32_sel( uint32_t mask, uint32_t a, uint32_t b )
{
  return ( ( mask & (uint32_t)a) | ( (~mask) & (uint32_t)b ) );
}

static inline uint32_t mask_u32_lt( int32_t a, int32_t b )
{
  // NOTE: Result is invalid when a=INT32_MAX, b=INT32_MIN
  return (uint32_t)( (int32_t)( a - b ) >> 31 );
}

static inline uint32_t mask_u32_gt( int32_t a, int32_t b )
{
  // NOTE: Result is invalid when a=INT32_MIN, b=INT32_MAX
  return (uint32_t)( (int32_t)( b - a ) >> 31 );
}

static inline uint32_t mask_u32_nz( int32_t a )
{
  return (uint32_t)( (int32_t)( a | -a ) >> 31 );
}

static inline uint32_t mask_u32_nz_p( int32_t a )
{
  return (uint32_t)( (uint32_t)( a | -a ) >> 31 );
}

static inline uint32_t mask_u32_eq( int32_t a, int32_t b )
{
  return ( ~mask_u32_nz( a-b ) );
}

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

uint32_t
half_to_float( uint16_t h )
{
  uint32_t result;
  
  uint32_t  hs = (uint32_t)(h & 0x8000);
  uint32_t  he = (uint32_t)(h & 0x7c00);
  uint32_t  hm = (uint32_t)(h & 0x03ff);
  uint32_t  fs = ( hs << 16 );
  uint32_t  fe = ( he >> 10 ) - 15 + 127;
  uint32_t  fm = ( hm << 13 );

  // MJA - When the exponent is ZERO, adjusting the bias is
  // not valid. (has no meaning, 
  // See section 2.1.2 of
  // http://oss.sgi.com/projects/ogl-sample/registry/ARB/half_float_pixel.txt

  // (-1)^S * 0.0 (+/- ZERO)
  if ( ( he == 0x0000 ) && ( hm == 0x0000 ) )
  {
    fm = 0;
    fe = 0;
  }

  // (-1)^S * 2^-14 * (M / 2^10)
  else if ( he == 0x0000 )
  {
    int sa = nlz_u16( hm ) + 10;

    fm  = ((uint32_t)hm) << (sa-2);
    fe  = (1-sa) + 127;
    fm &= 0x007fffff;
  }

  // (-1)^S * INF
  else if ( ( he == 0x7c00 ) && ( hm == 0x0000 ) )
  {
    fe = (uint32_t)(0x00ff);
    fm = (uint32_t)(0x0000);
  }

  // NaN
  else if ( he == 0x7c00 )
  {
    fe = (uint32_t)(0x00ff);
  }

  result = fs | ( fe << 23 ) | fm;

  return (result);
}

uint16_t
half_from_float( uint32_t f )
{
  const uint32_t s                   = (f & 0x80000000) >> 16;
  const uint32_t e                   = ((f & 0x7f800000) >> 23) - (127-15);
  const uint32_t m                   = f & 0x007fffff;
  const uint32_t m_rounded           = m + ( (m & 0x00001000) << 1 );
  const uint32_t m_denorm            = (m_rounded | 0x00800000) >> (1 - e);
  const uint16_t h_denorm            = s | (m_denorm >> 13);
  const uint16_t h_inf               = s | 0x7c00;
  const uint16_t m_nan               = m >> 13;
  const uint16_t h_nan               = s | 0x7c00 | m_nan;
  const uint16_t h_nan_notinf        = h_nan | 0x01;
  const uint16_t h_norm_overflow     = s | ((e+1) << 10);
  const uint16_t h_norm              = s | (e << 10) | ( m_rounded >> 13 );
  const uint16_t f_is_small          = mask_u32_lt( e, 1 );
  const uint16_t e_is_flagged        = mask_u32_eq( e,  0xff - (127 - 15) );
  const uint16_t f_is_inf            = e_is_flagged & mask_u32_eq( m, 0 );
  const uint16_t f_is_nan_inf        = e_is_flagged & mask_u32_eq( m_nan, 0 );
  const uint16_t f_is_nan            = e_is_flagged;
  const uint16_t e_is_overflow       = mask_u32_gt( e, 30 );
  const uint16_t m_norm_is_overflow  = mask_u32_nz( m_rounded & 0x00800000 );
  const uint16_t h_is_inf            = e_is_overflow | f_is_inf;
  const uint16_t result_0            = h_norm;
  const uint16_t result_1            = mask_u32_sel( m_norm_is_overflow, h_norm_overflow, result_0 );
  const uint16_t result_2            = mask_u32_sel( f_is_nan,           h_nan,           result_1 );
  const uint16_t result_3            = mask_u32_sel( f_is_nan_inf,       h_nan_notinf,    result_2 );
  const uint16_t result_4            = mask_u32_sel( h_is_inf,           h_inf,           result_3 );
  const uint16_t result_5            = mask_u32_sel( f_is_small,         h_denorm,        result_4 );

  return (result_5);
}


uint16_t
half_add( uint16_t x, uint16_t y )
{
  uint16_t x_s               = x & 0x8000;
  uint16_t x_e               = x & 0x7c00;
  uint16_t x_m               = x & 0x03ff;
  uint16_t y_s               = y & 0x8000;
  uint16_t y_e               = y & 0x7c00;
  uint16_t y_m               = y & 0x03ff;
  uint16_t larger_mask       = mask_u32_gt( y_e, x_e );
  uint16_t nlarger_mask      = ~larger_mask;
  uint16_t a_s               = (y_s & larger_mask)|(x_s & nlarger_mask);
  uint16_t a_e               = (y_e & larger_mask)|(x_e & nlarger_mask);
  uint16_t a_m_no_hidden_bit = (y_m & larger_mask)|(x_m & nlarger_mask);
  uint16_t b_s               = (x_s & larger_mask)|(y_s & nlarger_mask);
  uint16_t b_e               = (x_e & larger_mask)|(y_e & nlarger_mask);
  uint16_t b_m_no_hidden_bit = (x_m & larger_mask)|(y_m & nlarger_mask);
  uint16_t a_is_inf          = mask_u32_eq( a_e|a_m_no_hidden_bit, 0x7c00 );
  uint16_t a_inf             = a_s | 0x7c00;
  uint16_t a_m_no_grs        = a_m_no_hidden_bit | (((a_e | (-a_e)) >> 5) & 0x400);
  uint16_t b_m_no_grs        = b_m_no_hidden_bit | (((b_e | (-b_e)) >> 5) & 0x400);
  uint16_t m_sa_unbias       = (a_e-16) >> 10;
  uint16_t m_sa_default      = ((a_e >> 10) - (b_e >> 10));
  uint16_t b_e_z_mask        = ~mask_u32_nz(b_e);
  uint16_t a_e_nz_mask       = mask_u32_nz(a_e);
  uint16_t m_sa_unbias_mask  = b_e_z_mask & a_e_nz_mask;
  uint16_t m_sa              = (m_sa_unbias & m_sa_unbias_mask)|(m_sa_default & ~m_sa_unbias_mask);
  uint16_t a_m               = a_m_no_grs << 3;
  uint16_t b_m_no_sticky     = b_m_no_grs << 3;
  uint16_t sh_m              = b_m_no_sticky >> m_sa;
  uint16_t sticky_mask       = ( 1 << m_sa )-1;
  uint16_t sticky            = mask_u32_nz_p( b_m_no_sticky & sticky_mask );
  uint16_t b_m               = sh_m | sticky;

  uint16_t c_e               = a_e >> 10;
  uint16_t z_s               = a_s;
  uint16_t same_sign         = mask_u32_nz(a_s ^ b_s);

  uint16_t z_m;

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

    if ( z_m == 0 )
    {
      return (0);
    }

    uint16_t sa  = nlz_u16( z_m ) - 1;

    if (c_e)
    {
      if ( c_e < sa )
      {
        z_m = z_m << (c_e-1);
        c_e = 0;
      }
      else
      {
        z_m = z_m << sa;
        // c_e = ( ( c_e >> 10 ) - sa ) << 10;
        c_e = c_e - sa;
      }
    }
    
    if (z_m & 0x4000 )
    {
      z_m = ( z_m >> 1 );
      // c_e = ( ( c_e >> 10 ) + 1 ) << 10;
      c_e = c_e + 1;
    }
  }
  else
  {
    z_m = a_m + b_m;
  }

  if (z_m & 0x4000 )
  {
    z_m = ( z_m >> 1 );
    // c_e = ( ( c_e >> 10 ) + 1 ) << 10;
    c_e = c_e + 1;
  }

  if (a_e == 0)
  {
    if ( ( z_m & 0x2000 ) == 0x2000 )
    {
      // c_e = ( ( c_e >> 10 ) + 1 ) << 10;
      c_e = c_e + 1;
    }
  }

  z_m = z_m + (z_m & 0x07);
  c_e = (( c_e ) + ((z_m&0x4000)>>14) );
  z_m = (z_m >> 3);
  z_m = z_m & 0x3ff;

  uint16_t z_e    = c_e << 10;
  uint16_t h      = z_s | z_e | z_m;
  uint16_t result = (a_inf & a_is_inf)|(h & (~a_is_inf));

  return (result);
}

