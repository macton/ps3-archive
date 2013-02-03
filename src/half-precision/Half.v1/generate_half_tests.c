#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define HALF_EPSILON 0.00097656f

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

    printf("print \"    test.f32 = %ff;\\n\"\n", f32);
    printf("print \"    CHECK_EQUAL( test.u32, 0x\", f32, \" )\\n\"\n" );
    printf("print \"    test.f16 = convert_half( 0x\", f32, \" )\\n\"\n" );
    printf("print \"    CHECK_EQUAL( test_f16, 0x\", f16, \" )\\n\"\n" );
}

int
main( void )
{
    printf("obase=16\n");
    printf("ibase=2\n");

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


    printf("quit\n");
    return (0);
}
