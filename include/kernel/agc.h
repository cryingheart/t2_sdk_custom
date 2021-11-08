#pragma once
#include <stdint.h>
#include <kernel/rfdrv.h>

void agc_init(void);
void agc_set_wifi(uint32_t ch_freq);
void agc_set_blue(uint32_t ch_freq);
int8_t agc_get_rssi(void);

extern uint8_t agc_gain_max;
extern uint8_t agc_pow0_gain_target;
extern uint8_t agc_pow0_gain_error_threshold;
extern int8_t  agc_ed_threshold;
