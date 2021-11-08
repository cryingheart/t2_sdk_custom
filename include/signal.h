#pragma once

#include <errno.h>

typedef volatile __SIG_ATOMIC_TYPE__ sig_atomic_t;

#define SIG_DFL  ((void(*)(int))2)
#define SIG_IGN  ((void(*)(int))4)
#define SIG_ERR  ((void(*)(int))6)

#define SIGABRT 1
#define SIGFPE  2
#define SIGILL  3
#define SIGINT  4
#define SIGSEGV 5
#define SIGTERM 6

/* BEGIN GENERATED PROTOS */

/* END GENERATED PROTOS */

#define __not_implemented(_name) \
    __attribute__((__warning__(#_name " is not implemented"))) _name

static inline void (*__not_implemented(signal)(int sig, void (*func)(int)))(int)
{
    errno = EINVAL;
    return SIG_ERR;
}

static inline int __not_implemented(raise)(int sig)
{
    return 1;
}
