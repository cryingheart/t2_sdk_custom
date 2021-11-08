#pragma once
#include <stdint.h>
#include <hwreg/dma.h>

#define HW_SUPPORT_STACK_MSP       0
#define HW_SUPPORT_STACK_PSP       1

struct hw_support {
    struct hw_support_innotrace {
        uint8_t                    halt_en;
        uint8_t                    serdes_en;
        uint8_t                    dma_en;
        struct hw_support_innotrace_mac {
            uint32_t               enable;
#define SUPPORT_ENABLE_CPU_MASK    0x00000003
#define SUPPORT_ENABLE_COP_WIFI_MASK 0x0000000c
#define SUPPORT_ENABLE_COP_BLUE_MASK 0x00000030
#define SUPPORT_ENABLE_COP_SDIO_MASK 0x000000c0
#define SUPPORT_ENABLE_HW_EVENT_MASK 0x00000300
#define SUPPORT_ENABLE_HW_GPIO     0x00000400
#define SUPPORT_ENABLE_HW_TIMER    0x00000800
#define SUPPORT_ENABLE_HW_BACKOFF  0x00001000
#define SUPPORT_ENABLE_INTNUM      0x00002000
#define SUPPORT_ENABLE_CURRPRI     0x00004000
#define SUPPORT_ENABLE_BACKOFF_CNTR_0 0x00008000
#define SUPPORT_ENABLE_BACKOFF_CNTR_1 0x00010000
#define SUPPORT_ENABLE_BACKOFF_CNTR_2 0x00020000
#define SUPPORT_ENABLE_BACKOFF_CNTR_3 0x00040000
#define SUPPORT_ENABLE_BACKOFF_CNTR_4 0x00080000
#define SUPPORT_ENABLE_BACKOFF_CNTR_5 0x00100000
        } mac;
        struct hw_support_innotrace_phy {
            uint32_t               enable;
#define SUPPORT_ENABLE_ADC         0x00000001
#define SUPPORT_ENABLE_RSSI        0x00000002
#define SUPPORT_ENABLE_GAIN        0x00000004
#define SUPPORT_ENABLE_QPSK_MRC    0x00000008
#define SUPPORT_ENABLE_OFDM_VAC    0x00000010
#define SUPPORT_ENABLE_OFDM_WAC    0x00000020
#define SUPPORT_ENABLE_OFDM_NACV1  0x00000040
#define SUPPORT_ENABLE_OFDM_NACV4  0x00000080
#define SUPPORT_ENABLE_BLE_FREQ    0x00000100
#define SUPPORT_ENABLE_BLE_CORR    0x00000200
#define SUPPORT_ENABLE_QPSK_STATE  0x00000400
#define SUPPORT_ENABLE_OFDM_STATE  0x00000800
#define SUPPORT_ENABLE_CLK         0x00001000
#define SUPPORT_ENABLE_RST         0x00002000
#define SUPPORT_ENABLE_POW_0       0x00004000
#define SUPPORT_ENABLE_POW_1       0x00008000
#define SUPPORT_ENABLE_POW_2       0x00010000
#define SUPPORT_ENABLE_QPSK_ERR_I  0x00020000
#define SUPPORT_ENABLE_QPSK_ERR_Q  0x00040000
#define SUPPORT_ENABLE_OFDM_PH_EST 0x00080000
#define SUPPORT_ENABLE_BLE_EDR_PH_ERR 0x00100000
#define SUPPORT_ENABLE_BLE_EDR_CONST 0x00200000
#define SUPPORT_ENABLE_BLE_DC_RESIDUAL 0x00400000
#define SUPPORT_ENABLE_BLE_EDR_SYNC 0x00800000
            struct hw_support_innotrace_phy_fend {
                uint8_t            format;
                uint8_t            n_max;
                uint8_t            ed_gated;
            } fend;
        } phy;
    } innotrace;
    uint32_t                       stack_guard[2];
    struct hw_support_default_slave {
        uint32_t                   addr;
        uint8_t                    id;
        uint8_t                    hit;
    } default_slave;
    struct hw_support_console {
        uint8_t                    txd;
        uint8_t                    full;
        uint16_t                   numerator;
        uint16_t                   denominator;
        uint8_t                    use_innotrace;
        uint8_t                    use_dma;
    } console;
    struct hw_support_phy_interface {
        uint8_t                    swap;
        uint8_t                    transparent;
    } phy_interface;
    struct hw_support_gpio_mux {
        uint32_t                   config;
        uint8_t                    sel[20];
    } gpio_mux;
    struct hw_support_clkgate {
        uint32_t                   override;
        uint16_t                   cipher_override;
    } clkgate;
    struct hw_support_iject {
        uint8_t                    mode;
        uint32_t                   table_ptr;
        uint32_t                   patched[8];
        uint16_t                   last_tag[4];
        uint32_t                   last_pc[4];
    } iject;
    struct hw_support_hsl {
        uint8_t                    format;
        uint8_t                    comma;
    } hsl;
    struct hw_dma_ctrl             dma_ctrl;
    struct hw_support_id {
        uint8_t                    ahb_id;
    } id;
};
