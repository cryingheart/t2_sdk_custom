#pragma once
#include <stdint.h>

struct hw_wifi_rx_ofdm {
    struct hw_wifi_rx_ofdm_frame_info {
        struct hw_wifi_rx_ofdm_frame_info_ofdm {
            uint8_t                rate;
            uint8_t                parity;
            uint16_t               length;
        } ofdm;
        struct hw_wifi_rx_ofdm_frame_info_ht {
            uint8_t                mcs;
            uint16_t               length;
            uint32_t               status;
#define WIFI_RX_OFDM_STATUS_CRC_OK 0x00000001
#define WIFI_RX_OFDM_STATUS_SMOOTHING 0x00000002
#define WIFI_RX_OFDM_STATUS_NOT_SOUNDING 0x00000004
#define WIFI_RX_OFDM_STATUS_AGGREGATION 0x00000008
#define WIFI_RX_OFDM_STATUS_STBC_MASK 0x00000030
#define WIFI_RX_OFDM_STATUS_FEC_CODING 0x00000040
#define WIFI_RX_OFDM_STATUS_SHORT_GI 0x00000080
#define WIFI_RX_OFDM_STATUS_NESS_MASK 0x00000300
#define WIFI_RX_OFDM_STATUS_CBW    0x00000400
        } ht;
    } frame_info;
    struct hw_wifi_rx_ofdm_frontend {
        struct hw_wifi_rx_ofdm_frontend_stf {
            uint8_t                dc_comp_en;
            uint8_t                alpha_shift;
            uint8_t                stf_th_when_11b;
            uint16_t               pow_u_accu_th_0;
            uint16_t               pow_u_accu_th_1;
            uint16_t               pow_u_accu_th_2;
            uint16_t               pow_u_accu_th_3;
            uint16_t               nvac_th_0;
            uint16_t               nvac_th_1;
            uint16_t               nvac_th_2;
            uint16_t               nvac_th_3;
            uint16_t               nwac_th_0;
            uint16_t               nwac_th_1;
            uint16_t               nwac_th_2;
            uint16_t               nwac_th_3;
            uint8_t                nvac_scale;
            uint8_t                nwac_scale;
            uint8_t                wait_time_0;
            uint8_t                wait_time_1;
            uint8_t                ph_est_mode;
            uint8_t                prof_down;
        } stf;
        struct hw_wifi_rx_ofdm_frontend_ltf {
            uint16_t               ctr_b_th;
            uint16_t               nacv1_det_1_th;
            uint16_t               nacv4_det_1_th;
            uint8_t                nacv_det_2_th;
            uint8_t                gain_target;
            uint32_t               max_ma16_norm_mf_th;
            uint16_t               snr_est_th;
            uint8_t                prof_down;
            uint8_t                ptr_offset;
            uint8_t                mf_offset;
            uint8_t                mf_win_ctrl;
            struct hw_wifi_rx_ofdm_frontend_ltf_ac {
                uint16_t           normv_th;
            } ac;
            struct hw_wifi_rx_ofdm_frontend_ltf_dc_est {
                uint16_t           abs_ph_est_eff_lo_th;
                uint16_t           abs_ph_est_eff_hi_th;
            } dc_est;
        } ltf;
    } frontend;
    struct hw_wifi_rx_ofdm_backend {
        uint8_t                    enable_regulate;
        uint8_t                    pilot_extract_avg;
        uint8_t                    chest_smoothing_alw;
        uint8_t                    short_gi_enable;
        uint16_t                   ph_est;
    } backend;
    struct hw_wifi_rx_ofdm_spectrum {
        uint8_t                    mode;
#define WIFI_RX_OFDM_MODE_POW      0x01
#define WIFI_RX_OFDM_MODE_SRC2     0x02
#define WIFI_RX_OFDM_MODE_SC64     0x04
        uint8_t                    symbols;
    } spectrum;
    struct hw_wifi_rx_ofdm_rx_samp_comp {
        uint32_t                   fc_mode_scale;
        uint8_t                    group_delay_mu;
        uint8_t                    ppm_error_en;
    } rx_samp_comp;
    uint8_t                        src_4_int_th;
    uint16_t                       snr_est;
};
