#pragma once

#include <kernel/spinlock.h>
#include <kernel/pkt/packet.h>

TAILQ_HEAD(pqhead,packet);

struct pqueue {
    struct pqhead       pq_head;
};

struct pqueue_locked {
    struct spinlock     pq_lock;
    struct pqueue       pq_queue;
};

static inline void
pqueue_init(struct pqueue *q)
{
    TAILQ_INIT(&q->pq_head);
}

static inline void
pqueue_init_locked(struct pqueue_locked *q)
{
    spin_lock_init(&q->pq_lock);
    pqueue_init(&q->pq_queue);
}

static inline int
pqueue_empty(const struct pqueue *q)
{
    return TAILQ_EMPTY(&q->pq_head);
}

static inline struct packet *
pqueue_peek(const struct pqueue *q)
{
    return TAILQ_FIRST(&q->pq_head);
}

static inline struct packet *
pqueue_locked_peek(const struct pqueue_locked *q)
{
    return pqueue_peek(&q->pq_queue);
}

static inline struct packet *
pqueue_last(const struct pqueue *q)
{
    return TAILQ_LAST(&q->pq_head, pqhead);
}

static inline void
pqueue_locked_concat_not_locked_lock(struct pqueue_locked *q1, struct pqueue *q2)
{
    os_irqstate_t flags = spin_lock_irqsave(&q1->pq_lock);
    TAILQ_CONCAT(&q1->pq_queue.pq_head, &q2->pq_head, p_entry);
    spin_unlock_irqrestore(&q1->pq_lock, flags);
}

static inline void
pqueue_locked_remove(struct pqueue_locked *q, struct packet *pkt)
{
    /* remove pkt (at any place in q) from q */
    os_irqstate_t flags = spin_lock_irqsave(&q->pq_lock);
    if (pkt)
        TAILQ_REMOVE(&q->pq_queue.pq_head, pkt, p_entry);
    spin_unlock_irqrestore(&q->pq_lock, flags);
}

static __always_inline void
_pqueue_enqueue_nolock(struct pqueue *q, struct packet *pkt)
{
    TAILQ_INSERT_TAIL(&q->pq_head, pkt, p_entry);
}

static __always_inline void
_pqueue_enqueue_inline(struct pqueue *q, struct packet *pkt)
{
    os_irqstate_t flags = cpu_irq_disable();
    _pqueue_enqueue_nolock(q, pkt);
    cpu_irq_restore(flags);
}

static __always_inline void
_pqueue_locked_enqueue_inline(struct pqueue_locked *q, struct packet *pkt)
{
    os_irqstate_t flags;

    flags = spin_lock_irqsave(&q->pq_lock);
    _pqueue_enqueue_nolock(&q->pq_queue, pkt);
    spin_unlock_irqrestore(&q->pq_lock, flags);
}

static __always_inline void
_pqueue_requeue_nolock(struct pqueue *q, struct packet *pkt)
{
    TAILQ_INSERT_HEAD(&q->pq_head, pkt, p_entry);
}

static __always_inline void
_pqueue_requeue_inline(struct pqueue *q, struct packet *pkt)
{
    os_irqstate_t flags = cpu_irq_disable();
    _pqueue_requeue_nolock(q, pkt);
    cpu_irq_restore(flags);
}

static inline void
_pqueue_locked_requeue_inline(struct pqueue_locked *q, struct packet *pkt)
{
    os_irqstate_t flags;

    flags = spin_lock_irqsave(&q->pq_lock);
    _pqueue_requeue_nolock(&q->pq_queue, pkt);
    spin_unlock_irqrestore(&q->pq_lock, flags);
}

static inline struct packet *
_pqueue_dequeue_nolock(struct pqueue *q)
{
    struct packet *pkt = pqueue_peek(q);
    if (pkt)
        TAILQ_REMOVE(&q->pq_head, pkt, p_entry);
    return pkt;
}

static inline struct packet *
_pqueue_dequeue_inline(struct pqueue *q)
{
    os_irqstate_t flags = cpu_irq_disable();
    struct packet *pkt = _pqueue_dequeue_nolock(q);
    cpu_irq_restore(flags);
    return pkt;
}

static inline struct packet *
_pqueue_locked_dequeue_inline(struct pqueue_locked *q)
{
    os_irqstate_t flags;
    struct packet *pkt;

    flags = spin_lock_irqsave(&q->pq_lock);
    pkt = _pqueue_dequeue_nolock(&q->pq_queue);
    spin_unlock_irqrestore(&q->pq_lock, flags);

    return pkt;
}

/* BEGIN GENERATED PROTOS */

struct packet* _pqueue_dequeue_extern(struct pqueue* q);

void _pqueue_enqueue_extern(struct pqueue* q, struct packet* pkt);

struct packet* _pqueue_locked_dequeue_extern(struct pqueue_locked* q);

void
_pqueue_locked_enqueue_extern(struct pqueue_locked* q,
                              struct packet* pkt);

void
_pqueue_locked_requeue_extern(struct pqueue_locked* q,
                              struct packet* pkt);

void _pqueue_requeue_extern(struct pqueue* q, struct packet* pkt);

/* END GENERATED PROTOS */

static __always_inline void
pqueue_enqueue(struct pqueue *q, struct packet *pkt)
{
#ifdef __ARM_ARCH_6M__
    _pqueue_enqueue_inline(q, pkt);
#else
    _pqueue_enqueue_extern(q, pkt);
#endif
}

static __always_inline void
pqueue_locked_enqueue(struct pqueue_locked *q, struct packet *pkt)
{
#ifdef __ARM_ARCH_6M__
    _pqueue_locked_enqueue_inline(q, pkt);
#else
    _pqueue_locked_enqueue_extern(q, pkt);
#endif
}

static __always_inline void
pqueue_requeue(struct pqueue *q, struct packet *pkt)
{
#ifdef __ARM_ARCH_6M__
    _pqueue_requeue_inline(q, pkt);
#else
    _pqueue_requeue_extern(q, pkt);
#endif
}

static __always_inline void
pqueue_locked_requeue(struct pqueue_locked *q, struct packet *pkt)
{
#ifdef __ARM_ARCH_6M__
    _pqueue_locked_requeue_inline(q, pkt);
#else
    _pqueue_locked_requeue_extern(q, pkt);
#endif
}

static __always_inline struct packet *
pqueue_dequeue(struct pqueue *q)
{
#ifdef __ARM_ARCH_6M__
    return _pqueue_dequeue_inline(q);
#else
    return _pqueue_dequeue_extern(q);
#endif
}

static __always_inline struct packet *
pqueue_locked_dequeue(struct pqueue_locked *q)
{
#ifdef __ARM_ARCH_6M__
    return _pqueue_locked_dequeue_inline(q);
#else
    return _pqueue_locked_dequeue_extern(q);
#endif
}
