#pragma once
#include <kernel/hwreg.h>
#include <kernel/io.h>

static inline void
device_request(uint32_t mask)
{
    mmio_wr16_set(mask, &hw_pmu->reset_unit.device[0]);

}

static inline void
device_release(uint32_t mask)
{
    mmio_wr16_clr(mask, &hw_pmu->reset_unit.device[0]);
}

static inline void
device_reset(uint32_t mask)
{
    mmio_wr16_clr(mask, &hw_pmu->reset_unit.device[0]);
    mmio_wr16_set(mask, &hw_pmu->reset_unit.device[0]);
}
