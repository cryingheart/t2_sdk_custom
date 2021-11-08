#pragma once

#include <stddef.h>
#include <stdint.h>
#include <kernel/dma.h>
#include <kernel/pkt/packet.h>
#include <kernel/pkt/pfrag.h>
#include <kernel/pkt/data_buffer.h>

struct dma_frag {
    struct dma_desc     df_dma;
    struct data_buffer *df_db;
};

struct dma_chain {
    struct dma_desc  *dc_head;
    struct dma_desc **dc_tail;
    struct dma_frag   dc_frag[0];
};

/* BEGIN GENERATED PROTOS */

void dma_free(struct dma_chain* dc);

struct dma_chain*
packet_dma_map(const struct packet* pkt,
               size_t offset,
               uint16_t dma_flags);

/* END GENERATED PROTOS */
