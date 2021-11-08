#pragma once
#include <stdint.h>

struct hw_clone {
    struct hw_clone_regfile {
        uint32_t                   regs[16];
    } regfile;
    struct hw_clone_cop {
        uint8_t                    run_ctrl;
#define CLONE_RUN_CTRL_START       0x01
#define CLONE_RUN_CTRL_STOP        0x02
#define CLONE_RUN_CTRL_HALT        0x04
        uint8_t                    dbg_pause;
        uint8_t                    idle;
        uint8_t                    exception;
        uint32_t                   stack_guard;
    } cop;
    struct hw_clone_wp {
        uint32_t                   addr_lo;
        uint32_t                   addr_hi;
        uint8_t                    en;
        uint8_t                    hitted;
        uint32_t                   hit_addr;
    } wp;
    struct hw_clone_dma {
        uint16_t                   total_cntr;
        uint16_t                   cntr;
        uint32_t                   haddr;
        uint8_t                    dir;
#define CLONE_DIR_RX               0x0
#define CLONE_DIR_TX               0x1
    } dma;
};
