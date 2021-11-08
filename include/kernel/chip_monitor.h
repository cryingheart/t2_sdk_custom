#pragma once
#include <stdlib.h>
#include <kernel/os.h>
#include <kernel/io.h>
#include <kernel/callout.h>
#include <kernel/workqueue.h>

/**
 * @addtogroup chip_monitor
 * @{
 */

/**
 * Chip Monitor Service
 */
enum chip_monitor_source {
    /** Temperature of chip core */
    CHIP_MSOURCE_CORE_TEMP = 0,
    /** Voltage of VBAT */
    CHIP_MSOURCE_VBAT,
    /** External ADC */
    CHIP_MSOURCE_ADC,
    /** Number of sources */
    CHIP_MSOURCE_NUM_SOURCES
};

/** @} */

enum energy_consumers {
    ENERGY_CONSUMER_CHIP_STARTUP,
    ENERGY_CONSUMER_HFC,
    ENERGY_CONSUMER_RF_RX,
    ENERGY_CONSUMER_RF_TX,
    NUM_ENERGY_CONSUMERS
};

struct energy_report {
    uint32_t time_start; /* Last start time read from os_systime(). Set to
                            zero when not running. */
    uint32_t time_total; /* Total time in micro seconds not including the time
                            from last start. */
    uint32_t current_when_on; /* mA */
};

struct chip_monitor_power;

struct chip_monitor_serv {
  enum chip_monitor_source source; /*service source*/
  uint32_t     threshold;        /*service threshold*/
  uint32_t     serv_time_delta;  /*sampling time*/
  uint32_t     last_sample_value;/*last sampled value*/
  void (*serv_callback)(struct chip_monitor_serv*, enum chip_monitor_source, uint32_t);
  struct callout delayed_clbk;
  struct os_work work;
  struct chip_monitor_power *power;
};

int chip_monitor_start(struct chip_monitor_serv* serv, enum chip_monitor_source  source,
                        uint32_t last_sample_value, uint32_t threshold, uint32_t time_delta,
                            void (*serv_callback)(struct chip_monitor_serv*, enum chip_monitor_source, uint32_t));

void chip_monitor_stop(struct chip_monitor_serv* serv);

/* Get the struct energy_report where the time and current shall be reported. */
struct energy_report * chip_monitor_get_report(enum energy_consumers e);
