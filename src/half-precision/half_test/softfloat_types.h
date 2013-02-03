#ifndef SOFTFLOAT_TYPES_H
#define SOFTFLOAT_TYPES_H

#include <stdint.h>

typedef float32 uint32_t;

/*----------------------------------------------------------------------------
| Each of the following `typedef's defines the most convenient type that holds
| integers of at least as many bits as specified.  For example, `uint8' should
| be the most convenient type that can hold unsigned integers of as many as
| 8 bits.  The `flag' type must be able to hold either a 0 or 1.  For most
| implementations of C, `flag', `uint8', and `int8' should all be `typedef'ed
| to the same as `int'.
*----------------------------------------------------------------------------*/
typedef uint32_t  flag;
typedef uint32_t  uint8;
typedef int32_t   int8;
typedef uint32_t  uint16;
typedef int32_t   int16;
typedef uint32_t  uint32;
typedef int32_t   int32;
typedef uint64_t  uint64;
typedef int64_t   int64;

/*----------------------------------------------------------------------------
| Each of the following `typedef's defines a type that holds integers
| of _exactly_ the number of bits specified.  For instance, for most
| implementation of C, `bits16' and `sbits16' should be `typedef'ed to
| `unsigned short int' and `signed short int' (or `short int'), respectively.
*----------------------------------------------------------------------------*/
typedef uint32_t  bits8;
typedef int32_t   sbits8;
typedef uint32_t  bits16;
typedef int32_t   sbits16;
typedef uint32_t  bits32;
typedef int32_t   sbits32;
typedef uint64_t  bits64;
typedef int64_t   sbits64;

#endif
