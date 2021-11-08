/**
 * @addtogroup drv_pwm
 * @{
 */
#pragma once
#include <stddef.h>
#include <stdint.h>

/**
 * Opaque object representing a PWM device.
 */
struct pwm_device;

struct pwm_device *pwm_create(int channel, uint32_t period, uint32_t width);
void pwm_destroy(struct pwm_device *pwm);
void pwm_set_period(struct pwm_device *pwm, uint32_t period);
void pwm_set_width(struct pwm_device *pwm, uint32_t width);
uint32_t pwm_get_period(const struct pwm_device *pwm);
uint32_t pwm_get_width(const struct pwm_device *pwm);

/** @} */
