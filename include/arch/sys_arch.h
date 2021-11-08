/**--------------------------------------------------
 * Copyright (c) 2017, InnophaseInc
 * All rights reserved.
 *
 * @authors: agonga@InnophaseInc.com,
 *
 * @description: arch/sys_arch.h lwip2x port file
 *--------------------------------------------------*/

#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include <errno.h>

typedef os_irqstate_t sys_prot_t;

struct os_semaphore;
typedef struct os_semaphore sys_sem_t;
#define sys_sem_valid(sem) (1)

/* let sys.h use binary semaphores for mutexes */
#define LWIP_COMPAT_MUTEX 1 //0-original

struct sys_mbox;
typedef struct sys_mbox *sys_mbox_t;
#define sys_mbox_valid(mbox) ((void*)(mbox) != NULL && *(mbox) != NULL)
#define sys_mbox_set_invalid(mbox) do { if((mbox) != NULL) { *(mbox) = NULL; }}while(0)

struct os_thread;
typedef struct os_thread *sys_thread_t;

#endif  /* LWIP_ARCH_SYS_ARCH_H */
