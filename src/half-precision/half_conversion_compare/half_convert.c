// Branch-free implementation of half-precision (16 bit) floating point
// 
// Copyright 2006 Mike Acton <macton@gmail.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE
// 
//
// Half-precision floating point format
// ------------------------------------
//
//   | Field    | Last | First | Note
//   |----------|------|-------|----------
//   | Sign     | 15   | 15    |
//   | Exponent | 14   | 10    | Bias = 15
//   | Mantissa | 9    | 0     |
//
// Compiling
// ---------------------------
//
//  Preferred compile flags for GCC: 
//     -O3 -fstrict-aliasing -std=c99 -pedantic -Wall -Wstrict-aliasing
//
//     This file is a C99 source file, intended to be compiled with a C99 
//     compliant compiler. However, for the moment it remains combatible
//     with C++98. Therefore if you are using a compiler that poorly implements
//     C standards (e.g. MSVC), it may be compiled as C++. This is not
//     guaranteed for future versions. 
//

#include <stdint.h>

static inline uint32_t _uint32_li( uint32_t a )                                  __attribute__((always_inline));
static inline uint32_t _uint32_dec( uint32_t a )                                 __attribute__((always_inline));
static inline uint32_t _uint32_neg( uint32_t a )                                 __attribute__((always_inline));
static inline uint32_t _uint32_ext( uint32_t a )                                 __attribute__((always_inline));
static inline uint32_t _uint32_cntlz( uint32_t x )                               __attribute__((always_inline));
static inline uint32_t _uint32_and( uint32_t a, uint32_t b )                     __attribute__((always_inline));
static inline uint32_t _uint32_andc( uint32_t a, uint32_t b )                    __attribute__((always_inline));
static inline uint32_t _uint32_or( uint32_t a, uint32_t b )                      __attribute__((always_inline));
static inline uint32_t _uint32_srl( uint32_t a, int sa )                         __attribute__((always_inline));
static inline uint32_t _uint32_sll( uint32_t a, int sa )                         __attribute__((always_inline));
static inline uint32_t _uint32_add( uint32_t a, uint32_t b )                     __attribute__((always_inline));
static inline uint32_t _uint32_sub( uint32_t a, uint32_t b )                     __attribute__((always_inline));
static inline uint32_t _uint32_muxs( uint32_t test, uint32_t a, uint32_t b )     __attribute__((always_inline));
static inline uint16_t _half_from_float( uint32_t f )                            __attribute__((always_inline));
static inline uint32_t _half_to_float( uint16_t h )                              __attribute__((always_inline));

static inline uint32_t _uint32_li( uint32_t a )
{
  return (a);
}

static inline uint32_t _uint32_dec( uint32_t a )
{
  return (a - 1);
}

static inline uint32_t _uint32_neg( uint32_t a )
{
  return (-a);
}

static inline uint32_t _uint32_ext( uint32_t a )
{
  return (((int32_t)a)>>31);
}

static inline uint32_t _uint32_cntlz( uint32_t x )
{
  /* On PowerPC, this will map to: cntlzw */
  /* On Pentium, this will map to: clz    */
  uint32_t nlz = __builtin_clz( x );

  return (nlz);
}

static inline uint32_t _uint32_and( uint32_t a, uint32_t b )
{
  return (a & b);
}

static inline uint32_t _uint32_andc( uint32_t a, uint32_t b )
{
  return (a & ~b);
}

static inline uint32_t _uint32_or( uint32_t a, uint32_t b )
{
  return (a | b);
}

static inline uint32_t _uint32_srl( uint32_t a, int sa )
{
  return (a >> sa);
}

static inline uint32_t _uint32_sll( uint32_t a, int sa )
{
  return (a << sa);
}

static inline uint32_t _uint32_add( uint32_t a, uint32_t b )
{
  return (a + b);
}

static inline uint32_t _uint32_sub( uint32_t a, uint32_t b )
{
  return (a - b);
}

static inline uint32_t _uint32_muxs( uint32_t test, uint32_t a, uint32_t b )
{
  const uint32_t mask   = _uint32_ext( test );
  const uint32_t sel_a  = _uint32_and(  a,     mask  );
  const uint32_t sel_b  = _uint32_andc( b,     mask  );
  const uint32_t result = _uint32_or(   sel_a, sel_b );
  return (result);
}

static inline uint16_t _half_from_float( uint32_t f )
{
  const uint32_t one                        = _uint32_li( 0x00000001 );
  const uint32_t f_e_mask                   = _uint32_li( 0x7f800000 );
  const uint32_t f_m_mask                   = _uint32_li( 0x007fffff );
  const uint32_t f_s_mask                   = _uint32_li( 0x80000000 );
  const uint32_t h_e_mask                   = _uint32_li( 0x00007c00 );
  const uint32_t f_e_pos                    = _uint32_li( 0x00000017 );
  const uint32_t f_e_bias                   = _uint32_li( 0x0000007f );
  const uint32_t h_e_bias                   = _uint32_li( 0x0000000f );
  const uint32_t f_m_round_bit              = _uint32_li( 0x00001000 );
  const uint32_t h_nan_em_min               = _uint32_li( 0x00007c01 );   
  const uint32_t f_h_s_pos_offset           = _uint32_li( 0x00000010 );
  const uint32_t f_m_hidden_bit             = _uint32_li( 0x00800000 );
  const uint32_t f_h_m_pos_offset           = _uint32_li( 0x0000000d );

  const uint32_t f_e                        = _uint32_and( f, f_e_mask );
  const uint32_t f_m                        = _uint32_and( f, f_m_mask );
  const uint32_t f_s                        = _uint32_and( f, f_s_mask );

  const uint32_t f_h_bias_offset_amount     = _uint32_sub( f_e_bias,               h_e_bias        );
  const uint32_t f_h_bias_offset            = _uint32_sll( f_h_bias_offset_amount, f_e_pos         );
  const uint32_t f_e_h_bias                 = _uint32_sub( f_e,                    f_h_bias_offset );
  const uint32_t f_e_h_bias_amount          = _uint32_srl( f_e_h_bias,             f_e_pos         );

  const uint32_t f_m_round_mask             = _uint32_and( f_m,               f_m_round_bit     );
  const uint32_t f_m_round_offset           = _uint32_sll( f_m_round_mask,    one               );
  const uint32_t f_m_rounded                = _uint32_add( f_m,               f_m_round_offset  );
  const uint32_t f_m_denorm_sa              = _uint32_sub( one,               f_e_h_bias_amount );
  const uint32_t f_m_with_hidden            = _uint32_or(  f_m_rounded,       f_m_hidden_bit    );
  const uint32_t f_m_denorm                 = _uint32_srl( f_m_with_hidden,   f_m_denorm_sa     );
  const uint32_t f_em_norm_packed           = _uint32_or(  f_e_h_bias,        f_m_rounded       );

  const uint32_t h_s                        = _uint32_srl( f_s,               f_h_s_pos_offset );
  const uint32_t h_m_nan                    = _uint32_srl( f_m,               f_h_m_pos_offset );
  const uint32_t h_m_denorm                 = _uint32_srl( f_m_denorm,        f_h_m_pos_offset );
  const uint32_t h_em_norm                  = _uint32_srl( f_em_norm_packed,  f_h_m_pos_offset );

  const uint32_t is_e_eqz_msb               = _uint32_dec( f_e     );
  const uint32_t is_m_nez_msb               = _uint32_neg( f_m     );
  const uint32_t is_h_m_nan_nez_msb         = _uint32_neg( h_m_nan );
  const uint32_t is_e_nflagged_msb          = _uint32_sub( f_e,                f_e_mask          );
  const uint32_t is_ninf_msb                = _uint32_or(  is_e_nflagged_msb,  is_m_nez_msb      );
  const uint32_t is_underflow_msb           = _uint32_sub( is_e_eqz_msb,       f_h_bias_offset   );
  const uint32_t is_nan_nunderflow_msb      = _uint32_or(  is_h_m_nan_nez_msb, is_e_nflagged_msb );

  const uint32_t h_nan_underflow_result     = _uint32_muxs( is_nan_nunderflow_msb, h_em_norm,                h_nan_em_min     );
  const uint32_t h_inf_result               = _uint32_muxs( is_ninf_msb,           h_nan_underflow_result,   h_e_mask         );
  const uint32_t h_em_result                = _uint32_muxs( is_underflow_msb,      h_m_denorm,               h_inf_result     );
  const uint32_t h_result                   = _uint32_or( h_em_result, h_s );

  return (h_result);
}

static inline uint32_t _half_to_float( uint16_t h )
{
  const uint32_t h_e_mask              = _uint32_li( 0x00007c00 );
  const uint32_t h_m_mask              = _uint32_li( 0x000003ff );
  const uint32_t h_s_mask              = _uint32_li( 0x00008000 );
  const uint32_t h_f_s_pos_offset      = _uint32_li( 0x00000010 );
  const uint32_t h_f_e_pos_offset      = _uint32_li( 0x0000000d );
  const uint32_t h_f_bias_offset       = _uint32_li( 0x0001c000 );
  const uint32_t f_e_mask              = _uint32_li( 0x7f800000 );
  const uint32_t f_m_mask              = _uint32_li( 0x007fffff );
  const uint32_t h_f_e_denorm_bias     = _uint32_li( 0x0000007e );
  const uint32_t h_f_m_denorm_sa_bias  = _uint32_li( 0x00000008 );
  const uint32_t f_e_pos               = _uint32_li( 0x00000017 );

  const uint32_t h_e                   = _uint32_and( h,          h_e_mask         );
  const uint32_t h_m                   = _uint32_and( h,          h_m_mask         );
  const uint32_t h_s                   = _uint32_and( h,          h_s_mask         );

  const uint32_t h_e_f_bias            = _uint32_add( h_e, h_f_bias_offset );
  const uint32_t h_m_nlz               = _uint32_cntlz( h_m );

  const uint32_t f_s                   = _uint32_sll( h_s,        h_f_s_pos_offset );
  const uint32_t f_e                   = _uint32_sll( h_e_f_bias, h_f_e_pos_offset );
  const uint32_t f_m                   = _uint32_sll( h_m,        h_f_e_pos_offset );

  const uint32_t h_f_m_sa              = _uint32_sub( h_m_nlz,             h_f_m_denorm_sa_bias );
  const uint32_t f_e_denorm_unpacked   = _uint32_sub( h_f_e_denorm_bias,   h_f_m_sa             );
  const uint32_t h_f_m                 = _uint32_sll( h_m,                 h_f_m_sa             );
  const uint32_t f_m_denorm            = _uint32_and( h_f_m,               f_m_mask             );
  const uint32_t f_e_denorm            = _uint32_sll( f_e_denorm_unpacked, f_e_pos              );
  const uint32_t f_em_denorm           = _uint32_or(  f_e_denorm,          f_m_denorm           );

  const uint32_t is_e_eqz_msb          = _uint32_dec(  h_e );
  const uint32_t is_m_nez_msb          = _uint32_neg(  h_m );
  const uint32_t is_e_flagged_msb      = _uint32_sub(  h_e_mask,     is_e_eqz_msb     );
  const uint32_t is_denorm_msb         = _uint32_and(  is_e_eqz_msb, is_m_nez_msb     );
  const uint32_t is_ninf_msb           = _uint32_andc( is_m_nez_msb, is_e_flagged_msb );
  const uint32_t is_zero_msb           = _uint32_andc( is_e_eqz_msb, is_m_nez_msb     );
  const uint32_t is_ninf               = _uint32_ext(  is_ninf_msb );
  const uint32_t is_zero               = _uint32_ext(  is_zero_msb );

  const uint32_t f_e_flagged_result    = _uint32_muxs( is_e_flagged_msb, f_e_mask, f_e );
  const uint32_t f_m_inf_result        = _uint32_and( f_m, is_ninf );
  const uint32_t f_em_result_partial   = _uint32_or( f_e_flagged_result, f_m_inf_result );
  const uint32_t f_em_denorm_result    = _uint32_muxs( is_denorm_msb, f_em_denorm, f_em_result_partial );
  const uint32_t f_em_result           = _uint32_andc( f_em_denorm_result, is_zero );
  const uint32_t f_result              = _uint32_or( f_s, f_em_result );
 
  return (f_result);
}

uint16_t 
half_from_float( uint32_t f )
{
  return _half_from_float(f);
}

uint32_t 
half_to_float( uint16_t h )
{
  return _half_to_float(h);
}

uint32_t 
half_from_float_x2( uint64_t f2 )
{
  uint32_t f0     = (uint32_t)(f2);
  uint32_t f1     = (uint32_t)(f2 >> 32);
  uint32_t h0     = (uint64_t)_half_from_float(f0);
  uint32_t h1     = (uint64_t)_half_from_float(f1);
  uint32_t result = h0 | ( h1 << 16 );

  return (result);
}

uint64_t 
half_to_float_x2( uint32_t h2 )
{
  uint16_t h0     = (uint16_t)(h2);
  uint16_t h1     = (uint16_t)(h2 >> 16);
  uint64_t f0     = (uint64_t)_half_to_float(h0);
  uint64_t f1     = (uint64_t)_half_to_float(h1);
  uint64_t result = f0 | ( f1 << 32 );

  return (result);
}
