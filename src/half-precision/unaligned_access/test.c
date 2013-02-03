#include <stdint.h>
#include <sim_printf.h>
#include <spu_intrinsics.h>
#include "unaligned_double.h"
#include "vsl.h"

typedef union DOUBLE_64 DOUBLE_64;
union DOUBLE_64
{
  double   f64;
  uint64_t u64;
  uint32_t u32[4];
  qword    qw;
};

void
print_double( const char* const pre, double arg )
{
  DOUBLE_64 D = {.f64 = arg};
  sim_printf("%s: %2.8g (0x%08x_%08x)\n",pre,D.f64,D.u32[0],D.u32[1]);
}

// NEG_PI=-3.14159 (c0 09 21 fb 54 44 2d 18)

char data[] = { 0xff, 0xff ,0xff, 0xff, 0xff, 0xff ,0xff, 0xff
               ,0x00, 0x01, 0x02, 0x03
               ,0xc0, 0x09, 0x21, 0xfb, 0x54, 0x44, 0x2d, 0x18
               ,0x04, 0x05, 0x06, 0x07
               ,0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

int
main( void )
{
  double  d0     = unaligned_load_double( data+12 );
  unaligned_store_double( data+12, 1.0 );
  unaligned_store_double( data+4,  4.0 );
  double  d1     = unaligned_load_double( data+12 );
  double  d2     = unaligned_load_double( data+4 );
  unaligned_store_double( data+16,  9.87654321 );
  unaligned_store_double( data+8,   1.23456789 );
  double  d3     = unaligned_load_double( data+8 );
  double  d4     = unaligned_load_double( data+16 );

  sim_printf("----------------------------\n");
  sim_printf("d0=%2.8g\n",d0);
  sim_printf("d1=%2.8g\n",d1);
  print_double("d1",d1);
  sim_printf("d2=%2.8g\n",d2);
  sim_printf("d3=%2.8g\n",d3);
  sim_printf("d4=%2.8g\n",d4);
  sim_printf("----------------------------\n");

  return (0);
}

