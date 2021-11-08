#pragma once
#include <stdint.h>
#include <hwreg/dma.h>

struct hw_spi_slv {
    struct hw_dma_ctrl             dma_ctrl[2];
    struct hw_spi_slv_ctrl {
        uint8_t                    ready;
    } ctrl;
    uint8_t                        state;
    uint8_t                        sr;
    uint8_t                        wave;
};
