#pragma once
#include <stdint.h>
#include <hwreg/dma.h>

struct hw_pdm {
    uint8_t                        ctrl;
#define PDM_CTRL_ENABLE            0x01
#define PDM_CTRL_LR                0x02
    uint16_t                       numerator;
    uint16_t                       denominator;
    uint16_t                       cic_rate;
    uint8_t                        scale;
    struct hw_dma_ctrl             dma_ctrl;
};
