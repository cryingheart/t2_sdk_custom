#pragma once
#include <stdint.h>

struct hw_event_handler {
    struct hw_event_handler_ctrl {
        uint32_t                   raw[2];
        uint32_t                   raw_set[2];
        uint32_t                   raw_clr[2];
        struct hw_event_handler_ctrl_mask {
            uint32_t               lo;
            uint32_t               hi;
        } mask[4];
    } ctrl;
    struct hw_event_handler_event {
        uint8_t                    id;
        uint8_t                    tsf;
        uint32_t                   timestamp;
    } event[6];
};
