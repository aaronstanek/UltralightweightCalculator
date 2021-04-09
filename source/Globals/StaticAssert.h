/**
 * @file StaticAssert.h
 * @author Aaron Stanek
 * @brief Compile-time checks to ensure
 * that the program will run correctly
 * on the target system
*/
#pragma once
#include "Globals.h"

// what is the radix of floating point types?
static_assert( FLT_RADIX == 2 , "floating point types must use radix 2" );
// if we don't use 2 as the radix, then determining which
// ints are able to fit in a floating point type gets trickier
// this will almost always be 2,
// so we might as well elimiate the edge
// cases where the radix is not 2

// for the time functions to work, we will
// need to represent large enough spans of time
// singed 15 bits will expire in less than a day (seconds)
// we should have at least 23 bits of unsigned time range
// (3 bytes for integral time_t)

#define MIN_TIME_BITS (0x7FFFFF)

static_assert( (unsigned long)( (time_t)(MIN_TIME_BITS) ) == (unsigned long)(MIN_TIME_BITS) , "time_t must hold at least 23 bits of precision" );

// if time_t is a too-small integer, then the left side will
// be truncated, causing the assert to fail
// if time_t is a too-small floating type, then the value in the one's
// place will be rounded, causing the assert to fail

// double is the result of difftime
// we will want this to be able to represent individual
// units accurately
// but we will never need to measure spans of time larger than 17 bits (seconds)
// 17 bits is about 36 hours, we only need 24

static_assert( DBL_MANT_DIG >= 17 , "double must hold at least 17 bits of precision" );

// this should hold true on pretty much every platform ever, but it can't hurt
// to be sure about this