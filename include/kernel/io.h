#pragma once
#include <stdint.h>

#define barrier() asm volatile("" ::: "cc", "memory")


static inline uint64_t mmio_rd64(volatile uint32_t *addr)
{
    uint64_t val = *(volatile uint64_t *)addr;
    barrier();
    return val;
}

static inline void mmio_wr32(uint32_t val, volatile uint32_t *addr)
{
    *addr = val;
    barrier();
}

static inline uint32_t mmio_rd32(volatile uint32_t *addr)
{
    uint32_t val = *addr;
    barrier();
    return val;
}

static inline void mmio_wr32_set(uint32_t val, volatile uint32_t *addr)
{
    *addr |= val;
    barrier();
}

static inline void mmio_wr32_clr(uint32_t val, volatile uint32_t *addr)
{
    *addr &= ~val;
    barrier();
}


static inline void mmio_wr16(uint16_t val, volatile uint16_t *addr)
{
    *addr = val;
    barrier();
}

static inline void mmio_wr16_set(uint16_t val, volatile uint16_t *addr)
{
    *addr |= val;
    barrier();
}

static inline void mmio_wr16_clr(uint16_t val, volatile uint16_t *addr)
{
    *addr &= ~val;
    barrier();
}

static inline uint16_t mmio_rd16(volatile uint16_t *addr)
{
    uint16_t val = *addr;
    barrier();
    return val;
}

static inline void mmio_wr8(uint8_t val, volatile uint8_t *addr)
{
    *addr = val;
    barrier();
}

static inline void mmio_wr8_set(uint8_t val, volatile uint8_t *addr)
{
    *addr |= val;
    barrier();
}

static inline void mmio_wr8_clr(uint8_t val, volatile uint8_t *addr)
{
    *addr &= ~val;
    barrier();
}

static inline uint8_t mmio_rd8(volatile uint8_t *addr)
{
    uint8_t val = *addr;
    barrier();
    return val;
}

/* for use with special registers marked with set-clear; note that the
 * register is 16 bits wide but upper and lower 8 are treated
 * separately */
static inline void mmio_wr8_setclr(uint8_t set, uint8_t clr, volatile uint16_t *addr)
{
    *addr = set | (clr << 8);
    barrier();
}
