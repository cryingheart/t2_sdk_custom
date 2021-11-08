#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <kernel/os.h>
#include <kernel/hostio.h>
#include <kernel/spi-mem.h>
#include <kernel/interrupt.h>
#include <kernel/hwreg.h>
#include <kernel/secureboot.h>
#include <block/mtd.h>
#include <sys/mount.h>
#include <dirent.h>
#include "gordon.h"
#include "flash-api.h"

#define PAGESIZE 256
#define SECTORSIZE 4096

static struct spi_bus spi_bus;
static struct spi_mem_device spi_mem;
static bool   spi_initialized;
static struct {
    uint8_t   sector[SECTORSIZE];
    uint32_t  addr;
    bool      valid;
} cache;

static_assert(SECTORSIZE / PAGESIZE < 32,
              "there are more than 32 pages per sector");

#define MUST_ERASE ~0u

static void
invalidate_cache(void)
{
    cache.valid = false;
    memset(cache.sector, 0xff, SECTORSIZE);
}

/* return bitmask of which pages need to be programmed to result in
 * NEW, given content on flash in OLD, return MUST_ERASE if erase is
 * necessary */
unsigned int
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
unsigned int
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
void
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
int
sector_program(struct spi_mem_device *dev,
               unsigned int sector_address,
               const unsigned char *sector, size_t len)
{
    unsigned int program;
    uint8_t *worksector;

    assert(len <= SECTORSIZE);
    assert((sector_address & (SECTORSIZE-1)) == 0);

    /* read sector */
    if ((worksector = os_alloc(SECTORSIZE)) == NULL)
        return ENOMEM;
    spi_mem_read(dev, sector_address, worksector, SECTORSIZE);

    /* compare to desired content */
    program = check_program_state(sector, worksector, len);
    memcpy(worksector, sector, len);

    if(program == MUST_ERASE) {
        os_printf("Erasing sector %u\n", sector_address / SECTORSIZE);
        spi_sector_erase(dev, sector_address);
        program = check_erased_state(worksector, SECTORSIZE);
    }

    if(program != 0) {
        os_printf("Programming sector %u: ", sector_address / SECTORSIZE);
        for(unsigned page_index = 0; page_index < SECTORSIZE / PAGESIZE; page_index++) {
            if((program & BIT(page_index)) != 0) {
		os_printf("%2u ", page_index);
		page_program(dev, sector_address, page_index, worksector);
            }
        }
        os_printf("\n");
    }

    os_free(worksector);

    return 0;
}

static struct packet *
flash_identify(void *ctx, struct packet *msg)
{
    uint32_t status = 0;
    uint32_t idcode = ~0U;
    uint32_t capacity = 0;

    spi_bus_init(&spi_bus);

    if(spi_mem_init(&spi_mem, &spi_bus, 0) != 0) {
        pr_err("Failed to detect SPI memory device on address zero\n");
        status = ENODEV;
    } else if(spi_mem_get_sector_size(&spi_mem) != SECTORSIZE) {
        os_printf("This device has a sectorsize of %u bytes\n",
                  spi_mem_get_sector_size(&spi_mem));
        os_printf("but this program only works with sectorsize of %u.\n",
                  SECTORSIZE);
        status = EINVAL;
    } else if(spi_mem_get_page_size(&spi_mem) != PAGESIZE) {
        os_printf("This device has an unusual pagesize of %u bytes\n",
                  spi_mem_get_sector_size(&spi_mem));
        status = EINVAL;
    } else {
	idcode = spi_mem_get_idcode(&spi_mem);
	capacity = spi_mem_get_page_count(&spi_mem);
        invalidate_cache();
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
        spi_mem_read(&spi_mem, req->address, pfrag_insert_tail(frag, req->length), req->length);
        pkt = create_flash_read_rsp(0, NULL);
        packet_add_frag(pkt, frag);
    }

    return pkt;
}

static struct packet *
flash_write(void *ctx, struct packet *msg)
{
    uint32_t status = 0;

    if(!spi_initialized) {
        status = EINVAL;
    } else {
        struct flash_write_req *req = packet_data(msg);
        uint32_t sector_addr = req->address & ~(SECTORSIZE-1);
        uint32_t sector_offs = req->address & (SECTORSIZE-1);

        if (cache.valid && (sector_addr != cache.addr || req->length == 0)) {
            status = sector_program(&spi_mem, cache.addr, cache.sector, SECTORSIZE);
            invalidate_cache();
        }

        memcpy(&cache.sector[sector_offs], req->data, req->length);
        cache.addr = sector_addr;
        cache.valid = true;
    }

    return hio_response_status(status);
}


static struct packet *
flash_enroll(void *ctx, struct packet *msg)
{
    uint32_t status = 0;
    struct boot_block *boot_block = (void*)0x71000;

    if(!spi_initialized) {
        status = EINVAL;
    } else {
        struct flash_enroll_req __unused *req = packet_data(msg);
	enroll(boot_block, req->secureboot_mode, req->secret, req->ecdsa_key, req->fw_key);
	status = sector_program(&spi_mem, 0x0, (void*)boot_block, sizeof(struct boot_block));
    }

    return hio_response_status(status);
}

/* smartfs file system */

/* mount filesystem */
static bool   smartfs_mounted;
struct mtd_dev_s *mtd_raw, *mtd_p1;

int do_mount(struct spi_mem_device * device, off_t start_block, off_t nblocks, const char *mountpt)
{
    if (smartfs_mounted)
        return 0;

    mtd_raw = mtd_spi_open_mem(device);

    if(mtd_raw == NULL) {
        os_printf("mtd_spi_open failed\n");
        return -1;
    }

    mtd_p1 = mtd_partition(mtd_raw, start_block, nblocks);
    if(mtd_p1 == NULL) {
        os_printf("mtd_partition failed\n");
        return -1;
    }

    int retval = smart_initialize(0, mtd_p1, "p1");

    if(retval != 0) {
        os_printf("smart_initialize = %d\n", retval);
        return -1;
    }

    retval = mount("/dev/smart0p1", mountpt, "smartfs", 0, NULL);

    if(retval != 0) {
        os_printf("mount = %d\n", retval);
        return -1;
    }

    smartfs_mounted = true;
    os_printf("smartfs mounted\n");
    return 0;
}

static struct packet *
flash_file_mount(void *ctx, struct packet *msg)
{
    uint32_t status = 0;

    if(!spi_initialized) {
        status = EINVAL;
    } else {
        struct flash_file_mount_req *req = packet_data(msg);

        do_mount(&spi_mem, req->address, req->size, "/flash");
    }

    return hio_response_status(status);
}

FILE *flash_fp;

static struct packet *
flash_file_open(void *ctx, struct packet *msg)
{
    uint32_t status = 0;

    if(!spi_initialized) {
        status = EINVAL;
    } else {
        struct flash_file_open_req __unused *req = packet_data(msg);

        if (req->mode == 0)
            flash_fp = fopen((char*)req->path, "r");
        else
            flash_fp = fopen((char*)req->path, "w");
    }
    return hio_response_status(status);
}

static struct packet *
flash_file_remove(void *ctx, struct packet *msg)
{
    uint32_t status = 0;

    if(!spi_initialized) {
        status = EINVAL;
    } else {
        struct flash_file_remove_req __unused *req = packet_data(msg);
        unlink((char*)req->path);
    }
    return hio_response_status(status);
}

static struct packet *
flash_file_read(void *ctx, struct packet *msg)
{
    struct packet *pkt; /* response packet */

    if(!spi_initialized) {
        pkt = hio_response_status(EINVAL);
    } else {
        struct flash_file_read_req __unused *req = packet_data(msg);
        struct pfrag *frag = pfrag_alloc(req->length);

        uint32_t actual = fread(pfrag_insert_tail(frag, req->length), 1, req->length, flash_fp);

        pkt = create_flash_file_read_rsp(0, actual, NULL);
        packet_add_frag(pkt, frag);
    }
    return pkt;
}

static struct packet *
flash_file_write(void *ctx, struct packet *msg)
{
    uint32_t status = 0;

    if(!spi_initialized) {
        status = EINVAL;
    } else {
        struct flash_file_write_req __unused *req = packet_data(msg);

        uint32_t __unused sz_out = fwrite(req->data, 1, req->length, flash_fp);
        os_printf(".");
    }

    return hio_response_status(status);
}

static struct packet *
flash_file_close(void *ctx, struct packet *msg)
{
    uint32_t status = 0;

    if(!spi_initialized) {
        status = EINVAL;
    } else {
        struct flash_file_close_req __unused *req = packet_data(msg);

        fclose(flash_fp);
    }

    return hio_response_status(status);
}

static struct packet *
flash_file_stat(void *ctx, struct packet *msg)
{
    uint32_t status = 0;

    if(!spi_initialized) {
        status = EINVAL;
    } else {
        struct flash_file_stat_req __unused *req = packet_data(msg);

        //req->path, "r");
    }
    return hio_response_status(status);
}

DIR *flash_dp;

static struct packet *
flash_file_dir_open(void *ctx, struct packet *msg)
{
    uint32_t status = 0;

    if(!spi_initialized) {
        status = EINVAL;
    } else {
        struct flash_file_dir_open_req __unused *req = packet_data(msg);

        flash_dp = opendir((char*)req->path);
        if (flash_dp == NULL)
            status = errno;
    }
    return hio_response_status(status);
}

static struct packet *
flash_file_dir_read(void *ctx, struct packet *msg)
{
    struct packet *pkt; /* response packet */

    if(!spi_initialized) {
        pkt = hio_response_status(EINVAL);
    } else {
        struct flash_file_dir_read_req __unused *req = packet_data(msg);

        struct dirent *dent = readdir(flash_dp);

        if (dent == NULL)  {
            closedir(flash_dp);
            pkt = create_flash_file_dir_read_rsp(0, 0, 0, NULL);
        } else {
            uint32_t actual = strlen(dent->d_name);
            struct pfrag *frag = pfrag_alloc(actual);

            memcpy(pfrag_insert_tail(frag, actual), dent->d_name, actual);

            pkt = create_flash_file_dir_read_rsp(0, dent->d_type, actual, NULL);
            packet_add_frag(pkt, frag);
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
     .group = 10,
     .num_handlers = 13,
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
     }
 };
/* [[[end]]] */

int
main(void)
{
    hio_api_init(&flash_api, NULL);
    os_printf("flash: Gordon ready!\n");

    os_park_thread(os_self());
    return 0;
}
