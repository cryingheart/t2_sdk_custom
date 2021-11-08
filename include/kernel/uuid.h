#pragma once
#include <stdint.h>

struct uuid_128 {
    uint8_t uuid[16];
};

enum uuid_type {
    /** HW based UUID - current implementation read the HW uuid from spi flash */
    UUID_SCOPE_HW,
    /** SW based UUID - from struct device_info */
    UUID_SCOPE_USER,
};

int os_uuid_get(enum uuid_type scope, struct uuid_128 *uuid);
