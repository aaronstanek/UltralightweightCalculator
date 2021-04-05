#pragma once

#include <float.h>

// we need to define ftype
// it should be the smallest floating point number
// type with at least 32 bits of precision
// and at least 7 bits of exponent
// specifically, we need the positive exponent
// to take on values at least as large as 32 (round to 40)
// StaticAssert.h will ensure that the radix is 2

#ifdef FLT_MANT_DIG
    #if (FLT_MANT_DIG >= 32) && (FLT_MIN_EXP <= -40) && (FLT_MAX_EXP >= 40)
        typedef float ftype;
        #define FTYPE_PRECISION FLT_MANT_DIG
        #define FTYPE_EXPONENT_MIN FLT_MIN_EXP
        #define FTYPE_EXPONENT_MAX FLT_MAX_EXP
    #endif
#endif

#ifndef FTYPE_PRECISION
    #ifdef DBL_MANT_DIG
        #if (DBL_MANT_DIG >= 32) && (DBL_MIN_EXP <= -40) && (DBL_MAX_EXP >= 40)
            typedef double ftype;
            #define FTYPE_PRECISION DBL_MANT_DIG
            #define FTYPE_EXPONENT_MIN DBL_MIN_EXP
            #define FTYPE_EXPONENT_MAX DBL_MAX_EXP
        #endif
    #endif
#endif

#ifndef FTYPE_PRECISION
    #ifdef LDBL_MANT_DIG
        #if (LDBL_MANT_DIG >= 32) && (LDBL_MIN_EXP <= -40) && (LDBL_MAX_EXP >= 40)
            typedef long double ftype;
            #define FTYPE_PRECISION LDBL_MANT_DIG
            #define FTYPE_EXPONENT_MIN LDBL_MIN_EXP
            #define FTYPE_EXPONENT_MAX LDBL_MAX_EXP
        #endif
    #endif
#endif

#ifndef FTYPE_PRECISION
    #error no suitable floating point type could be identified
#endif