#pragma once

#define DRAGONFLY_EXCESSIVE_DEBUG 0

#ifdef __linux__
#define df_debug(fmt, ...)                                              \
    df_printf("%s:%u " fmt "\n", __func__, __LINE__, ## __VA_ARGS__)
#else
#define df_debug(fmt, ...)                                              \
    pr_debug("%s:%u " fmt "\n", __func__, __LINE__, ## __VA_ARGS__)
#endif

#include <dragonfly/df_mpint.h>

#include <dragonfly/df-cpp-begin.h>

/* BEGIN GENERATED PROTOS */

void
df_debug_hexdump(const char* prefix,
                 const void* data,
                 size_t len);

enum dragonfly_status
df_debug_mpint(const char* label,
               const df_mpint* value);

enum dragonfly_status
df_debug_point(const char* label,
               const df_ec_point* point);

/* END GENERATED PROTOS */

#include <dragonfly/df-cpp-end.h>
