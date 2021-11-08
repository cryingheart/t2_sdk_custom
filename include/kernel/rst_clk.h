#pragma once
#include <stdint.h>
#include <kernel/hwreg.h>
#include <kernel/io.h>

/**
 * Enable both rst and clk_en for blocks specified via bitmask.
 */
static inline void
rst_clk_enable(uint32_t mask)
{
    mmio_wr32(mask | (mask << 16), &hw_rstclk->rstclk.set);
}

/**
 * Disable both rst and clk_en for blocks specified via bitmask.
 */
static inline void
rst_clk_disable(uint32_t mask)
{
    mmio_wr32(mask | (mask << 16), &hw_rstclk->rstclk.clr);
}
