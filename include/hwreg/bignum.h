#pragma once
#include <stdint.h>
#include <hwreg/dma.h>

#define HW_BIGNUM_DMA_UNITS        3

struct hw_bignum {
    struct hw_dma_ctrl             dma_ctrl[3];
    struct hw_bignum_operation {
        uint8_t                    num;
        uint16_t                   c;
        uint8_t                    operation;
#define BIGNUM_OPERATION_A_MUL_C   0x1
#define BIGNUM_OPERATION_A_ADD_B   0x2
#define BIGNUM_OPERATION_A_SUB_B   0x3
#define BIGNUM_OPERATION_A_SHIFT_C 0x4
        uint8_t                    idle;
    } operation;
};
