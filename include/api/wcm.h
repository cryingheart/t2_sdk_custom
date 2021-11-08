/*
 * Wireless Connection Manager API
 */
#pragma once
#include <string.h>
#include <kernel/hostio.h>

/*
[[[cog
import cog, generate
generate.doxygen(generate.Emitter(cog.out), 'wcm')
]]] */
/**
 * @addtogroup hioapi_wcm
 * @{
 *
 * This is an interface to the WIFI Connection Manager. This interface containts operations for:
 *
 * <ul>
 *   <li> creating and destroying interfaces </li>
 *   <li> scanning </li>
 *   <li> connecting and disconnecting </li>
 *   <li> querying connection information </li>
 * </ul>
 *
 * <table>
 * <caption>Message summary (group=2)</caption>
 * <tr><th>OPERATION</th><th>REQ</th><th>RSP</th><th>IND</th></tr>
 * <tr><td>if_create</td><td>0x00 - wcm_if_create_req</td><td>0x80 - wcm_if_create_rsp</td><td></td></tr>
 * <tr><td>if_destroy</td><td>0x01 - wcm_if_destroy_req</td><td>0x81 - wcm_if_destroy_rsp</td><td></td></tr>
 * <tr><td>if_notify</td><td></td><td></td><td>0xc2 - wcm_if_notify_ind</td></tr>
 * <tr><td>if_scan</td><td>0x03 - wcm_if_scan_req</td><td>0x83 - wcm_if_scan_rsp</td><td>0xc3 - wcm_if_scan_ind</td></tr>
 * <tr><td>if_connect</td><td>0x04 - wcm_if_connect_req</td><td>0x84 - wcm_if_connect_rsp</td><td></td></tr>
 * <tr><td>if_disconnect</td><td>0x05 - wcm_if_disconnect_req</td><td>0x85 - wcm_if_disconnect_rsp</td><td></td></tr>
 * <tr><td>if_set_addr4</td><td>0x06 - wcm_if_set_addr4_req</td><td>0x86 - wcm_if_set_addr4_rsp</td><td></td></tr>
 * <tr><td>if_set_addr6</td><td>0x07 - wcm_if_set_addr6_req</td><td>0x87 - wcm_if_set_addr6_rsp</td><td></td></tr>
 * <tr><td>if_get_addr4</td><td>0x08 - wcm_if_get_addr4_req</td><td>0x88 - wcm_if_get_addr4_rsp</td><td></td></tr>
 * <tr><td>if_get_addr6</td><td>0x09 - wcm_if_get_addr6_req</td><td>0x89 - wcm_if_get_addr6_rsp</td><td></td></tr>
 * <tr><td>if_set_pmconfig</td><td>0x0a - wcm_if_set_pmconfig_req</td><td>0x8a - wcm_if_set_pmconfig_rsp</td><td></td></tr>
 * <tr><td>if_get_rssi</td><td>0x0b - wcm_if_get_rssi_req</td><td>0x8b - wcm_if_get_rssi_rsp</td><td></td></tr>
 * <tr><td>if_get_counters</td><td>0x0c - wcm_if_get_counters_req</td><td>0x8c - wcm_if_get_counters_rsp</td><td></td></tr>
 * <tr><td>if_txpower</td><td>0x0d - wcm_if_txpower_req</td><td>0x8d - wcm_if_txpower_rsp</td><td></td></tr>
 * <tr><td>if_get_bssid</td><td>0x0e - wcm_if_get_bssid_req</td><td>0x8e - wcm_if_get_bssid_rsp</td><td></td></tr>
 * <tr><td>if_add_network</td><td>0x0f - wcm_if_add_network_req</td><td>0x8f - wcm_if_add_network_rsp</td><td></td></tr>
 * <tr><td>if_remove_network</td><td>0x10 - wcm_if_remove_network_req</td><td>0x90 - wcm_if_remove_network_rsp</td><td></td></tr>
 * <tr><td>if_auto_connect</td><td>0x11 - wcm_if_auto_connect_req</td><td>0x91 - wcm_if_auto_connect_rsp</td><td></td></tr>
 * <tr><td>if_add_network_uri</td><td>0x12 - wcm_if_add_network_uri_req</td><td>0x92 - wcm_if_add_network_uri_rsp</td><td></td></tr>
 * <tr><td>set_mfp_mode</td><td>0x13 - wcm_set_mfp_mode_req</td><td>0x93 - wcm_set_mfp_mode_rsp</td><td></td></tr>
 * <tr><td>testmode</td><td>0x14 - wcm_testmode_req</td><td>0x94 - wcm_testmode_rsp</td><td></td></tr>
 * <tr><td>set_regulatory_domain</td><td>0x15 - wcm_set_regulatory_domain_req</td><td>0x95 - wcm_set_regulatory_domain_rsp</td><td></td></tr>
 * </table>
 */
/* [[[end]]] */

/*
[[[cog
cog.outl('')
generate.header(generate.Emitter(cog.out), 'wcm')
cog.outl('')
]]] */

#define HIO_GROUP_WCM 2

/*
 * Message if_create
 */

#define WCM_IF_CREATE_REQ 0x00
#define WCM_IF_CREATE_RSP 0x80

/** Create a WiFi network interface. Request structure.
 */
struct wcm_if_create_req {
    uint8_t addr[6]; /**< MAC address of the WIFI interface. If an invalid MAC address is specified (e.g. all zero), the device will generate a random address. */
};

static inline struct packet *
alloc_wcm_if_create_req(struct wcm_if_create_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_create_req(const uint8_t addr[6])
{
    struct wcm_if_create_req *req;
    struct packet *pkt = alloc_wcm_if_create_req(&req);
    if (pkt) {
        memcpy(req->addr, addr, sizeof req->addr);
    }
    return pkt;
}

/** Create a WiFi network interface. Response structure.
 */
struct wcm_if_create_rsp {
    uint32_t handle; /**< interface handle of created interface */
    uint8_t hwaddr[6]; /**< The MAC address used by the interface */
};

static inline struct packet *
alloc_wcm_if_create_rsp(struct wcm_if_create_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_create_rsp(const uint32_t handle, const uint8_t hwaddr[6])
{
    struct wcm_if_create_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_create_rsp(&rsp);
    if (pkt) {
        rsp->handle = handle;
        memcpy(rsp->hwaddr, hwaddr, sizeof rsp->hwaddr);
    }
    return pkt;
}
/*
 * Message if_destroy
 */

#define WCM_IF_DESTROY_REQ 0x01
#define WCM_IF_DESTROY_RSP 0x81

/** Shutdown and cleanup a WiFi Connection Manager interface. Request structure.
 */
struct wcm_if_destroy_req {
    uint32_t handle; /**< interface handle */
};

static inline struct packet *
alloc_wcm_if_destroy_req(struct wcm_if_destroy_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_destroy_req(const uint32_t handle)
{
    struct wcm_if_destroy_req *req;
    struct packet *pkt = alloc_wcm_if_destroy_req(&req);
    if (pkt) {
        req->handle = handle;
    }
    return pkt;
}

/** Shutdown and cleanup a WiFi Connection Manager interface. Response structure.
 */
struct wcm_if_destroy_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_if_destroy_rsp(struct wcm_if_destroy_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_destroy_rsp(const int32_t status)
{
    struct wcm_if_destroy_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_destroy_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message if_notify
 */

#define WCM_IF_NOTIFY_IND 0xc2

/** Connection manager notification. Indication structure.
 */
struct wcm_if_notify_ind {
    uint32_t type; /**< Notification type: Link up (200),  Link down (201),  Address (202) */
    char attr[0]; /**< Notification data */
};

static inline struct packet *
alloc_wcm_if_notify_ind(struct wcm_if_notify_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_notify_ind(const uint32_t type, const char attr[0])
{
    struct wcm_if_notify_ind *ind;
    struct packet *pkt = alloc_wcm_if_notify_ind(&ind);
    if (pkt) {
        ind->type = type;
        strncpy(ind->attr, attr, 0);
    }
    return pkt;
}
/*
 * Message if_scan
 */

#define WCM_IF_SCAN_REQ 0x03
#define WCM_IF_SCAN_RSP 0x83
#define WCM_IF_SCAN_IND 0xc3

/** Scan for WiFi networks. Request structure.
 */
struct wcm_if_scan_req {
    uint32_t handle; /**< interface handle */
    char ssid[33]; /**< Zero terminated SSID to scan for, empty for broadcast scan. */
    uint8_t max; /**< Max number of responses to return. */
    uint16_t num_probes; /**< Max number of probes to send in active scan. */
    uint16_t idleslots; /**< Max number of idle slots to decide if we should keep listening */
    uint16_t min_listen_time; /**< The minimum amount of time (in milliseconds) to stay on the channel after transmitting the probe request and listening for responses. */
    uint16_t max_listen_time; /**< The maximum amount of time (in milliseconds) to stay on the channel after transmitting the probe request and listening for responses. */
    uint16_t probe_tx_timeout; /**< The time (in milliseconds) a (2nd) probe request is aborted if transmission was not possible. */
    uint16_t wait_time; /**< The time to wait after finished probing one channel and proceeding to the next. */
    uint16_t rate; /**< The rate as defined by @ref rate_t used to transmit the probe request. If this field is set to the value 0xffff, no probes will be sent and the scan is only passive (ony listening) */
};

static inline struct packet *
alloc_wcm_if_scan_req(struct wcm_if_scan_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_scan_req(const uint32_t handle, const char ssid[33], const uint8_t max, const uint16_t num_probes, const uint16_t idleslots, const uint16_t min_listen_time, const uint16_t max_listen_time, const uint16_t probe_tx_timeout, const uint16_t wait_time, const uint16_t rate)
{
    struct wcm_if_scan_req *req;
    struct packet *pkt = alloc_wcm_if_scan_req(&req);
    if (pkt) {
        req->handle = handle;
        strncpy(req->ssid, ssid, 33);
        req->max = max;
        req->num_probes = num_probes;
        req->idleslots = idleslots;
        req->min_listen_time = min_listen_time;
        req->max_listen_time = max_listen_time;
        req->probe_tx_timeout = probe_tx_timeout;
        req->wait_time = wait_time;
        req->rate = rate;
    }
    return pkt;
}

/** Scan for WiFi networks. Response structure.
 */
struct wcm_if_scan_rsp {
    int32_t status; /**< result status, zero is success */
    uint8_t num; /**< Number of scan responses received (may be larger than max in request). */
};

static inline struct packet *
alloc_wcm_if_scan_rsp(struct wcm_if_scan_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_scan_rsp(const int32_t status, const uint8_t num)
{
    struct wcm_if_scan_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_scan_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->num = num;
    }
    return pkt;
}

/** Scan for WiFi networks. Indication structure.
 */
struct wcm_if_scan_ind {
    uint8_t bssid[6]; /**< BSSID */
    uint16_t caps; /**< Capability information */
    uint64_t timestamp; /**< TSF timestamp from Beacon/Probe-Rsp */
    uint16_t interval; /**< Beacon interval */
    uint16_t fc; /**< Frame control field */
    int16_t rssi; /**< Estimated RSSI for the station */
    int16_t rate; /**< Transmission rate for the received frame */
    uint8_t ielist[0]; /**< List of information elements */
};

static inline struct packet *
alloc_wcm_if_scan_ind(struct wcm_if_scan_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_scan_ind(const uint8_t bssid[6], const uint16_t caps, const uint64_t timestamp, const uint16_t interval, const uint16_t fc, const int16_t rssi, const int16_t rate, const uint8_t ielist[0])
{
    struct wcm_if_scan_ind *ind;
    struct packet *pkt = alloc_wcm_if_scan_ind(&ind);
    if (pkt) {
        memcpy(ind->bssid, bssid, sizeof ind->bssid);
        ind->caps = caps;
        ind->timestamp = timestamp;
        ind->interval = interval;
        ind->fc = fc;
        ind->rssi = rssi;
        ind->rate = rate;
        /* ielist must be set by the caller */
    }
    return pkt;
}
/*
 * Message if_connect
 */

#define WCM_IF_CONNECT_REQ 0x04
#define WCM_IF_CONNECT_RSP 0x84

/** Connect to WiFI network. Request structure.
 */
struct wcm_if_connect_req {
    uint32_t handle; /**< interface handle */
    char ssid[33]; /**< SSID to connect to */
    char passphrase[65]; /**< passphrase for RSN, key for WEP or empty string for unencrypted connection */
};

static inline struct packet *
alloc_wcm_if_connect_req(struct wcm_if_connect_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_connect_req(const uint32_t handle, const char ssid[33], const char passphrase[65])
{
    struct wcm_if_connect_req *req;
    struct packet *pkt = alloc_wcm_if_connect_req(&req);
    if (pkt) {
        req->handle = handle;
        strncpy(req->ssid, ssid, 33);
        strncpy(req->passphrase, passphrase, 65);
    }
    return pkt;
}

/** Connect to WiFI network. Response structure.
 */
struct wcm_if_connect_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_if_connect_rsp(struct wcm_if_connect_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_connect_rsp(const int32_t status)
{
    struct wcm_if_connect_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_connect_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message if_disconnect
 */

#define WCM_IF_DISCONNECT_REQ 0x05
#define WCM_IF_DISCONNECT_RSP 0x85

/** Disconnect from WiFI network. Request structure.
 */
struct wcm_if_disconnect_req {
    uint32_t handle; /**< interface handle */
};

static inline struct packet *
alloc_wcm_if_disconnect_req(struct wcm_if_disconnect_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_disconnect_req(const uint32_t handle)
{
    struct wcm_if_disconnect_req *req;
    struct packet *pkt = alloc_wcm_if_disconnect_req(&req);
    if (pkt) {
        req->handle = handle;
    }
    return pkt;
}

/** Disconnect from WiFI network. Response structure.
 */
struct wcm_if_disconnect_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_if_disconnect_rsp(struct wcm_if_disconnect_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_disconnect_rsp(const int32_t status)
{
    struct wcm_if_disconnect_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_disconnect_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message if_set_addr4
 */

#define WCM_IF_SET_ADDR4_REQ 0x06
#define WCM_IF_SET_ADDR4_RSP 0x86

/** Set interface IPv4 ,netmask, gateway and dns addresses. Request structure.
 */
struct wcm_if_set_addr4_req {
    uint32_t handle; /**< interface handle */
    uint32_t ipaddr4; /**< address, as big-endian integer */
    uint32_t netmask; /**< netmask, as big-endian integer */
    uint32_t gw; /**< default-route, as big-endian integer */
    uint32_t dns_server; /**< DNS server, as big-endian integer */
};

static inline struct packet *
alloc_wcm_if_set_addr4_req(struct wcm_if_set_addr4_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_set_addr4_req(const uint32_t handle, const uint32_t ipaddr4, const uint32_t netmask, const uint32_t gw, const uint32_t dns_server)
{
    struct wcm_if_set_addr4_req *req;
    struct packet *pkt = alloc_wcm_if_set_addr4_req(&req);
    if (pkt) {
        req->handle = handle;
        req->ipaddr4 = ipaddr4;
        req->netmask = netmask;
        req->gw = gw;
        req->dns_server = dns_server;
    }
    return pkt;
}

/** Set interface IPv4 ,netmask, gateway and dns addresses. Response structure.
 */
struct wcm_if_set_addr4_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_if_set_addr4_rsp(struct wcm_if_set_addr4_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_set_addr4_rsp(const int32_t status)
{
    struct wcm_if_set_addr4_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_set_addr4_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message if_set_addr6
 */

#define WCM_IF_SET_ADDR6_REQ 0x07
#define WCM_IF_SET_ADDR6_RSP 0x87

/** Set interface IPv6 address number. Request structure.
 */
struct wcm_if_set_addr6_req {
    uint32_t handle; /**< interface handle */
    uint32_t addr6_idx; /**< address index and MUST be >= 0. NOTE that index 0 is link-local IPv6 address */
    uint8_t ipaddr6[16]; /**< interface IPv6 address */
};

static inline struct packet *
alloc_wcm_if_set_addr6_req(struct wcm_if_set_addr6_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_set_addr6_req(const uint32_t handle, const uint32_t addr6_idx, const uint8_t ipaddr6[16])
{
    struct wcm_if_set_addr6_req *req;
    struct packet *pkt = alloc_wcm_if_set_addr6_req(&req);
    if (pkt) {
        req->handle = handle;
        req->addr6_idx = addr6_idx;
        memcpy(req->ipaddr6, ipaddr6, sizeof req->ipaddr6);
    }
    return pkt;
}

/** Set interface IPv6 address number. Response structure.
 */
struct wcm_if_set_addr6_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_if_set_addr6_rsp(struct wcm_if_set_addr6_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_set_addr6_rsp(const int32_t status)
{
    struct wcm_if_set_addr6_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_set_addr6_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message if_get_addr4
 */

#define WCM_IF_GET_ADDR4_REQ 0x08
#define WCM_IF_GET_ADDR4_RSP 0x88

/** Get current IPv4/IPv6 addresses. Request structure.
 */
struct wcm_if_get_addr4_req {
    uint32_t handle; /**< interface handle */
};

static inline struct packet *
alloc_wcm_if_get_addr4_req(struct wcm_if_get_addr4_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_get_addr4_req(const uint32_t handle)
{
    struct wcm_if_get_addr4_req *req;
    struct packet *pkt = alloc_wcm_if_get_addr4_req(&req);
    if (pkt) {
        req->handle = handle;
    }
    return pkt;
}

/** Get current IPv4/IPv6 addresses. Response structure.
 */
struct wcm_if_get_addr4_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t ipaddr4; /**< address, as big-endian integer */
    uint32_t netmask; /**< netmask, as big-endian integer */
    uint32_t gw; /**< default-route, as big-endian integer */
    uint32_t dns; /**< DNS server, as big-endian integer */
};

static inline struct packet *
alloc_wcm_if_get_addr4_rsp(struct wcm_if_get_addr4_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_get_addr4_rsp(const int32_t status, const uint32_t ipaddr4, const uint32_t netmask, const uint32_t gw, const uint32_t dns)
{
    struct wcm_if_get_addr4_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_get_addr4_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->ipaddr4 = ipaddr4;
        rsp->netmask = netmask;
        rsp->gw = gw;
        rsp->dns = dns;
    }
    return pkt;
}
/*
 * Message if_get_addr6
 */

#define WCM_IF_GET_ADDR6_REQ 0x09
#define WCM_IF_GET_ADDR6_RSP 0x89

/** Get current IPv4/IPv6 addresses. Request structure.
 */
struct wcm_if_get_addr6_req {
    uint32_t handle; /**< interface handle */
};

static inline struct packet *
alloc_wcm_if_get_addr6_req(struct wcm_if_get_addr6_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_get_addr6_req(const uint32_t handle)
{
    struct wcm_if_get_addr6_req *req;
    struct packet *pkt = alloc_wcm_if_get_addr6_req(&req);
    if (pkt) {
        req->handle = handle;
    }
    return pkt;
}

/** Get current IPv4/IPv6 addresses. Response structure.
 */
struct wcm_if_get_addr6_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t numaddr6; /**< number of returned addr6 */
    uint8_t ipaddr6[48]; /**< interface list of IPv6 addresses */
};

static inline struct packet *
alloc_wcm_if_get_addr6_rsp(struct wcm_if_get_addr6_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_get_addr6_rsp(const int32_t status, const uint32_t numaddr6, const uint8_t ipaddr6[48])
{
    struct wcm_if_get_addr6_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_get_addr6_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->numaddr6 = numaddr6;
        memcpy(rsp->ipaddr6, ipaddr6, sizeof rsp->ipaddr6);
    }
    return pkt;
}
/*
 * Message if_set_pmconfig
 */

#define WCM_IF_SET_PMCONFIG_REQ 0x0a
#define WCM_IF_SET_PMCONFIG_RSP 0x8a

/** Configure WiFi power-save parameters. Request structure.
 */
struct wcm_if_set_pmconfig_req {
    uint32_t handle; /**< interface handle */
    uint32_t listen_interval; /**< Listen interval in units of beacon intervals */
    uint32_t traffic_tmo; /**< Traffic timeout (in ms) */
    uint8_t ps_poll; /**< Set 1 to use ps poll when beacon was missed */
    uint8_t dyn_listen_int; /**< Set 1 to listen to all beacons if there was traffic recently */
};

static inline struct packet *
alloc_wcm_if_set_pmconfig_req(struct wcm_if_set_pmconfig_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_set_pmconfig_req(const uint32_t handle, const uint32_t listen_interval, const uint32_t traffic_tmo, const uint8_t ps_poll, const uint8_t dyn_listen_int)
{
    struct wcm_if_set_pmconfig_req *req;
    struct packet *pkt = alloc_wcm_if_set_pmconfig_req(&req);
    if (pkt) {
        req->handle = handle;
        req->listen_interval = listen_interval;
        req->traffic_tmo = traffic_tmo;
        req->ps_poll = ps_poll;
        req->dyn_listen_int = dyn_listen_int;
    }
    return pkt;
}

/** Configure WiFi power-save parameters. Response structure.
 */
struct wcm_if_set_pmconfig_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_if_set_pmconfig_rsp(struct wcm_if_set_pmconfig_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_set_pmconfig_rsp(const int32_t status)
{
    struct wcm_if_set_pmconfig_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_set_pmconfig_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message if_get_rssi
 */

#define WCM_IF_GET_RSSI_REQ 0x0b
#define WCM_IF_GET_RSSI_RSP 0x8b

/** Return the current RSSI. Request structure.
 */
struct wcm_if_get_rssi_req {
    uint32_t handle; /**< interface handle */
};

static inline struct packet *
alloc_wcm_if_get_rssi_req(struct wcm_if_get_rssi_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_get_rssi_req(const uint32_t handle)
{
    struct wcm_if_get_rssi_req *req;
    struct packet *pkt = alloc_wcm_if_get_rssi_req(&req);
    if (pkt) {
        req->handle = handle;
    }
    return pkt;
}

/** Return the current RSSI. Response structure.
 */
struct wcm_if_get_rssi_rsp {
    int32_t rssi; /**< Current average RSSI (zero if not assocaited) */
};

static inline struct packet *
alloc_wcm_if_get_rssi_rsp(struct wcm_if_get_rssi_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_get_rssi_rsp(const int32_t rssi)
{
    struct wcm_if_get_rssi_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_get_rssi_rsp(&rsp);
    if (pkt) {
        rsp->rssi = rssi;
    }
    return pkt;
}
/*
 * Message if_get_counters
 */

#define WCM_IF_GET_COUNTERS_REQ 0x0c
#define WCM_IF_GET_COUNTERS_RSP 0x8c

/** Return link stats counters. Request structure.
 */
struct wcm_if_get_counters_req {
    uint32_t handle; /**< interface handle */
};

static inline struct packet *
alloc_wcm_if_get_counters_req(struct wcm_if_get_counters_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_get_counters_req(const uint32_t handle)
{
    struct wcm_if_get_counters_req *req;
    struct packet *pkt = alloc_wcm_if_get_counters_req(&req);
    if (pkt) {
        req->handle = handle;
    }
    return pkt;
}

/** Return link stats counters. Response structure.
 */
struct wcm_if_get_counters_rsp {
    uint64_t txbytes; /**< number of transmitted bytes */
    uint32_t txframe; /**< number of transmitted frames */
    uint32_t txmframe; /**< number of transmitted multicast frames */
    uint32_t txnoack; /**< number of frames without ACK */
    uint32_t txfail; /**< number of transmitted frame failures */
    uint32_t txretry; /**< number of with at least one retry */
    uint32_t txretries; /**< number of with multiple retries */
    uint64_t rxbytes; /**< number of received bytes */
    uint32_t rxframe; /**< number of received frames */
    uint32_t rxmframe; /**< number of received multicast frames */
    uint32_t rxdup; /**< number of dupicate frames */
    uint32_t rxfcserr; /**< number of frames with FCS error */
};

static inline struct packet *
alloc_wcm_if_get_counters_rsp(struct wcm_if_get_counters_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_get_counters_rsp(const uint64_t txbytes, const uint32_t txframe, const uint32_t txmframe, const uint32_t txnoack, const uint32_t txfail, const uint32_t txretry, const uint32_t txretries, const uint64_t rxbytes, const uint32_t rxframe, const uint32_t rxmframe, const uint32_t rxdup, const uint32_t rxfcserr)
{
    struct wcm_if_get_counters_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_get_counters_rsp(&rsp);
    if (pkt) {
        rsp->txbytes = txbytes;
        rsp->txframe = txframe;
        rsp->txmframe = txmframe;
        rsp->txnoack = txnoack;
        rsp->txfail = txfail;
        rsp->txretry = txretry;
        rsp->txretries = txretries;
        rsp->rxbytes = rxbytes;
        rsp->rxframe = rxframe;
        rsp->rxmframe = rxmframe;
        rsp->rxdup = rxdup;
        rsp->rxfcserr = rxfcserr;
    }
    return pkt;
}
/*
 * Message if_txpower
 */

#define WCM_IF_TXPOWER_REQ 0x0d
#define WCM_IF_TXPOWER_RSP 0x8d

/** Set TX power. Request structure.
 */
struct wcm_if_txpower_req {
    uint32_t handle; /**< interface handle */
    int8_t tx_power; /**< TX power in dBm (-10..20(max)) */
    uint8_t reserved1; /**< Reserved. Must be zero */
    uint16_t reserved2; /**< Reserved. Must be zero */
};

static inline struct packet *
alloc_wcm_if_txpower_req(struct wcm_if_txpower_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_txpower_req(const uint32_t handle, const int8_t tx_power)
{
    struct wcm_if_txpower_req *req;
    struct packet *pkt = alloc_wcm_if_txpower_req(&req);
    if (pkt) {
        req->handle = handle;
        req->tx_power = tx_power;
        req->reserved1 = 0;
        req->reserved2 = 0;
    }
    return pkt;
}

/** Set TX power. Response structure.
 */
struct wcm_if_txpower_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_if_txpower_rsp(struct wcm_if_txpower_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_txpower_rsp(const int32_t status)
{
    struct wcm_if_txpower_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_txpower_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message if_get_bssid
 */

#define WCM_IF_GET_BSSID_REQ 0x0e
#define WCM_IF_GET_BSSID_RSP 0x8e

/** Get current BSSID. Request structure.
 */
struct wcm_if_get_bssid_req {
    uint32_t handle; /**< interface handle */
};

static inline struct packet *
alloc_wcm_if_get_bssid_req(struct wcm_if_get_bssid_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_get_bssid_req(const uint32_t handle)
{
    struct wcm_if_get_bssid_req *req;
    struct packet *pkt = alloc_wcm_if_get_bssid_req(&req);
    if (pkt) {
        req->handle = handle;
    }
    return pkt;
}

/** Get current BSSID. Response structure.
 */
struct wcm_if_get_bssid_rsp {
    int32_t status; /**< result status, zero is success */
    uint8_t bssid[6]; /**< BSSID of the network if associated, zero otherwize */
};

static inline struct packet *
alloc_wcm_if_get_bssid_rsp(struct wcm_if_get_bssid_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_get_bssid_rsp(const int32_t status, const uint8_t bssid[6])
{
    struct wcm_if_get_bssid_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_get_bssid_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        memcpy(rsp->bssid, bssid, sizeof rsp->bssid);
    }
    return pkt;
}
/*
 * Message if_add_network
 */

#define WCM_IF_ADD_NETWORK_REQ 0x0f
#define WCM_IF_ADD_NETWORK_RSP 0x8f

/** Add network to connect to (asynchronously). Request structure.
 */
struct wcm_if_add_network_req {
    uint32_t handle; /**< interface handle */
    char ssid[33]; /**< SSID of the network or empty string if bssid is set. */
    uint8_t bssid[6]; /**< BSSID of the network, set to all zeroes if ssid is set */
    char passphrase[65]; /**< passphrase for RSN, key for WEP or empty string for unencrypted connection */
};

static inline struct packet *
alloc_wcm_if_add_network_req(struct wcm_if_add_network_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_add_network_req(const uint32_t handle, const char ssid[33], const uint8_t bssid[6], const char passphrase[65])
{
    struct wcm_if_add_network_req *req;
    struct packet *pkt = alloc_wcm_if_add_network_req(&req);
    if (pkt) {
        req->handle = handle;
        strncpy(req->ssid, ssid, 33);
        memcpy(req->bssid, bssid, sizeof req->bssid);
        strncpy(req->passphrase, passphrase, 65);
    }
    return pkt;
}

/** Add network to connect to (asynchronously). Response structure.
 */
struct wcm_if_add_network_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_if_add_network_rsp(struct wcm_if_add_network_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_add_network_rsp(const int32_t status)
{
    struct wcm_if_add_network_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_add_network_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message if_remove_network
 */

#define WCM_IF_REMOVE_NETWORK_REQ 0x10
#define WCM_IF_REMOVE_NETWORK_RSP 0x90

/** Remove network. Request structure.
 */
struct wcm_if_remove_network_req {
    uint32_t handle; /**< interface handle */
    char ssid[33]; /**< SSID of the network or empty string if bssid is set. */
    uint8_t bssid[6]; /**< BSSID of the network, set to all zeroes if ssid is set */
};

static inline struct packet *
alloc_wcm_if_remove_network_req(struct wcm_if_remove_network_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_remove_network_req(const uint32_t handle, const char ssid[33], const uint8_t bssid[6])
{
    struct wcm_if_remove_network_req *req;
    struct packet *pkt = alloc_wcm_if_remove_network_req(&req);
    if (pkt) {
        req->handle = handle;
        strncpy(req->ssid, ssid, 33);
        memcpy(req->bssid, bssid, sizeof req->bssid);
    }
    return pkt;
}

/** Remove network. Response structure.
 */
struct wcm_if_remove_network_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_if_remove_network_rsp(struct wcm_if_remove_network_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_remove_network_rsp(const int32_t status)
{
    struct wcm_if_remove_network_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_remove_network_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message if_auto_connect
 */

#define WCM_IF_AUTO_CONNECT_REQ 0x11
#define WCM_IF_AUTO_CONNECT_RSP 0x91

/** Enable/Disable async connect configured by if_add_network. Request structure.
 */
struct wcm_if_auto_connect_req {
    uint32_t handle; /**< interface handle */
    uint32_t enabled; /**< allow wcm to connect. 1=enabled, 0=disabled */
};

static inline struct packet *
alloc_wcm_if_auto_connect_req(struct wcm_if_auto_connect_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_auto_connect_req(const uint32_t handle, const uint32_t enabled)
{
    struct wcm_if_auto_connect_req *req;
    struct packet *pkt = alloc_wcm_if_auto_connect_req(&req);
    if (pkt) {
        req->handle = handle;
        req->enabled = enabled;
    }
    return pkt;
}

/** Enable/Disable async connect configured by if_add_network. Response structure.
 */
struct wcm_if_auto_connect_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_if_auto_connect_rsp(struct wcm_if_auto_connect_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_auto_connect_rsp(const int32_t status)
{
    struct wcm_if_auto_connect_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_auto_connect_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message if_add_network_uri
 */

#define WCM_IF_ADD_NETWORK_URI_REQ 0x12
#define WCM_IF_ADD_NETWORK_URI_RSP 0x92

/** Add network to connect to (asynchronously). Request structure.
 */
struct wcm_if_add_network_uri_req {
    uint32_t handle; /**< interface handle */
    char uri[0]; /**< A "WIFI" style URI specifying the network to connect to. */
};

static inline struct packet *
alloc_wcm_if_add_network_uri_req(struct wcm_if_add_network_uri_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_add_network_uri_req(const uint32_t handle, const char uri[0])
{
    struct wcm_if_add_network_uri_req *req;
    struct packet *pkt = alloc_wcm_if_add_network_uri_req(&req);
    if (pkt) {
        req->handle = handle;
        strncpy(req->uri, uri, 0);
    }
    return pkt;
}

/** Add network to connect to (asynchronously). Response structure.
 */
struct wcm_if_add_network_uri_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_if_add_network_uri_rsp(struct wcm_if_add_network_uri_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_if_add_network_uri_rsp(const int32_t status)
{
    struct wcm_if_add_network_uri_rsp *rsp;
    struct packet *pkt = alloc_wcm_if_add_network_uri_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message set_mfp_mode
 */

#define WCM_SET_MFP_MODE_REQ 0x13
#define WCM_SET_MFP_MODE_RSP 0x93

/** Set the desired mode for management frame protection. Request structure.
 */
struct wcm_set_mfp_mode_req {
    uint32_t handle; /**< interface handle */
    uint32_t mfp_required; /**< make MFP required if non-zero */
};

static inline struct packet *
alloc_wcm_set_mfp_mode_req(struct wcm_set_mfp_mode_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_set_mfp_mode_req(const uint32_t handle, const uint32_t mfp_required)
{
    struct wcm_set_mfp_mode_req *req;
    struct packet *pkt = alloc_wcm_set_mfp_mode_req(&req);
    if (pkt) {
        req->handle = handle;
        req->mfp_required = mfp_required;
    }
    return pkt;
}

/** Set the desired mode for management frame protection. Response structure.
 */
struct wcm_set_mfp_mode_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_set_mfp_mode_rsp(struct wcm_set_mfp_mode_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_set_mfp_mode_rsp(const int32_t status)
{
    struct wcm_set_mfp_mode_rsp *rsp;
    struct packet *pkt = alloc_wcm_set_mfp_mode_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message testmode
 */

#define WCM_TESTMODE_REQ 0x14
#define WCM_TESTMODE_RSP 0x94

/** Configure test mode. Request structure.
 */
struct wcm_testmode_req {
    uint32_t handle; /**< interface handle */
    uint32_t mode; /**< undocumented mode selector. */
    uint32_t control; /**< undocumented per-mode control parameter. */
};

static inline struct packet *
alloc_wcm_testmode_req(struct wcm_testmode_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_testmode_req(const uint32_t handle, const uint32_t mode, const uint32_t control)
{
    struct wcm_testmode_req *req;
    struct packet *pkt = alloc_wcm_testmode_req(&req);
    if (pkt) {
        req->handle = handle;
        req->mode = mode;
        req->control = control;
    }
    return pkt;
}

/** Configure test mode. Response structure.
 */
struct wcm_testmode_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_testmode_rsp(struct wcm_testmode_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_testmode_rsp(const int32_t status)
{
    struct wcm_testmode_rsp *rsp;
    struct packet *pkt = alloc_wcm_testmode_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message set_regulatory_domain
 */

#define WCM_SET_REGULATORY_DOMAIN_REQ 0x15
#define WCM_SET_REGULATORY_DOMAIN_RSP 0x95

/** Set regulatory domain, with channels and power limits. Request structure.
 */
struct wcm_set_regulatory_domain_req {
    uint32_t handle; /**< interface handle */
    char domain[64]; /**< FCC, ETSI, TELEC, KCC, SRCC (or e.g. 1..11@20) */
};

static inline struct packet *
alloc_wcm_set_regulatory_domain_req(struct wcm_set_regulatory_domain_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_wcm_set_regulatory_domain_req(const uint32_t handle, const char domain[64])
{
    struct wcm_set_regulatory_domain_req *req;
    struct packet *pkt = alloc_wcm_set_regulatory_domain_req(&req);
    if (pkt) {
        req->handle = handle;
        strncpy(req->domain, domain, 64);
    }
    return pkt;
}

/** Set regulatory domain, with channels and power limits. Response structure.
 */
struct wcm_set_regulatory_domain_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_wcm_set_regulatory_domain_rsp(struct wcm_set_regulatory_domain_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_wcm_set_regulatory_domain_rsp(const int32_t status)
{
    struct wcm_set_regulatory_domain_rsp *rsp;
    struct packet *pkt = alloc_wcm_set_regulatory_domain_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/* [[[end]]] */
/** @} */
