/**
 * @addtogroup drv_uart
 * @{
 */
#pragma once
#include <stddef.h>
#include <stdint.h>

#define UART_DEFAULT_BAUDRATE 921600
#define UART_AUTO_RESUME 1

struct uart;

void uart_init(unsigned baudrate);
struct uart *uart_open(unsigned baudrate);
struct uart *uart_open_ex(unsigned baudrate, int tx_pin, int rx_pin, int flags);
int  uart_close(struct uart *u);
int  uart_getchar(struct uart *u);
int  uart_getchar_tmo(struct uart *u, uint32_t tmo);
char *uart_gets(struct uart *u, char *str, size_t size);
int  uart_putchar(struct uart *u, int c);
int  uart_puts(struct uart *u, const char *str);
void uart_flush(struct uart *u);

typedef void (*uart_callback)(void);
void uart_set_callback(struct uart *u, uart_callback cb, char c);

/**
 * Uart events
 */
enum uart_event {
    /** Terminal-break */
    UART_EVENT_BREAK,
    /** Wakeup from suspend */
    UART_EVENT_WAKEUP
};

typedef void (*uart_event_callback)(struct uart *u, enum uart_event, void *priv);
void uart_set_event_callback(struct uart *u, uart_event_callback cb, void *priv);
void uart_suspend_enable(struct uart *u);

/** @} */
