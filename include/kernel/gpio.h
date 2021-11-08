/**
 * @addtogroup drv_gpio
 * @{
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <kernel/compiler.h>
#include <kernel/bitops.h>

#define GPIO_PIN(N) (1U << N)

/**
 * GPIO MUX function inputs
 */
enum gpio_mux_sel {
    /** Console TX */
    GPIO_MUX_SEL_CONSOLE_TX = 0,
    /** UART RXD */
    GPIO_MUX_SEL_UART_RXD   = 1,
    /** UART TXD */
    GPIO_MUX_SEL_UART_TXD   = 2,
    /** PDM CLK */
    GPIO_MUX_SEL_PDM_CLK    = 3,
    /** PDM Data */
    GPIO_MUX_SEL_PDM_DATA   = 4,
    /** @i2c Clock */
    GPIO_MUX_SEL_SCL        = 5,
    /** @i2c Data */
    GPIO_MUX_SEL_SDA        = 6,
    /** SPI CS[0] */
    GPIO_MUX_SEL_SPI_CS_0   = 7,
    /** SPI CS[1] */
    GPIO_MUX_SEL_SPI_CS_1   = 8,
    /** EVENT[2] */
    GPIO_MUX_SEL_EVT_2      = 9,
    /** EVENT[3] */
    GPIO_MUX_SEL_EVT_3      = 10,
    /** I2S SCK */
    GPIO_MUX_SEL_I2S_SCK    = 11,
    /** I2S WS */
    GPIO_MUX_SEL_I2S_WS     = 12,
    /** I2S SD */
    GPIO_MUX_SEL_I2S_SD     = 13,
    /** PWM */
    GPIO_MUX_SEL_PWM_0      = 14,
    GPIO_MUX_SEL_PWM_1      = 15,
    GPIO_MUX_SEL_PWM_2      = 16,
    GPIO_MUX_SEL_PWM_3      = 17,
    /** UART CTS */
    GPIO_MUX_SEL_UART_CTS   = 18,
    /** UART RTS */
    GPIO_MUX_SEL_UART_RTS   = 19
};

#define GPIO_SDIO_SET                           \
    (GPIO_PIN_SDIO_CLK_0                        \
     | GPIO_PIN_SDIO_CMD_1                      \
     | GPIO_PIN_SDIO_DAT_0_2                    \
     | GPIO_PIN_SDIO_DAT_1_3                    \
     | GPIO_PIN_SDIO_DAT_2_4                    \
     | GPIO_PIN_SDIO_DAT_3_5)

#define GPIO_SDIO_MASK_SET                      \
    (GPIO_MASK_SDIO_CLK_0                       \
     | GPIO_MASK_SDIO_CMD_1                     \
     | GPIO_MASK_SDIO_DAT_0_2                   \
     | GPIO_MASK_SDIO_DAT_1_3                   \
     | GPIO_MASK_SDIO_DAT_2_4                   \
     | GPIO_MASK_SDIO_DAT_3_5)

#define GPIO_SDIO_MUX_SET                       \
    (GPIO_MUX_SDIO_CLK_0                        \
     | GPIO_MUX_SDIO_CMD_1                      \
     | GPIO_MUX_SDIO_DAT_0_2                    \
     | GPIO_MUX_SDIO_DAT_1_3                    \
     | GPIO_MUX_SDIO_DAT_2_4                    \
     | GPIO_MUX_SDIO_DAT_3_5)

#define GPIO_SSPI_SET                           \
    (GPIO_PIN_SDIO_CLK_0                        \
     | GPIO_PIN_SPI_SLV_CS_5                    \
     | GPIO_PIN_SPI_SLV_D_0_1                   \
     | GPIO_PIN_SPI_SLV_D_1_2)

#define GPIO_SSPI_MASK_SET                      \
    (GPIO_MASK_SPI_SLV_CS_5                     \
     | GPIO_MASK_SPI_SLV_D_0_1                  \
     | GPIO_MASK_SPI_SLV_D_1_2)

#define GPIO_SSPI_MUX_SET                       \
    (GPIO_MUX_SPI_SLV_CS_5                      \
     | GPIO_MUX_SPI_SLV_D_0_1                   \
     | GPIO_MUX_SPI_SLV_D_1_2)

/* override of uart-rx and spi-si on pin 1, clear to enable set to
 * disable */
#define GPIO_MUX_BOOT_OVERRIDE BIT(31)

/**
 * Representation of a set of GPIO pins
 *
 * A set of GPIO pins are represented as a bit mask, with each bit position
 * corresponding to one GPIO pin. GPIO-0 is the lest significant bit in the
 * mask.
 */
typedef unsigned int os_gpio_set_t;

/* ** BEGIN GENERATED CODE ** */
#define GPIO_MUX_SDIO_CLK_0     (3u << 0)
#define GPIO_MASK_SDIO_CLK_0    (BITMASK(2) << 0)
#define GPIO_PIN_SDIO_CLK_0     GPIO_PIN(0)
#define GPIO_MUX_SDIO_CMD_1     (3u << 2)
#define GPIO_MASK_SDIO_CMD_1    (BITMASK(2) << 2)
#define GPIO_PIN_SDIO_CMD_1     GPIO_PIN(1)
#define GPIO_MUX_SDIO_DAT_0_2   (3u << 4)
#define GPIO_MASK_SDIO_DAT_0_2  (BITMASK(2) << 4)
#define GPIO_PIN_SDIO_DAT_0_2   GPIO_PIN(2)
#define GPIO_MUX_SDIO_DAT_1_3   (3u << 6)
#define GPIO_MASK_SDIO_DAT_1_3  (BITMASK(2) << 6)
#define GPIO_PIN_SDIO_DAT_1_3   GPIO_PIN(3)
#define GPIO_MUX_SDIO_DAT_2_4   (3u << 8)
#define GPIO_MASK_SDIO_DAT_2_4  (BITMASK(3) << 8)
#define GPIO_PIN_SDIO_DAT_2_4   GPIO_PIN(4)
#define GPIO_MUX_SDIO_DAT_3_5   (3u << 11)
#define GPIO_MASK_SDIO_DAT_3_5  (BITMASK(3) << 11)
#define GPIO_PIN_SDIO_DAT_3_5   GPIO_PIN(5)
#define GPIO_MUX_SPI_CLK_0      (1u << 0)
#define GPIO_MASK_SPI_CLK_0     (BITMASK(2) << 0)
#define GPIO_PIN_SPI_CLK_0      GPIO_PIN(0)
#define GPIO_MUX_SPI_CLK_6      (0u << 11)
#define GPIO_MASK_SPI_CLK_6     (BITMASK(0) << 11)
#define GPIO_PIN_SPI_CLK_6      GPIO_PIN(6)
#define GPIO_MUX_SPI_CLK_N_0    (2u << 0)
#define GPIO_MASK_SPI_CLK_N_0   (BITMASK(2) << 0)
#define GPIO_PIN_SPI_CLK_N_0    GPIO_PIN(0)
#define GPIO_MUX_SPI_CS_0_5     (1u << 11)
#define GPIO_MASK_SPI_CS_0_5    (BITMASK(3) << 11)
#define GPIO_PIN_SPI_CS_0_5     GPIO_PIN(5)
#define GPIO_MUX_SPI_D_0_1      (1u << 2)
#define GPIO_MASK_SPI_D_0_1     (BITMASK(2) << 2)
#define GPIO_PIN_SPI_D_0_1      GPIO_PIN(1)
#define GPIO_MUX_SPI_D_1_2      (1u << 4)
#define GPIO_MASK_SPI_D_1_2     (BITMASK(2) << 4)
#define GPIO_PIN_SPI_D_1_2      GPIO_PIN(2)
#define GPIO_MUX_SPI_D_2_3      (1u << 6)
#define GPIO_MASK_SPI_D_2_3     (BITMASK(2) << 6)
#define GPIO_PIN_SPI_D_2_3      GPIO_PIN(3)
#define GPIO_MUX_SPI_D_3_4      (1u << 8)
#define GPIO_MASK_SPI_D_3_4     (BITMASK(3) << 8)
#define GPIO_PIN_SPI_D_3_4      GPIO_PIN(4)
#define GPIO_MUX_SPI_D_0_7      (0u << 11)
#define GPIO_MASK_SPI_D_0_7     (BITMASK(0) << 11)
#define GPIO_PIN_SPI_D_0_7      GPIO_PIN(7)
#define GPIO_MUX_SPI_D_1_8      (0u << 11)
#define GPIO_MASK_SPI_D_1_8     (BITMASK(0) << 11)
#define GPIO_PIN_SPI_D_1_8      GPIO_PIN(8)
#define GPIO_MUX_SPI_D_2_9      (0u << 11)
#define GPIO_MASK_SPI_D_2_9     (BITMASK(0) << 11)
#define GPIO_PIN_SPI_D_2_9      GPIO_PIN(9)
#define GPIO_MUX_SPI_D_3_10     (0u << 11)
#define GPIO_MASK_SPI_D_3_10    (BITMASK(0) << 11)
#define GPIO_PIN_SPI_D_3_10     GPIO_PIN(10)
#define GPIO_MUX_SPI_SLV_CS_5   (0u << 11)
#define GPIO_MASK_SPI_SLV_CS_5  (BITMASK(3) << 11)
#define GPIO_PIN_SPI_SLV_CS_5   GPIO_PIN(5)
#define GPIO_MUX_SPI_SLV_D_0_1  (0u << 2)
#define GPIO_MASK_SPI_SLV_D_0_1 (BITMASK(2) << 2)
#define GPIO_PIN_SPI_SLV_D_0_1  GPIO_PIN(1)
#define GPIO_MUX_SPI_SLV_D_1_2  (0u << 4)
#define GPIO_MASK_SPI_SLV_D_1_2 (BITMASK(2) << 4)
#define GPIO_PIN_SPI_SLV_D_1_2  GPIO_PIN(2)
/* ** END GENERATED CODE ** */

void
os_gpio_mux_sel(unsigned int sel,
                unsigned int pin);

void
os_gpio_mux_setup(uint32_t mask,
                  uint32_t mux);

bool
os_pin_request(os_gpio_set_t gpios);

void
os_pin_release(os_gpio_set_t gpios);

bool
os_gpio_request(os_gpio_set_t gpios);

void
os_gpio_free(os_gpio_set_t gpios);

void
os_gpio_set_input(os_gpio_set_t gpios);

void
os_gpio_set_output(os_gpio_set_t gpios);

void
os_gpio_set_value(os_gpio_set_t gpios_hi,
                  os_gpio_set_t gpios_lo);

void
os_gpio_set_pin(os_gpio_set_t gpios);

void
os_gpio_clr_pin(os_gpio_set_t gpios);

void
os_gpio_toggle_pin(os_gpio_set_t gpios);

uint32_t
os_gpio_get_value(os_gpio_set_t gpios);

void
os_gpio_enable_irq(os_gpio_set_t gpios,
                   unsigned int irqnum);

void
os_gpio_disable_irq(os_gpio_set_t gpios);

void
os_gpio_set_irq_edge_rising(os_gpio_set_t gpios);

void
os_gpio_set_irq_edge_falling(os_gpio_set_t gpios);

void
os_gpio_set_irq_edge_both(os_gpio_set_t gpios);

void
os_gpio_set_irq_level_low(os_gpio_set_t gpios);

void
os_gpio_set_irq_level_high(os_gpio_set_t gpios);

void
os_gpio_set_hold(os_gpio_set_t gpios);

void
os_gpio_clr_hold(os_gpio_set_t gpios);

void
os_gpio_clr_pull(os_gpio_set_t gpios);

void
os_gpio_set_pull(os_gpio_set_t gpios);

void
os_gpio_set_drive(os_gpio_set_t gpios);

void
os_gpio_clr_drive(os_gpio_set_t gpios);

/** @} */
