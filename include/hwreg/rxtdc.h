#pragma once
#include <stdint.h>

struct hw_rxtdc {
    uint8_t                        ph_mode;
    uint8_t                        settle_count;
    uint8_t                        acc_count;
    uint8_t                        treashold;
    uint8_t                        inc_dec;
    uint8_t                        pvt;
    uint8_t                        acq;
    uint8_t                        dac;
    uint8_t                        dac_step;
    uint8_t                        acq_step;
    uint8_t                        updn_pol;
    uint16_t                       updn_cntr;
    uint8_t                        enable;
#define RXTDC_ENABLE_DAC           0x01
#define RXTDC_ENABLE_ACQ           0x02
#define RXTDC_ENABLE_PVT           0x04
    uint8_t                        dir;
#define RXTDC_DIR_DAC              0x01
#define RXTDC_DIR_ACQ              0x02
#define RXTDC_DIR_PVT              0x04
};
