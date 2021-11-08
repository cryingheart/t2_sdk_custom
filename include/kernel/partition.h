#pragma once
#include <stdbool.h>
#include <stdint.h>

struct media_part {
    uint32_t entry[2];
#define PART_ENTRY_0_SECTOR_START 0x000fffff
#define PART_ENTRY_0_TYPE         0x01f00000
#define PART_ENTRY_0_RESERVED     0x7e000000
#define PART_ENTRY_0_ACTIVE       0x80000000 /* 0 == deleted */
#define PART_ENTRY_1_SECTOR_COUNT 0x000fffff
#define PART_ENTRY_1_RESERVED     0xfff00000
};

#define SYSFS_DEVNAME "/dev/sysfs"
#define SYSFS_MOUNTPT "/sys"

#define DATA_DEVNAME "/dev/data"
#define DATA_MOUNTPT "/data"

enum media_part_type {
    media_part_type_sysfs   = 14,
    media_part_type_data    = 15,
    media_part_type_panic   = 27,
    media_part_type_virt    = 29,
    media_part_type_boot    = 30,
    media_part_type_unused  = 31,
};

int os_mount(enum media_part_type ptype, const char* target, bool force_format);
int os_mount_sysfs(void);
int os_mount_data(void);

struct partition_table {
    uint32_t          header;
#define PARTITION_TABLE_MAGIC 0xff7c4170
#define PARTITION_TABLE_SIZE  16
    struct media_part part[PARTITION_TABLE_SIZE];
};

/* BEGIN GENERATED PROTOS */

bool media_part_get_active(const struct media_part* p);

uint32_t media_part_get_sector_count(const struct media_part* p);

uint32_t media_part_get_sector_start(const struct media_part* p);

enum media_part_type media_part_get_type(const struct media_part* p);

void media_part_init(struct media_part* p);

bool media_part_is_active(const struct media_part* p);

bool media_part_is_free(const struct media_part* p);

void media_part_set_active(struct media_part* p, bool active);

void media_part_set_sector_count(struct media_part* p, uint32_t count);

void media_part_set_sector_start(struct media_part* p, uint32_t start);

void
media_part_set_type(struct media_part* p,
                    enum media_part_type type);

/* END GENERATED PROTOS */
