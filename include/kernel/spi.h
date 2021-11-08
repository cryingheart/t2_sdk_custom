/**
 * @addtogroup drv_spi
 * @{
 */
#pragma once
#include <kernel/dma.h>
#include <kernel/bitops.h>

/**
 * Type of the object representing a SPI bus.
 */
struct spi_bus {
    struct os_semaphore  sb_sem;
    struct os_waitq      sb_waitq;
    struct spi_device   *sb_bus_holder;
};


/**
 * Type for objects representing a SPI device attached to @ref spi_bus.
 */
struct spi_device {
    struct spi_bus *sd_bus;
    bool            sd_open;
    unsigned int    sd_device_id;
    unsigned int    sd_max_freq;
    unsigned int    sd_mode;
};

struct spi_req {
    uint8_t  sr_cmd_mode;
    uint8_t  sr_cmd;
    uint8_t  sr_addr_mode;
    uint8_t  sr_addr_bits;
    uint32_t sr_addr;
    uint8_t  sr_mode_bits;
    uint8_t  sr_dummy_bits;
    uint8_t  sr_data_mode;
};


void spi_acquire_bus(struct spi_bus* bus, struct spi_device* dev);

void spi_bus_init(struct spi_bus* bus);

void spi_close_device(struct spi_device* dev);

void
spi_init_device(struct spi_device* dev,
                struct spi_bus* bus,
                unsigned int device_id,
                unsigned int max_freq,
                unsigned int mode);

void spi_open_device(struct spi_device* dev);

unsigned int spi_read_shift_reg(struct spi_bus* bus);

void spi_release_bus(struct spi_bus* bus);

void spi_set_device_id(struct spi_bus* bus, unsigned int device_id);

void spi_set_freq(struct spi_bus* bus, unsigned int max_freq);

void spi_set_mode(struct spi_bus* bus, unsigned int mode);

void spi_wait(struct spi_bus* bus);

void spi_wr32(struct spi_device* dev, uint32_t out);

void spi_wr24(struct spi_device* dev, uint32_t out);

void spi_wr16(struct spi_device* dev, uint32_t out);

void spi_wr8(struct spi_device* dev, uint8_t out);

void spi_wr8_rd32(struct spi_device* dev, uint8_t out, uint32_t* in);

void spi_wr8_rd8(struct spi_device* dev, uint8_t out, uint8_t* in);

void spi_wr8_rd16(struct spi_device* dev, uint8_t out, uint16_t *in);

void spi_xfer_dma(struct spi_device *dev,
                  const struct spi_req *req,
                  struct dma_desc *data);

/** @} */
