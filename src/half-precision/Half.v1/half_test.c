#include <stdio.h>
#include <stdint.h>
typedef union FLOAT_32 FLOAT_32;
union FLOAT_32
{
    float    f32;
    uint32_t u32;
};
uint16_t half_convert( uint32_t );

#define CHECK_HALF(arg0,arg1,arg2) if (arg0 != arg1) { FLOAT_32 f; f.u32 = arg2; test_count++; printf("[%s:%d] FAILED: 0x%04x != 0x%04x (%f)\n",__FILE__,__LINE__,arg0,arg1,f.f32); } else { test_count++; test_pass_count++; }

int main( void )
{
    FLOAT_32 test;
    uint16_t test_f16;
    int      test_count = 0;
    int      test_pass_count = 0;

    test_f16 = half_convert( 0x80000000 );
    CHECK_HALF( test_f16, 0x8000, 0x80000000 )
    test_f16 = half_convert( 0x0 );
    CHECK_HALF( test_f16, 0x0, 0x0 )
    test_f16 = half_convert( 0x3F800000 );
    CHECK_HALF( test_f16, 0x3C00, 0x3F800000 )
    test_f16 = half_convert( 0x3F802000 );
    CHECK_HALF( test_f16, 0x3C01, 0x3F802000 )
    test_f16 = half_convert( 0x3F802000 );
    CHECK_HALF( test_f16, 0x3C01, 0x3F802000 )
    test_f16 = half_convert( 0x3F800000 );
    CHECK_HALF( test_f16, 0x3C00, 0x3F800000 )
    test_f16 = half_convert( 0x3F802000 );
    CHECK_HALF( test_f16, 0x3C01, 0x3F802000 )
    test_f16 = half_convert( 0x3F7FE000 );
    CHECK_HALF( test_f16, 0x3BFF, 0x3F7FE000 )
    test_f16 = half_convert( 0x3F800000 );
    CHECK_HALF( test_f16, 0x3C00, 0x3F800000 )
    test_f16 = half_convert( 0x3F800000 );
    CHECK_HALF( test_f16, 0x3C00, 0x3F800000 )
    test_f16 = half_convert( 0x3F7FE000 );
    CHECK_HALF( test_f16, 0x3BFF, 0x3F7FE000 )
    test_f16 = half_convert( 0x33800000 );
    CHECK_HALF( test_f16, 0x1, 0x33800000 )
    test_f16 = half_convert( 0x34000000 );
    CHECK_HALF( test_f16, 0x2, 0x34000000 )
    test_f16 = half_convert( 0x34000000 );
    CHECK_HALF( test_f16, 0x2, 0x34000000 )
    test_f16 = half_convert( 0x33800000 );
    CHECK_HALF( test_f16, 0x1, 0x33800000 )
    test_f16 = half_convert( 0x34000000 );
    CHECK_HALF( test_f16, 0x2, 0x34000000 )
    test_f16 = half_convert( 0x0 );
    CHECK_HALF( test_f16, 0x0, 0x0 )
    test_f16 = half_convert( 0x33800000 );
    CHECK_HALF( test_f16, 0x1, 0x33800000 )
    test_f16 = half_convert( 0x33800000 );
    CHECK_HALF( test_f16, 0x1, 0x33800000 )
    test_f16 = half_convert( 0x0 );
    CHECK_HALF( test_f16, 0x0, 0x0 )
    test_f16 = half_convert( 0x38800000 );
    CHECK_HALF( test_f16, 0x400, 0x38800000 )
    test_f16 = half_convert( 0x38802000 );
    CHECK_HALF( test_f16, 0x401, 0x38802000 )
    test_f16 = half_convert( 0x38802000 );
    CHECK_HALF( test_f16, 0x401, 0x38802000 )
    test_f16 = half_convert( 0x38800000 );
    CHECK_HALF( test_f16, 0x400, 0x38800000 )
    test_f16 = half_convert( 0x38802000 );
    CHECK_HALF( test_f16, 0x401, 0x38802000 )
    test_f16 = half_convert( 0x387FC000 );
    CHECK_HALF( test_f16, 0x3FF, 0x387FC000 )
    test_f16 = half_convert( 0x38800000 );
    CHECK_HALF( test_f16, 0x400, 0x38800000 )
    test_f16 = half_convert( 0x38800000 );
    CHECK_HALF( test_f16, 0x400, 0x38800000 )
    test_f16 = half_convert( 0x387FC000 );
    CHECK_HALF( test_f16, 0x3FF, 0x387FC000 )
    test_f16 = half_convert( 0x40000000 );
    CHECK_HALF( test_f16, 0x4000, 0x40000000 )
    test_f16 = half_convert( 0x40400000 );
    CHECK_HALF( test_f16, 0x4200, 0x40400000 )
    test_f16 = half_convert( 0x41200000 );
    CHECK_HALF( test_f16, 0x4900, 0x41200000 )
    test_f16 = half_convert( 0x3DCCC000 );
    CHECK_HALF( test_f16, 0x2E66, 0x3DCCC000 )
    test_f16 = half_convert( 0x3E4CC000 );
    CHECK_HALF( test_f16, 0x3266, 0x3E4CC000 )
    test_f16 = half_convert( 0x3E99A000 );
    CHECK_HALF( test_f16, 0x34CD, 0x3E99A000 )
    test_f16 = half_convert( 0x477FE000 );
    CHECK_HALF( test_f16, 0x7BFF, 0x477FE000 )
    test_f16 = half_convert( 0x7F800000 );
    CHECK_HALF( test_f16, 0x7C00, 0x7F800000 )
    test_f16 = half_convert( 0x7F800000 );
    CHECK_HALF( test_f16, 0x7C00, 0x7F800000 )
    test_f16 = half_convert( 0x477FE000 );
    CHECK_HALF( test_f16, 0x7BFF, 0x477FE000 )
    test_f16 = half_convert( 0x7F800000 );
    CHECK_HALF( test_f16, 0x7C00, 0x7F800000 )
    test_f16 = half_convert( 0x7F800000 );
    CHECK_HALF( test_f16, 0x7C00, 0x7F800000 )
    test_f16 = half_convert( 0x7F800000 );
    CHECK_HALF( test_f16, 0x7C00, 0x7F800000 )
    test_f16 = half_convert( 0xBF800000 );
    CHECK_HALF( test_f16, 0xBC00, 0xBF800000 )
    test_f16 = half_convert( 0xBF802000 );
    CHECK_HALF( test_f16, 0xBC01, 0xBF802000 )
    test_f16 = half_convert( 0xBF802000 );
    CHECK_HALF( test_f16, 0xBC01, 0xBF802000 )
    test_f16 = half_convert( 0xBF800000 );
    CHECK_HALF( test_f16, 0xBC00, 0xBF800000 )
    test_f16 = half_convert( 0xBF802000 );
    CHECK_HALF( test_f16, 0xBC01, 0xBF802000 )
    test_f16 = half_convert( 0xBF7FE000 );
    CHECK_HALF( test_f16, 0xBBFF, 0xBF7FE000 )
    test_f16 = half_convert( 0xBF800000 );
    CHECK_HALF( test_f16, 0xBC00, 0xBF800000 )
    test_f16 = half_convert( 0xBF800000 );
    CHECK_HALF( test_f16, 0xBC00, 0xBF800000 )
    test_f16 = half_convert( 0xBF7FE000 );
    CHECK_HALF( test_f16, 0xBBFF, 0xBF7FE000 )
    test_f16 = half_convert( 0xB3800000 );
    CHECK_HALF( test_f16, 0x8001, 0xB3800000 )
    test_f16 = half_convert( 0xB4000000 );
    CHECK_HALF( test_f16, 0x8002, 0xB4000000 )
    test_f16 = half_convert( 0xB4000000 );
    CHECK_HALF( test_f16, 0x8002, 0xB4000000 )
    test_f16 = half_convert( 0xB3800000 );
    CHECK_HALF( test_f16, 0x8001, 0xB3800000 )
    test_f16 = half_convert( 0xB4000000 );
    CHECK_HALF( test_f16, 0x8002, 0xB4000000 )
    test_f16 = half_convert( 0x0 );
    CHECK_HALF( test_f16, 0x0, 0x0 )
    test_f16 = half_convert( 0xB3800000 );
    CHECK_HALF( test_f16, 0x8001, 0xB3800000 )
    test_f16 = half_convert( 0xB3800000 );
    CHECK_HALF( test_f16, 0x8001, 0xB3800000 )
    test_f16 = half_convert( 0x0 );
    CHECK_HALF( test_f16, 0x0, 0x0 )
    test_f16 = half_convert( 0xB8800000 );
    CHECK_HALF( test_f16, 0x8400, 0xB8800000 )
    test_f16 = half_convert( 0xB8802000 );
    CHECK_HALF( test_f16, 0x8401, 0xB8802000 )
    test_f16 = half_convert( 0xB8802000 );
    CHECK_HALF( test_f16, 0x8401, 0xB8802000 )
    test_f16 = half_convert( 0xB8800000 );
    CHECK_HALF( test_f16, 0x8400, 0xB8800000 )
    test_f16 = half_convert( 0xB8802000 );
    CHECK_HALF( test_f16, 0x8401, 0xB8802000 )
    test_f16 = half_convert( 0xB87FC000 );
    CHECK_HALF( test_f16, 0x83FF, 0xB87FC000 )
    test_f16 = half_convert( 0xB8800000 );
    CHECK_HALF( test_f16, 0x8400, 0xB8800000 )
    test_f16 = half_convert( 0xB8800000 );
    CHECK_HALF( test_f16, 0x8400, 0xB8800000 )
    test_f16 = half_convert( 0xB87FC000 );
    CHECK_HALF( test_f16, 0x83FF, 0xB87FC000 )
    test_f16 = half_convert( 0xC0000000 );
    CHECK_HALF( test_f16, 0xC000, 0xC0000000 )
    test_f16 = half_convert( 0xC0400000 );
    CHECK_HALF( test_f16, 0xC200, 0xC0400000 )
    test_f16 = half_convert( 0xC1200000 );
    CHECK_HALF( test_f16, 0xC900, 0xC1200000 )
    test_f16 = half_convert( 0xBDCCC000 );
    CHECK_HALF( test_f16, 0xAE66, 0xBDCCC000 )
    test_f16 = half_convert( 0xBE4CC000 );
    CHECK_HALF( test_f16, 0xB266, 0xBE4CC000 )
    test_f16 = half_convert( 0xBE99A000 );
    CHECK_HALF( test_f16, 0xB4CD, 0xBE99A000 )
    test_f16 = half_convert( 0xC77FE000 );
    CHECK_HALF( test_f16, 0xFBFF, 0xC77FE000 )
    test_f16 = half_convert( 0xFF800000 );
    CHECK_HALF( test_f16, 0xFC00, 0xFF800000 )
    test_f16 = half_convert( 0xFF800000 );
    CHECK_HALF( test_f16, 0xFC00, 0xFF800000 )
    test_f16 = half_convert( 0xC77FE000 );
    CHECK_HALF( test_f16, 0xFBFF, 0xC77FE000 )
    test_f16 = half_convert( 0xFF800000 );
    CHECK_HALF( test_f16, 0xFC00, 0xFF800000 )

    printf("[%s] Passed %d of %d\n", __FILE__, test_pass_count, test_count);
    return (0);
}
