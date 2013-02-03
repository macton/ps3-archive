#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <float.h>

void 
testBits ( float f32, const char* f16_binstr, const char* f32_binstr )
{
    int len;
    int i;

    printf("f16 = ");

    len = strlen( f16_binstr );
    for (i=0;i<len;i++)
    {
        if ( f16_binstr[i] == '1' )
        {
            printf("1");
        }
        else if ( f16_binstr[i] == '0' )
        {
            printf("0");
        }
    }

    printf("\n");

    printf("f32 = ");

    len = strlen( f32_binstr );
    for (i=0;i<len;i++)
    {
        if ( f32_binstr[i] == '1' )
        {
            printf("1");
        }
        else if ( f32_binstr[i] == '0' )
        {
            printf("0");
        }
    }

    printf("\n");

    printf("print \"    test_f16 = half_convert( 0x\", f32, \" );\\n\"\n" );
    printf("print \"    CHECK_HALF( test_f16, 0x\", f16, \", 0x\", f32, \" )\\n\"\n" );
}

void print_half_tests( void );

int
main( void )
{
    printf("obase=16\n");
    printf("ibase=2\n");

    printf("print \"#include <stdio.h>\\n\"\n" );
    printf("print \"#include <stdint.h>\\n\"\n" );
    printf("print \"typedef union FLOAT_32 FLOAT_32;\\n\"\n" );
    printf("print \"union FLOAT_32\\n\"\n" );
    printf("print \"{\\n\"\n" );
    printf("print \"    float    f32;\\n\"\n" );
    printf("print \"    uint32_t u32;\\n\"\n" );
    printf("print \"};\\n\"\n" );
    printf("print \"uint16_t half_convert( uint32_t );\\n\"\n" );
    printf("print \"\\n\"\n" );
    printf("print \"#define CHECK_HALF(arg0,arg1,arg2) if (arg0 != arg1) { FLOAT_32 f; f.u32 = arg2; test_count++; printf(\\q[%%s:%%d] FAILED: 0x%%04x != 0x%%04x (%%f)\\\\n\\q,__FILE__,__LINE__,arg0,arg1,f.f32); } else { test_count++; test_pass_count++; }\\n\"\n" );
    printf("print \"\\n\"\n" );

    printf("print \"int main( void )\\n\"\n" );
    printf("print \"{\\n\"\n" );
    printf("print \"    FLOAT_32 test;\\n\"\n" );
    printf("print \"    uint16_t test_f16;\\n\"\n" );
    printf("print \"    int      test_count = 0;\\n\"\n" );
    printf("print \"    int      test_pass_count = 0;\\n\"\n" );
    printf("print \"\\n\"\n" );

    print_half_tests();

    printf("print \"\\n\"\n" );
    printf("print \"    printf(\\q[%%s] Passed %%d of %%d\\\\n\\q, __FILE__, test_pass_count, test_count);\\n\"\n" );
    printf("print \"    return (0);\\n\"\n" );
    printf("print \"}\\n\"\n" );

    printf("quit\n");
    return (0);
}

// 
// NOTE: Tests below taken from OpenEXR halfTests/testBitPatterns.cpp
// 

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

#define HALF_MIN	5.96046448e-08	// Smallest positive half

#define HALF_NRM_MIN	6.10351562e-05	// Smallest positive normalized half

#define HALF_MAX	65504.0		// Largest positive half

#define HALF_EPSILON	0.00097656	// Smallest positive e for which

#define HALF_MANT_DIG	11		// Number of digits in mantissa
					// (significand + hidden leading 1)

#define HALF_DIG	2		// Number of base 10 digits that
					// can be represented without change

#define HALF_RADIX	2		// Base of the exponent

#define HALF_MIN_EXP	-13		// Minimum negative integer such that
					// HALF_RADIX raised to the power of
					// one less than that integer is a
					// normalized half

#define HALF_MAX_EXP	16		// Maximum positive integer such that
					// HALF_RADIX raised to the power of
					// one less than that integer is a
					// normalized half

#define HALF_MIN_10_EXP	-4		// Minimum positive integer such
					// that 10 raised to that power is
					// a normalized half

#define HALF_MAX_10_EXP	4		// Maximum positive integer such
					// that 10 raised to that power is
					// a normalized half

void
print_half_tests( void )
{
    testBits (-0.0,
	      "1 00000 0000000000",
	      "1 00000000 00000000000000000000000");

    testBits (0.0,
	      "0 00000 0000000000",
	      "0 00000000 00000000000000000000000");
    //
    // Numbers close to 1.0
    //

    testBits (1.0,
	      "0 01111 0000000000",
	      "0 01111111 00000000000000000000000");
    testBits (1.0 + HALF_EPSILON,
	      "0 01111 0000000001",
	      "0 01111111 00000000010000000000000");
    testBits (1.0 + HALF_EPSILON * 0.5,
	      "0 01111 0000000001",
	      "0 01111111 00000000010000000000000");
    testBits (1.0 + HALF_EPSILON * 0.4999,
	      "0 01111 0000000000",
	      "0 01111111 00000000000000000000000");
    testBits (1.0 + HALF_EPSILON * 0.5001,
	      "0 01111 0000000001",
	      "0 01111111 00000000010000000000000");
    testBits (1.0 - HALF_EPSILON * 0.5,
	      "0 01110 1111111111",
	      "0 01111110 11111111110000000000000");
    testBits (1.0 - HALF_EPSILON * 0.5 * 0.5,
	      "0 01111 0000000000",
	      "0 01111111 00000000000000000000000");
    testBits (1.0 - HALF_EPSILON * 0.5 * 0.4999,
	      "0 01111 0000000000",
	      "0 01111111 00000000000000000000000");
    testBits (1.0 - HALF_EPSILON * 0.5 * 0.5001,
	      "0 01110 1111111111",
	      "0 01111110 11111111110000000000000");

    //
    // Numbers close to HALF_MIN
    //

    testBits (HALF_MIN,
	      "0 00000 0000000001",
	      "0 01100111 00000000000000000000000");
    testBits (HALF_MIN + HALF_MIN,
	      "0 00000 0000000010",
	      "0 01101000 00000000000000000000000");
    testBits (HALF_MIN + HALF_MIN * 0.5,
	      "0 00000 0000000010",
	      "0 01101000 00000000000000000000000");
    testBits (HALF_MIN + HALF_MIN * 0.4999,
	      "0 00000 0000000001",
	      "0 01100111 00000000000000000000000");
    testBits (HALF_MIN + HALF_MIN * 0.5001,
	      "0 00000 0000000010",
	      "0 01101000 00000000000000000000000");
    testBits (HALF_MIN - HALF_MIN,
	      "0 00000 0000000000",
	      "0 00000000 00000000000000000000000");
    testBits (HALF_MIN - HALF_MIN * 0.5,
	      "0 00000 0000000001",
	      "0 01100111 00000000000000000000000");
    testBits (HALF_MIN - HALF_MIN * 0.4999,
	      "0 00000 0000000001",
	      "0 01100111 00000000000000000000000");
    testBits (HALF_MIN - HALF_MIN * 0.5001,
	      "0 00000 0000000000",
	      "0 00000000 00000000000000000000000");

    //
    // Numbers close to HALF_NRM_MIN
    //

    testBits (HALF_NRM_MIN,
	      "0 00001 0000000000",
	      "0 01110001 00000000000000000000000");
    testBits (HALF_NRM_MIN + HALF_MIN,
	      "0 00001 0000000001",
	      "0 01110001 00000000010000000000000");
    testBits (HALF_NRM_MIN + HALF_MIN * 0.5,
	      "0 00001 0000000001",
	      "0 01110001 00000000010000000000000");
    testBits (HALF_NRM_MIN + HALF_MIN * 0.4999,
	      "0 00001 0000000000",
	      "0 01110001 00000000000000000000000");
    testBits (HALF_NRM_MIN + HALF_MIN * 0.5001,
	      "0 00001 0000000001",
	      "0 01110001 00000000010000000000000");
    testBits (HALF_NRM_MIN - HALF_MIN,
	      "0 00000 1111111111",
	      "0 01110000 11111111100000000000000");
    testBits (HALF_NRM_MIN - HALF_MIN * 0.5,
	      "0 00001 0000000000",
	      "0 01110001 00000000000000000000000");
    testBits (HALF_NRM_MIN - HALF_MIN * 0.49995,
	      "0 00001 0000000000",
	      "0 01110001 00000000000000000000000");
    testBits (HALF_NRM_MIN - HALF_MIN * 0.50005,
	      "0 00000 1111111111",
	      "0 01110000 11111111100000000000000");

    //
    // Small positive integers and simple decimal fractions
    //

    testBits (2,
	      "0 10000 0000000000",
	      "0 10000000 00000000000000000000000");
    testBits (3,
	      "0 10000 1000000000",
	      "0 10000000 10000000000000000000000");
    testBits (10,
	      "0 10010 0100000000",
	      "0 10000010 01000000000000000000000");
    testBits (0.1,
	      "0 01011 1001100110",
	      "0 01111011 10011001100000000000000");
    testBits (0.2,
	      "0 01100 1001100110",
	      "0 01111100 10011001100000000000000");
    testBits (0.3,
	      "0 01101 0011001101",
	      "0 01111101 00110011010000000000000");

    //
    // Numbers close to HALF_MAX
    //

    testBits (HALF_MAX,
	      "0 11110 1111111111",
	      "0 10001110 11111111110000000000000");
    testBits ((1 << HALF_MAX_EXP) * 1.0,
	      "0 11111 0000000000",			// +infinity
	      "0 11111111 00000000000000000000000");	// +infinity
    testBits ((1 << HALF_MAX_EXP) * (1.0 - HALF_EPSILON * 0.25),
	      "0 11111 0000000000",			// +infinity
	      "0 11111111 00000000000000000000000");	// +infinity
    testBits ((1 << HALF_MAX_EXP) * (1.0 - HALF_EPSILON * 0.25005),
	      "0 11110 1111111111",
	      "0 10001110 11111111110000000000000");
    testBits ((1 << HALF_MAX_EXP) * (1.0 - HALF_EPSILON * 0.24995),
	      "0 11111 0000000000",			// +infinity
	      "0 11111111 00000000000000000000000");	// +infinity

    //
    // Large positive numbers, positive infinity and NANs
    //

    testBits (HALF_MAX * HALF_MAX,
	      "0 11111 0000000000",			// +infinity
	      "0 11111111 00000000000000000000000");	// +infinity
    testBits (FLT_MAX,
	      "0 11111 0000000000",			// +infinity
	      "0 11111111 00000000000000000000000");	// +infinity
#if 0
    testBits (floatPosInfinity(),
	      "0 11111 0000000000",			// +infinity
	      "0 11111111 00000000000000000000000");	// +infinity
    testBits (floatPosQNan1(),
	      "0 11111 1111111111",			// nan
	      "0 11111111 11111111110000000000000");	// nan
    testBits (floatPosQNan2(),
	      "0 11111 1010101010",			// nan
	      "0 11111111 10101010100000000000000");	// nan
#endif

    //
    // Numbers close to -1.0
    //

    testBits (-1.0,
	      "1 01111 0000000000",
	      "1 01111111 00000000000000000000000");
    testBits (-(1.0 + HALF_EPSILON),
	      "1 01111 0000000001",
	      "1 01111111 00000000010000000000000");
    testBits (-(1.0 + HALF_EPSILON * 0.5),
	      "1 01111 0000000001",
	      "1 01111111 00000000010000000000000");
    testBits (-(1.0 + HALF_EPSILON * 0.4999),
	      "1 01111 0000000000",
	      "1 01111111 00000000000000000000000");
    testBits (-(1.0 + HALF_EPSILON * 0.5001),
	      "1 01111 0000000001",
	      "1 01111111 00000000010000000000000");
    testBits (-(1.0 - HALF_EPSILON * 0.5),
	      "1 01110 1111111111",
	      "1 01111110 11111111110000000000000");
    testBits (-(1.0 - HALF_EPSILON * 0.5 * 0.5),
	      "1 01111 0000000000",
	      "1 01111111 00000000000000000000000");
    testBits (-(1.0 - HALF_EPSILON * 0.5 * 0.4999),
	      "1 01111 0000000000",
	      "1 01111111 00000000000000000000000");
    testBits (-(1.0 - HALF_EPSILON * 0.5 * 0.5001),
	      "1 01110 1111111111",
	      "1 01111110 11111111110000000000000");

    //
    // Numbers close to -HALF_MIN
    //

    testBits (-HALF_MIN,
	      "1 00000 0000000001",
	      "1 01100111 00000000000000000000000");
    testBits (-(HALF_MIN + HALF_MIN),
	      "1 00000 0000000010",
	      "1 01101000 00000000000000000000000");
    testBits (-(HALF_MIN + HALF_MIN * 0.5),
	      "1 00000 0000000010",
	      "1 01101000 00000000000000000000000");
    testBits (-(HALF_MIN + HALF_MIN * 0.4999),
	      "1 00000 0000000001",
	      "1 01100111 00000000000000000000000");
    testBits (-(HALF_MIN + HALF_MIN * 0.5001),
	      "1 00000 0000000010",
	      "1 01101000 00000000000000000000000");
    testBits (-(HALF_MIN - HALF_MIN),
	      "0 00000 0000000000",
	      "0 00000000 00000000000000000000000");
    testBits (-(HALF_MIN - HALF_MIN * 0.5),
	      "1 00000 0000000001",
	      "1 01100111 00000000000000000000000");
    testBits (-(HALF_MIN - HALF_MIN * 0.4999),
	      "1 00000 0000000001",
	      "1 01100111 00000000000000000000000");
    testBits (-(HALF_MIN - HALF_MIN * 0.5001),
	      "0 00000 0000000000",
	      "0 00000000 00000000000000000000000");

    //
    // Numbers close to -HALF_NRM_MIN
    //

    testBits (-HALF_NRM_MIN,
	      "1 00001 0000000000",
	      "1 01110001 00000000000000000000000");
    testBits (-(HALF_NRM_MIN + HALF_MIN),
	      "1 00001 0000000001",
	      "1 01110001 00000000010000000000000");
    testBits (-(HALF_NRM_MIN + HALF_MIN * 0.5),
	      "1 00001 0000000001",
	      "1 01110001 00000000010000000000000");
    testBits (-(HALF_NRM_MIN + HALF_MIN * 0.4999),
	      "1 00001 0000000000",
	      "1 01110001 00000000000000000000000");
    testBits (-(HALF_NRM_MIN + HALF_MIN * 0.5001),
	      "1 00001 0000000001",
	      "1 01110001 00000000010000000000000");
    testBits (-(HALF_NRM_MIN - HALF_MIN),
	      "1 00000 1111111111",
	      "1 01110000 11111111100000000000000");
    testBits (-(HALF_NRM_MIN - HALF_MIN * 0.5),
	      "1 00001 0000000000",
	      "1 01110001 00000000000000000000000");
    testBits (-(HALF_NRM_MIN - HALF_MIN * 0.49995),
	      "1 00001 0000000000",
	      "1 01110001 00000000000000000000000");
    testBits (-(HALF_NRM_MIN - HALF_MIN * 0.50005),
	      "1 00000 1111111111",
	      "1 01110000 11111111100000000000000");

    //
    // Small negative integers and simple decimal fractions
    //

    testBits (-2,
	      "1 10000 0000000000",
	      "1 10000000 00000000000000000000000");
    testBits (-3,
	      "1 10000 1000000000",
	      "1 10000000 10000000000000000000000");
    testBits (-10,
	      "1 10010 0100000000",
	      "1 10000010 01000000000000000000000");
    testBits (-0.1,
	      "1 01011 1001100110",
	      "1 01111011 10011001100000000000000");
    testBits (-0.2,
	      "1 01100 1001100110",
	      "1 01111100 10011001100000000000000");
    testBits (-0.3,
	      "1 01101 0011001101",
	      "1 01111101 00110011010000000000000");

    //
    // Numbers close to -HALF_MAX
    //

    testBits (-HALF_MAX,
	      "1 11110 1111111111",
	      "1 10001110 11111111110000000000000");
    testBits (-(1 << HALF_MAX_EXP) * 1.0,
	      "1 11111 0000000000",			// +infinity
	      "1 11111111 00000000000000000000000");	// +infinity
    testBits (-(1 << HALF_MAX_EXP) * (1.0 - HALF_EPSILON * 0.25),
	      "1 11111 0000000000",			// +infinity
	      "1 11111111 00000000000000000000000");	// +infinity
    testBits (-(1 << HALF_MAX_EXP) * (1.0 - HALF_EPSILON * 0.25005),
	      "1 11110 1111111111",
	      "1 10001110 11111111110000000000000");
    testBits (-(1 << HALF_MAX_EXP) * (1.0 - HALF_EPSILON * 0.24995),
	      "1 11111 0000000000",			// +infinity
	      "1 11111111 00000000000000000000000");	// +infinity

#if 0
    //
    // Large negative numbers, negative infinity and NANs
    //

    testBits (-HALF_MAX * HALF_MAX,
	      "1 11111 0000000000",			// +infinity
	      "1 11111111 00000000000000000000000");	// +infinity
    testBits (-FLT_MAX,
	      "1 11111 0000000000",			// +infinity
	      "1 11111111 00000000000000000000000");	// +infinity
    testBits (floatNegInfinity(),
	      "1 11111 0000000000",			// +infinity
	      "1 11111111 00000000000000000000000");	// +infinity
    testBits (floatNegQNan1(),
	      "1 11111 1111111111",			// nan
	      "1 11111111 11111111110000000000000");	// nan
    testBits (floatNegQNan2(),
	      "1 11111 1010101010",			// nan
	      "1 11111111 10101010100000000000000");	// nan
#endif
}
