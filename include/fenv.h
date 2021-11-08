#pragma once

#include <assert.h>

#define FE_ALL_EXCEPT 0
#define FE_TONEAREST  0

typedef unsigned int fexcept_t;

typedef unsigned int fenv_t;

#define FE_DFL_ENV      ((const fenv_t *) -1)

/* BEGIN GENERATED PROTOS */

int feclearexcept(int excepts);

int fegetenv(fenv_t* envp);

int fegetexceptflag(fexcept_t* flagp, int excepts);

int fegetround(void);

int feholdexcept(fenv_t* envp);

int feraiseexcept(int excepts);

int fesetenv(const fenv_t* envp);

int fesetexceptflag(const fexcept_t* flagp, int excepts);

int fesetround(int round);

int fetestexcept(int excepts);

int feupdateenv(const fenv_t* envp);

/* END GENERATED PROTOS */
