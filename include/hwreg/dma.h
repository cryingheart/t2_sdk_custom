#pragma once
#include <stdint.h>

struct hw_dma {
    struct hw_dma_ctrl {
        uint32_t                   desc_pointer;
        uint16_t                   size;
        uint16_t                   curr;
        uint16_t                   circular_free;
        uint16_t                   xfr;
#define DMA_XFR_START              0x0001
#define DMA_XFR_STOP               0x0002
#define DMA_XFR_MODE_MASK          0x000c
#define DMA_MODE_DESC              0x0000
#define DMA_MODE_LINEAR            0x0008
#define DMA_MODE_CIRCULAR          0x000c
#define DMA_XFR_DIR                0x0010
#define DMA_DIR_TX                 0x0000
#define DMA_DIR_RX                 0x0010
#define DMA_XFR_GEN_IRQ            0x0020
#define DMA_XFR_INHIBIT            0x0040
#define DMA_XFR_BYTE_MODE          0x0080
#define DMA_XFR_WORD_MODE          0x0100
        uint32_t                   status;
#define DMA_STATUS_HADDR_MASK      0x00ffffff
#define DMA_STATUS_BUSY            0x01000000
    } ctrl;
};
