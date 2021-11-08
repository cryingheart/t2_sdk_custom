#pragma once

#include <bits/seek.h>

#include <kernel/pkt/packet.h>
#include <kernel/pkt/pfrag.h>

struct packet_iter {
    struct packet *pi_packet;
    off_t          pi_offset; /* offset (as in seek) into packet */
    struct pfrag  *pi_frag;   /* current fragment */
    unsigned int   pi_cursor; /* points to before an allocated byte in
                               * pi_frag or after the last allocated
                               * byte, if pi_frag is the last frag with data */
};

/*
 *  -----------
 * |XX|     |X|
 *  -----------
 *    ^  ^  ^
 *    |  |  \ pi_frag:tail
 *    |   \ pi_cursor
 *     \ pi_frag:head
 */

/* return a buffer pointer to where ITER is located */
static inline void* piter_data_pointer(struct packet_iter *iter)
{
    return pfrag_head(iter->pi_frag) + iter->pi_cursor;
}

/* return amount of linear space available at data pointer */
static inline size_t piter_data_length(struct packet_iter *iter)
{
    return pfrag_size(iter->pi_frag) - iter->pi_cursor;
}

/* BEGIN GENERATED PROTOS */

size_t
piter_copy(struct packet_iter* dst,
           struct packet_iter* src,
           size_t length);

size_t
piter_copy_from(void* dst,
                struct packet_iter* src,
                size_t length);

size_t
piter_copy_to(struct packet_iter* dst,
              const void* src,
              size_t length);

void piter_init(struct packet_iter* iter, struct packet* pkt);

int piter_insert(struct packet_iter* iter, size_t length)
    __attribute__ ((__warn_unused_result__));

size_t piter_linearize(struct packet_iter* pi, size_t amount)
    __attribute__((__warn_unused_result__));

void piter_remove_after(struct packet_iter* iter, size_t length);

void piter_remove_before(struct packet_iter* iter, size_t length);

off_t piter_seek(struct packet_iter* iter, off_t offset, int whence);

struct packet* piter_split_packet(struct packet_iter* pi);

/* END GENERATED PROTOS */
