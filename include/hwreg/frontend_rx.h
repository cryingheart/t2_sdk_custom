#pragma once
#include <stdint.h>

struct hw_frontend_rx {
    struct hw_frontend_rx_agc {
        uint8_t                    enable;
        uint8_t                    restart;
        uint8_t                    wait_time;
        uint8_t                    sat_th;
        uint16_t                   sat_max;
        struct hw_frontend_rx_agc_pow {
            uint8_t                slice_samples;
            uint8_t                moving_average;
            uint8_t                gain_target;
            uint8_t                gain_error_threshold;
            uint8_t                pow_var_log2;
            uint16_t               gain_error;
        } pow[4];
        uint8_t                    gain_error_threshold_sat;
        struct hw_frontend_rx_agc_differential_gain_error {
            uint8_t                slope;
            uint8_t                threshold;
        } differential_gain_error;
        struct hw_frontend_rx_agc_idet {
            uint8_t                threshold_0;
            uint8_t                threshold_1;
        } idet;
        uint8_t                    func;
        uint16_t                   rssi;
        uint16_t                   cca_ed_hi;
        uint16_t                   cca_ed_lo;
        uint16_t                   trace_ed_hi;
        uint16_t                   trace_ed_lo;
        uint8_t                    mode;
        uint8_t                    gain_m;
        uint8_t                    gain_sat_step;
        uint8_t                    gain_max;
        uint8_t                    gain_min;
        uint8_t                    curr_gain;
        uint8_t                    curr_gain_inx;
        uint16_t                   curr_gain_error;
        uint8_t                    dc_free;
        struct hw_frontend_rx_agc_ctrl {
            uint8_t                gain_change_src;
            uint8_t                rssi_sel;
        } ctrl;
        uint32_t                   gain_tbl[128];
#define FRONTEND_RX_GAIN_TBL_PMIX_GC_MASK 0x00000007
#define FRONTEND_RX_GAIN_TBL_PMIX_LG_MIXGM 0x00000008
#define FRONTEND_RX_GAIN_TBL_LUT_MASK 0x000000f0
#define FRONTEND_RX_GAIN_TBL_GAIN_BUF_5_MASK 0x00000f00
#define FRONTEND_RX_GAIN_TBL_GAIN_BUF_4_MASK 0x0000f000
#define FRONTEND_RX_GAIN_TBL_GAIN_BUF_3_MASK 0x001f0000
#define FRONTEND_RX_GAIN_TBL_GAIN_BUF_2_MASK 0x01e00000
        struct hw_frontend_rx_agc_lna_lut {
            uint8_t                bits;
#define FRONTEND_RX_BITS_BYPASS_1  0x01
#define FRONTEND_RX_BITS_BYPASS_2  0x02
#define FRONTEND_RX_BITS_BYPASS_3  0x04
#define FRONTEND_RX_BITS_GAIN_CTRL_1 0x08
            uint8_t                gm_gain_2;
            uint8_t                gm_gain_tone_2;
            uint8_t                gm_gain_3;
            uint8_t                gm_gain_tone_3;
        } lna_lut[16];
    } agc;
    struct hw_frontend_rx_rx_notch {
        uint8_t                    notch_en;
        uint8_t                    alpha_acq_shift;
        uint8_t                    alpha_trk_shift;
        uint16_t                   ctr_a_th;
        uint16_t                   phase_step;
    } rx_notch[2];
    struct hw_frontend_rx_delay_comp {
        uint8_t                    n;
        uint8_t                    mu;
    } delay_comp;
    struct hw_frontend_rx_phase_scale {
        uint16_t                   scale;
        uint8_t                    shift;
    } phase_scale;
    uint8_t                        loopback;
    uint8_t                        phase_env_only_mode;
    uint8_t                        phase_only_env;
    uint8_t                        iq_swap;
    uint16_t                       env_dc_offset;
    uint8_t                        env_dc_shift;
};
