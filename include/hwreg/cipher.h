#pragma once
#include <stdint.h>
#include <hwreg/dma.h>

#define HW_CIPHER_DMA_UNITS        3

struct hw_cipher {
    struct hw_dma_ctrl             dma_ctrl[3];
    struct hw_cipher_status {
        uint8_t                    busy;
        uint8_t                    raw;
    } status;
};
