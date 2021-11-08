#pragma once
#include <stdint.h>
#include <kernel/i2c.h>

struct i2c_device *ads_init(int busno);
int ads_reset(struct i2c_device *dev);
int ads_enable(struct i2c_device *dev);
int ads_write(struct i2c_device *dev, unsigned reg, uint16_t value);
int ads_read(struct i2c_device *dev, unsigned reg, uint16_t *value);
