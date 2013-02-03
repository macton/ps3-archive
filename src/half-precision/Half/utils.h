void 
print_u32( char* str, uint32_t arg )
{
  int i;

  printf("%s 0x%08x ",str, (unsigned int)arg);
  printf(" b");

  for ( i = 31; i >= 0; i-- ) 
  {
    int bit = (int)(( arg & (1 << i) ) >> i);
    printf("%d",bit);
  }         
  printf("\n");
}

void 
print_u16( char* str, uint16_t arg )
{
  int i;

  printf("%s 0x%04x ",str, (unsigned int)arg);
  printf(" b");
  for ( i = 15; i >= 0; i-- ) 
  {
    int bit = (int)(( arg & (1 << i) ) >> i);
    printf("%d",bit);
  }         
  printf("\n");
}
