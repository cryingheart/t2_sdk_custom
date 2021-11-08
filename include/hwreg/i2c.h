#pragma once
#include <stdint.h>

struct hw_i2c {
    uint8_t                        numerator;
    uint8_t                        denominator;
    uint8_t                        divider_lo;
    uint8_t                        divider_hi;
    uint8_t                        ufm;
    uint16_t                       cmd_fifo;
#define I2C_CMD_FIFO_START         0x100
#define I2C_CMD_FIFO_STOP          0x200
#define I2C_CMD_FIFO_TX            0x400
#define I2C_CMD_FIFO_ACK           0x800
    uint8_t                        rx_fifo;
    uint8_t                        status;
#define I2C_STATUS_CMD_DONE        0x01
#define I2C_STATUS_NACK            0x02
#define I2C_STATUS_ARB_LOST        0x04
#define I2C_STATUS_BUS_BUSY        0x08
#define I2C_STATUS_CMD_EMPTY       0x10
#define I2C_STATUS_CMD_FULL        0x20
#define I2C_STATUS_RXD_EMPTY       0x40
#define I2C_STATUS_RXD_FULL        0x80
    uint8_t                        irq_en;
};
