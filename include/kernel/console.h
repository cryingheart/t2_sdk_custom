/**
 * @addtogroup console
 * @{
 */
#pragma once
#include <stddef.h>
#include <stdarg.h>

/**
 * Console driver
 *
 * This object represents a console driver. The console is an output only
 * device dedicated to diagnosic messages.
 */
struct console_dev {
    /** Initialize the console */
    void (*init)(void);
    /** Flush out any pending output */
    void (*flush)(void);
    /** Write a diagnostic message string to the console */
    void (*write)(const char *s, size_t len);
};

/** Console driver for console port using MMIO */
extern const struct console_dev console_dbg;

/** Console driver for console port using DMA */
extern const struct console_dev console_dma;

/** Console driver for UART port */
extern const struct console_dev console_uart;

void os_console_baudrate(int baud);
void os_console_init(const struct console_dev *con);
void os_console_flush(void);
void os_console_write(const char *s, size_t len);
void os_console_hexdump(const char *tag, const void *buf, size_t len);
void os_printf(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
void os_vprintf(const char *fmt, va_list v) __attribute__((__format__(__printf__, 1, 0)));

/** @} */
