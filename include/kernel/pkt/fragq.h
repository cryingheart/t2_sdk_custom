#pragma once

#include <kernel/spinlock.h>
#include <kernel/pkt/debug.h>
#include <kernel/pkt/pfrag.h>

struct fragq_locked {
    struct spinlock    fq_lock;
    struct fragq       fq_frags;
};

static inline void
fragq_init(struct fragq *q)
{
    TAILQ_INIT(q);
}

static inline void
fragq_init_locked(struct fragq_locked *q)
{
    spin_lock_init(&q->fq_lock);
    fragq_init(&q->fq_frags);
}

static inline int
fragq_empty(const struct fragq *q)
{
    return TAILQ_EMPTY(q);
}

static inline struct pfrag *
fragq_first(const struct fragq *q)
{
    return TAILQ_FIRST(q);
}

static inline void
fragq_enqueue(struct fragq *q, struct pfrag *pf)
{
    TAILQ_INSERT_TAIL(q, pf, pf_queue);
}

static __always_inline void
_fragq_enqueue_locked_inline(struct fragq_locked *q, struct pfrag *pf)
{
    os_irqstate_t flags;

    flags = spin_lock_irqsave(&q->fq_lock);
    fragq_enqueue(&q->fq_frags, pf);
    spin_unlock_irqrestore(&q->fq_lock, flags);
}

static inline struct pfrag *
fragq_dequeue(struct fragq *q)
{
    struct pfrag *pf = fragq_first(q);
    if (pf)
        TAILQ_REMOVE(q, pf, pf_queue);
    return pf;
}

static __always_inline struct pfrag *
_fragq_dequeue_locked_inline(struct fragq_locked *q)
{
    os_irqstate_t flags;
    struct pfrag *pf;

    flags = spin_lock_irqsave(&q->fq_lock);
    pf = fragq_dequeue(&q->fq_frags);
    spin_unlock_irqrestore(&q->fq_lock, flags);

    return pf;
}

static inline void
fragq_insert_head(struct fragq *q, struct pfrag *pf)
{
    TAILQ_INSERT_HEAD(q, pf, pf_queue);
}

static inline struct pfrag *
fragq_remove_frag(struct fragq *q, struct pfrag *pf)
{
    TAILQ_REMOVE(q, pf, pf_queue);
    return pf;
}

static inline void
fragq_free_all_frags(struct fragq *q)
{
    while (!fragq_empty(q))
        pfrag_free(fragq_dequeue(q));
}

/* BEGIN GENERATED PROTOS */

struct pfrag* _fragq_dequeue_locked_extern(struct fragq_locked* q);

void
_fragq_enqueue_locked_extern(struct fragq_locked* q,
                             struct pfrag* pf);

/* END GENERATED PROTOS */

static __always_inline void
fragq_enqueue_locked(struct fragq_locked *q, struct pfrag *pf)
{
#ifdef __ARM_ARCH_6M__
    _fragq_enqueue_locked_inline(q, pf);
#else
    _fragq_enqueue_locked_extern(q, pf);
#endif
}

static __always_inline struct pfrag *
fragq_dequeue_locked(struct fragq_locked *q)
{
#ifdef __ARM_ARCH_6M__
    return _fragq_dequeue_locked_inline(q);
#else
    return _fragq_dequeue_locked_extern(q);
#endif
}
