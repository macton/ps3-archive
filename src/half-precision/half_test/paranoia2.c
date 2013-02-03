// Floating point paranoia tests,
//
//    Mike Acton, Copyright Sep. 2006
//    macton@cellperformance.com
//    http://www.cellperformance.com
// 
//
// Based on paranoia.c, by:
//
//    David M. Gay           and      Thos Sumner
//    AT&T Bell Labs                  Computer Center, Rm. U-76
//    600 Mountain Avenue             University of California
//    Murray Hill, NJ 07974           San Francisco, CA 94143
//    USA                             USA
//
// Was was a conversion from the pascal version by:
//
//    B. A. Wichmann
//    National Physical Laboratory
//    Teddington Middx
//    TW11 OLW
//    UK
//
// Which was based on the original version, in BASIC version by:
//
//    Professor W. M. Kahan, (C) Apr 19 1983
//    567 Evans Hall
//    Electrical Engineering & Computer Science Dept.
//    University of California
//    Berkeley, California 94720
//    USA

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define ftype_log logf

typedef float ftype;

static inline ftype ftype_zero( void )
{
  return (0.0f);
}

static inline ftype ftype_one( void )
{
  return (1.0f);
}

static inline ftype ftype_mul( ftype a, ftype b )
{
  return (a * b);
}

static inline ftype ftype_add( ftype a, ftype b )
{
  return (a + b);
}

static inline ftype ftype_sub( ftype a, ftype b )
{
  return (a - b);
}

static inline ftype ftype_div( ftype a, ftype b )
{
  return (a / b);
}

static inline ftype ftype_neg( ftype a )
{
  return (-a);
}

static inline ftype ftype_abs( ftype a )
{
  return fabs(a);
}

static inline ftype ftype_floor( ftype a )
{
  return (ftype)(int32_t)(a);
}

static inline int ftype_cmp_equal( ftype a, ftype b )
{
  return ( a == b);
}

static inline int ftype_cmp_not_equal( ftype a, ftype b )
{
  return ( a != b);
}

static inline int ftype_cmp_greater_than( ftype a, ftype b )
{
  return ( a > b);
}

static inline int ftype_cmp_less_than( ftype a, ftype b )
{
  return ( a < b);
}

static int test_equal( const char* const restrict str, ftype a, ftype b )
{
  if (!ftype_cmp_equal(a,b))
  {
    printf("\n    FAILED: %s\n",str);
    return (1);
  }

  return (0);
}

static int test_not_equal( const char* const restrict str, ftype a, ftype b )
{
  if (!ftype_cmp_not_equal(a,b))
  {
    printf("\n    FAILED: %s\n",str);
    return (1);
  }

  return (0);
}

static int test_greater_than( const char* const restrict str, ftype a, ftype b )
{
  if (!ftype_cmp_greater_than(a,b))
  {
    printf("\n    FAILED: %s\n",str);
    return (1);
  }

  return (0);
}

ftype
ftype_calculate_W( void )
{
    const ftype Zero     = ftype_zero();
    const ftype One      = ftype_one();
    const ftype MinusOne = ftype_neg( One );

    ftype Y;
    ftype Z;
    ftype W;
    ftype Radix;

    W = One;
    do  
    {
        W = ftype_add( W, W );
        Y = ftype_add( W, One );
        Z = ftype_sub( Y, W );
        Y = ftype_sub( Z, One );
        Z = ftype_add( MinusOne, ftype_abs(Y) );
    } 
    while (ftype_cmp_less_than(Z,Zero));

    /*.. now W is just big enough that |((W+1)-W)-1| >= 1 ...*/
    return (W);
}

ftype
ftype_calculate_radix( void )
{
    const ftype Zero     = ftype_zero();
    const ftype One      = ftype_one();
    const ftype MinusOne = ftype_neg( One );
    const ftype Two      = ftype_add( One, One );

    ftype X;
    ftype Y;
    ftype Z;
    ftype W;
    ftype Radix;

    W  = ftype_calculate_W();
    Y  = One;
    do  
    {
        Radix = ftype_add( W,     Y );
        Y     = ftype_add( Y,     Y );
        Radix = ftype_sub( Radix, W );
    }
    while (ftype_cmp_equal(Radix, Zero));

    if (ftype_cmp_less_than(Radix,Two))
    {
        Radix = One;
    }

    return (Radix);
}

ftype
ftype_calculate_U1( void )
{
    const ftype One   = ftype_one();
    ftype       Radix = ftype_calculate_radix();
    ftype       W     = ftype_calculate_W();
   
    ftype Y;
    ftype Z;

    if (ftype_cmp_not_equal(Radix,One))
    {
        W = One;
        do  
        {
            W         = ftype_mul( W,         Radix );
            Y         = ftype_add( W,         One   );
            Z         = ftype_sub(Y,W);
        } 
        while (ftype_cmp_equal(Z,One));
    }

    /*... now W == Radix^Precision is barely too big to satisfy (W+1)-W == 1 ...*/
    const ftype U1 = ftype_div( One,   W  );

    return (U1);
}

ftype
ftype_calculate_precision_alt( void )
{
    const ftype zero             = ftype_zero();
    const ftype one              = ftype_one();
    const ftype two              = ftype_add( one,    one   );
    const ftype one_half         = ftype_div( one,    two );
    const ftype three            = ftype_add( two,    one   );
    const ftype four             = ftype_add( three,  one   );
    const ftype five             = ftype_add( four,   one   );
    const ftype eight            = ftype_add( four,   four  );
    const ftype nine             = ftype_mul( three,  three );
    const ftype twenty_seven     = ftype_mul( nine,   three );
    const ftype thirty_two       = ftype_mul( four,   eight );
    const ftype twenty           = ftype_mul( four,   five  );
    const ftype sixty            = ftype_mul( twenty, three );
    const ftype two_forty        = ftype_mul( sixty,  four  );
    const ftype neg_two_forty    = ftype_neg( two_forty );
    const ftype radix            = ftype_calculate_radix();

printf("radix=%f\n",radix);

    if (ftype_cmp_not_equal(radix,one))
    {

        const ftype U1          = ftype_calculate_U1();
        const ftype LOG_U1      = ftype_log(U1);
        const ftype LOG_RADIX   = ftype_log(radix);

        const ftype X0          = ftype_mul(neg_two_forty,LOG_U1);
        const ftype X1          = ftype_div(X0,LOG_RADIX);

        const ftype Y0          = ftype_add(one_half,X1);
        const ftype Y1          = ftype_floor(Y0);
        const ftype Z0          = ftype_sub(X1,Y1);
        const ftype Z1          = ftype_abs(Z0);
        const ftype Z2          = ftype_mul(Z1,four);

        ftype X = X1;
        if ( ftype_cmp_less_than(Z2,one) )
        {
            X = Y1;
        }

printf("X1=%f\n",X1);

        ftype Precision = ftype_div( X, two_forty );

        const ftype Q0        = ftype_add(one_half, Precision);
        const ftype Q1        = ftype_floor(Q0);
        const ftype R0        = ftype_sub(Precision,Q1);
        const ftype R1        = ftype_abs(R0);
        const ftype R2        = ftype_mul(R1,two_forty);

        if ( ftype_cmp_less_than(R2,one_half) )
        {
            Precision = Q1;
        }

        return (Precision);
    }

    return (zero);
}

uint16_t
first_primes[ 1000 ] = 
{
    0x0002, 0x0003, 0x0005, 0x0007, 0x000b, 0x000d, 0x0011, 0x0013, 0x0017, 0x001d, 
    0x001f, 0x0025, 0x0029, 0x002b, 0x002f, 0x0035, 0x003b, 0x003d, 0x0043, 0x0047, 
    0x0049, 0x004f, 0x0053, 0x0059, 0x0061, 0x0065, 0x0067, 0x006b, 0x006d, 0x0071, 
    0x007f, 0x0083, 0x0089, 0x008b, 0x0095, 0x0097, 0x009d, 0x00a3, 0x00a7, 0x00ad, 
    0x00b3, 0x00b5, 0x00bf, 0x00c1, 0x00c5, 0x00c7, 0x00d3, 0x00df, 0x00e3, 0x00e5, 
    0x00e9, 0x00ef, 0x00f1, 0x00fb, 0x0101, 0x0107, 0x010d, 0x010f, 0x0115, 0x0119, 
    0x011b, 0x0125, 0x0133, 0x0137, 0x0139, 0x013d, 0x014b, 0x0151, 0x015b, 0x015d, 
    0x0161, 0x0167, 0x016f, 0x0175, 0x017b, 0x017f, 0x0185, 0x018d, 0x0191, 0x0199, 
    0x01a3, 0x01a5, 0x01af, 0x01b1, 0x01b7, 0x01bb, 0x01c1, 0x01c9, 0x01cd, 0x01cf, 
    0x01d3, 0x01df, 0x01e7, 0x01eb, 0x01f3, 0x01f7, 0x01fd, 0x0209, 0x020b, 0x021d, 
    0x0223, 0x022d, 0x0233, 0x0239, 0x023b, 0x0241, 0x024b, 0x0251, 0x0257, 0x0259, 
    0x025f, 0x0265, 0x0269, 0x026b, 0x0277, 0x0281, 0x0283, 0x0287, 0x028d, 0x0293, 
    0x0295, 0x02a1, 0x02a5, 0x02ab, 0x02b3, 0x02bd, 0x02c5, 0x02cf, 0x02d7, 0x02dd, 
    0x02e3, 0x02e7, 0x02ef, 0x02f5, 0x02f9, 0x0301, 0x0305, 0x0313, 0x031d, 0x0329, 
    0x032b, 0x0335, 0x0337, 0x033b, 0x033d, 0x0347, 0x0355, 0x0359, 0x035b, 0x035f, 
    0x036d, 0x0371, 0x0373, 0x0377, 0x038b, 0x038f, 0x0397, 0x03a1, 0x03a9, 0x03ad, 
    0x03b3, 0x03b9, 0x03c7, 0x03cb, 0x03d1, 0x03d7, 0x03df, 0x03e5, 0x03f1, 0x03f5, 
    0x03fb, 0x03fd, 0x0407, 0x0409, 0x040f, 0x0419, 0x041b, 0x0425, 0x0427, 0x042d, 
    0x043f, 0x0443, 0x0445, 0x0449, 0x044f, 0x0455, 0x045d, 0x0463, 0x0469, 0x047f, 
    0x0481, 0x048b, 0x0493, 0x049d, 0x04a3, 0x04a9, 0x04b1, 0x04bd, 0x04c1, 0x04c7, 
    0x04cd, 0x04cf, 0x04d5, 0x04e1, 0x04eb, 0x04fd, 0x04ff, 0x0503, 0x0509, 0x050b, 
    0x0511, 0x0515, 0x0517, 0x051b, 0x0527, 0x0529, 0x052f, 0x0551, 0x0557, 0x055d, 
    0x0565, 0x0577, 0x0581, 0x058f, 0x0593, 0x0595, 0x0599, 0x059f, 0x05a7, 0x05ab, 
    0x05ad, 0x05b3, 0x05bf, 0x05c9, 0x05cb, 0x05cf, 0x05d1, 0x05d5, 0x05db, 0x05e7, 
    0x05f3, 0x05fb, 0x0607, 0x060d, 0x0611, 0x0617, 0x061f, 0x0623, 0x062b, 0x062f, 
    0x063d, 0x0641, 0x0647, 0x0649, 0x064d, 0x0653, 0x0655, 0x065b, 0x0665, 0x0679, 
    0x067f, 0x0683, 0x0685, 0x069d, 0x06a1, 0x06a3, 0x06ad, 0x06b9, 0x06bb, 0x06c5, 
    0x06cd, 0x06d3, 0x06d9, 0x06df, 0x06f1, 0x06f7, 0x06fb, 0x06fd, 0x0709, 0x0713, 
    0x071f, 0x0727, 0x0737, 0x0745, 0x074b, 0x074f, 0x0751, 0x0755, 0x0757, 0x0761, 
    0x076d, 0x0773, 0x0779, 0x078b, 0x078d, 0x079d, 0x079f, 0x07b5, 0x07bb, 0x07c3, 
    0x07c9, 0x07cd, 0x07cf, 0x07d3, 0x07db, 0x07e1, 0x07eb, 0x07ed, 0x07f7, 0x0805, 
    0x080f, 0x0815, 0x0821, 0x0823, 0x0827, 0x0829, 0x0833, 0x083f, 0x0841, 0x0851, 
    0x0853, 0x0859, 0x085d, 0x085f, 0x0869, 0x0871, 0x0883, 0x089b, 0x089f, 0x08a5, 
    0x08ad, 0x08bd, 0x08bf, 0x08c3, 0x08cb, 0x08db, 0x08dd, 0x08e1, 0x08e9, 0x08ef, 
    0x08f5, 0x08f9, 0x0905, 0x0907, 0x091d, 0x0923, 0x0925, 0x092b, 0x092f, 0x0935, 
    0x0943, 0x0949, 0x094d, 0x094f, 0x0955, 0x0959, 0x095f, 0x096b, 0x0971, 0x0977, 
    0x0985, 0x0989, 0x098f, 0x099b, 0x09a3, 0x09a9, 0x09ad, 0x09c7, 0x09d9, 0x09e3, 
    0x09eb, 0x09ef, 0x09f5, 0x09f7, 0x09fd, 0x0a13, 0x0a1f, 0x0a21, 0x0a31, 0x0a39, 
    0x0a3d, 0x0a49, 0x0a57, 0x0a61, 0x0a63, 0x0a67, 0x0a6f, 0x0a75, 0x0a7b, 0x0a7f, 
    0x0a81, 0x0a85, 0x0a8b, 0x0a93, 0x0a97, 0x0a99, 0x0a9f, 0x0aa9, 0x0aab, 0x0ab5, 
    0x0abd, 0x0ac1, 0x0acf, 0x0ad9, 0x0ae5, 0x0ae7, 0x0aed, 0x0af1, 0x0af3, 0x0b03, 
    0x0b11, 0x0b15, 0x0b1b, 0x0b23, 0x0b29, 0x0b2d, 0x0b3f, 0x0b47, 0x0b51, 0x0b57, 
    0x0b5d, 0x0b65, 0x0b6f, 0x0b7b, 0x0b89, 0x0b8d, 0x0b93, 0x0b99, 0x0b9b, 0x0bb7, 
    0x0bb9, 0x0bc3, 0x0bcb, 0x0bcf, 0x0bdd, 0x0be1, 0x0be9, 0x0bf5, 0x0bfb, 0x0c07, 
    0x0c0b, 0x0c11, 0x0c25, 0x0c2f, 0x0c31, 0x0c41, 0x0c5b, 0x0c5f, 0x0c61, 0x0c6d, 
    0x0c73, 0x0c77, 0x0c83, 0x0c89, 0x0c91, 0x0c95, 0x0c9d, 0x0cb3, 0x0cb5, 0x0cb9, 
    0x0cbb, 0x0cc7, 0x0ce3, 0x0ce5, 0x0ceb, 0x0cf1, 0x0cf7, 0x0cfb, 0x0d01, 0x0d03, 
    0x0d0f, 0x0d13, 0x0d1f, 0x0d21, 0x0d2b, 0x0d2d, 0x0d3d, 0x0d3f, 0x0d4f, 0x0d55, 
    0x0d69, 0x0d79, 0x0d81, 0x0d85, 0x0d87, 0x0d8b, 0x0d8d, 0x0da3, 0x0dab, 0x0db7, 
    0x0dbd, 0x0dc7, 0x0dc9, 0x0dcd, 0x0dd3, 0x0dd5, 0x0ddb, 0x0de5, 0x0de7, 0x0df3, 
    0x0dfd, 0x0dff, 0x0e09, 0x0e17, 0x0e1d, 0x0e21, 0x0e27, 0x0e2f, 0x0e35, 0x0e3b, 
    0x0e4b, 0x0e57, 0x0e59, 0x0e5d, 0x0e6b, 0x0e71, 0x0e75, 0x0e7d, 0x0e87, 0x0e8f, 
    0x0e95, 0x0e9b, 0x0eb1, 0x0eb7, 0x0eb9, 0x0ec3, 0x0ed1, 0x0ed5, 0x0edb, 0x0eed, 
    0x0eef, 0x0ef9, 0x0f07, 0x0f0b, 0x0f0d, 0x0f17, 0x0f25, 0x0f29, 0x0f31, 0x0f43, 
    0x0f47, 0x0f4d, 0x0f4f, 0x0f53, 0x0f59, 0x0f5b, 0x0f67, 0x0f6b, 0x0f7f, 0x0f95, 
    0x0fa1, 0x0fa3, 0x0fa7, 0x0fad, 0x0fb3, 0x0fb5, 0x0fbb, 0x0fd1, 0x0fd3, 0x0fd9, 
    0x0fe9, 0x0fef, 0x0ffb, 0x0ffd, 0x1003, 0x100f, 0x101f, 0x1021, 0x1025, 0x102b, 
    0x1039, 0x103d, 0x103f, 0x1051, 0x1069, 0x1073, 0x1079, 0x107b, 0x1085, 0x1087, 
    0x1091, 0x1093, 0x109d, 0x10a3, 0x10a5, 0x10af, 0x10b1, 0x10bb, 0x10c1, 0x10c9, 
    0x10e7, 0x10f1, 0x10f3, 0x10fd, 0x1105, 0x110b, 0x1115, 0x1127, 0x112d, 0x1139, 
    0x1145, 0x1147, 0x1159, 0x115f, 0x1163, 0x1169, 0x116f, 0x1181, 0x1183, 0x118d, 
    0x119b, 0x11a1, 0x11a5, 0x11a7, 0x11ab, 0x11c3, 0x11c5, 0x11d1, 0x11d7, 0x11e7, 
    0x11ef, 0x11f5, 0x11fb, 0x120d, 0x121d, 0x121f, 0x1223, 0x1229, 0x122b, 0x1231, 
    0x1237, 0x1241, 0x1247, 0x1253, 0x125f, 0x1271, 0x1273, 0x1279, 0x127d, 0x128f, 
    0x1297, 0x12af, 0x12b3, 0x12b5, 0x12b9, 0x12bf, 0x12c1, 0x12cd, 0x12d1, 0x12df, 
    0x12fd, 0x1307, 0x130d, 0x1319, 0x1327, 0x132d, 0x1337, 0x1343, 0x1345, 0x1349, 
    0x134f, 0x1357, 0x135d, 0x1367, 0x1369, 0x136d, 0x137b, 0x1381, 0x1387, 0x138b, 
    0x1391, 0x1393, 0x139d, 0x139f, 0x13af, 0x13bb, 0x13c3, 0x13d5, 0x13d9, 0x13df, 
    0x13eb, 0x13ed, 0x13f3, 0x13f9, 0x13ff, 0x141b, 0x1421, 0x142f, 0x1433, 0x143b, 
    0x1445, 0x144d, 0x1459, 0x146b, 0x146f, 0x1471, 0x1475, 0x148d, 0x1499, 0x149f, 
    0x14a1, 0x14b1, 0x14b7, 0x14bd, 0x14cb, 0x14d5, 0x14e3, 0x14e7, 0x1505, 0x150b, 
    0x1511, 0x1517, 0x151f, 0x1525, 0x1529, 0x152b, 0x1537, 0x153d, 0x1541, 0x1543, 
    0x1549, 0x155f, 0x1565, 0x1567, 0x156b, 0x157d, 0x157f, 0x1583, 0x158f, 0x1591, 
    0x1597, 0x159b, 0x15b5, 0x15bb, 0x15c1, 0x15c5, 0x15cd, 0x15d7, 0x15f7, 0x1607, 
    0x1609, 0x160f, 0x1613, 0x1615, 0x1619, 0x161b, 0x1625, 0x1633, 0x1639, 0x163d, 
    0x1645, 0x164f, 0x1655, 0x1669, 0x166d, 0x166f, 0x1675, 0x1693, 0x1697, 0x169f, 
    0x16a9, 0x16af, 0x16b5, 0x16bd, 0x16c3, 0x16cf, 0x16d3, 0x16d9, 0x16db, 0x16e1, 
    0x16e5, 0x16eb, 0x16ed, 0x16f7, 0x16f9, 0x1709, 0x170f, 0x1723, 0x1727, 0x1733, 
    0x1741, 0x175d, 0x1763, 0x1777, 0x177b, 0x178d, 0x1795, 0x179b, 0x179f, 0x17a5, 
    0x17b3, 0x17b9, 0x17bf, 0x17c9, 0x17cb, 0x17d5, 0x17e1, 0x17e9, 0x17f3, 0x17f5, 
    0x17ff, 0x1807, 0x1813, 0x181d, 0x1835, 0x1837, 0x183b, 0x1843, 0x1849, 0x184d, 
    0x1855, 0x1867, 0x1871, 0x1877, 0x187d, 0x187f, 0x1885, 0x188f, 0x189b, 0x189d, 
    0x18a7, 0x18ad, 0x18b3, 0x18b9, 0x18c1, 0x18c7, 0x18d1, 0x18d7, 0x18d9, 0x18df, 
    0x18e5, 0x18eb, 0x18f5, 0x18fd, 0x1915, 0x191b, 0x1931, 0x1933, 0x1945, 0x1949, 
    0x1951, 0x195b, 0x1979, 0x1981, 0x1993, 0x1997, 0x1999, 0x19a3, 0x19a9, 0x19ab, 
    0x19b1, 0x19b5, 0x19c7, 0x19cf, 0x19db, 0x19ed, 0x19fd, 0x1a03, 0x1a05, 0x1a11, 
    0x1a17, 0x1a21, 0x1a23, 0x1a2d, 0x1a2f, 0x1a35, 0x1a3f, 0x1a4d, 0x1a51, 0x1a69, 
    0x1a6b, 0x1a7b, 0x1a7d, 0x1a87, 0x1a89, 0x1a93, 0x1aa7, 0x1aab, 0x1aad, 0x1ab1, 
    0x1ab9, 0x1ac9, 0x1acf, 0x1ad5, 0x1ad7, 0x1ae3, 0x1af3, 0x1afb, 0x1aff, 0x1b05, 
    0x1b23, 0x1b25, 0x1b2f, 0x1b31, 0x1b37, 0x1b3b, 0x1b41, 0x1b47, 0x1b4f, 0x1b55, 
    0x1b59, 0x1b65, 0x1b6b, 0x1b73, 0x1b7f, 0x1b83, 0x1b91, 0x1b9d, 0x1ba7, 0x1bbf, 
    0x1bc5, 0x1bd1, 0x1bd7, 0x1bd9, 0x1bef, 0x1bf7, 0x1c09, 0x1c13, 0x1c19, 0x1c27, 
    0x1c2b, 0x1c2d, 0x1c33, 0x1c3d, 0x1c45, 0x1c4b, 0x1c4f, 0x1c55, 0x1c73, 0x1c81, 
    0x1c8b, 0x1c8d, 0x1c99, 0x1ca3, 0x1ca5, 0x1cb5, 0x1cb7, 0x1cc9, 0x1ce1, 0x1cf3, 
    0x1cf9, 0x1d09, 0x1d1b, 0x1d21, 0x1d23, 0x1d35, 0x1d39, 0x1d3f, 0x1d41, 0x1d4b, 
    0x1d53, 0x1d5d, 0x1d63, 0x1d69, 0x1d71, 0x1d75, 0x1d7b, 0x1d7d, 0x1d87, 0x1d89, 
    0x1d95, 0x1d99, 0x1d9f, 0x1da5, 0x1da7, 0x1db3, 0x1db7, 0x1dc5, 0x1dd7, 0x1ddb, 
    0x1de1, 0x1df5, 0x1df9, 0x1e01, 0x1e07, 0x1e0b, 0x1e13, 0x1e17, 0x1e25, 0x1e2b, 
    0x1e2f, 0x1e3d, 0x1e49, 0x1e4d, 0x1e4f, 0x1e6d, 0x1e71, 0x1e89, 0x1e8f, 0x1e95, 
    0x1ea1, 0x1ead, 0x1ebb, 0x1ec1, 0x1ec5, 0x1ec7, 0x1ecb, 0x1edd, 0x1ee3, 0x1eef, 
};

int
small_integer_tests( void )
{
    printf("Small Integer Tests...");
    fflush( stdout );

    int failure_count = 0;

    {
      const ftype zero   = ftype_zero();
      const ftype a      = ftype_add( zero, zero );
      const ftype b      = zero;

      failure_count += test_equal( "0+0 != 0", a, b );
    }

    {
      const ftype zero   = ftype_zero();
      const ftype one    = ftype_one();
      const ftype a      = ftype_sub( one, one );
      const ftype b      = zero;

      failure_count += test_equal( "1-1 != 0", a, b );
    }

    {
      const ftype zero = ftype_zero();
      const ftype one  = ftype_one();
      const ftype a    = one;
      const ftype b    = zero;

      failure_count += test_greater_than( "1 <= 0", a, b );
    }

    {
      const ftype zero = ftype_zero();
      const ftype a    = ftype_neg( zero );
      const ftype b    = zero;

      failure_count += test_equal( "-0 != 0", a, b );
    }

    {
      const ftype one    = ftype_one();
      const ftype two    = ftype_add( one, one );
      const ftype a      = ftype_add( two, one );
      const ftype b      = ftype_add( one, two );

      failure_count += test_equal( "(2+1) != (1+2)", a, b );
    }


    {
      const ftype one    = ftype_one();
      const ftype two    = ftype_add( one,   one   );
      const ftype three  = ftype_add( two,   one   );
      const ftype a      = ftype_add( one,   three );
      const ftype b      = ftype_add( three, one   );

      failure_count += test_equal( "(2+1) != (1+2)", a, b );
    }

    {
      const ftype zero     = ftype_zero();
      const ftype one      = ftype_one();
      const ftype two      = ftype_add( one,   one   );
      const ftype three    = ftype_add( two,   one   );
      const ftype four     = ftype_add( three, one   );
      const ftype neg_two  = ftype_neg( two );
      const ftype neg_four = ftype_mul( two, neg_two );

      const ftype a        = ftype_add( four, neg_four );
      const ftype b        = zero;

      failure_count += test_equal( "4+(2*(-2)) != 0", a, b );
    }

    {
      const ftype zero     = ftype_zero();
      const ftype one      = ftype_one();
      const ftype two      = ftype_add( one,   one   );
      const ftype three    = ftype_add( two,   one   );
      const ftype four     = ftype_add( three, one   );
      const ftype one_b    = ftype_sub( four,  three );
      const ftype zero_b   = ftype_sub( one_b, one   );

      const ftype a        = zero_b;
      const ftype b        = zero;

      failure_count += test_equal( "4-3-1 != 0", a, b );
    }

    {
      const ftype zero     = ftype_zero();
      const ftype one      = ftype_one();

      const ftype a        = ftype_neg( one );
      const ftype b        = ftype_sub( zero, one );

      failure_count += test_equal( "-1 != 0-1", a, b );
    }

    {
      const ftype zero      = ftype_zero();
      const ftype one       = ftype_one();
      const ftype minus_one = ftype_neg( one );

      const ftype a         = ftype_add( minus_one, one );
      const ftype b         = zero;

      failure_count += test_equal( "-1+1 != 0", a, b );
    }

    {
      const ftype zero      = ftype_zero();
      const ftype one       = ftype_one();
      const ftype minus_one = ftype_neg( one );

      const ftype a         = ftype_add( one, minus_one );
      const ftype b         = zero;

      failure_count += test_equal( "1+(-1) != 0", a, b );
    }

    {
      const ftype zero      = ftype_zero();
      const ftype one       = ftype_one();
      const ftype minus_one = ftype_neg( one );
      const ftype abs_one   = ftype_abs( one );

      const ftype a         = ftype_add( minus_one, abs_one );
      const ftype b         = zero;

      failure_count += test_equal( "-1+abs(1) != 0", a, b );
    }

    {
      const ftype zero      = ftype_zero();
      const ftype one       = ftype_one();
      const ftype minus_one = ftype_neg( one );
      const ftype one_b     = ftype_mul( minus_one, minus_one );
      const ftype zero_b    = ftype_add( minus_one, one_b );

      const ftype a         = zero_b;
      const ftype b         = zero;

      failure_count += test_equal( "-1+((-1)*(-1)) != 0", a, b );
    }

    {
      const ftype zero           = ftype_zero();
      const ftype one            = ftype_one();
      const ftype two            = ftype_add( one, one );
      const ftype one_half       = ftype_div( one, two );
      const ftype minus_one      = ftype_neg( one );
      const ftype minus_one_half = ftype_add( one_half, minus_one );
      const ftype zero_b         = ftype_add( minus_one_half, one_half );

      const ftype a         = zero_b;
      const ftype b         = zero;

      failure_count += test_equal( "0.5+(-1)+0.5 != 0", a, b );
    }

    {
      const ftype zero         = ftype_zero();
      const ftype one          = ftype_one();
      const ftype two          = ftype_add( one,    one   );
      const ftype three        = ftype_add( two,    one   );
      const ftype four         = ftype_add( three,  one   );
      const ftype five         = ftype_add( four,   one   );
      const ftype eight        = ftype_add( four,   four  );
      const ftype nine         = ftype_mul( three,  three );
      const ftype twenty_seven = ftype_mul( nine,   three );
      const ftype thirty_two   = ftype_mul( four,   eight );

      const ftype five_b       = ftype_sub( thirty_two, twenty_seven );
      const ftype one_b        = ftype_sub( five_b,     four );
      const ftype zero_b       = ftype_sub( one_b,      one  );

      const ftype a           = zero_b;
      const ftype b           = zero;

      failure_count += test_equal( "32-27-4-1 != 0", a, b );
    }
 
    {
      const ftype zero         = ftype_zero();
      const ftype one          = ftype_one();
      const ftype two          = ftype_add( one,    one   );
      const ftype three        = ftype_add( two,    one   );
      const ftype four         = ftype_add( three,  one   );
      const ftype five         = ftype_add( four,   one   );
      const ftype eight        = ftype_add( four,   four  );
      const ftype nine         = ftype_mul( three,  three );
      const ftype twenty_seven = ftype_mul( nine,   three );
      const ftype thirty_two   = ftype_mul( four,   eight );
      const ftype twenty       = ftype_mul( four,   five  );
      const ftype sixty        = ftype_mul( twenty, three );
      const ftype two_forty    = ftype_mul( sixty,  four  );

      const ftype eighty       = ftype_div( two_forty, three    );
      const ftype sixteen      = ftype_mul( four,      four     );
      const ftype eighty_b     = ftype_mul( sixteen,   five     );
      const ftype zero_b       = ftype_sub( eighty,    eighty_b );
   
      const ftype a            = zero_b;
      const ftype b            = zero;

      failure_count += test_equal( "(240/3)-(4*4*5) != 0", a, b );
    }

    {
      const ftype zero         = ftype_zero();
      const ftype one          = ftype_one();
      const ftype two          = ftype_add( one,    one   );
      const ftype three        = ftype_add( two,    one   );
      const ftype four         = ftype_add( three,  one   );
      const ftype five         = ftype_add( four,   one   );
      const ftype eight        = ftype_add( four,   four  );
      const ftype nine         = ftype_mul( three,  three );
      const ftype twenty_seven = ftype_mul( nine,   three );
      const ftype thirty_two   = ftype_mul( four,   eight );
      const ftype twenty       = ftype_mul( four,   five  );
      const ftype sixty        = ftype_mul( twenty, three );
      const ftype two_forty    = ftype_mul( sixty,  four  );

      const ftype sixty_b      = ftype_div( two_forty, four    );
      const ftype fifteen      = ftype_mul( five,      three   );
      const ftype sixty_c      = ftype_mul( fifteen,   four    );
      const ftype zero_b       = ftype_sub( sixty_b,   sixty_c );
   
      const ftype a            = zero_b;
      const ftype b            = zero;

      failure_count += test_equal( "(240/4)-(5*3*4) != 0", a, b );
    }

    {
      const ftype zero          = ftype_zero();
      const ftype one           = ftype_one();
      const ftype two           = ftype_add( one,    one   );
      const ftype three         = ftype_add( two,    one   );
      const ftype four          = ftype_add( three,  one   );
      const ftype five          = ftype_add( four,   one   );
      const ftype eight         = ftype_add( four,   four  );
      const ftype nine          = ftype_mul( three,  three );
      const ftype twenty_seven  = ftype_mul( nine,   three );
      const ftype thirty_two    = ftype_mul( four,   eight );
      const ftype twenty        = ftype_mul( four,   five  );
      const ftype sixty         = ftype_mul( twenty, three );
      const ftype two_forty     = ftype_mul( sixty,  four  );

      const ftype forty_eight   = ftype_div( two_forty,   five  );
      const ftype twelve        = ftype_mul( four,        three );
      const ftype forty_eight_b = ftype_mul( twelve,      four  );
      const ftype zero_b        = ftype_sub( forty_eight, forty_eight_b );
   
      const ftype a             = zero_b;
      const ftype b             = zero;

      failure_count += test_equal( "(240/5)-(4*3*4) != 0", a, b );
    }

    if ( failure_count == 0 )
    {
      printf(" PASSED.\n");
    }

    return (failure_count);
}

int
small_prime_integer_tests( void )
{
  int i;
  int j;
  int failure_count = 0;
  int test_count    = 0;

  printf("Testing prime divisors... ");
  fflush( stdout );

  const float One  = 1.0f;
  const float Zero = 0.0f;

  for (i=0;i<1000;i++)
  {
      const uint16_t p = first_primes[i];

      for (j=i+1;j<1000;j++)
      {
          const uint32_t q = first_primes[j];
          const uint32_t n = p * q;
          const float    P = (float)p;
          const float    Q = (float)q;
          const float    N = (float)P * Q;

          if (N+One == N)
          {
              break;
          }

          {
              const float tau_0     = (P-1) * (Q-1);
              const float tau_1     = N + One -P -Q;
              const float tau_diff  = tau_0 - tau_1;
              char        tau_desc[256];

              sprintf(tau_desc,"(%d-1)*(%d-1) != %d+1-(%d+%d)\n",p,q,n,p,q);
              failure_count += test_equal( tau_desc, tau_diff, Zero );
          }

          {
              const float tau_0     = (P-1);
              const float tau_1     = ( N + One -P -Q ) / (Q-1);
              const float tau_diff  = tau_0 - tau_1;
              char        tau_desc[256];

              sprintf(tau_desc,"(%d-1) != (%d+1-(%d+%d))/(%d-1)\n",p,n,p,q,q);

              failure_count += test_equal( tau_desc, tau_diff, Zero );
          }

          {
              const float tau_0     = (Q-1);
              const float tau_1     = ( N + One -P -Q ) / (P-1);
              const float tau_diff  = tau_0 - tau_1;
              char        tau_desc[256];

              sprintf(tau_desc,"(%d-1) != (%d+1-(%d+%d))/(%d-1)\n",q,n,p,q,p);

              failure_count += test_equal( tau_desc, tau_diff, Zero );
          }

          {
              const float sigma_0     = (P+1) * (Q+1);
              const float sigma_1     = N + One + ( P + Q );
              const float sigma_diff  = sigma_0 - sigma_1;
              char        sigma_desc[256];

              sprintf(sigma_desc,"(%d+1)*(%d+1) != %d+1+(%d+%d)\n",p,q,n,p,q);

              failure_count += test_equal( sigma_desc, sigma_diff, Zero );
          }

          {
              const float sigma_0     = (Q+1);
              const float sigma_1     = (N + One + ( P + Q ) ) / (P+1);
              const float sigma_diff  = sigma_0 - sigma_1;
              char        sigma_desc[256];

              sprintf(sigma_desc,"(%d+1) != (%d+1+(%d+%d))/(%d+1)\n",q,n,p,q,p);

              failure_count += test_equal( sigma_desc, sigma_diff, Zero );
          }

          {
              const float sigma_0     = (P+1);
              const float sigma_1     = (N + One + ( P + Q ) ) / (Q+1);
              const float sigma_diff  = sigma_0 - sigma_1;
              char        sigma_desc[256];

              sprintf(sigma_desc,"(%d+1) != (%d+1+(%d+%d))/(%d+1)\n",p,n,p,q,q);

              failure_count += test_equal( sigma_desc, sigma_diff, Zero );
          }
      }
  }
  
  if ( failure_count == 0 )
  {
      printf("PASSED\n");
  }

  return (failure_count);
}
int
radix_and_precision_tests( void )
{
    int failure_count = 0;

    printf("Searching for Radix and Precision...");
    fflush( stdout );

    const ftype One           = ftype_one();
    const ftype Zero          = ftype_zero();
    const ftype MinusOne      = ftype_neg( One );
    const ftype Two           = ftype_add( One, One );
    const ftype Three         = ftype_add( Two, One );
    const ftype Half          = ftype_div( One, Two );
    const ftype Four          = ftype_add( Three, One   );
    const ftype Eight         = ftype_add( Four, Four );
    const ftype ThirtyTwo     = ftype_mul( Four, Eight );

    ftype W;
    ftype X;
    ftype Y;
    ftype Z;
    ftype U1;
    ftype U2;
    ftype Radix;
    ftype Precision;
    ftype Third;
    ftype E0, E1, E3;
    ftype E9;
    ftype F6;
    ftype F9;

    W = One;
    do  
    {
        W = ftype_add( W, W );
        Y = ftype_add( W, One );
        Z = ftype_sub( Y, W );
        Y = ftype_sub( Z, One );
    } 
    while (MinusOne + ftype_abs(Y) < Zero);

    /*.. now W is just big enough that |((W+1)-W)-1| >= 1 ...*/
    Precision = Zero;
    Y         = One;

    do  
    {
        Radix = ftype_add( W,     Y );
        Y     = ftype_add( Y,     Y );
        Radix = ftype_sub( Radix, W );
    }
    while (Radix == Zero);

    if (Radix < Two)
    {
        Radix = One;
    }

    failure_count += test_equal( "Radix != 2", Radix, Two );

    if (Radix != 1) 
    {
        W = One;
        do  {
            Precision = ftype_add( Precision, One   );
            W         = ftype_mul( W,         Radix );
            Y         = ftype_add( W,         One   );
        } 
        while ((Y - W) == One);
    }
    /*... now W == Radix^Precision is barely too big to satisfy (W+1)-W == 1 ...*/

   

    U1 = One / W;
    U2 = Radix * U1;

    {
      union F32 { float f32; uint32_t u32; } U1_F;
      U1_F.f32 = U1;
      printf("\nClosest relative separation found is U1 = %.7e . (0x%08x)\n\n", U1,U1_F.u32);
    }

    if ( failure_count == 0 )
    {
        printf(" PASSED.\n");
    }

    printf("Recalculating radix and precision\n ");
    
    /*save old values*/
    E0 = Radix;
    E1 = U1;
    E9 = U2;
    E3 = Precision;
    
    X     = ftype_div( Four, Three );
    Third = ftype_sub( X,    One   );
    F6    = ftype_sub( Half, Third );
    X     = ftype_add( F6,   F6    );
    X     = ftype_abs(X - Third);

    if (X < U2)
    {
        X = U2;
    }
    
    /*... now X = (unknown no.) ulps of 1+...*/
    do  
    {
        U2 = X;
        Y = Half * U2 + ThirtyTwo * U2 * U2;
        Y = One + Y;
        X = Y - One;
    } 
    while ( ! ((U2 <= X) || (X <= Zero)));
    
    /*... now U2 == 1 ulp of 1 + ... */
    X = Two / Three;
    F6 = X - Half;
    Third = F6 + F6;
    X = Third - Half;
    X = ftype_abs(X + F6);
    if (X < U1) X = U1;
    
    /*... now  X == (unknown no.) ulps of 1 -... */
    do  {
        U1 = X;
        Y = Half * U1 + ThirtyTwo * U1 * U1;
        Y = Half - Y;
        X = Half + Y;
        Y = Half - X;
        X = Half + Y;
        } while ( ! ((U1 <= X) || (X <= Zero)));
    /*... now U1 == 1 ulp of 1 - ... */
    if (U1 == E1) printf("confirms closest relative separation U1 .\n");
    else printf("gets better closest relative separation U1 = %.7e .\n", U1);
    W = One / U1;
    F9 = (Half - U1) + Half;
    Radix = ftype_floor(0.01 + U2 / U1);
    if (Radix == E0) printf("Radix confirmed.\n");
    else printf("MYSTERY: recalculated Radix = %.7e .\n", Radix);

    if (Radix != One)  {

      const ftype zero          = ftype_zero();
      const ftype one           = ftype_one();
      const ftype two           = ftype_add( one,    one   );
      const ftype three         = ftype_add( two,    one   );
      const ftype four          = ftype_add( three,  one   );
      const ftype five          = ftype_add( four,   one   );
      const ftype eight         = ftype_add( four,   four  );
      const ftype nine          = ftype_mul( three,  three );
      const ftype twenty_seven  = ftype_mul( nine,   three );
      const ftype thirty_two    = ftype_mul( four,   eight );
      const ftype twenty        = ftype_mul( four,   five  );
      const ftype sixty         = ftype_mul( twenty, three );
      const ftype two_forty     = ftype_mul( sixty,  four  );

        X = - two_forty * logf(U1) / logf(Radix);
        Y = ftype_floor(Half + X);
        if (ftype_abs(X - Y) * Four < One) X = Y;
        Precision = X / two_forty;
        Y = ftype_floor(Half + Precision);
        if (ftype_abs(Precision - Y) * two_forty < Half) Precision = Y;
        }

    printf("X=%f\n",X);
    printf("Radix=%f\n",Radix);
    printf("U1=%f\n",U1);
    ftype U1Alt = ftype_calculate_U1();
    printf("U1Alt=%f\n",U1Alt);
    printf("PrecisionAlt=%f\n",Precision);
    ftype Precision2 = ftype_calculate_precision_alt();
    printf("Precision2 =%f\n",Precision2);

    return (failure_count);
}
#if 0
    W = One;
    do  {
        W = W + W;
        Y = W + One;
        Z = Y - W;
        Y = Z - One;
        } while (MinusOne + ftype_abs(Y) < Zero);
    /*.. now W is just big enough that |((W+1)-W)-1| >= 1 ...*/
    Precision = Zero;
    Y = One;
    do  {
        Radix = W + Y;
        Y = Y + Y;
        Radix = Radix - W;
        } while ( Radix == Zero);
    if (Radix < Two) Radix = One;
    printf("Radix = %f .\n", Radix);
    if (Radix != 1) {
        W = One;
        do  {
            Precision = Precision + One;
            W = W * Radix;
            Y = W + One;
            } while ((Y - W) == One);
        }
    /*... now W == Radix^Precision is barely too big to satisfy (W+1)-W == 1
                                          ...*/
    U1 = One / W;
    U2 = Radix * U1;
    printf("Closest relative separation found is U1 = %.7e .\n\n", U1);
    printf("Recalculating radix and precision\n ");
    
    /*save old values*/
    E0 = Radix;
    E1 = U1;
    E9 = U2;
    E3 = Precision;
    
    X = Four / Three;
    Third = X - One;
    F6 = Half - Third;
    X = F6 + F6;
    X = ftype_abs(X - Third);
    if (X < U2) X = U2;
    
    /*... now X = (unknown no.) ulps of 1+...*/
    do  {
        U2 = X;
        Y = Half * U2 + ThirtyTwo * U2 * U2;
        Y = One + Y;
        X = Y - One;
        } while ( ! ((U2 <= X) || (X <= Zero)));
    
    /*... now U2 == 1 ulp of 1 + ... */
    X = Two / Three;
    F6 = X - Half;
    Third = F6 + F6;
    X = Third - Half;
    X = ftype_abs(X + F6);
    if (X < U1) X = U1;
    
    /*... now  X == (unknown no.) ulps of 1 -... */
    do  {
        U1 = X;
        Y = Half * U1 + ThirtyTwo * U1 * U1;
        Y = Half - Y;
        X = Half + Y;
        Y = Half - X;
        X = Half + Y;
        } while ( ! ((U1 <= X) || (X <= Zero)));
    /*... now U1 == 1 ulp of 1 - ... */
    if (U1 == E1) printf("confirms closest relative separation U1 .\n");
    else printf("gets better closest relative separation U1 = %.7e .\n", U1);
    W = One / U1;
    F9 = (Half - U1) + Half;
    Radix = ftype_floor(0.01 + U2 / U1);
    if (Radix == E0) printf("Radix confirmed.\n");
    else printf("MYSTERY: recalculated Radix = %.7e .\n", Radix);

    TstCond (Defect, Radix <= Eight + Eight,
           "Radix is too big: roundoff problems");
    TstCond (Flaw, (Radix == Two) || (Radix == 10)
           || (Radix == One), "Radix is not as good as 2 or 10");
    /*=============================================*/
    Milestone = 20;
    /*=============================================*/
    TstCond (Failure, F9 - Half < Half,
           "(1-U1)-1/2 < 1/2 is FALSE, prog. fails?");
    X = F9;
    I = 1;
    Y = X - Half;
    Z = Y - Half;
    TstCond (Failure, (X != One)
           || (Z == Zero), "Comparison is fuzzy,X=1 but X-1/2-1/2 != 0");
    X = One + U2;
    I = 0;
    /*=============================================*/
    Milestone = 25;
    /*=============================================*/
    /*... BMinusU2 = nextafter(Radix, 0) */
    BMinusU2 = Radix - One;
    BMinusU2 = (BMinusU2 - U2) + One;
    /* Purify Integers */
    if (Radix != One)  {
        X = - TwoForty * LOG(U1) / LOG(Radix);
        Y = ftype_floor(Half + X);
        if (ftype_abs(X - Y) * Four < One) X = Y;
        Precision = X / TwoForty;
        Y = ftype_floor(Half + Precision);
        if (ftype_abs(Precision - Y) * TwoForty < Half) Precision = Y;
        }
    if ((Precision != ftype_floor(Precision)) || (Radix == One)) {
        printf("Precision cannot be characterized by an Integer number\n");
        printf("of significant digits but, by itself, this is a minor flaw.\n");
        }
    if (Radix == One) 
        printf("logarithmic encoding has precision characterized solely by U1.\n");
    else printf("The number of significant digits of the Radix is %f .\n",
            Precision);
    TstCond (Serious, U2 * Nine * Nine * TwoForty < One,
           "Precision worse than 5 decimal figures  ");
    /*=============================================*/
    Milestone = 30;
    /*=============================================*/
    /* Test for extra-precise subepressions */
    X = ftype_abs(((Four / Three - One) - One / Four) * Three - One / Four);
    do  {
        Z2 = X;
        X = (One + (Half * Z2 + ThirtyTwo * Z2 * Z2)) - One;
        } while ( ! ((Z2 <= X) || (X <= Zero)));
    X = Y = Z = ftype_abs((Three / Four - Two / Three) * Three - One / Four);
    do  {
        Z1 = Z;
        Z = (One / Two - ((One / Two - (Half * Z1 + ThirtyTwo * Z1 * Z1))
            + One / Two)) + One / Two;
        } while ( ! ((Z1 <= Z) || (Z <= Zero)));
    do  {
        do  {
            Y1 = Y;
            Y = (Half - ((Half - (Half * Y1 + ThirtyTwo * Y1 * Y1)) + Half
                )) + Half;
            } while ( ! ((Y1 <= Y) || (Y <= Zero)));
        X1 = X;
        X = ((Half * X1 + ThirtyTwo * X1 * X1) - F9) + F9;
        } while ( ! ((X1 <= X) || (X <= Zero)));
    if ((X1 != Y1) || (X1 != Z1)) {
        BadCond(Serious, "Disagreements among the values X1, Y1, Z1,\n");
        printf("respectively  %.7e,  %.7e,  %.7e,\n", X1, Y1, Z1);
        printf("are symptoms of inconsistencies introduced\n");
        printf("by extra-precise evaluation of arithmetic subexpressions.\n");
        notify("Possibly some part of this");
        if ((X1 == U1) || (Y1 == U1) || (Z1 == U1))  printf(
            "That feature is not tested further by this program.\n") ;
        }
    else  {
        if ((Z1 != U1) || (Z2 != U2)) {
            if ((Z1 >= U1) || (Z2 >= U2)) {
                BadCond(Failure, "");
                notify("Precision");
                printf("\tU1 = %.7e, Z1 - U1 = %.7e\n",U1,Z1-U1);
                printf("\tU2 = %.7e, Z2 - U2 = %.7e\n",U2,Z2-U2);
                }
            else {
                if ((Z1 <= Zero) || (Z2 <= Zero)) {
                    printf("Because of unusual Radix = %f", Radix);
                    printf(", or exact rational arithmetic a result\n");
                    printf("Z1 = %.7e, or Z2 = %.7e ", Z1, Z2);
                    notify("of an\nextra-precision");
                    }
                if (Z1 != Z2 || Z1 > Zero) {
                    X = Z1 / U1;
                    Y = Z2 / U2;
                    if (Y > X) X = Y;
                    Q = - LOG(X);
                    printf("Some subexpressions appear to be calculated extra\n");
                    printf("precisely with about %g extra B-digits, i.e.\n",
                        (Q / LOG(Radix)));
                    printf("roughly %g extra significant decimals.\n",
                        Q / LOG(10.));
                    }
                printf("That feature is not tested further by this program.\n");
                }
            }
        }
    Pause();
    /*=============================================*/
    /*SPLIT
    }
#include "paranoia.h"
void part3(VOID){
*/
    Milestone = 35;
    /*=============================================*/
    if (Radix >= Two) {
        X = W / (Radix * Radix);
        Y = X + One;
        Z = Y - X;
        T = Z + U2;
        X = T - Z;
        TstCond (Failure, X == U2,
            "Subtraction is not normalized X=Y,X+Z != Y+Z!");
        if (X == U2) printf(
            "Subtraction appears to be normalized, as it should be.");
        }
    printf("\nChecking for guard digit in *, /, and -.\n");
    Y = F9 * One;
    Z = One * F9;
    X = F9 - Half;
    Y = (Y - Half) - X;
    Z = (Z - Half) - X;
    X = One + U2;
    T = X * Radix;
    R = Radix * X;
    X = T - Radix;
    X = X - Radix * U2;
    T = R - Radix;
    T = T - Radix * U2;
    X = X * (Radix - One);
    T = T * (Radix - One);
    if ((X == Zero) && (Y == Zero) && (Z == Zero) && (T == Zero)) GMult = Yes;
    else {
        GMult = No;
        TstCond (Serious, False,
            "* lacks a Guard Digit, so 1*X != X");
        }
    Z = Radix * U2;
    X = One + Z;
    Y = ftype_abs((X + Z) - X * X) - U2;
    X = One - U2;
    Z = ftype_abs((X - U2) - X * X) - U1;
    TstCond (Failure, (Y <= Zero)
           && (Z <= Zero), "* gets too many final digits wrong.\n");
    Y = One - U2;
    X = One + U2;
    Z = One / Y;
    Y = Z - X;
    X = One / Three;
    Z = Three / Nine;
    X = X - Z;
    T = Nine / TwentySeven;
    Z = Z - T;
    TstCond(Defect, X == Zero && Y == Zero && Z == Zero,
        "Division lacks a Guard Digit, so error can exceed 1 ulp\n\
or  1/3  and  3/9  and  9/27 may disagree");
    Y = F9 / One;
    X = F9 - Half;
    Y = (Y - Half) - X;
    X = One + U2;
    T = X / One;
    X = T - X;
    if ((X == Zero) && (Y == Zero) && (Z == Zero)) GDiv = Yes;
    else {
        GDiv = No;
        TstCond (Serious, False,
            "Division lacks a Guard Digit, so X/1 != X");
        }
    X = One / (One + U2);
    Y = X - Half - Half;
    TstCond (Serious, Y < Zero,
           "Computed value of 1/1.000..1 >= 1");
    X = One - U2;
    Y = One + Radix * U2;
    Z = X * Radix;
    T = Y * Radix;
    R = Z / Radix;
    StickyBit = T / Radix;
    X = R - X;
    Y = StickyBit - Y;
    TstCond (Failure, X == Zero && Y == Zero,
            "* and/or / gets too many last digits wrong");
    Y = One - U1;
    X = One - F9;
    Y = One - Y;
    T = Radix - U2;
    Z = Radix - BMinusU2;
    T = Radix - T;
    if ((X == U1) && (Y == U1) && (Z == U2) && (T == U2)) GAddSub = Yes;
    else {
        GAddSub = No;
        TstCond (Serious, False,
            "- lacks Guard Digit, so cancellation is obscured");
        }
    if (F9 != One && F9 - One >= Zero) {
        BadCond(Serious, "comparison alleges  (1-U1) < 1  although\n");
        printf("  subtraction yields  (1-U1) - 1 = 0 , thereby vitiating\n");
        printf("  such precautions against division by zero as\n");
        printf("  ...  if (X == 1.0) {.....} else {.../(X-1.0)...}\n");
        }
    if (GMult == Yes && GDiv == Yes && GAddSub == Yes) printf(
        "     *, /, and - appear to have guard digits, as they should.\n");
    /*=============================================*/
    Milestone = 40;
    /*=============================================*/
    Pause();
    printf("Checking rounding on multiply, divide and add/subtract.\n");
    RMult = Other;
    RDiv = Other;
    RAddSub = Other;
    RadixD2 = Radix / Two;
    A1 = Two;
    Done = False;
    do  {
        AInvrse = Radix;
        do  {
            X = AInvrse;
            AInvrse = AInvrse / A1;
            } while ( ! (ftype_floor(AInvrse) != AInvrse));
        Done = (X == One) || (A1 > Three);
        if (! Done) A1 = Nine + One;
        } while ( ! (Done));
    if (X == One) A1 = Radix;
    AInvrse = One / A1;
    X = A1;
    Y = AInvrse;
    Done = False;
    do  {
        Z = X * Y - Half;
        TstCond (Failure, Z == Half,
            "X * (1/X) differs from 1");
        Done = X == Radix;
        X = Radix;
        Y = One / X;
        } while ( ! (Done));
    Y2 = One + U2;
    Y1 = One - U2;
    X = OneAndHalf - U2;
    Y = OneAndHalf + U2;
    Z = (X - U2) * Y2;
    T = Y * Y1;
    Z = Z - X;
    T = T - X;
    X = X * Y2;
    Y = (Y + U2) * Y1;
    X = X - OneAndHalf;
    Y = Y - OneAndHalf;
    if ((X == Zero) && (Y == Zero) && (Z == Zero) && (T <= Zero)) {
        X = (OneAndHalf + U2) * Y2;
        Y = OneAndHalf - U2 - U2;
        Z = OneAndHalf + U2 + U2;
        T = (OneAndHalf - U2) * Y1;
        X = X - (Z + U2);
        StickyBit = Y * Y1;
        S = Z * Y2;
        T = T - Y;
        Y = (U2 - Y) + StickyBit;
        Z = S - (Z + U2 + U2);
        StickyBit = (Y2 + U2) * Y1;
        Y1 = Y2 * Y1;
        StickyBit = StickyBit - Y2;
        Y1 = Y1 - Half;
        if ((X == Zero) && (Y == Zero) && (Z == Zero) && (T == Zero)
            && ( StickyBit == Zero) && (Y1 == Half)) {
            RMult = Rounded;
            printf("Multiplication appears to round correctly.\n");
            }
        else    if ((X + U2 == Zero) && (Y < Zero) && (Z + U2 == Zero)
                && (T < Zero) && (StickyBit + U2 == Zero)
                && (Y1 < Half)) {
                RMult = Chopped;
                printf("Multiplication appears to chop.\n");
                }
            else printf("* is neither chopped nor correctly rounded.\n");
        if ((RMult == Rounded) && (GMult == No)) notify("Multiplication");
        }
    else printf("* is neither chopped nor correctly rounded.\n");
    /*=============================================*/
    Milestone = 45;
    /*=============================================*/
    Y2 = One + U2;
    Y1 = One - U2;
    Z = OneAndHalf + U2 + U2;
    X = Z / Y2;
    T = OneAndHalf - U2 - U2;
    Y = (T - U2) / Y1;
    Z = (Z + U2) / Y2;
    X = X - OneAndHalf;
    Y = Y - T;
    T = T / Y1;
    Z = Z - (OneAndHalf + U2);
    T = (U2 - OneAndHalf) + T;
    if (! ((X > Zero) || (Y > Zero) || (Z > Zero) || (T > Zero))) {
        X = OneAndHalf / Y2;
        Y = OneAndHalf - U2;
        Z = OneAndHalf + U2;
        X = X - Y;
        T = OneAndHalf / Y1;
        Y = Y / Y1;
        T = T - (Z + U2);
        Y = Y - Z;
        Z = Z / Y2;
        Y1 = (Y2 + U2) / Y2;
        Z = Z - OneAndHalf;
        Y2 = Y1 - Y2;
        Y1 = (F9 - U1) / F9;
        if ((X == Zero) && (Y == Zero) && (Z == Zero) && (T == Zero)
            && (Y2 == Zero) && (Y2 == Zero)
            && (Y1 - Half == F9 - Half )) {
            RDiv = Rounded;
            printf("Division appears to round correctly.\n");
            if (GDiv == No) notify("Division");
            }
        else if ((X < Zero) && (Y < Zero) && (Z < Zero) && (T < Zero)
            && (Y2 < Zero) && (Y1 - Half < F9 - Half)) {
            RDiv = Chopped;
            printf("Division appears to chop.\n");
            }
        }
    if (RDiv == Other) printf("/ is neither chopped nor correctly rounded.\n");
    BInvrse = One / Radix;
    TstCond (Failure, (BInvrse * Radix - Half == Half),
           "Radix * ( 1 / Radix ) differs from 1");
    /*=============================================*/
    /*SPLIT
    }
#include "paranoia.h"
void part4(VOID){
*/
    Milestone = 50;
    /*=============================================*/
    TstCond (Failure, ((F9 + U1) - Half == Half)
           && ((BMinusU2 + U2 ) - One == Radix - One),
           "Incomplete carry-propagation in Addition");
    X = One - U1 * U1;
    Y = One + U2 * (One - U2);
    Z = F9 - Half;
    X = (X - Half) - Z;
    Y = Y - One;
    if ((X == Zero) && (Y == Zero)) {
        RAddSub = Chopped;
        printf("Add/Subtract appears to be chopped.\n");
        }
    if (GAddSub == Yes) {
        X = (Half + U2) * U2;
        Y = (Half - U2) * U2;
        X = One + X;
        Y = One + Y;
        X = (One + U2) - X;
        Y = One - Y;
        if ((X == Zero) && (Y == Zero)) {
            X = (Half + U2) * U1;
            Y = (Half - U2) * U1;
            X = One - X;
            Y = One - Y;
            X = F9 - X;
            Y = One - Y;
            if ((X == Zero) && (Y == Zero)) {
                RAddSub = Rounded;
                printf("Addition/Subtraction appears to round correctly.\n");
                if (GAddSub == No) notify("Add/Subtract");
                }
            else printf("Addition/Subtraction neither rounds nor chops.\n");
            }
        else printf("Addition/Subtraction neither rounds nor chops.\n");
        }
    else printf("Addition/Subtraction neither rounds nor chops.\n");
    S = One;
    X = One + Half * (One + Half);
    Y = (One + U2) * Half;
    Z = X - Y;
    T = Y - X;
    StickyBit = Z + T;
    if (StickyBit != Zero) {
        S = Zero;
        BadCond(Flaw, "(X - Y) + (Y - X) is non zero!\n");
        }
    StickyBit = Zero;
    if ((GMult == Yes) && (GDiv == Yes) && (GAddSub == Yes)
        && (RMult == Rounded) && (RDiv == Rounded)
        && (RAddSub == Rounded) && (ftype_floor(RadixD2) == RadixD2)) {
        printf("Checking for sticky bit.\n");
        X = (Half + U1) * U2;
        Y = Half * U2;
        Z = One + Y;
        T = One + X;
        if ((Z - One <= Zero) && (T - One >= U2)) {
            Z = T + Y;
            Y = Z - X;
            if ((Z - T >= U2) && (Y - T == Zero)) {
                X = (Half + U1) * U1;
                Y = Half * U1;
                Z = One - Y;
                T = One - X;
                if ((Z - One == Zero) && (T - F9 == Zero)) {
                    Z = (Half - U1) * U1;
                    T = F9 - Z;
                    Q = F9 - Y;
                    if ((T - F9 == Zero) && (F9 - U1 - Q == Zero)) {
                        Z = (One + U2) * OneAndHalf;
                        T = (OneAndHalf + U2) - Z + U2;
                        X = One + Half / Radix;
                        Y = One + Radix * U2;
                        Z = X * Y;
                        if (T == Zero && X + Radix * U2 - Z == Zero) {
                            if (Radix != Two) {
                                X = Two + U2;
                                Y = X / Two;
                                if ((Y - One == Zero)) StickyBit = S;
                                }
                            else StickyBit = S;
                            }
                        }
                    }
                }
            }
        }
    if (StickyBit == One) printf("Sticky bit apparently used correctly.\n");
    else printf("Sticky bit used incorrectly or not at all.\n");
    TstCond (Flaw, !(GMult == No || GDiv == No || GAddSub == No ||
            RMult == Other || RDiv == Other || RAddSub == Other),
        "lack(s) of guard digits or failure(s) to correctly round or chop\n\
(noted above) count as one flaw in the final tally below");
    /*=============================================*/
    Milestone = 60;
    /*=============================================*/
    printf("\n");
    printf("Does Multiplication commute?  ");
    printf("Testing on %d random pairs.\n", NoTrials);
    Random9 = SQRT(3.0);
    Random1 = Third;
    I = 1;
    do  {
        X = Random();
        Y = Random();
        Z9 = Y * X;
        Z = X * Y;
        Z9 = Z - Z9;
        I = I + 1;
        } while ( ! ((I > NoTrials) || (Z9 != Zero)));
    if (I == NoTrials) {
        Random1 = One + Half / Three;
        Random2 = (U2 + U1) + One;
        Z = Random1 * Random2;
        Y = Random2 * Random1;
        Z9 = (One + Half / Three) * ((U2 + U1) + One) - (One + Half /
            Three) * ((U2 + U1) + One);
        }
    if (! ((I == NoTrials) || (Z9 == Zero)))
        BadCond(Defect, "X * Y == Y * X trial fails.\n");
    else printf("     No failures found in %d integer pairs.\n", NoTrials);
    /*=============================================*/
    Milestone = 70;
    /*=============================================*/
    printf("\nRunning test of square root(x).\n");
    TstCond (Failure, (Zero == SQRT(Zero))
           && (- Zero == SQRT(- Zero))
           && (One == SQRT(One)), "Square root of 0.0, -0.0 or 1.0 wrong");
    MinSqEr = Zero;
    MaxSqEr = Zero;
    J = Zero;
    X = Radix;
    OneUlp = U2;
    SqXMinX (Serious);
    X = BInvrse;
    OneUlp = BInvrse * U1;
    SqXMinX (Serious);
    X = U1;
    OneUlp = U1 * U1;
    SqXMinX (Serious);
    if (J != Zero) Pause();
    printf("Testing if sqrt(X * X) == X for %d Integers X.\n", NoTrials);
    J = Zero;
    X = Two;
    Y = Radix;
    if ((Radix != One)) do  {
        X = Y;
        Y = Radix * Y;
        } while ( ! ((Y - X >= NoTrials)));
    OneUlp = X * U2;
    I = 1;
    while (I <= NoTrials) {
        X = X + One;
        SqXMinX (Defect);
        if (J > Zero) break;
        I = I + 1;
        }
    printf("Test for sqrt monotonicity.\n");
    I = - 1;
    X = BMinusU2;
    Y = Radix;
    Z = Radix + Radix * U2;
    NotMonot = False;
    Monot = False;
    while ( ! (NotMonot || Monot)) {
        I = I + 1;
        X = SQRT(X);
        Q = SQRT(Y);
        Z = SQRT(Z);
        if ((X > Q) || (Q > Z)) NotMonot = True;
        else {
            Q = ftype_floor(Q + Half);
            if (!(I > 0 || Radix == Q * Q)) Monot = True;
            else if (I > 0) {
            if (I > 1) Monot = True;
            else {
                Y = Y * BInvrse;
                X = Y - U1;
                Z = Y + U1;
                }
            }
            else {
                Y = Q;
                X = Y - U2;
                Z = Y + U2;
                }
            }
        }
    if (Monot) printf("sqrt has passed a test for Monotonicity.\n");
    else {
        BadCond(Defect, "");
        printf("sqrt(X) is non-monotonic for X near %.7e .\n", Y);
        }
    /*=============================================*/
    /*SPLIT
    }
#include "paranoia.h"
void part5(VOID){
*/
    Milestone = 80;
    /*=============================================*/
    MinSqEr = MinSqEr + Half;
    MaxSqEr = MaxSqEr - Half;
    Y = (SQRT(One + U2) - One) / U2;
    SqEr = (Y - One) + U2 / Eight;
    if (SqEr > MaxSqEr) MaxSqEr = SqEr;
    SqEr = Y + U2 / Eight;
    if (SqEr < MinSqEr) MinSqEr = SqEr;
    Y = ((SQRT(F9) - U2) - (One - U2)) / U1;
    SqEr = Y + U1 / Eight;
    if (SqEr > MaxSqEr) MaxSqEr = SqEr;
    SqEr = (Y + One) + U1 / Eight;
    if (SqEr < MinSqEr) MinSqEr = SqEr;
    OneUlp = U2;
    X = OneUlp;
    for( Indx = 1; Indx <= 3; ++Indx) {
        Y = SQRT((X + U1 + X) + F9);
        Y = ((Y - U2) - ((One - U2) + X)) / OneUlp;
        Z = ((U1 - X) + F9) * Half * X * X / OneUlp;
        SqEr = (Y + Half) + Z;
        if (SqEr < MinSqEr) MinSqEr = SqEr;
        SqEr = (Y - Half) + Z;
        if (SqEr > MaxSqEr) MaxSqEr = SqEr;
        if (((Indx == 1) || (Indx == 3))) 
            X = OneUlp * Sign (X) * ftype_floor(Eight / (Nine * SQRT(OneUlp)));
        else {
            OneUlp = U1;
            X = - OneUlp;
            }
        }
    /*=============================================*/
    Milestone = 85;
    /*=============================================*/
    SqRWrng = False;
    Anomaly = False;
    RSqrt = Other; /* ~dgh */
    if (Radix != One) {
        printf("Testing whether sqrt is rounded or chopped.\n");
        D = ftype_floor(Half + POW(Radix, One + Precision - ftype_floor(Precision)));
    /* ... == Radix^(1 + fract) if (Precision == Integer + fract. */
        X = D / Radix;
        Y = D / A1;
        if ((X != ftype_floor(X)) || (Y != ftype_floor(Y))) {
            Anomaly = True;
            }
        else {
            X = Zero;
            Z2 = X;
            Y = One;
            Y2 = Y;
            Z1 = Radix - One;
            FourD = Four * D;
            do  {
                if (Y2 > Z2) {
                    Q = Radix;
                    Y1 = Y;
                    do  {
                        X1 = ftype_abs(Q + ftype_floor(Half - Q / Y1) * Y1);
                        Q = Y1;
                        Y1 = X1;
                        } while ( ! (X1 <= Zero));
                    if (Q <= One) {
                        Z2 = Y2;
                        Z = Y;
                        }
                    }
                Y = Y + Two;
                X = X + Eight;
                Y2 = Y2 + X;
                if (Y2 >= FourD) Y2 = Y2 - FourD;
                } while ( ! (Y >= D));
            X8 = FourD - Z2;
            Q = (X8 + Z * Z) / FourD;
            X8 = X8 / Eight;
            if (Q != ftype_floor(Q)) Anomaly = True;
            else {
                Break = False;
                do  {
                    X = Z1 * Z;
                    X = X - ftype_floor(X / Radix) * Radix;
                    if (X == One) 
                        Break = True;
                    else
                        Z1 = Z1 - One;
                    } while ( ! (Break || (Z1 <= Zero)));
                if ((Z1 <= Zero) && (! Break)) Anomaly = True;
                else {
                    if (Z1 > RadixD2) Z1 = Z1 - Radix;
                    do  {
                        NewD();
                        } while ( ! (U2 * D >= F9));
                    if (D * Radix - D != W - D) Anomaly = True;
                    else {
                        Z2 = D;
                        I = 0;
                        Y = D + (One + Z) * Half;
                        X = D + Z + Q;
                        SR3750();
                        Y = D + (One - Z) * Half + D;
                        X = D - Z + D;
                        X = X + Q + X;
                        SR3750();
                        NewD();
                        if (D - Z2 != W - Z2) Anomaly = True;
                        else {
                            Y = (D - Z2) + (Z2 + (One - Z) * Half);
                            X = (D - Z2) + (Z2 - Z + Q);
                            SR3750();
                            Y = (One + Z) * Half;
                            X = Q;
                            SR3750();
                            if (I == 0) Anomaly = True;
                            }
                        }
                    }
                }
            }
        if ((I == 0) || Anomaly) {
            BadCond(Failure, "Anomalous arithmetic with Integer < ");
            printf("Radix^Precision = %.7e\n", W);
            printf(" fails test whether sqrt rounds or chops.\n");
            SqRWrng = True;
            }
        }
    if (! Anomaly) {
        if (! ((MinSqEr < Zero) || (MaxSqEr > Zero))) {
            RSqrt = Rounded;
            printf("Square root appears to be correctly rounded.\n");
            }
        else  {
            if ((MaxSqEr + U2 > U2 - Half) || (MinSqEr > Half)
                || (MinSqEr + Radix < Half)) SqRWrng = True;
            else {
                RSqrt = Chopped;
                printf("Square root appears to be chopped.\n");
                }
            }
        }
    if (SqRWrng) {
        printf("Square root is neither chopped nor correctly rounded.\n");
        printf("Observed errors run from %.7e ", MinSqEr - Half);
        printf("to %.7e ulps.\n", Half + MaxSqEr);
        TstCond (Serious, MaxSqEr - MinSqEr < Radix * Radix,
            "sqrt gets too many last digits wrong");
        }
    /*=============================================*/
    Milestone = 90;
    /*=============================================*/
    Pause();
    printf("Testing powers Z^i for small Integers Z and i.\n");
    N = 0;
    /* ... test powers of zero. */
    I = 0;
    Z = -Zero;
    M = 3;
    Break = False;
    do  {
        X = One;
        SR3980();
        if (I <= 10) {
            I = 1023;
            SR3980();
            }
        if (Z == MinusOne) Break = True;
        else {
            Z = MinusOne;
            /* .. if(-1)^N is invalid, replace MinusOne by One. */
            I = - 4;
            }
        } while ( ! Break);
    PrintIfNPositive();
    N1 = N;
    N = 0;
    Z = A1;
    M = (int)ftype_floor(Two * LOG(W) / LOG(A1));
    Break = False;
    do  {
        X = Z;
        I = 1;
        SR3980();
        if (Z == AInvrse) Break = True;
        else Z = AInvrse;
        } while ( ! (Break));
    /*=============================================*/
        Milestone = 100;
    /*=============================================*/
    /*  Powers of Radix have been tested, */
    /*         next try a few primes     */
    M = NoTrials;
    Z = Three;
    do  {
        X = Z;
        I = 1;
        SR3980();
        do  {
            Z = Z + Two;
            } while ( Three * ftype_floor(Z / Three) == Z );
        } while ( Z < Eight * Three );
    if (N > 0) {
        printf("Errors like this may invalidate financial calculations\n");
        printf("\tinvolving interest rates.\n");
        }
    PrintIfNPositive();
    N += N1;
    if (N == 0) printf("... no discrepancies found.\n");
    if (N > 0) Pause();
    else printf("\n");
    /*=============================================*/
    /*SPLIT
    }
#include "paranoia.h"
void part6(VOID){
*/
    Milestone = 110;
    /*=============================================*/
    printf("Seeking Underflow thresholds UfThold and E0.\n");
    D = U1;
    if (Precision != ftype_floor(Precision)) {
        D = BInvrse;
        X = Precision;
        do  {
            D = D * BInvrse;
            X = X - One;
            } while ( X > Zero);
        }
    Y = One;
    Z = D;
    /* ... D is power of 1/Radix < 1. */
    do  {
        C = Y;
        Y = Z;
        Z = Y * Y;
        } while ((Y > Z) && (Z + Z > Z));
    Y = C;
    Z = Y * D;
    do  {
        C = Y;
        Y = Z;
        Z = Y * D;
        } while ((Y > Z) && (Z + Z > Z));
    if (Radix < Two) HInvrse = Two;
    else HInvrse = Radix;
    H = One / HInvrse;
    /* ... 1/HInvrse == H == Min(1/Radix, 1/2) */
    CInvrse = One / C;
    E0 = C;
    Z = E0 * H;
    /* ...1/Radix^(BIG Integer) << 1 << CInvrse == 1/C */
    do  {
        Y = E0;
        E0 = Z;
        Z = E0 * H;
        } while ((E0 > Z) && (Z + Z > Z));
    UfThold = E0;
    E1 = Zero;
    Q = Zero;
    E9 = U2;
    S = One + E9;
    D = C * S;
    if (D <= C) {
        E9 = Radix * U2;
        S = One + E9;
        D = C * S;
        if (D <= C) {
            BadCond(Failure, "multiplication gets too many last digits wrong.\n");
            Underflow = E0;
            Y1 = Zero;
            PseudoZero = Z;
            Pause();
            }
        }
    else {
        Underflow = D;
        PseudoZero = Underflow * H;
        UfThold = Zero;
        do  {
            Y1 = Underflow;
            Underflow = PseudoZero;
            if (E1 + E1 <= E1) {
                Y2 = Underflow * HInvrse;
                E1 = ftype_abs(Y1 - Y2);
                Q = Y1;
                if ((UfThold == Zero) && (Y1 != Y2)) UfThold = Y1;
                }
            PseudoZero = PseudoZero * H;
            } while ((Underflow > PseudoZero)
                && (PseudoZero + PseudoZero > PseudoZero));
        }
    /* Comment line 4530 .. 4560 */
    if (PseudoZero != Zero) {
        printf("\n");
        Z = PseudoZero;
    /* ... Test PseudoZero for "phoney- zero" violates */
    /* ... PseudoZero < Underflow or PseudoZero < PseudoZero + PseudoZero
           ... */
        if (PseudoZero <= Zero) {
            BadCond(Failure, "Positive expressions can underflow to an\n");
            printf("allegedly negative value\n");
            printf("PseudoZero that prints out as: %g .\n", PseudoZero);
            X = - PseudoZero;
            if (X <= Zero) {
                printf("But -PseudoZero, which should be\n");
                printf("positive, isn't; it prints out as  %g .\n", X);
                }
            }
        else {
            BadCond(Flaw, "Underflow can stick at an allegedly positive\n");
            printf("value PseudoZero that prints out as %g .\n", PseudoZero);
            }
        TstPtUf();
        }
    /*=============================================*/
    Milestone = 120;
    /*=============================================*/
    if (CInvrse * Y > CInvrse * Y1) {
        S = H * S;
        E0 = Underflow;
        }
    if (! ((E1 == Zero) || (E1 == E0))) {
        BadCond(Defect, "");
        if (E1 < E0) {
            printf("Products underflow at a higher");
            printf(" threshold than differences.\n");
            if (PseudoZero == Zero) 
            E0 = E1;
            }
        else {
            printf("Difference underflows at a higher");
            printf(" threshold than products.\n");
            }
        }
    printf("Smallest strictly positive number found is E0 = %g .\n", E0);
    Z = E0;
    TstPtUf();
    Underflow = E0;
    if (N == 1) Underflow = Y;
    I = 4;
    if (E1 == Zero) I = 3;
    if (UfThold == Zero) I = I - 2;
    UfNGrad = True;
    switch (I)  {
        case    1:
        UfThold = Underflow;
        if ((CInvrse * Q) != ((CInvrse * Y) * S)) {
            UfThold = Y;
            BadCond(Failure, "Either accuracy deteriorates as numbers\n");
            printf("approach a threshold = %.17e\n", UfThold);;
            printf(" coming down from %.17e\n", C);
            printf(" or else multiplication gets too many last digits wrong.\n");
            }
        Pause();
        break;
    
        case    2:
        BadCond(Failure, "Underflow confuses Comparison, which alleges that\n");
        printf("Q == Y while denying that |Q - Y| == 0; these values\n");
        printf("print out as Q = %.17e, Y = %.17e .\n", Q, Y2);
        printf ("|Q - Y| = %.17e .\n" , ftype_abs(Q - Y2));
        UfThold = Q;
        break;
    
        case    3:
        X = X;
        break;
    
        case    4:
        if ((Q == UfThold) && (E1 == E0)
            && (ftype_abs( UfThold - E1 / E9) <= E1)) {
            UfNGrad = False;
            printf("Underflow is gradual; it incurs Absolute Error =\n");
            printf("(roundoff in UfThold) < E0.\n");
            Y = E0 * CInvrse;
            Y = Y * (OneAndHalf + U2);
            X = CInvrse * (One + U2);
            Y = Y / X;
            IEEE = (Y == E0);
            }
        }
    if (UfNGrad) {
        printf("\n");
        sigsave = sigfpe;
        if (setjmp(ovfl_buf)) {
            printf("Underflow / UfThold failed!\n");
            R = H + H;
            }
        else R = SQRT(Underflow / UfThold);
        sigsave = 0;
        if (R <= H) {
            Z = R * UfThold;
            X = Z * (One + R * H * (One + H));
            }
        else {
            Z = UfThold;
            X = Z * (One + H * H * (One + H));
            }
        if (! ((X == Z) || (X - Z != Zero))) {
            BadCond(Flaw, "");
            printf("X = %.17e\n\tis not equal to Z = %.17e .\n", X, Z);
            Z9 = X - Z;
            printf("yet X - Z yields %.17e .\n", Z9);
            printf("    Should this NOT signal Underflow, ");
            printf("this is a SERIOUS DEFECT\nthat causes ");
            printf("confusion when innocent statements like\n");;
            printf("    if (X == Z)  ...  else");
            printf("  ... (f(X) - f(Z)) / (X - Z) ...\n");
            printf("encounter Division by Zero although actually\n");
            sigsave = sigfpe;
            if (setjmp(ovfl_buf)) printf("X / Z fails!\n");
            else printf("X / Z = 1 + %g .\n", (X / Z - Half) - Half);
            sigsave = 0;
            }
        }
    printf("The Underflow threshold is %.17e, %s\n", UfThold,
           " below which");
    printf("calculation may suffer larger Relative error than ");
    printf("merely roundoff.\n");
    Y2 = U1 * U1;
    Y = Y2 * Y2;
    Y2 = Y * U1;
    if (Y2 <= UfThold) {
        if (Y > E0) {
            BadCond(Defect, "");
            I = 5;
            }
        else {
            BadCond(Serious, "");
            I = 4;
            }
        printf("Range is too narrow; U1^%d Underflows.\n", I);
        }
    /*=============================================*/
    /*SPLIT
    }
#include "paranoia.h"
void part7(VOID){
*/
    Milestone = 130;
    /*=============================================*/
    Y = - ftype_floor(Half - TwoForty * LOG(UfThold) / LOG(HInvrse)) / TwoForty;
    Y2 = Y + Y;
    printf("Since underflow occurs below the threshold\n");
    printf("UfThold = (%.17e) ^ (%.17e)\nonly underflow ", HInvrse, Y);
    printf("should afflict the expression\n\t(%.17e) ^ (%.17e);\n",
        HInvrse, Y2);
    printf("actually calculating yields:");
    if (setjmp(ovfl_buf)) {
        sigsave = 0;
        BadCond(Serious, "trap on underflow.\n");
        }
    else {
        sigsave = sigfpe;
        V9 = POW(HInvrse, Y2);
        sigsave = 0;
        printf(" %.17e .\n", V9);
        if (! ((V9 >= Zero) && (V9 <= (Radix + Radix + E9) * UfThold))) {
            BadCond(Serious, "this is not between 0 and underflow\n");
        printf("   threshold = %.17e .\n", UfThold);
        }
        else if (! (V9 > UfThold * (One + E9)))
            printf("This computed value is O.K.\n");
        else {
            BadCond(Defect, "this is not between 0 and underflow\n");
            printf("   threshold = %.17e .\n", UfThold);
            }
        }
    /*=============================================*/
    Milestone = 140;
    /*=============================================*/
    printf("\n");
    /* ...calculate Exp2 == exp(2) == 7.389056099... */
    X = Zero;
    I = 2;
    Y = Two * Three;
    Q = Zero;
    N = 0;
    do  {
        Z = X;
        I = I + 1;
        Y = Y / (I + I);
        R = Y + Q;
        X = Z + R;
        Q = (Z - X) + R;
        } while(X > Z);
    Z = (OneAndHalf + One / Eight) + X / (OneAndHalf * ThirtyTwo);
    X = Z * Z;
    Exp2 = X * X;
    X = F9;
    Y = X - U1;
    printf("Testing X^((X + 1) / (X - 1)) vs. exp(2) = %.17e as X -> 1.\n",
        Exp2);
    for(I = 1;;) {
        Z = X - BInvrse;
        Z = (X + One) / (Z - (One - BInvrse));
        Q = POW(X, Z) - Exp2;
        if (ftype_abs(Q) > TwoForty * U2) {
            N = 1;
             V9 = (X - BInvrse) - (One - BInvrse);
            BadCond(Defect, "Calculated");
            printf(" %.17e for\n", POW(X,Z));
            printf("\t(1 + (%.17e) ^ (%.17e);\n", V9, Z);
            printf("\tdiffers from correct value by %.17e .\n", Q);
            printf("\tThis much error may spoil financial\n");
            printf("\tcalculations involving tiny interest rates.\n");
            break;
            }
        else {
            Z = (Y - X) * Two + Y;
            X = Y;
            Y = Z;
            Z = One + (X - F9)*(X - F9);
            if (Z > One && I < NoTrials) I++;
            else  {
                if (X > One) {
                    if (N == 0)
                       printf("Accuracy seems adequate.\n");
                    break;
                    }
                else {
                    X = One + U2;
                    Y = U2 + U2;
                    Y += X;
                    I = 1;
                    }
                }
            }
        }
    /*=============================================*/
    Milestone = 150;
    /*=============================================*/
    printf("Testing powers Z^Q at four nearly extreme values.\n");
    N = 0;
    Z = A1;
    Q = ftype_floor(Half - LOG(C) / LOG(A1));
    Break = False;
    do  {
        X = CInvrse;
        Y = POW(Z, Q);
        IsYeqX();
        Q = - Q;
        X = C;
        Y = POW(Z, Q);
        IsYeqX();
        if (Z < One) Break = True;
        else Z = AInvrse;
        } while ( ! (Break));
    PrintIfNPositive();
    if (N == 0) printf(" ... no discrepancies found.\n");
    printf("\n");
    
    /*=============================================*/
    Milestone = 160;
    /*=============================================*/
    Pause();
    printf("Searching for Overflow threshold:\n");
    printf("This may generate an error.\n");
    Y = - CInvrse;
    V9 = HInvrse * Y;
    sigsave = sigfpe;
    if (setjmp(ovfl_buf)) { I = 0; V9 = Y; goto overflow; }
    do {
        V = Y;
        Y = V9;
        V9 = HInvrse * Y;
        } while(V9 < Y);
    I = 1;
overflow:
    sigsave = 0;
    Z = V9;
    printf("Can `Z = -Y' overflow?\n");
    printf("Trying it on Y = %.17e .\n", Y);
    V9 = - Y;
    V0 = V9;
    if (V - Y == V + V0) printf("Seems O.K.\n");
    else {
        printf("finds a ");
        BadCond(Flaw, "-(-Y) differs from Y.\n");
        }
    if (Z != Y) {
        BadCond(Serious, "");
        printf("overflow past %.17e\n\tshrinks to %.17e .\n", Y, Z);
        }
    if (I) {
        Y = V * (HInvrse * U2 - HInvrse);
        Z = Y + ((One - HInvrse) * U2) * V;
        if (Z < V0) Y = Z;
        if (Y < V0) V = Y;
        if (V0 - V < V0) V = V0;
        }
    else {
        V = Y * (HInvrse * U2 - HInvrse);
        V = V + ((One - HInvrse) * U2) * Y;
        }
    printf("Overflow threshold is V  = %.17e .\n", V);
    if (I) printf("Overflow saturates at V0 = %.17e .\n", V0);
    else printf("There is no saturation value because \
the system traps on overflow.\n");
    V9 = V * One;
    printf("No Overflow should be signaled for V * 1 = %.17e\n", V9);
    V9 = V / One;
    printf("                           nor for V / 1 = %.17e .\n", V9);
    printf("Any overflow signal separating this * from the one\n");
    printf("above is a DEFECT.\n");
    /*=============================================*/
    Milestone = 170;
    /*=============================================*/
    if (!(-V < V && -V0 < V0 && -UfThold < V && UfThold < V)) {
        BadCond(Failure, "Comparisons involving ");
        printf("+-%g, +-%g\nand +-%g are confused by Overflow.",
            V, V0, UfThold);
        }
    /*=============================================*/
    Milestone = 175;
    /*=============================================*/
    printf("\n");
    for(Indx = 1; Indx <= 3; ++Indx) {
        switch (Indx)  {
            case 1: Z = UfThold; break;
            case 2: Z = E0; break;
            case 3: Z = PseudoZero; break;
            }
        if (Z != Zero) {
            V9 = SQRT(Z);
            Y = V9 * V9;
            if (Y / (One - Radix * E9) < Z
               || Y > (One + Radix * E9) * Z) { /* dgh: + E9 --> * E9 */
                if (V9 > U1) BadCond(Serious, "");
                else BadCond(Defect, "");
                printf("Comparison alleges that what prints as Z = %.17e\n", Z);
                printf(" is too far from sqrt(Z) ^ 2 = %.17e .\n", Y);
                }
            }
        }
    /*=============================================*/
    Milestone = 180;
    /*=============================================*/
    for(Indx = 1; Indx <= 2; ++Indx) {
        if (Indx == 1) Z = V;
        else Z = V0;
        V9 = SQRT(Z);
        X = (One - Radix * E9) * V9;
        V9 = V9 * X;
        if (((V9 < (One - Two * Radix * E9) * Z) || (V9 > Z))) {
            Y = V9;
            if (X < W) BadCond(Serious, "");
            else BadCond(Defect, "");
            printf("Comparison alleges that Z = %17e\n", Z);
            printf(" is too far from sqrt(Z) ^ 2 (%.17e) .\n", Y);
            }
        }
    /*=============================================*/
    /*SPLIT
    }
#include "paranoia.h"
int part8(VOID){
*/
    Milestone = 190;
    /*=============================================*/
    Pause();
    X = UfThold * V;
    Y = Radix * Radix;
    if (X*Y < One || X > Y) {
        if (X * Y < U1 || X > Y/U1) BadCond(Defect, "Badly");
        else BadCond(Flaw, "");
            
        printf(" unbalanced range; UfThold * V = %.17e\n\t%s\n",
            X, "is too far from 1.\n");
        }
    /*=============================================*/
    Milestone = 200;
    /*=============================================*/
    for (Indx = 1; Indx <= 5; ++Indx)  {
        X = F9;
        switch (Indx)  {
            case 2: X = One + U2; break;
            case 3: X = V; break;
            case 4: X = UfThold; break;
            case 5: X = Radix;
            }
        Y = X;
        sigsave = sigfpe;
        if (setjmp(ovfl_buf))
            printf("  X / X  traps when X = %g\n", X);
        else {
            V9 = (Y / X - Half) - Half;
            if (V9 == Zero) continue;
            if (V9 == - U1 && Indx < 5) BadCond(Flaw, "");
            else BadCond(Serious, "");
            printf("  X / X differs from 1 when X = %.17e\n", X);
            printf("  instead, X / X - 1/2 - 1/2 = %.17e .\n", V9);
            }
        sigsave = 0;
        }
    /*=============================================*/
    Milestone = 210;
    /*=============================================*/
    MyZero = Zero;
    printf("\n");
    printf("What message and/or values does Division by Zero produce?\n") ;
#ifndef NOPAUSE
    printf("This can interupt your program.  You can ");
    printf("skip this part if you wish.\n");
    printf("Do you wish to compute 1 / 0? ");
    fflush(stdout);
    read (KEYBOARD, ch, 8);
    if ((ch[0] == 'Y') || (ch[0] == 'y')) {
#endif
        sigsave = sigfpe;
        printf("    Trying to compute 1 / 0 produces ...");
        if (!setjmp(ovfl_buf)) printf("  %.7e .\n", One / MyZero);
        sigsave = 0;
#ifndef NOPAUSE
        }
    else printf("O.K.\n");
    printf("\nDo you wish to compute 0 / 0? ");
    fflush(stdout);
    read (KEYBOARD, ch, 80);
    if ((ch[0] == 'Y') || (ch[0] == 'y')) {
#endif
        sigsave = sigfpe;
        printf("\n    Trying to compute 0 / 0 produces ...");
        if (!setjmp(ovfl_buf)) printf("  %.7e .\n", Zero / MyZero);
        sigsave = 0;
#ifndef NOPAUSE
        }
    else printf("O.K.\n");
#endif
    /*=============================================*/
    Milestone = 220;
    /*=============================================*/
    Pause();
    printf("\n");
    {
        static char *msg[] = {
            "FAILUREs  encountered =",
            "SERIOUS DEFECTs  discovered =",
            "DEFECTs  discovered =",
            "FLAWs  discovered =" };
        int i;
        for(i = 0; i < 4; i++) if (ErrCnt[i])
            printf("The number of  %-29s %d.\n",
                msg[i], ErrCnt[i]);
        }
    printf("\n");
    if ((ErrCnt[Failure] + ErrCnt[Serious] + ErrCnt[Defect]
            + ErrCnt[Flaw]) > 0) {
        if ((ErrCnt[Failure] + ErrCnt[Serious] + ErrCnt[
            Defect] == 0) && (ErrCnt[Flaw] > 0)) {
            printf("The arithmetic diagnosed seems ");
            printf("Satisfactory though flawed.\n");
            }
        if ((ErrCnt[Failure] + ErrCnt[Serious] == 0)
            && ( ErrCnt[Defect] > 0)) {
            printf("The arithmetic diagnosed may be Acceptable\n");
            printf("despite inconvenient Defects.\n");
            }
        if ((ErrCnt[Failure] + ErrCnt[Serious]) > 0) {
            printf("The arithmetic diagnosed has ");
            printf("unacceptable Serious Defects.\n");
            }
        if (ErrCnt[Failure] > 0) {
            printf("Potentially fatal FAILURE may have spoiled this");
            printf(" program's subsequent diagnoses.\n");
            }
        }
    else {
        printf("No failures, defects nor flaws have been discovered.\n");
        if (! ((RMult == Rounded) && (RDiv == Rounded)
            && (RAddSub == Rounded) && (RSqrt == Rounded))) 
            printf("The arithmetic diagnosed seems Satisfactory.\n");
        else {
            if (StickyBit >= One &&
                (Radix - Two) * (Radix - Nine - One) == Zero) {
                printf("Rounding appears to conform to ");
                printf("the proposed IEEE standard P");
                if ((Radix == Two) &&
                     ((Precision - Four * Three * Two) *
                      ( Precision - TwentySeven -
                       TwentySeven + One) == Zero)) 
                    printf("754");
                else printf("854");
                if (IEEE) printf(".\n");
                else {
                    printf(",\nexcept for possibly Double Rounding");
                    printf(" during Gradual Underflow.\n");
                    }
                }
            printf("The arithmetic diagnosed appears to be Excellent!\n");
            }
        }
    if (fpecount)
        printf("\nA total of %d floating point exceptions were registered.\n",
            fpecount);
    printf("END OF TEST.\n");
    return 0;
    }

/*SPLIT subs.c
#include "paranoia.h"
*/

 FLOAT
Sign (FP X)
#ifdef KR_headers
FLOAT X;
#endif
{ return X >= 0. ? 1.0 : -1.0; }

 void
Pause(VOID)
{
#ifndef NOPAUSE
    char ch[8];

    printf("\nTo continue, press RETURN");
    fflush(stdout);
    read(KEYBOARD, ch, 8);
#endif
    printf("\nDiagnosis resumes after milestone Number %d", Milestone);
    printf("          Page: %d\n\n", PageNo);
    ++Milestone;
    ++PageNo;
    }

 void
TstCond (INT K, INT Valid, CHARP T)
#ifdef KR_headers
int K, Valid;
char *T;
#endif
{ if (! Valid) { BadCond(K,T); printf(".\n"); } }

 void
BadCond(INT K, CHARP T)
#ifdef KR_headers
int K;
char *T;
#endif
{
    static char *msg[] = { "FAILURE", "SERIOUS DEFECT", "DEFECT", "FLAW" };

    ErrCnt [K] = ErrCnt [K] + 1;
    printf("%s:  %s", msg[K], T);
    }


 FLOAT
Random(VOID)
/*  Random computes
     X = (Random1 + Random9)^5
     Random1 = X - ftype_floor(X) + 0.000005 * X;
   and returns the new value of Random1
*/
{
    FLOAT X, Y;
    
    X = Random1 + Random9;
    Y = X * X;
    Y = Y * Y;
    X = X * Y;
    Y = X - ftype_floor(X);
    Random1 = Y + X * 0.000005;
    return(Random1);
    }

 void
SqXMinX (INT ErrKind)
#ifdef KR_headers
int ErrKind;
#endif
{
    FLOAT XA, XB;
    
    XB = X * BInvrse;
    XA = X - XB;
    SqEr = ((SQRT(X * X) - XB) - XA) / OneUlp;
    if (SqEr != Zero) {
        if (SqEr < MinSqEr) MinSqEr = SqEr;
        if (SqEr > MaxSqEr) MaxSqEr = SqEr;
        J = J + 1.0;
        BadCond(ErrKind, "\n");
        printf("sqrt( %.17e) - %.17e  = %.17e\n", X * X, X, OneUlp * SqEr);
        printf("\tinstead of correct value 0 .\n");
        }
    }

 void
NewD(VOID)
{
    X = Z1 * Q;
    X = ftype_floor(Half - X / Radix) * Radix + X;
    Q = (Q - X * Z) / Radix + X * X * (D / Radix);
    Z = Z - Two * X * D;
    if (Z <= Zero) {
        Z = - Z;
        Z1 = - Z1;
        }
    D = Radix * D;
    }

 void
SR3750(VOID)
{
    if (! ((X - Radix < Z2 - Radix) || (X - Z2 > W - Z2))) {
        I = I + 1;
        X2 = SQRT(X * D);
        Y2 = (X2 - Z2) - (Y - Z2);
        X2 = X8 / (Y - Half);
        X2 = X2 - Half * X2 * X2;
        SqEr = (Y2 + Half) + (Half - X2);
        if (SqEr < MinSqEr) MinSqEr = SqEr;
        SqEr = Y2 - X2;
        if (SqEr > MaxSqEr) MaxSqEr = SqEr;
        }
    }

 void
IsYeqX(VOID)
{
    if (Y != X) {
        if (N <= 0) {
            if (Z == Zero && Q <= Zero)
                printf("WARNING:  computing\n");
            else BadCond(Defect, "computing\n");
            printf("\t(%.17e) ^ (%.17e)\n", Z, Q);
            printf("\tyielded %.17e;\n", Y);
            printf("\twhich compared unequal to correct %.17e ;\n",
                X);
            printf("\t\tthey differ by %.17e .\n", Y - X);
            }
        N = N + 1; /* ... count discrepancies. */
        }
    }

 void
SR3980(VOID)
{
    do {
        Q = (FLOAT) I;
        Y = POW(Z, Q);
        IsYeqX();
        if (++I > M) break;
        X = Z * X;
        } while ( X < W );
    }

 void
PrintIfNPositive(VOID)
{
    if (N > 0) printf("Similar discrepancies have occurred %d times.\n", N);
    }

 void
TstPtUf(VOID)
{
    N = 0;
    if (Z != Zero) {
        printf("Since comparison denies Z = 0, evaluating ");
        printf("(Z + Z) / Z should be safe.\n");
        sigsave = sigfpe;
        if (setjmp(ovfl_buf)) goto very_serious;
        Q9 = (Z + Z) / Z;
        printf("What the machine gets for (Z + Z) / Z is  %.17e .\n",
            Q9);
        if (ftype_abs(Q9 - Two) < Radix * U2) {
            printf("This is O.K., provided Over/Underflow");
            printf(" has NOT just been signaled.\n");
            }
        else {
            if ((Q9 < One) || (Q9 > Two)) {
very_serious:
                N = 1;
                ErrCnt [Serious] = ErrCnt [Serious] + 1;
                printf("This is a VERY SERIOUS DEFECT!\n");
                }
            else {
                N = 1;
                ErrCnt [Defect] = ErrCnt [Defect] + 1;
                printf("This is a DEFECT!\n");
                }
            }
        sigsave = 0;
        V9 = Z * One;
        Random1 = V9;
        V9 = One * Z;
        Random2 = V9;
        V9 = Z / One;
        if ((Z == Random1) && (Z == Random2) && (Z == V9)) {
            if (N > 0) Pause();
            }
        else {
            N = 1;
            BadCond(Defect, "What prints as Z = ");
            printf("%.17e\n\tcompares different from  ", Z);
            if (Z != Random1) printf("Z * 1 = %.17e ", Random1);
            if (! ((Z == Random2)
                || (Random2 == Random1)))
                printf("1 * Z == %g\n", Random2);
            if (! (Z == V9)) printf("Z / 1 = %.17e\n", V9);
            if (Random2 != Random1) {
                ErrCnt [Defect] = ErrCnt [Defect] + 1;
                BadCond(Defect, "Multiplication does not commute!\n");
                printf("\tComparison alleges that 1 * Z = %.17e\n",
                    Random2);
                printf("\tdiffers from Z * 1 = %.17e\n", Random1);
                }
            Pause();
            }
        }
    }

 void
notify(CHARP s)
#ifdef KR_headers
 char *s;
#endif
{
    printf("%s test appears to be inconsistent...\n", s);
    printf("   PLEASE NOTIFY KARPINKSI!\n");
    }

/*SPLIT msgs.c
#include "paranoia.h"
*/

 void
msglist(CHARPP s)
#ifdef KR_headers
char **s;
#endif
{ while(*s) printf("%s\n", *s++); }

 void
Instructions(VOID)
{
  static char *instr[] = {
    "Lest this program stop prematurely, i.e. before displaying\n",
    "    `END OF TEST',\n",
    "try to persuade the computer NOT to terminate execution when an",
    "error like Over/Underflow or Division by Zero occurs, but rather",
    "to persevere with a surrogate value after, perhaps, displaying some",
    "warning.  If persuasion avails naught, don't despair but run this",
    "program anyway to see how many milestones it passes, and then",
    "amend it to make further progress.\n",
    "Answer questions with Y, y, N or n (unless otherwise indicated).\n",
    0};

    msglist(instr);
    }

 void
Heading(VOID)
{
  static char *head[] = {
    "Users are invited to help debug and augment this program so it will",
    "cope with unanticipated and newly uncovered arithmetic pathologies.\n",
    "Please send suggestions and interesting results to",
    "\tRichard Karpinski",
    "\tComputer Center U-76",
    "\tUniversity of California",
    "\tSan Francisco, CA 94143-0704, USA\n",
    "In doing so, please include the following information:",
#ifdef Single
    "\tPrecision:\tsingle;",
#else
    "\tPrecision:\tdouble;",
#endif
    "\tVersion:\t10 February 1989;",
    "\tComputer:\n",
    "\tCompiler:\n",
    "\tOptimization level:\n",
    "\tOther relevant compiler options:",
    0};

    msglist(head);
    }

 void
Characteristics(VOID)
{
    static char *chars[] = {
     "Running this program should reveal these characteristics:",
    "     Radix = 1, 2, 4, 8, 10, 16, 100, 256 ...",
    "     Precision = number of significant digits carried.",
    "     U2 = Radix/Radix^Precision = One Ulp",
    "\t(OneUlpnit in the Last Place) of 1.000xxx .",
    "     U1 = 1/Radix^Precision = One Ulp of numbers a little less than 1.0 .",
    "     Adequacy of guard digits for Mult., Div. and Subt.",
    "     Whether arithmetic is chopped, correctly rounded, or something else",
    "\tfor Mult., Div., Add/Subt. and Sqrt.",
    "     Whether a Sticky Bit used correctly for rounding.",
    "     UnderflowThreshold = an underflow threshold.",
    "     E0 and PseudoZero tell whether underflow is abrupt, gradual, or fuzzy.",
    "     V = an overflow threshold, roughly.",
    "     V0  tells, roughly, whether  Infinity  is represented.",
    "     Comparisions are checked for consistency with subtraction",
    "\tand for contamination with pseudo-zeros.",
    "     Sqrt is tested.  Y^X is not tested.",
    "     Extra-precise subexpressions are revealed but NOT YET tested.",
    "     Decimal-Binary conversion is NOT YET tested for accuracy.",
    0};

    msglist(chars);
    }

 void
History(VOID)
{ /* History */
 /* Converted from Brian Wichmann's Pascal version to C by Thos Sumner,
    with further massaging by David M. Gay. */

  static char *hist[] = {
    "The program attempts to discriminate among",
    "   FLAWs, like lack of a sticky bit,",
    "   Serious DEFECTs, like lack of a guard digit, and",
    "   FAILUREs, like 2+2 == 5 .",
    "Failures may confound subsequent diagnoses.\n",
    "The diagnostic capabilities of this program go beyond an earlier",
    "program called `MACHAR', which can be found at the end of the",
    "book  `Software Manual for the Elementary Functions' (1980) by",
    "W. J. Cody and W. Waite. Although both programs try to discover",
    "the Radix, Precision and range (over/underflow thresholds)",
    "of the arithmetic, this program tries to cope with a wider variety",
    "of pathologies, and to say how well the arithmetic is implemented.",
    "\nThe program is based upon a conventional radix representation for",
    "floating-point numbers, but also allows logarithmic encoding",
    "as used by certain early WANG machines.\n",
    "BASIC version of this program (C) 1983 by Prof. W. M. Kahan;",
    "see source comments for more history.",
    0};

    msglist(hist);
    }
#endif

int 
main(void)
{
    int total_failure_count = 0;

    total_failure_count += small_integer_tests();
    total_failure_count += small_prime_integer_tests();
    total_failure_count += radix_and_precision_tests();

    return (0);
}

