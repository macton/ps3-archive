#ifndef SOFT_HALF_H
#define SOFT_HALF_H

#include <stdint.h>

uint32_t half_to_float( uint16_t h );
uint16_t half_from_float( uint32_t f );
uint16_t half_add( uint16_t arg0, uint16_t arg1 );
uint16_t half_sub( uint16_t arg0, uint16_t arg1 );
uint16_t half_mul( uint16_t arg0, uint16_t arg1 );
uint16_t half_div( uint16_t arg0, uint16_t arg1 );

#endif /* SOFT_HALF_H */
