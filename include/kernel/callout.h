/**
 * @addtogroup callout
 * @{
 */
#pragma once
#include <stdint.h>
#include <kernel/bitops.h>
#include <kernel/queue.h>

/**
 * @protected
 * Callout object representing a function to be called when a timeout expires.
 *
 * @internal
 * @note None of the fields in the struct callout needs to (or should) be
 * accessed directly.
 * @endinternal
 */
struct callout {
    /** Absolute time when the callout expires */
    uint32_t             c_time;
    /** Pointer to the callback function */
    void               (*c_func)(struct callout *);
    /** @internal */
    TAILQ_ENTRY(callout) c_list;
    /** @internal */
    uint32_t             c_flags;
};

#define	CALLOUT_PENDING_BIT  1 /* callout is waiting for timeout */
#define	CALLOUT_DEFER_IT     2 /* defer callback to workqueue. NOTE: not implemented */
#define	CALLOUT_SOFT         3 /* may be aligned to next non-soft timeout (to save power) */

/**
 * Initialize a callout object
 *
 * @param co Pointer to @ref callout object
 * @param fn Pointer to function to be invoked on this callout
 */
static inline void
callout_init(struct callout *co, void (*fn)(struct callout *))
{
    co->c_func = fn;
    co->c_flags = 0;
}

/**
 * Initialize callout object with soft deadline
 *
 * A soft callout timeout is one that accepts the timeout to be invoked at a
 * later time than requested, if this could lead to savings in power
 * consumption. A soft callout expiration will be aligned to the closest
 * non-soft one when entering suspend.
 *
 * @param co Pointer to @ref callout object
 * @param fn Pointer to function to be invoked on this callout
 */
static inline void
callout_init_soft(struct callout *co, void (*fn)(struct callout *))
{
    co->c_func = fn;
    co->c_flags = BIT(CALLOUT_SOFT);
}

/**
 * Check the state of a callout object
 *
 * Use this function to check if the specified callout object is scheduled or
 * not.
 *
 * @param co Pointer to @ref callout object
 *
 * @return True if the specified callout object is scheduled.
 */
static inline int
callout_pending(const struct callout *co)
{
    return (co->c_flags & BIT(CALLOUT_PENDING_BIT)) != 0;
}

int callout_schedule(struct callout *, uint32_t);
int callout_schedule_at(struct callout *co, uint32_t);
int callout_stop(struct callout *);
void callout_align(void);

/** @} */
