#include <stdint.h>
#include "uint32_instr.h"
#include "uint64_instr.h"

/*----------------------------------------------------------------------------
| The pattern for a default generated single-precision NaN.
*----------------------------------------------------------------------------*/
#define cp_float_default_nan 0xFFC00000

/*----------------------------------------------------------------------------
| Software IEC/IEEE floating-point underflow tininess-detection mode.
*----------------------------------------------------------------------------*/
enum {
    float_tininess_after_rounding  = 0,
    float_tininess_before_rounding = 1
};
// signed char float_detect_tininess = float_tininess_before_rounding;
extern signed char float_detect_tininess;

/*----------------------------------------------------------------------------
| Software IEC/IEEE floating-point rounding mode.
*----------------------------------------------------------------------------*/
enum {
    float_round_nearest_even = 0,
    float_round_down         = 1,
    float_round_up           = 2,
    float_round_to_zero      = 3
};
signed char cp_float_rounding_mode = float_round_nearest_even;

/*----------------------------------------------------------------------------
| Software IEC/IEEE floating-point exception flags.
*----------------------------------------------------------------------------*/
enum {
    float_flag_invalid   =  1,
    float_flag_divbyzero =  4,
    float_flag_overflow  =  8,
    float_flag_underflow = 16,
    float_flag_inexact   = 32
};
// signed char float_exception_flags = 0;
extern signed char float_exception_flags;

void cp_float_raise( uint32_t flags )
{
    float_exception_flags |= flags;
}

/*----------------------------------------------------------------------------
| Returns the fraction bits of the single-precision floating-point value `a'.
*----------------------------------------------------------------------------*/

static inline uint32_t cp_float_extract_m( uint32_t a )
{
    const uint32_t m_mask = 0x007fffff;
    const uint32_t m      = uint32_and( a, m_mask );

    return (m);
}

/*----------------------------------------------------------------------------
| Returns the exponent bits of the single-precision floating-point value `a'.
*----------------------------------------------------------------------------*/

static inline uint32_t cp_float_extract_e( uint32_t a )
{
    const uint32_t e_pos  = 23;
    const uint32_t e_mask = 0x000000ff;
    const uint32_t es     = uint32_srl( a,  e_pos  );
    const uint32_t e      = uint32_and( es, e_mask );

    return (e);
}

/*----------------------------------------------------------------------------
| Returns the sign bit of the single-precision floating-point value `a'.
*----------------------------------------------------------------------------*/

static inline uint32_t cp_float_extract_s( uint32_t a )
{
    const uint32_t s = uint32_srl( a, 31 );

    return (s);
}

/*----------------------------------------------------------------------------
| Returns 1 if the single-precision floating-point value `a' is a NaN;
| otherwise returns 0.
*----------------------------------------------------------------------------*/

static inline uint32_t cp_float_is_nan( uint32_t a )
{
    uint32_t em          = uint32_sll( a, 1 );
    uint32_t is_nan_msb  = uint32_sub( 0xff000000, em );
    uint32_t is_nan_pred = uint32_srl( is_nan_msb, 31 );

    return (is_nan_pred);
}

/*----------------------------------------------------------------------------
| Returns 1 if the single-precision floating-point value `a' is a signaling
| NaN; otherwise returns 0.
*----------------------------------------------------------------------------*/

uint32_t cp_float_is_snan( uint32_t a )
{
    uint32_t flag_mask       = 0x1ff;
    uint32_t flag_expected   = 0x1fe;
    uint32_t flag_bits       = uint32_srl( a, 22 );
    uint32_t flag_unsigned   = uint32_and( flag_bits, flag_mask );
    uint32_t is_flagged      = uint32_cmpeq( flag_unsigned, flag_expected );
    uint32_t m_lo            = uint32_and( a, 0x003fffff );
    uint32_t is_m_lo_nez     = uint32_cmpnez( m_lo );
    uint32_t is_snan         = uint32_and( is_flagged, is_m_lo_nez );
    
    return (is_snan);
}

/*----------------------------------------------------------------------------
| Takes two single-precision floating-point values `a' and `b', one of which
| is a NaN, and returns the appropriate NaN result.  If either `a' or `b' is a
| signaling NaN, the invalid exception is raised.
*----------------------------------------------------------------------------*/

static uint32_t cp_float_select_nan( uint32_t a, uint32_t b )
{
    uint32_t a_is_nan, a_is_snan, b_is_nan, b_is_snan;

    a_is_nan  = cp_float_is_nan( a );
    a_is_snan = cp_float_is_snan( a );
    b_is_nan  = cp_float_is_nan( b );
    b_is_snan = cp_float_is_snan( b );

    a |= 0x00400000;
    b |= 0x00400000;

    uint32_t is_both_snan        = uint32_and( a_is_snan, b_is_snan );
    uint32_t is_both_nan         = uint32_and( a_is_nan, b_is_nan );
    uint32_t is_either_snan      = uint32_or( a_is_snan, b_is_snan );
    uint32_t is_either_snan_mask = uint32_cmpnez( is_either_snan );
    uint32_t exception_flags     = uint32_and( is_either_snan_mask, float_flag_invalid );

    cp_float_raise( exception_flags );

    // if ( a_is_snan | b_is_snan ) cp_float_raise( float_flag_invalid );

    if ( ( a_is_snan && b_is_snan ) || ( a_is_nan  && b_is_nan  ) )
    {
        if ( (uint32_t) ( a<<1 ) < (uint32_t) ( b<<1 ) ) return b;
        if ( (uint32_t) ( b<<1 ) < (uint32_t) ( a<<1 ) ) return a;
        return ( a < b ) ? a : b;
    }

    if ( a_is_snan ) {
        // if ( b_is_snan ) goto returnLarger_sificand;
        return b_is_nan ? b : a;
    }

    else if ( a_is_nan ) {
        if ( b_is_snan | ! b_is_nan ) return a;

 // returnLarger_sificand:
        // if ( (uint32_t) ( a<<1 ) < (uint32_t) ( b<<1 ) ) return b;
        // if ( (uint32_t) ( b<<1 ) < (uint32_t) ( a<<1 ) ) return a;
        // return ( a < b ) ? a : b;
    }
    else {
        return b;
    }

   printf("FAIL\n");
   return 0;
}

/*----------------------------------------------------------------------------
| Shifts `a' right by the number of bits given in `count'.  If any nonzero
| bits are shifted off, they are ``jammed'' into the least significant bit of
| the result by setting the least significant bit to 1.  The value of `count'
| can be arbitrarily large; in particular, if `count' is greater than 32, the
| result will be either 0 or 1, depending on whether `a' is zero or nonzero.
| The result is stored in the location pointed to by `zPtr'.
*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
| Shifts `a' right by the number of bits given in `count'.  If any nonzero
| bits are shifted off, they are ``jammed'' into the least significant bit of
| the result by setting the least significant bit to 1.  The value of `count'
| can be arbitrarily large; in particular, if `count' is greater than 64, the
| result will be either 0 or 1, depending on whether `a' is zero or nonzero.
| The result is stored in the location pointed to by `zPtr'.
*----------------------------------------------------------------------------*/

static inline uint32_t uint32_srl_sticky( uint32_t  a, uint32_t count )
{
    uint32_t is_no_shift_mask    = uint32_cmpeqz( count );
    uint32_t is_valid_shift_mask = uint32_cmplt( count, 32 );
    uint32_t is_a_nez_mask       = uint32_cmpnez( a );
    uint32_t overshift_sticky    = uint32_srl( is_a_nez_mask, 31 );
    uint64_t a_shifted           = uint32_srl( a, count );
    uint32_t normal_sticky       = a_shifted | ( ( a<<( ( - count ) & 31 ) ) != 0 );
    uint32_t result_0            = uint32_sel( is_no_shift_mask, a, normal_sticky );
    uint32_t result_1            = uint32_sel( is_valid_shift_mask, result_0, overshift_sticky );
 
    return (result_1);
}


/*----------------------------------------------------------------------------
| Normalizes the subnormal single-precision floating-point value represented
| by the denormalized significand `a_m'.  The normalized exponent and
| significand are stored at the locations pointed to by `z_ePtr' and
| `z_mPtr', respectively.
*----------------------------------------------------------------------------*/

// a_m > 0
static void
normalizeFloat32Subnormal( uint32_t a_m, uint32_t* restrict const out_e, uint32_t* restrict const out_m)
{
    uint32_t m_clz        = __builtin_clz( a_m );
    uint32_t m_sa         = uint32_sub( m_clz, 8 );
    uint32_t m_normalized = uint32_sll( a_m, m_sa );
    uint32_t e_normalized = uint32_sub( 1, m_sa );

    *out_m = m_normalized;
    *out_e = e_normalized;
}


/*----------------------------------------------------------------------------
| Packs the sign `z_s', exponent `z_e', and significand `z_m' into a
| single-precision floating-point value, returning the result.  After being
| shifted into the proper positions, the three fields are simply added
| together to form the result.  This means that any integer portion of `z_m'
| will be added into the exponent.  Since a properly normalized significand
| will have an integer portion equal to 1, the `z_e' input should be 1 less
| than the desired result exponent whenever `z_m' is a complete, normalized
| significand.
*----------------------------------------------------------------------------*/

static inline uint32_t cp_float_pack( uint32_t z_s, uint32_t z_e, uint32_t z_m )
{
    uint32_t result_s  = uint32_sll( z_s, 31 );
    uint32_t result_e  = uint32_sll( z_e, 23 );
    uint32_t result_se = uint32_add( result_s,  result_e );
    uint32_t result    = uint32_add( result_se, z_m );

    return (result);
}

/*----------------------------------------------------------------------------
| Takes an abstract floating-point value having sign `z_s', exponent `z_e',
| and significand `z_m', and returns the proper single-precision floating-
| point value corresponding to the abstract input.  Ordinarily, the abstract
| value is simply rounded and packed into the single-precision format, with
| the inexact exception raised if the abstract input cannot be represented
| exactly.  However, if the abstract value is too large, the overflow and
| inexact exceptions are raised and an infinity or maximal finite value is
| returned.  If the abstract value is too small, the input value is rounded to
| a subnormal number, and the underflow and inexact exceptions are raised if
| the abstract input cannot be represented exactly as a subnormal single-
| precision floating-point number.
|     The input significand `z_m' has its binary point between bits 30
| and 29, which is 7 bits to the left of the usual location.  This shifted
| significand must be normalized or smaller.  If `z_m' is not normalized,
| `z_e' must be 0; in that case, the result returned is a subnormal number,
| and it must not require rounding.  In the usual case that `z_m' is
| normalized, `z_e' must be 1 less than the ``true'' floating-point exponent.
| The handling of underflow and overflow follows the IEC/IEEE Standard for
| Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static uint32_t cp_float_round_and_pack( uint32_t z_s, uint32_t z_e, uint32_t z_m )
{
    uint32_t roundingMode;
    uint32_t roundNearestEven;
    uint32_t roundIncrement, roundBits;
    uint32_t isTiny;

    roundingMode     = cp_float_rounding_mode;
    roundNearestEven = ( roundingMode == float_round_nearest_even );
    roundIncrement   = 0x40;

    if ( !roundNearestEven ) {
        if ( roundingMode == float_round_to_zero ) {
            roundIncrement = 0;
        }
        else {
            roundIncrement = 0x7F;
            if ( z_s ) {
                if ( roundingMode == float_round_up ) roundIncrement = 0;
            }
            else {
                if ( roundingMode == float_round_down ) roundIncrement = 0;
            }
        }
    }

    roundBits = z_m & 0x7F;

    if ( 0xFD <= (uint32_t) z_e ) {
        if (    ( 0xFD < z_e )
             || (    ( z_e == 0xFD )
                  && ( (int32_t) ( z_m + roundIncrement ) < 0 ) )
           ) {
            cp_float_raise( float_flag_overflow | float_flag_inexact );
            return cp_float_pack( z_s, 0xFF, 0 ) - ( roundIncrement == 0 );
        }
        if ( z_e < 0 ) {
            isTiny =
                   ( float_detect_tininess == float_tininess_before_rounding )
                || ( z_e < -1 )
                || ( z_m + roundIncrement < 0x80000000 );
            z_m = uint32_srl_sticky( z_m, - z_e );
            z_e = 0;
            roundBits = z_m & 0x7F;
            if ( isTiny && roundBits ) cp_float_raise( float_flag_underflow );
        }
    }

    if ( roundBits ) float_exception_flags |= float_flag_inexact;

    z_m = ( z_m + roundIncrement )>>7;
    z_m &= ~ ( ( ( roundBits ^ 0x40 ) == 0 ) & roundNearestEven );

    if ( z_m == 0 ) z_e = 0;

    return cp_float_pack( z_s, z_e, z_m );

}



/*----------------------------------------------------------------------------
| Returns the result of dividing the single-precision floating-point value `a'
| by the corresponding value `b'.  The operation is performed according to the
| IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

uint32_t cp_float_div( uint32_t a, uint32_t b )
{
    uint32_t a_m  = cp_float_extract_m( a );
    uint32_t a_e  = cp_float_extract_e( a );
    uint32_t a_s  = cp_float_extract_s( a );
    uint32_t b_m  = cp_float_extract_m( b );
    uint32_t b_e  = cp_float_extract_e( b );
    uint32_t b_s  = cp_float_extract_s( b );
    uint32_t z_s  = uint32_xor( a_s, b_s );
    uint32_t z_e;
    uint32_t z_m;

    if ( a_e == 0xFF ) {
        if ( a_m ) return cp_float_select_nan( a, b );
        if ( b_e == 0xFF ) {
            if ( b_m ) return cp_float_select_nan( a, b );
            cp_float_raise( float_flag_invalid );
            return cp_float_default_nan;
        }
        return cp_float_pack( z_s, 0xFF, 0 );
    }
    if ( b_e == 0xFF ) {
        if ( b_m ) return cp_float_select_nan( a, b );
        return cp_float_pack( z_s, 0, 0 );
    }

    if ( b_e == 0 ) {
        if ( b_m == 0 ) {
            if ( ( a_e | a_m ) == 0 ) {
                cp_float_raise( float_flag_invalid );
                return cp_float_default_nan;
            }
            cp_float_raise( float_flag_divbyzero );
            return cp_float_pack( z_s, 0xFF, 0 );
        }
        normalizeFloat32Subnormal( b_m, &b_e, &b_m );
    }
    if ( a_e == 0 ) {
        if ( a_m == 0 ) return cp_float_pack( z_s, 0, 0 );
        normalizeFloat32Subnormal( a_m, &a_e, &a_m );
    }
    z_e = a_e - b_e + 0x7D;
    a_m = ( a_m | 0x00800000 )<<7;
    b_m = ( b_m | 0x00800000 )<<8;
    if ( b_m <= ( a_m + a_m ) ) {
        a_m >>= 1;
        ++z_e;
    }
    z_m = ( ( (uint64_t) a_m )<<32 ) / b_m;

    if ( ( z_m & 0x3F ) == 0 ) {
        z_m |= ( (uint64_t) b_m * z_m != ( (uint64_t) a_m )<<32 );
    }
    return cp_float_round_and_pack( z_s, z_e, z_m );
}

uint32_t cp_float_mul( uint32_t a, uint32_t b )
{
    uint32_t a_m  = cp_float_extract_m( a );
    uint32_t a_e  = cp_float_extract_e( a );
    uint32_t a_s  = cp_float_extract_s( a );
    uint32_t b_m  = cp_float_extract_m( b );
    uint32_t b_e  = cp_float_extract_e( b );
    uint32_t b_s  = cp_float_extract_s( b );
    uint32_t z_s  = uint32_xor( a_s,  b_s );

    uint32_t z_e;
    uint32_t z_m;
    uint64_t z_m64;

    if ( a_e == 0xFF ) {
        if ( a_m || ( ( b_e == 0xFF ) && b_m ) ) {
            return cp_float_select_nan( a, b );
        }
        if ( ( b_e | b_m ) == 0 ) {
            cp_float_raise( float_flag_invalid );
            return cp_float_default_nan;
        }
        return cp_float_pack( z_s, 0xFF, 0 );
    }

    if ( b_e == 0xFF ) {
        if ( b_m ) return cp_float_select_nan( a, b );
        if ( ( a_e | a_m ) == 0 ) {
            cp_float_raise( float_flag_invalid );
            return cp_float_default_nan;
        }
        return cp_float_pack( z_s, 0xFF, 0 );
    }

    if ( a_e == 0 ) {
        if ( a_m == 0 ) return cp_float_pack( z_s, 0, 0 );
        normalizeFloat32Subnormal( a_m, &a_e, &a_m );
    }

    if ( b_e == 0 ) {
        if ( b_m == 0 ) return cp_float_pack( z_s, 0, 0 );
        normalizeFloat32Subnormal( b_m, &b_e, &b_m );
    }

    z_e = a_e + b_e - 0x7F;
    a_m = ( a_m | 0x00800000 )<<7;
    b_m = ( b_m | 0x00800000 )<<8;
    {
      int64_t a_m64 = a_m;
      int64_t b_m64 = b_m;

      z_m64 = a_m64 * b_m64;
      z_m64 = ( z_m64>>32) | ( ( z_m64<<( ( -32) & 63 ) ) != 0 );
    }
    z_m = z_m64;
    if ( 0 <= (int32_t) ( z_m<<1 ) ) {
        z_m <<= 1;
        --z_e;
    }
    return cp_float_round_and_pack( z_s, z_e, z_m );

}

/*----------------------------------------------------------------------------
| Returns the result of adding the single-precision floating-point values `a'
| and `b'.  The operation is performed according to the IEC/IEEE Standard for
| Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
| Returns the result of adding the absolute values of the single-precision
| floating-point values `a' and `b'.  If `z_s' is 1, the sum is negated
| before being returned.  `z_s' is ignored if the result is a NaN.
| The addition is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
| Returns the result of subtracting the absolute values of the single-
| precision floating-point values `a' and `b'.  If `z_s' is 1, the
| difference is negated before being returned.  `z_s' is ignored if the
| result is a NaN.  The subtraction is performed according to the IEC/IEEE
| Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

uint32_t 
cp_float_add( uint32_t x, uint32_t y )
{
    // Addition between values of differing signs is handled
    // slightly differently.

    uint32_t is_sign_differ_msb           = uint32_xor( x, y );
    uint32_t is_sign_differ_mask          = uint32_sra( is_sign_differ_msb, 31 );

    // extract
    uint32_t x_s                          = cp_float_extract_s( x );
    uint32_t x_e                          = cp_float_extract_e( x );
    uint32_t x_m                          = cp_float_extract_m( x );
    uint32_t y_s                          = cp_float_extract_s( y );
    uint32_t y_e                          = cp_float_extract_e( y );
    uint32_t y_m                          = cp_float_extract_m( y );

    // swap
    uint32_t diff_e                       = uint32_sub( x_e, y_e );
    uint32_t diff_m                       = uint32_sub( x_m, y_m );
    uint32_t sum_m                        = uint32_add( x_m, y_m );
    uint32_t is_x_e_smaller_mask          = uint32_sra( diff_e, 31 );
    uint32_t is_both_e_same_mask          = uint32_cmpeqz( diff_e );
    uint32_t is_x_m_smaller_mask          = uint32_sra( diff_m, 31 );
    uint32_t swap_m_mask                  = uint32_and( is_both_e_same_mask, is_x_m_smaller_mask );
    uint32_t swap_mask                    = uint32_or( is_x_e_smaller_mask, swap_m_mask );
    uint32_t abs_diff_e                   = uint32_selneg( swap_mask, diff_e );

    uint32_t a                            = uint32_sel( swap_mask, y,    x    );
    uint32_t a_s                          = uint32_sel( swap_mask, y_s,  x_s  );
    uint32_t a_e                          = uint32_sel( swap_mask, y_e,  x_e  );
    uint32_t b                            = uint32_sel( swap_mask, x,    y    );
    uint32_t b_e                          = uint32_sel( swap_mask, x_e,  y_e  );
    uint32_t a_m_unchanged                = uint32_sel( swap_mask, y_m,  x_m  );
    uint32_t b_m_unchanged                = uint32_sel( swap_mask, x_m,  y_m  );
    uint32_t ss_a_m                       = uint32_sll( a_m_unchanged, 6 );
    uint32_t ss_b_m                       = uint32_sll( b_m_unchanged, 6 );
    uint32_t ds_a_m                       = uint32_sll( a_m_unchanged, 7 );
    uint32_t ds_b_m                       = uint32_sll( b_m_unchanged, 7 );
    uint32_t a_m                          = uint32_sel( is_sign_differ_mask, ds_a_m, ss_a_m );
    uint32_t b_m                          = uint32_sel( is_sign_differ_mask, ds_b_m, ss_b_m );

    uint32_t nan_result                   = cp_float_select_nan( a, b );
    uint32_t is_a_e_flagged_mask          = uint32_cmpeq( a_e, 0xff );
    uint32_t is_b_e_eqz_mask              = uint32_cmpeqz( b_e );
    uint32_t is_a_e_nez_mask              = uint32_cmpnez( a_e );
    uint32_t is_a_e_eqz_mask              = uint32_not( is_a_e_nez_mask );
    uint32_t is_a_m_nez_mask              = uint32_cmpnez( a_m );
    uint32_t is_both_m_nez                = uint32_or( a_m, b_m );
    uint32_t is_both_m_nez_mask           = uint32_cmpnez( is_both_m_nez );
    uint32_t is_nan_result_mask           = uint32_and(  is_a_e_flagged_mask, is_both_m_nez_mask );
    uint32_t is_inf_result_mask           = uint32_andc( is_a_e_flagged_mask, is_both_m_nez_mask );

    // Different exponent (de_*)

    uint32_t de_flagged_result            = uint32_sel( is_a_m_nez_mask, nan_result, a );
    uint32_t de_hidden_m_add              = uint32_li( 0x20000000 );
    uint32_t de_hidden_m_sub              = uint32_li( 0x40000000 );
    uint32_t de_hidden_m                  = uint32_sel( is_sign_differ_mask, de_hidden_m_sub, de_hidden_m_add );
    uint32_t de_m_sa                      = uint32_add( abs_diff_e, is_b_e_eqz_mask );
    uint32_t de_a_m_hidden                = uint32_or( a_m, de_hidden_m );
    uint32_t de_b_m_hidden                = uint32_or( b_m, de_hidden_m );
    uint32_t de_b_m_hidden_shifted        = uint32_srl_sticky( de_b_m_hidden, de_m_sa );
    uint32_t de_add_m                     = uint32_add( de_a_m_hidden, de_b_m_hidden_shifted );
    uint32_t de_add_m_shifted             = uint32_sll( de_add_m, 1 );
    uint32_t de_is_add_m_shifted_ltz_mask = uint32_sra( de_add_m_shifted, 31 );
    uint32_t de_sub_m                     = uint32_sub( de_a_m_hidden, de_b_m_hidden_shifted );
    uint32_t de_z_m_add_0                 = uint32_sel( de_is_add_m_shifted_ltz_mask,  de_add_m, de_add_m_shifted );
    uint32_t de_z_m_add_1                 = uint32_sel( is_sign_differ_mask,        de_sub_m, de_z_m_add_0 );
    uint32_t de_z_e_shift_0               = uint32_andc( de_is_add_m_shifted_ltz_mask, is_sign_differ_mask );
    uint32_t de_z_e_shift_1               = uint32_add( de_z_e_shift_0, 1 );
    uint32_t de_z_e_add                   = uint32_sub( a_e, de_z_e_shift_1 );


    // Same exponent, different sign (se_ds_*)

    uint32_t se_ds_d_diff_m               = uint32_sub( b_m, a_m );
    uint32_t se_ds_d_neg_diff_m           = uint32_neg( se_ds_d_diff_m );
    uint32_t se_ds_d_diff_m_nez_mask      = uint32_sra( se_ds_d_diff_m, 31 );
    uint32_t se_ds_e                      = uint32_add( a_e, is_a_e_nez_mask );
    uint32_t se_ds_is_nan_result_mask     = uint32_and( is_a_e_flagged_mask, is_both_m_nez_mask );

    uint32_t se_norm_e                    = uint32_sel( is_sign_differ_mask, se_ds_e,            a_e );
    uint32_t se_norm_m                    = uint32_sel( is_sign_differ_mask, se_ds_d_neg_diff_m, de_add_m );
    uint32_t norm_e_unshifted             = uint32_sel( is_both_e_same_mask, se_norm_e,          de_z_e_add   );
    uint32_t norm_m_unshifted             = uint32_sel( is_both_e_same_mask, se_norm_m,          de_z_m_add_1 );

    /*----------------------------------------------------------------------------
    | Takes an abstract floating-point value having sign `z_s', exponent `z_e',
    | and significand `z_m', and returns the proper single-precision floating-
    | point value corresponding to the abstract input.  This routine is just like
    | `cp_float_round_and_pack' except that `z_m' does not have to be normalized.
    | Bit 31 of `z_m' must be zero, and `z_e' must be 1 less than the ``true''
    | floating-point exponent.
    *----------------------------------------------------------------------------*/

    uint32_t norm_m_nlz                   = __builtin_clz( norm_m_unshifted );
    uint32_t norm_sa                      = uint32_sub( norm_m_nlz, 1 );
    uint32_t norm_e                       = uint32_sub( norm_e_unshifted, norm_sa );
    uint32_t norm_m                       = uint32_sll( norm_m_unshifted, norm_sa );
    uint32_t norm_result                  = cp_float_round_and_pack( a_s, norm_e, norm_m );

    // Same exponent, same sign (se_ss_*)

    uint32_t se_ss_denorm_result          = cp_float_pack( a_s, 0, sum_m );
    uint32_t se_ss_result0                = uint32_sel( is_nan_result_mask,    nan_result,           norm_result   );
    uint32_t se_ss_result1                = uint32_sel( is_a_e_eqz_mask,       se_ss_denorm_result,  se_ss_result0 );
    uint32_t se_ss_result2                = uint32_sel( is_inf_result_mask,    a,                    se_ss_result1 );

    uint32_t se_ds_value3                 = cp_float_pack( cp_float_rounding_mode == float_round_down, 0, 0 );
    uint32_t se_ds_result0                = uint32_sel( is_a_e_flagged_mask,       cp_float_default_nan, se_ds_value3  );
    uint32_t se_ds_result1                = uint32_sel( se_ds_is_nan_result_mask,  nan_result,          se_ds_result0 );
    uint32_t se_ds_result2                = uint32_sel( se_ds_d_diff_m_nez_mask,   norm_result,        se_ds_result1 );

    // Same exponent (se_*)

    uint32_t de_result                    = uint32_sel( is_a_e_flagged_mask, de_flagged_result, norm_result );
    uint32_t se_result                    = uint32_sel( is_sign_differ_mask, se_ds_result2, se_ss_result2 );

    uint32_t result                       = uint32_sel( is_both_e_same_mask, se_result, de_result );

    return (result);
}


/*----------------------------------------------------------------------------
| Returns the result of subtracting the single-precision floating-point values
| `a' and `b'.  The operation is performed according to the IEC/IEEE Standard
| for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

uint32_t 
cp_float_sub( uint32_t a, uint32_t b )
{
    return cp_float_add( a, b ^ 0x80000000 );
}
