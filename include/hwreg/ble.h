#pragma once
#include <stdint.h>

struct hw_ble {
    uint8_t                        mode;
#define BLE_MODE_NONE              0x0
#define BLE_MODE_TX                0x1
#define BLE_MODE_RX                0x2
#define BLE_MODE_RX2MBPS           0x6
#define BLE_MODE_OQPSK             0x8
    uint8_t                        tick_divider;
    uint8_t                        divider_phase;
    struct hw_ble_frontend {
        uint16_t                   nco_freq_i;
        uint8_t                    nco_freq_n;
        uint8_t                    nco_freq_d;
        uint16_t                   ampl;
        uint8_t                    ampl_gain;
        uint8_t                    tx_polar;
#define BLE_TX_POLAR_IQ2IQ         0x0
#define BLE_TX_POLAR_IQ2POL        0x1
#define BLE_TX_POLAR_POL2IQ        0x2
#define BLE_TX_POLAR_POL2POL       0x3
        uint8_t                    rx_polar;
        uint8_t                    cic_rate;
        uint8_t                    cic_shift;
        uint8_t                    cic_filter_sel;
        uint8_t                    rrc_bypass;
        uint16_t                   power_threshold;
        uint16_t                   power_est;
    } frontend;
    struct hw_ble_tx {
        uint8_t                    enable;
#define BLE_ENABLE_GFSK_ENABLE     0x01
#define BLE_ENABLE_PSK_ENABLE      0x02
#define BLE_ENABLE_OQPSK_ENABLE    0x04
#define BLE_ENABLE_BT_MODE         0x08
#define BLE_BT_MODE_BLE            0x00
#define BLE_BT_MODE_CLASSIC        0x08
#define BLE_ENABLE_PSK_RATE        0x10
#define BLE_PSK_RATE_2MBIT         0x00
#define BLE_PSK_RATE_3MBIT         0x10
        uint8_t                    whitener_pause;
        uint8_t                    fec_rate;
#define BLE_FEC_RATE_CL_NON        0x0
#define BLE_FEC_RATE_CL_13         0x1
#define BLE_FEC_RATE_CL_23         0x2
#define BLE_FEC_RATE_LE_NON        0x0
#define BLE_FEC_RATE_LE_S2         0x1
#define BLE_FEC_RATE_LE_S8         0x3
        uint8_t                    byte_size;
        uint8_t                    swap;
        uint16_t                   minx;
        uint16_t                   ampl;
        uint8_t                    gauss_rate;
        uint8_t                    psk_sample_rate;
        uint8_t                    whitener_seed;
    } tx;
    struct hw_ble_rx {
        uint16_t                   enable;
#define BLE_ENABLE_RX_ENABLE       0x0001
#define BLE_ENABLE_CORR_64BIT      0x0002
#define BLE_ENABLE_BT_CLASSIC      0x0004
#define BLE_ENABLE_BLEC_ENABLE     0x0008
#define BLE_ENABLE_FREQ_EST        0x0010
#define BLE_ENABLE_DC_MM_ENABLE    0x0020
#define BLE_ENABLE_DC_AVG_ENABLE   0x0040
#define BLE_ENABLE_DC_EST_HALF     0x0080
#define BLE_ENABLE_SPC_EN          0x0100
#define BLE_ENABLE_DC_AVG_RT_ENABLE 0x0200
        uint32_t                   access_code_lo;
        uint32_t                   access_code_hi;
        uint8_t                    swap;
        uint8_t                    seed;
        uint8_t                    threshold;
        uint8_t                    access_code_bit_cnt_th;
        uint16_t                   bits_to_dma;
        uint8_t                    freq_est_div_cl;
        uint8_t                    freq_est_div_edr;
        uint8_t                    dc_residual_div;
        uint8_t                    spc_threshold;
        uint16_t                   freq_error_est;
        uint8_t                    edr_enable;
        uint8_t                    edr_rate;
        uint8_t                    edr_coherent;
        uint8_t                    edr_avg_fb_enable;
        uint16_t                   edr_guard_time;
        uint8_t                    edr_freq_err_avg_nco;
        uint8_t                    edr_freq_err_avg_local;
        uint8_t                    edr_phase_err_sr0;
        uint8_t                    edr_phase_err_sr1;
        uint8_t                    edr_spc_th;
        uint8_t                    edr_spc_hysteresis;
        uint8_t                    edr_sp;
        uint32_t                   phase_err_sum;
        uint16_t                   phase_err_max;
        struct hw_ble_rx_classic {
            uint8_t                fec_code;
#define BLE_FEC_CODE_NON           0x0
#define BLE_FEC_CODE_13            0x1
#define BLE_FEC_CODE_23            0x2
            uint8_t                fec_err_cntr;
        } classic;
        struct hw_ble_rx_blec {
            uint8_t                ci;
#define BLE_CI_S8                  0x0
#define BLE_CI_S2                  0x1
#define BLE_CI_FREQ                0x2
            uint8_t                pdu_enable;
            uint16_t               pdu_bits;
            uint8_t                soft_scale;
            uint8_t                freq_to_dma;
            uint8_t                coding_indicator;
        } blec;
        struct hw_ble_rx_oqpsk {
            uint8_t                num_peaks;
            uint8_t                sfd;
            uint8_t                status;
#define BLE_STATUS_SFD             0x01
#define BLE_STATUS_CS              0x02
        } oqpsk;
        uint8_t                    start_pos;
        uint8_t                    dc_avg_mode;
    } rx;
    struct hw_ble_cipher_e0 {
        uint32_t                   init[6];
        uint32_t                   init_cl;
        uint8_t                    enable;
#define BLE_ENABLE_IDLE            0x0
#define BLE_ENABLE_SEED            0x1
#define BLE_ENABLE_CIPHER          0x3
    } cipher_e0;
};
