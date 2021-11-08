#pragma once

#include <stdlib.h>
#include <kernel/queue.h>
#include <kernel/pkt/debug.h>
#include <kernel/pkt/data_buffer.h>

/**
 * Fragment header
 *
 */
struct pfrag {
    DECLARE_MAGIC      (pf_magic);
    struct data_buffer *pf_data;
    size_t              pf_head;
    size_t              pf_tail;
    union {
        /* Use either a double linked list or a single linked list */
        TAILQ_ENTRY(pfrag) pf_queue;
        struct {
            struct pfrag  *pf_next;
            void          *pf_unused;
        };
    };
};

TAILQ_HEAD(fragq, pfrag);


/**
 * Should be removed. All iterations should be done via struct packet
 */
static inline struct pfrag*
pfrag_next(const struct pfrag *pf)
{
    return TAILQ_NEXT(pf, pf_queue);
}

static inline struct pfrag *
pfrag_prev(struct pfrag *pf)
{
    return TAILQ_PREV(pf, fragq, pf_queue);
}


static inline size_t
pfrag_size(struct pfrag *pf)
{
    return pf->pf_tail - pf->pf_head;
}

static inline void*
pfrag_head(struct pfrag *pf)
{
    return (uint8_t *)data_buffer_data(pf->pf_data) + pf->pf_head;
}

static inline void*
pfrag_tail(struct pfrag *pf)
{
    return (uint8_t *)data_buffer_data(pf->pf_data) + pf->pf_tail;
}

static inline size_t
pfrag_headroom(struct pfrag *pf)
{
    return pf->pf_head;
}

static inline size_t
pfrag_tailroom(struct pfrag *pf)
{
    return data_buffer_size(pf->pf_data) - pf->pf_tail;
}

static inline void*
pfrag_insert_head(struct pfrag *pf, size_t size)
{
    //assert(pfrag_headroom(pf) >= size);

    pf->pf_head -= size;

    return pfrag_head(pf);
}

static inline void*
pfrag_remove_head(struct pfrag *pf, size_t size)
{
    //assert(pfrag_size(pf) >= size);

    pf->pf_head += size;

    return pfrag_head(pf);
}

static inline void*
pfrag_insert_tail(struct pfrag *pf, size_t size)
{
    void *retval;
    //assert(pfrag_tailroom(pf) >= size);

    retval = pfrag_tail(pf);
    pf->pf_tail += size;

    return retval;
}

static inline void*
pfrag_remove_tail(struct pfrag *pf, size_t size)
{
    //assert(pfrag_size(pf) >= size);

    pf->pf_tail -= size;

    return pfrag_tail(pf);
}

static inline void*
pfrag_reserve(struct pfrag *pf, size_t size)
{
    //assert(pfrag_size(pf) == 0);
    pfrag_insert_tail(pf, size);
    pfrag_remove_head(pf, size);
    return pfrag_head(pf);
}

/* BEGIN GENERATED PROTOS */

struct pfrag* pfrag_alloc(size_t size);

struct pfrag* pfrag_alloc_default_size(void);

struct pfrag* pfrag_alloc_fixme(size_t size);

struct pfrag* pfrag_clone(const struct pfrag* pf);

struct pfrag* pfrag_encapsulate(void* data, size_t size);

void pfrag_free(struct pfrag* pf);

off_t pfrag_move_data(struct pfrag* pf, off_t offset);

/* END GENERATED PROTOS */
