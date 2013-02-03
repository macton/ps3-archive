#include <stdint.h>

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

typedef union FLOAT_32 FLOAT_32;
union FLOAT_32
{
    float    f32;
    uint32_t u32;
};

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
half_from_float( uint32_t i )
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
            // half_overflow();    // Cause a hardware floating point overflow;
            return s | 0x7c00;  // if this returns, the half becomes an
        }                       // infinity with the same sign as f.
    
        //
        // Assemble the half from s, e and m.
        //
    
        return s | (e << 10) | (m >> 13);
    }
}