#pragma once
#include <stdint.h>
#include <kernel/secureboot.h>

#define PAGESIZE 256
#define SECTORSIZE 4096

struct spi_mem_device;

int
enroll(struct secure_boot *bb, uint8_t mode, const uint8_t *secret,
       const uint8_t *public_key, const uint8_t *cipher_key);

void sector_cache_flush_all(struct spi_mem_device* dev);

void sector_cache_erase_sector(struct spi_mem_device* dev,
                               uint32_t address);

void sector_cache_init(void);

void
sector_cache_read(struct spi_mem_device* dev,
                  unsigned int address,
                  void* data,
                  size_t len);

void
sector_cache_write(struct spi_mem_device* dev,
                   unsigned int address,
                   const void* data,
                   size_t len);

struct pfrag * handle_flash_hash(uint32_t flash_addr, size_t len);
