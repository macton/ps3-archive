#include "unaligned_double.h"
#include "vsl.h"
#include <stdint.h>
#include <spu_intrinsics.h>

qword 
_ldwu( const char* const arg0 )
{
    uintptr_t  source_addr_u         = (uintptr_t)arg0;
    qword      source_addr           = si_from_uint( source_addr_u );

    // Unaligned load requires two reads since the 64 bit value may be
    // stored across two aligned quadwords. These loads will read
    // starting at the quadword aligned address of source_addr
    // i.e. will ignore the low 4 bits of source_addr

    qword      source_lo             = si_lqd( source_addr, 0x00 );
    qword      source_hi             = si_lqd( source_addr, 0x10 );

    // Get low 4 bits of source address. This is the offset from the
    // aligned address.

    qword      shift_offset          = si_andi( source_addr, 0x0f );

    // Create a shuffle pattern which selects the appropriate
    // bytes of the 64 bit value from the two quadwords.
    //
    // The value of shift offset is stored in byte[3] of shift_offset,
    // 1. Generate the pattern 0x03030303_03030303_03030303_03030303
    // 2. Use shuffle to generate a qword with each byte filled with 
    //    the shift offset.

    qword      lo_byte_pattern       = si_ilh( 0x0303 );
    qword      shift_offset_pattern  = si_shufb( shift_offset, shift_offset, lo_byte_pattern );

    // Add shift_offset_pattern to a vector for shift left (which is
    // just a byte vector with bytes incrementing from 0-15). This will
    // generate a shuffle pattern which will be used store the unaligned 
    // 64 bit value in the preferred slot of the result.

    qword      vector_shift_left     = (qword)_vsl;
    qword      shift_pattern         = si_a( shift_offset_pattern, vector_shift_left );

    // Move 64 bit value into preferred slot and extract.

    qword      result_preferred      = si_shufb( source_lo, source_hi, shift_pattern );

    return (result_preferred);
}

void 
_stdwu( char* const arg0, qword value )
{
    uintptr_t  source_addr_u         = (uintptr_t)arg0;
    qword      source_addr           = si_from_uint( source_addr_u );

    // Unaligned store requires two reads since the 64 bit value may be
    // stored across two aligned quadwords. The two source lines
    // will be loaded, modified then stored.

    qword      source_lo             = si_lqd( source_addr, 0x00 );
    qword      source_hi             = si_lqd( source_addr, 0x10 );

    // Get low 4 bits of source address. This is the offset from the
    // aligned address.

    qword      shift_offset          = si_andi( source_addr, 0x0f );

    // Create a shuffle pattern which selects the appropriate
    // bytes of the 64 bit value from the low quadword.
    //
    // The value of shift offset is stored in byte[3] of shift_offset,
    // 1. Generate the pattern 0x03030303_03030303_03030303_03030303
    // 2. Use shuffle to generate a qword with each byte filled with 
    //    the shift offset.

    qword      lo_byte_pattern          = si_ilh( 0x0303 );
    qword      shift_offset_pattern_lo  = si_shufb( shift_offset, shift_offset, lo_byte_pattern );


    // Create a shuffle pattern which selects the appropriate
    // bytes of the 64 bit value from the high quadword 
    // high offset = (16 bytes - low offset)

    qword      shift_adjust_hi          = si_ilh( 0x1010 );
    qword      shift_offset_pattern_hi  = si_sf( shift_adjust_hi, shift_offset_pattern_lo );

    // Subtract shift offset pattern from a vector for shift left (which is
    // just a byte vector with bytes incrementing from 0-15). This will
    // generate a shuffle pattern which will be used store the unaligned 
    // 64 bit value at the unaliged locations used in the first source line.

    qword      vector_shift_left     = (qword)_vsl;
    qword      shift_pattern_lo      = si_sf( shift_offset_pattern_lo, vector_shift_left );
    qword      shift_pattern_hi      = si_sf( shift_offset_pattern_hi, vector_shift_left );

    // Mask the bits that will be unmodified in the source lines.
    // Any shuffle pattern outside the range [0x00,0x07] is not being used
    // by the value, so that will be kept in the source lines.

    qword      source_bits_mask_lo   = si_clgtbi( shift_pattern_lo, 0x07 );
    qword      source_bits_mask_hi   = si_clgtbi( shift_pattern_hi, 0x07 );
    qword      value_lo              = si_shufb( value, value, shift_pattern_lo );
    qword      value_hi              = si_shufb( value, value, shift_pattern_hi );

    // Clear space in source lines to store the value

    qword      prepped_source_lo     = si_and( source_lo, source_bits_mask_lo );
    qword      prepped_source_hi     = si_and( source_hi, source_bits_mask_hi );

    // Clear everything unwanted from the value

    qword      prepped_value_lo     = si_andc( value_lo, source_bits_mask_lo );
    qword      prepped_value_hi     = si_andc( value_hi, source_bits_mask_hi );

    // Combine the source lines and the value lines

    qword      result_lo            = si_or( prepped_source_lo, prepped_value_lo );
    qword      result_hi            = si_or( prepped_source_hi, prepped_value_hi );

    // Store the result

    si_stqd( result_lo, source_addr, 0x00 );
    si_stqd( result_hi, source_addr, 0x10 );
}

