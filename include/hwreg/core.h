#pragma once
#include <stdint.h>

struct hw_core {
    struct hw_core_serdes {
        uint8_t                    tx_en;
        uint8_t                    rx_en;
        uint8_t                    loopback_en;
        uint8_t                    mux;
        uint8_t                    load_delay_tx;
        uint8_t                    load_delay_rx;
        struct hw_core_serdes_lvds {
            uint8_t                mode;
#define CORE_MODE_HC_0             0x01
#define CORE_MODE_HC_1             0x02
#define CORE_MODE_LOOP0            0x04
#define CORE_MODE_LOOP1            0x08
            uint8_t                pd;
#define CORE_PD_RX                 0x01
#define CORE_PD_TX_0               0x02
#define CORE_PD_TX_1               0x04
#define CORE_PD_TX_BIAS            0x08
#define CORE_PD_BIAS               0x10
            struct hw_core_serdes_lvds_tune {
                uint8_t            vcm_trim;
                uint8_t            rsel[2];
                uint8_t            ib_sel;
            } tune;
        } lvds;
    } serdes;
    struct hw_core_fuse {
        uint8_t                    pgm;
        uint8_t                    refresh;
        uint32_t                   data;
        uint32_t                   data_read[8];
    } fuse;
    struct hw_core_top {
        uint8_t                    cdc_sel;
#define CORE_CDC_SEL_RX_ENV        0x01
#define CORE_CDC_SEL_RX_PH         0x02
#define CORE_CDC_SEL_PH_DBG        0x04
#define CORE_CDC_SEL_TX_ENV_DPA    0x08
#define CORE_CDC_SEL_TX_ENV_DPLL   0x10
#define CORE_CDC_SEL_TX_PH_DPLL    0x20
#define CORE_CDC_SEL_DPLL_DBG      0x40
        uint8_t                    freq_sel[3];
    } top;
    struct hw_core_dpa {
        uint8_t                    clock_en;
    } dpa;
    struct hw_core_dpll {
        uint8_t                    phase_sel;
    } dpll;
};
