#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <kernel/compiler.h>
#include <cop/bits.h>
#include <bits/noreturn.h>

static inline void
cop_mcr(unsigned int coproc, unsigned int crn, unsigned int crm,
        unsigned int opc1, unsigned int opc2, unsigned int value)
{
    asm volatile(".arch\tarmv7-m");
    asm volatile("mcr\t%c[coproc], %c[opc1], %[reg], cr%c[crn], cr%c[crm], %c[opc2]"
                 : /* no outputs */ :
                 [coproc] "I" (coproc),
                 [opc1] "I" (opc1),
                 [reg] "r" (value),
                 [crn] "I" (crn),
                 [crm] "I" (crm),
                 [opc2] "I" (opc2)
                 : "memory");
}

static inline unsigned int
cop_mcrr(unsigned int coproc, unsigned int crm, unsigned int opc1,
         unsigned int value)
{
    unsigned int result;

    asm volatile(".arch\tarmv7-m");
    asm volatile("mcrr\t%c[coproc], %c[opc1], %[reg], %[reg2], cr%c[crm]"
                 : [reg2] "=r"(result)
                 : [coproc] "I" (coproc),
                   [opc1] "I" (opc1),
                   [reg] "r" (value),
                   [crm] "I" (crm));
    return result;
}

static inline unsigned int
cop_mcrr2(unsigned int coproc, unsigned int crm, unsigned int opc1,
          unsigned int value)
{
    unsigned int result;

    asm volatile(".arch\tarmv7-m");
    asm volatile("mcrr2\t%c[coproc], %c[opc1], %[reg], %[reg2], c%[crm]"
                 : [reg2] "=r"(result)
                 : [coproc] "I" (coproc),
                   [opc1] "I" (opc1),
                   [reg] "r" (value),
                   [crm] "r" (crm));
    return result;
}

static inline unsigned int
cop_mrc(unsigned int coproc, unsigned int crn, unsigned int crm,
        unsigned int opc1, unsigned int opc2)
{
    unsigned int result;

    asm volatile(".arch\tarmv7-m");
    asm volatile("mrc\t%c[coproc], %c[opc1], %[reg], cr%c[crn], cr%c[crm], %c[opc2]"
                 : [reg] "=r"(result)
                 : [coproc] "I" (coproc),
                 [opc1] "I" (opc1),
                 [crn] "I" (crn),
                 [crm] "I" (crm),
                 [opc2] "I" (opc2)
                 : "memory");
    return result;
}

static inline void __noreturn
cop_idle(void)
{
    asm(".inst 0xbfe0" ::: "memory");
    __builtin_unreachable();
}

static inline uint32_t
cop_udiv(uint32_t dividend, uint32_t divisor)
{
    uint32_t result;

    if(__builtin_constant_p(dividend) && __builtin_constant_p(divisor))
        return dividend / divisor;

    asm volatile(".arch\tarmv7-m");
    asm ("udiv\t%[Rd], %[Rn], %[Rm]" : [Rd] "=r"(result) : [Rn] "r"(dividend), [Rm] "r"(divisor));
    return result;
}

static inline uint32_t
cop_mod(uint32_t dividend, uint32_t divisor)
{
    return dividend - cop_udiv(dividend, divisor) * divisor;
}

/*
 * TIMER & EVENT
 */

static inline void cop_set_timer_match(unsigned int timer_id,
                                       unsigned int timer_value)
{
    cop_mcr(COP_CPROC_TE, COP_CPROC_TE_CRN_TIMER_0 + timer_id, 0, 0, 0, timer_value);
}

static inline unsigned int cop_get_timer_value(unsigned int timer_id)
{
    return cop_mrc(COP_CPROC_TE, COP_CPROC_TE_CRN_TIMER_0 + timer_id, 0, 0, 0);
}

static inline unsigned int cop_get_event_timestamp(unsigned int event_id)
{
    return cop_mrc(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_TS0 + event_id, 0, 0, 0);
}

static inline void cop_set_event_mask_lo(unsigned int event_mask)
{
    cop_mcr(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_MASK, 0,
            COP_CPROC_TE_OPC1_EVENT_MASK_LO, 0, event_mask);
}

static inline void cop_set_event_mask_hi(unsigned int event_mask)
{
    cop_mcr(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_MASK, 0,
            COP_CPROC_TE_OPC1_EVENT_MASK_HI, 0, event_mask);
}

static inline unsigned int cop_get_event_mask_lo(void)
{
    return cop_mrc(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_MASK, 0,
                   COP_CPROC_TE_OPC1_EVENT_MASK_LO, 0);
}

static inline unsigned int cop_get_event_mask_hi(void)
{
    return cop_mrc(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_MASK, 0,
                   COP_CPROC_TE_OPC1_EVENT_MASK_HI, 0);
}

static inline void cop_enable_event(unsigned int event_id)
{
    if(event_id < 32)
        cop_set_event_mask_lo(cop_get_event_mask_lo() | BIT(event_id));
    else
        cop_set_event_mask_hi(cop_get_event_mask_hi() | BIT(event_id-32));
}

static inline void cop_disable_event(unsigned int event_id)
{
    if(event_id < 32)
        cop_set_event_mask_lo(cop_get_event_mask_lo() & ~BIT(event_id));
    else
        cop_set_event_mask_hi(cop_get_event_mask_hi() & ~BIT(event_id-32));
}

static inline unsigned int cop_get_event_pending_lo(void)
{
    return cop_mrc(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_MASK, 0,
                   COP_CPROC_TE_OPC1_EVENT_MASK_PENDING_LO, 0);
}

static inline unsigned int cop_get_event_pending_hi(void)
{
    return cop_mrc(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_MASK, 0,
                   COP_CPROC_TE_OPC1_EVENT_MASK_PENDING_HI, 0);
}

static inline unsigned int cop_get_event_raw_lo(void)
{
    return cop_mrc(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_MASK, 0,
                   COP_CPROC_TE_OPC1_EVENT_MASK_RAW_LO, 0);
}

static inline unsigned int cop_get_event_raw_hi(void)
{
    return cop_mrc(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_MASK, 0,
                   COP_CPROC_TE_OPC1_EVENT_MASK_RAW_HI, 0);
}

static inline int cop_get_event_pending_id(void)
{
    /* Return lowest bit set, -1 if no bit set. */
    return cop_mrc(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_MASK, 0,
                   COP_CPROC_TE_OPC1_EVENT_MASK_PENDING_ID, 0);
}

static inline void cop_set_event(unsigned int eventnum)
{
    if(eventnum < 32)
        cop_mcr(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_SET, 0,
                COP_CPROC_TE_OPC1_EVENT_SET_LO, 0, BIT(eventnum));
    else
        cop_mcr(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_SET, 0,
                COP_CPROC_TE_OPC1_EVENT_SET_HI, 0, BIT(eventnum-32));
}

static inline void cop_clear_event(unsigned int eventnum)
{
    if(eventnum < 32)
        cop_mcr(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_CLR, 0,
                COP_CPROC_TE_OPC1_EVENT_CLR_LO, 0, BIT(eventnum));
    else
        cop_mcr(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_CLR, 0,
                COP_CPROC_TE_OPC1_EVENT_CLR_HI, 0, BIT(eventnum-32));
}

static inline bool cop_test_event(unsigned int event_id)
{
    if(event_id < 32)
        return cop_get_event_pending_lo() & BIT(event_id);
    return cop_get_event_pending_hi() & BIT(event_id-32);
}

static inline void cop_clear_event_lo(unsigned int mask)
{
    cop_mcr(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_CLR, 0,
            COP_CPROC_TE_OPC1_EVENT_CLR_LO, 0, mask);
}

static inline void cop_clear_event_hi(unsigned int mask)
{
    cop_mcr(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_CLR, 0,
            COP_CPROC_TE_OPC1_EVENT_CLR_HI, 0, mask);
}

static inline uint32_t cop_wait_event(unsigned int mask)
{
    return cop_mrc(COP_CPROC_TE, COP_CPROC_TE_CRN_EVENT_WAIT, 0, mask, 0);
}

static __always_inline uint32_t cop_wait_event_timeout(unsigned int mask, unsigned int usec)
{
    uint32_t match = cop_get_timer_value(TIMER_BASE_HFC) + usec * 80;
    cop_set_timer_match(TIMER_BASE_HFC, match);
    return cop_wait_event(mask | COP_WAIT_TIMER);
}

static inline void
cop_udelay(unsigned int usec)
{
    cop_wait_event_timeout(0, usec);
}

/** Cycles-based coprocessor delay.
 *
 * This function is no more precise than cop_udelay, but allows for
 * accuracy below one microsecond.
 *
 * @param cycles number of cycles to delay for
 */
static inline void cop_cycle_delay(unsigned int cycles)
{
    uint32_t match = cop_get_timer_value(TIMER_BASE_HFC) + cycles;
    cop_set_timer_match(TIMER_BASE_HFC, match);
    cop_wait_event(COP_WAIT_TIMER);
}

/** Convert nanoseconds to cpu cycles.
 *
 * @param nsec nanoseconds to convert, must be less than 2**31
 *
 * @retval number of cycles (rounded towards zero) corresponding to @a
 * nsec nanoseconds.
 */
static inline unsigned int cop_nsecs_to_cycles(unsigned int nsec)
{
    return cop_udiv(2*nsec, 25);
}

/*
 * DMA
 */

static inline uint32_t cop_get_dma_event(void)
{
    return cop_mrc(COP_CPROC_DMA, COP_CPROC_DMA_CRN_EVENT, 0, 0, 0);
}

static inline uint32_t cop_get_dma_count(void)
{
    return cop_mrc(COP_CPROC_DMA, COP_CPROC_DMA_CRN_SIZE, 0, 0, 0);
}

static inline void cop_start_dma(unsigned int size, unsigned int mode)
{
    cop_mcr(COP_CPROC_DMA, COP_CPROC_DMA_CRN_SIZE, 0, mode, 0, size);
}

static inline uint32_t cop_get_dma_total(void)
{
    return cop_mrc(COP_CPROC_DMA, COP_CPROC_DMA_CRN_TOTAL, 0, 0, 0);
}

static inline void cop_set_dma_total(uint32_t count)
{
    cop_mcr(COP_CPROC_DMA, COP_CPROC_DMA_CRN_TOTAL, 0, 0, 0, count);
}

static inline void cop_dma_flush(void)
{
    const uint32_t zero = 0;
    cop_mcr(COP_CPROC_DMA, COP_CPROC_DMA_CRN_FLUSH, 0, 0, 0, zero);
}

static inline void* cop_get_dma_addr(void)
{
    return (void*)cop_mrc(COP_CPROC_DMA, COP_CPROC_DMA_CRN_ADDR, 0, 0, 0);
}

static inline void cop_set_dma_addr(void *addr)
{
    cop_mcr(COP_CPROC_DMA, COP_CPROC_DMA_CRN_ADDR, 0, 0, 0, (uintptr_t)addr);
}

static inline uint32_t cop_get_dma_evmask(void)
{
    return cop_mrc(COP_CPROC_DMA, COP_CPROC_DMA_CRN_EV_MASK, 0, 0, 0);
}

static inline void cop_set_dma_evmask(uint32_t mask)
{
    cop_mcr(COP_CPROC_DMA, COP_CPROC_DMA_CRN_EV_MASK, 0, 0, 0, mask);
}

static inline uint8_t cop_get_dma_byte(void)
{
    return cop_mrc(COP_CPROC_DMA, COP_CPROC_DMA_CRN_DATA_8, 0, 0, 0);
}

static inline uint8_t cop_get_dma_byte_crc(void)
{
    return cop_mrc(COP_CPROC_DMA, COP_CPROC_DMA_CRN_DATA_8, 0, 1, 0);
}

static inline void cop_put_dma_byte(uint8_t data)
{
    cop_mcr(COP_CPROC_DMA, COP_CPROC_DMA_CRN_DATA_8, 0, 0, 0, data);
}

static inline void cop_put_dma_byte_crc(uint8_t data)
{
    cop_mcr(COP_CPROC_DMA, COP_CPROC_DMA_CRN_DATA_8, 0, 1, 0, data);
}

static inline uint32_t cop_get_dma_word(void)
{
    return cop_mrc(COP_CPROC_DMA, COP_CPROC_DMA_CRN_DATA_32, 0, 0, 0);
}

static inline uint32_t cop_get_dma_word_crc(void)
{
    return cop_mrc(COP_CPROC_DMA, COP_CPROC_DMA_CRN_DATA_32, 0, 1, 0);
}

static inline void cop_put_dma_word(uint32_t data)
{
    cop_mcr(COP_CPROC_DMA, COP_CPROC_DMA_CRN_DATA_32, 0, 0, 0, data);
}

static inline void cop_put_dma_word_crc(uint32_t data)
{
    cop_mcr(COP_CPROC_DMA, COP_CPROC_DMA_CRN_DATA_32, 0, 1, 0, data);
}

static inline void cop_set_crc_polynomial(unsigned int polynomial)
{
    cop_mcr(COP_CPROC_CRC, COP_CPROC_CRC_CRN_POLY, 0, 0, 0, polynomial);
}

static inline unsigned int cop_get_crc_polynomial(void)
{
    return cop_mrc(COP_CPROC_CRC, COP_CPROC_CRC_CRN_POLY, 0, 0, 0);
}

static inline void cop_set_crc_checksum(unsigned int checksum)
{
    cop_mcr(COP_CPROC_CRC, COP_CPROC_CRC_CRN_CSUM, 0, 0, 0, checksum);
}

static inline unsigned int cop_get_crc_checksum(void)
{
    asm("nop");
    return cop_mrc(COP_CPROC_CRC, COP_CPROC_CRC_CRN_CSUM, 0, 0, 0);
}

static inline void cop_crc_data_1(unsigned int value)
{
    cop_mcr(COP_CPROC_CRC, COP_CPROC_CRC_CRN_DATA_1, 0, 0, 0, value);
}

static inline void cop_crc_data_8(unsigned int value)
{
    cop_mcr(COP_CPROC_CRC, COP_CPROC_CRC_CRN_DATA_8, 0, 0, 0, value);
}

static inline void cop_crc_data_16(unsigned int value)
{
    cop_mcr(COP_CPROC_CRC, COP_CPROC_CRC_CRN_DATA_16, 0, 0, 0, value);
}

static inline void cop_crc_data_32(unsigned int value)
{
    cop_mcr(COP_CPROC_CRC, COP_CPROC_CRC_CRN_DATA_32, 0, 0, 0, value);
}

/*
 * WATCHDOG
 */

static inline void cop_wdog_enable(unsigned int timeout)
{
    cop_mcr(COP_CPROC_WDOG, COP_CPROC_WDOG_CRN_TIMEOUT, 0, 0, 0, timeout);
    cop_mcr(COP_CPROC_WDOG, COP_CPROC_WDOG_CRN_EN, 0, 0, 0, 1);
}

static inline void cop_wdog_disable(void)
{
    cop_mcr(COP_CPROC_WDOG, COP_CPROC_WDOG_CRN_EN, 0, 0, 0, 0);
}

static inline unsigned int cop_wdog_is_enabled(void)
{
    return cop_mrc(COP_CPROC_WDOG, COP_CPROC_WDOG_CRN_EN, 0, 0, 0);
}

static inline unsigned int cop_wdog_get_timeout(void)
{
    return cop_mrc(COP_CPROC_WDOG, COP_CPROC_WDOG_CRN_TIMEOUT, 0, 0, 0);
}

static inline void cop_prof(unsigned int tag, unsigned int value)
{
    cop_mcr(7, (tag >> 7) & 15, (tag >> 0) & 15, (tag >> 11) & 7, (tag >> 4) & 7, value);
}

static inline unsigned int cop_rbit(unsigned int value)
{
    unsigned int result;
    asm(".arch\tarmv7-m");
    asm("rbit\t%0, %1" : "=r"(result) : "r"(value));
    return result;
}

static inline unsigned int cop_log2(unsigned int value)
{
    return cop_mcrr(3, 0, 0, value);
}

static inline unsigned int cop_pow2(unsigned int value)
{
    return cop_mcrr(3, 0, 1, value);
}

static inline unsigned int cop_perm5(unsigned int input, unsigned int ctrl)
{
    return cop_mcrr2(3, input, 0, ctrl);
}

static inline unsigned int cop_clz(unsigned int value)
{
    unsigned int result;
    asm(".arch\tarmv7-m");
    asm("clz\t%0, %1" : "=r"(result) : "r"(value));
    return result;
}

static inline unsigned int cop_ctz(unsigned int value)
{
    return cop_clz(cop_rbit(value));
}
