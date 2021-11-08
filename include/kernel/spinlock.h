#pragma once
#include <kernel/atomic.h>
#include <kernel/cpu.h>

struct spinlock {
    atomic_t value;
};

static inline void
spin_lock_init(struct spinlock *lock)
{
    atomic_set(&lock->value, 0);
}

static inline void
spin_lock(struct spinlock *lock)
{
    do {
        while (atomic_load(&lock->value) != 0)
            ;
    } while (atomic_store(&lock->value, 1) != 0);
}

static inline int
spin_trylock(struct spinlock *lock)
{
    do {
        if (atomic_load(&lock->value) != 0)
            return 0;
    } while (atomic_store(&lock->value, 1) != 0);
    return 1;
}

static inline void
spin_unlock(struct spinlock *lock)
{
    atomic_set(&lock->value, 0);
}

static inline os_irqstate_t
spin_lock_irqsave(struct spinlock *lock)
{
    os_irqstate_t flags = cpu_irq_disable();
    spin_lock(lock);
    return flags;
}

static inline void
spin_unlock_irqrestore(struct spinlock *lock, os_irqstate_t flags)
{
    spin_unlock(lock);
    cpu_irq_restore(flags);
}
