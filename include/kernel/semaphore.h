/**
 * @addtogroup semaphore
 * @{
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <kernel/waitq.h>

/**
 * Semaphore struct
 *
 * @note This struct should be considered opaque to the caller. None of the
 * fields needs to be directly accessed.
 */
struct os_semaphore {
    int              value;
    struct os_waitq  waitq;
};

#define OS_SEM_INITALIZER(_name, _value) { .value = (_value), .waitq = OS_WAITQ_INITIALIZER((_name).waitq) }

void os_sem_init(struct os_semaphore *sem, int value);
void os_sem_wait(struct os_semaphore *sem);
int  os_sem_wait_timeout(struct os_semaphore *sem, uint32_t tmo);
void os_sem_post(struct os_semaphore *sem);
bool os_sem_waiting(struct os_semaphore *sem);

/** @} */
