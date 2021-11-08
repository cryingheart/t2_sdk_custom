#pragma once
#include <kernel/hwreg.h>
#include <kernel/io.h>

static inline void wfe_wait(uint8_t mode)
{
    mmio_wr8(mode, &hw_wfe->mask_1);
    do {
        asm("wfe");
    } while ((mmio_rd8(&hw_wfe->raw) & mode) == 0);
}
