#pragma once
#include <stdint.h>
#include <kernel/atomic.h>
#include <kernel/cdefs.h>

static inline int
test_and_set_bit(int nr, uint32_t *p)
{
    uint32_t tmp;

    do {
        tmp = atomic_load((atomic_t *)p);
    } while (atomic_store((atomic_t *)p, tmp | BIT(nr)) != 0);

    return tmp & BIT(nr);
}

static inline int
test_and_clear_bit(int nr, uint32_t *p)
{
    uint32_t tmp;

    do {
        tmp = atomic_load((atomic_t *)p);
    } while (atomic_store((atomic_t *)p, tmp & ~BIT(nr)) != 0);

    return tmp & BIT(nr);
}

static inline int
test_bit(int nr, const uint32_t *p)
{
    return *p & BIT(nr);
}

static inline void
set_bit(int nr, uint32_t *p)
{
    (void) test_and_set_bit(nr, p);
}

static inline void
clear_bit(int nr, uint32_t *p)
{
    (void) test_and_clear_bit(nr, p);
}

static inline int
signext(uint32_t word, int bits)
{
    unsigned shift = 32 - bits;
    return ((int)((word & BITMASK(bits)) << shift)) >> shift;
}

static inline unsigned
bfx(uint32_t value, uint32_t mask)
{
    return (value & mask) >> __builtin_ctz(mask);
}

/* Insert low-order bits of value into field at bits specified by
 * mask. Return computed value.
 *
 * One bits of mask must be contiguous.
 */
static inline uint32_t
bfi2(uint32_t field, uint32_t value, uint32_t mask)
{
#ifdef __ARM_FEATURE_CLZ
    if(!__builtin_constant_p(value) && __builtin_constant_p(mask)) {
        uint32_t lsb = __builtin_ctz(mask);
        uint32_t width = __builtin_ctz(~(mask >> lsb));
        asm("bfi %0, %1, %2, %3" : "+r"(field) : "r"(value), "i"(lsb), "i"(width));
        return field;
    }
#endif
    return (field & ~mask) | ((value << __builtin_ctz(mask)) & mask);
}

static inline unsigned
bfi(uint32_t value, uint32_t mask)
{
    return bfi2(0, value, mask);
}
