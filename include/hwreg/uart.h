#pragma once
#include <stdint.h>
#include <hwreg/dma.h>

#define HW_UART_WL_NUMERATOR       14
#define HW_UART_WL_DENOMINATOR     18
#define HW_UART_DMA_UNITS          2

struct hw_uart {
    struct hw_dma_ctrl             dma_ctrl[2];
    struct hw_uart_ctrl {
        uint16_t                   numerator;
        uint32_t                   denominator;
        uint8_t                    flags;
#define UART_FLAGS_OBEY_CTS        0x01
#define UART_FLAGS_FORCE_RTS_MASK  0x06
#define UART_FLAGS_LOOPBACK        0x08
#define UART_FLAGS_TX_EMPTY        0x10
    } ctrl;
};
