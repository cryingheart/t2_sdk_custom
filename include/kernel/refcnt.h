#pragma once

#include <stdbool.h>
#include <kernel/atomic.h>

struct refcnt {
    atomic_t ref_count;
};

static inline void
refcnt_init(struct refcnt *ref)
{
    atomic_set(&ref->ref_count, 1);
}

static inline atomic_t
refcnt_inc(struct refcnt *ref)
{
    return atomic_add(&ref->ref_count, 1);
}

static inline void
refcnt_dec(struct refcnt *ref, void (*release)(struct refcnt*))
{
    if(atomic_sub(&ref->ref_count, 1) == 0)
        (*release)(ref);
}

static inline bool
refcnt_shared(const struct refcnt *ref)
{
    return atomic_read(&ref->ref_count) > 1;
}
