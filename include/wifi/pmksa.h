#pragma once

#include <stdint.h>
#include <string.h>
#include <kernel/cipher_buffer.h>

#include <wifi/akm_suite.h>
#include <wifi/pmkid.h>
#include <kernel/hwaddr.h>

struct pmksa {
    struct pmkid         pmkid;
    struct cipher_buffer pmk;
    enum akm_suite       akm_suite;
    uint8_t              address[ETHER_ADDR_LEN];
};

#define PMKSA_INITIALIZER(sa_) { .pmk = CIPHER_BUFFER_INITIALIZER }

static inline void pmksa_init(struct pmksa *sa)
{
    memset(sa, 0, sizeof(*sa));
    cipher_buffer_init(&sa->pmk);
}

static inline void pmksa_clear(struct pmksa *sa)
{
    cipher_buffer_free(&sa->pmk);
    memset(sa, 0, sizeof(*sa));
}

static inline bool pmksa_valid(const struct pmksa *sa)
{
    return sa->akm_suite != 0
        && pmkid_valid(&sa->pmkid)
        && os_hwaddr_valid(HWADDR_SCOPE_WIFI, sa->address);
}

static inline int pmksa_copy(struct pmksa *dst, const struct pmksa *src)
{
    struct cipher_buffer tmp = CIPHER_BUFFER_INITIALIZER;
    int rval;

    rval = cipher_buffer_copy(&tmp, &src->pmk);
    if(rval < 0)
        return rval;

    cipher_buffer_move(&dst->pmk, &tmp);

    pmkid_copy(&dst->pmkid, &src->pmkid);

    dst->akm_suite = src->akm_suite;

    os_hwaddr_copy(dst->address, src->address);

    return 0;
}
