#pragma once
#include <stdint.h>

struct hw_wdg {
    uint16_t                       preload;
    uint16_t                       count;
    uint8_t                        running;
    uint8_t                        unlock;
    uint8_t                        run;
    uint8_t                        stop;
    uint8_t                        kick;
    uint8_t                        trip;
#define WDG_TRIP_IRQ               0x01
#define WDG_TRIP_RESET             0x02
#define WDG_TRIP_ENABLE_PAUSE      0x04
};
