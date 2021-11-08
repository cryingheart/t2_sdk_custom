#pragma once
#include <stdint.h>

#define HW_WFE_HANDLER_WL_STATUS   8

struct hw_wfe_handler {
    uint8_t                        raw;
#define WFE_HANDLER_RAW_CONSOLE_TX_FULL 0x01
#define WFE_HANDLER_RAW_CIPHER_IDLE 0x02
#define WFE_HANDLER_RAW_I2C_EVT    0x04
#define WFE_HANDLER_RAW_WFE_TIMER  0x08
#define WFE_HANDLER_RAW_CONSOLE_TX_EMPTY 0x10
    uint8_t                        mask_1;
    uint8_t                        mask_0;
};
