/**
 * @addtogroup workqueue
 * @{
 */
#pragma once
#include <stdint.h>
#include <kernel/os.h>
#include <kernel/list.h>
#include <kernel/callout.h>

struct os_work;
struct os_workqueue;

/**
 * Function declaration for os_work callback
 */
typedef void (*os_workfn_t)(struct os_work *work);

/**
 * Representation of work to be executed on a workqueue. This work is
 * essentially a function that is called with a pointer to a struct of this
 * type as its argument.
 *
 * @note This struct should be considered opaque to the caller. None of the
 * fields needs to be directly accessed.
 */
struct os_work {
    struct list_head  list;
    os_workfn_t       func;
    uint32_t          flags;
#define WORK_FLAG_PENDING 0
};

/**
 * Similar to @ref os_work with the addition that this type of work may be
 * scheduled to be run at a specific time in the future.
 *
 * @note This struct should be considered opaque to the caller. None of the
 * fields needs to be directly accessed.
 */
struct os_delayed_work {
    struct os_work       work;
    struct callout       timer;
    struct os_workqueue *wq;
};

/**
 * Define and initialize an object of type @ref os_work. This macro will define
 * a variable of type @ref os_work and initialize it using the provided
 * arguments.
 *
 * @param _name Name of the @ref os_work object
 * @param _fn Pointer to function that will perform the actual work
 */
#define DECLARE_WORK(_name, _fn) \
    struct os_work _name = { \
        .list  = { &(_name).list, &(_name).list }, \
        .func  = _fn, \
        .flags = 0 \
    }

/**
 * Initialize a struct @ref os_work with a work function
 *
 * @param work Pointer to @ref os_work to be initialized
 * @param func Pointer to function that will perform the actual work
 */
static inline void
os_init_work(struct os_work *work, os_workfn_t func)
{
    list_init(&work->list);
    work->func = func;
    work->flags = 0;
}

/**
 * A workqueue is a list of @ref os_work. The workqueue is typically serviced
 * by a dedicated thread as a way to defer work to thread context.
 *
 * @note This struct should be considered opaque to the caller. None of the
 * fields needs to be directly accessed.
 */
struct os_workqueue {
    struct list_head worklist;
    struct os_semaphore sem;
    uint32_t            flags;
#define WQ_FLAG_FLUSH BIT(0)
};

void os_init_workqueue(struct os_workqueue *wq);
void os_run_work(struct os_workqueue *wq);
void os_flush_workqueue(struct os_workqueue *wq);

int os_queue_work(struct os_workqueue *wq, struct os_work *work);
int os_cancel_work(struct os_work *work);
int os_queue_system_work(struct os_work *work);

void os_init_delayed_work(struct os_delayed_work *dw, os_workfn_t func);
int os_queue_delayed_work(struct os_delayed_work *dw, struct os_workqueue *wq, uint32_t expire);
int os_cancel_delayed_work(struct os_delayed_work *dw);

/** @} */
