/**
 * @addtogroup drv_i2c
 * @{
 */
#pragma once
#include <stddef.h>
#include <kernel/bitops.h>

struct i2c_bus;
struct i2c_device;

#define I2C_CLK_100K   100000
#define I2C_CLK_400K   400000
#define I2C_CLK_1M    1000000

/**
 * I2C message
 */
struct i2c_msg {
    /** Number of bytes requested to be transfered */
    uint16_t    im_len;
    /** Number of bytes actually transfered */
    uint16_t    im_xfr;
    /** Flags that modifies the behaviour of the transfer */
    uint16_t    im_flags;
#define I2C_M_RD                BIT(0)  /** read data, from slave to master */
#define I2C_M_ADDR10            BIT(1)  /** use 10 bit slave address */
#define I2C_M_STOP              BIT(2)  /** Send stop condition at end of message */
#define I2C_M_NOSTART           BIT(3)  /** Don't send start condition before message */
#define I2C_M_IGNORE_NAK        BIT(4)  /** Ignore NAK from slave device */
#define I2C_M_NO_RD_ACK         BIT(5)
#define I2C_M_RECV_LEN          BIT(6)  /** Length is the first received byte from slave */
    /** Pointer to message bytes */
    uint8_t    *im_buf;
};

/* I2C BUS */
struct i2c_bus *i2c_bus_init(int busno);
void i2c_acquire_bus(struct i2c_bus *bus, struct i2c_device *dev);
void i2c_release_bus(struct i2c_bus *bus);

/* I2C DEVICE */
struct i2c_device *i2c_create_device(struct i2c_bus *bus, uint16_t addr, uint32_t max_freq);
void i2c_destroy_device(struct i2c_device *dev);
int i2c_set_address(struct i2c_device *dev, int addr);
int i2c_transfer(struct i2c_device *dev, struct i2c_msg *msg, size_t len);

/** @} */
