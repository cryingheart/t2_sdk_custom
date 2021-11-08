/**
 * @addtogroup thread
 * @{
 */
#pragma once
#include <kernel/list.h>

struct os_waitq {
    struct list_head waiters;
};

#define OS_WAITQ_INITIALIZER(_name) { .waiters = LIST_HEAD_INIT((_name).waiters) }

void os_init_waitq(struct os_waitq *waitq);
void os_wait(struct os_waitq *waitq);
void os_wakeup_all(struct os_waitq *waitq);

#define os_wait_event(_waitq, _cond) \
    do { \
        os_irqstate_t flags = cpu_irq_disable(); \
        while (!(_cond)) { \
            os_wait((_waitq));         \
            cpu_irq_restore(flags);    \
            flags = cpu_irq_disable(); \
        } \
        cpu_irq_restore(flags); \
    } while(0)

/** @} */
