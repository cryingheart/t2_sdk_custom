#include <unistd.h>
#pragma once

struct stringstream {
    char  *buf;
    size_t pos;
    size_t max;
};

void str_init(struct stringstream *s);
void str_free(struct stringstream *s);
char *str_string(const struct stringstream *s, size_t *n);
ssize_t str_printf(struct stringstream *s, const char *fmt, ...);
