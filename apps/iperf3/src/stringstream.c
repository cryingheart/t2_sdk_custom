#include <stdarg.h>
#include <stdio.h>
#include <kernel/os.h>
#include "stringstream.h"

void
str_init(struct stringstream *s)
{
    s->max = 128;
    s->pos = 0;
    s->buf = os_alloc(s->max);
}

void
str_free(struct stringstream *s)
{
    os_free(s->buf);
}

char *
str_string(const struct stringstream *s, size_t *n)
{
    if (n)
        *n = s->pos;
    return s->buf;
}

ssize_t
str_printf(struct stringstream *s, const char *fmt, ...)
{
    va_list v;
    ssize_t n = 0;

    va_start(v, fmt);
    while (s->pos < s->max) {
        const size_t left = s->max - s->pos;
        n = vsnprintf(&s->buf[s->pos], s->max - s->pos, fmt, v);
	if (n < left)
            break;
        if (s->max < 1024) {
            s->max *= 2;
            s->buf = os_realloc(s->buf, s->max);
            assert(s->buf);
        }
    }
    s->pos += n;
    va_end(v);
    return n;
}
