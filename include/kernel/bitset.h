#pragma once

#ifndef NBBY
#define NBBY 8
#endif

/* Bitset related macros. */
#define setbit(a,i) (((unsigned char *)(a))[(i)/NBBY] |= 1<<((i)%NBBY))
#define clrbit(a,i) (((unsigned char *)(a))[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define isset(a,i)  (((const unsigned char *)(a))[(i)/NBBY] & (1<<((i)%NBBY)))
#define isclr(a,i)  ((((const unsigned char *)(a))[(i)/NBBY] & (1<<((i)%NBBY))) == 0)
