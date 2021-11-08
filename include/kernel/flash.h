#pragma once
#include <kernel/spi-mem.h>
#include <kernel/partition.h>

void os_flash_init(void);
struct spi_mem_device * os_flash_get_spi_dev(void);

const struct media_part*
os_flash_get_partition(enum media_part_type ptype);

struct mtd_dev_s;
struct mtd_dev_s* os_flash_get_mtd_dev(void);

int os_flash_create_mtd_partition(const char *devname, const struct media_part *mp);
int os_flash_create_mtd_partition_by_type(const char *devname, enum media_part_type ptype);
