#pragma once
#include <assert.h>
#include <stdint.h>
#include <kernel/partition.h>
#include <kernel/uuid.h>

#define BOOTSECTOR_ADDRESS 0

struct secure_boot {
    /**
     * HDA for 256 bits secret encoded to 2112 bytes
     */
    uint32_t hda[2112/4];
    /**
     * ECDSA public key
     */
    uint8_t ecdsa_pub[64];
    /**
     * Firmware encryption key 256 bits
     */
    uint8_t enc_key[32];
    /**
     * Emulated FUSE 256 bits
     */
    uint8_t fuse[32];
};

struct factory_code
{
    uint8_t padding[1];
    uint8_t checksum;
    uint8_t model[4];
    uint8_t hw_ver[2];
    uint8_t test_date[6];
    uint8_t test_sw_ver[4];
    uint8_t test_station_id[6];
};

struct device_info
{
    /**
     * Padding for 4-byte allignment
     */
    uint8_t padding[2];
    /**
     * CT calibration of 40MHz clock
     */
    uint8_t ct_cal[2];

    /**
     * Hardware address.
     */
    uint8_t            hwaddr[6];
    /**
     * BT Hardware address.
     */
    uint8_t            bt_hwaddr[6];
    /**
     * BT UUID.
     */
    struct uuid_128    uuid;
};

struct bootsector {
    /**
     * Secure boot storage.
     */
    struct secure_boot secure;
    /**
     * Partition table.
     */
    struct partition_table ptable;
    /**
     * Reserved storage
     */
    uint8_t            reserved[1668];
    /**
     * Factory Code
     */
    struct factory_code factorycode;
    /**
     * Device Info
     */
    struct device_info deviceinfo;
};

static_assert(sizeof(struct bootsector) == 4096,
              "Invalid size for struct boot_sector");
