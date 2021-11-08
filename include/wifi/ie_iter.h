#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <kernel/cdefs.h>

#define ELEMENT_ID          0
#define ELEMENT_LENGTH      1
#define ELEMENT_INFORMATION 2

struct ie_iterator {
    const uint8_t *buf;
    const uint8_t *pos;
    const uint8_t *limit;
};

static inline void
ie_iter_init(struct ie_iterator *iter, const uint8_t *ie_list, size_t len)
{
    iter->buf = ie_list;
    iter->pos = ie_list;
    iter->limit = ie_list + len;
}

static inline void
ie_iter_reset(struct ie_iterator *iter)
{
    iter->pos = iter->buf;
}

static inline bool
ie_iter_end(const struct ie_iterator *iter)
{
    return iter->pos - iter->limit >= 0;
}

static inline const uint8_t*
ie_iter_ptr(const struct ie_iterator *iter)
{
    if (ie_iter_end(iter))
        return NULL;
    return &iter->pos[ELEMENT_ID];
}

static inline unsigned int
ie_iter_id(const struct ie_iterator *iter)
{
    if (ie_iter_end(iter))
        return -1;
    return iter->pos[ELEMENT_ID];
}

static inline unsigned int
ie_iter_len(const struct ie_iterator *iter)
{
    if (ie_iter_end(iter))
        return 0;
    return iter->pos[ELEMENT_LENGTH];
}

static inline const uint8_t *
ie_iter_value(const struct ie_iterator *iter)
{
    return &iter->pos[ELEMENT_INFORMATION];
}

static inline void
ie_iter_next(struct ie_iterator *iter)
{
    if (!ie_iter_end(iter))
        iter->pos += 2 + ie_iter_len(iter);
}

static inline bool
ie_iter_find(struct ie_iterator *iter, unsigned id)
{
    ie_iter_reset(iter);

    while (!ie_iter_end(iter)) {
        if (ie_iter_id(iter) == id)
            return true;
        ie_iter_next(iter);
    }
    return false;
}

/* compare two IEs, return integer representing order in a qsort
 * compatible manner */
static inline int __attribute__((__nonnull__(1, 2)))
ie_cmp(const uint8_t *a, const uint8_t *b)
{
    int cmp = a[ELEMENT_ID] - b[ELEMENT_ID];
    if(cmp == 0)
        cmp = memcmp(&a[ELEMENT_INFORMATION], &b[ELEMENT_INFORMATION],
                     min(a[ELEMENT_LENGTH], b[ELEMENT_LENGTH]));
    if(cmp == 0)
        cmp = a[ELEMENT_LENGTH] - b[ELEMENT_LENGTH];

    return cmp;
}
