/**
 * @addtogroup wifi_core
 * @{
 */
#pragma once

#include <stdint.h>

/**
 * Link status counters
 */
struct wifi_counters {
    /** Successfully transmitted bytes */
    uint64_t txbytes;
    /** Successfully transmitted frames */
    uint32_t txframe;
    /** Successfully transmitted multicast frames */
    uint32_t txmframe;
    /** Transmitted frames without ACK */
    uint32_t txnoack;
    /** Unsuccessfully transmitted frames */
    uint32_t txfail;
    /** Transmissions with at least one retry */
    uint32_t txretry;
    /** Transmissions with multiple retries */
    uint32_t txretries;

    /** Successfully received bytes */
    uint64_t rxbytes;
    /** Successfully received frames */
    uint32_t rxframe;
    /** Successfully multicast received frames */
    uint32_t rxmframe;
    /** Frames duplicates */
    uint32_t rxdup;
    /** Frames with FCS error */
    uint32_t rxfcserr;
};

/** @} */
