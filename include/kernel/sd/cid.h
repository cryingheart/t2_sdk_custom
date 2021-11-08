#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Manufacturer ID */
static inline unsigned int sd_cid_mid(uint32_t cid[4])
{
    return (cid[3] >> 24) & 0xff;
}

/* OEM/Application ID */
static inline unsigned int sd_cid_oid(uint32_t cid[4])
{
    return (cid[3] >> 8) & 0xffff;
}

/* Product name */
static inline void sd_cid_pnm(uint32_t cid[4], char name[6], size_t size)
{
    unsigned int i = 0;
    if(i < size)
        name[i++] = cid[3] & 0xff;
    if(i < size)
        name[i++] = (cid[2] >> 24) & 0xff;
    if(i < size)
        name[i++] = (cid[2] >> 16) & 0xff;
    if(i < size)
        name[i++] = (cid[2] >> 8) & 0xff;
    if(i < size)
        name[i++] = (cid[2] >> 0) & 0xff;
    if(i < size)
        name[i] = '\0';
    else if(size > 0)
        name[size-1] = '\0';
}

/* Product revision */
static inline unsigned int sd_cid_prv(uint32_t cid[4])
{
    return (cid[1] >> 24) & 0xff;
}

/* Product serial number */
static inline unsigned int sd_cid_psn(uint32_t cid[4])
{
    return ((cid[0] >> 24) & 0xff) | ((cid[1] << 8) & 0xffffff00);
}

/* Manufacturing date */
static inline unsigned int sd_cid_mdt(uint32_t cid[4])
{
    return (cid[0] >> 8) & 0xfff;
}
