#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <kernel/os.h>
#include <kernel/hostio.h>
#include <kernel/spi-mem.h>
#include <kernel/interrupt.h>
#include <kernel/hwreg.h>
#include <kernel/secureboot.h>
#include <kernel/flash.h>
#include "gordon.h"
#include "flash-api.h"

void boot_sha256_init(SHA256_CTX *ctx);
void boot_sha256_update(SHA256_CTX *ctx, const void *buf, size_t len);
void boot_sha256_transform(SHA256_CTX *ctx);
void boot_sha256_final(SHA256_CTX *ctx, uint8_t *digest);

#define CHUNK 4096

static int __irq
dma_irq_dummy(uint32_t irqno, void *arg, struct cpu_context *ctx)
{
    return IRQ_HANDLED;
}

void spi_mem_read_section(struct spi_mem_device *dev, SHA256_CTX *ctx, uint32_t flash_addr, uint32_t data_addr, size_t len)
{
    uint8_t *data_ptr = (data_addr) ? (uint8_t*)data_addr : (uint8_t*)os_alloc(CHUNK);

    os_attach_irq(IRQ_CIPHER_RX, dma_irq_dummy, NULL);

    while(len) {
        size_t chunk = len;
        if (chunk>CHUNK)
            chunk = CHUNK;

        //spi_mem_read(dev, flash_addr, data_ptr, chunk);
        sector_cache_read(dev, flash_addr, data_ptr, chunk);
	boot_sha256_update(ctx, data_ptr, chunk);

        flash_addr += chunk;
        data_ptr   += (data_addr) ? chunk : 0;
        len -= chunk;
    }

    if (data_addr == 0)
        os_free(data_ptr);
}

struct pfrag * handle_flash_hash(uint32_t flash_addr, size_t len)
{
    struct pfrag *frag = pfrag_alloc(SHA256_DIGEST_LENGTH);

    if (frag) {
        SHA256_CTX sha_ctx;
        boot_sha256_init(&sha_ctx);

        struct spi_mem_device * spi_mem = os_flash_get_spi_dev();
        spi_mem_read_section(spi_mem, &sha_ctx, flash_addr, 0, len);

        boot_sha256_final(&sha_ctx, pfrag_insert_tail(frag, SHA256_DIGEST_LENGTH));
    }
    return frag;
}
