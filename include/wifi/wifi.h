/**
 * @addtogroup wifi_core
 * @{
 */
#pragma once
#include <assert.h>
#include <kernel/packet.h>
#include <kernel/notify.h>
#include <wifi/ieee80211.h>
#include <wifi/rates.h>
#include <wifi/scan.h>
#include <wifi/ssid.h>
#include <wifi/counters.h>

#define WIFI_RXI_MAGIC 0xabcd1234
#define WIFI_TXI_MAGIC 0xabcd5678

#define DEFAULT_TX_TIMEOUT 500 /* in ms, dot11MaxTransmitMSDULifetime */

enum wifi_tx_flags {
    /** TX muilticast flag (addr1 is multicast address) */
    WIFI_TXFLAG_MCAST    = BIT(0),
    /** TX without expecting an ACK */
    WIFI_TXFLAG_NOACK    = BIT(1),
    /** Expire time has been set */
    WIFI_TXFLAG_EXPIRES_SET = BIT(2),
    /** TX with encryption */
    WIFI_TXFLAG_ENCRYPT  = BIT(4),
    /** TX buffer is EAPOL frame */
    WIFI_TXFLAG_EAPOL    = BIT(5),
    /** Send this frame after all ordinary frames */
    WIFI_TXFLAG_LAST     = BIT(6),
    /** Set duration field for this frame */
    WIFI_TXFLAG_DURATION = BIT(9),
    /** The frame is a control frame */
    WIFI_TXFLAG_CONTROL  = BIT(10),
    /** BAR frame injected by the COP */
    WIFI_TXFLAG_COP_BAR  = BIT(11),
    /** BAR frame created in main CPU */
    WIFI_TXFLAG_STA_BAR  = BIT(12),
    /** TX status: ACK received */
    WIFI_TXFLAG_RXACK    = BIT(14),
    /** TX status: Transmission timeout */
    WIFI_TXFLAG_TIMEOUT  = BIT(15),
    /** TX status: Retry count exceeded */
    WIFI_TXFLAG_RCE      = BIT(16),
    /** TX status: Encryption succeeded */
    WIFI_TXFLAG_CRYPTO_OK = BIT(17),
    /** The frame will be reused, no need to free */
    WIFI_TXFLAG_NO_FREE = BIT(18),
};

enum wifi_tx_protection {
    TXI_PMODE_NONE,
    TXI_PMODE_RTS,
    TXI_PMODE_CTS
};

enum wifi_rate_list_type {
    /* All available rates */
    WIFI_RATE_LIST_ALL,
    /* Optimized for block ack agreement (BAA) mode,
     * with 11n (AMPDUs) rates for good radio conditions and
     * low 11b rates for poor radio conditions. */
    WIFI_RATE_LIST_BAA,
    WIFI_NUM_RATE_LIST_TYPES,
};

struct wifi_rate_list {
    struct wifi_tx_rate {
        /** Max number of retransmission attempts */
        uint8_t  txr_maxtries;
        /** Reserved field */
        uint8_t  txr_pformat:2; /* format for protection frame */
        uint8_t  txr_pindex:3;  /* index into ctlrsp */
        uint8_t  txr_pmode:2; /* TXI_PMODE_* */
        uint8_t  txr_wild_card:1;
        /** Transmission rate as defined by @ref rate_t */
        rate_t   txr_hwrate;
    } txr_try[5];
};

/**
 * Packet TX metadata used by coprocessor to transmit a packet.
 */
struct wifi_tx_info {
    /** Magic value */
    uint32_t txi_magic;
    /** Incremented on each transmission attempt. Used mostly for statistics. */
    uint8_t  txi_nxmit;
    /** Index of transmit queue */
    uint8_t  txi_queue;
    /** Frame priority */
    uint8_t  txi_aci:2;
    uint8_t  txi_reserved:6;
    uint8_t  txi_unused;
    /** Frame control field from 802.11 header (for debug purposes) */
    uint16_t txi_fc;
    /** Sequence number from 802.11 header (for debug purposes) */
    uint16_t txi_seq;
    /** Packet TX flags as defined by @ref wifi_tx_flags */
    uint32_t txi_flags;
    struct wifi_rate_list  *txi_rate_list;
    bool txi_rate_list_from_node; /* FIXME, intermediate solution */
    /** Expiration time for the transmission of this frame */
    uint32_t txi_expires;
    /** Timestamp when first symbol hit air interface (valid only after a
     * successul transmit) */
    uint32_t txi_timestamp;
    /** Timestamp of the received ACK (valid only when WIFI_TXFLAG_RXACK is set
     * in txi_flags) */
    uint32_t txi_acktime;
    /** Specifies which channel the frame will be transmitted to */
    const struct wifi_channel *txi_channel;
    /** Time that should be spent on the transmit channel after a successful
     * transmission (in microseconds) */
    uint32_t txi_channel_timeout;
    /** Reference to the @ref wifi_node that is the destination of this transmission */
    struct wifi_node *txi_node;
    /** Reference to the @ref wifi_media_user used by this transmission */
    struct wifi_media_user *txi_media;
    struct resource_ref *txi_media_traffic_ref;
    /** Transmit complete callback */
    void (*txi_callback) (struct packet *, void *);
    /** Transmit callback function argument */
    void            *txi_arg;
    /** Length of frame including FCS */
    size_t           txi_psdu_len;

    struct ba_context *txi_baa;
    uint32_t         txi_ampdu_delimiter;
    uint8_t          txi_start_spacing[8]; /* per-rate, in delimiters */
};


static inline void
wifi_txi_set_txi_expires(
    struct wifi_tx_info *txi,
    uint32_t expire)
{
    if((txi->txi_flags & WIFI_TXFLAG_EXPIRES_SET) == 0) {
        txi->txi_flags |= WIFI_TXFLAG_EXPIRES_SET;
        txi->txi_expires = os_systime() + expire;
    }
}

#define pkt_tx_info(_packet) \
    ({ struct wifi_tx_info *_txi = (_packet)->p_info; assert(_txi->txi_magic == WIFI_TXI_MAGIC); _txi; })


static inline void
wifi_rate_set_tx_rate(struct wifi_rate_list *rate_list,
                      unsigned int index,
                      unsigned int maxtries,
                      rate_t rate,
                      enum wifi_tx_protection protect_mode,
                      rate_t protect_rate,
                      bool wild_card)
{
    rate_list->txr_try[index].txr_maxtries = maxtries;
    rate_list->txr_try[index].txr_hwrate = rate;
    rate_list->txr_try[index].txr_pmode = protect_mode;
    rate_list->txr_try[index].txr_pformat = rate_to_format(protect_rate);
    rate_list->txr_try[index].txr_pindex = rate_to_index(protect_rate);
    rate_list->txr_try[index].txr_wild_card = wild_card ? 1 : 0;
}

/**
 * Flags for each received frame
 */
enum wifi_rx_flags {
    /** Set when the frame was received with good FCS */
    WIFI_RXI_FCS_OK     = BIT(0),
    /** Set when the destination address matches the interfaces MAC address */
    WIFI_RXI_ADDR_MATCH = BIT(1),
    /** Set when the destination address is a multicast or broadcast address */
    WIFI_RXI_MULTICAST  = BIT(2),
    /** Set when the frame was received as a part of an A-MPDU */
    WIFI_RXI_AMPDU      = BIT(3),
    /** Frame was originally protected */
    WIFI_RXI_PROTECTED  = BIT(4),
    /**
     * Set for packets containing a BEACON frame where the TIM information
     * element was processed and decoded by the coprocessor. This bit indicates
     * that the the value of bits @ref WIFI_RXI_TIM_MCAST and @ref
     * WIFI_RXI_TIM_UCAST are valid.
     */
    WIFI_RXI_TIM_VALID  = BIT(5),
    /** TIM information element indicates multicast traffic */
    WIFI_RXI_TIM_MCAST  = BIT(6),
    /** TIM information element indicates unicast traffic to us */
    WIFI_RXI_TIM_UCAST  = BIT(7),
    /** Received as AMPDU but no block ack agreement available */
    WIFI_RXI_AMPDU_WITHOUT_BA = BIT(8),
    /** Set when the frame reception was aborted due to lack of resources (for
     * example due lack of RX buffers) */
    WIFI_RXI_ABORT      = BIT(31)
};

enum wifi_rx_abort_reason {
    WIFI_RXI_ABORT_NONE,
     /** Frame was deliberately cut short to conserve power. */
    WIFI_RXI_ABORT_EARLY_BEACON,
    /** Gain changed during reception. Likely a collision.  */
    WIFI_RXI_ABORT_GAIN,
    /** Memory shortage. */
    WIFI_RXI_ABORT_MEMORY,
    /** Frame is multicast but not broadcast. */
    WIFI_RXI_ABORT_MULTICAST,
};

/**
 * Packet RX information inserted by coprocessor to hold packet metadata.
 */
struct wifi_rx_info {
    uint32_t rxi_magic;
    /** Packet flags as defined by @ref wifi_rx_flags */
    uint32_t rxi_flags;
    /** Reason for WIFI_RXI_ABORT, see enum wifi_rx_abort_reason */
    uint8_t  rxi_abort_reason;
    /** Received RSSI */
    int8_t   rxi_rssi;
    /** Received rate in units of 500kbps */
    rate_t   rxi_rate;
    /** Packet signal-to-noise ratio */
    int16_t  rxi_snr;
    /* Length of frame in microseconds */
    uint16_t rxi_duration;
    /* Key used to decrypt packet */
    struct ieee80211_key *rxi_key;
    /* Cipher packet number */
    uint64_t rxi_pn;
    /** Storage available to whoever has this frame queued */
    uint32_t rxi_misc;
    /** Timestamp when first symbol was received by air interface */
    uint32_t rxi_timestamp;
};

#define pkt_rx_info(_packet) \
    ({ struct wifi_rx_info *_rxi = (_packet)->p_info; assert(_rxi->rxi_magic == WIFI_RXI_MAGIC); _rxi; })

#define wifi_rxi_fcs_ok(_rxi) \
    (((_rxi)->rxi_flags & WIFI_RXI_FCS_OK) != 0)

#define wifi_rxi_to_us(_rxi) \
    (((_rxi)->rxi_flags & (WIFI_RXI_ADDR_MATCH | WIFI_RXI_MULTICAST)) != 0)

static inline void
wifi_rxi_set_abort_reason(struct wifi_rx_info *rxi,
                          enum wifi_rx_abort_reason reason)
{
    rxi->rxi_abort_reason = reason;
    rxi->rxi_flags |= WIFI_RXI_ABORT;
}

static inline enum wifi_rx_abort_reason
wifi_rxi_get_abort_reason(struct wifi_rx_info *rxi)
{
    if(rxi->rxi_flags & WIFI_RXI_ABORT)
        return rxi->rxi_abort_reason;
    return WIFI_RXI_ABORT_NONE;
}

/**
 * @struct wifi_media_user
 *
 * Opaque struct representing a request to use the media for transmit or receive.
 */
struct wifi_media_user;

/**
 * @struct wifi_dev
 *
 * This struct is representing the WiFi device.
 */
struct wifi_dev;

/**
 * @struct wifi_net
 *
 * This struct is representing a WiFi network (or interface).
 */
struct wifi_net;

/**
 * @struct wifi_node
 *
 * This struct is representing a node (aka station) in a WiFi network.
 */
struct wifi_node;

/**
 * @struct wifi_channel
 *
 * This struct is representing a WiFi channel.
 */
struct wifi_channel;

/**
 * Enumeration for supported WiFi operation modes
 */
enum wifi_opmode {
    WIFI_MODE_STA,
    WIFI_MODE_P2P,
    WIFI_MODE_HOSTAP,
    WIFI_MODE_MONITOR,
    WIFI_MODE_SCAN
};

/**
 * List of supported cipher types.
 *
 * Specified setting up the network key, to indicate what type of cipher that
 * will be used.
 */
enum wifi_citype {
    WIFI_CIPHER_NONE,
    WIFI_CIPHER_RESERVED,
    WIFI_CIPHER_TKIP,
    WIFI_CIPHER_CCMP,
    WIFI_CIPHER_BIP,
    WIFI_CIPHER_MAX
};

/**
 * Definition for the WiFi client interface.
 *
 * The WiFi interface client registers an instance of this interface to be able
 * to receive frames from the WiFi interface. The interface consists of a set
 * of callback functions invoked by the WiFi stack. Each of the callbacks gets
 * a pointer to the instance if the interface as the first argument.r
 * @see wifi_client_attach
 *
 */
struct wifi_client {
    /**
     * This callback is used to pass frames to the WiFi client.
     */
    void (*wc_input)(const struct wifi_client *wc, struct wifi_node *node, struct packet *pkt);
    /**
     * This callback is used to notify the client about results from an ongoing
     * scan operation. The callback receives a pointer to a @ref wifi_netinfo
     * struct with information about the detected network.
     */
    void (*wc_scan_notify)(const struct wifi_client *wc, struct wifi_netinfo *ni);
    /**
     * Notification to the client when a scan operation is completed.
     */
    void (*wc_scan_done)(const struct wifi_client *wc);
};

int wifi_client_attach(struct wifi_net *net, const struct wifi_client *wc);
void wifi_null_input(const struct wifi_client *wc, struct wifi_node *node, struct packet *pkt);

extern const uint8_t ieee80211_broadcastaddr[];
extern const uint8_t ieee80211_zeroaddr[];

int  ieee80211_addr_eq(const uint8_t *a1, const uint8_t *a2);
void ieee80211_addr_copy(uint8_t *dst, const uint8_t *src);
bool ieee80211_valid_hwaddr(const uint8_t *addr);

struct wifi_net *wifi_if_create(enum wifi_opmode mode, const uint8_t *hwaddr);
struct wifi_dev *wifi_if_dev(struct wifi_net *net);
int8_t wifi_get_txpower(struct wifi_net *net);
int8_t wifi_get_txpower_vbm(struct wifi_net *net);
void wifi_set_txpower(struct wifi_net *net, int8_t tx_power);
void wifi_set_txpower_vbm(struct wifi_net *net, int8_t tx_power);
void wifi_mcast(struct wifi_net *net, bool enable);
void wifi_only_broadcast(struct wifi_net *net, bool enable);
void wifi_pause(struct wifi_net *net, bool enable);
void wifi_if_set_netif(struct wifi_net *net, void *netif);
void *wifi_if_get_netif(struct wifi_net *net);
void wifi_if_destroy(struct wifi_net *net);
const uint8_t *wifi_if_get_hwaddr(struct wifi_net *net);
bool wifi_if_set_hwaddr(struct wifi_net *net, const uint8_t *macaddr);
int wifi_raw_transmit(struct wifi_net *, struct wifi_node *, struct packet *);
int wifi_net_transmit(struct wifi_net *net, struct packet *pkt);
void wifi_cancel_traffic_timeout(struct wifi_net *net);
void wifi_notify_register(struct wifi_net *net, struct os_notify_entry *entry);
void wifi_notify_deregister(struct wifi_net *net, struct os_notify_entry *entry);
void wifi_if_read_counters(const struct wifi_net *net, struct wifi_counters *result, size_t size);
int wifi_set_channel_spec(struct wifi_net *net, const char *spec);

struct packet *wifi_alloc_txframe(const struct wifi_net *net, size_t headroom, size_t pktlen);
int wifi_add_callback(struct packet *pkt, void (*func) (struct packet *, void *), void *arg);
void wifi_dump_pkt(struct packet *pkt);

int wifi_get_dtim_period(const struct wifi_net *net);
int wifi_get_sleep_period(const struct wifi_net *net);

int wifi_set_key(struct wifi_net *net, enum wifi_citype,
                 int index, uint64_t keyrsc, const uint8_t *addr,
                 const uint8_t *key, size_t key_len);
void wifi_mfp_enable(struct wifi_net *net);

/* FIXME: This enum is placed in the WCM documentation due to lack of better
 * place right now. */
/**
 * @ingroup wifi_wcm
 * Power management flags for WiFi
 */
enum wifi_pm_flags {
    /** Send ps poll if a beacon was missed. This will increase power
     * consumption slightly but decrease latency. */
    WIFI_PM_PS_POLL         = BIT(0),
    /** Dynamic listen interval. Listen to each beacon if there has been
     * traffic recently */
    WIFI_PM_DYN_LISTEN_INT  = BIT(1)
};

int wifi_set_pm(struct wifi_net *net, uint32_t listen_interval, uint32_t traffic_tmo, uint32_t pm_flags);
int wifi_set_channel(struct wifi_net *net, uint32_t channel);
int wifi_scan(struct wifi_net *net, const struct wifi_scan_param *scan_req);
void wifi_init_scanreq(struct wifi_scan_param *scan_req);


uint32_t ieee80211_txtime(size_t num_bytes, rate_t rate, bool partial);


void wifi_net_set_msdu_lifetime(struct wifi_net *net, uint32_t lifetime_ms);
uint32_t wifi_net_get_msdu_lifetime_us(struct wifi_net *net);

/** @} */
