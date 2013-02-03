#include "half.h"
#include <stdio.h>

static inline uint32_t uint32_nor( int32_t a, int32_t b )
{
  return ~(a | b);
}

static inline uint32_t uint32_andc( int32_t a, int32_t b )
{
  return (a & (~b));
}

static inline uint32_t uint32_and( int32_t a, int32_t b )
{
  return (a & b);
}

static inline uint32_t uint32_sel( uint32_t mask, uint32_t a, uint32_t b )
{
  return ( ( mask & (uint32_t)a) | ( (~mask) & (uint32_t)b ) );
}

static inline uint32_t uint32_lt( int32_t a, int32_t b )
{
  // NOTE: Result is invalid when a=INT32_MAX, b=INT32_MIN
  // For the purposes used in half.c the result is always valid
  return (uint32_t)( (int32_t)( a - b ) >> 31 );
}

static inline uint32_t uint32_gte( int32_t a, int32_t b )
{
  return ~uint32_lt( a, b );
}

static inline uint32_t uint32_gt( int32_t a, int32_t b )
{
  // NOTE: Result is invalid when b=INT32_MIN, a=INT32_MAX
  // For the purposes used in half.c the result is always valid
  return (uint32_t)( (int32_t)( b - a ) >> 31 );
}

static inline uint32_t uint32_nez( int32_t a )
{
  return (uint32_t)( (int32_t)( a | -a ) >> 31 );
}

static inline uint32_t uint32_nez_p( int32_t a )
{
  return (uint32_t)( (uint32_t)( a | -a ) >> 31 );
}

static inline uint32_t uint32_eq( int32_t a, int32_t b )
{
  return ( ~uint32_nez( a-b ) );
}

static inline uint16_t uint16_sel( uint16_t mask, uint16_t a, uint16_t b )
{
  return ( ( mask & (uint16_t)a) | ( (~mask) & (uint16_t)b ) );
}

static inline uint16_t uint16_lt( uint16_t a, uint16_t b )
{
  // NOTE: Result is invalid when a=INT16_MAX, b=INT16_MIN
  // For the purposes used in half.c the result is always valid
  return (uint16_t)( (int16_t)( a - b ) >> 15 );
}

static inline uint16_t uint16_gt( uint16_t a, uint16_t b )
{
  // NOTE: Result is invalid when b=INT32_MIN, a=INT32_MAX
  // For the purposes used in half.c the result is always valid
  return (uint16_t)( (int16_t)( b - a ) >> 15 );
}

static inline uint16_t uint16_nez( uint16_t a )
{
  return (uint16_t)( (int16_t)( a | -a ) >> 15 );
}

static inline uint16_t uint16_eqz( uint16_t a )
{
  return ~uint16_nez( a );
}

static inline uint16_t uint16_nez_p( uint16_t a )
{
  return (uint16_t)( (uint16_t)( a | -a ) >> 15 );
}

static inline uint16_t uint16_eqz_p( uint16_t a )
{
  return ~uint16_nez_p( a );
}

static inline uint16_t uint16_eq( uint16_t a, uint16_t b )
{
  return ( ~uint16_nez( a-b ) );
}

static inline uint16_t uint16_andc( uint16_t a, uint16_t b )
{
  return (a & (~b));
}

static inline uint16_t uint16_and( uint16_t a, uint16_t b )
{
  return (a & b);
}

static inline uint16_t uint16_or( uint16_t a, uint16_t b )
{
  return (a | b);
}

static inline uint16_t uint16_or3( uint16_t a, uint16_t b, uint16_t c )
{
  return (a | b | c);
}

static inline uint16_t uint16_add( uint16_t a, uint16_t b )
{
  return (a + b);
}

static inline uint16_t uint16_sub( uint16_t a, uint16_t b )
{
  return (a - b);
}

static inline uint16_t uint16_nlz( uint16_t x )
{
  const uint16_t x0  = x >> 1;
  const uint16_t x1  = x | x0;
  const uint16_t x2  = x1 >> 2;
  const uint16_t x3  = x1 | x2;
  const uint16_t x4  = x3 >> 4;
  const uint16_t x5  = x3 | x4;
  const uint16_t x6  = x5 >> 8;
  const uint16_t x7  = x5 | x6;
  const uint16_t x8  = ~x7;
  const uint16_t x9  = ( x8 >> 1 ) & 0x5555;
  const uint16_t xA  = x8 - x9;
  const uint16_t xB  = xA & 0x3333;
  const uint16_t xC  = ( xA >> 2 ) & 0x3333;
  const uint16_t xD  = xB + xC;
  const uint16_t xE  = xD >> 4;
  const uint16_t xF  = ( xD + xE ) & 0x0f0f;
  const uint16_t x10 = xF >> 8;
  const uint16_t x11 = ( xF + x10 ) & 0x001f;

  return ( x11 );
}

uint32_t
half_to_float( uint16_t h )
{
  const uint32_t h_f_bias_offset     = 15 - 127;
  const uint32_t h_s                 = uint32_and( h, 0x8000 );
  const uint32_t h_e                 = uint32_and( h, 0x7c00 );
  const uint32_t h_m                 = uint32_and( h, 0x03ff );
  const uint32_t f_s                 = ( h_s << 16 );
  const uint32_t f_e                 = ( h_e >> 10 ) - h_f_bias_offset;
  const uint32_t f_m                 = ( h_m << 13 );
  const uint32_t is_e_nezero         = uint32_nez( h_e );
  const uint32_t is_m_nezero         = uint32_nez( h_m );
  const uint32_t is_zero             = uint32_nor( is_e_nezero, is_m_nezero );
  const uint32_t is_denorm           = uint32_nor( is_zero, is_e_nezero );
  const uint32_t is_e_flagged        = uint32_eq( h_e, 0x00007c00 );
  const uint32_t is_inf              = uint32_andc( is_e_flagged, is_m_nezero );
  const uint32_t is_nan              = uint32_and( is_e_flagged,  is_m_nezero ); 
  const uint32_t h_denorm_sa         = uint16_nlz( (uint16_t) h_m ) + 10;
  const uint32_t f_denorm_m          = (h_m << (h_denorm_sa-2)) & 0x007fffff;
  const uint32_t f_denorm_e          = (1-h_denorm_sa) + 127;
  const uint32_t f_denorm            = f_s | ( f_denorm_e << 23 ) | f_denorm_m;
  const uint32_t f_inf               = f_s | 0x7f800000;
  const uint32_t f_nan               = f_s | 0x7f800000 | f_m;
  const uint32_t f_zero              = f_s;
  const uint32_t packed              = f_s | (f_e << 23) | f_m;
  const uint32_t check_zero_result   = uint32_sel( is_zero,   f_zero,   packed );
  const uint32_t check_denorm_result = uint32_sel( is_denorm, f_denorm, check_zero_result );
  const uint32_t check_inf_result    = uint32_sel( is_inf,    f_inf,    check_denorm_result );
  const uint32_t check_nan_result    = uint32_sel( is_nan,    f_nan,    check_inf_result    );
  const uint32_t result              = check_nan_result;

  return (result);
}

uint16_t
half_from_float( uint32_t f )
{
  const uint32_t f_h_bias_offset            = 127 - 15;
  const uint16_t f_s                        = (f & 0x80000000) >> 16;
  const uint16_t f_e                        = (f & 0x7f800000) >> 23;
  const uint32_t f_m                        = uint32_and( f, 0x007fffff );
  const uint16_t f_e_half_bias              = f_e - f_h_bias_offset;
  const uint32_t m_rounded                  = f_m + ( (f_m & 0x00001000) << 1 );
  const uint32_t m_denorm                   = (m_rounded | 0x00800000) >> (1 - f_e_half_bias);
  const uint16_t h_denorm                   = f_s | (m_denorm >> 13);
  const uint16_t h_inf                      = f_s | 0x7c00;
  const uint16_t m_nan                      = f_m >> 13;
  const uint16_t h_nan                      = f_s | 0x7c00 | m_nan;
  const uint16_t h_nan_notinf               = h_nan | 0x01;
  const uint16_t h_norm_overflow            = f_s | ((f_e_half_bias+1) << 10);
  const uint16_t h_norm                     = f_s | (f_e_half_bias << 10) | ( m_rounded >> 13 );
  const uint16_t h_is_denorm                = uint32_gte( f_h_bias_offset, f_e );
  const uint16_t e_is_flagged               = uint32_eq( f_e_half_bias,  0xff - f_h_bias_offset );
  const uint16_t f_is_inf                   = e_is_flagged & uint32_eq( f_m, 0 );
  const uint16_t f_is_nan_underflow         = e_is_flagged & uint32_eq( m_nan, 0 );
  const uint16_t f_is_nan                   = e_is_flagged;
  const uint16_t e_is_overflow              = uint32_gt( f_e_half_bias, 30 );
  const uint16_t m_norm_is_overflow         = uint32_nez( m_rounded & 0x00800000 );
  const uint16_t h_is_inf                   = e_is_overflow | f_is_inf;
  const uint16_t check_overflow_result      = uint32_sel( m_norm_is_overflow, h_norm_overflow, h_norm );
  const uint16_t check_nan_result           = uint32_sel( f_is_nan,           h_nan,           check_overflow_result );
  const uint16_t check_nan_underflow_result = uint32_sel( f_is_nan_underflow, h_nan_notinf,    check_nan_result );
  const uint16_t check_inf_result           = uint32_sel( h_is_inf,           h_inf,           check_nan_underflow_result );
  const uint16_t check_denorm_result        = uint32_sel( h_is_denorm,        h_denorm,        check_inf_result );
  const uint16_t result                     = check_denorm_result;

  return (result);
}


// Branch-free implementation of half-precision (16 bit) floating point add
//
// Features
// --------
//
//   * Signed inputs. (+x) + (-y) == (+x) - (+y)
//   * NaN + <x>  = NaN
//   * <x> + +INF = +INF
//   * <x> - -INF = -INF
//   * Denormalized values
//   * Difference of ZEROs is always +ZERO
//   * Round up with gaurd + round + sticky bit (grs)
//   * And of course... no branching
// 
// Precision of Sum
// ----------------
//
//   uint16_t z = half_add( x, y );
//
// Will have exactly (0 ulps difference) the same result as:
// (For 32 bit IEEE 784 floating point and same rounding mode)
//
//   union FLOAT_32
//   {
//     float    f32;
//     uint32_t u32;
//   };
//
//   union FLOAT_32 fx = { .u32 = half_to_float( x ) };
//   union FLOAT_32 fy = { .u32 = half_to_float( y ) };
//   union FLOAT_32 fz = { .f32 = fx.f32 + fy.f32    };
//   uint16_t       z  = float_to_half( fz );
//
// Precision of Difference
// -----------------------
//
//   uint16_t z = half_add( x, -y );
//
// Will have exactly (0 ulps difference) the same result as:
// (For 32 bit IEEE 784 floating point and same rounding mode)
//
//   union FLOAT_32
//   {
//     float    f32;
//     uint32_t u32;
//   };
//
//   union FLOAT_32 fx = { .u32 = half_to_float( x ) };
//   union FLOAT_32 fy = { .u32 = half_to_float( y ) };
//   union FLOAT_32 fz = { .f32 = fx.f32 - fy.f32    };
//   uint16_t       z  = float_to_half( fz.u32 );

uint16_t
half_add( uint16_t x, uint16_t y )
{
  const uint16_t x_s                  = uint16_and( x, 0x8000 );
  const uint16_t x_e                  = uint16_and( x, 0x7c00 );
  const uint16_t x_m                  = uint16_and( x, 0x03ff );
  const uint16_t y_s                  = uint16_and( y, 0x8000 );
  const uint16_t y_e                  = uint16_and( y, 0x7c00 );
  const uint16_t y_m                  = uint16_and( y, 0x03ff );
  const uint16_t is_y_e_larger        = uint16_gt( y_e, x_e );
  const uint16_t a_s                  = uint16_sel( is_y_e_larger, y_s, x_s );
  const uint16_t a_e                  = uint16_sel( is_y_e_larger, y_e, x_e );
  const uint16_t a_m_no_hidden_bit    = uint16_sel( is_y_e_larger, y_m, x_m );
  const uint16_t b_s                  = uint16_sel( is_y_e_larger, x_s, y_s );
  const uint16_t b_e                  = uint16_sel( is_y_e_larger, x_e, y_e );
  const uint16_t b_m_no_hidden_bit    = uint16_sel( is_y_e_larger, x_m, y_m );
  const uint16_t a_is_inf             = uint16_eq( a_e|a_m_no_hidden_bit, 0x7c00 );
  const uint16_t a_inf                = a_s | 0x7c00;
  const uint16_t a_m_no_grs           = a_m_no_hidden_bit | (((a_e | (-a_e)) >> 5) & 0x400);
  const uint16_t b_m_no_grs           = b_m_no_hidden_bit | (((b_e | (-b_e)) >> 5) & 0x400);
  const uint16_t m_sa_unbias          = (a_e-16) >> 10;
  const uint16_t m_sa_default         = ((a_e >> 10) - (b_e >> 10));
  const uint16_t b_e_z_mask           = uint16_eqz(b_e);
  const uint16_t a_e_nez_mask         = uint16_nez(a_e);
  const uint16_t m_sa_unbias_mask     = b_e_z_mask & a_e_nez_mask;
  const uint16_t m_sa                 = uint16_sel( m_sa_unbias_mask, m_sa_unbias, m_sa_default );
  const uint16_t a_m                  = a_m_no_grs << 3;
  const uint16_t b_m_no_sticky        = b_m_no_grs << 3;
  const uint16_t sh_m                 = b_m_no_sticky >> m_sa;
  const uint16_t sticky_mask          = ( 1 << m_sa )-1;
  const uint16_t sticky               = uint16_nez_p( b_m_no_sticky & sticky_mask );
  const uint16_t b_m                  = uint16_or( sh_m, sticky );
  const uint16_t a_e_biased           = a_e >> 10;
  const uint16_t is_diff_sign         = uint16_nez(a_s ^ b_s);
  const uint16_t z_m_sum              = uint16_add( a_m, b_m );
  const uint16_t z_m_diff_ab          = uint16_sub( a_m, b_m );
  const uint16_t z_m_diff_ba          = uint16_sub( b_m, a_m );
  const uint16_t is_z_m_ab_pos        = uint16_gt( a_m, b_m );
  const uint16_t z_m_smag_diff        = uint16_sel( is_z_m_ab_pos, z_m_diff_ab, z_m_diff_ba );
  const uint16_t z_s_diff             = uint16_sel( is_z_m_ab_pos, a_s, b_s );
  const uint16_t z_s                  = uint16_sel( is_diff_sign, z_s_diff, a_s );
  const uint16_t diff_norm_sa         = uint16_nlz( z_m_smag_diff ) - 1;
  const uint16_t is_diff_denorm       = uint16_lt( a_e_biased, diff_norm_sa );
  const uint16_t z_m_diff_denorm      = z_m_smag_diff << (a_e_biased-1);
  const uint16_t z_m_diff_norm        = z_m_smag_diff << diff_norm_sa;
  const uint16_t c_e_diff_norm        = uint16_sub( a_e_biased,  diff_norm_sa );
  const uint16_t is_a_or_b_norm       = uint16_nez( a_e_biased );
  const uint16_t z_m_diff_ab_norm     = uint16_sel( is_diff_denorm, z_m_diff_denorm, z_m_diff_norm );
  const uint16_t c_e_diff_ab_norm     = uint16_sel( is_diff_denorm, 0x0000, c_e_diff_norm );
  const uint16_t z_m_diff_ab_denorm   = z_m_smag_diff;
  const uint16_t c_e_diff_ab_denorm   = a_e_biased;
  const uint16_t z_m_diff             = uint16_sel( is_a_or_b_norm, z_m_diff_ab_norm, z_m_diff_ab_denorm );
  const uint16_t c_e_diff             = uint16_sel( is_a_or_b_norm, c_e_diff_ab_norm, c_e_diff_ab_denorm );
  const uint16_t is_diff_zero         = uint16_eqz( z_m_diff );
  const uint16_t is_diff_exactly_zero = uint16_and( is_diff_sign, is_diff_zero );
  const uint16_t z_m_added            = uint16_sel( is_diff_sign, z_m_diff, z_m_sum );
  const uint16_t c_e_added            = uint16_sel( is_diff_sign, c_e_diff, a_e_biased );
  const uint16_t is_set_hidden_mask   = (uint16_t)((int16_t)((z_m_added & 0x4000) << 1)>>15);
  const uint16_t c_e_hidden_offset    = ( z_m_added & 0x4000 ) >> 14;
  const uint16_t z_m_sub_hidden       = z_m_added >> 1;
  const uint16_t z_m_no_hidden        = uint16_sel( is_set_hidden_mask, z_m_sub_hidden, z_m_added );
  const uint16_t c_e_no_hidden        = uint16_add( c_e_added, c_e_hidden_offset );
  const uint16_t is_undenorm          = uint16_eqz_p( a_e );
  const uint16_t undenorm_m_msb_odd   = uint16_nez_p( z_m_no_hidden & 0x2000 );
  const uint16_t undenorm_fix_e       = is_undenorm & undenorm_m_msb_odd;
  const uint16_t c_e_fixed            = uint16_add( c_e_no_hidden, undenorm_fix_e );
  const uint16_t z_m_rounded          = uint16_add( z_m_no_hidden, z_m_no_hidden & 0x07 );
  const uint16_t z_m_round_overflow   = ( z_m_rounded & 0x4000) >> 14;
  const uint16_t c_e_rounded          = uint16_add( c_e_fixed, z_m_round_overflow );
  const uint16_t z_m_no_grs           = ( z_m_rounded >> 3 ) & 0x3ff;
  const uint16_t z_e                  = c_e_rounded << 10;
  const uint16_t z_packed             = uint16_or3( z_s, z_e, z_m_no_grs );
  const uint16_t check_inf_result     = uint16_sel( a_is_inf, a_inf, z_packed );
  const uint16_t check_zero_result    = uint16_andc( check_inf_result, is_diff_exactly_zero );
  const uint16_t result               = check_zero_result;

  return (result);
}

