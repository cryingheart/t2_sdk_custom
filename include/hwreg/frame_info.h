#pragma once
#include <stdint.h>

struct hw_frame_info {
    uint16_t                       sync_word;
    uint16_t                       frame_tag;
};
