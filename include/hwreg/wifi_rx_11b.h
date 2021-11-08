#pragma once
#include <stdint.h>

struct hw_wifi_rx_11b {
    struct hw_wifi_rx_11b_frame_info {
        uint8_t                    rate;
        uint16_t                   length;
        uint32_t                   status;
#define WIFI_RX_11B_STATUS_SERVICE_MASK 0x000000ff
#define WIFI_RX_11B_STATUS_PLCP_PREAMBLE_IDX_MASK 0x0000ff00
#define WIFI_RX_11B_STATUS_FAIL    0x00010000
#define WIFI_RX_11B_STATUS_PLCP_PREAMBLE_FORMAT 0x00020000
    } frame_info;
    uint8_t                        dc_comp_en;
    uint16_t                       fc_mode;
    uint32_t                       sum_norm_th;
    uint16_t                       mrc_th;
    uint8_t                        sync_num_th;
    uint8_t                        norm_2_mf_th;
    uint16_t                       norm_x_log_th;
    uint8_t                        n_guard_symb;
    uint8_t                        hard_dec_th;
    uint8_t                        neg_ppm_error_en;
    uint8_t                        unlocked_clock_en;
    uint8_t                        dfe_update;
    uint8_t                        dfe_gain_ff;
    uint8_t                        dfe_gain_fb;
    uint8_t                        read_offset;
    uint8_t                        win_mode;
    uint8_t                        fine_timing_offset;
    uint8_t                        peak_mode;
    uint8_t                        preamble_training;
    uint8_t                        fixed_scrambler_seed;
    uint16_t                       sqi_i;
    uint16_t                       sqi_q;
};
