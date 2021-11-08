#pragma once
#include <stdint.h>
#include <stddef.h>

struct cipher_buffer {
    uint8_t *ptr;
    size_t size;
};

#define CIPHER_BUFFER_INITIALIZER { .ptr = NULL, .size = 0 }

#define DECLARE_CB(name) \
    struct cipher_buffer name __cleanup(cipher_buffer_free) = CIPHER_BUFFER_INITIALIZER

/* BEGIN GENERATED PROTOS */

void cipher_buffer_init(struct cipher_buffer* cb);

int cipher_buffer_alloc(struct cipher_buffer* cb, size_t size);

void cipher_buffer_free(struct cipher_buffer* cb);

int
cipher_buffer_from_memory(struct cipher_buffer* cb,
                          const void* ptr,
                          size_t size);

int
cipher_buffer_copy(struct cipher_buffer* dst,
                   const struct cipher_buffer* src);

void
cipher_buffer_move(struct cipher_buffer* dst,
                   struct cipher_buffer* src);

int
cipher_buffer_cmp(const struct cipher_buffer* a,
                  const struct cipher_buffer* b)
    __attribute__((__nonnull__ (1, 2)));

/* END GENERATED PROTOS */
