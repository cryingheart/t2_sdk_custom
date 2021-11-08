#pragma once

#include <stddef.h>
#include <kernel/refcnt.h>
#include <kernel/pkt/debug.h>

/**
 * Data buffer
 *
 */
struct data_buffer {
    DECLARE_MAGIC (db_magic);
    struct refcnt  db_ref;
    size_t         db_size;
    void          *db_data;
    unsigned char  db_buffer[0];
};

static inline size_t
data_buffer_size(const struct data_buffer *db)
{
    return db->db_size;
}

static inline void *
data_buffer_data(const struct data_buffer *db)
{
    return db->db_data;
}

static inline bool
data_buffer_shared(const struct data_buffer *db)
{
    return refcnt_shared(&db->db_ref);
}

/* BEGIN GENERATED PROTOS */

struct data_buffer* data_buffer_alloc(size_t size);

void data_buffer_decref(struct data_buffer* db);

struct data_buffer* data_buffer_encapsulate(void* data, size_t size);

struct data_buffer* data_buffer_incref(struct data_buffer* db);

void data_buffer_release(struct refcnt* ref);

/* END GENERATED PROTOS */
