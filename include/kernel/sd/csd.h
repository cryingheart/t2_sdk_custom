#pragma once
#include <stdint.h>
#include <stdbool.h>

static inline unsigned int sd_csd_structure(uint32_t csd[4])
{
    return (csd[3] >> 30) & 0x3;
}

static inline unsigned int sd_csd_taac(uint32_t csd[4])
{
    return (csd[3] >> 16) & 0xff;
}

static inline unsigned int sd_csd_nsac(uint32_t csd[4])
{
    return (csd[3] >> 8) & 0xff;
}

static inline unsigned int sd_csd_tran_speed(uint32_t csd[4])
{
    return (csd[3] >> 0) & 0xff;
}

static inline unsigned int sd_csd_ccc(uint32_t csd[4])
{
    return (csd[2] >> 20) & 0xfff;
}

static inline unsigned int sd_csd_read_bl_len(uint32_t csd[4])
{
    return (csd[2] >> 16) & 0xf;
}

static inline bool sd_csd_read_bl_partial(uint32_t csd[4])
{
    return (csd[2] >> 15) & 0x1;
}

static inline bool sd_csd_write_blk_misalign(uint32_t csd[4])
{
    return (csd[2] >> 14) & 0x1;
}

static inline bool sd_csd_read_blk_misalign(uint32_t csd[4])
{
    return (csd[2] >> 13) & 0x1;
}

static inline bool sd_csd_dsr_imp(uint32_t csd[4])
{
    return (csd[2] >> 12) & 0x1;
}

static inline unsigned int sd_csd1_c_size(uint32_t csd[4])
{
    return ((csd[1] >> 30) & 0x3) | ((csd[2] << 2) & 0xffc);
}

static inline unsigned int sd_csd2_c_size(uint32_t csd[4])
{
    return ((csd[1] >> 16) & 0xffff) | ((csd[2] << 16) & 0x3f0000);
}

static inline unsigned int sd_csd1_vdd_r_curr_min(uint32_t csd[4])
{
    return (csd[1] >> 27) & 0x7;
}

static inline unsigned int sd_csd1_vdd_r_curr_max(uint32_t csd[4])
{
    return (csd[1] >> 24) & 0x7;
}

static inline unsigned int sd_csd1_vdd_w_curr_min(uint32_t csd[4])
{
    return (csd[1] >> 21) & 0x7;
}

static inline unsigned int sd_csd1_vdd_w_curr_max(uint32_t csd[4])
{
    return (csd[1] >> 18) & 0x7;
}

static inline unsigned int sd_csd1_c_size_mult(uint32_t csd[4])
{
    return (csd[1] >> 15) & 0x7;
}

static inline bool sd_csd_erase_blk_en(uint32_t csd[4])
{
    return (csd[1] >> 14) & 0x1;
}

static inline unsigned int sd_csd_sector_size(uint32_t csd[4])
{
    return (csd[1] >> 7) & 0x7f;
}

static inline unsigned int sd_csd_wp_grp_size(uint32_t csd[4])
{
    return (csd[1] >> 0) & 0x7f;
}

static inline bool sd_csd_wp_grp_enable(uint32_t csd[4])
{
    return (csd[0] >> 31) & 0x1;
}

static inline unsigned int sd_csd_r2w_factor(uint32_t csd[4])
{
    return (csd[0] >> 26) & 0x7;
}

static inline unsigned int sd_csd_write_bl_len(uint32_t csd[4])
{
    return (csd[0] >> 22) & 0xf;
}

static inline bool sd_csd_write_bl_partial(uint32_t csd[4])
{
    return (csd[0] >> 21) & 0x1;
}

static inline bool sd_csd_file_format_grp(uint32_t csd[4])
{
    return (csd[0] >> 15) & 0x1;
}

static inline bool sd_csd_copy(uint32_t csd[4])
{
    return (csd[0] >> 14) & 0x1;
}

static inline bool sd_csd_perm_write_protect(uint32_t csd[4])
{
    return (csd[0] >> 13) & 0x1;
}

static inline bool sd_csd_tmp_write_protect(uint32_t csd[4])
{
    return (csd[0] >> 12) & 0x1;
}

static inline unsigned int sd_csd_file_format(uint32_t csd[4])
{
    return (csd[0] >> 10) & 0x3;
}
