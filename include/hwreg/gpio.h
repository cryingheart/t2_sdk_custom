#pragma once
#include <stdint.h>

#define HW_GPIO_NUM_GPIO_PINS      23

struct hw_gpio {
    uint32_t                       en;
    uint32_t                       oe;
    uint32_t                       o;
    uint32_t                       o_set;
    uint32_t                       o_clr;
    uint32_t                       o_xor;
    uint32_t                       i;
    uint32_t                       irq_up;
    uint32_t                       irq_dn;
    uint32_t                       irq_edge;
    uint32_t                       irq_en[4];
    uint32_t                       trace_mask;
    uint32_t                       pe;
    uint32_t                       ds;
    uint8_t                        amux;
    uint8_t                        jtag_mux;
    uint32_t                       input_disable;
    uint32_t                       edge;
};
