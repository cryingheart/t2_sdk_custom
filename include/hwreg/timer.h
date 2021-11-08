#pragma once
#include <stdint.h>

#define HW_TIMER_COUNTER_UNITS     3
#define HW_TIMER_MATCH_UNITS       16
#define HW_TIMER_WL_PRESCALER      16

struct hw_timer {
    struct hw_timer_counter {
        uint16_t                   denominator;
        uint16_t                   numerator;
        uint8_t                    run_ctrl;
#define TIMER_RUN_CTRL_RUN         0x01
#define TIMER_RUN_CTRL_STOP_ON_DBG 0x02
        uint32_t                   cntr[2];
        uint32_t                   nudge[2];
    } counter[3];
    struct hw_timer_sys {
        uint16_t                   denominator;
        uint16_t                   numerator;
        uint8_t                    run_ctrl;
#define TIMER_RUN_CTRL_RUN         0x01
#define TIMER_RUN_CTRL_STOP_ON_DBG 0x02
        uint32_t                   cntr[2];
    } sys;
    struct hw_timer_match_unit {
        uint32_t                   match;
        uint8_t                    source;
        uint8_t                    irq;
        uint8_t                    enable;
    } match_unit[16];
    struct hw_timer_match_status {
        uint16_t                   expired;
        uint16_t                   status[4];
    } match_status;
};
