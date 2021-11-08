#pragma once

#define complex _Complex
#define _Complex_I (0.0f+1.0fi)
#define I _Complex_I

#define creal(x)  __builtin_creal(x)
#define crealf(x) __builtin_crealf(x)
#define creall(x) __builtin_creall(x)

#define cimag(x)  __builtin_cimag(x)
#define cimagf(x) __builtin_cimagf(x)
#define cimagl(x) __builtin_cimagl(x)

#define __CMPLX(x, y, t) (__builtin_complex((t)(x), (t)(y)))
#define CMPLX(x, y) __CMPLX(x, y, double)
#define CMPLXF(x, y) __CMPLX(x, y, float)
#define CMPLXL(x, y) __CMPLX(x, y, long double)

/* BEGIN GENERATED PROTOS */

double cabs(double complex z)
    __attribute__((__warning__("cabs is not implemented")));

float cabsf(float complex z)
    __attribute__((__warning__("cabsf is not implemented")));

long double cabsl(long double complex z)
    __attribute__((__warning__("cabsl is not implemented")));

double complex cacos(double complex z)
    __attribute__((__warning__("cacos is not implemented")));

float complex cacosf(float complex z)
    __attribute__((__warning__("cacosf is not implemented")));

double complex cacosh(double complex z)
    __attribute__((__warning__("cacosh is not implemented")));

float complex cacoshf(float complex z)
    __attribute__((__warning__("cacoshf is not implemented")));

long double complex cacoshl(long double complex z)
    __attribute__((__warning__("cacoshl is not implemented")));

long double complex cacosl(long double complex z)
    __attribute__((__warning__("cacosl is not implemented")));

double carg(double complex z)
    __attribute__((__warning__("carg is not implemented")));

float cargf(float complex z)
    __attribute__((__warning__("cargf is not implemented")));

long double cargl(long double complex z)
    __attribute__((__warning__("cargl is not implemented")));

double complex casin(double complex z)
    __attribute__((__warning__("casin is not implemented")));

float complex casinf(float complex z)
    __attribute__((__warning__("casinf is not implemented")));

double complex casinh(double complex z)
    __attribute__((__warning__("casinh is not implemented")));

float complex casinhf(float complex z)
    __attribute__((__warning__("casinhf is not implemented")));

long double complex casinhl(long double complex z)
    __attribute__((__warning__("casinhl is not implemented")));

long double complex casinl(long double complex z)
    __attribute__((__warning__("casinl is not implemented")));

double complex catan(double complex z)
    __attribute__((__warning__("catan is not implemented")));

float complex catanf(float complex z)
    __attribute__((__warning__("catanf is not implemented")));

double complex catanh(double complex z)
    __attribute__((__warning__("catanh is not implemented")));

float complex catanhf(float complex z)
    __attribute__((__warning__("catanhf is not implemented")));

long double complex catanhl(long double complex z)
    __attribute__((__warning__("catanhl is not implemented")));

long double complex catanl(long double complex z)
    __attribute__((__warning__("catanl is not implemented")));

double complex ccos(double complex z)
    __attribute__((__warning__("ccos is not implemented")));

float complex ccosf(float complex z)
    __attribute__((__warning__("ccosf is not implemented")));

double complex ccosh(double complex z)
    __attribute__((__warning__("ccosh is not implemented")));

float complex ccoshf(float complex z)
    __attribute__((__warning__("ccoshf is not implemented")));

long double complex ccoshl(long double complex z)
    __attribute__((__warning__("ccoshl is not implemented")));

long double complex ccosl(long double complex z)
    __attribute__((__warning__("ccosl is not implemented")));

double complex cexp(double complex z)
    __attribute__((__warning__("cexp is not implemented")));

float complex cexpf(float complex z)
    __attribute__((__warning__("cexpf is not implemented")));

long double complex cexpl(long double complex z)
    __attribute__((__warning__("cexpl is not implemented")));

double complex clog(double complex z)
    __attribute__((__warning__("clog is not implemented")));

float complex clogf(float complex z)
    __attribute__((__warning__("clogf is not implemented")));

long double complex clogl(long double complex z)
    __attribute__((__warning__("clogl is not implemented")));

double complex conj(double complex z)
    __attribute__((__warning__("conj is not implemented")));

float complex conjf(float complex z)
    __attribute__((__warning__("conjf is not implemented")));

long double complex conjl(long double complex z)
    __attribute__((__warning__("conjl is not implemented")));

double complex cpow(double complex x, double complex y)
    __attribute__((__warning__("cpow is not implemented")));

float complex cpowf(float complex x, float complex y)
    __attribute__((__warning__("cpowf is not implemented")));

long double complex
cpowl(long double complex x,
      long double complex y)
    __attribute__((__warning__("cpowl is not implemented")));

double complex cproj(double complex z)
    __attribute__((__warning__("cproj is not implemented")));

float complex cprojf(float complex z)
    __attribute__((__warning__("cprojf is not implemented")));

long double complex cprojl(long double complex z)
    __attribute__((__warning__("cprojl is not implemented")));

double complex csin(double complex z)
    __attribute__((__warning__("csin is not implemented")));

float complex csinf(float complex z)
    __attribute__((__warning__("csinf is not implemented")));

double complex csinh(double complex z)
    __attribute__((__warning__("csinh is not implemented")));

float complex csinhf(float complex z)
    __attribute__((__warning__("csinhf is not implemented")));

long double complex csinhl(long double complex z)
    __attribute__((__warning__("csinhl is not implemented")));

long double complex csinl(long double complex z)
    __attribute__((__warning__("csinl is not implemented")));

double complex csqrt(double complex z)
    __attribute__((__warning__("csqrt is not implemented")));

float complex csqrtf(float complex z)
    __attribute__((__warning__("csqrtf is not implemented")));

long double complex csqrtl(long double complex z)
    __attribute__((__warning__("csqrtl is not implemented")));

double complex ctan(double complex z)
    __attribute__((__warning__("ctan is not implemented")));

float complex ctanf(float complex z)
    __attribute__((__warning__("ctanf is not implemented")));

double complex ctanh(double complex z)
    __attribute__((__warning__("ctanh is not implemented")));

float complex ctanhf(float complex z)
    __attribute__((__warning__("ctanhf is not implemented")));

long double complex ctanhl(long double complex z)
    __attribute__((__warning__("ctanhl is not implemented")));

long double complex ctanl(long double complex z)
    __attribute__((__warning__("ctanl is not implemented")));

/* END GENERATED PROTOS */
