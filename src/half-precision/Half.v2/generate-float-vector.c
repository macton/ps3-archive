/*
===============================================================================

This C source file is part of TestFloat, Release 2a, a package of programs
for testing the correctness of floating-point arithmetic complying to the
IEC/IEEE Standard for Floating-Point.

Written by John R. Hauser.  More information is available through the Web
page `http://HTTP.CS.Berkeley.EDU/~jhauser/arithmetic/TestFloat.html'.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort
has been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT
TIMES RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO
PERSONS AND ORGANIZATIONS WHO CAN AND WILL TAKE FULL RESPONSIBILITY FOR ANY
AND ALL LOSSES, COSTS, OR OTHER PROBLEMS ARISING FROM ITS USE.

Derivative works are acceptable, even for commercial purposes, so long as
(1) they include prominent notice that the work is derivative, and (2) they
include prominent notice akin to these four paragraphs for those parts of
this code that are retained.

===============================================================================
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>


uint8_t randomUint8( void )
{
    return (uint8_t) ( rand()>>4 );
}

uint16_t randomUint16( void )
{
    return ( ( rand() & 0x0FF0 )<<4 ) | ( ( rand()>>4 ) & 0xFF );
}

uint32_t randomUint32( void )
{
    return
          ( ( (uint32_t) ( rand() & 0x0FF0 ) )<<20 )
        | ( ( (uint32_t) ( rand() & 0x0FF0 ) )<<12 )
        | ( ( rand() & 0x0FF0 )<<4 )
        | ( ( rand()>>4 ) & 0xFF );
}

enum {
    int32NumP1 = 124
};

static const uint32_t int32P1[ int32NumP1 ] = {
    0x00000000,
    0x00000001,
    0x00000002,
    0x00000004,
    0x00000008,
    0x00000010,
    0x00000020,
    0x00000040,
    0x00000080,
    0x00000100,
    0x00000200,
    0x00000400,
    0x00000800,
    0x00001000,
    0x00002000,
    0x00004000,
    0x00008000,
    0x00010000,
    0x00020000,
    0x00040000,
    0x00080000,
    0x00100000,
    0x00200000,
    0x00400000,
    0x00800000,
    0x01000000,
    0x02000000,
    0x04000000,
    0x08000000,
    0x10000000,
    0x20000000,
    0x40000000,
    0x80000000,
    0xC0000000,
    0xE0000000,
    0xF0000000,
    0xF8000000,
    0xFC000000,
    0xFE000000,
    0xFF000000,
    0xFF800000,
    0xFFC00000,
    0xFFE00000,
    0xFFF00000,
    0xFFF80000,
    0xFFFC0000,
    0xFFFE0000,
    0xFFFF0000,
    0xFFFF8000,
    0xFFFFC000,
    0xFFFFE000,
    0xFFFFF000,
    0xFFFFF800,
    0xFFFFFC00,
    0xFFFFFE00,
    0xFFFFFF00,
    0xFFFFFF80,
    0xFFFFFFC0,
    0xFFFFFFE0,
    0xFFFFFFF0,
    0xFFFFFFF8,
    0xFFFFFFFC,
    0xFFFFFFFE,
    0xFFFFFFFF,
    0xFFFFFFFD,
    0xFFFFFFFB,
    0xFFFFFFF7,
    0xFFFFFFEF,
    0xFFFFFFDF,
    0xFFFFFFBF,
    0xFFFFFF7F,
    0xFFFFFEFF,
    0xFFFFFDFF,
    0xFFFFFBFF,
    0xFFFFF7FF,
    0xFFFFEFFF,
    0xFFFFDFFF,
    0xFFFFBFFF,
    0xFFFF7FFF,
    0xFFFEFFFF,
    0xFFFDFFFF,
    0xFFFBFFFF,
    0xFFF7FFFF,
    0xFFEFFFFF,
    0xFFDFFFFF,
    0xFFBFFFFF,
    0xFF7FFFFF,
    0xFEFFFFFF,
    0xFDFFFFFF,
    0xFBFFFFFF,
    0xF7FFFFFF,
    0xEFFFFFFF,
    0xDFFFFFFF,
    0xBFFFFFFF,
    0x7FFFFFFF,
    0x3FFFFFFF,
    0x1FFFFFFF,
    0x0FFFFFFF,
    0x07FFFFFF,
    0x03FFFFFF,
    0x01FFFFFF,
    0x00FFFFFF,
    0x007FFFFF,
    0x003FFFFF,
    0x001FFFFF,
    0x000FFFFF,
    0x0007FFFF,
    0x0003FFFF,
    0x0001FFFF,
    0x0000FFFF,
    0x00007FFF,
    0x00003FFF,
    0x00001FFF,
    0x00000FFF,
    0x000007FF,
    0x000003FF,
    0x000001FF,
    0x000000FF,
    0x0000007F,
    0x0000003F,
    0x0000001F,
    0x0000000F,
    0x00000007,
    0x00000003
};

static const int32_t int32NumP2 = ( int32NumP1 * int32NumP1 + int32NumP1 ) / 2;

static int32_t int32RandomP3( void )
{

    return
        (int32_t) (
              int32P1[ randomUint8() % int32NumP1 ]
            + int32P1[ randomUint8() % int32NumP1 ]
            + int32P1[ randomUint8() % int32NumP1 ]
        );

}

enum {
    int32NumPInfWeightMasks = 29
};

static const uint32_t int32PInfWeightMasks[ int32NumPInfWeightMasks ] = {
    0xFFFFFFFF,
    0x7FFFFFFF,
    0x3FFFFFFF,
    0x1FFFFFFF,
    0x0FFFFFFF,
    0x07FFFFFF,
    0x03FFFFFF,
    0x01FFFFFF,
    0x00FFFFFF,
    0x007FFFFF,
    0x003FFFFF,
    0x001FFFFF,
    0x000FFFFF,
    0x0007FFFF,
    0x0003FFFF,
    0x0001FFFF,
    0x0000FFFF,
    0x00007FFF,
    0x00003FFF,
    0x00001FFF,
    0x00000FFF,
    0x000007FF,
    0x000003FF,
    0x000001FF,
    0x000000FF,
    0x0000007F,
    0x0000003F,
    0x0000001F,
    0x0000000F
};

static const uint32_t int32PInfWeightOffsets[ int32NumPInfWeightMasks ] = {
    0x00000000,
    0xC0000000,
    0xE0000000,
    0xF0000000,
    0xF8000000,
    0xFC000000,
    0xFE000000,
    0xFF000000,
    0xFF800000,
    0xFFC00000,
    0xFFE00000,
    0xFFF00000,
    0xFFF80000,
    0xFFFC0000,
    0xFFFE0000,
    0xFFFF0000,
    0xFFFF8000,
    0xFFFFC000,
    0xFFFFE000,
    0xFFFFF000,
    0xFFFFF800,
    0xFFFFFC00,
    0xFFFFFE00,
    0xFFFFFF00,
    0xFFFFFF80,
    0xFFFFFFC0,
    0xFFFFFFE0,
    0xFFFFFFF0,
    0xFFFFFFF8
};

static int32_t int32RandomPInf( void )
{
    int8_t weightMaskNum;

    weightMaskNum = randomUint8() % int32NumPInfWeightMasks;
    return
        (int32_t) (
              ( randomUint32() & int32PInfWeightMasks[ weightMaskNum ] )
            + int32PInfWeightOffsets[ weightMaskNum ]
        );

}

enum {
    floatNumQIn  = 22,
    floatNumQOut = 50,
    floatNumP1   =  4,
    floatNumP2   = 88
};

static const uint32_t floatQIn[ floatNumQIn ] = {
    0x00000000,		/* positive, subnormal		*/
    0x00800000,		/* positive, -126		*/
    0x33800000,		/* positive,  -24		*/
    0x3E800000,		/* positive,   -2		*/
    0x3F000000,		/* positive,   -1		*/
    0x3F800000,		/* positive,    0		*/
    0x40000000,		/* positive,    1		*/
    0x40800000,		/* positive,    2		*/
    0x4B800000,		/* positive,   24		*/
    0x7F000000,		/* positive,  127		*/
    0x7F800000,		/* positive, infinity or NaN	*/
    0x80000000,		/* negative, subnormal		*/
    0x80800000,		/* negative, -126		*/
    0xB3800000,		/* negative,  -24		*/
    0xBE800000,		/* negative,   -2		*/
    0xBF000000,		/* negative,   -1		*/
    0xBF800000,		/* negative,    0		*/
    0xC0000000,		/* negative,    1		*/
    0xC0800000,		/* negative,    2		*/
    0xCB800000,		/* negative,   24		*/
    0xFE800000,		/* negative,  126		*/
    0xFF800000		/* negative, infinity or NaN	*/
};

static const uint32_t floatQOut[ floatNumQOut ] = {
    0x00000000,		/* positive, subnormal		*/
    0x00800000,		/* positive, -126		*/
    0x01000000,		/* positive, -125		*/
    0x33800000,		/* positive,  -24		*/
    0x3D800000,		/* positive,   -4		*/
    0x3E000000,		/* positive,   -3		*/
    0x3E800000,		/* positive,   -2		*/
    0x3F000000,		/* positive,   -1		*/
    0x3F800000,		/* positive,    0		*/
    0x40000000,		/* positive,    1		*/
    0x40800000,		/* positive,    2		*/
    0x41000000,		/* positive,    3		*/
    0x41800000,		/* positive,    4		*/
    0x4B800000,		/* positive,   24		*/
    0x4E000000,		/* positive,   29		*/
    0x4E800000,		/* positive,   30		*/
    0x4F000000,		/* positive,   31		*/
    0x4F800000,		/* positive,   32		*/
    0x5E000000,		/* positive,   61		*/
    0x5E800000,		/* positive,   62		*/
    0x5F000000,		/* positive,   63		*/
    0x5F800000,		/* positive,   64		*/
    0x7E800000,		/* positive,  126		*/
    0x7F000000,		/* positive,  127		*/
    0x7F800000,		/* positive, infinity or NaN	*/
    0x80000000,		/* negative, subnormal		*/
    0x80800000,		/* negative, -126		*/
    0x81000000,		/* negative, -125		*/
    0xB3800000,		/* negative,  -24		*/
    0xBD800000,		/* negative,   -4		*/
    0xBE000000,		/* negative,   -3		*/
    0xBE800000,		/* negative,   -2		*/
    0xBF000000,		/* negative,   -1		*/
    0xBF800000,		/* negative,    0		*/
    0xC0000000,		/* negative,    1		*/
    0xC0800000,		/* negative,    2		*/
    0xC1000000,		/* negative,    3		*/
    0xC1800000,		/* negative,    4		*/
    0xCB800000,		/* negative,   24		*/
    0xCE000000,		/* negative,   29		*/
    0xCE800000,		/* negative,   30		*/
    0xCF000000,		/* negative,   31		*/
    0xCF800000,		/* negative,   32		*/
    0xDE000000,		/* negative,   61		*/
    0xDE800000,		/* negative,   62		*/
    0xDF000000,		/* negative,   63		*/
    0xDF800000,		/* negative,   64		*/
    0xFE800000,		/* negative,  126		*/
    0xFF000000,		/* negative,  127		*/
    0xFF800000		/* negative, infinity or NaN	*/
};

static const uint32_t floatP1[ floatNumP1 ] = {
    0x00000000,
    0x00000001,
    0x007FFFFF,
    0x007FFFFE
};

static const uint32_t floatP2[ floatNumP2 ] = {
    0x00000000,
    0x00000001,
    0x00000002,
    0x00000004,
    0x00000008,
    0x00000010,
    0x00000020,
    0x00000040,
    0x00000080,
    0x00000100,
    0x00000200,
    0x00000400,
    0x00000800,
    0x00001000,
    0x00002000,
    0x00004000,
    0x00008000,
    0x00010000,
    0x00020000,
    0x00040000,
    0x00080000,
    0x00100000,
    0x00200000,
    0x00400000,
    0x00600000,
    0x00700000,
    0x00780000,
    0x007C0000,
    0x007E0000,
    0x007F0000,
    0x007F8000,
    0x007FC000,
    0x007FE000,
    0x007FF000,
    0x007FF800,
    0x007FFC00,
    0x007FFE00,
    0x007FFF00,
    0x007FFF80,
    0x007FFFC0,
    0x007FFFE0,
    0x007FFFF0,
    0x007FFFF8,
    0x007FFFFC,
    0x007FFFFE,
    0x007FFFFF,
    0x007FFFFD,
    0x007FFFFB,
    0x007FFFF7,
    0x007FFFEF,
    0x007FFFDF,
    0x007FFFBF,
    0x007FFF7F,
    0x007FFEFF,
    0x007FFDFF,
    0x007FFBFF,
    0x007FF7FF,
    0x007FEFFF,
    0x007FDFFF,
    0x007FBFFF,
    0x007F7FFF,
    0x007EFFFF,
    0x007DFFFF,
    0x007BFFFF,
    0x0077FFFF,
    0x006FFFFF,
    0x005FFFFF,
    0x003FFFFF,
    0x001FFFFF,
    0x000FFFFF,
    0x0007FFFF,
    0x0003FFFF,
    0x0001FFFF,
    0x0000FFFF,
    0x00007FFF,
    0x00003FFF,
    0x00001FFF,
    0x00000FFF,
    0x000007FF,
    0x000003FF,
    0x000001FF,
    0x000000FF,
    0x0000007F,
    0x0000003F,
    0x0000001F,
    0x0000000F,
    0x00000007,
    0x00000003
};

static float floatRandomQOutP3( void )
{

    return
          floatQOut[ randomUint8() % floatNumQOut ]
        | (   (   floatP2[ randomUint8() % floatNumP2 ]
                + floatP2[ randomUint8() % floatNumP2 ] )
            & 0x007FFFFF );

}

static float floatRandomQOutPInf( void )
{

    return
          floatQOut[ randomUint8() % floatNumQOut ]
        | ( randomUint32() & 0x007FFFFF );

}

enum {
    floatNumQInfWeightMasks = 7
};

static const uint32_t floatQInfWeightMasks[ floatNumQInfWeightMasks ] = {
    0x7F800000,
    0x7F800000,
    0x3F800000,
    0x1F800000,
    0x0F800000,
    0x07800000,
    0x03800000
};

static const uint32_t floatQInfWeightOffsets[ floatNumQInfWeightMasks ] = {
    0x00000000,
    0x00000000,
    0x20000000,
    0x30000000,
    0x38000000,
    0x3C000000,
    0x3E000000
};

static float floatRandomQInfP3( void )
{
    int8_t weightMaskNum;

    weightMaskNum = randomUint8() % floatNumQInfWeightMasks;
    return
          ( ( (uint32_t) ( randomUint8() & 1 ) )<<31 )
        | (   (   ( ( (uint32_t) ( randomUint16() & 0x1FF ) )<<23 )
                & floatQInfWeightMasks[ weightMaskNum ] )
            + floatQInfWeightOffsets[ weightMaskNum ]
          )
        | (   (   floatP2[ randomUint8() % floatNumP2 ]
                + floatP2[ randomUint8() % floatNumP2 ] )
            & 0x007FFFFF );

}

static float floatRandomQInfPInf( void )
{
    int8_t weightMaskNum;

    weightMaskNum = randomUint8() % floatNumQInfWeightMasks;
    return
          ( ( (uint32_t) ( randomUint8() & 1 ) )<<31 )
        | (   (   ( ( (uint32_t) ( randomUint16() & 0x1FF ) )<<23 )
                & floatQInfWeightMasks[ weightMaskNum ] )
            + floatQInfWeightOffsets[ weightMaskNum ]
          )
        | ( randomUint32() & 0x007FFFFF );

}

static float floatRandom( void )
{

    switch ( randomUint8() & 7 ) {
     case 0:
     case 1:
     case 2:
        return floatRandomQOutP3();
     case 3:
        return floatRandomQOutPInf();
     case 4:
     case 5:
     case 6:
        return floatRandomQInfP3();
     case 7:
        return floatRandomQInfPInf();
    }

}

#define TEST_VECTOR_SIZE 1024*16

typedef union FLOAT_32 FLOAT_32;

union FLOAT_32
{
  float    f32;
  uint32_t u32;
};

int
main( void )
{
  uint32_t i;

  printf("const uint32_t float_test_vector_count = %d;\n",TEST_VECTOR_SIZE);

  printf("const uint32_t float_test_vector_big[] =\n");
  printf("{\n");
  for (i=0;i<TEST_VECTOR_SIZE;i++)
  {
    FLOAT_32 tf;

    do {
    tf.f32 = floatRandom() + (floatRandom() / ( ( floatRandom() / (float)i ) ));
    }
    while ( tf.f32 < 6400000.0000000000 );
   
    printf("  0x%08x, /* %.10f */\n",tf.u32,tf.f32);
  }
  printf("};\n");

  printf("const uint32_t float_test_vector_med[] =\n");
  printf("{\n");
  for (i=0;i<TEST_VECTOR_SIZE;i++)
  {
    FLOAT_32 tf;

    do {
    tf.f32 = floatRandom() / ( ( floatRandom() / (float)i ) );
    } 
    while (tf.f32 > 32768.0f);

   
    printf("  0x%08x, /* %.10f */\n",tf.u32,tf.f32);
  }
  printf("};\n");

  printf("const uint32_t float_test_vector_small[] =\n");
  printf("{\n");
  for (i=0;i<TEST_VECTOR_SIZE;i++)
  {
    FLOAT_32 tf;

    do {
    tf.f32 = floatRandom() / ( ( floatRandom() * (float)i ) );
    } 
    while (tf.f32 > 1.0f);
   
    printf("  0x%08x, /* %.10f */\n",tf.u32,tf.f32);
  }
  printf("};\n");

  return (0);
}

