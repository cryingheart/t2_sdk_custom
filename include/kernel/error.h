/**
 * @addtogroup error
 * @{
 */
#pragma once
#include <stdint.h>
#include <kernel/compiler.h>
#include <kernel/cpu.h>

#define OS_ERR_HAS_EXTRA(_errcode) (((_errcode) & 0x80) != 0)

/**
 * OS error codes
 */
typedef enum {
    /** Error in application */
    OS_ERR_APPLICATION          = 0x00,
    /** Heap is out of memory */
    OS_ERR_HEAP_EXHAUSTED       = 0x01,
    /** Failed to initialize virtual memory */
    OS_ERR_VM_INIT_FAILED       = 0x02,
    /** Invalid argument in os function call */
    OS_ERR_INVALID_ARGUMENT     = 0xfa,
    /** An spurious event was triggered */
    OS_ERR_SPURIOUS_EVENT       = 0xfb,
    /** OS internal error */
    OS_ERR_INTERNAL_ERROR       = 0xfc,
    /** Timer callback missing */
    OS_ERR_INVALID_TIMER        = 0xfd,
    /** Assertion failure */
    OS_ERR_ASSERTION_FAILED     = 0xfe,
    /** Unexpected exception */
    OS_ERR_UNEXPECTED_EXCEPTION = 0xff,
} os_err_t;


/**
 * Report fatal error. The error hander is invoked via an undefined instruction
 * exception. The exception handler will dump the CPU register state and parts
 * of the current stack on the debug console and halt the system.
 *
 * @param errcode Error code from @ref os_err_t
 */
static inline void __noreturn
os_error(uint32_t errcode)
{
    asm("udf\t%0" :: "I"(errcode));
    __builtin_unreachable();
}

/**
 * Report fatal error with an extra argument. The \a extra parameter may be
 * used to pass some additonal information about the error. Other than the
 * extra parameter, the result is identical to that of calling @ref os_error().
 *
 * @param errcode Error code from @ref os_err_t
 * @param extra Extra information about the error.
 */
static __always_inline void __noreturn
os_error2(uint32_t errcode, uint32_t extra)
{
    asm("mov r12, %1; udf\t%0" :: "I"(errcode), "r"(extra));
    __builtin_unreachable();
}

#define OS_ERROR_ON_NULL(_expr) ({                      \
            __auto_type __ptr = (_expr);                \
            if(__ptr == NULL)                           \
                os_error(OS_ERR_HEAP_EXHAUSTED);        \
            __ptr;                                      \
        })

/** @internal
 *
 * Annotate functions that might put the thread to sleep
 *
 * It is not allowed to put threads to sleep with interrupts disabled or
 * while running in interrupt context. By adding a call to this function
 * an OS_ERR_INTERNAL_ERROR will be generated in such cases.
 */
static inline void might_sleep(void)
{
    if (in_interrupt() || irq_is_disabled())
        os_error(OS_ERR_INTERNAL_ERROR);
}


void os_coredump(struct cpu_context *ctx);

/**
 * This sections covers functions to handle and debug error cases.
 * Please see Appendix for crash debug for more information on how to interpret the crash printout.
 */

void os_set_crash_handler(void (*crash_cb) (void));

/** @} */
