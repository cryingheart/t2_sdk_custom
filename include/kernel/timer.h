#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <kernel/compiler.h>
#include <bits/timer_base.h>

/**
 * @addtogroup time
 * @{
 */

typedef unsigned int os_timer_id_t;
#define TIMER_INVALID ~0U
#define TIMER_CALLOUT  0
#define TIMER_KERNEL   1
#define TIMER_WFE      14
#define TIMER_NAV      15
#define TIMER_ANY      TIMER_INVALID

/**
 * Signature for timer callback functions
 */
typedef void (*timer_callback_t)(void *userdata);

os_timer_id_t os_timer_allocate(enum timer_base base, unsigned idx,
                                timer_callback_t callback, void *userdata);
void os_timer_wakeup_enable(os_timer_id_t timer);
void os_timer_wakeup_disable(os_timer_id_t timer);
uint32_t os_timer_read(os_timer_id_t timer);
uint64_t os_timer_read64(os_timer_id_t timer);
void os_timer_release(os_timer_id_t timer);
void os_timer_set(os_timer_id_t timer, uint32_t ticks);
void os_timer_reset(os_timer_id_t timer);
bool os_timer_running_p(os_timer_id_t timer);
uint32_t os_timer_next_timeout(uint32_t now);
uint32_t timer_timebase_to_lfc(uint64_t ticks, enum timer_base newbase);
uint64_t timer_lfc_to_timebase(uint32_t lfc_ticks, enum timer_base newbase);

uint32_t os_get_total_sleep_cycles(void);

/** @} */
