/*
  Name:     imath.c
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

#include "imath.h"

#if DEBUG
#include <stdio.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <assert.h>

#ifdef __innos__
#include <kernel/os.h>

#define malloc  os_alloc
#define free    os_free
#define realloc os_realloc

#undef isdigit
#define isdigit(ch) ((ch) >= '0' && (ch) <= '9')
#undef isalpha
#define isalpha(ch) (((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z'))
#undef toupper
#define toupper(ch) (((ch) >= 'a' && (ch) <= 'z') ? (ch) ^ 0x20 : (ch))
#undef isspace
#define isspace(ch) ((ch) == ' ' || ((ch) >= '\t' && (ch) <= '\r'))

#include "hw_yoda.h"
#endif /* __innos__ */

extern const char * const s_unknown_err;
extern const char * const s_error_msg[];

/* Argument checking macros
   Use CHECK() where a return value is required; NRCHECK() elsewhere */
#define CHECK(TEST)   assert(TEST)
#define NRCHECK(TEST) assert(TEST)

extern const double s_log2[];

/* Return the number of digits needed to represent a static value */
#define MP_VALUE_DIGITS(V) \
((sizeof(V)+(sizeof(mp_digit)-1))/sizeof(mp_digit))

/* Round precision P to nearest word boundary */
#define ROUND_PREC(P) ((mp_size)(2*(((P)+1)/2)))

/* Set array P of S digits to zero */
#define ZERO(P, S) \
do{ \
  mp_size i__ = (S) * sizeof(mp_digit); \
  mp_digit *p__ = (P); \
  memset(p__, 0, i__); \
} while(0)

/* Copy S digits from array P to array Q */
#define COPY(P, Q, S) \
do{ \
  mp_size i__ = (S) * sizeof(mp_digit); \
  mp_digit *p__ = (P), *q__ = (Q); \
  memcpy(q__, p__, i__); \
} while(0)

/* Reverse N elements of type T in array A */
#define REV(T, A, N) \
do{ \
  T *u_ = (A), *v_ = u_ + (N) - 1; \
  while (u_ < v_) { \
    T xch = *u_; \
    *u_++ = *v_; \
    *v_-- = xch; \
  } \
} while(0)

#define CLAMP(Z) \
do{ \
  mp_int z_ = (Z); \
  mp_size uz_ = MP_USED(z_); \
  mp_digit *dz_ = MP_DIGITS(z_) + uz_ -1; \
  while (uz_ > 1 && (*dz_-- == 0)) \
    --uz_; \
  MP_USED(z_) = uz_; \
} while(0)

/* Select min/max.  Do not provide expressions for which multiple
   evaluation would be problematic, e.g. x++ */
#define MIN(A, B) ((B)<(A)?(B):(A))
#define MAX(A, B) ((B)>(A)?(B):(A))

/* Exchange lvalues A and B of type T, e.g.
   SWAP(int, x, y) where x and y are variables of type int. */
#define SWAP(T, A, B) \
do{ \
  T t_ = (A); \
  A = (B); \
  B = t_; \
} while(0)

/* Used to set up and access simple temp stacks within functions. */
#define DECLARE_TEMP(N) \
  mpz_t temp[(N)]; \
  int last__ = 0
#define CLEANUP_TEMP() \
 CLEANUP: \
  while (--last__ >= 0) \
    mp_int_clear(TEMP(last__))
#define TEMP(K) (temp + (K))
#define LAST_TEMP() TEMP(last__)
#define SETUP(E) \
do{ \
  if ((res = (E)) != MP_OK) \
    goto CLEANUP; \
  ++(last__); \
} while(0)

/* Compare value to zero. */
#define CMPZ(Z) \
(((Z)->used==1&&(Z)->digits[0]==0)?0:((Z)->sign==MP_NEG)?-1:1)

/* Multiply X by Y into Z, ignoring signs.  Requires that Z have
   enough storage preallocated to hold the result. */
#define UMUL(X, Y, Z) \
do{ \
  mp_size ua_ = MP_USED(X), ub_ = MP_USED(Y); \
  mp_size o_ = ua_ + ub_; \
  ZERO(MP_DIGITS(Z), o_); \
  (void) s_kmul(MP_DIGITS(X), MP_DIGITS(Y), MP_DIGITS(Z), ua_, ub_); \
  MP_USED(Z) = o_; \
  CLAMP(Z); \
} while(0)

/* Square X into Z.  Requires that Z have enough storage to hold the
   result. */
#define USQR(X, Z) \
do{ \
  mp_size ua_ = MP_USED(X), o_ = ua_ + ua_; \
  ZERO(MP_DIGITS(Z), o_); \
  (void) s_ksqr(MP_DIGITS(X), MP_DIGITS(Z), ua_); \
  MP_USED(Z) = o_; \
  CLAMP(Z); \
} while(0)

#define UPPER_HALF(W)           ((mp_word)((W) >> MP_DIGIT_BIT))
#define LOWER_HALF(W)           ((mp_digit)(W))
#define HIGH_BIT_SET(W)         ((W) >> (MP_WORD_BIT - 1))
#define ADD_WILL_OVERFLOW(W, V) ((MP_WORD_MAX - (V)) < (W))

/* Default number of digits allocated to a new mp_int */
extern const mp_size default_precision;

/* Minimum number of digits to invoke recursive multiply */
extern const mp_size multiply_threshold;

#include "imath-private.h"
