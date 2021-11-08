/**
 * @addtogroup ble_gap
 * @{
 */
#pragma once
#include <stdint.h>
#include <kernel/packet.h>
#include <bt/bt_uuid.h>
#include <bt/bt_smp.h>
#include <bt/bt_att.h>
#include <bt/bt_hci.h>

/**
 * List of errors returned from Bluetooth GAP functions
 */
typedef enum {
    /** Operation succeeded with no error */
    GAP_ERROR_SUCCESS = 0,
    /** Operation could not be performed due to the device being busy */
    GAP_ERROR_BUSY = 1,
    /** A parameter was invalid */
    GAP_ERROR_INVALID_PARAMETER = 2,
    /** Operation was impossible to perform */
    GAP_ERROR_IMPOSSIBLE = 3,
} bt_gap_error_t;

/**
 * Specifies the security mode of the device.
 */
typedef enum {
    /** No authentication and no encryption */
    BT_GAP_SEC_MODE_NONE    = 0,
    /** Unauthenticated pairing with encryption */
    BT_GAP_SEC_MODE_UNAUTH  = 1,
    /** Authenticated pairing with encryption */
    BT_GAP_SEC_MODE_AUTH    = 2,
    /** Authenticated LE Secure Connections pairing with encryption using a
     * 128-bit strength encryption key.
     */
    BT_GAP_SEC_MODE_AUTH_SC = 3,
} bt_gap_sec_mode_t;

#define BT_GAP_USE_LEGACY_BIT   (1<<0)
#define BT_GAP_USE_EXTENDED_BIT (1<<1)
/**
 * Legacy and extended support
 */
typedef enum {
    /** Use legacy advertise, scan and initiate. */
    BT_GAP_USE_LEGACY      = 0x1,
    /** Use extended advertise, scan and initiate. */
    BT_GAP_USE_EXTENDED    = 0x2,
    /** Both legacy and extended advertise, scan and initiate will be used. */
    BT_GAP_USE_LEG_AND_EXT = 0x3,
} bt_gap_use_t;

/**
 * GAP advertising data (AD) type values.
 * See "Supplement to the Bluetooth Core Specification" Part A, section 1.
 */
#define GAP_AD_TYPE_FLAGS                       0x01
#define GAP_AD_TYPE_SERVICE_16_INCOMPLETE       0x02
#define GAP_AD_TYPE_SERVICE_16_COMPLETE         0x03
#define GAP_AD_TYPE_SERVICE_32_INCOMPLETE       0x04
#define GAP_AD_TYPE_SERVICE_32_COMPLETE         0x05
#define GAP_AD_TYPE_SERVICE_128_INCOMPLETE      0x06
#define GAP_AD_TYPE_SERVICE_128_COMPLETE        0x07
#define GAP_AD_TYPE_LOCAL_NAME_SHORTENED        0x08
#define GAP_AD_TYPE_LOCAL_NAME_COMPLETE         0x09
#define GAP_AD_TYPE_TX_POWER_LEVEL              0x0A /* 0xXX: -127 to +127 dBm */
#define GAP_AD_TYPE_CLASS_OF_DEVICE             0x0D
#define GAP_AD_TYPE_SIMPLE_PAIRING_HASH_C       0x0E
#define GAP_AD_TYPE_SIMPLE_PAIRING_RANDOMIZER_R 0x0F
#define GAP_AD_TYPE_DEVICE_ID                   0x10
#define GAP_AD_TYPE_SECURITY_MANAGER_TX_VALUE   0x10
#define GAP_AD_TYPE_SECURITY_MANAGER_OOB_FLAGS  0x11
#define GAP_AD_TYPE_SLAVE_CONNECTION_INTERVAL_RANGE 0x12 /* 4 - <min interval>, <max interval> */
#define GAP_AD_TYPE_SERVICE_SOLICITATION_16     0x14
#define GAP_AD_TYPE_SERVICE_SOLICITATION_128    0x15
#define GAP_AD_TYPE_SERVICE_AD_16               0x16
#define GAP_AD_TYPE_SERVICE_SOLICITATION_32     0x1F
#define GAP_AD_TYPE_SERVICE_AD_32               0x20
#define GAP_AD_TYPE_SERVICE_AD_128              0x21
#define GAP_AD_TYPE_LE_SECURE_CONFIRMATION      0x22
#define GAP_AD_TYPE_LE_SECURE_RANDOM            0x23
// more to come ...

/**
 * GAP Timers and Constants - see Vol 3, part C, Appendix A.
 * In s, ms or slots (=0.625 ms).
 */
#define TGAP_100 10240 /* Time span that a Buetooth device performs device discovery. */
#define TGAP_103 30720 /* Min time span that a device is in discoverable mode (required). */
#define TGAP_104 60000 /* Max time span that a device is in limited discoverable mode). */

#define TGAP_adv_fast_interval1_coded    240 /* (=150ms, 90-180) - Advertising interval LE Coded PHY and connectable. */
#define TGAP_adv_fast_interval1           80 /* (=50ms, 30-60) - Advertising interval LE 1M PHY and connectable. */
#define TGAP_adv_fast_interval2_coded    600 /* (=375ms, 300-450) - Advertising interval LE Coded PHY and non-connectable. */
#define TGAP_adv_fast_interval2          200 /* (=125ms, 100-150) - Advertising interval LE 1M PHY and non-connectable. */
#define TGAP_adv_fast_period           30000 /* Min time to perform advertising when user initiated. */
#define TGAP_adv_slow_interval_coded    4800 /* (=3s, 3-3.6s) - Advertising interval (background) LE Coded PHY. */
#define TGAP_adv_slow_interval          1600 /* (=1s, 1-1.2s) - Advertising interval (background) LE 1M PHY. */
#define TGAP_lim_adv_timeout           18000 /* Max time to remain advertising when in the limited discoverable mode (required). */

#define TGAP_conn_param_timeout        30000 /* Connection parameter update procedure timer. */
#define TGAP_conn_pause_central         1000 /* Central idle timer before connection update procedure. */
#define TGAP_conn_pause_peripheral      5000 /* Peripheral idle timer before connection update procedure. */

#define TGAP_initial_conn_interval_coded  96 /* (=120ms, 90-150) Connection interval (establishment) LE Coded PHY. */
#define TGAP_initial_conn_interval        32 /* (=40ms, 30-50) Connection interval (establishment) LE 1M PHY. */

#define TGAP_scan_fast_interval_coded    288 /* (=180ms, 90-180) Scan interval LE Coded PHY. */
#define TGAP_scan_fast_interval           96 /* (=60ms, 30-60)  Scan interval LE 1M PHY. */
#define TGAP_scan_period_coded         30720 /* Min time for general discovery procedure LE Coded PHY (gen/lim_disc_scan_min_coded). */
#define TGAP_scan_period               10240 /* Min time for general discovery procedure LE 1M PHY (gen/lim_disc_scan_min). */
#define TGAP_scan_fast_window_coded      144 /* (=90ms) Scan window LE Coded PHY (lim_disc_scan_int_coded=54). */
#define TGAP_scan_fast_window             48 /* (=30ms) Scan window LE 1M PHY (lim_disc_scan_int=18). */
#define TGAP_scan_slow_interval1_coded  6144 /* (=3.84s) Scan interval (background) LE Coded PHY. */
#define TGAP_scan_slow_interval1        2048 /* (=1.28s) Scan interval (background) LE 1M PHY. */
#define TGAP_scan_slow_interval2_coded 12288 /* (=7.68s) Scan interval (background) LE Coded PHY. */
#define TGAP_scan_slow_interval2        4096 /* (=2.56s)Scan interval (background) LE 1M PHY. */
#define TGAP_scan_slow_window1_coded      54 /* (=33.75ms) Scan window (background) LE Coded PHY. */
#define TGAP_scan_slow_window1            18 /* (=11.25ms) Scan window (background) LE 1M PHY. */
#define TGAP_scan_slow_window2_coded     108 /* (=67.5ms)  Scan window (background) LE Coded PHY. */
#define TGAP_scan_slow_window2            36 /* (=22.5ms)  Scan window (background) LE 1M PHY. */

#define TGAP_sync_scan_interval          512 /* (=320ms) Interval between the start of adjacent synchronization scan windows. */
#define TGAP_sync_scan_window            146 /* (=91.25ms) Duration of synchronization scan window. */
#define TGAP_sync_train_duration       30720 /* Duration of synchronizability mode (required). */
#define TGAP_sync_train_interval         128 /* (=80ms) Interval between synchronization train. */

#define TGAP_private_addr_int             15 /* (minutes) Time interval between private address change. */

/**
 * Broadcast mode - see Vol 3, part C, section 9.1
 */
typedef enum {
    /** Disable broadcast mode */
    GAP_BROADCAST_MODE_DISABLE = 0,
    /** Enable broadcast mode */
    GAP_BROADCAST_MODE_ENABLE = 1,
} gap_broadcast_mode_t;

/**
 * Discoverable mode - see Vol 3, part C, section 9.2
 */
typedef enum {
    /** Disable discoverable mode */
    GAP_DISCOVERABLE_MODE_DISABLE = 0,
    /** No LE Discoverable Mode flag set in AD data. */
    GAP_DISCOVERABLE_MODE_NON     = 1,
    /** LE Limited Discoverable Mode flag set in AD data */
    GAP_DISCOVERABLE_MODE_LIMITED = 2,
    /** LE General Discoverable Mode flag set in AD data */
    GAP_DISCOVERABLE_MODE_GENERAL = 3,

} gap_discoverable_mode_t;

/**
 * Discovery mode - see Vol 3, part C, section 9.2
 */
typedef enum {
    /** Disable discovery mode */
    GAP_DISCOVERY_MODE_DISABLE = 0,
    /** Discover devices in limited discoverable mode */
    GAP_DISCOVERY_MODE_LIMITED = 1,
    /** Discover devices in general discoverable mode */
    GAP_DISCOVERY_MODE_GENERAL = 2,
    /** Obtain the Bluetooth Device Name of a remote connectable device */
    GAP_DISCOVERY_MODE_NAME    = 3,
} gap_discovery_mode_t;

/**
 * Connectable mode - see Vol 3, part C, section 9.3
 */
typedef enum {
    /** Disable connectable mode */
    GAP_CONNECTABLE_MODE_DISABLE  = 0,
    /** Do not allow a connection to be established */
    GAP_CONNECTABLE_MODE_NON      = 1,
    /** Accept a connection request from a known peer device */
    GAP_CONNECTABLE_MODE_DIRECT   = 2,
    /** Accept a connection request from a any device */
    GAP_CONNECTABLE_MODE_UNDIRECT = 3,
} gap_connectable_mode_t;

/**
 * Connection mode - see Vol 3, part C, section 9.3
 */
typedef enum {
    /** Disable connection mode */
    GAP_CONNECTION_MODE_DISABLE   = 0,
    /** Use Auto Connection Establishment Procedure - requires a white list */
    GAP_CONNECTION_MODE_AUTO      = 1,
    /** General Connection Establishment Procedure */
    GAP_CONNECTION_MODE_GENERAL   = 2,
    /** Selective Connection Establishment Procedure - requires a white list  */
    GAP_CONNECTION_MODE_SELECTIVE = 3,
    /** Direct Connection Establishment Procedure */
    GAP_CONNECTION_MODE_DIRECT    = 4,
} gap_connection_mode_t;

/**
 * GAP callback functions
 */
typedef struct gap_ops_t {
    /** hci_event Event as a result of a connection procedure */
    void (*connected_cb)(bt_hci_event_t *hci_event);
    /** hci_event Event as a result of a disconnection */
    void (*disconnected_cb)(bt_hci_event_t *hci_event);
    /** hci_event Event as a result of a discovery procedure */
    void (*discovery_cb)(bt_hci_event_t *hci_event);
} gap_ops_t;

extern const bt_address_t address_zero;
extern const bt_hci_addr_type_t addr_type_zero;
extern struct os_workqueue bt_host_workqueue;

/**
 * @struct bt_gap_cfg_adv_t
 * Configure advertisment.
 */
typedef struct bt_gap_cfg_adv_t {
    /** fast_period In ms (0 = infinit), using fast_int for fast_period, then using slow_int (default: 0) */
    uint16_t fast_period;
    /* slow_period In ms (0 = infinit), using slow_int for slow_period, then disable advertise (default: 0) */
    uint16_t slow_period;
    /** fast_int In 625 us, range: 0x0020 to 0x4000 (default: 200) */
    uint16_t fast_interval;
    /** slow_int In 625 us, range: 0x0020 to 0x4000 (default: 1600) */
    uint16_t slow_interval;
    /** tx_power In dBm, range: -127 to 10, and 127 (127=no preference) (default: 127) */
    int8_t tx_power;
    /** dv_ch_map Channel map used: bit0=ch37, bit1=ch38, bit2=ch39 (default: 0x7) */
    uint8_t channel_map;
} bt_gap_cfg_adv_t;

/**
 * @struct bt_gap_cfg_scan_t
 * Configure scan. Background scan is used if device has existing connections.
 */
typedef struct bt_gap_cfg_scan_t {
    /** period Scan period in ms, foreground scanning (no connected link) (default: 10240) */
    uint16_t period;
    /* interval Scan interval in 625 us, range: 0x0004 to 0x4000 (default: 96) */
    uint16_t interval;
    /** win Scan window in 625 us, range: 0x0004 to 0x4000 (default: 48) */
    uint16_t window;
    /** bkg_int Background scan interval in 625 us, range: 0x0004 to 0x4000 (default: 2048) */
    uint16_t background_interval;
    /** bkg_win Background scan window in 625 us, range: 0x0004 to 0x4000 (default: 18) */
    uint16_t background_window;
    /** filter_duplicates Filter duplicates (1=True, 0=False) (default: 1) */
    uint8_t filter_duplicates;
} bt_gap_cfg_scan_t;

/**
 * @struct bt_gap_cfg_conn_t
 * Configure connection
 */
typedef struct bt_gap_cfg_conn_t {
    /** interval Connection interval in 1.25 ms, range: 0x0006 to 0x0C80 (default: 80) */
    uint16_t interval;
    /** latency Connection latency in intervals, range: 0x0000 to 0x01F3 (default: 0) */
    uint16_t latency;
    /** timeout Connection timeout in ms, range: 0x000A to 0x0C80 (default: 2000) */
    uint16_t timeout;
    /** params_int_min Parameter update min connection interval in 1.25 ms (default: 6) */
    uint16_t params_int_min;
    /** params_int_max Parameter update max connection interval in 1.25 ms (default: 800) */
    uint16_t params_int_max;
    /** params_reject Parameter update reject (1=True, 0=False) (default: 0) */
    uint8_t params_reject;
} bt_gap_cfg_conn_t;


/**
 * @struct bt_gap_cfg_ext_t
 * Configure extended.
 */
typedef struct bt_gap_cfg_ext_t {
    /** use Extended (5.0) usage: 1-legacy, 2-extended, 3-legacy+extended (default: 1) */
    uint8_t use;
    /** adv_pri_phy Primary PHY: 1-LE 1M, 3-LE Coded (default: 3) */
    uint8_t adv_pri_phy;
    /** adv_sec_phy Secondary PHY:  1-LE 1M, (2-LE 2M,) 3-LE Coded (default: 3) */
    uint8_t adv_sec_phy;
    /** adv_sid Advertising SID (0..15) (default: 13) */
    uint8_t adv_sid;
    /** conn_phy Preferred phy(s) during connection - bit0: 1M, bit1: 2M, bit2: CodedS2, bit3: CodedS8, 0=no preference (default: 2M) */
    uint8_t conn_phy;
    /** conn_len Max length of transmitted data during connection (27..251) (default: 27) */
    uint8_t conn_len;
} bt_gap_cfg_ext_t;

/**
 * @struct bt_gap_cfg_t
 * GAP configuration
 */
typedef struct bt_gap_cfg_t {
    bt_gap_cfg_adv_t adv;
    bt_gap_cfg_scan_t scan;
    bt_gap_cfg_conn_t conn;
    bt_gap_cfg_ext_t ext;
    // 64 bytes
    bt_address_t address[2]; /* public (0) is not configurable */
    // 80 bytes padded
} bt_gap_cfg_t;

/* Interface to be used by Application (external) */
int bt_gap_init(void);
int bt_gap_destroy(void);
void bt_gap_cfg_init_default(struct bt_gap_cfg_t *cfg);
void bt_gap_cfg_get(struct bt_gap_cfg_t *cfg);

bt_gap_error_t bt_gap_addr_set(const bt_hci_addr_type_t type, const bt_address_t addr);
bt_gap_error_t bt_gap_addr_get(const bt_hci_addr_type_t type, bt_address_t addr);
bt_gap_error_t bt_gap_cfg_adv_set(const bt_gap_cfg_adv_t *adv_cfg);
bt_gap_error_t bt_gap_set_adv_data(const uint8_t length, const uint8_t data[31]);
bt_gap_error_t bt_gap_set_srsp_data(const uint8_t length, const uint8_t data[31]);
bt_gap_error_t bt_gap_cfg_scan_set(const bt_gap_cfg_scan_t *scan_cfg);
bt_gap_error_t bt_gap_cfg_ext_set(const bt_gap_cfg_ext_t *ext_cfg);
bt_gap_error_t bt_gap_cfg_conn_set(const bt_gap_cfg_conn_t *conn_cfg);
bt_gap_error_t bt_gap_add_device_to_white_list(const bt_hci_addr_type_t type, const bt_address_t address);
bt_gap_error_t bt_gap_remove_device_from_white_list(const bt_hci_addr_type_t type, const bt_address_t address);
bt_gap_error_t bt_gap_clear_white_list(void);
bt_gap_error_t bt_gap_add_device_to_resolving_list(const bt_hci_addr_type_t type, const bt_address_t address, const uint8_t peer_irk[16], const uint8_t local_irk[16]);
bt_gap_error_t bt_gap_remove_device_from_resolving_list(const bt_hci_addr_type_t type, const bt_address_t address);
bt_gap_error_t bt_gap_clear_resolving_list(void);
bt_gap_error_t bt_gap_set_address_resolution_enable(uint16_t timeout, uint8_t enable);
bt_gap_error_t bt_gap_connection_update(const uint16_t handle, const uint16_t interval_min, const uint16_t interval_max,
                                        const uint16_t latency, const uint16_t timeout);

bt_gap_error_t bt_gap_broadcast_mode(const gap_broadcast_mode_t mode, const bt_hci_addr_type_t own_type, const bt_hci_addr_type_t peer_type,
                                     const bt_address_t peer_address);
bt_gap_error_t bt_gap_discoverable_mode(const gap_discoverable_mode_t mode, const bt_hci_addr_type_t own_type, const bt_hci_addr_type_t peer_type,
                                        const bt_address_t peer_address, const gap_ops_t *ops);
bt_gap_error_t bt_gap_discovery_mode(const gap_discovery_mode_t mode, const bt_hci_addr_type_t own_type, const bt_hci_addr_type_t peer_type,
                                     const bt_address_t peer_address, const gap_ops_t *ops);
bt_gap_error_t bt_gap_connectable_mode(const gap_connectable_mode_t mode, const bt_hci_addr_type_t own_type, const bt_hci_addr_type_t peer_type,
                                       const bt_address_t peer_address, const gap_ops_t *ops);
bt_gap_error_t bt_gap_connection_mode(const gap_connection_mode_t mode, const bt_hci_addr_type_t own_type, const bt_hci_addr_type_t peer_type,
                                      const bt_address_t peer_address, const gap_ops_t *ops);
bt_gap_error_t bt_gap_terminate_connection(const uint16_t handle, const bt_error_t reason);
bt_gap_error_t bt_gap_authenticate(const uint8_t handle, const uint8_t oob, const uint8_t bondable, const uint8_t mitm, const uint8_t sc, const uint8_t key128);
bt_gap_error_t bt_gap_read_rssi(const uint8_t handle, void (*cb)(uint8_t handle, int8_t rssi));

struct gatt_srv_link *bt_gap_server_link_add(const uint8_t handle);
void bt_gap_server_link_remove(const struct gatt_srv_link *link);
struct gatt_cli_link *bt_gap_client_link_add(const uint8_t handle);
void bt_gap_client_link_remove(const struct gatt_cli_link *link);

/* Hook for reception of unsupported CID */
typedef void (*bt_l2cap_cid_cb_t)(struct pfrag *pf);
void bt_l2cap_cid_cb(bt_l2cap_cid_cb_t cid_cb);

/** @} */
