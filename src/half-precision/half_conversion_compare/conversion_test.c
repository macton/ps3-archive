#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

#include "float_test_vector.h"
#include "test_float.h"

uint16_t half_from_float( uint32_t f );
uint32_t half_to_float( uint16_t h );
uint32_t half_from_float_x2( uint64_t f2 );
uint64_t half_to_float_x2( uint32_t h2 );

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
    
const uint32_t float_test_vector_special[80] = 
    { 0x80000000, 0x00000000, 0x3F800000, 0x3F802000, 
      0x3F802000, 0x3F800000, 0x3F802000, 0x3F7FE000, 
      0x3F800000, 0x3F800000, 0x3F7FE000, 0x33800000, 
      0x34000000, 0x34000000, 0x33800000, 0x34000000, 
      0x00000000, 0x33800000, 0x33800000, 0x00000000,
      0x38800000, 0x38802000, 0x38802000, 0x38800000, 
      0x38802000, 0x387FC000, 0x38800000, 0x38800000, 
      0x387FC000, 0x40000000, 0x40400000, 0x41200000, 
      0x3DCCC000, 0x3E4CC000, 0x3E99A000, 0x477FE000, 
      0x7F800000, 0x7F800000, 0x477FE000, 0x7F800000, 
      0x7F800000, 0x7F800000, 0xBF800000, 0xBF802000, 
      0xBF802000, 0xBF800000, 0xBF802000, 0xBF7FE000, 
      0xBF800000, 0xBF800000, 0xBF7FE000, 0xB3800000, 
      0xB4000000, 0xB4000000, 0xB3800000, 0xB4000000, 
      0x00000000, 0xB3800000, 0xB3800000, 0x00000000,
      0xB8800000, 0xB8802000, 0xB8802000, 0xB8800000, 
      0xB8802000, 0xB87FC000, 0xB8800000, 0xB8800000, 
      0xB87FC000, 0xC0000000, 0xC0400000, 0xC1200000, 
      0xBDCCC000, 0xBE4CC000, 0xBE99A000, 0xC77FE000, 
      0xFF800000, 0xFF800000, 0xC77FE000, 0xFF800fff };

const uint16_t half_test_vector_special[80] = 
  { 0x8000, 0x0000, 0x3C00, 0x3C01, 0x3C01, 0x3C00, 0x3C01, 0x3BFF, 
    0x3C00, 0x3C00, 0x3BFF, 0x0001, 0x0002, 0x0002, 0x0001, 0x0002, 
    0x0000, 0x0001, 0x0001, 0x0000, 0x0400, 0x0401, 0x0401, 0x0400, 
    0x0401, 0x03FF, 0x0400, 0x0400, 0x03FF, 0x4000, 0x4200, 0x4900, 
    0x2E66, 0x3266, 0x34CD, 0x7BFF, 0x7C00, 0x7C00, 0x7BFF, 0x7C00, 
    0x7C00, 0x7C00, 0xBC00, 0xBC01, 0xBC01, 0xBC00, 0xBC01, 0xBBFF, 
    0xBC00, 0xBC00, 0xBBFF, 0x8001, 0x8002, 0x8002, 0x8001, 0x8002, 
    0x0000, 0x8001, 0x8001, 0x0000, 0x8400, 0x8401, 0x8401, 0x8400, 
    0x8401, 0x83FF, 0x8400, 0x8400, 0x83FF, 0xC000, 0xC200, 0xC900, 
    0xAE66, 0xB266, 0xB4CD, 0xFBFF, 0xFC00, 0xFC00, 0xFBFF, 0xFC01 };

extern int mixed_test_count;
extern int mixed_test[];

void
test_conversion( void ) 
{
    int test_ndx;

    for ( test_ndx = 0; test_ndx < N_TESTS ; test_ndx++ )
    {
        int i = test_ndx;
        if ( i == 0 ) i = 1;

        uint32_t test_in;
        uint16_t test_f16;

        test_in  = float_test_vector_special[i];
        test_f16 = half_from_float( test_in );
  
        if (test_f16 != half_test_vector_special[i])
        {
           FLOAT_32 in;
  
           in.u32  = test_in;

           printf("float->half: FAILED ");
           print_float( in.f32 );
           printf(" -> ");
           print_half( test_f16 );
           printf(" !- ");
           print_half( half_test_vector_special[i] );
           printf("\n");
           return;
        }
    }

    for ( test_ndx = 0; test_ndx < float_test_vector_count; test_ndx++ )
    {
        int i = test_ndx;

        uint32_t test_in;

        test_in  = float_test_vector_med[i];
  
        uint16_t test_f16          = half_from_float( test_in );
        uint32_t test_f32          = half_to_float( test_f16 );
        uint16_t test_f16_reversed = half_from_float( test_f32 );
        uint16_t test_diff         = test_f16_reversed - test_f16;
  
        if ( test_diff > 0 )
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
           return;
        }
    }

    for ( test_ndx = 0; test_ndx < float_test_vector_count; test_ndx++ )
    {
        int i = test_ndx;

        uint32_t test_in;

        test_in  = float_test_vector_small[i];
  
        uint16_t test_f16          = half_from_float( test_in );
        uint32_t test_f32          = half_to_float( test_f16 );
        uint16_t test_f16_reversed = half_from_float( test_f32 );
        uint16_t test_diff         = test_f16_reversed - test_f16;
  
        if ( test_diff > 0 )
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
           return;
        }
    }

    for ( test_ndx = 0; test_ndx < mixed_test_count; test_ndx++ )
    {
      int test_type = mixed_test[test_ndx] & 0x03;

      if ( test_type == 0 )
      {
        int i = mixed_test[test_ndx] % N_TESTS;
        if ( i == 0 ) i = 1;

        uint32_t test_in;
        uint16_t test_f16;

        test_in  = float_test_vector_special[i];
        test_f16 = half_from_float( test_in );
  
        if (test_f16 != half_test_vector_special[i])
        {
           FLOAT_32 in;
  
           in.u32  = test_in;

           printf("float->half: FAILED ");
           print_float( in.f32 );
           printf(" -> ");
           print_half( test_f16 );
           printf(" !- ");
           print_half( half_test_vector_special[i] );
           printf("\n");
           return;
        }
      }
      else
      {
        int i = mixed_test[test_ndx] % float_test_vector_count;

        uint32_t test_in;

        if ( test_type == 1 )
        {
          test_in  = float_test_vector_med[i];
        }
        else 
        {
          test_in  = float_test_vector_small[i];
        }
  
        uint16_t test_f16          = half_from_float( test_in );
        uint32_t test_f32          = half_to_float( test_f16 );
        uint16_t test_f16_reversed = half_from_float( test_f32 );
        uint16_t test_diff         = test_f16_reversed - test_f16;
  
        if ( test_diff > 0 )
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
           return;
        }
      }
    }
}

void
test_conversion_x2( void ) 
{
    int test_ndx;

    for ( test_ndx = 0; test_ndx < N_TESTS ; test_ndx++ )
    {
        int i = test_ndx;
        if ( i == 0 ) i = 1;

        uint32_t test_in;
        uint16_t test_f16;
        uint64_t test_in_x2;
        uint32_t test_out_x2;

        test_in     = float_test_vector_special[i];
        test_in_x2  = test_in | ((uint64_t)test_in << 32);
        test_out_x2 = half_from_float_x2( test_in_x2 );
        test_f16    = test_out_x2 >> 16;
  
        if (test_f16 != half_test_vector_special[i])
        {
           FLOAT_32 in;
  
           in.u32  = test_in;

           printf("float->half: FAILED ");
           print_float( in.f32 );
           printf(" -> ");
           print_half( test_f16 );
           printf(" !- ");
           print_half( half_test_vector_special[i] );
           printf("\n");
           return;
        }
    }

    for ( test_ndx = 0; test_ndx < float_test_vector_count; test_ndx++ )
    {
        int i = test_ndx;

        uint32_t test_in;
        uint64_t test_in_x2;

        test_in    = float_test_vector_med[i];
        test_in_x2 = test_in | ( (uint64_t)test_in << 32 );
  
        uint32_t test_f16_x2          = half_from_float_x2( test_in_x2 );
        uint64_t test_f32_x2          = half_to_float_x2( test_f16_x2 );
        uint32_t test_f16_reversed_x2 = half_from_float_x2( test_f32_x2 );
        uint16_t test_f16_reversed    = test_f16_reversed_x2 >> 16;
        uint16_t test_f16             = test_f16_x2 >> 16;
        uint16_t test_diff            = test_f16_reversed - test_f16;
        uint32_t test_f32             = test_f32_x2 >> 32;
  
        if ( test_diff > 0 )
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
           return;
        }
    }

    for ( test_ndx = 0; test_ndx < float_test_vector_count; test_ndx++ )
    {
        int i = test_ndx;

        uint32_t test_in;
        uint64_t test_in_x2;

        test_in    = float_test_vector_small[i];
        test_in_x2 = test_in | ( (uint64_t)test_in << 32 );
  
        uint32_t test_f16_x2          = half_from_float_x2( test_in_x2 );
        uint64_t test_f32_x2          = half_to_float_x2( test_f16_x2 );
        uint32_t test_f16_reversed_x2 = half_from_float_x2( test_f32_x2 );
        uint16_t test_f16_reversed    = test_f16_reversed_x2 >> 16;
        uint16_t test_f16             = test_f16_x2 >> 16;
        uint16_t test_diff            = test_f16_reversed - test_f16;
        uint32_t test_f32             = test_f32_x2 >> 32;
  
        if ( test_diff > 0 )
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
           return;
        }
    }

    for ( test_ndx = 0; test_ndx < mixed_test_count; test_ndx++ )
    {
      int test_type = mixed_test[test_ndx] & 0x03;

      if ( test_type == 0 )
      {
        int i = mixed_test[test_ndx] % N_TESTS;
        if ( i == 0 ) i = 1;

        uint32_t test_in;
        uint16_t test_f16;
        uint64_t test_in_x2;
        uint32_t test_out_x2;

        test_in     = float_test_vector_special[i];
        test_in_x2  = test_in | ((uint64_t)test_in << 32);
        test_out_x2 = half_from_float_x2( test_in_x2 );
        test_f16    = test_out_x2 >> 16;
  
        if (test_f16 != half_test_vector_special[i])
        {
           FLOAT_32 in;
  
           in.u32  = test_in;

           printf("float->half: FAILED ");
           print_float( in.f32 );
           printf(" -> ");
           print_half( test_f16 );
           printf(" !- ");
           print_half( half_test_vector_special[i] );
           printf("\n");
           return;
        }
      }
      else
      {
        int i = mixed_test[test_ndx] % float_test_vector_count;

        uint32_t test_in;
        uint64_t test_in_x2;

        if ( test_type == 1 )
        {
          test_in  = float_test_vector_med[i];
        }
        else 
        {
          test_in  = float_test_vector_small[i];
        }

        test_in_x2 = test_in | ( (uint64_t)test_in << 32 );
  
        uint32_t test_f16_x2          = half_from_float_x2( test_in_x2 );
        uint64_t test_f32_x2          = half_to_float_x2( test_f16_x2 );
        uint32_t test_f16_reversed_x2 = half_from_float_x2( test_f32_x2 );
        uint16_t test_f16_reversed    = test_f16_reversed_x2 >> 16;
        uint16_t test_f16             = test_f16_x2 >> 16;
        uint16_t test_diff            = test_f16_reversed - test_f16;
        uint32_t test_f32             = test_f32_x2 >> 32;
  
        if ( test_diff > 0 )
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
           return;
        }
      }
    }
}

#define RUN_COUNT 100
void
test( void )
{   
    struct timeval start;
    struct timeval end;
    double total_time;
    int i;

    total_time=0;
    gettimeofday(&start,NULL);
    for (i=0;i<RUN_COUNT;i++)
    {
      test_conversion();
    }
    gettimeofday(&end,NULL);
    total_time += ((end.tv_sec-start.tv_sec)) + ((end.tv_usec-start.tv_usec) / 1.e6);

    printf("time:%g sec\n",total_time);
}

void
test_x2( void )
{   
    struct timeval start;
    struct timeval end;
    double total_time;
    int i;

    total_time=0;
    gettimeofday(&start,NULL);
    for (i=0;i<RUN_COUNT;i++)
    {
      test_conversion_x2();
    }
    gettimeofday(&end,NULL);
    total_time += ((end.tv_sec-start.tv_sec)) + ((end.tv_usec-start.tv_usec) / 1.e6);

    printf("time (x2):%g sec\n",total_time);
}


int
main( void )
{
  test();
  test_x2();
}
