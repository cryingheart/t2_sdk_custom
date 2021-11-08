
#pragma once
#include <stdint.h>
#include <string.h>
#include <kernel/flash.h>
#include <assert.h>


#define PAGESIZE 256
#define SECTORSIZE 4096
void
sector_cache_flush_all(struct spi_mem_device *dev);

void sector_cache_read(struct spi_mem_device *dev,
                       unsigned int address, void *data, size_t len);

void
sector_cache_write(struct spi_mem_device *dev,
                   unsigned int address, const void *data, size_t len);

void
sector_cache_init(void);
