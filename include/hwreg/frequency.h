#pragma once
#include <stdint.h>

struct hw_frequency {
    uint8_t                        ctrl;
#define FREQUENCY_CTRL_START       0x01
#define FREQUENCY_CTRL_IDLE        0x02
    uint32_t                       counter_cmp;
    uint32_t                       counter_1;
    uint32_t                       counter_2;
};
