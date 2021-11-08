#pragma once
#include <stdint.h>

struct hw_frontend {
    struct hw_frontend_tx {
        uint8_t                    src;
#define FRONTEND_SRC_WIFI_IQ       0x0
#define FRONTEND_SRC_BLUE_IQ       0x1
#define FRONTEND_SRC_BLUE_POLAR    0x2
#define FRONTEND_SRC_SDL_IQ        0x3
#define FRONTEND_SRC_SDL_IQ_80     0x4
#define FRONTEND_SRC_TONE_IQ       0x5
#define FRONTEND_SRC_SERDES_POLAR  0x6
#define FRONTEND_SRC_MTG_IQ        0x7
        struct hw_frontend_tx_awgn {
            uint8_t                enable;
            uint16_t               power;
        } awgn;
        uint8_t                    iq_swap;
        struct hw_frontend_tx_hbf2 {
            uint16_t               b[12];
            uint8_t                bypass;
        } hbf2;
        struct hw_frontend_tx_cfr {
            uint16_t               t;
            uint16_t               alpha;
            uint8_t                b[8];
            uint8_t                bypass;
        } cfr;
        struct hw_frontend_tx_sign {
            uint8_t                mode;
#define FRONTEND_MODE_ENABLE       0x01
#define FRONTEND_MODE_AVG          0x02
#define FRONTEND_MODE_PHASE        0x04
#define FRONTEND_MODE_CLEAN        0x08
            uint16_t               min_phase;
        } sign;
        struct hw_frontend_tx_dpd {
            uint8_t                enable;
#define FRONTEND_ENABLE_ENVELOPE   0x1
#define FRONTEND_ENABLE_PHASE      0x2
            uint16_t               lut[258];
        } dpd;
        struct hw_frontend_tx_dpd_2nd {
            uint8_t                enable;
            uint8_t                slope;
        } dpd_2nd;
        struct hw_frontend_tx_env_ph_delay_comp {
            uint8_t                n;
            uint8_t                mu;
        } env_ph_delay_comp;
        uint8_t                    scale;
        struct hw_frontend_tx_tone_gen {
            uint8_t                en;
            uint8_t                mode;
            uint16_t               index[2];
        } tone_gen;
        struct hw_frontend_tx_mtg {
            uint8_t                en;
            uint8_t                mode;
#define FRONTEND_MODE_STF          0x0
#define FRONTEND_MODE_LTF          0x1
#define FRONTEND_MODE_HTLTF        0x2
        } mtg;
        uint16_t                   phase_mux_comp;
    } tx;
    struct hw_frontend_sdl {
        uint8_t                    enable_dma;
#define FRONTEND_ENABLE_DMA_TX     0x01
#define FRONTEND_ENABLE_DMA_RX     0x02
        uint8_t                    tx_dma_src;
#define FRONTEND_TX_DMA_SRC_IQ_40  0x0
#define FRONTEND_TX_DMA_SRC_IQ_80  0x1
#define FRONTEND_TX_DMA_SRC_POLAR  0x2
        uint8_t                    tx_dma_format;
        uint8_t                    rx_dma_src;
#define FRONTEND_RX_DMA_SRC_IQ_20  0x0
#define FRONTEND_RX_DMA_SRC_IQ_40  0x1
#define FRONTEND_RX_DMA_SRC_IQ_80  0x2
#define FRONTEND_RX_DMA_SRC_POLAR_DIFF_E4 0x3
#define FRONTEND_RX_DMA_SRC_POLAR_DIFF_E5_P9 0x4
#define FRONTEND_RX_DMA_SRC_POLAR_DIFF_E6_P8 0x5
#define FRONTEND_RX_DMA_SRC_DBG_RX_PH 0x6
#define FRONTEND_RX_DMA_SRC_DBG_DPLL 0x7
#define FRONTEND_RX_DMA_SRC_SPECTRUM 0x8
        uint8_t                    serdes_format;
        uint8_t                    dma_format;
        uint32_t                   pattern;
        uint16_t                   rx_data;
        uint16_t                   static_am;
        uint16_t                   static_pm;
        uint8_t                    rx_iq_80_src;
        uint8_t                    tx_iq_80_src;
        uint8_t                    tx_pol_am_src;
        uint8_t                    tx_pol_pm_src;
    } sdl;
};
