#pragma once
#include <kernel/cpu.h>

#ifdef __ARM_ARCH_6M__
#define arch_v6_ext() asm(".arch\tarmv7-m")
#else
#define arch_v6_ext()
#endif

typedef int atomic_t;
typedef void *atomic_ptr_t;

static inline int
atomic_read(const atomic_t *addr)
{
   return *addr;
}

static inline void
atomic_set(atomic_t *addr, int value)
{
   *addr = value;
}

static inline int
atomic_load(atomic_t *addr)
{
    int retval;

    arch_v6_ext();
    asm("ldrex\t%[retval], [%[addr]]"
        : [retval] "=r"(retval)
        : [addr] "r"(addr), "m"(*addr));
    return retval;
}

static inline int
atomic_store(atomic_t *addr, int val)
{
    int flag;

    arch_v6_ext();
    asm("strex\t%[flag], %[value], [%[addr]]"
        : [flag] "=&r"(flag), "=m"(*addr)
        : [value] "r"(val), [addr] "r"(addr));
    return flag;
}

static inline int
atomic_add(atomic_t *addr, int val)
{
    int retval;

    do {
        retval = atomic_load(addr);
        retval += val;
    } while (atomic_store(addr, retval) != 0);

    return retval;
}

static inline int
atomic_sub(atomic_t *addr, int val)
{
    return atomic_add(addr, -val);
}

static inline unsigned int
atomic_tas(atomic_t *addr, unsigned int mask)
{
    unsigned int retval;

    do {
        retval = atomic_load(addr);
    } while (atomic_store(addr, retval | mask) != 0);

    return retval & mask;
}

static inline unsigned int
atomic_bis(atomic_t *addr, unsigned int mask)
{
    unsigned int retval;

    do {
        retval = atomic_load(addr);
        retval |= mask;
    } while (atomic_store(addr, retval) != 0);

    return retval;
}

static inline unsigned int
atomic_bic(atomic_t *addr, unsigned int mask)
{
    unsigned int retval;

    do {
        retval = atomic_load(addr);
        retval &= ~mask;
    } while (atomic_store(addr, retval) != 0);

    return retval;
}

static inline void *
atomic_ptr_load(const atomic_ptr_t *addr)
{
    void *retval;

    arch_v6_ext();
    asm("ldrex\t%[retval], [%[addr]]"
        : [retval] "=r"(retval)
        : [addr] "r"(addr), "m"(*addr));
    return retval;
}

static inline int
atomic_ptr_store(atomic_ptr_t *addr, void *ptr)
{
    int flag;

    arch_v6_ext();
    asm("strex\t%[flag], %[value], [%[addr]]"
        : [flag] "=&r"(flag), "=m"(*addr)
        : [value] "r"(ptr), [addr] "r"(addr));
    return flag;
}
