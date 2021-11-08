/**
 * @addtogroup memory
 * @{
 */
#pragma once
#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 256

typedef uintptr_t page_t;

static inline size_t os_getpagesize(void)
{
    return PAGE_SIZE;
}

size_t os_avail_pages(void);

void* os_page_alloc(unsigned int npages, unsigned int align);

void* os_page_balloc(size_t* size);

void os_page_claim(const void* start, const void* end);

void os_page_free(const void* address, unsigned int npages);

unsigned int os_page_maxalloc(unsigned int align);

/** @} */
