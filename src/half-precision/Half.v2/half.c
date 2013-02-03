// Branch-free implementation of half-precision (16 bit) floating point
// Copyright 2006 Mike Acton <macton@gmail.com>
//
// Note: USE A BETTER COMPILER
// ---------------------------
//
//     This file is a C99 source file, intended to be compiled with a C99 
//     compliant compiler. However, for the moment it remains combatible
//     with C++98. Therefore if you are using a compiler that poorly implements
//     C standards (e.g. MSVC), it may be compiled as C++. This is not
//     guaranteed for future versions. 
//
//     There are pleanty of good options for the x86 platform, for example.
//     Intel has a compiler; There is GCC for Cygwin; Dev-C++ is built on GCC;
//     MinGW has a version of GCC. All the world is not Visual Studio.
//
// Features
// --------
//
//     * Signed inputs. (+x) + (-y) == (+x) - (+y)
//     * NaN + <x>  = NaN
//     * <x> + +INF = +INF
//     * <x> - -INF = -INF
//     * Denormalized values
//     * Difference of ZEROs is always +ZERO
//     * Round up with guard + round + sticky bit (grs)
//     * And of course... no branching
// 
// Precision of Sum
// ----------------
//
//     uint16_t z = half_add( x, y );
//
//     Will have exactly (0 ulps difference) the same result as:
//     (For 32 bit IEEE 784 floating point and same rounding mode)
//
//     union FLOAT_32
//     {
//       float    f32;
//       uint32_t u32;
//     };
//
//     union FLOAT_32 fx = { .u32 = half_to_float( x ) };
//     union FLOAT_32 fy = { .u32 = half_to_float( y ) };
//     union FLOAT_32 fz = { .f32 = fx.f32 + fy.f32    };
//     uint16_t       z  = float_to_half( fz );
//
// Precision of Difference
// -----------------------
//
//     uint16_t z = half_add( x, -y );
//
//     Will have exactly (0 ulps difference) the same result as:
//     (For 32 bit IEEE 784 floating point and same rounding mode)
//
//     union FLOAT_32
//     {
//       float    f32;
//       uint32_t u32;
//     };
//
//     union FLOAT_32 fx = { .u32 = half_to_float( x ) };
//     union FLOAT_32 fy = { .u32 = half_to_float( y ) };
//     union FLOAT_32 fz = { .f32 = fx.f32 - fy.f32    };
//     uint16_t       z  = float_to_half( fz.u32 );

#include "int_insn.h"
#include "half.h"
#include <stdio.h>

uint32_t
half_to_float( uint16_t h )
{
  const uint16_t one                  = 0x0001;
  const uint16_t two                  = 0x0002;
  const uint32_t f_e_mask             = 0x7f800000;
  const uint32_t f_m_mask             = 0x007fffff;
  const uint16_t f_e_bias             = 0x007f;
  const uint16_t h_e_bias             = 0x000f;
  const uint16_t h_s_mask             = 0x8000;
  const uint16_t h_e_mask             = 0x7c00;
  const uint16_t h_m_mask             = 0x03ff;
  const uint16_t h_e_pos              = 0x000a;
  const uint16_t f_e_pos              = 0x0017;
  const uint16_t f_s_pos              = 0x001f;
  const uint16_t h_s_pos              = 0x000f;
  const uint16_t h_f_denorm_sa_offset = 0x000a;
  const uint32_t h_f_bias_offset      = uint16_sub( h_e_bias, f_e_bias );
  const uint16_t h_f_s_pos_offset     = uint16_sub( f_s_pos, h_s_pos );
  const uint16_t h_f_m_pos_offset     = uint16_sub( f_e_pos, h_e_pos );
  const uint32_t h_s                  = uint32_and( h, h_s_mask );
  const uint32_t h_e                  = uint32_and( h, h_e_mask );
  const uint16_t h_m                  = uint32_and( h, h_m_mask );
  const uint32_t f_s                  = uint32_sll( h_s, h_f_s_pos_offset );
  const uint16_t h_e_half_bias        = uint16_srl( h_e, h_e_pos );
  const uint32_t f_e                  = uint16_sub( h_e_half_bias, h_f_bias_offset );
  const uint32_t f_m                  = uint32_sll( h_m, h_f_m_pos_offset );
  const uint32_t is_e_nezero          = uint32_nez( h_e );
  const uint32_t is_m_nezero          = uint32_nez( h_m );
  const uint32_t is_zero              = uint32_nor( is_e_nezero, is_m_nezero );
  const uint32_t is_denorm            = uint32_nor( is_zero, is_e_nezero );
  const uint32_t is_e_flagged         = uint32_eq( h_e, h_e_mask );
  const uint32_t is_inf               = uint32_andc( is_e_flagged, is_m_nezero );
  const uint32_t is_nan               = uint32_and( is_e_flagged,  is_m_nezero ); 
  const uint16_t h_m_nlz              = uint16_cntlz( h_m );
  const uint16_t h_denorm_sa          = uint32_add( h_m_nlz, h_f_denorm_sa_offset );
  const uint32_t h_f_denorm_m_sa      = uint32_sub( h_denorm_sa, two );
  const uint32_t h_f_denorm_m         = uint32_sll( h_m, h_f_denorm_m_sa );
  const uint32_t f_denorm_m           = uint32_and( h_f_denorm_m, f_m_mask );
  const uint32_t h_f_denorm_sa        = uint32_sub( one, h_denorm_sa );
  const uint32_t f_denorm_e           = uint32_add( h_f_denorm_sa, f_e_bias );
  const uint32_t f_denorm_e_packed    = uint32_sll( f_denorm_e, f_e_pos );
  const uint32_t f_denorm             = uint32_or3( f_s, f_denorm_e_packed, f_denorm_m );
  const uint32_t f_inf                = uint32_or( f_s,  f_e_mask );
  const uint32_t f_nan                = uint32_or3( f_s, f_e_mask, f_m );
  const uint32_t f_zero               = uint32_cp( f_s );
  const uint32_t f_e_packed           = uint32_sll( f_e, f_e_pos );
  const uint32_t packed               = uint32_or3( f_s, f_e_packed, f_m );
  const uint32_t check_zero_result    = uint32_mux( is_zero, f_zero, packed );
  const uint32_t check_denorm_result  = uint32_mux( is_denorm, f_denorm, check_zero_result );
  const uint32_t check_inf_result     = uint32_mux( is_inf, f_inf, check_denorm_result );
  const uint32_t check_nan_result     = uint32_mux( is_nan, f_nan, check_inf_result    );
  const uint32_t result               = check_nan_result;

  return (result);
}

uint16_t
half_from_float( uint32_t f )
{
  const uint16_t one                        = 0x0001;
  const uint32_t f_s_mask                   = 0x80000000;
  const uint32_t f_e_mask                   = 0x7f800000;
  const uint32_t f_m_mask                   = 0x007fffff;
  const uint32_t f_m_hidden_bit             = 0x00800000;
  const uint32_t f_m_round_bit              = 0x00001000;
  const uint16_t f_e_bias                   = 0x007f;
  const uint16_t h_e_bias                   = 0x000f;
  const uint16_t f_s_pos                    = 0x001f;
  const uint16_t h_s_pos                    = 0x000f;
  const uint16_t f_e_pos                    = 0x0017;
  const uint16_t h_e_pos                    = 0x000a;
  const uint16_t h_e_mask                   = 0x7c00;
  const uint16_t f_e_flagged_value          = 0x00ff;
  const uint16_t h_e_mask_value             = uint16_srl( h_e_mask, h_e_pos );
  const uint16_t h_e_max_value              = uint16_sub( h_e_mask_value, one );
  const uint16_t f_h_s_pos_offset           = uint16_sub( f_s_pos, h_s_pos );
  const uint16_t f_h_bias_offset            = uint16_sub( f_e_bias, h_e_bias );
  const uint16_t f_h_m_pos_offset           = uint16_sub( f_e_pos, h_e_pos );
  const uint16_t h_nan_min                  = uint16_or( h_e_mask, one );
  const uint32_t f_s_masked                 = uint32_and( f, f_s_mask );
  const uint32_t f_e_masked                 = uint32_and( f, f_e_mask );
  const uint16_t h_s                        = uint32_srl( f_s_masked, f_h_s_pos_offset );
  const uint16_t f_e                        = uint32_srl( f_e_masked, f_e_pos );
  const uint32_t f_m                        = uint32_and( f, f_m_mask );
  const uint16_t f_e_half_bias              = uint16_sub( f_e, f_h_bias_offset );
  const uint32_t f_m_round_mask             = uint32_and( f_m, f_m_round_bit );
  const uint32_t f_m_round_offset           = uint32_sll( f_m_round_mask, one );
  const uint32_t f_m_rounded                = uint32_add( f_m, f_m_round_offset );
  const uint32_t f_m_denorm_sa              = uint32_sub( one, f_e_half_bias );
  const uint32_t f_m_with_hidden            = uint32_or( f_m_rounded, f_m_hidden_bit );
  const uint32_t f_m_denorm                 = uint32_srl( f_m_with_hidden, f_m_denorm_sa );
  const uint16_t h_m_denorm                 = uint32_srl( f_m_denorm, f_h_m_pos_offset );
  const uint16_t h_denorm                   = uint16_or( h_s, h_m_denorm );
  const uint16_t h_inf                      = uint16_or( h_s, h_e_mask );
  const uint16_t m_nan                      = uint16_srl( f_m, f_h_m_pos_offset );
  const uint16_t h_nan                      = uint16_or3( h_s, h_e_mask, m_nan );
  const uint16_t h_nan_notinf               = uint16_or( h_s, h_nan_min );
  const uint16_t h_e_norm_overflow_offset   = uint16_add( f_e_half_bias, one );
  const uint16_t h_e_norm_overflow          = uint16_sll( h_e_norm_overflow_offset, h_e_pos );
  const uint16_t h_norm_overflow            = uint16_or( h_s, h_e_norm_overflow );
  const uint16_t h_e_norm                   = uint16_sll( f_e_half_bias, h_e_pos );
  const uint16_t h_m_norm                   = uint32_srl( f_m_rounded, f_h_m_pos_offset );
  const uint16_t h_norm                     = uint16_or3( h_s, h_e_norm, h_m_norm );
  const uint16_t h_is_denorm                = uint16_gte( f_h_bias_offset, f_e );
  const uint16_t f_h_e_biased_flag          = uint16_sub( f_e_flagged_value, f_h_bias_offset );
  const uint16_t f_e_is_flagged             = uint16_eq( f_e_half_bias, f_h_e_biased_flag );
  const uint16_t is_f_m_zero                = uint16_eqz( f_m );
  const uint16_t is_h_nan_zero              = uint16_eqz( m_nan );
  const uint16_t f_is_inf                   = uint16_and( f_e_is_flagged, is_f_m_zero );
  const uint16_t f_is_nan_underflow         = uint16_and( f_e_is_flagged, is_h_nan_zero );
  const uint16_t f_is_nan                   = uint16_cp( f_e_is_flagged );
  const uint16_t e_is_overflow              = uint16_gt( f_e_half_bias, h_e_max_value );
  const uint32_t f_m_rounded_overflow       = uint32_and( f_m_rounded, f_m_hidden_bit );
  const uint32_t m_norm_is_overflow         = uint32_nez( f_m_rounded_overflow );
  const uint16_t h_is_inf                   = uint16_or( e_is_overflow, f_is_inf );
  const uint16_t check_overflow_result      = uint16_mux( m_norm_is_overflow, h_norm_overflow, h_norm );
  const uint16_t check_nan_result           = uint16_mux( f_is_nan, h_nan, check_overflow_result );
  const uint16_t check_nan_underflow_result = uint16_mux( f_is_nan_underflow, h_nan_notinf,    check_nan_result );
  const uint16_t check_inf_result           = uint16_mux( h_is_inf, h_inf, check_nan_underflow_result );
  const uint16_t check_denorm_result        = uint16_mux( h_is_denorm, h_denorm, check_inf_result );
  const uint16_t result                     = uint16_cp( check_denorm_result );

  return (result);
}

uint16_t
half_add( uint16_t x, uint16_t y )
{
  const uint16_t one                  = 0x0001;
  const uint16_t h_s_mask             = 0x8000;
  const uint16_t h_e_mask             = 0x7c00;
  const uint16_t h_m_mask             = 0x03ff;
  const uint16_t h_m_msb_mask         = 0x2000;
  const uint16_t h_m_hidden           = 0x0400;
  const uint16_t h_e_pos              = 0x000a;
  const uint16_t h_e_bias             = 0x000f;
  const uint16_t h_m_grs_carry        = 0x4000;
  const uint16_t h_m_grs_carry_pos    = 0x000e;
  const uint16_t h_grs_size           = 0x0003;
  const uint16_t h_grs_round_carry    = uint16_sll( one, h_grs_size );
  const uint16_t h_grs_round_mask     = uint16_sub( h_grs_round_carry, one );
  const uint16_t x_s                  = uint16_and( x, h_s_mask );
  const uint16_t x_e                  = uint16_and( x, h_e_mask );
  const uint16_t x_m                  = uint16_and( x, h_m_mask );
  const uint16_t y_s                  = uint16_and( y, h_s_mask );
  const uint16_t y_e                  = uint16_and( y, h_e_mask );
  const uint16_t y_m                  = uint16_and( y, h_m_mask );
  const uint16_t is_y_e_larger        = uint16_gt( y_e, x_e );
  const uint16_t a_s                  = uint16_mux( is_y_e_larger, y_s, x_s );
  const uint16_t a_e                  = uint16_mux( is_y_e_larger, y_e, x_e );
  const uint16_t a_m_no_hidden_bit    = uint16_mux( is_y_e_larger, y_m, x_m );
  const uint16_t b_s                  = uint16_mux( is_y_e_larger, x_s, y_s );
  const uint16_t b_e                  = uint16_mux( is_y_e_larger, x_e, y_e );
  const uint16_t b_m_no_hidden_bit    = uint16_mux( is_y_e_larger, x_m, y_m );
  const uint16_t a_e_m_no_hidden_bit  = uint16_or( a_e, a_m_no_hidden_bit );
  const uint16_t a_is_inf             = uint16_eq( a_e_m_no_hidden_bit, h_e_mask );
  const uint16_t a_inf                = uint16_or( a_s, h_e_mask );
  const uint16_t a_e_nzero            = uint16_nez( a_e );
  const uint16_t b_e_nzero            = uint16_nez( b_e );
  const uint16_t a_m_hidden_bit       = uint16_and( a_e_nzero, h_m_hidden );
  const uint16_t b_m_hidden_bit       = uint16_and( b_e_nzero, h_m_hidden );
  const uint16_t a_m_no_grs           = uint16_or( a_m_no_hidden_bit, a_m_hidden_bit );
  const uint16_t b_m_no_grs           = uint16_or( b_m_no_hidden_bit, b_m_hidden_bit );
  const uint16_t a_e_unbias           = uint16_sub( a_e, h_e_bias );
  const uint16_t a_e_unbias_adj       = uint16_sub( a_e_unbias, one );
  const uint16_t m_sa_unbias          = uint16_srl( a_e_unbias_adj, h_e_pos );
  const uint16_t diff_e               = uint16_sub( a_e, b_e );
  const uint16_t m_sa_default         = uint16_srl( diff_e, h_e_pos );
  const uint16_t b_e_z_mask           = uint16_eqz( b_e );
  const uint16_t a_e_nez_mask         = uint16_nez( a_e );
  const uint16_t m_sa_unbias_mask     = uint16_and( b_e_z_mask, a_e_nez_mask );
  const uint16_t m_sa                 = uint16_mux( m_sa_unbias_mask, m_sa_unbias, m_sa_default );
  const uint16_t a_m                  = uint16_sll( a_m_no_grs, h_grs_size );
  const uint16_t b_m_no_sticky        = uint16_sll( b_m_no_grs, h_grs_size );
  const uint16_t sh_m                 = uint16_srl( b_m_no_sticky, m_sa );
  const uint16_t sticky_overflow      = uint16_sll( one, m_sa );
  const uint16_t sticky_mask          = uint16_sub( sticky_overflow, one );
  const uint16_t sticky_collect       = uint16_and( b_m_no_sticky, sticky_mask );
  const uint16_t sticky               = uint16_nez_p( sticky_collect );
  const uint16_t b_m                  = uint16_or( sh_m, sticky );
  const uint16_t a_e_biased           = uint16_srl( a_e, h_e_pos );
  const uint16_t sel_sign             = uint16_xor( a_s, b_s );
  const uint16_t is_diff_sign         = uint16_nez( sel_sign );
  const uint16_t z_m_sum              = uint16_add( a_m, b_m );
  const uint16_t z_m_diff_ab          = uint16_sub( a_m, b_m );
  const uint16_t z_m_diff_ba          = uint16_sub( b_m, a_m );
  const uint16_t is_z_m_ab_pos        = uint16_gt( a_m, b_m );
  const uint16_t z_m_smag_diff        = uint16_mux( is_z_m_ab_pos, z_m_diff_ab, z_m_diff_ba );
  const uint16_t z_s_diff             = uint16_mux( is_z_m_ab_pos, a_s, b_s );
  const uint16_t z_s                  = uint16_mux( is_diff_sign, z_s_diff, a_s );
  const uint16_t z_m_smag_diff_nlz    = uint16_cntlz( z_m_smag_diff );
  const uint16_t diff_norm_sa         = uint16_sub( z_m_smag_diff_nlz, one );
  const uint16_t is_diff_denorm       = uint16_lt( a_e_biased, diff_norm_sa );
  const uint16_t diff_denorm_sa       = uint16_sub( a_e_biased, one );
  const uint16_t z_m_diff_denorm      = uint16_sll( z_m_smag_diff, diff_denorm_sa );
  const uint16_t z_m_diff_norm        = uint16_sll( z_m_smag_diff, diff_norm_sa );
  const uint16_t c_e_diff_norm        = uint16_sub( a_e_biased,  diff_norm_sa );
  const uint16_t is_a_or_b_norm       = uint16_nez( a_e_biased );
  const uint16_t z_m_diff_ab_norm     = uint16_mux( is_diff_denorm, z_m_diff_denorm, z_m_diff_norm );
  const uint16_t c_e_diff_ab_norm     = uint16_andc( c_e_diff_norm, is_diff_denorm );
  const uint16_t z_m_diff_ab_denorm   = uint16_cp( z_m_smag_diff );
  const uint16_t c_e_diff_ab_denorm   = uint16_cp( a_e_biased );
  const uint16_t z_m_diff             = uint16_mux( is_a_or_b_norm, z_m_diff_ab_norm, z_m_diff_ab_denorm );
  const uint16_t c_e_diff             = uint16_mux( is_a_or_b_norm, c_e_diff_ab_norm, c_e_diff_ab_denorm );
  const uint16_t is_diff_zero         = uint16_eqz( z_m_diff );
  const uint16_t is_diff_exactly_zero = uint16_and( is_diff_sign, is_diff_zero );
  const uint16_t z_m_added            = uint16_mux( is_diff_sign, z_m_diff, z_m_sum );
  const uint16_t c_e_added            = uint16_mux( is_diff_sign, c_e_diff, a_e_biased );
  const uint16_t z_m_carry            = uint16_and( z_m_added, h_m_grs_carry );
  const uint16_t is_z_m_carry         = uint16_nez( z_m_carry );
  const uint16_t c_e_hidden_offset    = uint16_andsrl( z_m_added, h_m_grs_carry, h_m_grs_carry_pos);
  const uint16_t z_m_sub_hidden       = uint16_srl( z_m_added, one );
  const uint16_t z_m_no_hidden        = uint16_mux( is_z_m_carry, z_m_sub_hidden, z_m_added );
  const uint16_t c_e_no_hidden        = uint16_add( c_e_added, c_e_hidden_offset );
  const uint16_t is_undenorm          = uint16_eqz_p( a_e );
  const uint16_t z_m_no_hidden_msb    = uint16_and( z_m_no_hidden, h_m_msb_mask );
  const uint16_t undenorm_m_msb_odd   = uint16_nez_p( z_m_no_hidden_msb );
  const uint16_t undenorm_fix_e       = uint16_and( is_undenorm, undenorm_m_msb_odd );
  const uint16_t c_e_fixed            = uint16_add( c_e_no_hidden, undenorm_fix_e );
  const uint16_t z_m_round_amount     = uint16_and( z_m_no_hidden, h_grs_round_mask );
  const uint16_t z_m_rounded          = uint16_add( z_m_no_hidden, z_m_round_amount );
  const uint16_t z_m_round_overflow   = uint16_andsrl( z_m_rounded, h_m_grs_carry, h_m_grs_carry_pos );
  const uint16_t c_e_rounded          = uint16_add( c_e_fixed, z_m_round_overflow );
  const uint16_t z_m_no_grs           = uint16_srlm( z_m_rounded, h_grs_size,  h_m_mask );
  const uint16_t z_e                  = uint16_sll( c_e_rounded, h_e_pos );
  const uint16_t z_packed             = uint16_or3( z_s, z_e, z_m_no_grs );
  const uint16_t check_inf_result     = uint16_mux( a_is_inf, a_inf, z_packed );
  const uint16_t check_zero_result    = uint16_andc( check_inf_result, is_diff_exactly_zero );
  const uint16_t result               = uint16_cp( check_zero_result );

  return (result);
}
