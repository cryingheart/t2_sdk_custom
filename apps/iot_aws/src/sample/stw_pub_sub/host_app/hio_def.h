#pragma once
#include <stdint.h>

#define HIO_GROUP_HIO 0

#define HIO_QUERY_REQ 0x00
#define HIO_QUERY_RSP 0x80
#define HIO_CONFIG_REQ 0x01
#define HIO_CONFIG_RSP 0x81
#define HIO_SLEEP_REQ 0x02
#define HIO_IRQACK_REQ 0x03
#define HIO_PING_REQ 0x04
#define HIO_PING_RSP 0x84
#define HIO_WAKEUP_IND 0xc5
#define HIO_CONSOLE_IND 0xc6

struct hio_header_info {
	short int len;
	char groupid;
	char msgid;
	short int trandid;
};

typedef struct hio_res_header {
	unsigned char group_id;
	unsigned char res_code;
	short trans_id;
} HIO_Response_Header;

/** Query device about available procedures. Request structure.
 */
struct hio_query_req {
};

struct hio_query_rsp {
    uint32_t status; /**< result status, zero is success */
    uint8_t version; /**< protocol version */
    uint8_t ngroups; /**< length of nmsg vector */
    uint16_t maxsize; /**< max request size */
    char fw_rev[10]; /**< Firware revision string */
    char patch_rev[10]; /**< ROM patch revision string */
    uint16_t nmsg[0]; /**< supported hostio interfaces, coded as 256*group + num_api */
};

/** Configure wake-up and interrupt pins. Request structure.
 */
struct hio_config_req {
    uint8_t wakeup_pin; /**< GPIO pin to use for wakeup, zero to disable */
    uint8_t wakeup_level; /**< wakeup level high(1) or low(0) */
    uint8_t irq_pin; /**< GPIO pin to use for interrupt */
    uint8_t irq_mode; /**< Interrupt mode to use: none(0), high(1), or toggle(2) */
};

/** Configure wake-up and interrupt pins. Response structure.
 */
struct hio_config_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Indicate that device may go to sleep. Request structure.
 */
struct hio_sleep_req {
};

/** Acknowledge level interrupt. Request structure.
 */
struct hio_irqack_req {
};

/** Solicit response from device. Request structure.
 */
struct hio_ping_req {
    uint8_t payload[0]; /**< Ping message content. */
};

/** Solicit response from device. Response structure.
 */
struct hio_ping_rsp {
    uint8_t payload[0]; /**< Ping response content. */
};

/** Informs host that device is awake. Keep wakeup_pin set until wakeup_ind is received. Indication structure.
 */
struct hio_wakeup_ind {
    uint32_t reserved;
};

/** Debug console output. Indication structure.
 */
struct hio_console_ind {
    char str[0]; /**< console message */
};


#define HIO_GROUP_WCM 2

#define WCM_IF_CREATE_REQ 0x00
#define WCM_IF_CREATE_RSP 0x80
#define WCM_IF_DESTROY_REQ 0x01
#define WCM_IF_DESTROY_RSP 0x81
#define WCM_IF_NOTIFY_IND 0xc2
#define WCM_IF_SCAN_REQ 0x03
#define WCM_IF_SCAN_RSP 0x83
#define WCM_IF_SCAN_IND 0xc3
#define WCM_IF_CONNECT_REQ 0x04
#define WCM_IF_CONNECT_RSP 0x84
#define WCM_IF_DISCONNECT_REQ 0x05
#define WCM_IF_DISCONNECT_RSP 0x85
#define WCM_IF_SET_ADDR4_REQ 0x06
#define WCM_IF_SET_ADDR4_RSP 0x86
#define WCM_IF_GET_ADDR4_REQ 0x07
#define WCM_IF_GET_ADDR4_RSP 0x87
#define WCM_IF_SET_PMCONFIG_REQ 0x08
#define WCM_IF_SET_PMCONFIG_RSP 0x88
#define WCM_IF_GET_RSSI_REQ 0x09
#define WCM_IF_GET_RSSI_RSP 0x89
#define WCM_IF_GET_COUNTERS_REQ 0x0a
#define WCM_IF_GET_COUNTERS_RSP 0x8a
#define WCM_IF_TXPOWER_REQ 0x0b
#define WCM_IF_TXPOWER_RSP 0x8b
#define WCM_IF_GET_BSSID_REQ 0x0c
#define WCM_IF_GET_BSSID_RSP 0x8c
#define WCM_IF_ADD_NETWORK_REQ 0x0d
#define WCM_IF_ADD_NETWORK_RSP 0x8d
#define WCM_IF_REMOVE_NETWORK_REQ 0x0e
#define WCM_IF_REMOVE_NETWORK_RSP 0x8e
#define WCM_IF_SET_POWER_MODE_REQ 0x0f
#define WCM_IF_SET_POWER_MODE_RSP 0x8f

/** Create a WiFi network interface. Request structure.
 */
struct wcm_if_create_req {
    uint8_t addr[6]; /**< MAC address of the WIFI interface. If an invalid MAC address is specified (e.g. all zero), the device will generate a random address. */
};

/** Create a WiFi network interface. Response structure.
 */
struct wcm_if_create_rsp {
    uint32_t handle; /**< interface handle of created interface */
    uint8_t hwaddr[6]; /**< The MAC address used by the interface */
};

/** Shutdown and cleanup a WiFi Connection Manager interface. Request structure.
 */
struct wcm_if_destroy_req {
    uint32_t handle; /**< interface handle */
};

/** Shutdown and cleanup a WiFi Connection Manager interface. Response structure.
 */
struct wcm_if_destroy_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Connection manager notification. Indication structure.
 */
struct wcm_if_notify_ind {
    uint32_t type; /**< Notification type: Link up (200),  Link down (201),  Address (202) */
    char attr[0]; /**< Notification data */
};

/** Scan for WiFi networks. Request structure.
 */
struct wcm_if_scan_req {
    uint32_t handle; /**< interface handle */
    char ssid[33]; /**< Zero terminated SSID to scan for, empty for broadcast scan. */
    uint8_t max; /**< Max number of responses to return. */
    uint16_t dwell; /**< Channel time (in milliseconds). This is the time spent waiting for probe responses on each channel. */
    uint16_t wait; /**< The time to wait after finished probing one channel and proceeding to the next. */
    uint16_t rate; /**< The rate as defined by @ref rate_t used to transmit the probe request. If this field is set to the value 0xffff, no probes will be sent and the scan is only passive (ony listening) */
};

/** Scan for WiFi networks. Response structure.
 */
struct wcm_if_scan_rsp {
    uint32_t status; /**< result status, zero is success */
    uint8_t num; /**< Number of scan responses received (may be larger than max in request). */
};

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

/** Connect to WiFI network. Request structure.
 */
struct wcm_if_connect_req {
    uint32_t handle; /**< interface handle */
    char ssid[33]; /**< SSID to connect to */
    char passphrase[65]; /**< passphrase for RSN, key for WEP or empty string for unencrypted connection */
};

/** Connect to WiFI network. Response structure.
 */
struct wcm_if_connect_rsp {
    uint32_t status; /**< result status, zero is success */
};	int status;

/** Disconnect from WiFI network. Request structure.
 */
struct wcm_if_disconnect_req {
    uint32_t handle; /**< interface handle */
};

/** Disconnect from WiFI network. Response structure.
 */
struct wcm_if_disconnect_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Set IPv4 state. Request structure.
 */
struct wcm_if_set_addr4_req {
    uint32_t handle; /**< interface handle */
    uint32_t ipaddr4; /**< address, as big-endian integer */
    uint32_t netmask; /**< netmask, as big-endian integer */
    uint32_t gw; /**< default-route, as big-endian integer */
    uint32_t dns_server; /**< DNS server, as big-endian integer */
};

/** Set IPv4 state. Response structure.
 */
struct wcm_if_set_addr4_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Get current IPv4 state. Request structure.
 */
struct wcm_if_get_addr4_req {
    uint32_t handle; /**< interface handle */
};

/** Get current IPv4 state. Response structure.
 */
struct wcm_if_get_addr4_rsp {
    uint32_t status; /**< result status, zero is success */
    uint32_t ipaddr4; /**< address, as big-endian integer */
    uint32_t netmask; /**< netmask, as big-endian integer */
    uint32_t gw; /**< default-route, as big-endian integer */
    uint32_t dns; /**< DNS server, as big-endian integer */
};

/** Configure WiFi power-save parameters. Request structure.
 */
struct wcm_if_set_pmconfig_req {
    uint32_t handle; /**< interface handle */
    uint32_t listen_interval; /**< Listen interval in units of beacon intervals */
    uint32_t traffic_tmo; /**< Traffic timeout (in ms) */
};

/** Configure WiFi power-save parameters. Response structure.
 */
struct wcm_if_set_pmconfig_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Return the current RSSI. Request structure.
 */
struct wcm_if_get_rssi_req {
    uint32_t handle; /**< interface handle */
};

/** Return the current RSSI. Response structure.
 */
struct wcm_if_get_rssi_rsp {
    int32_t rssi; /**< Current average RSSI (zero if not assocaited) */
};

/** Return link stats counters. Request structure.
 */
struct wcm_if_get_counters_req {
    uint32_t handle; /**< interface handle */
};

/** Return link stats counters. Response structure.
 */
struct wcm_if_get_counters_rsp {
    uint64_t txbytes; /**< # transmitted bytes */
    uint32_t txframe; /**< # transmitted frames */
    uint32_t txmframe; /**< # transmitted multicast frames */
    uint32_t txnoack; /**< # frames without ACK */
    uint32_t txfail; /**< # transmitted frame failures */
    uint32_t txretry; /**< # with at least one retry */
    uint32_t txretries; /**< # with multiple retries */
    uint64_t rxbytes; /**< # received bytes */
    uint32_t rxframe; /**< # received frames */
    uint32_t rxmframe; /**< # received multicast frames */
    uint32_t rxdup; /**< # dupicate frames */
    uint32_t rxfcserr; /**< # frames with FCS error */
};

/** Set TX power. Request structure.
 */
struct wcm_if_txpower_req {
    uint32_t handle; /**< interface handle */
    int8_t tx_power; /**< TX power in dBm (-10..20(max)) */
    uint8_t reserved1; /**< Reserved. Must be zero */
    uint16_t reserved2; /**< Reserved. Must be zero */
};

/** Set TX power. Response structure.
 */
struct wcm_if_txpower_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Get current BSSID. Request structure.
 */
struct wcm_if_get_bssid_req {
    uint32_t handle; /**< interface handle */
};

/** Get current BSSID. Response structure.
 */
struct wcm_if_get_bssid_rsp {
    uint32_t status; /**< result status, zero is success */
    uint8_t bssid[6]; /**< BSSID of the network if associated, zero otherwize */
};

/** Add network to connect to (asynchronously). Request structure.
 */
struct wcm_if_add_network_req {
    uint32_t handle; /**< interface handle */
    char ssid[33]; /**< SSID of the network or empty string if bssid is set. */
    uint8_t bssid[6]; /**< BSSID of the network, set to all zeroes if ssid is set */
    char passphrase[65]; /**< passphrase for RSN, key for WEP or empty string for unencrypted connection */
};

/** Add network to connect to (asynchronously). Response structure.
 */
struct wcm_if_add_network_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Remove network. Request structure.
 */
struct wcm_if_remove_network_req {
    uint32_t handle; /**< interface handle */
    char ssid[33]; /**< SSID of the network or empty string if bssid is set. */
    uint8_t bssid[6]; /**< BSSID of the network, set to all zeroes if ssid is set */
};

/** Remove network. Response structure.
 */
struct wcm_if_remove_network_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Set power mode. Request structure.
 */
struct wcm_if_set_power_mode_req {
    uint32_t handle; /**< interface handle */
    uint32_t power_mode; /**< 0: Extra low power, 1: Normal, 2: Low latency, 3: Extra low latency, 4: Throughput */
};

/** Set power mode. Response structure.
 */
struct wcm_if_set_power_mode_rsp {
    uint32_t status; /**< result status, zero is success */
};

#define HIO_GROUP_BT 4

#define BT_HCI_REQ 0x00
#define BT_HCI_RSP 0x80
#define BT_HCI_IND 0xc0
#define BT_CMD_REQ 0x01
#define BT_CMD_RSP 0x81
#define BT_CMD_IND 0xc1
#define BT_DAT_REQ 0x02
#define BT_DAT_RSP 0x82
#define BT_DAT_IND 0xc2

/** Send command/data (req) and receive event/data (rsp, ind) - hci format. Request structure.
 */
struct bt_hci_req {
    uint8_t payload[0]; /**< hci command or acl data */
};

/** Send command/data (req) and receive event/data (rsp, ind) - hci format. Response structure.
 */
struct bt_hci_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Send command/data (req) and receive event/data (rsp, ind) - hci format. Indication structure.
 */
struct bt_hci_ind {
    uint8_t payload[0]; /**< hci event  or acl data */
};

/** Send HCI command (req) and receive HCI event (rsb, ind) - explicit format. Request structure.
 */
struct bt_cmd_req {
    uint8_t ocf; /**< opcode command field */
    uint8_t ogf; /**< opcode group field */
    uint8_t len; /**< parameter length (0..255) */
    uint8_t param[0]; /**< parameters */
};

/** Send HCI command (req) and receive HCI event (rsb, ind) - explicit format. Response structure.
 */
struct bt_cmd_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Send HCI command (req) and receive HCI event (rsb, ind) - explicit format. Indication structure.
 */
struct bt_cmd_ind {
    uint8_t code; /**< event code */
    uint8_t len; /**< parameter length (0..255) */
    uint8_t param[0]; /**< parameters */
};

/** Send ACL data (req) and receive ACL data (ind) - explicit format. Request structure.
 */
struct bt_dat_req {
    uint8_t handle; /**< connection handle */
    uint8_t pbf; /**< packet boundary flags: 0-start, 1-cont */
    uint8_t bcf; /**< broadcast flags: 0-point-to-point (no broadcast) */
    uint8_t len; /**< data length (0..255) */
    uint8_t data[0]; /**< data */
};

/** Send ACL data (req) and receive ACL data (ind) - explicit format. Response structure.
 */
struct bt_dat_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Send ACL data (req) and receive ACL data (ind) - explicit format. Indication structure.
 */
struct bt_dat_ind {
    uint8_t handle; /**< connection handle */
    uint8_t pbf; /**< packet boundary flags: 0-start, 1-cont */
    uint8_t bcf; /**< broadcast flags: 0-point-to-point (no broadcast) */
    uint8_t len; /**< data length (0..255) */
    uint8_t data[0]; /**< data */
};

#define HIO_GROUP_SOCK 5

#define SOCK_SERVER_REQ 0x00
#define SOCK_SERVER_RSP 0x80
#define SOCK_CLIENT_REQ 0x01
#define SOCK_CLIENT_RSP 0x81
#define SOCK_CONNECTION_IND 0xc2
#define SOCK_CLOSE_REQ 0x03
#define SOCK_CLOSE_RSP 0x83
#define SOCK_CLOSE_IND 0xc3
#define SOCK_NOTIFICATION_REQ 0x04
#define SOCK_NOTIFICATION_RSP 0x84
#define SOCK_NOTIFICATION_IND 0xc4
#define SOCK_RECVFROM_REQ 0x05
#define SOCK_RECVFROM_RSP 0x85
#define SOCK_SENDTO_REQ 0x06
#define SOCK_SENDTO_RSP 0x86
#define SOCK_RESOLVE_REQ 0x07
#define SOCK_RESOLVE_RSP 0x87

/** Create a server socket that binds to the protocol, address and port specified by the URI. Request structure.
 */
struct sock_server_req {
    char uri[64]; /**< URI on the form `<protocol>://<host>:<port>` where protocol may be tcp or udp */
};

/** Create a server socket that binds to the protocol, address and port specified by the URI. Response structure.
 */
struct sock_server_rsp {
    uint32_t status; /**< result status, zero is success */
    uint32_t socket; /**< handle to the created socket if successful */
};

/** Create a client socket that connects to the protocol, address and port specified by the URI. Request structure.
 */
struct sock_client_req {
    char uri[64]; /**< URI on the form `<protocol>://<host>:<port>` where protocol may be tcp or udp */
};

/** Create a client socket that connects to the protocol, address and port specified by the URI. Response structure.
 */
struct sock_client_rsp {
    uint32_t status; /**< result status, zero is success */
    uint32_t socket; /**< handle to the created socket if successful */
};

/** Notities about a new server connection. Indication structure.
 */
struct sock_connection_ind {
    uint32_t status; /**< result status, zero is success */
    uint32_t socket; /**< handle to socket for the new connection */
};

/** Closes and destroys the socket specified by the handle. Request structure.
 */
struct sock_close_req {
    uint32_t socket; /**< handle to socket that will be closed and destroyed */
};

/** Closes and destroys the socket specified by the handle. Response structure.
 */
struct sock_close_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Closes and destroys the socket specified by the handle. Indication structure.
 */
struct sock_close_ind {
    uint32_t status; /**< result status, zero is success */
    uint32_t socket; /**< handle of socket to close */
};

/** Request notification when socket is readable. Request structure.
 */
struct sock_notification_req {
    uint32_t socket; /**< handle to socket that will be closed and destroyed */
    uint32_t threshold; /**< Fire notification when abount of readable data is above threshold */
    uint32_t flags; /**< reserved for future use */
};

/** Request notification when socket is readable. Response structure.
 */
struct sock_notification_rsp {
    uint32_t status; /**< result status, zero is success */
};

/** Request notification when socket is readable. Indication structure.
 */
struct sock_notification_ind {
    uint32_t status; /**< result status, zero is success */
    uint32_t socket; /**< handle to socket that can be read */
    uint32_t flags; /**< reserved for future use */
};

/** Reads up to `size` data bytes from the socket. May return less than `size`. If not data is available the `actual` size will be 0 and status != 0. Request structure.
 */
struct sock_recvfrom_req {
    uint32_t socket; /**< socket descriptor */
    uint32_t size; /**< number of bytes to receive */
    uint32_t flags; /**< reserved for future use */
};

/** Reads up to `size` data bytes from the socket. May return less than `size`. If not data is available the `actual` size will be 0 and status != 0. Response structure.
 */
struct sock_recvfrom_rsp {
    uint32_t status; /**< result status, zero is success */
    int32_t actual; /**< number of bytes received from the socket */
    uint8_t addr[16]; /**< source address */
    uint16_t addrlen; /**< size of the address IPv4(4)/Pv6(16) Ipv4ONLY for now */
    uint16_t port; /**< source port */
    uint8_t data[0]; /**< buffer with received data */
};

/** Writes `size` data bytes into the socket. Used on non-connected sockets. Request structure.
 */
struct sock_sendto_req {
    uint32_t socket; /**< socket descriptor */
    uint32_t size; /**< size of data to write to the socket */
    uint32_t flags; /**< reserved for future use */
    uint8_t addr[16]; /**< destination address */
    uint16_t addrlen; /**< size of the address IPv4(4)/Pv6(16) */
    uint16_t port; /**< destination port */
    uint8_t data[0]; /**< buffer containing the data to write to the socket */
};

/** Writes `size` data bytes into the socket. Used on non-connected sockets. Response structure.
 */
struct sock_sendto_rsp {
    uint32_t status; /**< result status, zero is success */
    uint32_t actual; /**< number of bytes written to the socket */
};

/** Returns the IPv4 address of a hostname. Request structure.
 */
struct sock_resolve_req {
    char hostname[0]; /**< the hostname to resolve */
};

/** Returns the IPv4 address of a hostname. Response structure.
 */
struct sock_resolve_rsp {
    uint32_t status; /**< result status, zero is success */
    uint32_t ipaddr4; /**< the IPv4 address of the hostname */
};
