/**
 * @addtogroup efuse
 * @{
 */
#pragma once
#include <stdint.h>

uint32_t os_efuse_read(unsigned int start_bit);
void os_efuse_write(unsigned int start_bit, uint32_t value);

/** @} */
