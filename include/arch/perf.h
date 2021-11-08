/**--------------------------------------------------
 * Copyright (c) 2017, InnophaseInc
 * All rights reserved.
 *
 * @authors: agonga@InnophaseInc.com,
 *
 * @description: arch/perf.h lwip2x port file
 *--------------------------------------------------*/

#ifndef LWIP_ARCH_PERF_H
#define LWIP_ARCH_PERF_H

#include <sys/times.h>

#ifdef PERF
#define PERF_START  { \
unsigned long __c1l, __c1h, __c2l, __c2h; \
__asm__(".byte 0x0f, 0x31" : "=a" (__c1l), "=d" (__c1h))
#define PERF_STOP(x)   __asm__(".byte 0x0f, 0x31" : "=a" (__c2l), "=d" (__c2h)); \
perf_print(__c1l, __c1h, __c2l, __c2h, x);}

#else /* PERF */
#define PERF_START    /* null definition */
#define PERF_STOP(x)  /* null definition */
#endif /* PERF */

void perf_print(unsigned long c1l, unsigned long c1h,
                unsigned long c2l, unsigned long c2h,
                char *key);

void perf_print_times(struct tms *start, struct tms *end, char *key);

void perf_init(char *fname);

#endif /*LWIP_ARCH_PERF_H*/
