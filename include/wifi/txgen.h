#pragma once
#include <stdint.h>
#include <kernel/compiler.h>
#include <wifi/rates.h>

#define TXGEN_MODE_RANDOM   0
#define TXGEN_MODE_SEQUENCE 1
#define TXGEN_MODE_ZERO     2

struct txgen {
    uint8_t  mode;
    struct rf_txpower *txpower;
    uint16_t freq;
    uint16_t len;
    rate_t   hwrate;
    uint32_t num;
    uint32_t gap;
};

void __entrypoint cop_txgen_start(const struct txgen *param);
void __entrypoint cop_rf_tone(const struct txgen *param, const uint32_t dpa_power);
