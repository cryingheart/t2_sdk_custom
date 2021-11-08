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

static bool   spi_initialized;

static_assert(SECTORSIZE / PAGESIZE < 32,
              "there are more than 32 pages per sector");

#define __weak __attribute__((__weak__))

static struct packet *
flash_identify(void *ctx, struct packet *msg)
{
    uint32_t status = 0;
    uint32_t idcode = ~0U;
    uint32_t capacity = 0;
    struct spi_mem_device *spi_mem = os_flash_get_spi_dev();

    if(spi_mem == NULL) {
        pr_err("Failed to detect SPI memory device on address zero\n");
        status = ENODEV;
    } else if(spi_mem_get_sector_size(spi_mem) != SECTORSIZE) {
        os_printf("This device has a sectorsize of %u bytes\n",
                  spi_mem_get_sector_size(spi_mem));
        os_printf("but this program only works with sectorsize of %u.\n",
                  SECTORSIZE);
        status = EINVAL;
    } else if(spi_mem_get_page_size(spi_mem) != PAGESIZE) {
        os_printf("This device has an unusual pagesize of %u bytes\n",
                  spi_mem_get_sector_size(spi_mem));
        status = EINVAL;
    } else {
        idcode = spi_mem_get_idcode(spi_mem);
        capacity = spi_mem_get_page_count(spi_mem);
    }

    spi_initialized = status == 0;

    return create_flash_identify_rsp(status, PAGESIZE, SECTORSIZE, capacity, idcode);
}

static struct packet *
flash_read(void *ctx, struct packet *msg)
{
    struct packet *pkt; /* response packet */

    if(!spi_initialized) {
        pkt = hio_response_status(EINVAL);
    } else {
        struct flash_read_req *req = packet_data(msg);
        struct pfrag *frag = pfrag_alloc(req->length);
        struct spi_mem_device * spi_mem = os_flash_get_spi_dev();
        sector_cache_read(spi_mem, req->address, pfrag_insert_tail(frag, req->length), req->length);
        pkt = create_flash_read_rsp(0, NULL);
        packet_add_frag(pkt, frag);
    }

    return pkt;
}

static struct packet *
flash_write(void *ctx, struct packet *msg)
{
    struct flash_write_req *req = packet_data(msg);
    uint32_t status = 0;

    if(!spi_initialized) {
        status = EINVAL;
    } else {
        struct spi_mem_device * spi_mem = os_flash_get_spi_dev();
        if(req->length == 0)
            /* this is for backwards compatibility */
            sector_cache_flush_all(spi_mem);
        else
            sector_cache_write(spi_mem, req->address, req->data, req->length);
    }

    return hio_response_status(status);
}


static struct packet *
flash_enroll(void *ctx, struct packet *msg)
{
    struct flash_enroll_req *req = packet_data(msg);
    uint32_t status = 0;
    struct bootsector *boot_block = (void*)0x71000;

    if(!spi_initialized) {
        status = EINVAL;
    } else {
        status = enroll(&boot_block->secure,
                        req->secureboot_mode,
                        req->secret,
                        req->ecdsa_key,
                        req->fw_key);
        if(status == 0) {
            struct spi_mem_device * spi_mem = os_flash_get_spi_dev();
            sector_cache_write(spi_mem, BOOTSECTOR_ADDRESS,
                               boot_block, sizeof(*boot_block));
            sector_cache_flush_all(spi_mem);
        }
    }

    return hio_response_status(status);
}

/* the filesystem support, is stubbed, to support implement api below */

int __weak handle_flash_file_mount(uint32_t addr, uint32_t size)
{
    return EINVAL;
}

int __weak handle_flash_file_open(char* path, uint8_t mode)
{
    return EINVAL;
}

int __weak handle_flash_file_remove(char* path)
{
    return EINVAL;
}

struct pfrag * __weak handle_flash_file_read(size_t length)
{
    return NULL;
}

int __weak handle_flash_file_write(uint8_t *data, size_t length)
{
    return EINVAL;
}

int __weak handle_flash_file_close()
{
    return EINVAL;
}

int __weak handle_flash_file_stat(char *path)
{
    return EINVAL;
}

int __weak handle_flash_dir_open(char *path)
{
    os_printf("unsupported API\n");
    return EINVAL;
}

struct pfrag * __weak handle_flash_dir_read(uint32_t *mode)
{
    return NULL;
}

int __weak handle_flash_boot(uint32_t address, uint8_t *data, size_t len)
{
    return EINVAL;
}

struct pfrag * __weak handle_flash_hash(uint32_t address, size_t len)
{
    return NULL;
}

static struct packet *
flash_file_mount(void *ctx, struct packet *msg)
{
    struct flash_file_mount_req *req = packet_data(msg);
    return hio_response_status( handle_flash_file_mount(req->address, req->size) );
}

static struct packet *
flash_file_open(void *ctx, struct packet *msg)
{
    struct flash_file_open_req *req = packet_data(msg);
    return hio_response_status( handle_flash_file_open((char*)req->path, req->mode) );
}

static struct packet *
flash_file_remove(void *ctx, struct packet *msg)
{
    struct flash_file_remove_req __unused *req = packet_data(msg);
    return hio_response_status( handle_flash_file_remove((char*)req->path));
}

static struct packet *
flash_file_read(void *ctx, struct packet *msg)
{
    struct flash_file_read_req *req = packet_data(msg);
    struct pfrag *frag = handle_flash_file_read(req->length);

    if (frag) {
        struct packet *pkt = create_flash_file_read_rsp(0, NULL);
        packet_add_frag(pkt, frag);
        return pkt;
    }
    return hio_response_status(EINVAL);
}

static struct packet *
flash_file_write(void *ctx, struct packet *msg)
{
    struct flash_file_write_req *req = packet_data(msg);
    return hio_response_status( handle_flash_file_write(req->data, req->length));
}

static struct packet *
flash_file_close(void *ctx, struct packet *msg)
{
    return hio_response_status( handle_flash_file_close());
}

static struct packet *
flash_file_stat(void *ctx, struct packet *msg)
{
    return hio_response_status(EINVAL);
}

static struct packet *
flash_file_dir_open(void *ctx, struct packet *msg)
{
    struct flash_file_dir_open_req *req = packet_data(msg);
    return hio_response_status( handle_flash_dir_open((char*)req->path));
}

static struct packet *
flash_file_dir_read(void *ctx, struct packet *msg)
{
    struct flash_file_dir_read_req __unused *req = packet_data(msg);

    uint32_t mode;
    struct pfrag * frag = handle_flash_dir_read(&mode);
    struct packet *pkt = create_flash_file_dir_read_rsp(0, mode, NULL);
    if (frag)
        packet_add_frag(pkt, frag);
    return pkt;
}

static struct packet *
flash_boot(void *ctx, struct packet *msg)
{
    struct flash_boot_req *req = packet_data(msg);
    return hio_response_status( handle_flash_boot( req->address, req->data, req->length));
}


static struct packet *
flash_hash(void *ctx, struct packet *msg)
{
    struct packet *pkt; /* response packet */

    if(!spi_initialized) {
        pkt = hio_response_status(EINVAL);
    } else {
        struct flash_hash_req *req = packet_data(msg);

        struct pfrag *frag = handle_flash_hash(req->address, req->length);

        if (frag == NULL)
            return hio_response_status(EINVAL);
        pkt = create_flash_hash_rsp(0, NULL);
        packet_add_frag(pkt, frag);
    }
    return pkt;
}

static struct packet *
flash_flush(void *ctx, struct packet *msg)
{
    int status;

    if(!spi_initialized) {
        status = EINVAL;
    } else {
        struct spi_mem_device * spi_mem = os_flash_get_spi_dev();
        sector_cache_flush_all(spi_mem);
        status = 0;
    }

    return hio_response_status(status);
}

static struct packet *
flash_get_hwaddr(void *ctx, struct packet *msg)
{
    struct flash_get_hwaddr_rsp *rsp;
    struct packet *pkt = alloc_flash_get_hwaddr_rsp(&rsp);

    OS_ERROR_ON_NULL(pkt);

    if(!spi_initialized) {
        rsp->status = EINVAL;
    } else {
        struct spi_mem_device * spi_mem = os_flash_get_spi_dev();
        sector_cache_read(spi_mem,
                          BOOTSECTOR_ADDRESS + offsetof(struct bootsector, deviceinfo.hwaddr),
                          rsp->hwaddr, sizeof(rsp->hwaddr));
        rsp->status = 0;
    }
    return pkt;
}

static struct packet *
flash_set_hwaddr(void *ctx, struct packet *msg)
{
    struct flash_set_hwaddr_req *req = packet_data(msg);
    struct flash_set_hwaddr_rsp *rsp;
    struct packet *pkt = alloc_flash_set_hwaddr_rsp(&rsp);

    OS_ERROR_ON_NULL(pkt);

    if(!spi_initialized) {
        rsp->status = EINVAL;
    } else {
        struct spi_mem_device * spi_mem = os_flash_get_spi_dev();
        sector_cache_write(spi_mem,
                           BOOTSECTOR_ADDRESS + offsetof(struct bootsector, deviceinfo.hwaddr),
                           req->hwaddr, sizeof(req->hwaddr));
        rsp->status = 0;
    }
    return pkt;
}

static struct packet *
flash_get_uuid(void *ctx, struct packet *msg)
{
    struct flash_get_uuid_rsp *rsp;
    struct packet *pkt = alloc_flash_get_uuid_rsp(&rsp);

    OS_ERROR_ON_NULL(pkt);

    if(!spi_initialized) {
        rsp->status = EINVAL;
    } else {
        struct spi_mem_device * spi_mem = os_flash_get_spi_dev();
        sector_cache_read(spi_mem,
                          BOOTSECTOR_ADDRESS + offsetof(struct bootsector, deviceinfo.uuid),
                          rsp->uuid, sizeof(rsp->uuid));
        rsp->status = 0;
    }
    return pkt;
}

static struct packet *
flash_set_uuid(void *ctx, struct packet *msg)
{
    struct flash_set_uuid_req *req = packet_data(msg);
    struct flash_set_uuid_rsp *rsp;
    struct packet *pkt = alloc_flash_set_uuid_rsp(&rsp);

    OS_ERROR_ON_NULL(pkt);

    if(!spi_initialized) {
        rsp->status = EINVAL;
    } else {
        struct spi_mem_device * spi_mem = os_flash_get_spi_dev();
        sector_cache_write(spi_mem,
                           BOOTSECTOR_ADDRESS + offsetof(struct bootsector, deviceinfo.uuid),
                           req->uuid, sizeof(req->uuid));
        rsp->status = 0;
    }
    return pkt;
}

void
handle_add_partition(const struct flash_add_partition_req* req,
                     struct flash_add_partition_rsp* rsp);

void
handle_delete_partition(const struct flash_delete_partition_req* req,
                        struct flash_delete_partition_rsp* rsp);

void
handle_get_partition(const struct flash_get_partition_req* req,
                     struct flash_get_partition_rsp* rsp);

void
handle_load_partition_table(const struct flash_load_partition_table_req* req,
                            struct flash_load_partition_table_rsp* rsp);

void
handle_save_partition_table(const struct flash_save_partition_table_req* req,
                            struct flash_save_partition_table_rsp* rsp);

static struct packet*
flash_load_partition_table(void *ctx, struct packet *msg)
{
    struct flash_load_partition_table_req *req = packet_data(msg);
    struct flash_load_partition_table_rsp *rsp;
    struct packet *pkt = alloc_flash_load_partition_table_rsp(&rsp);

    OS_ERROR_ON_NULL(pkt);

    if(!spi_initialized) {
        rsp->status = EINVAL;
    } else {
        handle_load_partition_table(req, rsp);
    }
    return pkt;
}

static struct packet*
flash_save_partition_table(void *ctx, struct packet *msg)
{
    struct flash_save_partition_table_req *req = packet_data(msg);
    struct flash_save_partition_table_rsp *rsp;
    struct packet *pkt = alloc_flash_save_partition_table_rsp(&rsp);

    OS_ERROR_ON_NULL(pkt);

    if(!spi_initialized) {
        rsp->status = EINVAL;
    } else {
        handle_save_partition_table(req, rsp);
    }
    return pkt;
}

static struct packet*
flash_get_partition(void *ctx, struct packet *msg)
{
    struct flash_get_partition_req *req = packet_data(msg);
    struct flash_get_partition_rsp *rsp;
    struct packet *pkt = alloc_flash_get_partition_rsp(&rsp);

    OS_ERROR_ON_NULL(pkt);

    if(!spi_initialized) {
        rsp->status = EINVAL;
    } else {
        handle_get_partition(req, rsp);
    }
    return pkt;
}

static struct packet*
flash_add_partition(void *ctx, struct packet *msg)
{
    struct flash_add_partition_req *req = packet_data(msg);
    struct flash_add_partition_rsp *rsp;
    struct packet *pkt = alloc_flash_add_partition_rsp(&rsp);

    OS_ERROR_ON_NULL(pkt);

    if(!spi_initialized) {
        rsp->status = EINVAL;
    } else {
        handle_add_partition(req, rsp);
    }
    return pkt;
}

static struct packet*
flash_delete_partition(void *ctx, struct packet *msg)
{
    struct flash_delete_partition_req *req = packet_data(msg);
    struct flash_delete_partition_rsp *rsp;
    struct packet *pkt = alloc_flash_delete_partition_rsp(&rsp);

    OS_ERROR_ON_NULL(pkt);

    if(!spi_initialized) {
        rsp->status = EINVAL;
    } else {
        handle_delete_partition(req, rsp);
    }
    return pkt;
}

static struct packet*
flash_erase_by_sector(void *ctx, struct packet *msg)
{
    struct flash_erase_by_sector_req *req = packet_data(msg);
    struct flash_erase_by_sector_rsp *rsp;
    struct packet *pkt = alloc_flash_erase_by_sector_rsp(&rsp);

    OS_ERROR_ON_NULL(pkt);

    if (!spi_initialized) {
        rsp->status = EINVAL;
    }
    else {
        struct spi_mem_device * spi_mem = os_flash_get_spi_dev();

        if (req->address > spi_mem_get_sector_count(spi_mem) || req->address == 0) {
            os_printf("Wrong Sector Address %d\n", req->address);
            rsp->status = EINVAL;
        }
        else {
            req->address = SECTORSIZE*(req->address);
            sector_cache_erase_sector(spi_mem,req->address);
            rsp->status = 0;
        }
    }
    return pkt;
}

/*
 * [[[cog
 * import cog, generate
 * generate.api(generate.Emitter(cog.out), 'flash')
 * ]]] */
 static const struct hio_api flash_api = {
     .group = 8,
     .num_handlers = 26,
     .handler = {
         flash_identify,
         flash_read,
         flash_write,
         flash_enroll,
         flash_file_mount,
         flash_file_open,
         flash_file_remove,
         flash_file_read,
         flash_file_write,
         flash_file_close,
         flash_file_stat,
         flash_file_dir_open,
         flash_file_dir_read,
         flash_hash,
         flash_boot,
         flash_flush,
         flash_get_hwaddr,
         flash_set_hwaddr,
         flash_get_uuid,
         flash_set_uuid,
         flash_load_partition_table,
         flash_save_partition_table,
         flash_get_partition,
         flash_add_partition,
         flash_delete_partition,
         flash_erase_by_sector,
     }
 };
/* [[[end]]] */

void gordon_hio_init(void)
{
    hio_api_init(&flash_api, NULL);
}
