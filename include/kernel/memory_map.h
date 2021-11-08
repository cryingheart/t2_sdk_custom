#pragma once

#include <stdint.h>
#include <stdbool.h>

#define START_OF_RAM  ((uintptr_t)0x040000)
#define END_OF_RAM    ((uintptr_t)0x0bffff)

#define START_OF_VMEM ((uintptr_t)0x100000)
#define END_OF_VMEM   ((uintptr_t)0xfbffff)

#ifndef __ASSEMBLER__
union __attribute__ ((__transparent_union__)) pointer_address {
    const void *pointer;
    uintptr_t address;
};

static inline bool is_rom_address(union pointer_address arg)
{
    return arg.address < START_OF_RAM;
}

static inline bool is_ram_address(union pointer_address arg)
{
    return arg.address >= START_OF_RAM
        && arg.address <= END_OF_RAM;
}

static inline bool is_virtual_address(union pointer_address arg)
{
    return arg.address >= START_OF_VMEM
        && arg.address <= END_OF_VMEM;
}
#endif
