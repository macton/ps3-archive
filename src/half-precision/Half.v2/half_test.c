// MJA - When compiling with GCC use:
// -O3 -fstrict-aliasing -Wall -std=c99

// MJA - When compiling with MSVC use:
// "Warning Level 4", "Compile as C++"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "half.h"
#include "float_test_vector.h"
#include "test_float.h"

typedef union FLOAT_32 FLOAT_32;
union FLOAT_32
{
    float    f32;
    uint32_t u32;
};

void 
print_float( float arg )
{
  FLOAT_32 f;
  int      i;

  f.f32 = arg;
  printf("0x%08x ",(unsigned int)f.u32);
  printf(" b");
  for ( i = 31; i >= 0; i-- ) 
  {
    int bit = (int)(( f.u32 & (1 << i) ) >> i);
    printf("%d",bit);
  }         
  printf(" %f",f.f32);
}

void 
print_half( uint16_t h )
{
  int i;

  printf("0x%04x ",(unsigned int)h);
  printf(" b");
  for ( i = 15; i >= 0; i-- ) 
  {
    int bit = (int)(( h & (1 << i) ) >> i);
    printf("%d",bit);
  }         
}

#define N_TESTS  80
    
const uint32_t float_bits[80] = 
    { 0x80000000, 0x0,        0x3F800000, 0x3F802000, 
      0x3F802000, 0x3F800000, 0x3F802000, 0x3F7FE000, 
      0x3F800000, 0x3F800000, 0x3F7FE000, 0x33800000, 
      0x34000000, 0x34000000, 0x33800000, 0x34000000, 
      0x0,        0x33800000, 0x33800000, 0x0, 
      0x38800000, 0x38802000, 0x38802000, 0x38800000, 
      0x38802000, 0x387FC000, 0x38800000, 0x38800000, 
      0x387FC000, 0x40000000, 0x40400000, 0x41200000, 
      0x3DCCC000, 0x3E4CC000, 0x3E99A000, 0x477FE000, 
      0x7F800000, 0x7F800000, 0x477FE000, 0x7F800000, 
      0x7F800000, 0x7F800000, 0xBF800000, 0xBF802000, 
      0xBF802000, 0xBF800000, 0xBF802000, 0xBF7FE000, 
      0xBF800000, 0xBF800000, 0xBF7FE000, 0xB3800000, 
      0xB4000000, 0xB4000000, 0xB3800000, 0xB4000000, 
      0x0,        0xB3800000, 0xB3800000, 0x0, 
      0xB8800000, 0xB8802000, 0xB8802000, 0xB8800000, 
      0xB8802000, 0xB87FC000, 0xB8800000, 0xB8800000, 
      0xB87FC000, 0xC0000000, 0xC0400000, 0xC1200000, 
      0xBDCCC000, 0xBE4CC000, 0xBE99A000, 0xC77FE000, 
      0xFF800000, 0xFF800000, 0xC77FE000, 0xFF800000 };

const uint16_t half_bits[80] = 
  { 0x8000, 0x0,    0x3C00, 0x3C01, 0x3C01, 0x3C00, 0x3C01, 0x3BFF, 
    0x3C00, 0x3C00, 0x3BFF, 0x1,    0x2,    0x2,    0x1,    0x2, 
    0x0,    0x1,    0x1,    0x0,    0x400,  0x401,  0x401,  0x400, 
    0x401,  0x3FF,  0x400,  0x400,  0x3FF,  0x4000, 0x4200, 0x4900, 
    0x2E66, 0x3266, 0x34CD, 0x7BFF, 0x7C00, 0x7C00, 0x7BFF, 0x7C00, 
    0x7C00, 0x7C00, 0xBC00, 0xBC01, 0xBC01, 0xBC00, 0xBC01, 0xBBFF, 
    0xBC00, 0xBC00, 0xBBFF, 0x8001, 0x8002, 0x8002, 0x8001, 0x8002, 
    0x0,    0x8001, 0x8001, 0x0,    0x8400, 0x8401, 0x8401, 0x8400, 
    0x8401, 0x83FF, 0x8400, 0x8400, 0x83FF, 0xC000, 0xC200, 0xC900, 
    0xAE66, 0xB266, 0xB4CD, 0xFBFF, 0xFC00, 0xFC00, 0xFBFF, 0xFC00 };

void
test_conversion( void ) 
{
    int      test_count;
    int      test_fail_count;
    uint32_t i;
 
    printf("---------------------------------------------\n");
    printf("Test float to half conversion (Special Cases)\n");
    printf("---------------------------------------------\n");

    test_count      = 0;
    test_fail_count = 0;

    for ( i=0; i < N_TESTS; i++ )
    {
      uint32_t test_in;
      uint16_t test_f16;

      test_in  = float_bits[i];
      test_f16 = half_from_float( test_in );

      if (test_f16 != half_bits[i])
      {
         FLOAT_32 in;

         in.u32  = test_in;

         printf("float->half: FAILED ");
         print_float( in.f32 );
         printf(" -> ");
         print_half( test_f16 );
         printf(" !- ");
         print_half( half_bits[i] );
         printf("\n");

         test_fail_count++;
      }

      test_count++;
    }

    printf("\n\n");
    if ( test_fail_count )
    {
      printf("FAILED %d of %d\n", test_fail_count, test_count);
    }
    else
    {
      printf("PASSED ALL (%d)\n", test_count);
    }
    printf("\n");

    printf("---------------------------------------------\n");
    printf("Test float->half->float conversion (Med)     \n");
    printf("---------------------------------------------\n");

    test_count      = 0;
    test_fail_count = 0;

    for ( i=0; i < float_test_vector_count; i++ )
    {
      uint32_t test_in;
      uint16_t test_f16;
      uint32_t test_f32;
      FLOAT_32 fa;
      FLOAT_32 fb;

      test_in  = float_test_vector_med[i];
      test_f16 = half_from_float( test_in );
      test_f32 = half_to_float( test_f16 );

      fa.u32 = test_in;
      fb.u32 = test_f32;
       
      if (!test_float_compare( fa.f32, fb.f32, 4096 )) /* 4096 Ulps max loss from half to float conversion */
      {
         FLOAT_32 in;
         FLOAT_32 out;

         in.u32  = test_in;
         out.u32 = test_f32;

         printf("float->half->float: FAILED ");
         print_float( in.f32 );
         printf(" -> ");
         print_half( test_f16 );
         printf(" -> ");
         print_float( out.f32 );
         printf("\n");

         test_fail_count++;
      }

      test_count++;
    }

    printf("\n\n");
    if ( test_fail_count )
    {
      printf("FAILED %d of %d\n", test_fail_count, test_count);
    }
    else
    {
      printf("PASSED ALL (%d)\n", test_count);
    }
    printf("\n");

    printf("---------------------------------------------\n");
    printf("Test float->half->float conversion (Small)   \n");
    printf("---------------------------------------------\n");

    test_count      = 0;
    test_fail_count = 0;

    for ( i=0; i < float_test_vector_count; i++ )
    {
      uint32_t test_in;
      uint16_t test_f16;
      uint32_t test_f32;
      FLOAT_32 fa;
      FLOAT_32 fb;

      test_in  = float_test_vector_small[i];
      test_f16 = half_from_float( test_in );
      test_f32 = half_to_float( test_f16 );

      fa.u32 = test_in;
      fb.u32 = test_f32;

      uint16_t h_expected = half_from_float( test_f32 );
      uint16_t h_received = test_f16;

      uint16_t h_dulps = h_expected - h_received;

      if ( h_received >  h_expected )
      {
        h_dulps = h_received - h_expected;
      }

      if ( h_dulps > 0 )
      {
         FLOAT_32 in;
         FLOAT_32 out;

         in.u32  = test_in;
         out.u32 = test_f32;

         printf("float->half->float: FAILED ");
         print_float( in.f32 );
         printf(" -> ");
         print_half( test_f16 );
         printf(" -> ");
         print_float( out.f32 );
         printf("\n    ");
         print_half(h_received);
         printf(" != ");
         print_half(h_expected);
         printf("\n");

         test_fail_count++;
      }

      test_count++;
    }

    printf("\n\n");
    if ( test_fail_count )
    {
      printf("FAILED %d of %d\n", test_fail_count, test_count);
    }
    else
    {
      printf("PASSED ALL (%d)\n", test_count);
    }
    printf("\n");
}

void
test_add( void ) 
{
    int      test_count;
    int      test_fail_count;
    uint32_t i;
 
    printf("---------------------------------------------\n");
    printf("Test half add (Special)\n");
    printf("---------------------------------------------\n");

    test_count      = 0;
    test_fail_count = 0;

    for ( i=0; i < N_TESTS; i++ )
    {
      uint16_t test_h_a;
      uint16_t test_h_b;
      uint16_t test_h_c;
      uint32_t test_f_a;
      uint32_t test_f_b;

      FLOAT_32 fa;
      FLOAT_32 fb;
      FLOAT_32 fc;
      FLOAT_32 fc_h;

      test_f_a = float_bits[i];
      test_f_b = float_bits[ (N_TESTS-1) - i ];

      test_h_a = half_from_float( test_f_a );
      test_h_b = half_from_float( test_f_b );
      test_h_c = half_add( test_h_a, test_h_b );

      fa.u32   = half_to_float( test_h_a );
      fb.u32   = half_to_float( test_h_b );
      fc.f32   = fa.f32 + fb.f32;
      fc_h.u32 = half_to_float( test_h_c );

      uint16_t h_expected = half_from_float( fc.u32 );
      uint16_t h_received = test_h_c;

      fc.u32   = fc.u32;

      uint16_t h_dulps = h_expected - h_received;

      if ( h_received >  h_expected )
      {
        h_dulps = h_received - h_expected;
      }

      if ( h_dulps > 0 )
      {
         printf("half_add: FAILED (dulps=%d)\n    ",h_dulps);
         print_half( test_h_a );
         printf(" + ");
         print_half( test_h_b );
         printf(" :\n    ");
         print_float( fc_h.f32 );
         printf(" !=\n    ");
         print_float( fc.f32 );
         printf("\n    ");
         print_float( fa.f32 );
         printf(" + ");
         print_float( fb.f32 );
         printf("\n    ");
         print_half(h_received);
         printf(" != ");
         print_half(h_expected);
         printf("\n");

         test_fail_count++;
      }

      test_count++;
    }

    printf("\n\n");
    if ( test_fail_count )
    {
      printf("FAILED %d of %d\n", test_fail_count, test_count);
    }
    else
    {
      printf("PASSED ALL (%d)\n", test_count);
    }
    printf("\n");

    printf("---------------------------------------------\n");
    printf("Test half add (Small) (+a) + (+b)\n");
    printf("---------------------------------------------\n");

    test_count      = 0;
    test_fail_count = 0;

    for ( i=0; i < float_test_vector_count; i++ )
    {
      uint16_t test_h_a;
      uint16_t test_h_b;
      uint16_t test_h_c;
      uint32_t test_f_a;
      uint32_t test_f_b;

      FLOAT_32 fa;
      FLOAT_32 fb;
      FLOAT_32 fc;
      FLOAT_32 fc_h;

      test_f_a = float_test_vector_small[i];
      test_f_b = float_test_vector_small[ (float_test_vector_count-1) - i ];

      test_h_a = half_from_float( test_f_a );
      test_h_b = half_from_float( test_f_b );
      test_h_c = half_add( test_h_a, test_h_b );

      fa.u32   = half_to_float( test_h_a );
      fb.u32   = half_to_float( test_h_b );
      fc.f32   = fa.f32 + fb.f32;
      fc_h.u32 = half_to_float( test_h_c );

      uint16_t h_expected = half_from_float( fc.u32 );
      uint16_t h_received = test_h_c;

      fc.u32   = fc.u32;

      uint16_t h_dulps = h_expected - h_received;

      if ( h_received >  h_expected )
      {
        h_dulps = h_received - h_expected;
      }

      if ( h_dulps > 0 )
      {
         printf("half_add: FAILED (dulps=%d)\n    ",h_dulps);
         print_half( test_h_a );
         printf(" + ");
         print_half( test_h_b );
         printf(" :\n    ");
         print_float( fc_h.f32 );
         printf(" !=\n    ");
         print_float( fc.f32 );
         printf("\n    ");
         print_float( fa.f32 );
         printf(" + ");
         print_float( fb.f32 );
         printf("\n    ");
         print_half(h_received);
         printf(" != ");
         print_half(h_expected);
         printf("\n");

         test_fail_count++;
      }

      test_count++;
    }

    printf("\n\n");
    if ( test_fail_count )
    {
      printf("FAILED %d of %d\n", test_fail_count, test_count);
    }
    else
    {
      printf("PASSED ALL (%d)\n", test_count);
    }
    printf("\n");

    printf("---------------------------------------------\n");
    printf("Test half add (Small) (-a) + (+b)\n");
    printf("---------------------------------------------\n");

    test_count      = 0;
    test_fail_count = 0;

    for ( i=0; i < float_test_vector_count; i++ )
    {
      uint16_t test_h_a;
      uint16_t test_h_b;
      uint16_t test_h_c;
      uint32_t test_f_a;
      uint32_t test_f_b;

      FLOAT_32 fa;
      FLOAT_32 fb;
      FLOAT_32 fc;
      FLOAT_32 fc_h;

      test_f_a = float_test_vector_small[i] ^ 0x80000000;
      test_f_b = float_test_vector_small[ (float_test_vector_count-1) - i ];

      test_h_a = half_from_float( test_f_a );
      test_h_b = half_from_float( test_f_b );
      test_h_c = half_add( test_h_a, test_h_b );

      fa.u32   = half_to_float( test_h_a );
      fb.u32   = half_to_float( test_h_b );
      fc.f32   = fa.f32 + fb.f32;
      fc_h.u32 = half_to_float( test_h_c );

      uint16_t h_expected = half_from_float( fc.u32 );
      uint16_t h_received = test_h_c;

      fc.u32   = fc.u32;

      uint16_t h_dulps = h_expected - h_received;

      if ( h_received >  h_expected )
      {
        h_dulps = h_received - h_expected;
      }

      if ( h_dulps > 0 )
      {
         printf("half_add: FAILED (dulps=%d)\n    ",h_dulps);
         print_half( test_h_a );
         printf(" + ");
         print_half( test_h_b );
         printf(" :\n    ");
         print_float( fc_h.f32 );
         printf(" !=\n    ");
         print_float( fc.f32 );
         printf("\n    ");
         print_float( fa.f32 );
         printf(" + ");
         print_float( fb.f32 );
         printf("\n    ");
         print_half(h_received);
         printf(" != ");
         print_half(h_expected);
         printf("\n");

         test_fail_count++;
      }

      test_count++;
    }

    printf("\n\n");
    if ( test_fail_count )
    {
      printf("FAILED %d of %d\n", test_fail_count, test_count);
    }
    else
    {
      printf("PASSED ALL (%d)\n", test_count);
    }
    printf("\n");

    printf("---------------------------------------------\n");
    printf("Test half add (Small) (+a) + (-b)\n");
    printf("---------------------------------------------\n");

    test_count      = 0;
    test_fail_count = 0;

    for ( i=0; i < float_test_vector_count; i++ )
    {
      uint16_t test_h_a;
      uint16_t test_h_b;
      uint16_t test_h_c;
      uint32_t test_f_a;
      uint32_t test_f_b;

      FLOAT_32 fa;
      FLOAT_32 fb;
      FLOAT_32 fc;
      FLOAT_32 fc_h;

      test_f_a = float_test_vector_small[i];
      test_f_b = float_test_vector_small[ (float_test_vector_count-1) - i ] ^ 0x80000000;

      test_h_a = half_from_float( test_f_a );
      test_h_b = half_from_float( test_f_b );
      test_h_c = half_add( test_h_a, test_h_b );

      fa.u32   = half_to_float( test_h_a );
      fb.u32   = half_to_float( test_h_b );
      fc.f32   = fa.f32 + fb.f32;
      fc_h.u32 = half_to_float( test_h_c );

      uint16_t h_expected = half_from_float( fc.u32 );
      uint16_t h_received = test_h_c;

      fc.u32   = fc.u32;

      uint16_t h_dulps = h_expected - h_received;

      if ( h_received >  h_expected )
      {
        h_dulps = h_received - h_expected;
      }

      if ( h_dulps > 0 )
      {
         printf("half_add: FAILED (dulps=%d)\n    ",h_dulps);
         print_half( test_h_a );
         printf(" + ");
         print_half( test_h_b );
         printf(" :\n    ");
         print_float( fc_h.f32 );
         printf(" !=\n    ");
         print_float( fc.f32 );
         printf("\n    ");
         print_float( fa.f32 );
         printf(" + ");
         print_float( fb.f32 );
         printf("\n    ");
         print_half(h_received);
         printf(" != ");
         print_half(h_expected);
         printf("\n");

         test_fail_count++;
      }

      test_count++;
    }

    printf("\n\n");
    if ( test_fail_count )
    {
      printf("FAILED %d of %d\n", test_fail_count, test_count);
    }
    else
    {
      printf("PASSED ALL (%d)\n", test_count);
    }
    printf("\n");


    printf("---------------------------------------------\n");
    printf("Test half add (Med) (+a) + (+b)\n");
    printf("---------------------------------------------\n");

    test_count      = 0;
    test_fail_count = 0;

    for ( i=0; i < float_test_vector_count; i++ )
    {
      uint16_t test_h_a;
      uint16_t test_h_b;
      uint16_t test_h_c;
      uint32_t test_f_a;
      uint32_t test_f_b;

      FLOAT_32 fa;
      FLOAT_32 fb;
      FLOAT_32 fc;
      FLOAT_32 fc_h;

      test_f_a = float_test_vector_med[i];
      test_f_b = float_test_vector_med[ (float_test_vector_count-1) - i ];

      test_h_a = half_from_float( test_f_a );
      test_h_b = half_from_float( test_f_b );
      test_h_c = half_add( test_h_a, test_h_b );

      fa.u32   = half_to_float( test_h_a );
      fb.u32   = half_to_float( test_h_b );
      fc.f32   = fa.f32 + fb.f32;
      fc_h.u32 = half_to_float( test_h_c );

      uint16_t h_expected = half_from_float( fc.u32 );
      uint16_t h_received = test_h_c;

      fc.u32   = fc.u32;

      uint16_t h_dulps = h_expected - h_received;

      if ( h_received >  h_expected )
      {
        h_dulps = h_received - h_expected;
      }

      if ( h_dulps > 0 )
      {
         printf("half_add: FAILED ");
         print_half( test_h_a );
         printf(" + ");
         print_half( test_h_b );
         printf(" :\n    ");
         print_float( fc_h.f32 );
         printf(" != ");
         print_float( fc.f32 );
         printf("\n    ");
         print_float( fa.f32 );
         printf(" + ");
         print_float( fb.f32 );
         printf("\n    ");
         print_half(h_received);
         printf(" != ");
         print_half(h_expected);
         printf("\n");

         test_fail_count++;
      }

      test_count++;
    }

    printf("\n\n");
    if ( test_fail_count )
    {
      printf("FAILED %d of %d\n", test_fail_count, test_count);
    }
    else
    {
      printf("PASSED ALL (%d)\n", test_count);
    }
    printf("\n");

    printf("---------------------------------------------\n");
    printf("Test half add (Med) (-a) + (+b)\n");
    printf("---------------------------------------------\n");

    test_count      = 0;
    test_fail_count = 0;

    for ( i=0; i < float_test_vector_count; i++ )
    {
      uint16_t test_h_a;
      uint16_t test_h_b;
      uint16_t test_h_c;
      uint32_t test_f_a;
      uint32_t test_f_b;

      FLOAT_32 fa;
      FLOAT_32 fb;
      FLOAT_32 fc;
      FLOAT_32 fc_h;

      test_f_a = float_test_vector_med[i] ^ 0x80000000;
      test_f_b = float_test_vector_med[ (float_test_vector_count-1) - i ];

      test_h_a = half_from_float( test_f_a );
      test_h_b = half_from_float( test_f_b );
      test_h_c = half_add( test_h_a, test_h_b );

      fa.u32   = half_to_float( test_h_a );
      fb.u32   = half_to_float( test_h_b );
      fc.f32   = fa.f32 + fb.f32;
      fc_h.u32 = half_to_float( test_h_c );

      uint16_t h_expected = half_from_float( fc.u32 );
      uint16_t h_received = test_h_c;

      fc.u32   = fc.u32;

      uint16_t h_dulps = h_expected - h_received;

      if ( h_received >  h_expected )
      {
        h_dulps = h_received - h_expected;
      }

      if ( h_dulps > 0 )
      {
         printf("half_add: FAILED ");
         print_half( test_h_a );
         printf(" + ");
         print_half( test_h_b );
         printf(" :\n    ");
         print_float( fc_h.f32 );
         printf(" != ");
         print_float( fc.f32 );
         printf("\n    ");
         print_float( fa.f32 );
         printf(" + ");
         print_float( fb.f32 );
         printf("\n    ");
         print_half(h_received);
         printf(" != ");
         print_half(h_expected);
         printf("\n");

         test_fail_count++;
      }

      test_count++;
    }

    printf("\n\n");
    if ( test_fail_count )
    {
      printf("FAILED %d of %d\n", test_fail_count, test_count);
    }
    else
    {
      printf("PASSED ALL (%d)\n", test_count);
    }
    printf("\n");

    printf("---------------------------------------------\n");
    printf("Test half add (Med) (+a) + (-b)\n");
    printf("---------------------------------------------\n");

    test_count      = 0;
    test_fail_count = 0;

    for ( i=0; i < float_test_vector_count; i++ )
    {
      uint16_t test_h_a;
      uint16_t test_h_b;
      uint16_t test_h_c;
      uint32_t test_f_a;
      uint32_t test_f_b;

      FLOAT_32 fa;
      FLOAT_32 fb;
      FLOAT_32 fc;
      FLOAT_32 fc_h;

      test_f_a = float_test_vector_med[i];
      test_f_b = float_test_vector_med[ (float_test_vector_count-1) - i ] ^ 0x80000000;

      test_h_a = half_from_float( test_f_a );
      test_h_b = half_from_float( test_f_b );
      test_h_c = half_add( test_h_a, test_h_b );

      fa.u32   = half_to_float( test_h_a );
      fb.u32   = half_to_float( test_h_b );
      fc.f32   = fa.f32 + fb.f32;
      fc_h.u32 = half_to_float( test_h_c );

      uint16_t h_expected = half_from_float( fc.u32 );
      uint16_t h_received = test_h_c;

      fc.u32   = fc.u32;

      uint16_t h_dulps = h_expected - h_received;

      if ( h_received >  h_expected )
      {
        h_dulps = h_received - h_expected;
      }

      if ( h_dulps > 0 )
      {
         printf("half_add: FAILED ");
         print_half( test_h_a );
         printf(" + ");
         print_half( test_h_b );
         printf(" :\n    ");
         print_float( fc_h.f32 );
         printf(" != ");
         print_float( fc.f32 );
         printf("\n    ");
         print_float( fa.f32 );
         printf(" + ");
         print_float( fb.f32 );
         printf("\n    ");
         print_half(h_received);
         printf(" != ");
         print_half(h_expected);
         printf("\n");

         test_fail_count++;
      }

      test_count++;
    }

    printf("\n\n");
    if ( test_fail_count )
    {
      printf("FAILED %d of %d\n", test_fail_count, test_count);
    }
    else
    {
      printf("PASSED ALL (%d)\n", test_count);
    }
    printf("\n");
}

void
test_sub( void ) 
{
    int      test_count;
    int      test_fail_count;
    uint32_t i;
 
    printf("---------------------------------------------\n");
    printf("Test half sub (Special Cases)\n");
    printf("---------------------------------------------\n");

    // MJA - Not doing these yet. We know sub will fail.
    printf("\n");
    printf("NOT TESTED (TODO)\n");
    printf("\n");

    printf("---------------------------------------------\n");
    printf("Test half sub (Med), (+a) - (+b)\n");
    printf("---------------------------------------------\n");

    test_count      = 0;
    test_fail_count = 0;

    for ( i=0; i < float_test_vector_count; i++ )
    {
      uint16_t test_h_a;
      uint16_t test_h_b;
      uint16_t test_h_c;
      uint32_t test_f_a;
      uint32_t test_f_b;

      FLOAT_32 fa;
      FLOAT_32 fb;
      FLOAT_32 fc;
      FLOAT_32 fc_h;

      test_f_a = float_test_vector_med[i];
      test_f_b = float_test_vector_med[ (float_test_vector_count-1) - i ];

      test_h_a = half_from_float( test_f_a );
      test_h_b = half_from_float( test_f_b );
      test_h_c = half_sub( test_h_a, test_h_b );

      fa.u32   = half_to_float( test_h_a );
      fb.u32   = half_to_float( test_h_b );
      fc.f32   = fa.f32 - fb.f32;
      fc_h.u32 = half_to_float( test_h_c );

      uint16_t h_expected = half_from_float( fc.u32 );
      uint16_t h_received = test_h_c;

      fc.u32   = fc.u32;

      uint16_t h_dulps = h_expected - h_received;

      if ( h_received >  h_expected )
      {
        h_dulps = h_received - h_expected;
      }

      if ( h_dulps > 0 )
      {
         printf("half_sub: FAILED ");
         print_half( test_h_a );
         printf(" + ");
         print_half( test_h_b );
         printf(" :\n    ");
         print_float( fc_h.f32 );
         printf(" != ");
         print_float( fc.f32 );
         printf("\n    ");
         print_float( fa.f32 );
         printf(" - ");
         print_float( fb.f32 );
         printf("\n    ");
         print_half(h_received);
         printf(" != ");
         print_half(h_expected);
         printf("\n");

         test_fail_count++;
      }

      test_count++;
    }

    printf("\n\n");
    if ( test_fail_count )
    {
      printf("FAILED %d of %d\n", test_fail_count, test_count);
    }
    else
    {
      printf("PASSED ALL (%d)\n", test_count);
    }
    printf("\n");

    printf("---------------------------------------------\n");
    printf("Test half sub (Med), (-a) - (+b)\n");
    printf("---------------------------------------------\n");

    test_count      = 0;
    test_fail_count = 0;

    for ( i=0; i < float_test_vector_count; i++ )
    {
      uint16_t test_h_a;
      uint16_t test_h_b;
      uint16_t test_h_c;
      uint32_t test_f_a;
      uint32_t test_f_b;

      FLOAT_32 fa;
      FLOAT_32 fb;
      FLOAT_32 fc;
      FLOAT_32 fc_h;

      test_f_a = float_test_vector_med[i] ^ 0x80000000;
      test_f_b = float_test_vector_med[ (float_test_vector_count-1) - i ];

      test_h_a = half_from_float( test_f_a );
      test_h_b = half_from_float( test_f_b );
      test_h_c = half_sub( test_h_a, test_h_b );

      fa.u32   = half_to_float( test_h_a );
      fb.u32   = half_to_float( test_h_b );
      fc.f32   = fa.f32 - fb.f32;
      fc_h.u32 = half_to_float( test_h_c );

      uint16_t h_expected = half_from_float( fc.u32 );
      uint16_t h_received = test_h_c;

      fc.u32   = fc.u32;

      uint16_t h_dulps = h_expected - h_received;

      if ( h_received >  h_expected )
      {
        h_dulps = h_received - h_expected;
      }

      if ( h_dulps > 0 )
      {
         printf("half_sub: FAILED ");
         print_half( test_h_a );
         printf(" + ");
         print_half( test_h_b );
         printf(" :\n    ");
         print_float( fc_h.f32 );
         printf(" != ");
         print_float( fc.f32 );
         printf("\n    ");
         print_float( fa.f32 );
         printf(" - ");
         print_float( fb.f32 );
         printf("\n    ");
         print_half(h_received);
         printf(" != ");
         print_half(h_expected);
         printf("\n");

         test_fail_count++;
      }

      test_count++;
    }

    printf("\n\n");
    if ( test_fail_count )
    {
      printf("FAILED %d of %d\n", test_fail_count, test_count);
    }
    else
    {
      printf("PASSED ALL (%d)\n", test_count);
    }
    printf("\n");
}


int 
main( void )
{   
    test_conversion();
    test_add();
    test_sub();

    return (0);
}

