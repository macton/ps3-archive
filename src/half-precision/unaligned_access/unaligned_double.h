#ifndef UNALIGNED_DOUBLE_H
#define UNALIGNED_DOUBLE_H

#include <spu_intrinsics.h>
#include <stdint.h>

qword     _ldwu( const char* const arg0 );
void      _stdwu( char* const arg0, qword value );

// Return the double stored at the unaligned address at (arg0)
static inline double unaligned_load_double( const char* const arg0 )
{
    qword      result_preferred      = _ldwu( arg0 );
    double     result                = si_to_double( result_preferred );

    return (result);
}

// Write the double (arg1) at the unaligned address at (arg0)
static inline void unaligned_store_double( char* const arg0, double arg1 )
{
    qword      value                 = si_from_double( arg1 );

    _stdwu( arg0, value );
}

// Return the uint64 stored at the unaligned address at (arg0)
static inline uint64_t unaligned_load_uint64( const char* const arg0 )
{
    qword      result_preferred      = _ldwu( arg0 );
    uint64_t   result                = si_to_ullong( result_preferred );

    return (result);
}

// Write the uint64 (arg1) at the unaligned address at (arg0)
static inline void unaligned_store_uint64( char* const arg0, uint64_t arg1 )
{
    qword      value                 = si_from_ullong( arg1 );

    _stdwu( arg0, value );
}

#endif
