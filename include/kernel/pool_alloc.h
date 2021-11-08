/**
 * @addtogroup memory
 * @{
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <kernel/queue.h>

/**
 * Definition of a memory pool.
 */
struct memory_pool {
    size_t       mp_element_size;
    size_t       mp_block_size;
    uint32_t     mp_total;
    uint32_t     mp_alloc;
    TAILQ_HEAD(, pool_block) mp_blocklist;
    TAILQ_HEAD(, pool_entry) mp_freelist;
};

void pool_init(struct memory_pool *pool, size_t element_size);
void pool_destroy(struct memory_pool *pool);
void *pool_alloc(struct memory_pool *pool);
void pool_free(struct memory_pool *pool, void *ptr);

/** @} */
