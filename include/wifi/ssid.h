/**
 * @addtogroup wifi_wcm
 * @{
 */
#pragma once

#include <stdint.h>
#include <wifi/ieee80211.h>
#include <wifi/ie_iter.h>

struct wifi_ssid {
    uint8_t ws_len;
    /* an SSID should be treated as binary data, but since much code
     * assumes it to be a string, we leave room for zero
     * termination */
    char    ws_ssid[IEEE80211_NWID_LEN+1];
};

int wifi_ssid_from_bytes(struct wifi_ssid *ssid,
                         const void *ssid_bytes,
                         size_t ssid_length);

int wifi_ssid_from_ie(struct wifi_ssid *ssid,
                      struct ie_iterator *ie);

int wifi_ssid_cmp(const struct wifi_ssid *a,
                  const struct wifi_ssid *b);

/** @} */
