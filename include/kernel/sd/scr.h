#pragma once
#include <stdint.h>
#include <stdbool.h>

static inline unsigned int sd_scr_structure(uint32_t scr[2])
{
    return (scr[1] >> 28) & 15;
}

static inline unsigned int sd_scr_sd_spec(uint32_t scr[2])
{
    return (scr[1] >> 24) & 15;
}

static inline bool sd_scr_data_stat_after_erase(uint32_t scr[2])
{
    return (scr[1] >> 23) & 1;
}

static inline unsigned int sd_scr_sd_security(uint32_t scr[2])
{
    return (scr[1] >> 20) & 7;
}

static inline unsigned int sd_scr_sd_bus_widths(uint32_t scr[2])
{
    return (scr[1] >> 16) & 15;
}

static inline bool sd_scr_sd_spec3(uint32_t scr[2])
{
    return (scr[1] >> 15) & 1;
}

static inline unsigned int sd_scr_ex_security(uint32_t scr[2])
{
    return (scr[1] >> 11) & 15;
}

static inline unsigned int sd_scr_cmd_support(uint32_t scr[2])
{
    return scr[1] & 3;
}

static inline unsigned int sd_scr_manufacturer(uint32_t scr[2])
{
    return scr[0];
}

