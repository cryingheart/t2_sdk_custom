#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct theap;

/* BEGIN GENERATED PROTOS */

void*
theap_aligned_alloc(struct theap* h,
                    size_t alignment,
                    size_t size);

void* theap_alloc(struct theap* h, size_t size);

size_t theap_avail(struct theap* h);

bool theap_decref(void* ptr);

void theap_dump(struct theap* h);

void theap_extend(struct theap* h, void* heap_start, size_t heap_size);

double theap_external_fragmentation(struct theap* h);

void* theap_free(struct theap* h, void* ptr);

size_t theap_get_buffer_size(struct theap* h, void* ptr);

void theap_incref(void* ptr);

struct theap* theap_init(void* heap_start, size_t heap_size);

double theap_internal_fragmentation(struct theap* h);

void* theap_realloc(struct theap* h, void* ptr, size_t size);

/* END GENERATED PROTOS */

/* ------------- Debug functions ---------------- */

#ifdef THEAP_DISABLE_DEBUG
static inline bool
theap_is_debug(void)
{
    return false;
}
#else
extern bool theap_debug_activated;
static inline bool
theap_is_debug(void)
{
    return theap_debug_activated;
}
#endif

/* Must be called before theap_init */
void theap_debug_enable(void);

void tdbg_set_caller(void *ptr, uint32_t caller);
static inline void theap_debug_set_caller(void *ptr, uint32_t caller)
{
    if (theap_is_debug())
        tdbg_set_caller(ptr, caller);
}

void tdbg_check_freelist(struct theap *h);
