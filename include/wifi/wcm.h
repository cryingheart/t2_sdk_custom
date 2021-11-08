/**
 * @addtogroup wifi_wcm
 * @{
 */
#pragma once
#include <wifi/wifi.h>
#include <lwip/ip_addr.h>

/**
 * @struct wcm_handle
 */
struct wcm_handle;

/**
 * WCM notification callback function pointer
 */
typedef void (*wcm_notify_cb)(void *ctx, struct os_msg *msg);

/**
 * Message types of WCM notify message callbacks
 */
enum wcm_notify_msg_type {
     /** WiFi link is up */
    WCM_NOTIFY_MSG_LINK_UP   = 200,
    /** WiFi link is down */
    WCM_NOTIFY_MSG_LINK_DOWN = 201,
    /** Address has changed. The message sent is of type @ref wcm_address_event */
    WCM_NOTIFY_MSG_ADDRESS   = 202,
    /** WiFi disconnect complete  */
    WCM_NOTIFY_MSG_DISCONNECT_DONE = 203,
    /** WiFi connect complete  */
    WCM_NOTIFY_MSG_CONNECTED = 204,
};

/**
 * Address changed event message
 */
struct wcm_address_event {
    /** Message header */
    struct os_msg iev_hdr;
    /** Typically AF_INET or AF_INET6 */
    unsigned int  iev_af;
    /** The address of up to 16 bytes (IPv6) */
    unsigned char iev_address[0];
};

struct wcm_in6_addr {
    union {
        uint8_t   s6_addr8[16];
        uint16_t  s6_addr16[8];
    };
} __packed;

struct wcm_handle * wcm_create_internal(const uint8_t *hwaddr);
void wcm_sae_enable(void);

/* cloudpm hook
 * NOTE: DO NOT use/call this function. It's called internally if
 * ENABLE_CLOUD_PM is defined.
 */
void wcm_cloud_pm_init(void);

/*cloudpm hooks*/
void wcm_cloud_pm_enable(bool enable);
uint32_t wcm_cloud_pm_get_ka_period(void);
bool wcm_cloud_pm_set_ka_period(const uint32_t ka_period);
bool wcm_cloud_pm_set_default_ipdest(const ip_addr_t* ip);
bool wcm_cloud_pm_set_pub_period(const uint32_t pub_period);
/**
 * Create a WiFi network interface
 *
 * @return Pointer to @ref wcm_handle
 */
static inline struct wcm_handle *
wcm_create(const uint8_t *hwaddr)
{
#ifdef ENABLE_WPA3
    /* If ENABLE_WPA3 is not defined the linker will not include the WPA3/SAE
     * functions. */
    wcm_sae_enable();
#endif

#ifdef ENABLE_CLOUD_PM
    /* Excludes ENABLE_CLOUD_PM functions if it is not defined */
    wcm_cloud_pm_init();
#endif

    return wcm_create_internal(hwaddr);
}

void wcm_destroy(struct wcm_handle *h);
int wcm_add_network(struct wcm_handle *h, const char *ssid, const uint8_t* bssid, const char *passphrase);
struct wifi_uri;
int wcm_add_network_uri(struct wcm_handle *h, const struct wifi_uri *uri);
int wcm_set_sae_password(struct wcm_handle *h, const char *password);
int wcm_remove_network(struct wcm_handle *h, const char *ssid, const uint8_t* bssid);
void wcm_set_scanfilter(struct wcm_handle *h, void *ctx, bool (*fn)(void *, const struct wifi_netinfo *));
int wcm_scan(struct wcm_handle *h, const struct wifi_scan_param *param, struct wifi_netinfo **result, size_t max);
void wcm_free_scanresult(struct wifi_netinfo **result, size_t num);
bool wcm_get_netif_addr4(const struct wcm_handle *h, uint32_t *ipaddr, uint32_t *netmask, uint32_t *gw, uint32_t *dns);
bool wcm_set_netif_addr4(struct wcm_handle *h, uint32_t ipaddr4, uint32_t netmask, uint32_t gw, uint32_t dns);
bool wcm_get_netif_addr6(const struct wcm_handle *h, struct wcm_in6_addr *wcm_addr6, int addr6_idx);
bool wcm_set_netif_addr6(struct wcm_handle *h, struct wcm_in6_addr *wcm_addr6, int addr6_idx);
void wcm_notify_enable(struct wcm_handle *h, wcm_notify_cb callback, void *ctx);
void wcm_pm_config(struct wcm_handle *h, uint32_t listen_interval, uint32_t traffic_tmo, uint32_t pm_flags);
void wcm_pm_config_get(struct wcm_handle *h, uint32_t *listen_interval, uint32_t *traffic_tmo, uint32_t *pm_flags);
void wcm_mcast(struct wcm_handle *h, bool enable);
void wcm_only_broadcast(struct wcm_handle *h, bool enable);
void wcm_get_counters(struct wcm_handle *h, struct wifi_counters *out, size_t size);
int wcm_get_rssi(struct wcm_handle *h);
int8_t wcm_get_txpower(struct wcm_handle *h);
void wcm_set_txpower(struct wcm_handle *h, int8_t tx_power);
int wcm_get_msdu_lifetime_us(struct wcm_handle *h);
int wcm_set_msdu_lifetime(struct wcm_handle *h, uint32_t msdu_lifetime_ms);
struct netif *wcm_get_netif(const struct wcm_handle *h);
const uint8_t *wcm_get_hwaddr(struct wcm_handle *h);
void wcm_get_bssid(struct wcm_handle *h, uint8_t *bssid /* output */);
int wcm_auto_connect(struct wcm_handle *h, bool enabled);
void wcm_print_ip(struct wcm_handle*);
void wcm_arp_grat_enable(struct wcm_handle *h, unsigned int period);
void wcm_arp_grat_disable(struct wcm_handle *h);
bool wcm_pm_get_dtim(struct wcm_handle *h, int *dtim);
bool wcm_pm_get_sleep_period(struct wcm_handle *h, int *sleep_period);

enum wcm_mfp_mode {
    WCM_MFP_ENABLED, /*!< use protected management frames if possible */
    WCM_MFP_REQUIRED /*!< only connect to APs that support MFP */
};

int wcm_set_mfp_mode(struct wcm_handle *h, enum wcm_mfp_mode mode);
int wcm_set_channel_spec(struct wcm_handle *h, const char *domain);

struct wcm_status {
    int32_t  rssi;
    int32_t  channel;
    int32_t  link_up;
    uint8_t  bssid[IEEE80211_ADDR_LEN];
};

void wcm_get_status(struct wcm_handle *h, struct wcm_status *output);

/** @} */
