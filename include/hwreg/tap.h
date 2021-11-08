#pragma once
#include <stdint.h>
#include <hwreg/dma.h>

#define HW_UART_WL_NUMERATOR       14
#define HW_UART_WL_DENOMINATOR     18
#define HW_UART_DMA_UNITS          2

struct hw_tap {                                  /* size=0x2c */
    struct hw_dma_ctrl             dma_ctrl[2];
    struct hw_tap_ctrl {                         /* size=0xc */
        uint16_t                   numerator;     /* @ 0x20 */
        uint32_t                   denominator;   /* @ 0x24 */
        uint8_t                    flags;         /* @ 0x28 */
#define UART_FLAGS_OBEY_CTS        0x01
#define UART_FLAGS_FORCE_RTS_MASK  0x06
#define UART_FLAGS_LOOPBACK        0x08
#define UART_FLAGS_TX_EMPTY        0x10
        uint16_t                   tx_size;
        uint16_t                   rx_size;
    } ctrl;
};
