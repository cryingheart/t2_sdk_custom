#pragma once
#include <stddef.h>
#include <kernel/os.h>
#include <kernel/io.h>
#include <hwreg/dma.h>

struct dma_desc {
    uint16_t         count;
    uint16_t         flags;
#define DESC_IRQ     (1 <<  0)
#define DESC_SEL(_x) (((_x) & 3) << 1)
#define DESC_FLAG    (1 <<  3)
#define DESC_RX      (1 << 14)
#define DESC_TX      (1 << 15)
    void *           buffer;
    uint16_t         actual;
    uint16_t         spare;
    struct dma_desc *next;
};

#define DMA_FOREACH(_desc, _head)\
    for((_desc) = (_head); (_desc) != NULL; (_desc) = (_desc)->next)

static inline void
print_dma_chain(struct dma_desc *d)
{
    if (0) {
        os_printf("DMA-CHAIN %p\n", d);
        DMA_FOREACH(d, d)
            os_printf("%p: %p %4u/%4u %4x\n",
                      d, d->buffer, d->count, d->actual, d->flags);
    }
}

static inline size_t
dma_size(struct dma_desc *desc)
{
    size_t len = 0;

    DMA_FOREACH(desc, desc)
        len += desc->count;

    return len;
}

/* return pointer to final descriptor*/
static inline struct dma_desc* dma_tail(struct dma_desc *first)
{
    DMA_FOREACH(first, first) {
        if(first->next == NULL)
            break;
    }
    return first;
}

static inline void
dma_link(struct dma_desc *first, struct dma_desc *second)
{
    first->next = second;
}

static inline void
dma_map_buffer(struct dma_desc *desc, void *buffer, size_t len, uint32_t flags)
{
    desc->buffer = buffer;
    desc->count = len;
    desc->actual = 0;
    desc->flags = flags;
}

static inline void
dma_start(volatile struct hw_dma_ctrl *dmac, void *buffer, size_t len, uint16_t mode)
{
    mmio_wr32((uint32_t)buffer, &dmac->desc_pointer);
    mmio_wr16(len, &dmac->size);
    mmio_wr16(mode | DMA_XFR_START, &dmac->xfr);
}

static inline void
dma_restart(volatile struct hw_dma_ctrl *dmac)
{
    mmio_wr16_set(DMA_XFR_START, &dmac->xfr);
}

static inline void
dma_stop(volatile struct hw_dma_ctrl *dmac)
{
    mmio_wr16_set(DMA_XFR_STOP, &dmac->xfr);
}

static inline void
dma_start_linear(volatile struct hw_dma_ctrl *dmac, void *buffer, size_t len)
{
    dma_start(dmac, buffer, len, DMA_MODE_LINEAR | DMA_DIR_TX);
}

static inline void
dma_start_linear_rx(volatile struct hw_dma_ctrl *dmac, void *buffer, size_t len)
{
    dma_start(dmac, buffer, len, DMA_MODE_LINEAR | DMA_DIR_RX);
}

static inline void
dma_start_linear_byte(volatile struct hw_dma_ctrl *dmac, void *buffer, size_t len)
{
    dma_start(dmac, buffer, len, DMA_MODE_LINEAR | DMA_DIR_TX | DMA_XFR_BYTE_MODE);
}

static inline void
dma_start_linear_byte_rx(volatile struct hw_dma_ctrl *dmac, void *buffer, size_t len)
{
    dma_start(dmac, buffer, len, DMA_MODE_LINEAR | DMA_DIR_RX | DMA_XFR_BYTE_MODE);
}

static inline void
dma_start_linear_byte_irq_rx(volatile struct hw_dma_ctrl *dmac, void *buffer, size_t len)
{
    dma_start(dmac, buffer, len, DMA_MODE_LINEAR | DMA_DIR_RX | DMA_XFR_BYTE_MODE | DMA_XFR_GEN_IRQ);
}

static inline void
dma_start_linear_word(volatile struct hw_dma_ctrl *dmac, void *buffer, size_t len)
{
    dma_start(dmac, buffer, len/4, DMA_MODE_LINEAR | DMA_DIR_TX | DMA_XFR_WORD_MODE);
}

static inline void
dma_start_linear_word_rx(volatile struct hw_dma_ctrl *dmac, void *buffer, size_t len)
{
    dma_start(dmac, buffer, len/4, DMA_MODE_LINEAR | DMA_DIR_RX | DMA_XFR_WORD_MODE);
}

static inline void
dma_start_circular(volatile struct hw_dma_ctrl *dmac, void *buffer, size_t len)
{
    dma_start(dmac, buffer, len, DMA_MODE_CIRCULAR | DMA_DIR_TX);
}

static inline void
dma_start_desc(volatile struct hw_dma_ctrl *dmac, struct dma_desc *desc)
{
    dma_start(dmac, desc, 0, DMA_MODE_DESC);
}

static inline void
dma_start_desc_byte(volatile struct hw_dma_ctrl *dmac, struct dma_desc *desc)
{
    dma_start(dmac, desc, 0, DMA_MODE_DESC | DMA_XFR_BYTE_MODE);
}

static inline void
dma_start_desc_word(volatile struct hw_dma_ctrl *dmac, struct dma_desc *desc)
{
    dma_start(dmac, desc, 0, DMA_MODE_DESC | DMA_XFR_WORD_MODE);
}

static inline bool
dma_busy(volatile struct hw_dma_ctrl *dmac)
{
    return mmio_rd32(&dmac->status) & DMA_STATUS_BUSY;
}

static inline void*
dma_haddr(volatile struct hw_dma_ctrl *dmac)
{
    return (void*)(mmio_rd32(&dmac->status) & DMA_STATUS_HADDR_MASK);
}

static inline size_t
dma_circular_free(volatile struct hw_dma_ctrl *dmac)
{
    return mmio_rd16(&dmac->circular_free);
}

static inline size_t
dma_hw_size(volatile struct hw_dma_ctrl *dmac)
{
    return mmio_rd16(&dmac->size);
}

static inline void
dma_wait(volatile struct hw_dma_ctrl *dmac)
{
    while (dma_busy(dmac))
        ;
}

static inline int32_t
dma_wait_timeout(volatile struct hw_dma_ctrl *dmac, uint32_t timeout)
{
    int32_t ret;

    timeout += os_systime();

    while ((ret = timeout - os_systime()) > 0 && dma_busy(dmac))
        ;

    return ret;
}

static inline void
dma_sync(volatile struct hw_dma_ctrl *dmac, struct dma_desc *tx, struct dma_desc *rx)
{
    dma_start_desc(&dmac[1], rx);
    dma_start_desc(&dmac[0], tx);
    /* Wait for RX DMA to complete */
    dma_wait(&dmac[1]);
}
