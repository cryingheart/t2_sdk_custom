#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <kernel/cipher_buffer.h>

#define PMKID_SIZE 16

struct pmkid {
    uint8_t              pmkid[PMKID_SIZE];
};

static inline void pmkid_copy(struct pmkid *dst, const struct pmkid *src)
{
    *dst = *src;
}

static inline void pmkid_clear(struct pmkid *dst)
{
    memset(dst->pmkid, 0, sizeof(dst->pmkid));
}

static inline bool pmkid_valid(const struct pmkid *pmkid)
{
    for(size_t i = 0; i < PMKID_SIZE; i++)
        if(pmkid->pmkid[i] != 0)
            return true;
    return false;
}

static inline int pmkid_cmp(const struct pmkid *a, const struct pmkid *b)
{
    return memcmp(a->pmkid, b->pmkid, PMKID_SIZE);
}
