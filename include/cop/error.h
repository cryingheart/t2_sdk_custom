#pragma once
#include <stdint.h>
#include <kernel/compiler.h>
#include <bits/noreturn.h>

#define COP_ERR_UNSPECIFIED          0x00
#define COP_ERR_PHY_OUT_OF_SYNC      0x01
#define COP_ERR_ASSERTION_FAILED     0xfe
#define COP_ERR_UNEXPECTED_EVENT     0xff

#define cop_bug_on(_expr) \
    do {                  \
        if (_expr)        \
            cop_error(COP_ERR_UNSPECIFIED); \
    } while(0)

static __always_inline void __noreturn
cop_error(uint32_t errcode)
{
    asm("udf\t%0" :: "I"(errcode));
    __builtin_unreachable();
}

static __always_inline void __noreturn
cop_error2(uint32_t errcode, uint32_t extra)
{
    asm("mov r0, %1; udf\t%0" :: "I"(errcode), "r"(extra));
    __builtin_unreachable();
}
