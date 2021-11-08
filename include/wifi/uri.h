#pragma once
#include <stdbool.h>
#include <kernel/cipher_buffer.h>
#include <wifi/wifi.h>
#include <wifi/transition_disable.h>

enum wifi_uri_type {
    WIFI_URI_UNKNOWN,
    WIFI_URI_WPA /* WPA-Personal */
};

struct wifi_uri {
    enum wifi_uri_type    wu_type;
    unsigned int          wu_trdisable;
    bool                  wu_hidden;
    uint8_t               wu_bssid[IEEE80211_ADDR_LEN];
    struct wifi_ssid      wu_ssid;
    struct cipher_buffer  wu_password;
    char                 *wu_password_id;
};

/* BEGIN GENERATED PROTOS */

int wifi_uri_init(struct wifi_uri* uri);

int wifi_uri_release(struct wifi_uri* uri);

int wifi_uri_parse(struct wifi_uri* uri, const char* string);

/* END GENERATED PROTOS */
