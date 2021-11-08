#include <errno.h>
#include <kernel/flash.h>
#include "gordon.h"
#include "flash-api.h"

static struct partition_table partition_table;

#define PARTITION_TABLE_ADDRESS \
    (BOOTSECTOR_ADDRESS + offsetof(struct bootsector, ptable))

void
handle_load_partition_table(const struct flash_load_partition_table_req *req,
                            struct flash_load_partition_table_rsp *rsp)
{
    struct spi_mem_device *dev = os_flash_get_spi_dev();

    if(req->revert == 0 && partition_table.header == PARTITION_TABLE_MAGIC) {
        /* already loaded */
        rsp->status = 0;
        return;
    }

    sector_cache_read(dev, PARTITION_TABLE_ADDRESS,
                      &partition_table, sizeof(partition_table));

    if(partition_table.header != PARTITION_TABLE_MAGIC) {
        partition_table.header = PARTITION_TABLE_MAGIC;
        ARRAY_FOREACH(mp, partition_table.part, __auto_type)
            media_part_init(mp);
    }
    rsp->status = 0;
}

void
handle_save_partition_table(const struct flash_save_partition_table_req *req,
                            struct flash_save_partition_table_rsp *rsp)
{
    struct spi_mem_device *dev = os_flash_get_spi_dev();
    sector_cache_write(dev, PARTITION_TABLE_ADDRESS,
                       &partition_table, sizeof(partition_table));
    rsp->status = 0;
}

void handle_get_partition(const struct flash_get_partition_req *req,
                          struct flash_get_partition_rsp *rsp)
{
    struct media_part *mp;

    if(partition_table.header != PARTITION_TABLE_MAGIC) {
        rsp->status = EINVAL;
        return;
    }
    if(req->index >= ARRAY_SIZE(partition_table.part)) {
        rsp->status = EINVAL;
        return;
    }
    mp = &partition_table.part[req->index];
    if(!media_part_is_active(mp)) {
        rsp->status = ENOENT;
        return;
    }
    rsp->type = media_part_get_type(mp);
    rsp->sect_start = media_part_get_sector_start(mp);
    rsp->sect_count = media_part_get_sector_count(mp);

    rsp->status = 0;
}


void handle_add_partition(const struct flash_add_partition_req *req,
                          struct flash_add_partition_rsp *rsp)
{
    struct media_part *mp;

    if(partition_table.header != PARTITION_TABLE_MAGIC) {
        rsp->status = EINVAL;
        return;
    }

    if(req->index >= ARRAY_SIZE(partition_table.part)) {
        rsp->status = EINVAL;
        return;
    }

    mp = &partition_table.part[req->index];

    if(media_part_is_active(mp)) {
        rsp->status = EEXIST;
        return;
    }

    if(!media_part_get_active(mp))
        /* deleted entry, re-init */
        media_part_init(mp);

    media_part_set_sector_start(mp, req->sect_start);
    media_part_set_sector_count(mp, req->sect_count);
    media_part_set_type(mp, req->type);

    pr_warn("--> %u %u %u %u %u\n",
            media_part_is_active(mp),
            media_part_get_active(mp),
            media_part_get_sector_start(mp),
            media_part_get_sector_count(mp),
            media_part_get_type(mp));

    rsp->status = 0;
}


void handle_delete_partition(const struct flash_delete_partition_req *req,
                             struct flash_delete_partition_rsp *rsp)
{
    struct media_part *mp;

    if(partition_table.header != PARTITION_TABLE_MAGIC) {
        rsp->status = EINVAL;
        return;
    }

    if(req->index >= ARRAY_SIZE(partition_table.part)) {
        rsp->status = EINVAL;
        return;
    }

    mp = &partition_table.part[req->index];

    media_part_set_active(mp, false);

    rsp->status = 0;
}
