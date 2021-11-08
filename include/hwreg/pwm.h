#pragma once
#include <stdint.h>
#include <hwreg/dma.h>

struct hw_pwm {
    struct hw_pwm_channel {
        uint8_t                    ctrl;
#define PWM_CTRL_ENABLE            0x01
#define PWM_CTRL_MODE              0x02
#define PWM_CTRL_DS_MUTE           0x04
#define PWM_CTRL_SEL_MASK          0x18
#define PWM_CTRL_DUAL              0x20
#define PWM_CTRL_DMA_EN            0x40
        uint16_t                   offset;
        uint16_t                   data;
        uint8_t                    ds_quant;
    } channel[2];
    struct hw_pwm_clk {
        uint8_t                    enable;
        uint16_t                   numerator;
        uint16_t                   denominator;
    } clk;
    struct hw_pwm_ramp {
        uint8_t                    enable;
        uint16_t                   numerator;
        uint16_t                   denominator;
    } ramp;
    struct hw_pwm_interp {
        uint8_t                    rate;
        uint16_t                   scale;
    } interp;
    uint16_t                       maximum;
    uint8_t                        mono;
    uint8_t                        att[2];
    uint8_t                        swap;
    struct hw_pwm_i2s {
        uint8_t                    enable;
        uint8_t                    en_sclk;
        uint8_t                    len;
    } i2s;
    struct hw_dma_ctrl             dma_ctrl;
};
