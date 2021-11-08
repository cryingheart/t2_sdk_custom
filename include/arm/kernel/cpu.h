#pragma once
#include <kernel/compiler.h>
#include <kernel/cdefs.h>
#include <bits/noreturn.h>

#define VECTOR_RESET            1
#define VECTOR_NMI              2
#define VECTOR_HARD_FAULT       3
#define VECTOR_MEM_MANAGE       4
#define VECTOR_BUS_FAULT        5
#define VECTOR_USAGE_FAULT      6
#define VECTOR_SVCALL           11
#define VECTOR_DEBUG            12
#define VECTOR_PENDSV           14
#define VECTOR_SYSTICK          15
#define VECTOR_HW0              16

#define NUM_EXCEPTION_VECTORS   16
#define NUM_EXT_IRQ             32
#define VECTOR_TO_IRQ(_vector)  ((_vector) - VECTOR_HW0)

#define CONTROL_nPRIV 0x01
#define CONTROL_SPSEL 0x02

/*
 * System Control Space Registers
 */

#define SCS_BASE        0xe000e000
#define ACTLR           (SCS_BASE + 0x008)
#define ACTLR_DISMCYCINT  (1<<0)
#define ACTLR_DISDEFWBUF  (1<<1)
#define ACTLR_DISFOLD     (1<<2)
#define NVIC_BASE       (SCS_BASE + 0x100)
#define SCB_BASE        (SCS_BASE + 0xd00)

#define ICSR_REG        0xd04
#define ICSR_NMIPENDSET  BIT(31)
#define ICSR_PENDSVSET   BIT(28)
#define ICSR_PENDSVCLR   BIT(27)
#define ICSR_PENDSTSET   BIT(26)
#define ICSR_PENDSTCLR   BIT(25)
#define ICSR_ISRPREEMPT  BIT(23)
#define ICSR_ISRPENDING  BIT(22)
#define ICSR_VECTPENDING BITMASK2(20, 12)
#define ICSR_RETTOBASE   BIT(11)
#define ICSR_VECTACTIVE  BITMASK2(8, 0)

#define DWT_BASE        0xe0001000

#ifndef __ASSEMBLER__
#include <stdint.h>

struct reg_scb { /* @ 0xE000ED00 */
    uint32_t cpuid;
    uint32_t icsr;
    uint32_t vtor;
    uint32_t aircr;
    uint32_t scr;
#define SCR_SLEEPONEXIT  (1U << 1)
#define SCR_SLEEPDEEP    (1U << 2)
#define SCR_SEVONPEND    (1U << 4)
    uint32_t ccr;
#define CCR_UNALIGN_TRP (1U << 3)
#define CCR_DIV0_TRP    (1U << 4)
    uint32_t shpr[3];
    uint32_t shcsr;
    uint32_t cfsr;
#define CFSR_IBUSERR     (1U<<(8+0))
#define CFSR_PRECISERR   (1U<<(8+1))
#define CFSR_IMPRECISERR (1U<<(8+2))
#define CFSR_UNSTKERR    (1U<<(8+3))
#define CFSR_STKERR      (1U<<(8+4))
#define CFSR_LSPERR      (1U<<(8+5))
#define CFSR_BFARVALID   (1U<<(8+7))
#define CFSR_UNDEFINSTR  (1U<<(16+0))
#define CFSR_INVSTATE    (1U<<(16+1))
#define CFSR_INVPC       (1U<<(16+2))
#define CFSR_NOCP        (1U<<(16+3))
#define CFSR_UNALIGNED   (1U<<(16+8))
#define CFSR_DIVBYZERO   (1U<<(16+9))
    uint32_t hfsr;
    uint32_t dfsr;
#define DFSR_HALTED      (1U<<0)
#define DFSR_BKPT        (1U<<1)
#define DFSR_DWTTRAP     (1U<<2)
#define DFSR_VCATCH      (1U<<3)
#define DFSR_EXTERNAL    (1U<<4)
    uint32_t mmfar;
    uint32_t bfar;
    uint32_t afsr;
    uint32_t __pad[44];
    uint32_t dhcsr;
#define DHCSR_C_DEBUGEN  (1U<<0)
#define DHCSR_C_HALT     (1U<<1)
#define DHCSR_C_STEP     (1U<<2)
#define DHCSR_C_MASKINTS (1U<<3)
#define DHCSR_S_REGRDY   (1U<<16)
#define DHCSR_S_HALT     (1U<<17)
#define DHCSR_DBGKEY     0xA05F0000
    uint32_t dhrcr;
    uint32_t dhrdr;
    uint32_t demcr;
#define MON_EN           (1U<<16)
#define MON_PEND         (1U<<17)
#define MON_STEP         (1U<<18)
#define MON_REQ          (1U<<19)
#define TRCENA           (1U<<24)
};

struct reg_nvic { /* @ 0xE000E100 */
    uint32_t iser[16]; /* Interrupt Set Enable */
    uint32_t pad0[16];
    uint32_t icer[16]; /* Interrupt Clr Enable */
    uint32_t pad1[16];
    uint32_t ispr[16]; /* Interrupt Set Pending */
    uint32_t pad2[16];
    uint32_t icpr[16]; /* Interrupt Clr Pending */
    uint32_t pad3[16];
    uint32_t iabr[16]; /* Interrupt Active Bit */
    uint32_t pad4[48];
    uint8_t  ipr[496]; /* Interrupt Priority */
};

struct reg_dwt {
    uint32_t ctrl;
    uint32_t cyctn;
    uint32_t cpicnt;
    uint32_t exccnt;
    uint32_t sleepcnt;
    uint32_t lsucnt;
    uint32_t foldcnt;
    uint32_t pcsr;
    struct {
        uint32_t addr;
        uint32_t mask;
        uint32_t func;
        uint32_t reserved;
    } comp[16];
};

static volatile __unused struct reg_scb *arm_scb = (volatile struct reg_scb *)SCB_BASE;
static volatile __unused struct reg_nvic *arm_nvic = (volatile struct reg_nvic *)NVIC_BASE;
static volatile __unused struct reg_dwt *dwt = (volatile struct reg_dwt *)DWT_BASE;

struct cpu_hwframe {
    /* Hardware frame */
    uint32_t r0_r3[4];
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
};

struct cpu_context {
    uint32_t r4_r11[8];
    uint32_t ret_pc;
    struct cpu_hwframe hw;
};

typedef uint32_t os_irqstate_t;

#define xPSR_T (1u << 24)

#define ny_error(swid) __asm__("bkpt %0" :: "i"(swid));

#define mfsr(_reg) \
    ({ uint32_t ret; asm("mrs\t%0, " #_reg :"=r"(ret)); ret; })

static inline unsigned find_msb(uint32_t x)
{
    uint32_t pos;
    asm("clz\t%0, %1" :"=r"(pos) : "r"(x));
    return 31 - pos;
}

static inline void
cpu_os_idle(void)
{
    asm("wfi");
}

/**
 * Disable interrupts to CPU and return the previous IRQ state
 */
static inline os_irqstate_t
cpu_irq_disable(void)
{
#if defined(__ARM_ARCH_6M__)
    return 0;
#else
    os_irqstate_t ret;
    asm volatile("mrs %0, PRIMASK; cpsid i" : "=r"(ret) :: "memory");
    return ret;
#endif
}

/**
 * Flush instruction pipeline.
 */
static inline void
cpu_isb(void)
{
    asm("isb");
}

/**
 * Restore IRQ state
 */
static inline void
cpu_irq_restore(os_irqstate_t state)
{
#if defined(__ARM_ARCH_6M__)
    (void) state;
#else
    asm volatile("msr PRIMASK, %0; isb" :: "r"(state) : "memory");
#endif
}

static inline int
irq_is_disabled(void)
{
#if defined(__ARM_ARCH_6M__)
    return 0;
#else
    int primask;
    asm volatile("mrs %0, primask" : "=r"(primask));
    return primask;
#endif
}

/*
* https://developer.arm.com/documentation/dui0552/a/the-cortex-m3-processor/programmers-model/core-registers
*/
static inline uint8_t
cpu_get_ipsr(void)
{
#if defined(__ARM_ARCH_6M__)
    return 0;
#else
    int ipsr;
    asm volatile("mrs %0, IPSR" : "=r"(ipsr));
    return ipsr;
#endif
}

static inline int
in_interrupt(void)
{
    return cpu_get_ipsr() != 0;
}

/**
 * Enable IRQ
 */
static inline void
cpu_irq_enable(void)
{
    asm volatile("cpsie\ti" ::: "memory");
}

static inline uint32_t
cpu_get_lr(void)
{
    uint32_t lr;
    asm ("mov %0, lr" : "=r" (lr));
    return lr;
}

#define PROF_THREAD         0
#define PROF_UNUSED         1
#define PROF_HEAP_ALLOC     2
#define PROF_HEAP_REALLOC   3
#define PROF_HEAP_FREE      4
#define PROF_VM_FAULT_VADDR 5
#define PROF_VM_FAULT_PTE   6
#define PROF_HEAP_AVAIL     7
#define PROF_PALLOC         8

#define PROF_WIFI_PS_EVENT  10
#define PROF_WIFI_MEDIA_REQ 11
#define PROF_WIFI_MEDIA_REL 12
#define PROF_WIFI_MEDIA_ST  13
#define PROF_WIFI_BEACON_MISS 14

#define PROF_STAR_STATE     20
#define PROF_STAR_REPORT    21

#define PROF_UNSTABLE_640   24
#define PROF_OS_TIME        25

#define PROF_SUPPLICANT_ERROR    0x200
#define PROF_SUPPLICANT_PROGRESS 0x201

static void __always_inline
prof(unsigned tag, unsigned value)
{
    asm("mcr\tp7, %c[opc1], %[value], CR%c[crn], CR%c[crm], %c[opc2]"
        : /* no outputs */ :
        [opc1]  "I"((tag >> 11) & 7),
        [value] "r"(value),
        [crn]   "I"((tag >> 7) & 15),
        [crm]   "I"((tag >> 0) & 15),
        [opc2]  "I"((tag >> 4) & 7));
}

static __always_inline unsigned int bitreverse(unsigned int value)
{
    unsigned int result;
    asm("rbit\t%0, %1" : "=r"(result) : "r"(value));
    return result;
}

struct os_thread;

/**
 * Initialize CPU registers
 */
void cpu_initialize(void);

/**
 * Initialize new CPU context for a thread.
 */
void cpu_init_context(struct os_thread *);

/**
 * Transfer control to thread context
 */
void cpu_enter_thread(struct os_thread *);

/**
 * Save current CPU context for the specified thread and call scheduler.
 */
void cpu_save_context(struct os_thread *);

/**
 * Restore CPU context.
 */
void *cpu_restore_context(struct os_thread *);

/**
 * Shutdown machine
 */
void shutdown(int status) __noreturn;

#endif
