#pragma once
#include <stdint.h>

#define HKADC_SRC_TEMP 3
#define HKADC_SRC_VBAT 5
#define HKADC_SRC_EXT  6

#define HKADC_CTRL_RATE_MASK (3 << 0)
#define HKADC_CTRL_40M       (0 << 0)
#define HKADC_CTRL_20M       (1 << 0)
#define HKADC_CTRL_10M       (2 << 0)
#define HKADC_CTRL_5M        (3 << 0)

#define HKADC_CTRL_AVG_EN    (1 << 4)
#define HKADC_CTRL_AVG_MASK  (3 << 5)
#define HKADC_CTRL_AVG2      ((0 << 5) | HKADC_CTRL_AVG_EN)
#define HKADC_CTRL_AVG4      ((1 << 5) | HKADC_CTRL_AVG_EN)
#define HKADC_CTRL_AVG8      ((2 << 5) | HKADC_CTRL_AVG_EN)
#define HKADC_CTRL_AVG16     ((3 << 5) | HKADC_CTRL_AVG_EN)

#define HKADC_CTRL_SINGLE    (1 << 8)

void hkadc_enable(uint32_t ctrl);
void hkadc_disable(void);
void hkadc_select(uint32_t src);
uint32_t hkadc_read(void);
