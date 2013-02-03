#include <stdint.h>

static inline uint32_t mask_s32_sel( uint32_t mask, int32_t a, int32_t b )
{
  return ( ( mask & (uint32_t)a) | ( (~mask) & (uint32_t)b ) );
}

static inline uint32_t mask_s32_lt( int32_t a, int32_t b )
{
  // NOTE: Result is invalid when a=INT32_MAX, b=INT32_MIN
  return (uint32_t)( ( a - b ) >> 31 );
}

static inline uint32_t mask_s32_gt( int32_t a, int32_t b )
{
  // NOTE: Result is invalid when a=INT32_MIN, b=INT32_MAX
  return (uint32_t)( ( b - a ) >> 31 );
}

static inline int32_t mask_s32_nz( int32_t a )
{
  return ( ( a | -a ) >> 31 );
}

static inline int32_t mask_s32_eq( int32_t a, int32_t b )
{
  return ( ~mask_s32_nz( a-b ) );
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
half_from_float( int32_t i )
{
  //
  // Our floating point number, f, is represented by the bit
  // pattern in integer i.  Disassemble that bit pattern into
  // the sign, s, the exponent, e, and the significand, m.
  // Shift s into the position where it will go in in the
  // resulting half number.
  // Adjust e, accounting for the different exponent bias
  // of float and half (127 versus 15).
  //

  const uint32_t s                   =  (i >> 16) & 0x00008000;
  const uint32_t e                   = ((i >> 23) & 0x000000ff) - (127 - 15);
  const uint32_t m                   =   i        & 0x007fffff;

  //
  // Round to nearest, round "0.5" up.
  //
  // Rounding may cause the significand to overflow and make
  // our number normalized.  Because of the way a half's bits
  // are laid out, we don't have to treat this case separately;
  // the code below will handle it correctly.
  // 

  const uint32_t m_rounded           = m + ( (m & 0x00001000) << 1 );

  //
  // Calculating denormalized value requires shift by variable amount.
  // This is microcoded on the PPU, so note that this operation is
  // extremely expensive. 
  //
  const uint32_t m_denorm            = (m_rounded | 0x00800000) >> (1 - e);

  const uint16_t h_denorm            = s | (m_denorm >> 13);
  const uint16_t h_zero              = s;
  const uint16_t h_inf               = s | 0x7c00;
  const uint16_t m_nan               = m >> 13;
  const uint16_t h_nan               = s | 0x7c00 | m_nan;
  const uint16_t h_nan_notinf        = h_nan | 0x01;
  const uint16_t h_norm_overflow     = s | ((e+1) << 10);
  const uint16_t h_norm              = s | (e << 10) | ( m_rounded >> 13 );

  //
  // E is less than -10.  The absolute value of f is
  // less than HALF_MIN (f may be a small normalized
  // float, a denormalized float or a zero).
  //
  // We convert f to a half zero.
  //
  const uint16_t    f_is_too_small      = (uint16_t)mask_s32_lt( e, -10 );

  //
  // E is between -10 and 0.  F is a normalized float,
  // whose magnitude is less than HALF_NRM_MIN.
  //
  // We convert f to a denormalized half.
  // 
  const uint16_t    f_is_small          = (uint16_t)mask_s32_lt( e, 1 );
  

  //
  // E can be flagged for either NAN or INF.
  //
  const uint16_t    e_is_flagged        = (uint16_t)mask_s32_eq( e,  0xff - (127 - 15) );

  //
  // F is an infinity; convert f to a half
  // infinity with the same sign as f.
  //
  const uint16_t    f_is_inf            = e_is_flagged & (uint16_t)mask_s32_eq( m, 0 );

  //
  // F is a NAN; we produce a half NAN that preserves
  // the sign bit and the 10 leftmost bits of the
  // significand of f, with one exception: If the 10
  // leftmost bits are all zero, the NAN would turn 
  // into an infinity, so we have to set at least one
  // bit in the significand.
  //
  const uint16_t    f_is_nan_inf        = e_is_flagged & (uint16_t)mask_s32_eq( m_nan, 0 );
  const uint16_t    f_is_nan            = e_is_flagged;

  //
  // Handle exponent overflow
  //
  const uint16_t    e_is_overflow       = (uint16_t)mask_s32_gt( e, 30 );
 
  //
  // Overflow in significand,
  // Adjust exponent
  //
  const uint16_t    m_norm_is_overflow  = (uint16_t)mask_s32_nz( m_rounded & 0x00800000 );

  //
  // Combine two cases that cause half to be INF
  //
  const uint16_t    h_is_inf            = e_is_overflow | f_is_inf;

  // 
  // Select correct result
  //

  //
  // Default:
  // E is greater than zero.  F is a normalized float.
  // We try to convert f to a normalized half.
  //

  const uint16_t result_0 = h_norm;
  const uint16_t result_1 = mask_s32_sel( m_norm_is_overflow, h_norm_overflow, result_0 );
  const uint16_t result_2 = mask_s32_sel( f_is_nan,           h_nan,           result_1 );
  const uint16_t result_3 = mask_s32_sel( f_is_nan_inf,       h_nan_notinf,    result_2 );
  const uint16_t result_4 = mask_s32_sel( h_is_inf,           h_inf,           result_3 );
  const uint16_t result_5 = mask_s32_sel( f_is_small,         h_denorm,        result_4 );
  const uint16_t result_6 = mask_s32_sel( f_is_too_small,     h_zero,          result_5 );

  return (result_6);
}
