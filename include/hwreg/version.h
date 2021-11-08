#pragma once
#include <stdint.h>

#define HW_VERSION_IMPL_CHIP       0
#define HW_VERSION_IMPL_FPGA       1
#define HW_VERSION_IMPL_FAKE       2
#define HW_VERSION_PLATFORM_HW     0
#define HW_VERSION_PLATFORM_HDL    1
#define HW_VERSION_PLATFORM_NSIM   2

struct hw_version {
    uint8_t                        date[32];
    uint8_t                        dirty;
    uint8_t                        rev[41];
    uint8_t                        impl;
    uint8_t                        platform;
};
