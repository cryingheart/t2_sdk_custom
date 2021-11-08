#pragma once
#include <stdint.h>

#define HW_BACKOFF_XIFS_UNITS      4
#define HW_BACKOFF_BACKOFF_UNITS   10

struct hw_backoff {
    struct hw_backoff_xifs {
        uint8_t                    sifs_time;
        uint8_t                    slot_time;
        uint8_t                    slots;
    } xifs[4];
    struct hw_backoff_cntr {
        uint8_t                    xifs;
        uint8_t                    enable;
        uint16_t                   value;
    } cntr[10];
    struct hw_backoff_ctrl {
        uint8_t                    soft_cca;
        uint8_t                    hold;
        uint8_t                    early_advance;
    } ctrl;
    struct hw_backoff_status {
        uint8_t                    busy;
#define BACKOFF_BUSY_CCA           0x01
#define BACKOFF_BUSY_NAV           0x02
#define BACKOFF_BUSY_SOFT          0x04
#define BACKOFF_BUSY_HOLD          0x08
        uint16_t                   candidates;
        uint16_t                   early;
    } status;
};
