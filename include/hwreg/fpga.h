#pragma once
#include <stdint.h>

struct hw_fpga {
    uint8_t                        is_fpga;
    struct hw_fpga_spb607 {
        struct hw_fpga_spb607_threewire {
            uint32_t               data;
            uint8_t                ctrl;
#define FPGA_CTRL_LEN_MASK         0x1f
#define FPGA_CTRL_UNIT_MASK        0x60
            uint8_t                busy;
        } threewire;
        struct hw_fpga_spb607_rf_chain {
            uint8_t                rf_gain;
            uint8_t                enable;
#define FPGA_ENABLE_TX_EN          0x01
#define FPGA_ENABLE_RX_HP          0x02
#define FPGA_ENABLE_RX_EN          0x04
#define FPGA_ENABLE_PA_2G_EN       0x08
#define FPGA_ENABLE_PA_5G_EN       0x10
#define FPGA_ENABLE_RX_EN_SW       0x20
#define FPGA_ENABLE_TX_EN_SW       0x40
#define FPGA_ENABLE_GAIN_SEL       0x80
        } rf_chain[2];
        uint8_t                    led;
    } spb607;
    struct hw_fpga_led {
        uint8_t                    intensity;
        uint8_t                    pin;
    } led;
    struct hw_fpga_mux {
        uint8_t                    dac_a;
        uint8_t                    dac_b;
        uint8_t                    adc_a;
        uint8_t                    adc_b;
        uint8_t                    adc_iq_swap;
        uint8_t                    adc_pos;
        uint8_t                    adc_del;
        uint8_t                    adc_pos_ind;
    } mux;
    struct hw_fpga_build {
        uint8_t                    year;
        uint8_t                    month;
        uint8_t                    day;
        uint8_t                    hour;
        uint8_t                    min;
        uint8_t                    sec;
    } build;
    struct hw_fpga_ila {
        uint8_t                    trig;
    } ila;
    struct hw_fpga_awgn {
        uint16_t                   gain;
    } awgn;
    struct hw_fpga_hsl {
        uint8_t                    delay_m;
        uint8_t                    delay_s;
        uint8_t                    delay_e;
        uint8_t                    enable_tune;
    } hsl;
};
