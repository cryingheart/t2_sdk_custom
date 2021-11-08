/**
 * @addtogroup wifi_wcm
 * @{
 */
#pragma once

#include <stdint.h>
#include <kernel/refcnt.h>
#include <kernel/queue.h>
#include <wifi/rates.h>
#include <wifi/ssid.h>

struct wifi_netinfo;

/**
 * @typedef rate_t
 * Set 0x00 for 1Mb/s and 0x10 for 6Mb/s
 */

/**
 * Parameters for WiFi scan operation
 */
struct wifi_scan_param {
    /** Max number of probes to send in active scan.*/
    uint8_t num_probes;
    /** Max number of idle slots to decide if we should keep listening*/
    uint8_t idleslots;
    /** List of channels to scan */
    uint8_t channel_mask[8];
    /** Destination address and BSSID for probe requests */
    uint8_t bssid[IEEE80211_ADDR_LEN];
    /** Rate to use for sending probe requests */
    rate_t  txrate;
    /** Scan for specific SSID (set to emptry string for ANY). */
    struct wifi_ssid ssid;
    /** The minimum amount of time (in milliseconds) to listen for probe responses on the channel after
     * transmitting the probe request */
    uint32_t min_listen_time;
    /** The maximum amount of time (in milliseconds, including listen and probe requests) to stay on the channel */
    uint32_t max_listen_time;
    /** The timeout (in milliseconds) after which if a 2nd probe request is not tramitted,  it should be aborted. */
    uint32_t probe_tx_timeout;
    /** Idle time between each channel (giving other parties access to the
     * media) */
    uint32_t wait_time;
    /** Max number of scan probe response results */
    uint32_t max_responses;
    /* Pointer to scan filter function */
    bool (*fn_filter)(void *, const struct wifi_netinfo *);
    /** Length of optional extra information elements included in the probe
     * request frames */
    size_t  ie_len;
    /** Buffer with information elements that will be inserted in each probe
     * request frame. */
    uint8_t ie_list[0];
};

/**
 * Struct holding the properties of a WiFi network.
 */
struct wifi_netinfo {
    /** Reference counter */
    struct refcnt              ni_ref;
    /** Entry in hash table */
    LIST_ENTRY(wifi_netinfo)   ni_hash;
    /** Beacon interval in TU */
    uint16_t                   ni_beacon_interval;
    /** Capabilities field */
    uint16_t                   ni_capabilities;
    /** Timestamp */
    uint64_t                   ni_timestamp;
    /** RSSI */
    int16_t                    ni_rssi;
    /** Frame control */
    uint16_t                   ni_fc;
    /** The networks BSSID */
    uint8_t                    ni_bssid[IEEE80211_ADDR_LEN];
    /* Active channel number, zero if unknown */
    uint8_t                    ni_channel;
    /* Cached pointer to SSID IE */
    const uint8_t              *ni_ssid;
    /** Length of IEEE 802.11 information elements */
    uint16_t                   ni_ielen;
    /** List of information elements (variable length) */
    uint8_t                    ni_ielist[0];
};


struct wifi_authmode {
#define IE_NOT_SUPPORTED    0x0000
#define IE_WPA_PERSONAL     0x0001
#define IE_WPA2_PERSONAL    0x0002
#define IE_WPA3_PERSONAL    0x0004
#define IE_WPA_ENTERPRISE   0x0008
#define IE_WPA2_ENTERPRISE  0x0010
#define IE_WPA3_ENTERPRISE  0x0020
#define IE_WPA2_MGMT_FR_PR  0x0040
#define IE_PROTECT_BIT_CAP  0x0080
    uint32_t authmask;
};

/* BEGIN GENERATED PROTOS */

struct wifi_netinfo* wifi_netinfo_incref(struct wifi_netinfo* ni);

void wifi_netinfo_decref(struct wifi_netinfo* ni);

int
wifi_netinfo_get_ssid(const struct wifi_netinfo* ni,
                      struct wifi_ssid* ssid);

int
wifi_netinfo_get_chan(const struct wifi_netinfo* ni,
                      uint8_t* chan);

int
wifi_netinfo_get_authmode(const struct wifi_netinfo* ni,
                          struct wifi_authmode* authmode);

size_t
wifi_netinfo_authmode_tostr(uint32_t authmask,
                            char* mode_name,
                            size_t size);

void wifi_init_scan_default(struct wifi_scan_param* param);

/* END GENERATED PROTOS */

/** @} */
