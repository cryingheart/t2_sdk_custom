#pragma once
#include <stdint.h>

struct hw_rnd {
    uint32_t                       lfsr;
    uint32_t                       seed;
    uint8_t                        entropy;
};
