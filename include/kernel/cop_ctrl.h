#pragma once
#include <stddef.h>
#include <kernel/bitops.h>

#define COP_WIFI 0
#define COP_BT   1
#define COP_HIF  2

void cop_ctrl_start(int cop, void *entry, void *stack, size_t stack_sz, size_t nregs, ...);
void cop_ctrl_stop(int cop);
void cop_ctrl_set_callback(int cop, int (*status_cb)(uint32_t status));
uint32_t cop_ctrl_wait_idle(int cop, uint32_t timeout);
uint32_t cop_ctrl_get_status(int cop);
void cop_ctrl_watch_range(int cop, void *base, size_t size, uint32_t flags);

#define COP_WATCH_READ  BIT(0)
#define COP_WATCH_WRITE BIT(1)
#define COP_WATCH_EXEC  BIT(2)

/* exception reasons */
#define CLONE_COP_EXC_IDLE  BIT(0) /* idle instruction */
#define CLONE_COP_EXC_BADOP BIT(1) /* invalid instruction */
#define CLONE_COP_EXC_STOP  BIT(2) /* stopped via regwrite */
#define CLONE_COP_EXC_WDOG  BIT(3) /* watchdog timeout */
#define CLONE_COP_EXC_ALIGN BIT(4) /* alignment fault */
#define CLONE_COP_EXC_STACK BIT(5) /* stack overflow */
#define CLONE_COP_EXC_WP    BIT(6) /* watchpoint */
