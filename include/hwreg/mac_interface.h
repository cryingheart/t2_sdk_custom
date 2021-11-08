#pragma once
#include <stdint.h>

struct hw_mac_interface {
    struct hw_mac_interface_rx_stream {
        uint8_t                    start_11b;
        uint8_t                    start_ofdm;
        uint16_t                   length;
        uint8_t                    pre_fi_selection;
        uint8_t                    post_fi_selection;
    } rx_stream;
};
