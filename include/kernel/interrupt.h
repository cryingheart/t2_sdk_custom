#pragma once
#include <stdint.h>

/**
 * @defgroup irq_table Interrupt numbers
 * @{
 *
 * @internal
 * This is the definition of all interupt sources. The numbers here are called
 * interrupt numbers (or irq number) and is the number passed to @ref
 * os_attach_irq to identify the interrupt.
 */
#define IRQ_TIMER0      0  /**< @brief Timer */
#define IRQ_TIMER1      1  /**< @brief Timer */
#define IRQ_EVENT0      4  /**< @brief Event handler */
#define IRQ_EVENT1      5  /**< @brief Event handler */
#define IRQ_EVENT2      6  /**< @brief Event handler */
#define IRQ_EVENT3      7  /**< @brief Event handler */
#define IRQ_CIPHER_TX   8  /**< @brief Cipher TX DMA */
#define IRQ_CIPHER_RX   9  /**< @brief Cipher RX DMA */
#define IRQ_CIPHER_TX2  10 /**< @brief Cipher TX DMA, second stream */
#define IRQ_UART_TX     11 /**< @brief UART TX DMA */
#define IRQ_UART_RX     12 /**< @brief UART RX DMA */
#define IRQ_TXRX_DMA    13 /**< @brief Debug DMA */
#define IRQ_SPI         14 /**< @brief Host SPI */
#define IRQ_SPI_DMA     15 /**< @brief Host SPI DMA */
#define IRQ_FREQ        16 /**< @brief Frequency counter */
#define IRQ_SSPI_TX     17 /**< @brief Slave SPI TX */
#define IRQ_SSPI_RX     18 /**< @brief Slave SPI RX */
#define IRQ_SSPI_START  19 /**< @brief Slave SPI start */
#define IRQ_I2C         20 /**< @brief I2C */
#define IRQ_SUPPORT_DMA 21 /**< @brief Console DMA */
#define IRQ_WDG_TRIP    22 /**< @brief Watchdog */
#define IRQ_PWM_DMA     23 /**< @brief Pulse width modulator */
#define IRQ_PDM_DMA     24 /**< @brief Pulse density demodulator */
#define IRQ_TAP_UART_TX 30 /**< @internal @brief Simulation only */
#define IRQ_TAP_UART_RX 31 /**< @internal @brief Simulation only */

/** @} */

/**
 * @addtogroup irq
 * @{
 */

#define IRQ_HANDLED  0
#define IRQ_NONE    -1

struct cpu_context;

/**
 * Function prototype for exception/interrupt handlers
 */
typedef int (*os_handler)(uint32_t vector, void *arg, struct cpu_context *ctx);

os_handler os_install_chained_handler(uint32_t vector, os_handler handler, void *arg, void **oldarg);
void os_install_handler(uint32_t vector, os_handler handler, void *arg);

void os_attach_irq(uint32_t irq, os_handler isr, void *arg);
void os_irq_enable(uint32_t irqno, uint32_t pri);
void os_irq_disable(uint32_t irqno);
int  os_irq_pending(uint32_t irqno);
void os_irq_clear_pending(uint32_t irqno);

/** @} */
