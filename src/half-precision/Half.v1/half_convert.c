///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

// Primary authors:
//     Florian Kainz <kainz@ilm.com>
//     Rod Bogart <rgb@ilm.com>

// Modifications:
//     Mike Acton <macton@gmail.com>
//         - Converted to C99
//         - Converted to use stdint.h
//         - Separated out just the float to half conversion function

#include <stdint.h>
#include <stdio.h>
#include <altivec.h>

//-----------------------------------------------------
// Force hardware float overflow exception
//-----------------------------------------------------

// Modifications:
//     Mike Acton <macton@gmail.com>
//         - Converted to inline

static inline float half_overflow( void )
{
    volatile float f = 1e10;

    for (int i = 0; i < 10; i++)    
        f *= f;     // this will overflow before
                    // the for­loop terminates
    return f;
}


//-----------------------------------------------------
// Float-to-half conversion -- general case, including
// zeroes, denormalized numbers and exponent overflows.
//-----------------------------------------------------

// Modifications:
//     Mike Acton <macton@gmail.com>
//         - Removed register keyword
//         - Modified to keep sign on convert from zero

uint16_t
half_convert_v0( int32_t i )
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

    int32_t s =  (i >> 16) & 0x00008000;
    int32_t e = ((i >> 23) & 0x000000ff) - (127 - 15);
    int32_t m =   i        & 0x007fffff;

    //
    // Now reassemble s, e and m into a half:
    //

    if (e <= 0)
    {
        if (e < -10)
        {
            //
            // E is less than -10.  The absolute value of f is
            // less than HALF_MIN (f may be a small normalized
            // float, a denormalized float or a zero).
            //
            // We convert f to a half zero.
            //
    
            return s;
        }

        //
        // E is between -10 and 0.  F is a normalized float,
        // whose magnitude is less than HALF_NRM_MIN.
        //
        // We convert f to a denormalized half.
        // 

        // Mike Acton <macton@gmail.com>:
        // Calculating denormalized value requires shift by variable amount.
        // This is microcoded on the PPU, so note that this operation is
        // extremely expensive. 
        //

        m = (m | 0x00800000) >> (1 - e);

        //
        // Round to nearest, round "0.5" up.
        //
        // Rounding may cause the significand to overflow and make
        // our number normalized.  Because of the way a half's bits
        // are laid out, we don't have to treat this case separately;
        // the code below will handle it correctly.
        // 

        if (m &  0x00001000)
            m += 0x00002000;

        //
        // Assemble the half from s, e (zero) and m.
        //

        return s | (m >> 13);
    }
    else if (e == 0xff - (127 - 15))
    {
        if (m == 0)
        {
            //
            // F is an infinity; convert f to a half
            // infinity with the same sign as f.
            //
    
            return s | 0x7c00;
        }
        else
        {
            //
            // F is a NAN; we produce a half NAN that preserves
            // the sign bit and the 10 leftmost bits of the
            // significand of f, with one exception: If the 10
            // leftmost bits are all zero, the NAN would turn 
            // into an infinity, so we have to set at least one
            // bit in the significand.
            //
    
            m >>= 13;
            return s | 0x7c00 | m | (m == 0);
        }
    }
    else
    {
        //
        // E is greater than zero.  F is a normalized float.
        // We try to convert f to a normalized half.
        //

        //
        // Round to nearest, round "0.5" up
        //

        if (m &  0x00001000)
        {
            m += 0x00002000;

            if (m & 0x00800000)
            {
                m =  0;        // overflow in significand,
                e += 1;        // adjust exponent
            }
        }

        //
        // Handle exponent overflow
        //
    
        if (e > 30)
        {
            half_overflow();    // Cause a hardware floating point overflow;
            return s | 0x7c00;  // if this returns, the half becomes an
        }                       // infinity with the same sign as f.
    
        //
        // Assemble the half from s, e and m.
        //
    
        return s | (e << 10) | (m >> 13);
    }
}

//-----------------------------------------------------
// Float-to-half conversion -- general case, including
// zeroes, denormalized numbers and exponent overflows.
//-----------------------------------------------------

// Modifications:
//     Mike Acton <macton@gmail.com>
//         - Flattened branch hierarchy

uint16_t
half_convert_v1( int32_t i )
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

    const int32_t s                 =  (i >> 16) & 0x00008000;
    const int32_t e                 = ((i >> 23) & 0x000000ff) - (127 - 15);
    const int32_t m                 =   i        & 0x007fffff;

    //
    // Round to nearest, round "0.5" up.
    //
    // Rounding may cause the significand to overflow and make
    // our number normalized.  Because of the way a half's bits
    // are laid out, we don't have to treat this case separately;
    // the code below will handle it correctly.
    // 

    const int32_t m_rounded         = m + ( (m & 0x00001000) << 1 );

    //
    // Calculating denormalized value requires shift by variable amount.
    // This is microcoded on the PPU, so note that this operation is
    // extremely expensive. 
    //
    const int32_t m_denorm          = (m_rounded | 0x00800000) >> (1 - e);

    const int32_t h_denorm          = s | (m_denorm >> 13);
    const int32_t h_zero            = s;
    const int32_t h_inf             = s | 0x7c00;
    const int32_t m_nan             = m >> 13;
    const int32_t h_nan             = s | 0x7c00 | m_nan;
    const int32_t h_nan_notinf      = h_nan | 0x01;
    const int32_t h_norm_overflow   = s | ((e+1) << 10);
    const int32_t h_norm            = s | (e << 10) | ( m_rounded >> 13 );

    //
    // E is less than -10.  The absolute value of f is
    // less than HALF_MIN (f may be a small normalized
    // float, a denormalized float or a zero).
    //
    // We convert f to a half zero.
    //
    
    if (e < -10)
    {
        return h_zero;
    }

    //
    // E is between -10 and 0.  F is a normalized float,
    // whose magnitude is less than HALF_NRM_MIN.
    //
    // We convert f to a denormalized half.
    // 

    if (e <= 0)
    {
        return h_denorm;
    }

    //
    // F is an infinity; convert f to a half
    // infinity with the same sign as f.
    //

    if ( (e == 0xff - (127 - 15)) && ( m == 0 ) )
    {
        return h_inf;
    }

    //
    // F is a NAN; we produce a half NAN that preserves
    // the sign bit and the 10 leftmost bits of the
    // significand of f, with one exception: If the 10
    // leftmost bits are all zero, the NAN would turn 
    // into an infinity, so we have to set at least one
    // bit in the significand.
    //

    if ( (e == 0xff - (127 - 15)) && ( m_nan == 0 ) )
    {
        return h_nan_notinf;
    }

    if (e == 0xff - (127 - 15))
    {
        return h_nan;
    }

    //
    // Handle exponent overflow
    // Cause a hardware floating point overflow;
    // if this returns, the half becomes an
    // infinity with the same sign as f.
    //

    if (e > 30)
    {
        half_overflow();    
        return h_inf;       
    }                       

    //
    // E is greater than zero.  F is a normalized float.
    // We try to convert f to a normalized half.
    //

    //
    // Overflow in significand,
    // Adjust exponent
    //
    if (m & 0x00800000)
    {
        return h_norm_overflow;
    }

    return h_norm;
}

//-----------------------------------------------------
// Float-to-half conversion -- general case, including
// zeroes, denormalized numbers and exponent overflows.
//-----------------------------------------------------

// Modifications:
//     Mike Acton <macton@gmail.com>
//         - Combine comparisons

uint16_t
half_convert_v2( int32_t i )
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

    const int32_t s                   =  (i >> 16) & 0x00008000;
    const int32_t e                   = ((i >> 23) & 0x000000ff) - (127 - 15);
    const int32_t m                   =   i        & 0x007fffff;

    //
    // Round to nearest, round "0.5" up.
    //
    // Rounding may cause the significand to overflow and make
    // our number normalized.  Because of the way a half's bits
    // are laid out, we don't have to treat this case separately;
    // the code below will handle it correctly.
    // 

    const int32_t m_rounded           = m + ( (m & 0x00001000) << 1 );

    //
    // Calculating denormalized value requires shift by variable amount.
    // This is microcoded on the PPU, so note that this operation is
    // extremely expensive. 
    //
    const int32_t m_denorm            = (m_rounded | 0x00800000) >> (1 - e);

    const int32_t h_denorm            = s | (m_denorm >> 13);
    const int32_t h_zero              = s;
    const int32_t h_inf               = s | 0x7c00;
    const int32_t m_nan               = m >> 13;
    const int32_t h_nan               = s | 0x7c00 | m_nan;
    const int32_t h_nan_notinf        = h_nan | 0x01;
    const int32_t h_norm_overflow     = s | ((e+1) << 10);
    const int32_t h_norm              = s | (e << 10) | ( m_rounded >> 13 );

    //
    // E is less than -10.  The absolute value of f is
    // less than HALF_MIN (f may be a small normalized
    // float, a denormalized float or a zero).
    //
    // We convert f to a half zero.
    //
    const int     f_is_too_small      = (e < -10);

    //
    // E is between -10 and 0.  F is a normalized float,
    // whose magnitude is less than HALF_NRM_MIN.
    //
    // We convert f to a denormalized half.
    // 
    const int     f_is_small          = (e <= 0);
    

    //
    // E can be flagged for either NAN or INF.
    //
    const int     e_is_flagged        = (e == 0xff - (127 - 15));

    //
    // F is an infinity; convert f to a half
    // infinity with the same sign as f.
    //
    const int     f_is_inf            = e_is_flagged && ( m == 0 );

    //
    // F is a NAN; we produce a half NAN that preserves
    // the sign bit and the 10 leftmost bits of the
    // significand of f, with one exception: If the 10
    // leftmost bits are all zero, the NAN would turn 
    // into an infinity, so we have to set at least one
    // bit in the significand.
    //
    const int     f_is_nan_inf        = e_is_flagged && ( m_nan == 0 );
    const int     f_is_nan            = e_is_flagged;

    //
    // Handle exponent overflow
    // Cause a hardware floating point overflow;
    // if this returns, the half becomes an
    // infinity with the same sign as f.
    //
    const int     e_is_overflow       = (e > 30);
 
    //
    // Overflow in significand,
    // Adjust exponent
    //
    const int     m_norm_is_overflow  = m_rounded & 0x00800000;

    // 
    // Select correct result
    //

    if ( f_is_too_small )
    {
        return h_zero;
    }

    if ( f_is_small )
    {
        return h_denorm;
    }

    if ( f_is_inf )
    {
        return h_inf;
    }

    if ( f_is_nan_inf )
    {
        return h_nan_notinf;
    }

    if ( f_is_nan )
    {
        return h_nan;
    }

    //
    // Cause a hardware floating point overflow;
    // if this returns, the half becomes an
    // infinity with the same sign as f.
    //

    if ( e_is_overflow )
    {
        half_overflow();    
        return h_inf;       
    }                       

    if ( m_norm_is_overflow )
    {
        return h_norm_overflow;
    }

    //
    // E is greater than zero.  F is a normalized float.
    // We try to convert f to a normalized half.
    //

    return h_norm;
}

//-----------------------------------------------------
// Float-to-half conversion -- general case, including
// zeroes, denormalized numbers and exponent overflows.
//-----------------------------------------------------

// Modifications:
//     Mike Acton <macton@gmail.com>
//         - Select result based on predicates

static inline int32_t pred_s32_sel( int pred, int32_t a, int32_t b )
{
  return ( ( pred ) ? a : b );
}

uint16_t
half_convert_v3( int32_t i )
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

    const int32_t s                   =  (i >> 16) & 0x00008000;
    const int32_t e                   = ((i >> 23) & 0x000000ff) - (127 - 15);
    const int32_t m                   =   i        & 0x007fffff;

    //
    // Round to nearest, round "0.5" up.
    //
    // Rounding may cause the significand to overflow and make
    // our number normalized.  Because of the way a half's bits
    // are laid out, we don't have to treat this case separately;
    // the code below will handle it correctly.
    // 

    const int32_t m_rounded           = m + ( (m & 0x00001000) << 1 );

    //
    // Calculating denormalized value requires shift by variable amount.
    // This is microcoded on the PPU, so note that this operation is
    // extremely expensive. 
    //
    const int32_t m_denorm            = (m_rounded | 0x00800000) >> (1 - e);

    const int32_t h_denorm            = s | (m_denorm >> 13);
    const int32_t h_zero              = s;
    const int32_t h_inf               = s | 0x7c00;
    const int32_t m_nan               = m >> 13;
    const int32_t h_nan               = s | 0x7c00 | m_nan;
    const int32_t h_nan_notinf        = h_nan | 0x01;
    const int32_t h_norm_overflow     = s | ((e+1) << 10);
    const int32_t h_norm              = s | (e << 10) | ( m_rounded >> 13 );

    //
    // E is less than -10.  The absolute value of f is
    // less than HALF_MIN (f may be a small normalized
    // float, a denormalized float or a zero).
    //
    // We convert f to a half zero.
    //
    const int     f_is_too_small      = (e < -10);

    //
    // E is between -10 and 0.  F is a normalized float,
    // whose magnitude is less than HALF_NRM_MIN.
    //
    // We convert f to a denormalized half.
    // 
    const int     f_is_small          = (e <= 0);
    

    //
    // E can be flagged for either NAN or INF.
    //
    const int     e_is_flagged        = (e == 0xff - (127 - 15));

    //
    // F is an infinity; convert f to a half
    // infinity with the same sign as f.
    //
    const int     f_is_inf            = e_is_flagged && ( m == 0 );

    //
    // F is a NAN; we produce a half NAN that preserves
    // the sign bit and the 10 leftmost bits of the
    // significand of f, with one exception: If the 10
    // leftmost bits are all zero, the NAN would turn 
    // into an infinity, so we have to set at least one
    // bit in the significand.
    //
    const int     f_is_nan_inf        = e_is_flagged && ( m_nan == 0 );
    const int     f_is_nan            = e_is_flagged;

    //
    // Handle exponent overflow
    // Cause a hardware floating point overflow;
    // if this returns, the half becomes an
    // infinity with the same sign as f.
    //
    const int     e_is_overflow       = (e > 30);
 
    //
    // Overflow in significand,
    // Adjust exponent
    //
    const int     m_norm_is_overflow  = m_rounded & 0x00800000;

    // 
    // Select correct result
    // NOTE: Order of tests is reversed from previous versions
    //       because rather than returning on each case, the value
    //       is being overidden.
    //

    //
    // Default:
    // E is greater than zero.  F is a normalized float.
    // We try to convert f to a normalized half.
    //


    const int32_t result_0 = h_norm;
    const int32_t result_1 = pred_s32_sel( m_norm_is_overflow, h_norm_overflow, result_0 );
    const int32_t result_2 = pred_s32_sel( e_is_overflow,      h_inf,           result_1 );
    const int32_t result_3 = pred_s32_sel( f_is_nan,           h_nan,           result_2 );
    const int32_t result_4 = pred_s32_sel( f_is_nan_inf,       h_nan_notinf,    result_3 );
    const int32_t result_5 = pred_s32_sel( f_is_inf,           h_inf,           result_4 );
    const int32_t result_6 = pred_s32_sel( f_is_small,         h_denorm,        result_5 );
    const int32_t result_7 = pred_s32_sel( f_is_too_small,     h_zero,          result_6 );

    //
    // Cause a hardware floating point overflow;
    // if this returns, the half becomes an
    // infinity with the same sign as f.
    //

    if ( e_is_overflow )
    {
        half_overflow();    
    }                       

    return (result_7);
}

//-----------------------------------------------------
// Float-to-half conversion -- general case, including
// zeroes, denormalized numbers and exponent overflows.
//-----------------------------------------------------

// Modifications:
//     Mike Acton <macton@gmail.com>
//         - Force comparisons to use predicate values so
//           we can use bit operations instead of boolean
//           operations.
//         - Combine two cases that create half INF.

static inline int32_t pred_s32_gt( int32_t a, int32_t b )
{
  return ( ( a > b ) ? 1 : 0 );
}

static inline int32_t pred_s32_lt( int32_t a, int32_t b )
{
  return ( ( a < b ) ? 1 : 0 );
}

static inline int32_t pred_s32_eq( int32_t a, int32_t b )
{
  return ( ( a == b ) ? 1 : 0 );
}

static inline int32_t pred_s32_nz( int32_t a )
{
  return ( ( a != 0 ) ? 1 : 0 );
}

uint16_t
half_convert_v4( int32_t i )
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

    const int32_t s                   =  (i >> 16) & 0x00008000;
    const int32_t e                   = ((i >> 23) & 0x000000ff) - (127 - 15);
    const int32_t m                   =   i        & 0x007fffff;

    //
    // Round to nearest, round "0.5" up.
    //
    // Rounding may cause the significand to overflow and make
    // our number normalized.  Because of the way a half's bits
    // are laid out, we don't have to treat this case separately;
    // the code below will handle it correctly.
    // 

    const int32_t m_rounded           = m + ( (m & 0x00001000) << 1 );

    //
    // Calculating denormalized value requires shift by variable amount.
    // This is microcoded on the PPU, so note that this operation is
    // extremely expensive. 
    //
    const int32_t m_denorm            = (m_rounded | 0x00800000) >> (1 - e);

    const int32_t h_denorm            = s | (m_denorm >> 13);
    const int32_t h_zero              = s;
    const int32_t h_inf               = s | 0x7c00;
    const int32_t m_nan               = m >> 13;
    const int32_t h_nan               = s | 0x7c00 | m_nan;
    const int32_t h_nan_notinf        = h_nan | 0x01;
    const int32_t h_norm_overflow     = s | ((e+1) << 10);
    const int32_t h_norm              = s | (e << 10) | ( m_rounded >> 13 );

    //
    // E is less than -10.  The absolute value of f is
    // less than HALF_MIN (f may be a small normalized
    // float, a denormalized float or a zero).
    //
    // We convert f to a half zero.
    //
    const int     f_is_too_small      = pred_s32_lt( e, -10 );

    //
    // E is between -10 and 0.  F is a normalized float,
    // whose magnitude is less than HALF_NRM_MIN.
    //
    // We convert f to a denormalized half.
    // 
    const int     f_is_small          = pred_s32_lt( e, 1 );
    

    //
    // E can be flagged for either NAN or INF.
    //
    const int     e_is_flagged        = pred_s32_eq( e,  0xff - (127 - 15) );

    //
    // F is an infinity; convert f to a half
    // infinity with the same sign as f.
    //
    const int     f_is_inf            = e_is_flagged & pred_s32_eq( m, 0 );

    //
    // F is a NAN; we produce a half NAN that preserves
    // the sign bit and the 10 leftmost bits of the
    // significand of f, with one exception: If the 10
    // leftmost bits are all zero, the NAN would turn 
    // into an infinity, so we have to set at least one
    // bit in the significand.
    //
    const int     f_is_nan_inf        = e_is_flagged & pred_s32_eq( m_nan, 0 );
    const int     f_is_nan            = e_is_flagged;

    //
    // Handle exponent overflow
    // Cause a hardware floating point overflow;
    // if this returns, the half becomes an
    // infinity with the same sign as f.
    //
    const int     e_is_overflow       = pred_s32_gt( e, 30 );
 
    //
    // Overflow in significand,
    // Adjust exponent
    //
    const int     m_norm_is_overflow  = pred_s32_nz( m_rounded & 0x00800000 );

    //
    // Combine two cases that cause half to be INF
    //
    const int     h_is_inf            = e_is_overflow | f_is_inf;

    // 
    // Select correct result
    //

    //
    // Default:
    // E is greater than zero.  F is a normalized float.
    // We try to convert f to a normalized half.
    //

    const int32_t result_0 = h_norm;
    const int32_t result_1 = pred_s32_sel( m_norm_is_overflow, h_norm_overflow, result_0 );
    const int32_t result_2 = pred_s32_sel( f_is_nan,           h_nan,           result_1 );
    const int32_t result_3 = pred_s32_sel( f_is_nan_inf,       h_nan_notinf,    result_2 );
    const int32_t result_4 = pred_s32_sel( h_is_inf,           h_inf,           result_3 );
    const int32_t result_5 = pred_s32_sel( f_is_small,         h_denorm,        result_4 );
    const int32_t result_6 = pred_s32_sel( f_is_too_small,     h_zero,          result_5 );

    //
    // Cause a hardware floating point overflow;
    // if this returns, the half becomes an
    // infinity with the same sign as f.
    //

    if ( e_is_overflow )
    {
        half_overflow();    
    }                       

    return (result_6);
}

//-----------------------------------------------------
// Float-to-half conversion -- general case, including
// zeroes, denormalized numbers and exponent overflows.
//-----------------------------------------------------

// Modifications:
//     Mike Acton <macton@gmail.com>
//         - Convert predicates to masks.
//         - No branches except for final overflow check.

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

uint16_t
half_convert_v5( int32_t i )
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

    const int32_t s                   =  (i >> 16) & 0x00008000;
    const int32_t e                   = ((i >> 23) & 0x000000ff) - (127 - 15);
    const int32_t m                   =   i        & 0x007fffff;

    //
    // Round to nearest, round "0.5" up.
    //
    // Rounding may cause the significand to overflow and make
    // our number normalized.  Because of the way a half's bits
    // are laid out, we don't have to treat this case separately;
    // the code below will handle it correctly.
    // 

    const int32_t m_rounded           = m + ( (m & 0x00001000) << 1 );

    //
    // Calculating denormalized value requires shift by variable amount.
    // This is microcoded on the PPU, so note that this operation is
    // extremely expensive. 
    //
    const int32_t m_denorm            = (m_rounded | 0x00800000) >> (1 - e);

    const int32_t h_denorm            = s | (m_denorm >> 13);
    const int32_t h_zero              = s;
    const int32_t h_inf               = s | 0x7c00;
    const int32_t m_nan               = m >> 13;
    const int32_t h_nan               = s | 0x7c00 | m_nan;
    const int32_t h_nan_notinf        = h_nan | 0x01;
    const int32_t h_norm_overflow     = s | ((e+1) << 10);
    const int32_t h_norm              = s | (e << 10) | ( m_rounded >> 13 );

    //
    // E is less than -10.  The absolute value of f is
    // less than HALF_MIN (f may be a small normalized
    // float, a denormalized float or a zero).
    //
    // We convert f to a half zero.
    //
    const uint32_t    f_is_too_small      = mask_s32_lt( e, -10 );

    //
    // E is between -10 and 0.  F is a normalized float,
    // whose magnitude is less than HALF_NRM_MIN.
    //
    // We convert f to a denormalized half.
    // 
    const uint32_t    f_is_small          = mask_s32_lt( e, 1 );
    

    //
    // E can be flagged for either NAN or INF.
    //
    const uint32_t    e_is_flagged        = mask_s32_eq( e,  0xff - (127 - 15) );

    //
    // F is an infinity; convert f to a half
    // infinity with the same sign as f.
    //
    const uint32_t    f_is_inf            = e_is_flagged & mask_s32_eq( m, 0 );

    //
    // F is a NAN; we produce a half NAN that preserves
    // the sign bit and the 10 leftmost bits of the
    // significand of f, with one exception: If the 10
    // leftmost bits are all zero, the NAN would turn 
    // into an infinity, so we have to set at least one
    // bit in the significand.
    //
    const uint32_t    f_is_nan_inf        = e_is_flagged & mask_s32_eq( m_nan, 0 );
    const uint32_t    f_is_nan            = e_is_flagged;

    //
    // Handle exponent overflow
    // Cause a hardware floating point overflow;
    // if this returns, the half becomes an
    // infinity with the same sign as f.
    //
    const uint32_t    e_is_overflow       = mask_s32_gt( e, 30 );
 
    //
    // Overflow in significand,
    // Adjust exponent
    //
    const uint32_t    m_norm_is_overflow  = mask_s32_nz( m_rounded & 0x00800000 );

    //
    // Combine two cases that cause half to be INF
    //
    const uint32_t    h_is_inf            = e_is_overflow | f_is_inf;

    // 
    // Select correct result
    //

    //
    // Default:
    // E is greater than zero.  F is a normalized float.
    // We try to convert f to a normalized half.
    //

    const int32_t result_0 = h_norm;
    const int32_t result_1 = mask_s32_sel( m_norm_is_overflow, h_norm_overflow, result_0 );
    const int32_t result_2 = mask_s32_sel( f_is_nan,           h_nan,           result_1 );
    const int32_t result_3 = mask_s32_sel( f_is_nan_inf,       h_nan_notinf,    result_2 );
    const int32_t result_4 = mask_s32_sel( h_is_inf,           h_inf,           result_3 );
    const int32_t result_5 = mask_s32_sel( f_is_small,         h_denorm,        result_4 );
    const int32_t result_6 = mask_s32_sel( f_is_too_small,     h_zero,          result_5 );

    //
    // Cause a hardware floating point overflow;
    // if this returns, the half becomes an
    // infinity with the same sign as f.
    //

    if ( e_is_overflow )
    {
        half_overflow();    
    }                       

    return (result_6);
}

//-----------------------------------------------------
// Float-to-half conversion -- general case, including
// zeroes, denormalized numbers and exponent overflows.
//-----------------------------------------------------

// Modifications:
//     Mike Acton <macton@gmail.com>
//         - Convert to 64bit math
//         - Remove FPU overflow on half overflow

static inline uint64_t mask_s64_sel( uint64_t mask, int64_t a, int64_t b )
{
  return ( ( mask & (uint64_t)a) | ( (~mask) & (uint64_t)b ) );
}

static inline uint64_t mask_s64_lt( int64_t a, int64_t b )
{
  // NOTE: Result is invalid when a=INT32_MAX, b=INT32_MIN
  return (uint64_t)( ( a - b ) >> 31 );
}

static inline uint64_t mask_s64_gt( int64_t a, int64_t b )
{
  // NOTE: Result is invalid when a=INT32_MIN, b=INT32_MAX
  return (uint64_t)( ( b - a ) >> 31 );
}

static inline int64_t mask_s64_nz( int64_t a )
{
  return ( ( a | -a ) >> 31 );
}

static inline int64_t mask_s64_eq( int64_t a, int64_t b )
{
  return ( ~mask_s64_nz( a-b ) );
}

uint16_t
half_convert( int32_t i )
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

    const int64_t s                   =  (i >> 16) & 0x00008000;
    const int64_t e                   = ((i >> 23) & 0x000000ff) - (127 - 15);
    const int64_t m                   =   i        & 0x007fffff;

    //
    // Round to nearest, round "0.5" up.
    //
    // Rounding may cause the significand to overflow and make
    // our number normalized.  Because of the way a half's bits
    // are laid out, we don't have to treat this case separately;
    // the code below will handle it correctly.
    // 

    const int64_t m_rounded           = m + ( (m & 0x00001000) << 1 );

    //
    // Calculating denormalized value requires shift by variable amount.
    // This is microcoded on the PPU, so note that this operation is
    // extremely expensive. 
    //
    const int64_t m_denorm            = (m_rounded | 0x00800000) >> (1 - e);

    const int64_t h_denorm            = s | (m_denorm >> 13);
    const int64_t h_zero              = s;
    const int64_t h_inf               = s | 0x7c00;
    const int64_t m_nan               = m >> 13;
    const int64_t h_nan               = s | 0x7c00 | m_nan;
    const int64_t h_nan_notinf        = h_nan | 0x01;
    const int64_t h_norm_overflow     = s | ((e+1) << 10);
    const int64_t h_norm              = s | (e << 10) | ( m_rounded >> 13 );

    //
    // E is less than -10.  The absolute value of f is
    // less than HALF_MIN (f may be a small normalized
    // float, a denormalized float or a zero).
    //
    // We convert f to a half zero.
    //
    const uint64_t    f_is_too_small      = mask_s64_lt( e, -10 );

    //
    // E is between -10 and 0.  F is a normalized float,
    // whose magnitude is less than HALF_NRM_MIN.
    //
    // We convert f to a denormalized half.
    // 
    const uint64_t    f_is_small          = mask_s64_lt( e, 1 );
    

    //
    // E can be flagged for either NAN or INF.
    //
    const uint64_t    e_is_flagged        = mask_s64_eq( e,  0xff - (127 - 15) );

    //
    // F is an infinity; convert f to a half
    // infinity with the same sign as f.
    //
    const uint64_t    f_is_inf            = e_is_flagged & mask_s64_eq( m, 0 );

    //
    // F is a NAN; we produce a half NAN that preserves
    // the sign bit and the 10 leftmost bits of the
    // significand of f, with one exception: If the 10
    // leftmost bits are all zero, the NAN would turn 
    // into an infinity, so we have to set at least one
    // bit in the significand.
    //
    const uint64_t    f_is_nan_inf        = e_is_flagged & mask_s64_eq( m_nan, 0 );
    const uint64_t    f_is_nan            = e_is_flagged;

    //
    // Handle exponent overflow
    //
    const uint64_t    e_is_overflow       = mask_s64_gt( e, 30 );
 
    //
    // Overflow in significand,
    // Adjust exponent
    //
    const uint64_t    m_norm_is_overflow  = mask_s64_nz( m_rounded & 0x00800000 );

    //
    // Combine two cases that cause half to be INF
    //
    const uint64_t    h_is_inf            = e_is_overflow | f_is_inf;

    // 
    // Select correct result
    //

    //
    // Default:
    // E is greater than zero.  F is a normalized float.
    // We try to convert f to a normalized half.
    //

    const int64_t result_0 = h_norm;
    const int64_t result_1 = mask_s64_sel( m_norm_is_overflow, h_norm_overflow, result_0 );
    const int64_t result_2 = mask_s64_sel( f_is_nan,           h_nan,           result_1 );
    const int64_t result_3 = mask_s64_sel( f_is_nan_inf,       h_nan_notinf,    result_2 );
    const int64_t result_4 = mask_s64_sel( h_is_inf,           h_inf,           result_3 );
    const int64_t result_5 = mask_s64_sel( f_is_small,         h_denorm,        result_4 );
    const int64_t result_6 = mask_s64_sel( f_is_too_small,     h_zero,          result_5 );

    return (result_6);
}

uint16_t
half_convert_32( int32_t i )
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

    const int32_t s                   =  (i >> 16) & 0x00008000;
    const int32_t e                   = ((i >> 23) & 0x000000ff) - (127 - 15);
    const int32_t m                   =   i        & 0x007fffff;

    //
    // Round to nearest, round "0.5" up.
    //
    // Rounding may cause the significand to overflow and make
    // our number normalized.  Because of the way a half's bits
    // are laid out, we don't have to treat this case separately;
    // the code below will handle it correctly.
    // 

    const int32_t m_rounded           = m + ( (m & 0x00001000) << 1 );

    //
    // Calculating denormalized value requires shift by variable amount.
    // This is microcoded on the PPU, so note that this operation is
    // extremely expensive. 
    //
    const int32_t m_denorm            = (m_rounded | 0x00800000) >> (1 - e);

    const int32_t h_denorm            = s | (m_denorm >> 13);
    const int32_t h_zero              = s;
    const int32_t h_inf               = s | 0x7c00;
    const int32_t m_nan               = m >> 13;
    const int32_t h_nan               = s | 0x7c00 | m_nan;
    const int32_t h_nan_notinf        = h_nan | 0x01;
    const int32_t h_norm_overflow     = s | ((e+1) << 10);
    const int32_t h_norm              = s | (e << 10) | ( m_rounded >> 13 );

    //
    // E is less than -10.  The absolute value of f is
    // less than HALF_MIN (f may be a small normalized
    // float, a denormalized float or a zero).
    //
    // We convert f to a half zero.
    //
    const uint32_t    f_is_too_small      = mask_s32_lt( e, -10 );

    //
    // E is between -10 and 0.  F is a normalized float,
    // whose magnitude is less than HALF_NRM_MIN.
    //
    // We convert f to a denormalized half.
    // 
    const uint32_t    f_is_small          = mask_s32_lt( e, 1 );
    

    //
    // E can be flagged for either NAN or INF.
    //
    const uint32_t    e_is_flagged        = mask_s32_eq( e,  0xff - (127 - 15) );

    //
    // F is an infinity; convert f to a half
    // infinity with the same sign as f.
    //
    const uint32_t    f_is_inf            = e_is_flagged & mask_s32_eq( m, 0 );

    //
    // F is a NAN; we produce a half NAN that preserves
    // the sign bit and the 10 leftmost bits of the
    // significand of f, with one exception: If the 10
    // leftmost bits are all zero, the NAN would turn 
    // into an infinity, so we have to set at least one
    // bit in the significand.
    //
    const uint32_t    f_is_nan_inf        = e_is_flagged & mask_s32_eq( m_nan, 0 );
    const uint32_t    f_is_nan            = e_is_flagged;

    //
    // Handle exponent overflow
    //
    const uint32_t    e_is_overflow       = mask_s32_gt( e, 30 );
 
    //
    // Overflow in significand,
    // Adjust exponent
    //
    const uint32_t    m_norm_is_overflow  = mask_s32_nz( m_rounded & 0x00800000 );

    //
    // Combine two cases that cause half to be INF
    //
    const uint32_t    h_is_inf            = e_is_overflow | f_is_inf;

    // 
    // Select correct result
    //

    //
    // Default:
    // E is greater than zero.  F is a normalized float.
    // We try to convert f to a normalized half.
    //

    const int32_t result_0 = h_norm;
    const int32_t result_1 = mask_s32_sel( m_norm_is_overflow, h_norm_overflow, result_0 );
    const int32_t result_2 = mask_s32_sel( f_is_nan,           h_nan,           result_1 );
    const int32_t result_3 = mask_s32_sel( f_is_nan_inf,       h_nan_notinf,    result_2 );
    const int32_t result_4 = mask_s32_sel( h_is_inf,           h_inf,           result_3 );
    const int32_t result_5 = mask_s32_sel( f_is_small,         h_denorm,        result_4 );
    const int32_t result_6 = mask_s32_sel( f_is_too_small,     h_zero,          result_5 );

    return (result_6);
}

//-----------------------------------------------------
// Float-to-half conversion -- fast case
// No denormalized numbers, no overflow, anything
// small is zero. In the most common case, we're going
// to be pretty sure that our numbers are in range.
//-----------------------------------------------------

uint16_t
half_convert_fast( int32_t i )
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

    const int64_t s        =  (i >> 16) & 0x00008000;
    const int64_t e        = ((i >> 23) & 0x000000ff) - (127 - 15);
    const int64_t m        =   i        & 0x007fffff;
    const int64_t h_zero   = s;
    const int64_t h_norm   = s | (e << 10) | ( m >> 13 );

    //
    // E is less than 0. 
    // F could be a small normalized float, a denormalized float
    // or zero.
    //
    // We convert f to a half zero.
    //
    const uint64_t f_is_too_small  = mask_s64_lt( e, 0 );

    //
    // Default:
    // E is greater than zero.  F is a normalized float.
    // We try to convert f to a normalized half.
    //
    const int64_t result = mask_s64_sel( f_is_too_small, h_zero, h_norm );

    return (result);
}

//-----------------------------------------------------
// Float-to-half conversion -- fast case x 4 (Altivec)
// No denormalized numbers, no overflow, anything
// small is zero. In the most common case, we're going
// to be pretty sure that our numbers are in range.
//-----------------------------------------------------

vector signed int
half_convert_fast4( vector signed int i )
{
    const vector signed   int s_mask              =  { 0x00008000, 0x00008000, 0x00008000, 0x00008000 };
    const vector unsigned int s_shc               =  { 0x00000010, 0x00000010, 0x00000010, 0x00000010 };
    const vector signed   int e_mask              =  { 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff };
    const vector signed   int e_bias              =  { 0x00000070, 0x00000070, 0x00000070, 0x00000070 };
    const vector unsigned int e_shc               =  { 0x00000017, 0x00000017, 0x00000017, 0x00000017 };
    const vector signed   int m_mask              =  { 0x007fffff, 0x007fffff, 0x007fffff, 0x007fffff };
    const vector unsigned int h_e_shc             =  { 0x0000000a, 0x0000000a, 0x0000000a, 0x0000000a };
    const vector unsigned int h_m_shc             =  { 0x0000000d, 0x0000000d, 0x0000000d, 0x0000000d };
    const vector signed   int zero                =  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

    const vector signed   int s_shifted           = vec_sra( i,         s_shc );
    const vector signed   int s                   = vec_and( s_shifted, s_mask );
    const vector signed   int e_shifted           = vec_sra( i,         e_shc );
    const vector signed   int e_masked            = vec_and( e_shifted, e_mask );
    const vector signed   int e                   = vec_sub( e_masked,  e_bias );
    const vector signed   int m                   = vec_and( i,         m_mask );
    const vector signed   int h_e                 = vec_sl( e, h_e_shc );
    const vector signed   int h_m                 = vec_sra( m, h_m_shc );
    const vector signed   int h_norm_unsigned     = vec_or( h_e, h_m );
    const vector signed   int h_norm              = vec_or( s, h_norm_unsigned );
    const vector signed   int f_is_too_small      = vec_cmplt( e, zero );
    const vector signed   int result              = vec_sel( h_norm, s, f_is_too_small );

    return (result);
}

//-----------------------------------------------------
// Float-to-half conversion -- fast case x 4 (Altivec)
// No denormalized numbers, no overflow, anything
// small is zero. In the most common case, we're going
// to be pretty sure that our numbers are in range.
//-----------------------------------------------------

// Modifications:
//     Mike Acton <macton@gmail.com>
//         - Build immediate values instead of loading them

vector signed int
half_convert_fast4_v2( vector signed int i )
{
    const vector unsigned int u32_0xff   = vec_splat_u32( -1   );          // 0xffffffff
    const vector signed   int s32_0x00   = vec_splat_s32( 0x00 );          // 0x00000000
    const vector signed   int s32_0x07   = vec_splat_s32( 0x07 );          // 0x00000007
    const vector signed   int s32_0x08   = vec_splat_s32( 0x08 );          // 0x00000008
    const vector signed   int s32_0x0d   = vec_splat_s32( 0x0d );          // 0x0000000d
    const vector unsigned int u32_0x01   = vec_splat_u32( 0x01 );          // 0x00000001
    const vector unsigned int u32_0x04   = vec_splat_u32( 0x04 );          // 0x00000004
    const vector unsigned int u32_0x09   = vec_splat_u32( 0x09 );          // 0x00000009
    const vector unsigned int u32_0x0a   = vec_splat_u32( 0x0a );          // 0x0000000a
    const vector unsigned int u32_0x0c   = vec_splat_u32( 0x0c );          // 0x0000000c
    const vector unsigned int u32_0x0d   = vec_splat_u32( 0x0d );          // 0x0000000d
    const vector signed   int s32_0xff   = (vector signed   int)u32_0xff;  // 0xffffffff
    const vector unsigned int u32_0x07   = (vector unsigned int)s32_0x07;  // 0x00000008
    const vector unsigned int u32_0x08   = (vector unsigned int)s32_0x08;  // 0x00000008
    const vector unsigned int u32_0x10   = vec_sl( u32_0x08, u32_0x01 );   // 0x00000010
    const vector unsigned int u32_0x17   = vec_or( u32_0x10, u32_0x07 );   // 0x00000017
    const vector unsigned int u32_0x18   = vec_or( u32_0x10, u32_0x08 );   // 0x00000018
    const vector signed   int s_mask     = vec_sl( s32_0x08, u32_0x0c );   // 0x00008000
    const vector signed   int e_mask     = vec_sr( s32_0xff, u32_0x18 );   // 0x000000ff
    const vector signed   int e_bias     = vec_sl( s32_0x07, u32_0x04 );   // 0x00000070
    const vector signed   int m_mask     = vec_sr( s32_0xff, u32_0x09 );   // 0x007fffff

    const vector signed   int s_shifted           = vec_sra   ( i,         u32_0x10        );
    const vector signed   int s                   = vec_and   ( s_shifted, s_mask          );
    const vector signed   int e_shifted           = vec_sra   ( i,         u32_0x17        );
    const vector signed   int e_masked            = vec_and   ( e_shifted, e_mask          );
    const vector signed   int e                   = vec_sub   ( e_masked,  e_bias          );
    const vector signed   int m                   = vec_and   ( i,         m_mask          );
    const vector signed   int h_e                 = vec_sl    ( e,         u32_0x0a        );
    const vector signed   int h_m                 = vec_sra   ( m,         u32_0x0d        );
    const vector signed   int h_norm_unsigned     = vec_or    ( h_e,       h_m             );
    const vector signed   int h_norm              = vec_or    ( s,         h_norm_unsigned );
    const vector signed   int f_is_too_small      = vec_cmplt ( e,         s32_0x00        );
    const vector signed   int result              = vec_sel   ( h_norm, s, f_is_too_small  );

    return (result);
}

void
half_convert_fast4n_v1( vector signed int* restrict src_buf, vector signed int* restrict dst_buf, int32_t count )
{
    const vector unsigned int u32_0xff   = vec_splat_u32( -1   );          // 0xffffffff
    const vector signed   int s32_0x00   = vec_splat_s32( 0x00 );          // 0x00000000
    const vector signed   int s32_0x07   = vec_splat_s32( 0x07 );          // 0x00000007
    const vector signed   int s32_0x08   = vec_splat_s32( 0x08 );          // 0x00000008
    const vector signed   int s32_0x0d   = vec_splat_s32( 0x0d );          // 0x0000000d
    const vector unsigned int u32_0x01   = vec_splat_u32( 0x01 );          // 0x00000001
    const vector unsigned int u32_0x04   = vec_splat_u32( 0x04 );          // 0x00000004
    const vector unsigned int u32_0x09   = vec_splat_u32( 0x09 );          // 0x00000009
    const vector unsigned int u32_0x0a   = vec_splat_u32( 0x0a );          // 0x0000000a
    const vector unsigned int u32_0x0c   = vec_splat_u32( 0x0c );          // 0x0000000c
    const vector unsigned int u32_0x0d   = vec_splat_u32( 0x0d );          // 0x0000000d
    const vector signed   int s32_0xff   = (vector signed   int)u32_0xff;  // 0xffffffff
    const vector unsigned int u32_0x07   = (vector unsigned int)s32_0x07;  // 0x00000008
    const vector unsigned int u32_0x08   = (vector unsigned int)s32_0x08;  // 0x00000008
    const vector unsigned int u32_0x10   = vec_sl( u32_0x08, u32_0x01 );   // 0x00000010
    const vector unsigned int u32_0x17   = vec_or( u32_0x10, u32_0x07 );   // 0x00000017
    const vector unsigned int u32_0x18   = vec_or( u32_0x10, u32_0x08 );   // 0x00000018
    const vector signed   int s_mask     = vec_sl( s32_0x08, u32_0x0c );   // 0x00008000
    const vector signed   int e_mask     = vec_sr( s32_0xff, u32_0x18 );   // 0x000000ff
    const vector signed   int e_bias     = vec_sl( s32_0x07, u32_0x04 );   // 0x00000070
    const vector signed   int m_mask     = vec_sr( s32_0xff, u32_0x09 );   // 0x007fffff

    for ( int32_t i_offset= 0; i_offset < (count<<0x04); i_offset += 0x04 )
    {
        const vector signed int i                = vec_ld    ( i_offset,   src_buf           );
        const vector signed int s_shifted        = vec_sra   ( i,          u32_0x10          );
        const vector signed int s                = vec_and   ( s_shifted,  s_mask            );
        const vector signed int e_shifted        = vec_sra   ( i,          u32_0x17          );
        const vector signed int e_masked         = vec_and   ( e_shifted,  e_mask            );
        const vector signed int e                = vec_sub   ( e_masked,   e_bias            );
        const vector signed int m                = vec_and   ( i,          m_mask            );
        const vector signed int h_e              = vec_sl    ( e,          u32_0x0a          );
        const vector signed int h_m              = vec_sr    ( m,          u32_0x0d          );
        const vector signed int h_norm_unsigned  = vec_or    ( h_e,        h_m               );
        const vector signed int h_norm           = vec_or    ( s,          h_norm_unsigned   );
        const vector signed int f_is_too_small   = vec_cmplt ( e,          s32_0x00          );
        const vector signed int result           = vec_sel   ( h_norm,     s, f_is_too_small );

        vec_st( result, i_offset, dst_buf );
    }
}

//-----------------------------------------------------
// Float-to-half conversion -- fast case x 4 (Altivec)
// No denormalized numbers, no overflow, anything
// small is zero. In the most common case, we're going
// to be pretty sure that our numbers are in range.
//-----------------------------------------------------

// Modifications:
//     Mike Acton <macton@gmail.com>
//         - Assuming count is div by 2, store every two 
//           results packed into a single vector short
void
half_convert_fast4n_packed( vector signed int* restrict src_buf, vector signed int* restrict dst_buf, int32_t count )
{
    const vector unsigned int u32_0xff   = vec_splat_u32( -1   );          // 0xffffffff
    const vector signed   int s32_0x00   = vec_splat_s32( 0x00 );          // 0x00000000
    const vector signed   int s32_0x07   = vec_splat_s32( 0x07 );          // 0x00000007
    const vector signed   int s32_0x08   = vec_splat_s32( 0x08 );          // 0x00000008
    const vector signed   int s32_0x0d   = vec_splat_s32( 0x0d );          // 0x0000000d
    const vector unsigned int u32_0x01   = vec_splat_u32( 0x01 );          // 0x00000001
    const vector unsigned int u32_0x04   = vec_splat_u32( 0x04 );          // 0x00000004
    const vector unsigned int u32_0x09   = vec_splat_u32( 0x09 );          // 0x00000009
    const vector unsigned int u32_0x0a   = vec_splat_u32( 0x0a );          // 0x0000000a
    const vector unsigned int u32_0x0c   = vec_splat_u32( 0x0c );          // 0x0000000c
    const vector unsigned int u32_0x0d   = vec_splat_u32( 0x0d );          // 0x0000000d
    const vector signed   int s32_0xff   = (vector signed   int)u32_0xff;  // 0xffffffff
    const vector unsigned int u32_0x07   = (vector unsigned int)s32_0x07;  // 0x00000008
    const vector unsigned int u32_0x08   = (vector unsigned int)s32_0x08;  // 0x00000008
    const vector unsigned int u32_0x10   = vec_sl( u32_0x08, u32_0x01 );   // 0x00000010
    const vector unsigned int u32_0x17   = vec_or( u32_0x10, u32_0x07 );   // 0x00000017
    const vector unsigned int u32_0x18   = vec_or( u32_0x10, u32_0x08 );   // 0x00000018
    const vector signed   int s_mask     = vec_sl( s32_0x08, u32_0x0c );   // 0x00008000
    const vector signed   int e_mask     = vec_sr( s32_0xff, u32_0x18 );   // 0x000000ff
    const vector signed   int e_bias     = vec_sl( s32_0x07, u32_0x04 );   // 0x00000070
    const vector signed   int m_mask     = vec_sr( s32_0xff, u32_0x09 );   // 0x007fffff

    for ( int32_t i_offset = 0, j_offset = 0x04; i_offset < (count<<0x04); i_offset += 0x08, j_offset += 0x08 )
    {
        const vector signed int   i                = vec_ld    ( i_offset,     src_buf               );
        const vector signed int   s_shifted_0      = vec_sra   ( i,            u32_0x10              );
        const vector signed int   s_0              = vec_and   ( s_shifted_0,  s_mask                );
        const vector signed int   e_shifted_0      = vec_sra   ( i,            u32_0x17              );
        const vector signed int   e_masked_0       = vec_and   ( e_shifted_0,  e_mask                );
        const vector signed int   e_0              = vec_sub   ( e_masked_0,   e_bias                );
        const vector signed int   m_0              = vec_and   ( i,            m_mask                );
        const vector signed int   h_e_0            = vec_sl    ( e_0,          u32_0x0a              );
        const vector signed int   h_m_0            = vec_sr    ( m_0,          u32_0x0d              );
        const vector signed int   h_norm_unsigned_0= vec_or    ( h_e_0,        h_m_0                 );
        const vector signed int   h_norm_0         = vec_or    ( s_0,          h_norm_unsigned_0     );
        const vector signed int   f_is_too_small_0 = vec_cmplt ( e_0,          s32_0x00              );
        const vector signed int   result_0         = vec_sel   ( h_norm_0,     s_0, f_is_too_small_0 );

        const vector signed int   j                = vec_ld    ( j_offset,     src_buf               );
        const vector signed int   s_shifted_1      = vec_sra   ( j,            u32_0x10              );
        const vector signed int   s_1              = vec_and   ( s_shifted_1,  s_mask                );
        const vector signed int   e_shifted_1      = vec_sra   ( j,            u32_0x17              );
        const vector signed int   e_masked_1       = vec_and   ( e_shifted_1,  e_mask                );
        const vector signed int   e_1              = vec_sub   ( e_masked_1,   e_bias                );
        const vector signed int   m_1              = vec_and   ( j,            m_mask                );
        const vector signed int   h_e_1            = vec_sl    ( e_1,          u32_0x0a              );
        const vector signed int   h_m_1            = vec_sr    ( m_1,          u32_0x0d              );
        const vector signed int   h_norm_unsigned_1= vec_or    ( h_e_1,        h_m_1                 );
        const vector signed int   h_norm_1         = vec_or    ( s_1,          h_norm_unsigned_1     );
        const vector signed int   f_is_too_small_1 = vec_cmplt ( e_1,          s32_0x00              );
        const vector signed int   result_1         = vec_sel   ( h_norm_1,     s_1, f_is_too_small_1 );

        const vector signed short result           = vec_pack  ( result_0,     result_1              );

        vec_st( result, 0, dst_buf );
        dst_buf++;
    }
}

void
half_convert_fast4n_packed_noconst( vector signed int* restrict src_buf, vector signed int* restrict dst_buf, int32_t count )
{
    vector unsigned int u32_0xff   = vec_splat_u32( -1   );          // 0xffffffff
    vector signed   int s32_0x00   = vec_splat_s32( 0x00 );          // 0x00000000
    vector signed   int s32_0x07   = vec_splat_s32( 0x07 );          // 0x00000007
    vector signed   int s32_0x08   = vec_splat_s32( 0x08 );          // 0x00000008
    vector signed   int s32_0x0d   = vec_splat_s32( 0x0d );          // 0x0000000d
    vector unsigned int u32_0x01   = vec_splat_u32( 0x01 );          // 0x00000001
    vector unsigned int u32_0x04   = vec_splat_u32( 0x04 );          // 0x00000004
    vector unsigned int u32_0x09   = vec_splat_u32( 0x09 );          // 0x00000009
    vector unsigned int u32_0x0a   = vec_splat_u32( 0x0a );          // 0x0000000a
    vector unsigned int u32_0x0c   = vec_splat_u32( 0x0c );          // 0x0000000c
    vector unsigned int u32_0x0d   = vec_splat_u32( 0x0d );          // 0x0000000d
    vector signed   int s32_0xff   = (vector signed   int)u32_0xff;  // 0xffffffff
    vector unsigned int u32_0x07   = (vector unsigned int)s32_0x07;  // 0x00000008
    vector unsigned int u32_0x08   = (vector unsigned int)s32_0x08;  // 0x00000008
    vector unsigned int u32_0x10   = vec_sl( u32_0x08, u32_0x01 );   // 0x00000010
    vector unsigned int u32_0x17   = vec_or( u32_0x10, u32_0x07 );   // 0x00000017
    vector unsigned int u32_0x18   = vec_or( u32_0x10, u32_0x08 );   // 0x00000018
    vector signed   int s_mask     = vec_sl( s32_0x08, u32_0x0c );   // 0x00008000
    vector signed   int e_mask     = vec_sr( s32_0xff, u32_0x18 );   // 0x000000ff
    vector signed   int e_bias     = vec_sl( s32_0x07, u32_0x04 );   // 0x00000070
    vector signed   int m_mask     = vec_sr( s32_0xff, u32_0x09 );   // 0x007fffff

    for ( int32_t i_offset = 0, j_offset = 0x04; i_offset < (count<<0x04); i_offset += 0x08, j_offset += 0x08 )
    {
        vector signed int   i                = vec_ld    ( i_offset,     src_buf               );
        vector signed int   s_shifted_0      = vec_sra   ( i,            u32_0x10              );
        vector signed int   s_0              = vec_and   ( s_shifted_0,  s_mask                );
        vector signed int   e_shifted_0      = vec_sra   ( i,            u32_0x17              );
        vector signed int   e_masked_0       = vec_and   ( e_shifted_0,  e_mask                );
        vector signed int   e_0              = vec_sub   ( e_masked_0,   e_bias                );
        vector signed int   m_0              = vec_and   ( i,            m_mask                );
        vector signed int   h_e_0            = vec_sl    ( e_0,          u32_0x0a              );
        vector signed int   h_m_0            = vec_sr    ( m_0,          u32_0x0d              );
        vector signed int   h_norm_unsigned_0= vec_or    ( h_e_0,        h_m_0                 );
        vector signed int   h_norm_0         = vec_or    ( s_0,          h_norm_unsigned_0     );
        vector signed int   f_is_too_small_0 = vec_cmplt ( e_0,          s32_0x00              );
        vector signed int   result_0         = vec_sel   ( h_norm_0,     s_0, f_is_too_small_0 );

        vector signed int   j                = vec_ld    ( j_offset,     src_buf               );
        vector signed int   s_shifted_1      = vec_sra   ( j,            u32_0x10              );
        vector signed int   s_1              = vec_and   ( s_shifted_1,  s_mask                );
        vector signed int   e_shifted_1      = vec_sra   ( j,            u32_0x17              );
        vector signed int   e_masked_1       = vec_and   ( e_shifted_1,  e_mask                );
        vector signed int   e_1              = vec_sub   ( e_masked_1,   e_bias                );
        vector signed int   m_1              = vec_and   ( j,            m_mask                );
        vector signed int   h_e_1            = vec_sl    ( e_1,          u32_0x0a              );
        vector signed int   h_m_1            = vec_sr    ( m_1,          u32_0x0d              );
        vector signed int   h_norm_unsigned_1= vec_or    ( h_e_1,        h_m_1                 );
        vector signed int   h_norm_1         = vec_or    ( s_1,          h_norm_unsigned_1     );
        vector signed int   f_is_too_small_1 = vec_cmplt ( e_1,          s32_0x00              );
        vector signed int   result_1         = vec_sel   ( h_norm_1,     s_1, f_is_too_small_1 );

        vector signed short result           = vec_pack  ( result_0,     result_1              );

        vec_st( result, 0, dst_buf );
        dst_buf++;
    }
}

