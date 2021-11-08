/*
 * Wireless Connection Manager API for the HIO interface
 */
#pragma once
#include <wifi/wcm.h>

void wcm_hio_init_internal(void);
/**
 * @addtogroup hioapi_wcm
 * @{
 */

/**
 * Initiate and register the hio module for WCM (WiFi Connection Manager)
 */
static inline void
wcm_hio_init(void)
{
#ifdef ENABLE_WPA3
    /* If ENABLE_WPA3 is not defined the linker will not include the WPA3/SAE
     * functions. */
    wcm_sae_enable();
#endif
    wcm_hio_init_internal();
}
/** @} */

bool wcm_hio_set_pm(uint32_t listen_interval, uint32_t traffic_tmo, uint32_t pm_flags);
