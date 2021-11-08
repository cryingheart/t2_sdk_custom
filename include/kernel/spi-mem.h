#pragma once
/**
 * @addtogroup drv_spimem
 * @{
 */
#include <kernel/spi.h>
#include <kernel/uuid.h>
#include <kernel/mutex.h>

/* typical commands */
#define SPIFLASH_OP_WREN        0x06 /* Write enable */
#define SPIFLASH_OP_WRDIS       0x04 /* Write disable */
#define SPIFLASH_OP_RDSR        0x05 /* Read status register */
#define SPIFLASH_OP_RCR         0x35 /* Read 2nd status register */
#define SPIFLASH_OP_WRSR        0x01 /* Write status register 1 (2) byte */
#define SPIFLASH_OP_READ        0x03 /* Read data bytes (low frequency) */
#define SPIFLASH_OP_READ_FAST_2 0x3b /* Read data bytes (Dual SPI) */
#define SPIFLASH_OP_READ_FAST_4 0x6b /* Read data bytes (Quad SPI) */
#define SPIFLASH_OP_PP          0x02 /* Page program (up to 256 bytes) */
#define SPIFLASH_OP_BE_4K       0x20 /* Erase 4KiB block */
#define SPIFLASH_OP_CHIP_ERASE  0xc7 /* Erase whole flash chip */
#define SPIFLASH_OP_RDID        0x9f /* Read JEDEC ID */
#define SPIFLASH_OP_RSFDP       0x5a
#define SPIFLASH_OP_PES         0x75 /* Program/Erase Suspend */
#define SPIFLASH_OP_PER         0x7a /* Program/Erase Resume */
#define SPIFLASH_OP_RUID        0x4b /* Read Unique ID */

/* about the only thing flash manufacturers have agreed on is that bit
 * zero is write-in-progress */
#define SPIFLASH_STATUS_WIP     BIT(0) /* Write in progress */

struct spi_mem_param_cmd {
    uint8_t opcode;
    uint8_t mode_cycles;
    uint8_t dummy_cycles;
};

/**
 * Memory device characteristics
 */
struct spi_mem_params {
    unsigned int mp_idcode;
    unsigned int mp_flags;
#define SPI_MP_QER_PRESENT BIT(0) /* quad enable requirements */
#define SPI_MP_QER_MASK    (BITMASK(3) << 1)
    /* bits 1,2,3 used by QER */

    unsigned int mp_page_size;
    unsigned int mp_page_count;
    unsigned int mp_page_program_time;
    /**/
    unsigned int mp_sector_size;
    unsigned int mp_sector_erase_time;
    /**/
    unsigned int mp_bulk_erase_time;
    /**/
    uint8_t mp_sector_erase_instr;
    uint8_t mp_read_sr2_instr;
    uint8_t mp_suspend_instr;
    uint8_t mp_resume_instr;
    uint16_t mp_suspend_time;

    struct spi_mem_param_cmd mp_read_112;
    struct spi_mem_param_cmd mp_read_114;
};

/**
 * SPI (Flash) memory device.
 */
struct spi_mem_device {
    /** Every memory device is also a @ref spi_device. */
    struct spi_device     sm_dev;
    struct os_mutex       sm_lock;
    /** Flags for the state of this device. */
    unsigned int          sm_flags;
#define SPI_MEM_WIP    BIT(0)
#define SPI_MEM_SUSP   BIT(1)
#define SPI_MEM_ATOMIC BIT(2)  /* Applies only to read/write/erase operations */
#define SPI_MEM_DUAL   BIT(3)
#define SPI_MEM_QUAD   BIT(4)
    /** Hint of the amount of time it takes to complete a page write. */
    unsigned int          sm_write_complete_time;
    unsigned int          sm_write_time_remaining;
    unsigned int          sm_write_region_start;
    unsigned int          sm_write_region_end;
    /** Device characteristics. */
    struct spi_mem_params sm_params;
};

static inline size_t spi_mem_get_idcode(const struct spi_mem_device *dev)
{
    return dev->sm_params.mp_idcode;
}

static inline size_t spi_mem_get_page_size(const struct spi_mem_device *dev)
{
    return dev->sm_params.mp_page_size;
}

static inline size_t spi_mem_get_page_count(const struct spi_mem_device *dev)
{
    return dev->sm_params.mp_page_count;
}

static inline size_t spi_mem_get_sector_size(const struct spi_mem_device *dev)
{
    return dev->sm_params.mp_sector_size;
}

static inline size_t spi_mem_get_sector_count(const struct spi_mem_device *dev)
{
    unsigned int pps;
    pps = spi_mem_get_sector_size(dev) / spi_mem_get_page_size(dev);
    return spi_mem_get_page_count(dev) / pps;
}

static inline void spi_mem_set_atomic(struct spi_mem_device *dev)
{
    dev->sm_flags |= SPI_MEM_ATOMIC;
}

static inline bool spi_mem_valid_instr(uint8_t instr)
{
    return instr != 0 && instr != 0xff;
}

static inline bool spi_mem_write_in_progress(struct spi_mem_device* dev)
{
    return dev->sm_write_region_start != dev->sm_write_region_end;
}

/* BEGIN GENERATED PROTOS */

void
spi_mem_read(
    struct spi_mem_device *dev,
    unsigned int address,
    void *data,
    size_t len);

void
spi_mem_read_sfdp(
    struct spi_mem_device *dev,
    unsigned int address,
    void *data,
    size_t len);

void
spi_mem_write(
    struct spi_mem_device *dev,
    unsigned int address,
    const void *data,
    size_t len);

void
spi_sector_erase(
    struct spi_mem_device *dev,
    unsigned int address);

/* END GENERATED PROTOS */

/** @} */
