#pragma once
#include <stdint.h>
#include <stdbool.h>

#define FMU_REFCLK  80000000
#define FMU_MAX_NUM (0xffffffffU / (FMU_REFCLK/1000))

enum os_fmu_source {
    FMU_SOURCE_RX_ILO_DIV32_OUT   = 0,
    FMU_SOURCE_RX_LNA_DIV64_OUT_2 = 1,
    FMU_SOURCE_RX_LNA_DIV64_OUT_3 = 2,
    FMU_SOURCE_DBB_40M            = 3,
    FMU_SOURCE_CK_160M_DIG        = 4,
    FMU_SOURCE_CK_160M_DPLL       = 5,
    FMU_SOURCE_MMD_OUT_2DIG       = 6,
    FMU_SOURCE_LFC                = 7,
    FMU_SOURCE_RCO                = 8,
};

void os_fmu_start(enum os_fmu_source src, uint32_t num);
bool os_fmu_ready(void);
uint32_t os_fmu_result(uint32_t timeout);
uint32_t os_fmu_frequency(uint32_t n_ref, uint32_t n_src);
