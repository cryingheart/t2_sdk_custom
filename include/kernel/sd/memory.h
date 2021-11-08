#pragma once
#include <stddef.h>
#include <stdint.h>

#define SD_STATUS_ATTACHED BIT(0)
#define SD_STATUS_4BIT     BIT(1)
#define SD_STATUS_CMD8     BIT(8)
#define SD_STATUS_SDHC     BIT(9)
#define SD_STATUS_IO       BIT(24)
#define SD_STATUS_INVALID  BIT(31)

struct sd_memory_card {
    uint32_t status;

    /* on-card registers */
    uint32_t          cid[4];
    uint16_t          rca;
    uint32_t          csd[4];
    uint32_t          scr[2];
    uint32_t          ocr;
    /* ssr */
    uint32_t          csr;

    /* extracted information */
    uint16_t          log2_block_size;
    size_t            capacity; /* in blocks */
};
