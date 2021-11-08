#pragma once

#include <kernel/pkt/debug.h>
#include <kernel/pkt/fragq.h>

/**
 * Packet header
 *
 */
struct packet {
    DECLARE_MAGIC               (p_magic);
    struct fragq                 p_frags;
    union {
        /* Use either a double linked list or a single linked list */
        TAILQ_ENTRY(packet)          p_entry;
        struct {
            struct packet           *p_next;
            /* Marked as used in the single linked list case */
            bool                     p_used;
        };
    };
    void                        *p_info;
};

/* iterate over each fragment in packet */
#define FOREACH_PACKET_FRAGMENT(_PF, _PACKET) \
    TAILQ_FOREACH((_PF), &(_PACKET)->p_frags, pf_queue)

#define mtod(_pkt, _type) \
    ((_type) packet_data(_pkt))

static inline struct pfrag *
packet_first_frag(const struct packet *pkt)
{
    return TAILQ_FIRST(&pkt->p_frags);
}

static inline struct pfrag *
packet_last_frag(const struct packet *pkt)
{
    return TAILQ_LAST(&pkt->p_frags, fragq);
}

/*!
 * Return a pointer to the first byte of the first fragment of PKT.
 *
 * @warning The caller of this function must know how much data is
 * available at the head of the packet. This function might return a
 * pointer to what is effectively a zero-size buffer.
 */
static inline void *
packet_data(const struct packet *pkt)
{
    return pfrag_head(packet_first_frag(pkt));
}

static inline void
packet_add_frag(struct packet *pkt, struct pfrag *pf)
{
    TAILQ_INSERT_TAIL(&pkt->p_frags, pf, pf_queue);
}

static inline struct pfrag *
packet_remove_frag(struct packet *pkt, struct pfrag *pf)
{
    TAILQ_REMOVE(&pkt->p_frags, pf, pf_queue);
    return pf;
}

static inline size_t
packet_nfrag(const struct packet *pkt)
{
    struct pfrag *pf;
    size_t n = 0;

    FOREACH_PACKET_FRAGMENT(pf, pkt)
        ++n;

    return n;
}

static inline size_t
packet_size(const struct packet *pkt)
{
    struct pfrag *pf;
    size_t n = 0;

    FOREACH_PACKET_FRAGMENT(pf, pkt)
        n += pfrag_size(pf);

    return n;
}

/* fragq_{restore,locked_restore} live in the twilight zone between
 * fragq and packet, but for structural convenience, they are put in
 * this header file.
 */

/**
 * Move all pfrags from pkt to pfrag queue q
 */
static inline void
fragq_restore(struct fragq *q, struct packet *pkt)
{
    TAILQ_CONCAT(q, &pkt->p_frags, pf_queue);
}

/**
 * Move all pfrags from pkt to pfrag queue q
 */
static inline void
fragq_locked_restore(struct fragq_locked *q, struct packet *pkt)
{
    os_irqstate_t flags;

    flags = spin_lock_irqsave(&q->fq_lock);
    fragq_restore(&q->fq_frags, pkt);
    spin_unlock_irqrestore(&q->fq_lock, flags);
}

/* BEGIN GENERATED PROTOS */

struct packet* packet_alloc(size_t len);

int packet_append(struct packet* pkt, const void* data, size_t len);

void
packet_copydata(const struct packet* pkt,
                size_t offset,
                size_t len,
                void* dest);

struct packet* packet_dup(const struct packet* pkt);

size_t packet_empty(struct packet* pkt);

size_t packet_free(struct packet* pkt);

void packet_init(struct packet* pkt);

void* packet_insert_head(struct packet* pkt, size_t len);

void* packet_insert_tail(struct packet* pkt, size_t len);

void packet_join(struct packet* car, struct packet* cdr);

void packet_print(const char* tag, const struct packet* pkt);

void packet_trim_head(struct packet* pkt, size_t len);

void* packet_trim_tail(struct packet* pkt, size_t len);

/* END GENERATED PROTOS */
