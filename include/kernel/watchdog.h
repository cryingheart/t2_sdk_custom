/**
 * @addtogroup drv_watchdog
 * @{
 */
#pragma once
#include <stdint.h>

uint32_t watchdog_init(uint32_t timeout_us, void (*expire_cb)(void));
void watchdog_start(void);
void watchdog_stop(void);
void watchdog_kick(void);
uint32_t watchdog_get_timeout(void);
uint32_t watchdog_get_counter(void);

/** @} */
