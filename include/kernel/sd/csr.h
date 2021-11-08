#pragma once
#include <stdint.h>
#include <stdbool.h>

/* error bit, command response, command execution */
static inline bool sd_csr_out_of_range(uint32_t csr)
{
    return (csr >> 31) & 1;
}

/* error bit, command response, command execution */
static inline bool sd_csr_address_error(uint32_t csr)
{
    return (csr >> 30) & 1;
}

/* error bit, command response, command execution */
static inline bool sd_csr_block_len_error(uint32_t csr)
{
    return (csr >> 29) & 1;
}

/* error bit, command response */
static inline bool sd_csr_erase_seq_error(uint32_t csr)
{
    return (csr >> 28) & 1;
}

/* error bit, command response, command execution */
static inline bool sd_csr_erase_param(uint32_t csr)
{
    return (csr >> 27) & 1;
}

/* error bit, command response, command execution */
static inline bool sd_csr_wp_violation(uint32_t csr)
{
    return (csr >> 26) & 1;
}

/* status bit, command execution */
static inline bool sd_csr_card_is_locked(uint32_t csr)
{
    return (csr >> 25) & 1;
}

/* error bit, command response, command execution */
static inline bool sd_csr_lock_unlock_failed(uint32_t csr)
{
    return (csr >> 24) & 1;
}

/* error bit, command response */
static inline bool sd_csr_com_crc_error(uint32_t csr)
{
    return (csr >> 23) & 1;
}

/* error bit, command response */
static inline bool sd_csr_illegal_command(uint32_t csr)
{
    return (csr >> 22) & 1;
}

/* error bit, command response, command execution */
static inline bool sd_csr_card_ecc_failed(uint32_t csr)
{
    return (csr >> 21) & 1;
}

/* error bit, command response, command execution */
static inline bool sd_csr_cc_error(uint32_t csr)
{
    return (csr >> 20) & 1;
}

/* error bit, command response, command execution */
static inline bool sd_csr_error(uint32_t csr)
{
    return (csr >> 19) & 1;
}

/* error bit, command response, command execution */
static inline bool sd_csr_csd_overwrite(uint32_t csr)
{
    return (csr >> 16) & 1;
}

/* error bit, command response, command execution */
static inline bool sd_csr_wp_erase_skip(uint32_t csr)
{
    return (csr >> 15) & 1;
}

/* status bit, command execution */
static inline bool sd_csr_card_ecc_disable(uint32_t csr)
{
    return (csr >> 14) & 1;
}

/* status bit, command response */
static inline bool sd_csr_erase_reset(uint32_t csr)
{
    return (csr >> 13) & 1;
}

/* status bit, command execution */
static inline unsigned int sd_csr_current_state(uint32_t csr)
{
    return (csr >> 9) & 15;
}

/* status bit, command execution */
static inline bool sd_csr_ready_for_data(uint32_t csr)
{
    return (csr >> 8) & 1;
}

/* status bit, command response */
static inline bool sd_csr_app_cmd(uint32_t csr)
{
    return (csr >> 5) & 1;
}

/* error bit, command response */
static inline bool sd_csr_ake_seq_error(uint32_t csr)
{
    return (csr >> 3) & 1;
}

static inline bool sd_csr_any_error(uint32_t csr)
{
    return csr & 0xfdf98008;
}
