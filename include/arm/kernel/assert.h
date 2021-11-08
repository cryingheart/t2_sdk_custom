#pragma once
#include <kernel/compiler.h>
#include <kernel/os.h>
#include <kernel/error.h>
#include <cop/error.h>

static __always_inline void
__assert_fail(const char *msg)
{
#if defined(__ARM_ARCH_6M__)
    cop_error2(COP_ERR_ASSERTION_FAILED, (unsigned int)msg);
#elif defined(ROM_BUILD)
    asm(".align 2; pli [r0]");
#else
    os_error2(OS_ERR_ASSERTION_FAILED, (unsigned int)msg);
#endif
}
