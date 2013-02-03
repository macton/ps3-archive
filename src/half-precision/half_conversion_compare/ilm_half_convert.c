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

// NOTE: This file has been slightly modified from the original by Mike Acton
//     * Methods converted to functions and renamed
//     * Changed to stdint types
//     * Moved the conversion functions from half.cpp and toFloat.cpp into this file.

#include <stdint.h>

uint16_t
half_from_float( uint32_t i )
{
    //
    // Our floating point number, f, is represented by the bit
    // pattern in int32_teger i.  Disassemble that bit pattern int32_to
    // the sign, s, the exponent, e, and the significand, m.
    // Shift s int32_to the position where it will go in in the
    // resulting half number.
    // Adjust e, accounting for the different exponent bias
    // of float and half (127 versus 15).
    //

    register int32_t s =  (i >> 16) & 0x00008000;
    register int32_t e = ((i >> 23) & 0x000000ff) - (127 - 15);
    register int32_t m =   i        & 0x007fffff;

    //
    // Now reassemble s, e and m int32_to a half:
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

        return 0;
    }

    //
    // E is between -10 and 0.  F is a normalized float,
    // whose magnitude is less than HALF_NRM_MIN.
    //
    // We convert f to a denormalized half.
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
        // int32_to an infinity, so we have to set at least one
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
        return s | 0x7c00;    // if this returns, the half becomes an
    }               // infinity with the same sign as f.

    //
    // Assemble the half from s, e and m.
    //

    return s | (e << 10) | (m >> 13);
    }
}

uint32_t
half_to_float(uint16_t y)
{

    int32_t s = (y >> 15) & 0x00000001;
    int32_t e = (y >> 10) & 0x0000001f;
    int32_t m =  y        & 0x000003ff;

    if (e == 0)
    {
    if (m == 0)
    {
        //
        // Plus or minus zero
        //

        return s << 31;
    }
    else
    {
        //
        // Denormalized number -- renormalize it
        //

        while (!(m & 0x00000400))
        {
        m <<= 1;
        e -=  1;
        }

        e += 1;
        m &= ~0x00000400;
    }
    }
    else if (e == 31)
    {
    if (m == 0)
    {
        //
        // Positive or negative infinity
        //

        return (s << 31) | 0x7f800000;
    }
    else
    {
        //
        // Nan -- preserve sign and significand bits
        //

        return (s << 31) | 0x7f800000 | (m << 13);
    }
    }

    //
    // Normalized number
    //

    e = e + (127 - 15);
    m = m << 13;

    //
    // Assemble s, e and m.
    //

    return (s << 31) | (e << 23) | m;
}

uint32_t 
half_from_float_x2( uint64_t f2 )
{
  uint32_t f0     = (uint32_t)(f2);
  uint32_t f1     = (uint32_t)(f2 >> 32);
  uint32_t h0     = (uint64_t)half_from_float(f0);
  uint32_t h1     = (uint64_t)half_from_float(f1);
  uint32_t result = h0 | ( h1 << 16 );

  return (result);
}

uint64_t 
half_to_float_x2( uint32_t h2 )
{
  uint16_t h0     = (uint16_t)(h2);
  uint16_t h1     = (uint16_t)(h2 >> 16);
  uint64_t f0     = (uint64_t)half_to_float(h0);
  uint64_t f1     = (uint64_t)half_to_float(h1);
  uint64_t result = f0 | ( f1 << 32 );

  return (result);
}
