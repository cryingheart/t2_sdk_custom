#pragma once
#include <stdint.h>

struct hw_tx_bb {
    struct hw_tx_bb_ctrl {
        uint8_t                    packet_start;
    } ctrl;
    struct hw_tx_bb_tx_vec {
        uint8_t                    format;
        uint8_t                    ppdu_format;
        uint16_t                   l_length;
        uint8_t                    l_rate_mode;
        uint16_t                   service;
        uint8_t                    smoothing;
        uint8_t                    not_sounding;
        uint8_t                    aggregation;
        uint8_t                    locked_clock;
        uint8_t                    mcs;
        uint16_t                   length;
    } tx_vec;
    struct hw_tx_bb_scr_11b {
        uint8_t                    seed_init;
        uint8_t                    seed;
    } scr_11b;
};
