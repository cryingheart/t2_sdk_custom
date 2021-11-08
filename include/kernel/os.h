#pragma once
#include <stdint.h>
#include <stdarg.h>
#include <kernel/cdefs.h>
#include <kernel/debug.h>
#include <kernel/timer.h>
#include <kernel/atomic.h>
#include <kernel/console.h>
#include <kernel/waitq.h>
#include <kernel/interrupt.h>
#include <kernel/event.h>
#include <kernel/semaphore.h>
#include <kernel/message.h>
#include <kernel/memory_map.h>
#include <kernel/bootarg.h>
#include "list.h"

struct os_appinfo {
    int stack_size;
};

#define OS_APPINFO \
    static const struct os_appinfo __os_appinfo; \
    const struct os_appinfo *os_query_appinfo(void) { \
        return &__os_appinfo; \
    } \
    static const struct os_appinfo __os_appinfo =

/**
 * @addtogroup thread
 * @{
 */

struct os_thread;

/**
 * The type of the argument passed to a thread.
 */
typedef void *os_threadarg_t;

/**
 * The function type of a threads entry point
 */
typedef void *(*os_entrypoint_t)(os_threadarg_t);

/** The lowest priority for a thread */
#define OS_THREADPRI_LO   0
/** The highest priority for a thread */
#define OS_THREADPRI_HI   7

#define OS_THREADPRI_NUM  (OS_THREADPRI_HI + 1)

/**
 * Specifies the thread priority via the \a flags argument to @ref
 * os_create_thread(). The \a _n argument should be a value from @ref
 * OS_THREADPRI_LO (0) to @ref OS_THREADPRI_HI (7).
 */
#define OS_CRTHREAD_PRIO(_n) ((_n) & 7)

/**
 * Value for the \a flags argument to @ref os_create_thread() to indicate that
 * the thread should be created in 'detached' state. A 'detached' thread is one
 * that can't be joined when it terminates. The threads return value will be
 * discarded when the thread terminates.
 */
#define OS_CRTHREAD_DETACHED BIT(4)

struct os_thread *os_self(void);
void ** os_thread_tls(struct os_thread *t);
uint16_t os_thread_stacksz(struct os_thread *t);
uint32_t* os_thread_stack_base(struct os_thread *t);
uint32_t* os_thread_stack_cur(struct os_thread *t);
struct os_thread *os_create_thread(const char *name, os_entrypoint_t entry, os_threadarg_t arg,
                                   uint32_t flags, size_t stacksz);
uint32_t os_set_prio(uint32_t prio);
int *os_thread_errno(struct os_thread *thread);
const char *os_thread_name(const struct os_thread *thread);
void os_thread_enable_vm(struct os_thread *thread);
void os_destroy_thread(struct os_thread *thread);
void *os_join_thread(struct os_thread *thread);
void os_yield(void);
void os_park_thread(struct os_thread *thread);
void os_unpark_thread(struct os_thread *thread);
void os_sendmsg(struct os_thread *thread, struct os_msg *msg);
void os_postmsg(struct os_thread *thread, struct os_msg *msg, struct os_thread *sender);
struct os_msg *os_recvmsg(bool noblock);
struct os_msg *os_recvmsg_type(unsigned int msg_type);
struct os_thread *os_sender(const struct os_msg *msg);
struct os_tmoref *os_request_timeoutmsg(uint32_t systime, struct os_msg *msg);
struct os_msg *os_cancel_timeoutmsg(struct os_tmoref *tmo);
/** @} */

/**
 * @addtogroup pm
 * @{
 */
void os_suspend_enable(void);
void os_suspend_disable(void);
/** @} */

/**
 * @addtogroup time
 * @{
 */

/** Convert seconds to system time. */
#define SYSTIME_SEC(_n) ((_n) * 1000000)

/** Convert milliseconds to system time. */
#define SYSTIME_MS(_n)  ((_n) * 1000)

/** Convert microseconds to system time. */
#define SYSTIME_US(_n)  ((_n) * 1)

/** Convert IEEE802.11 time units to system time. */
#define SYSTIME_TU(_n)  ((_n) * 1024)

/** Maximum difference of two 32-bit systime values */
#define SYSTIME_DIFFMAX (INT32_MAX - 1)

/**
 * Compare two timestamp values
 *
 * @return True if time ``a`` is after time ``b``.
 */
static inline int time_after(uint32_t a, uint32_t b)
{
    return (int32_t)(b - a) < 0;
}

/**
 * Compare two timestamp values
 *
 * @return True if time ``a`` is before time ``b``.
 */
static inline int time_before(uint32_t a, uint32_t b)
{
    return time_after(b,a);
}

/**
 * Compare two timestamp values
 *
 * @return True if time ``a`` is after or equal to time ``b``.
 */
static inline int time_after_eq(uint32_t a, uint32_t b)
{
    return (int32_t)(a - b) >= 0;
}

/**
 * Compare two timestamp values
 *
 * @return True if time ``a`` is before or equal to time ``b``.
 */
static inline int time_before_eq(uint32_t a, uint32_t b)
{
    return time_after_eq(b,a);
}

/**
 * Total CPU usage in microseconds in different modes.
 */
struct os_cpu_usage {
    /** Total execution time by threads (microseconds) */
    uint64_t user;
    /** Total execution time by interrupt handlers (microseconds) */
    uint64_t irq;
    /** Total time in idle mode (microseconds) */
    uint64_t idle;
};

struct os_tmoref;

uint32_t os_systime(void);
uint64_t os_systime64(void);

void os_get_cpu_usage(struct os_cpu_usage *usage);
void os_delay_until(uint32_t time);
void os_usleep(uint32_t time);
void os_msleep(uint32_t time);
void os_wait_usec(uint32_t wait_time_us);
uint64_t os_get_utcoffset(void);
void os_set_utcoffset(uint64_t offset);
/** @} */

uint32_t os_get_package(void);
void os_stop_trace(void);
uint32_t os_disable_mac_trace(uint32_t disable_mask);
void os_restore_mac_trace(uint32_t enable_bits);
void os_innoscope_init(void);
void os_shutdown(int) __noreturn;
void os_coredump_init(void);
void os_heap_debug_check(void);

/**
 * @addtogroup memory
 * @{
 */
void *os_alloc(size_t size);
void *os_zalloc(size_t size);
void* os_aligned_alloc(size_t alignment, size_t size);
void *os_realloc(void *ptr, size_t size);
void os_free(void *ptr);
void *os_mem_incref(void *ptr);
size_t os_avail_heap(void);

static inline void *os_calloc(size_t nmemb, size_t size)
{
    if(nmemb != 0 && size != 0 && SIZE_MAX / size >= nmemb)
        return os_zalloc(nmemb * size);
    return NULL;
}

struct heap_debug_api
{
    size_t (*pre_alloc)(struct heap_debug_api*, size_t size);
    void (*post_alloc)(struct heap_debug_api*, void *, size_t reqsize, size_t size);
    void (*pre_free)(struct heap_debug_api*, void *);
    void (*post_realloc)(struct heap_debug_api*, void *, size_t reqsize, size_t size);
    void (*failed_alloc)(struct heap_debug_api*, size_t size);
};

void os_heap_debug_api_init(struct heap_debug_api *api);
/** @} */



/**
 * @addtogroup drv_hkadc
 * @{
 */
int32_t os_core_temp(void);
uint32_t os_vbat(void);
uint32_t os_adc(void);
/** @} */

/**
 * @addtogroup random
 * @{
 */

void get_random_bytes(void *, size_t);
uint32_t get_random_uint(uint32_t max);

/** @} */

/**
 * @addtogroup debug
 * @{
 */
void os_print_cpu_usage(const struct os_cpu_usage *from);
void os_wakeup_assert_init(void);
/** @} */
