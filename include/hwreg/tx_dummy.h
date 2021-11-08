#pragma once
#include <stdint.h>

struct hw_tx_dummy {
    struct hw_tx_dummy_dummy_mac {
        uint8_t                    en;
        uint32_t                   seed;
        uint32_t                   n_packet;
        uint8_t                    psdu_gen_mode;
        uint32_t                   packet_gap_min;
        uint32_t                   packet_gap_max;
        uint32_t                   packet_gap_step;
        uint8_t                    mcs_index_min;
        uint8_t                    mcs_index_max;
        uint8_t                    mcs_index_step;
        uint16_t                   psdu_len_min;
        uint16_t                   psdu_len_max;
        uint16_t                   psdu_len_step;
        uint8_t                    format_min;
        uint8_t                    format_max;
        uint8_t                    format_step;
        uint8_t                    service;
        uint8_t                    ppdu_format;
        uint8_t                    locked_clock;
        uint8_t                    smoothing;
        uint8_t                    not_sounding;
        uint8_t                    aggregation;
        uint8_t                    num_exten_ss;
        uint8_t                    fec_coding;
        uint8_t                    stbc;
        uint8_t                    gi_type;
        uint8_t                    ch_bandwidth;
    } dummy_mac;
    struct hw_tx_dummy_cflt {
        uint8_t                    en;
        uint8_t                    beta;
        uint8_t                    gain;
        uint8_t                    shft;
    } cflt;
};
