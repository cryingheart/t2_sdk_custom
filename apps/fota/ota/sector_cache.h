#include <kernel/flash.h>

#define PAGESIZE 256
#define SECTORSIZE 4096

struct sector_cache {
    uint32_t address;
    uint32_t lru;
    enum { EMPTY, VALID, DIRTY } state;
    uint8_t data[SECTORSIZE];
};

void sector_cache_flush(struct spi_mem_device *dev, struct sector_cache *c);
void sector_cache_flush_all(struct spi_mem_device *dev);
struct sector_cache * __irq __attribute__((__returns_nonnull__))
    sector_cache_find(struct spi_mem_device *dev, uint32_t address);
struct sector_cache*
    sector_cache_load(struct spi_mem_device *dev, uint32_t address);
void sector_cache_read(struct spi_mem_device *dev,
    unsigned int address, void *data, size_t len);
void sector_cache_write(struct spi_mem_device *dev,
    unsigned int address, const void *data, size_t len);
void sector_cache_init(void);