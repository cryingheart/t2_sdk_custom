#include <stdint.h>
#include <string.h>
#include <kernel/flash.h>
#include <assert.h>


#define PAGESIZE 256
#define SECTORSIZE 4096

struct sector_cache {
    uint32_t address;
    uint32_t lru;
    enum { EMPTY, VALID, DIRTY } state;
    uint8_t data[SECTORSIZE];
};

static struct sector_cache *sector_cache;
static size_t sector_cache_size;
static uint8_t worksector[SECTORSIZE];

#define MUST_ERASE ~0u

/* return bitmask of which pages need to be programmed to result in
 * NEW, given content on flash in OLD, return MUST_ERASE if erase is
 * necessary */
static unsigned int
check_program_state(const unsigned char *new,
                    const unsigned char *old,
                    size_t len)
{
    unsigned int program = 0;
    unsigned int offset = 0;

    for(offset = 0; offset < len; offset++) {
        if(~new[offset] & old[offset])
            /* at least one 1 -> 0 transition: need to program page */
            program |= BIT(offset / PAGESIZE);
        if(new[offset] & ~old[offset])
            /* at least one 0 -> 1 transition: need to erase */
            return MUST_ERASE;
    }

    return program;
}

/* return bitmask of which pages need to be programmed in an erased
 * sector to result in SECTOR data on flash */
static unsigned int
check_erased_state(const unsigned char *sector, size_t len)
{
    unsigned int program = 0;
    unsigned int offset;

    for(offset = 0; offset < len; offset++) {
        if(sector[offset] != 0xff) {
            /* at least one zero bit: need to program page */
            program |= BIT(offset / PAGESIZE);
            /* skip rest of page */
            offset = ALIGN(offset + 1, PAGESIZE);
            continue;
        }
        offset++;
    }

    return program;
}

/* program page PAGE_INDEX of sector starting at SECTOR_ADDRESS with
 * data at relevant offset into SECTOR */
static void
page_program(struct spi_mem_device *dev,
             unsigned int sector_address,
             unsigned int page_index,
             const unsigned char *sector)
{
    spi_mem_write(dev, sector_address + page_index * PAGESIZE,
                  &sector[page_index * PAGESIZE], PAGESIZE);
}

/* make sector at SECTOR_ADDRESS contain data from SECTOR and LEN,
 * other data is left intact */
static int
sector_program(struct spi_mem_device *dev,
               unsigned int sector_address,
               const unsigned char *sector, size_t len)
{
    unsigned int program;

    assert(len <= SECTORSIZE);
    assert((sector_address & (SECTORSIZE-1)) == 0);

    spi_mem_read(dev, sector_address, worksector, SECTORSIZE);

    /* compare to desired content */
    program = check_program_state(sector, worksector, len);
    memcpy(worksector, sector, len);

    if(program == MUST_ERASE) {
        pr_info("Erasing sector %u\n", sector_address / SECTORSIZE);
        spi_sector_erase(dev, sector_address);
        program = check_erased_state(worksector, SECTORSIZE);
    }

    if(program != 0) {
        for(unsigned page_index = 0; page_index < SECTORSIZE / PAGESIZE; page_index++) {
            if((program & BIT(page_index)) != 0) {
                pr_info("%2u ", page_index);
               page_program(dev, sector_address, page_index, worksector);
            }
        }
        pr_info("\n");
    }

    return 0;
}

static void
sector_cache_flush(struct spi_mem_device *dev, struct sector_cache *c)
{
    //os_printf("sector_cache_flush\n");
    if(c->state != DIRTY)
        return;

    sector_program(dev, c->address, c->data, SECTORSIZE);
    c->state = VALID;
}

void
sector_cache_flush_all(struct spi_mem_device *dev)
{
    for(size_t idx = 0; idx < sector_cache_size; idx++)
        sector_cache_flush(dev, &sector_cache[idx]);
}

static struct sector_cache * __irq __attribute__((__returns_nonnull__))
sector_cache_find(struct spi_mem_device *dev, uint32_t address)
{
    //os_printf("sector_cache_find\n");
    struct sector_cache *valid = NULL;
    struct sector_cache *dirty = NULL;
    //os_wait_usec(8000);
    for(size_t idx = 0; idx < sector_cache_size; idx++) {
        struct sector_cache *c = &sector_cache[idx];
        if(c->state == EMPTY)
            return c;
        if(c->state == VALID
           && (valid == NULL || time_before(c->lru, valid->lru)))
            valid = c;
        if(c->state == DIRTY
           && (dirty == NULL || time_before(c->lru, dirty->lru)))
            dirty = c;
    }
    if(valid != NULL)
    {
        return valid;
    }

    assert(dirty != NULL);
    return dirty;
}

static struct sector_cache*
sector_cache_load(struct spi_mem_device *dev, uint32_t address)
{
    uint32_t sector_address = address & ~(SECTORSIZE-1);
    struct sector_cache *c;

    c = sector_cache_find(dev, sector_address);
    if(c->address != sector_address)
    {
        sector_cache_flush(dev, c);
    }

    if(c->address != sector_address || c->state == EMPTY) {
        spi_mem_read(dev, sector_address, c->data, SECTORSIZE);

        c->address = sector_address;
        c->state = VALID;
    }
    return c;
}

void sector_cache_read(struct spi_mem_device *dev,
                       unsigned int address, void *data, size_t len)
{
    struct sector_cache *c;

    while(len > 0) {
        uint32_t offset = address % SECTORSIZE;
        size_t n;

        c = sector_cache_load(dev, address);
        n = min(len, SECTORSIZE-offset);

        memcpy(data, c->data + offset, n);
        data += n;
        len -= n;
        address += n;
        c->lru = os_systime();
    }
}

void
sector_cache_write(struct spi_mem_device *dev,
                   unsigned int address, const void *data, size_t len)
{
    struct sector_cache *c;
    while(len > 0) {
        uint32_t offset = address % SECTORSIZE;
        size_t n;

        c = sector_cache_load(dev, address);
        n = min(len, SECTORSIZE-offset);

        memcpy(c->data + offset, data, n);
        data += n;
        len -= n;
        address += n;
        c->state = DIRTY;
        c->lru = os_systime();
    }
}

BOOTARG_INT("sector_cache", "num", "size (in sectors) of sector cache");

void
sector_cache_init(void)
{
    int size = os_get_boot_arg_int("sector_cache", 1);
    if(size < 1) {
        size = 1;
        pr_warn("unreasonable value for \"sector_cache\", using %d\n", size);
    }

    sector_cache = os_zalloc(size * sizeof(*sector_cache));
    if(sector_cache == NULL) {
        pr_warn("no memory for %d sector cache entries\n", size);
        if(size > 1) {
            size = 1;
            sector_cache = os_zalloc(size * sizeof(*sector_cache));
        }
        //os_printf("OS_ERROR_ON_NULL:%x\n", sector_cache);
    }
    sector_cache_size = size;
}
