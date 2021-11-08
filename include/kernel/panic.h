#pragma once
#include <stddef.h>

enum os_panic_type {
    PANIC_TYPE_CONSOLE = 0x736e6f63, /* "cons" */
    PANIC_TYPE_TRACE   = 0x65637274  /* "trce" */
};

/* this is a wrapper around the following functions, if you have
 * simple linear data, this is the function to use */
void os_panic_save(enum os_panic_type type, const void *data, size_t size);

void os_panic_save_first(enum os_panic_type type, size_t total_size);

void os_panic_save_next(const void *data, size_t size);

void os_panic_save_last(void);
