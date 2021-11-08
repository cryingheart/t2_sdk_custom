#pragma once

#include <kernel/elfnote.h>
#include "rom_patch.h"

#define CALLROM(fn) ((__typeof__(&(fn)))ROM_##fn)

#define _PATCHNOTE(_name, _id, _patched_function)\
    ELFNOTE(innophase, 1,                        \
            .long _id;                           \
            .long _name;                         \
            .long _patched_function;             \
            .asciz #_patched_function)

#define PATCHNOTE(_name, _patched_function)                     \
    _PATCHNOTE(_name, PATCH_ID_##_name, _patched_function)

#define __patch __attribute__((aligned(4)))
