#pragma once

#include <kernel/semaphore.h>

struct os_thread;

/* this implements a recursive mutex (held by single thread, with
 * multiple paired lock/unlock) */
struct os_mutex {
    struct os_semaphore  mtx_sem;
    struct os_thread    *mtx_holder;
};

/* BEGIN GENERATED PROTOS */

void
os_mutex_init(
    struct os_mutex *mtx);

void
os_mutex_lock(
    struct os_mutex *mtx);

void
os_mutex_unlock(
    struct os_mutex *mtx);

/* END GENERATED PROTOS */
