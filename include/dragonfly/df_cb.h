#pragma once

#include <dragonfly/df-cpp-begin.h>

#include <kernel/cipher_buffer.h>
#include <dragonfly/df_mpint.h>

/* BEGIN GENERATED PROTOS */

enum dragonfly_status
cipher_buffer_from_mpi_size(struct cipher_buffer* cb,
                            const df_mpint* src,
                            size_t size);

enum dragonfly_status
cipher_buffer_from_mpi(struct cipher_buffer* cb,
                       const df_mpint* src);

enum dragonfly_status
cipher_buffer_from_point_size(struct cipher_buffer* cb,
                              const df_ec_point* src,
                              size_t size);

int cipher_buffer_lsb(const struct cipher_buffer* cb);

/* END GENERATED PROTOS */

#include <dragonfly/df-cpp-end.h>
