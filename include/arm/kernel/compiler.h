#pragma once

#ifndef __noinline
#define __noinline      __attribute__((__noinline__))
#endif
#ifdef __always_inline
#undef __always_inline
#endif
#define __always_inline inline __attribute__((__always_inline__))
#define inline          inline __attribute__((__always_inline__))
#define __pure          __attribute__((__pure__))
#define __aligned(x)    __attribute__((__aligned__(x)))
#define __printf(a, b)  __attribute__((__format__(__printf__, a, b)))
#define __naked         __attribute__((__naked__)) __noinline
#define __unused        __attribute__((__unused__))
#define __packed        __attribute__((__packed__))

#define __irq           __attribute__((__section__(".text.irq"))) __noinline
#define __irqro         __attribute__((__section__(".rodata.irq")))
#define __entrypoint    __attribute__((__section__(".text.startup")))
#define __notrace       __attribute__((__section__(".text.notrace")))

#define __cleanup(func_) __attribute__((__cleanup__(func_)))
