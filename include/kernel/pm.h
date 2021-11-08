#pragma once
#include <kernel/resource.h>

void os_pm_enable_hfc(struct resource*);
void os_pm_disable_hfc(struct resource*);

/**
 * @addtogroup pm
 * @{
 *
 *
 */

enum vddio_vset {
    VDDIO_VSET_20V,
    VDDIO_VSET_25V,
    VDDIO_VSET_28V,
    VDDIO_VSET_30V,
};

void
pm_set_vddio(enum vddio_vset vset);

void os_pm_set_ct_cal(const uint8_t ct_cal[2]);

/** @} */
