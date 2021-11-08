#pragma once
#include <stdint.h>

struct hw_icu {
    uint32_t                       raw;
    uint32_t                       mask_0;
    uint32_t                       mask_1;
    uint32_t                       mask_2;
    uint32_t                       wakeup;
    uint32_t                       irq_0;
    uint32_t                       irq_1;
    uint32_t                       irq_2;
    uint32_t                       overrun;
};
