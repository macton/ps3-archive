#include <stdint.h>
#include <stdio.h>

#if 0
static float32 addFloat32Sigs( float32 a, float32 b, flag zSign )
{
  uint16_t   ha_s = ha & 0x8000;
  uint16_t   ha_e = ha & 0x7c00;
  uint16_t   ha_m = ha & 0x03ff;
  uint16_t   hb_s = hb & 0x8000;
  uint16_t   hb_e = hb & 0x7c00;
  uint16_t   hb_m = hb & 0x03ff;

  int16_t    de = ha_e - hb_e;

  ha_m <<= 6;
  hb_m <<= 6;

  if ( de > 0 )
  {
    if ( ha_e == 0x7c00 )
    {
      // NAN - propagate m
      return (0xffff);
    }

    if ( hb_e == 0x0000 )
    {
      de--;
    }
    else 
    {
      bSig |= 0x20000000;
    }
        shift32RightJamming( bSig, expDiff, &bSig );
        zExp = aExp;
    }
    else if ( expDiff < 0 ) {
        if ( bExp == 0xFF ) {
            if ( bSig ) return propagateFloat32NaN( a, b );
            return packFloat32( zSign, 0xFF, 0 );
        }
        if ( aExp == 0 ) {
            ++expDiff;
        }
        else {
            aSig |= 0x20000000;
        }
        shift32RightJamming( aSig, - expDiff, &aSig );
        zExp = bExp;
    }
    else {
        if ( aExp == 0xFF ) {
            if ( aSig | bSig ) return propagateFloat32NaN( a, b );
            return a;
        }
        if ( aExp == 0 ) return packFloat32( zSign, 0, ( aSig + bSig )>>6 );
        zSig = 0x40000000 + aSig + bSig;
        zExp = aExp;
        goto roundAndPack;
    }
    aSig |= 0x20000000;
    zSig = ( aSig + bSig )<<1;
    --zExp;
    if ( (sbits32) zSig < 0 ) {
        zSig = aSig + bSig;
        ++zExp;
    }
 roundAndPack:
    return roundAndPackFloat32( zSign, zExp, zSig );

}

float32 float32_add( float32 a, float32 b )
{
    flag aSign, bSign;

    aSign = extractFloat32Sign( a );
    bSign = extractFloat32Sign( b );
    if ( aSign == bSign ) {
        return addFloat32Sigs( a, b, aSign );
    }
    else {
        return subFloat32Sigs( a, b, aSign );
    }

}
#endif

uint16_t
_half_add( uint16_t ha, uint16_t hb )
{
  uint16_t   ha_s = ha & 0x8000;
  uint16_t   ha_e = ha & 0x7c00;
  uint16_t   ha_m = ha & 0x03ff;
  uint16_t   hb_s = hb & 0x8000;
  uint16_t   hb_e = hb & 0x7c00;
  uint16_t   hb_m = hb & 0x03ff;

  uint16_t e   = ha_e;
  uint16_t s   = ha_s;
  uint16_t m0  = ha_m;
  uint16_t m1  = hb_m;
  uint16_t s0  = ha_s;
  uint16_t s1  = hb_s;

  int16_t    de = ha_e - hb_e;

  if ( ha_e == 0x7c00 )
  {
    printf("ha_e == 0x7c00\n");
  }
  else if ( ha_e == 0x0000 )
  {
    printf("ha_e == 0x0000\n");
  }

  if ( hb_e == 0x7c00 )
  {
    printf("hb_e == 0x7c00\n");
  }
  else if ( hb_e == 0x0000 )
  {
    printf("hb_e == 0x0000\n");
  }

  // Align radix points
  if ( ha_e > hb_e )
  {
    uint16_t m_sa  = ((ha_e >> 10) - (hb_e >> 10)) - 1;
    uint16_t adj_m = ( hb_m >> 1 ) | 0x0200;
    uint16_t sh_m  = adj_m >> m_sa;

    e    = ha_e;
    s    = ha_s;
    s0   = ha_s;
    s1   = hb_s;
    m0   = ha_m;
    m1   = sh_m;

printf("CASE: ha_e > hb_e\n");
  }

  else if ( ha_e < hb_e )
  {
    uint16_t m_sa  = ((hb_e >> 10) - (ha_e >> 10)) - 1;
    uint16_t adj_m = ( ha_m >> 1 ) | 0x0200;
    uint16_t sh_m  = ( adj_m ) >> m_sa;

    e    = hb_e;
    s    = hb_s;
    s0   = hb_s;
    s1   = ha_s;
    m0   = hb_m;
    m1   = sh_m;
printf("CASE: ha_e < hb_e\n");
printf("ha_m = 0x%04x hb_m = 0x%04x\n",ha_m,hb_m);
printf("m_sa = %d\n",m_sa);
  }

  if ( s0 ^ s1 )
  {
printf("SIGN DIFF\n");
    if ( s0 )
    {
      m0 = (~m0)+1;
    }
    else
    {
      m1 = (~m1)+1;
    }
  }

  uint16_t zm = m0 + m1;
  uint16_t ze = e;

printf("HERE-0 m0=0x%04x m1=0x%04x\n",m0,m1);
printf("HERE-1 zm=0x%04x ze=0x%04x\n",zm,ze);

  if ( (int16_t) zm < 0 ) 
  {
zm = zm >> 1;
ze = ze-1;
printf("HERE-2 zm=0x%04x ze=0x%04x\n",zm,ze);
    // zm = (m0 + m1) >> 1;
    // ze = e-1;
  }

  uint16_t h = 0x0 | s | ze | zm;

  // TODO: Handle mantissa overflow
  // TODO: Handle INF
  // TODO: Handle NaN

  return (h);
}

uint16_t
half_add( uint16_t a, uint16_t b )
{
  uint16_t   a_s = a & 0x8000;
  uint16_t   a_e = a & 0x7c00;
  uint16_t   a_m = a & 0x03ff;

  uint16_t   b_s = b & 0x8000;
  uint16_t   b_e = b & 0x7c00;
  uint16_t   b_m = b & 0x03ff;

printf("ENTER\n");
printf("HERE a_e=0x%04x a_m=0x%04x\n",a_e,a_m);
printf("HERE b_e=0x%04x b_m=0x%04x\n",b_e,b_m);

  if ( b_e > a_e )
  {
    uint16_t t_m = b_m;
    b_m = a_m;
    a_m = t_m;

    uint16_t t_e = b_e;
    b_e = a_e;
    a_e = t_e;

    a_s = b_s;
  }

  a_m |= 0x400;   
  b_m |= 0x400;   

  // a > b 

  // grs (guard + round + sticky), 3 bits

  uint16_t m_sa        = ((a_e >> 10) - (b_e >> 10));

  a_m <<= 3;
  b_m <<= 3;

printf("HERE a_e=0x%04x a_m=0x%04x\n",a_e,a_m);
printf("HERE b_e=0x%04x b_m=0x%04x\n",b_e,b_m);

  uint16_t sh_m        = b_m >> m_sa;
printf("m_sa=%d\n",m_sa);

  uint16_t sticky_mask = ( 1 << m_sa ) -1;
  uint16_t sticky = ( b_m & sticky_mask ) ? 0x0001 : 0x0000;

  printf("sticky_mask=0x%04x\n",sticky_mask);
  printf("sticky=0x%04x\n",sticky);

  b_m = sh_m | sticky;

  if ( a_s ^ b_s )
  {
printf("SIGN DIFF\n");
    if ( a_s )
    {
      a_m = (~a_m)+1;
    }
    else
    {
      b_m = (~b_m)+1;
    }
  }

  uint16_t z_m = a_m + b_m;
  uint16_t z_e = a_e;
  uint16_t z_s = a_s;

printf("HERE a_e=0x%04x a_m=0x%04x\n",a_e,a_m);
printf("HERE b_e=0x%04x b_m=0x%04x\n",b_e,b_m);
printf("PRESHIFT z_e=0x%04x z_m=0x%04x\n",z_e,z_m);

  if ( z_m & 0x4000 )
  {
    z_m = ( z_m >> 1 );
    z_e = ( ( z_e >> 10 ) + 1 ) << 10;
    printf("SHIFT_M z_e=0x%04x z_m=0x%04x\n",z_e,z_m);
  }

#if 0
  if ( z_m & 0x1000 )
  {
    z_m = ( z_m >> 1 );
    z_e = ( ( z_e >> 10 ) + 1 ) << 10;
    printf("SHIFT_M2 z_e=0x%04x z_m=0x%04x\n",z_e,z_m);
  }
#endif

  z_m = ( z_m >> 3 );

  // remove implicit bit
  z_m = z_m & 0x3ff;

printf("LAST z_e=0x%04x z_m=0x%04x\n",z_e,z_m);

  uint16_t h = z_s | z_e | z_m;

  return (h);
}

uint16_t 
half_sub( uint16_t ha, uint16_t hb ) 
{
  // MJA - (a-b) is the same as (a+(-b))
  // b and -b are the same except the sign
  // bit is flipped.
  // Just flip the sign bit and call add.

  return half_add( ha, hb ^ 0x8000 );
}

uint16_t 
half_mul( uint16_t arg0, uint16_t arg1 ) 
{
  // MJA - TODO
  return (0);
}

uint16_t 
half_div( uint16_t arg0, uint16_t arg1 ) 
{
  // MJA - TODO
  return (0);
}
