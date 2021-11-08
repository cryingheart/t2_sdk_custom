#pragma once

#include <stdint.h>
#include <kernel/sd/io.h>

struct sdio_boot {
    struct card_state sb_card;
    union {
        /* 16 bytes in vendor reserved area of CIA */
        uint8_t sb_vendor_raw[16];
        struct  {
            uint32_t          sb_argc;
            uint32_t          sb_argv;
            uint32_t          sb_entry;
            uint8_t           sb_segment;
        };
    };
};

void boot_sdio_start(struct sdio_boot *sb);
