#pragma once
#ifndef __ASSEMBLER__

/**
 * @ingroup time
 *
 * Identifiers for the four available timer counters.
 */
enum timer_base {
    TIMER_BASE_US,  /**< System clock (1 MHz) */
    TIMER_BASE_BT,  /**< Bluetooth clock (1.6384 MHz) */
    TIMER_BASE_APP, /**< Application use */
    TIMER_BASE_HFC  /**< Hardware clock (80 MHz) */
};

#endif
