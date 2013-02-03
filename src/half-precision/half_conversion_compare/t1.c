#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int
main( void )
{
  int count = 64 * 1024;
  int i;

  printf("int mixed_test_count = %d;\n",count*4);
  printf("int mixed_test[] = {\n");
  for (i=0;i<count;i++)
  {
    int e0 = rand();
    int e1 = rand();
    int e2 = rand();
    int e3 = rand();

    printf("  0x%08x, 0x%08x, 0x%08x, 0x%08x,\n", e0,e1,e2,e3);
  }
  printf("};\n");

  return (0);
}
