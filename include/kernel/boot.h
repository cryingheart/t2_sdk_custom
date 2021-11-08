#pragma once

#include <stdint.h>

#define BOOT_SYNC      0x7e

#define BOOT_CMD_NOP   0
#define BOOT_CMD_READ  1
#define BOOT_CMD_WRITE 2
#define BOOT_CMD_ENTRY 3
#define BOOT_CMD_SPEED 4
#define BOOT_CMD_ARGS  7
#define BOOT_CMD_SIGN  8
#define BOOT_CMD_FILL  9
#define BOOT_CMD_PUF  10

struct boot_cmd {
    uint8_t  sync;
    uint8_t  op;
    uint16_t len;
    uint32_t addr;
};

struct boot_args {
    /* order here is to match pop insn */
    int    argc;
    char **argv;
    void (*entry)(int, char**);
};


void boot_init(struct boot_args*);
