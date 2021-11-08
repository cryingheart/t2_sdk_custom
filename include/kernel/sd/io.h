#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <kernel/sd/cccr.h>

struct cccr {
    uint16_t revision;
    uint8_t  io_enable;
    uint8_t  io_ready;
    uint8_t  int_pending;
    uint8_t  io_abort;
    uint8_t  bus_control;
    uint8_t  card_caps;
    uint8_t  cis_pointer[3];
    uint8_t  bus_suspend;
    uint8_t  function_select;
    uint8_t  exec_flags;
    uint8_t  ready_flags;
    uint16_t block_size;
    uint8_t  power_control;
    uint8_t  bus_speed;
    uint8_t  uhs1_support;
    uint8_t  driver_strength;
    uint8_t  interrupt_extension;
    uint8_t  rfu[217];
    uint8_t  vendor[16]; /* @240 */
};

struct cis_ptr {
    const void *ptr;
    size_t size;
};

struct fbr {
    uint8_t         iface_code;
    uint8_t         ext_iface_code;
};

struct card_state;

struct card_ops {
    unsigned int (*read_write)(struct card_state*, bool,
                               unsigned int, unsigned int);
    void         (*dma_step)(struct card_state *cs);
};

struct io_func_handler {
    const struct cis_ptr *cis;
    uint16_t block_size;
    struct card_ops ops;
    void (*trigger)(struct card_state*);
};

struct sdio_function {
    struct io_func_handler fh;
    struct fbr             fbr;
};

struct io_xfer {
    uint32_t                arg;   /* raw command */
    size_t                  total; /* total bytes */
    struct io_func_handler *fh;    /* function */
    uint32_t                state; /* temp storage for dma_step */
};


/* callback on well-formed sd request */
typedef void (*io_rx_handler)(struct card_state*, uint8_t, uint32_t);

struct card_state {
    uint8_t                 card_status;
    uint8_t                 next_card_status;
    uint16_t                rca;
    uint8_t                 fmask;    /* mask of available functions */
    uint8_t                 ioe_ior;  /* enabled functions */
    uint8_t                 ien;      /* interrupt enable mask */
    uint8_t                 irq;      /* interrupt state per function */
    bool                    is_selected;
    bool                    interrupt_period;
    bool                    interrupt_pending;
    bool                    four_wire_mode;
    bool                    cd_disable;
    struct io_xfer          xfer;
    struct io_func_handler *fh[8];
    io_rx_handler           rx_handler;
};

static inline void sd_io_card_update_irq_pending(struct card_state *cs)
{
    if(cs->ien & CCCR_IEN_IENM)
        cs->interrupt_pending = cs->ioe_ior & cs->ien & cs->irq;
    else
        cs->interrupt_pending = false;
}

static inline void sd_io_card_set_ien(struct card_state *cs, uint8_t ien)
{
    cs->ien = ien & (cs->fmask | CCCR_IEN_IENM);
    sd_io_card_update_irq_pending(cs);
}

static inline void sd_io_card_set_ioe(struct card_state *cs, uint8_t ioe)
{
    cs->ioe_ior = ioe & cs->fmask;
    sd_io_card_update_irq_pending(cs);
}

static inline void sd_io_card_set_irq(struct card_state *cs, unsigned int function)
{
    cs->irq |= BIT(function);
    sd_io_card_update_irq_pending(cs);
}

static inline void sd_io_card_clr_irq(struct card_state *cs, unsigned int function)
{
    cs->irq &= ~BIT(function);
    sd_io_card_update_irq_pending(cs);
}

#define CARD_STATUS_COM_CRC_ERROR   BIT(7)
#define CARD_STATUS_ILLEGAL_COMMAND BIT(6)
#define CARD_STATUS_STATE_DIS       (0u << 4)
#define CARD_STATUS_STATE_CMD       (1u << 4)
#define CARD_STATUS_STATE_TRN       (2u << 4)
#define CARD_STATUS_ERROR           BIT(3)
#define CARD_STATUS_FUNCTION_NUMBER BIT(1)
#define CARD_STATUS_OUT_OF_RANGE    BIT(0)

#define CARD_STATUS_AUTO_CLEAR \
    (CARD_STATUS_OUT_OF_RANGE|CARD_STATUS_ERROR)
