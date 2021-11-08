/*
  Name:     imath.h
  Purpose:  Arbitrary precision integer arithmetic routines.
  Author:   M. J. Fromberger <http://spinning-yarns.org/michael/>

  Copyright (C) 2002-2007 Michael J. Fromberger, All Rights Reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
 */

#ifndef IMATH_H_
#define IMATH_H_

#include <stdint.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char      mp_sign;
typedef unsigned int       mp_size;
typedef int                mp_result;
typedef long               mp_small;  /* must be a signed type */
typedef unsigned long      mp_usmall; /* must be an unsigned type */

/* Force building with uint64_t so that the library builds consistently
 * whether we build from the makefile or by embedding imath in another project.
 */
typedef uint16_t           mp_digit;
typedef uint32_t           mp_word;

typedef struct mpz {
  mp_digit    single;
  mp_digit   *digits;
  mp_size     alloc;
  mp_size     used;
  mp_sign     sign;
} mpz_t, *mp_int;

#define MP_DIGITS(Z) ((Z)->digits)
#define MP_ALLOC(Z)  ((Z)->alloc)
#define MP_USED(Z)   ((Z)->used)
#define MP_SIGN(Z)   ((Z)->sign)

enum {
    MP_OK     = 0,              /* no error, all is well  */
    MP_FALSE  = 0,
    MP_TRUE   = -1,
    MP_MEMORY = -2,             /* out of memory          */
    MP_RANGE  = -3,             /* argument out of range  */
    MP_UNDEF  = -4,             /* result undefined       */
    MP_TRUNC  = -5,             /* output truncated       */
    MP_BADARG = -6,             /* invalid null argument  */
    MP_MINERR = MP_BADARG
};

#define MP_DIGIT_BIT    (sizeof(mp_digit) * CHAR_BIT)
#define MP_WORD_BIT     (sizeof(mp_word) * CHAR_BIT)
#define MP_SMALL_MIN    LONG_MIN
#define MP_SMALL_MAX    LONG_MAX
#define MP_USMALL_MIN   ULONG_MIN
#define MP_USMALL_MAX   ULONG_MAX

#define MP_DIGIT_MAX   (UINT16_MAX * 1UL)
#define MP_WORD_MAX    (UINT32_MAX * 1UL)

#define MP_MIN_RADIX    2
#define MP_MAX_RADIX    36

/* Values with fewer than this many significant digits use the standard
   multiplication algorithm; otherwise, a recursive algorithm is used.  
   Choose a value to suit your platform.
 */
#define MP_MULT_THRESH  22

#define MP_DEFAULT_PREC 8   /* default memory allocation, in digits */

enum {
    MP_ZPOS,                    /* value is non-negative      */
    MP_NEG                      /* value is strictly negative */
};

#define mp_int_is_odd(Z)  ((Z)->digits[0] & 1)
#define mp_int_is_even(Z) !((Z)->digits[0] & 1)

#define   mp_int_mod_value(A, V, R) mp_int_div_value((A), (V), 0, (R))

#define   mp_int_sqrt(a, c) mp_int_root(a, 2, c)       /* c = floor(sqrt(a)) */

#include "imath-protos.h"

#ifdef __cplusplus
}
#endif
#endif /* end IMATH_H_ */
