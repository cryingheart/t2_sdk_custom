#pragma once
#include <stdint.h>
#include <stddef.h>
#include <kernel/os.h>

/*
 * [ptr]      [tail]            [head]
 *   |          |                 |
 *   v          v                 v
 *   +----------------------------------------------------------+
 *   | (free)  | (unread data)   |         (free space)         |
 *   +----------------------------------------------------------+
 *
 * [ptr]             [head]                 [tail]
 *   |                 |                      |
 *   v                 v                      v
 *   +----------------------------------------------------------+
 *   | (unread data)  |     (free space)     |  (unread data)   |
 *   +----------------------------------------------------------+
 *
 */
struct ring {
    uint8_t *ptr;
    uint16_t size;
    uint16_t mask;
    uint16_t head;              /* Index of next free position for put */
    uint16_t tail;              /* Index of next used position for get */
};

#define ring_idx(_ring, _name) \
    ((_ring)->_name & (_ring)->mask)

#define ring_ptr(_ring, _name) \
    (&(_ring)->ptr[ring_idx(_ring, _name)])

/**
 * Return number of bytes that can one can get from the ring
 */
static inline size_t
ring_avail(const struct ring *r)
{
    return r->head - r->tail;
}

/**
 * Return number of bytes that one can put into the ring
 */
static inline size_t
ring_space(const struct ring *r)
{
    return r->size - ring_avail(r);
}

int ring_init(struct ring *ring, void *mem, size_t size);
void ring_reset(struct ring *ring);
size_t ring_put(struct ring *ring, const void *msg, size_t len);
size_t ring_get(struct ring *ring, void *msg, size_t maxlen);
