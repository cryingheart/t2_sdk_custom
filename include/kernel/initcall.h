#pragma once
#include <assert.h>
#include <stdint.h>

struct initcall {
    uint8_t  ic_type;
    uint8_t  ic_runlevel;
    void    *ic_arg;
    void   (*ic_func)(void *);
};

#define __INITCALL(_type, _lvl, _fn, _arg)                      \
    static_assert((_lvl) >= 0 && (_lvl) <= 31,                  \
                  "level must be in the range 0..31");          \
    const  __attribute__((__section__(".rodata.initcall")))     \
    struct initcall _initcall_ ## _fn = {                       \
        .ic_type = (_type),                                     \
        .ic_runlevel = (_lvl),                                  \
        .ic_arg = (_arg),                                       \
        .ic_func = (_fn)                                        \
    }

/* called at the end of os_kernel_init; almost everything is working
 * at this point, but there are still no threads running */
#define MODULE_INIT(_lvl, _fn, _arg)     __INITCALL('i', _lvl, _fn, _arg)

/* called just before starting the main application */
#define MODULE_START(_lvl, _fn, _arg)    __INITCALL('a', _lvl, _fn, _arg)

/* called before suspend; this is called in reversed order to
 * MODULE_RESUME (higher values are called before lower); keep
 * duration of callback constant */
#define MODULE_SUSPEND(_lvl, _fn, _arg)  __INITCALL('s', 31-(_lvl), _fn, _arg)

/* called just after resume; ; keep duration of callback constant */
#define MODULE_RESUME(_lvl, _fn, _arg)   __INITCALL('r', _lvl, _fn, _arg)

/* called just before a (possible) coredump is generated; it's risky
 * to assume anything about kernel state at this point */
#define MODULE_COREDUMP(_lvl, _fn, _arg) __INITCALL('c', _lvl, _fn, _arg)
