#pragma once
#include <kernel/hostio.h>
#include <string.h>

/*
[[[cog
import cog, generate
generate.doxygen(generate.Emitter(cog.out), 'bt_host')
]]] */
/**
 * @addtogroup hioapi_bt_host
 * @{
 *
 * This interface is used to run BT in host mode.
 * See BT specification, Vol 3: Host Volume, part C: GAP, chapter 9.2, 9.3 and 9.10 for
 * GAP discoverable, GAP discovery, GAP connectable, GAP connection, GAP bonding modes and
 * GAP termination.
 *
 * <table>
 * <caption>Message summary (group=9)</caption>
 * <tr><th>OPERATION</th><th>REQ</th><th>RSP</th><th>IND</th></tr>
 * <tr><td>gap_addr_set</td><td>0x00 - bt_host_gap_addr_set_req</td><td>0x80 - bt_host_gap_addr_set_rsp</td><td></td></tr>
 * <tr><td>gap_addr_get</td><td>0x01 - bt_host_gap_addr_get_req</td><td>0x81 - bt_host_gap_addr_get_rsp</td><td></td></tr>
 * <tr><td>gap_cfg_adv</td><td>0x02 - bt_host_gap_cfg_adv_req</td><td>0x82 - bt_host_gap_cfg_adv_rsp</td><td></td></tr>
 * <tr><td>gap_set_adv_data</td><td>0x03 - bt_host_gap_set_adv_data_req</td><td>0x83 - bt_host_gap_set_adv_data_rsp</td><td></td></tr>
 * <tr><td>gap_set_srsp_data</td><td>0x04 - bt_host_gap_set_srsp_data_req</td><td>0x84 - bt_host_gap_set_srsp_data_rsp</td><td></td></tr>
 * <tr><td>gap_cfg_scan</td><td>0x05 - bt_host_gap_cfg_scan_req</td><td>0x85 - bt_host_gap_cfg_scan_rsp</td><td></td></tr>
 * <tr><td>gap_cfg_conn</td><td>0x06 - bt_host_gap_cfg_conn_req</td><td>0x86 - bt_host_gap_cfg_conn_rsp</td><td></td></tr>
 * <tr><td>gap_cfg_smp</td><td>0x07 - bt_host_gap_cfg_smp_req</td><td>0x87 - bt_host_gap_cfg_smp_rsp</td><td></td></tr>
 * <tr><td>gap_cfg_ext</td><td>0x08 - bt_host_gap_cfg_ext_req</td><td>0x88 - bt_host_gap_cfg_ext_rsp</td><td></td></tr>
 * <tr><td>gap_broadcast</td><td>0x09 - bt_host_gap_broadcast_req</td><td>0x89 - bt_host_gap_broadcast_rsp</td><td></td></tr>
 * <tr><td>gap_discoverable</td><td>0x0a - bt_host_gap_discoverable_req</td><td>0x8a - bt_host_gap_discoverable_rsp</td><td></td></tr>
 * <tr><td>gap_discovery</td><td>0x0b - bt_host_gap_discovery_req</td><td>0x8b - bt_host_gap_discovery_rsp</td><td></td></tr>
 * <tr><td>gap_connectable</td><td>0x0c - bt_host_gap_connectable_req</td><td>0x8c - bt_host_gap_connectable_rsp</td><td></td></tr>
 * <tr><td>gap_connection</td><td>0x0d - bt_host_gap_connection_req</td><td>0x8d - bt_host_gap_connection_rsp</td><td></td></tr>
 * <tr><td>gap_terminate</td><td>0x0e - bt_host_gap_terminate_req</td><td>0x8e - bt_host_gap_terminate_rsp</td><td></td></tr>
 * <tr><td>gap_authenticate</td><td>0x0f - bt_host_gap_authenticate_req</td><td>0x8f - bt_host_gap_authenticate_rsp</td><td></td></tr>
 * <tr><td>gap_connection_update</td><td>0x10 - bt_host_gap_connection_update_req</td><td>0x90 - bt_host_gap_connection_update_rsp</td><td></td></tr>
 * <tr><td>gap_add_device_to_white_list</td><td>0x11 - bt_host_gap_add_device_to_white_list_req</td><td>0x91 - bt_host_gap_add_device_to_white_list_rsp</td><td></td></tr>
 * <tr><td>gap_remove_device_from_white_list</td><td>0x12 - bt_host_gap_remove_device_from_white_list_req</td><td>0x92 - bt_host_gap_remove_device_from_white_list_rsp</td><td></td></tr>
 * <tr><td>gap_clear_white_list</td><td>0x13 - bt_host_gap_clear_white_list_req</td><td>0x93 - bt_host_gap_clear_white_list_rsp</td><td></td></tr>
 * <tr><td>gap_add_device_to_resolving_list</td><td>0x14 - bt_host_gap_add_device_to_resolving_list_req</td><td>0x94 - bt_host_gap_add_device_to_resolving_list_rsp</td><td></td></tr>
 * <tr><td>gap_remove_device_from_resolving_list</td><td>0x15 - bt_host_gap_remove_device_from_resolving_list_req</td><td>0x95 - bt_host_gap_remove_device_from_resolving_list_rsp</td><td></td></tr>
 * <tr><td>gap_clear_resolving_list</td><td>0x16 - bt_host_gap_clear_resolving_list_req</td><td>0x96 - bt_host_gap_clear_resolving_list_rsp</td><td></td></tr>
 * <tr><td>gap_set_address_resolution_enable</td><td>0x17 - bt_host_gap_set_address_resolution_enable_req</td><td>0x97 - bt_host_gap_set_address_resolution_enable_rsp</td><td></td></tr>
 * <tr><td>gap_event</td><td></td><td></td><td>0xd8 - bt_host_gap_event_ind</td></tr>
 * <tr><td>smp_passkey</td><td>0x19 - bt_host_smp_passkey_req</td><td>0x99 - bt_host_smp_passkey_rsp</td><td></td></tr>
 * <tr><td>gatt_exchange_mtu</td><td>0x1a - bt_host_gatt_exchange_mtu_req</td><td>0x9a - bt_host_gatt_exchange_mtu_rsp</td><td></td></tr>
 * <tr><td>gatt_discover_all_primary_services</td><td>0x1b - bt_host_gatt_discover_all_primary_services_req</td><td>0x9b - bt_host_gatt_discover_all_primary_services_rsp</td><td>0xdb - bt_host_gatt_discover_all_primary_services_ind</td></tr>
 * <tr><td>gatt_discover_primary_service_by_service_uuid</td><td>0x1c - bt_host_gatt_discover_primary_service_by_service_uuid_req</td><td>0x9c - bt_host_gatt_discover_primary_service_by_service_uuid_rsp</td><td>0xdc - bt_host_gatt_discover_primary_service_by_service_uuid_ind</td></tr>
 * <tr><td>gatt_find_included_services</td><td>0x1d - bt_host_gatt_find_included_services_req</td><td>0x9d - bt_host_gatt_find_included_services_rsp</td><td>0xdd - bt_host_gatt_find_included_services_ind</td></tr>
 * <tr><td>gatt_discover_all_characteristics_of_a_service</td><td>0x1e - bt_host_gatt_discover_all_characteristics_of_a_service_req</td><td>0x9e - bt_host_gatt_discover_all_characteristics_of_a_service_rsp</td><td>0xde - bt_host_gatt_discover_all_characteristics_of_a_service_ind</td></tr>
 * <tr><td>gatt_discover_characteristics_by_uuid</td><td>0x1f - bt_host_gatt_discover_characteristics_by_uuid_req</td><td>0x9f - bt_host_gatt_discover_characteristics_by_uuid_rsp</td><td>0xdf - bt_host_gatt_discover_characteristics_by_uuid_ind</td></tr>
 * <tr><td>gatt_discover_all_characteristic_descriptors</td><td>0x20 - bt_host_gatt_discover_all_characteristic_descriptors_req</td><td>0xa0 - bt_host_gatt_discover_all_characteristic_descriptors_rsp</td><td>0xe0 - bt_host_gatt_discover_all_characteristic_descriptors_ind</td></tr>
 * <tr><td>gatt_read_characteristic_value</td><td>0x21 - bt_host_gatt_read_characteristic_value_req</td><td>0xa1 - bt_host_gatt_read_characteristic_value_rsp</td><td>0xe1 - bt_host_gatt_read_characteristic_value_ind</td></tr>
 * <tr><td>gatt_read_using_characteristic_uuid</td><td>0x22 - bt_host_gatt_read_using_characteristic_uuid_req</td><td>0xa2 - bt_host_gatt_read_using_characteristic_uuid_rsp</td><td>0xe2 - bt_host_gatt_read_using_characteristic_uuid_ind</td></tr>
 * <tr><td>gatt_read_long_characteristic_value</td><td>0x23 - bt_host_gatt_read_long_characteristic_value_req</td><td>0xa3 - bt_host_gatt_read_long_characteristic_value_rsp</td><td>0xe3 - bt_host_gatt_read_long_characteristic_value_ind</td></tr>
 * <tr><td>gatt_read_multiple_characteristic_values</td><td>0x24 - bt_host_gatt_read_multiple_characteristic_values_req</td><td>0xa4 - bt_host_gatt_read_multiple_characteristic_values_rsp</td><td>0xe4 - bt_host_gatt_read_multiple_characteristic_values_ind</td></tr>
 * <tr><td>gatt_read_characteristic_descriptor</td><td>0x25 - bt_host_gatt_read_characteristic_descriptor_req</td><td>0xa5 - bt_host_gatt_read_characteristic_descriptor_rsp</td><td>0xe5 - bt_host_gatt_read_characteristic_descriptor_ind</td></tr>
 * <tr><td>gatt_read_long_characteristic_descriptor</td><td>0x26 - bt_host_gatt_read_long_characteristic_descriptor_req</td><td>0xa6 - bt_host_gatt_read_long_characteristic_descriptor_rsp</td><td>0xe6 - bt_host_gatt_read_long_characteristic_descriptor_ind</td></tr>
 * <tr><td>gatt_write_without_response</td><td>0x27 - bt_host_gatt_write_without_response_req</td><td>0xa7 - bt_host_gatt_write_without_response_rsp</td><td></td></tr>
 * <tr><td>gatt_signed_write_without_response</td><td>0x28 - bt_host_gatt_signed_write_without_response_req</td><td>0xa8 - bt_host_gatt_signed_write_without_response_rsp</td><td></td></tr>
 * <tr><td>gatt_write_characteristic_value</td><td>0x29 - bt_host_gatt_write_characteristic_value_req</td><td>0xa9 - bt_host_gatt_write_characteristic_value_rsp</td><td></td></tr>
 * <tr><td>gatt_write_characteristic_descriptor</td><td>0x2a - bt_host_gatt_write_characteristic_descriptor_req</td><td>0xaa - bt_host_gatt_write_characteristic_descriptor_rsp</td><td></td></tr>
 * <tr><td>gatt_notification</td><td>0x2b - bt_host_gatt_notification_req</td><td>0xab - bt_host_gatt_notification_rsp</td><td></td></tr>
 * <tr><td>gatt_indication</td><td>0x2c - bt_host_gatt_indication_req</td><td>0xac - bt_host_gatt_indication_rsp</td><td></td></tr>
 * <tr><td>gatt_service_changed</td><td>0x2d - bt_host_gatt_service_changed_req</td><td>0xad - bt_host_gatt_service_changed_rsp</td><td></td></tr>
 * <tr><td>bt_iperf</td><td>0x2e - bt_host_bt_iperf_req</td><td>0xae - bt_host_bt_iperf_rsp</td><td>0xee - bt_host_bt_iperf_ind</td></tr>
 * <tr><td>smp_bond_list</td><td>0x2f - bt_host_smp_bond_list_req</td><td>0xaf - bt_host_smp_bond_list_rsp</td><td></td></tr>
 * <tr><td>smp_bond_remove_one</td><td>0x30 - bt_host_smp_bond_remove_one_req</td><td>0xb0 - bt_host_smp_bond_remove_one_rsp</td><td></td></tr>
 * <tr><td>smp_bond_remove_all</td><td>0x31 - bt_host_smp_bond_remove_all_req</td><td>0xb1 - bt_host_smp_bond_remove_all_rsp</td><td></td></tr>
 * </table>
 */
/* [[[end]]] */

/*
[[[cog
cog.outl('')
generate.header(generate.Emitter(cog.out), 'bt_host')
cog.outl('')
]]] */

#define HIO_GROUP_BT_HOST 9

/*
 * Message gap_addr_set
 */

#define BT_HOST_GAP_ADDR_SET_REQ 0x00
#define BT_HOST_GAP_ADDR_SET_RSP 0x80

/** set address for given address type. Request structure.
 */
struct bt_host_gap_addr_set_req {
    uint8_t addr_type; /**< address type to set: 0=public, 1=random */
    uint8_t addr[6]; /**< address */
};

static inline struct packet *
alloc_bt_host_gap_addr_set_req(struct bt_host_gap_addr_set_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_addr_set_req(const uint8_t addr_type, const uint8_t addr[6])
{
    struct bt_host_gap_addr_set_req *req;
    struct packet *pkt = alloc_bt_host_gap_addr_set_req(&req);
    if (pkt) {
        req->addr_type = addr_type;
        memcpy(req->addr, addr, sizeof req->addr);
    }
    return pkt;
}

/** set address for given address type. Response structure.
 */
struct bt_host_gap_addr_set_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_addr_set_rsp(struct bt_host_gap_addr_set_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_addr_set_rsp(const int32_t status)
{
    struct bt_host_gap_addr_set_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_addr_set_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_addr_get
 */

#define BT_HOST_GAP_ADDR_GET_REQ 0x01
#define BT_HOST_GAP_ADDR_GET_RSP 0x81

/** get address for given address type. Request structure.
 */
struct bt_host_gap_addr_get_req {
    uint8_t addr_type; /**< address type to get: 0=public, 1=random */
};

static inline struct packet *
alloc_bt_host_gap_addr_get_req(struct bt_host_gap_addr_get_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_addr_get_req(const uint8_t addr_type)
{
    struct bt_host_gap_addr_get_req *req;
    struct packet *pkt = alloc_bt_host_gap_addr_get_req(&req);
    if (pkt) {
        req->addr_type = addr_type;
    }
    return pkt;
}

/** get address for given address type. Response structure.
 */
struct bt_host_gap_addr_get_rsp {
    int32_t status; /**< result status, zero is success */
    uint8_t addr[6]; /**< address used for the given address type */
};

static inline struct packet *
alloc_bt_host_gap_addr_get_rsp(struct bt_host_gap_addr_get_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_addr_get_rsp(const int32_t status, const uint8_t addr[6])
{
    struct bt_host_gap_addr_get_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_addr_get_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        memcpy(rsp->addr, addr, sizeof rsp->addr);
    }
    return pkt;
}
/*
 * Message gap_cfg_adv
 */

#define BT_HOST_GAP_CFG_ADV_REQ 0x02
#define BT_HOST_GAP_CFG_ADV_RSP 0x82

/** configure advertise. Request structure.
 */
struct bt_host_gap_cfg_adv_req {
    uint16_t adv_fast_period; /**< In ms (0 = infinit), using adv_fast_int for adv_fast_period, then using adv_slow_int (default: 0) */
    uint16_t adv_slow_period; /**< In ms (0 = infinit), using adv_slow_int for adv_slow_period, then disable advertise (default: 0) */
    uint16_t adv_fast_int; /**< In 625 us, range: 0x0020 to 0x4000 (default: 160) */
    uint16_t adv_slow_int; /**< In 625 us, range: 0x0020 to 0x4000 (default: 1600) */
    int8_t adv_tx_power; /**< In dBm, range: -127 to 10, and 127 (127=no preference) (default: 127) */
    uint8_t adv_ch_map; /**< Channel map used: bit0=ch37, bit1=ch38, bit2=ch39 (default: 0x7) */
};

static inline struct packet *
alloc_bt_host_gap_cfg_adv_req(struct bt_host_gap_cfg_adv_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_cfg_adv_req(const uint16_t adv_fast_period, const uint16_t adv_slow_period, const uint16_t adv_fast_int, const uint16_t adv_slow_int, const int8_t adv_tx_power, const uint8_t adv_ch_map)
{
    struct bt_host_gap_cfg_adv_req *req;
    struct packet *pkt = alloc_bt_host_gap_cfg_adv_req(&req);
    if (pkt) {
        req->adv_fast_period = adv_fast_period;
        req->adv_slow_period = adv_slow_period;
        req->adv_fast_int = adv_fast_int;
        req->adv_slow_int = adv_slow_int;
        req->adv_tx_power = adv_tx_power;
        req->adv_ch_map = adv_ch_map;
    }
    return pkt;
}

/** configure advertise. Response structure.
 */
struct bt_host_gap_cfg_adv_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_cfg_adv_rsp(struct bt_host_gap_cfg_adv_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_cfg_adv_rsp(const int32_t status)
{
    struct bt_host_gap_cfg_adv_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_cfg_adv_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_set_adv_data
 */

#define BT_HOST_GAP_SET_ADV_DATA_REQ 0x03
#define BT_HOST_GAP_SET_ADV_DATA_RSP 0x83

/** set advertising data. Request structure.
 */
struct bt_host_gap_set_adv_data_req {
    uint8_t length; /**< number of significant octets in the advertising data (1..31) */
    uint8_t data[31]; /**< advertising data */
};

static inline struct packet *
alloc_bt_host_gap_set_adv_data_req(struct bt_host_gap_set_adv_data_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_set_adv_data_req(const uint8_t length, const uint8_t data[31])
{
    struct bt_host_gap_set_adv_data_req *req;
    struct packet *pkt = alloc_bt_host_gap_set_adv_data_req(&req);
    if (pkt) {
        req->length = length;
        memcpy(req->data, data, sizeof req->data);
    }
    return pkt;
}

/** set advertising data. Response structure.
 */
struct bt_host_gap_set_adv_data_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_set_adv_data_rsp(struct bt_host_gap_set_adv_data_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_set_adv_data_rsp(const int32_t status)
{
    struct bt_host_gap_set_adv_data_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_set_adv_data_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_set_srsp_data
 */

#define BT_HOST_GAP_SET_SRSP_DATA_REQ 0x04
#define BT_HOST_GAP_SET_SRSP_DATA_RSP 0x84

/** set scan response data. Request structure.
 */
struct bt_host_gap_set_srsp_data_req {
    uint8_t length; /**< number of significant octets in the scan response data (1..31) */
    uint8_t data[31]; /**< scan response data */
};

static inline struct packet *
alloc_bt_host_gap_set_srsp_data_req(struct bt_host_gap_set_srsp_data_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_set_srsp_data_req(const uint8_t length, const uint8_t data[31])
{
    struct bt_host_gap_set_srsp_data_req *req;
    struct packet *pkt = alloc_bt_host_gap_set_srsp_data_req(&req);
    if (pkt) {
        req->length = length;
        memcpy(req->data, data, sizeof req->data);
    }
    return pkt;
}

/** set scan response data. Response structure.
 */
struct bt_host_gap_set_srsp_data_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_set_srsp_data_rsp(struct bt_host_gap_set_srsp_data_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_set_srsp_data_rsp(const int32_t status)
{
    struct bt_host_gap_set_srsp_data_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_set_srsp_data_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_cfg_scan
 */

#define BT_HOST_GAP_CFG_SCAN_REQ 0x05
#define BT_HOST_GAP_CFG_SCAN_RSP 0x85

/** configure scan. Request structure.
 */
struct bt_host_gap_cfg_scan_req {
    uint16_t scan_period; /**< In ms, foreground scanning (no connected link) (default: 10240) */
    uint16_t scan_int; /**< In 625 us, range: 0x0004 to 0x4000 (default: 96) */
    uint16_t scan_win; /**< In 625 us, range: 0x0004 to 0x4000 (default: 48) */
    uint16_t scan_bkg_int; /**< In 625 us, range: 0x0004 to 0x4000 (default: 2048) */
    uint16_t scan_bkg_win; /**< In 625 us, range: 0x0004 to 0x4000 (default: 18) */
    uint8_t scan_filter_duplicates; /**< 1=True, 0=False (default: 1) */
};

static inline struct packet *
alloc_bt_host_gap_cfg_scan_req(struct bt_host_gap_cfg_scan_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_cfg_scan_req(const uint16_t scan_period, const uint16_t scan_int, const uint16_t scan_win, const uint16_t scan_bkg_int, const uint16_t scan_bkg_win, const uint8_t scan_filter_duplicates)
{
    struct bt_host_gap_cfg_scan_req *req;
    struct packet *pkt = alloc_bt_host_gap_cfg_scan_req(&req);
    if (pkt) {
        req->scan_period = scan_period;
        req->scan_int = scan_int;
        req->scan_win = scan_win;
        req->scan_bkg_int = scan_bkg_int;
        req->scan_bkg_win = scan_bkg_win;
        req->scan_filter_duplicates = scan_filter_duplicates;
    }
    return pkt;
}

/** configure scan. Response structure.
 */
struct bt_host_gap_cfg_scan_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_cfg_scan_rsp(struct bt_host_gap_cfg_scan_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_cfg_scan_rsp(const int32_t status)
{
    struct bt_host_gap_cfg_scan_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_cfg_scan_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_cfg_conn
 */

#define BT_HOST_GAP_CFG_CONN_REQ 0x06
#define BT_HOST_GAP_CFG_CONN_RSP 0x86

/** configure connection. Request structure.
 */
struct bt_host_gap_cfg_conn_req {
    uint16_t conn_interval; /**< in 1.25 ms, range: 0x0006 to 0x0C80 (default: 80) */
    uint16_t conn_latency; /**< in intervals, range: 0x0000 to 0x01F3 (default: 0) */
    uint16_t conn_timeout; /**<  in ms, range: 0x000A to 0x0C80 (default: 2000) */
    uint16_t conn_params_reject; /**< Reject parameter update, 1=True, 0=False (default: 0) */
    uint16_t conn_params_int_min; /**< in 1.25 ms, parameter update min connection interval (default: 6) */
    uint16_t conn_params_int_max; /**< in 1.25 ms, parameter update max connection interval (default: 800) */
};

static inline struct packet *
alloc_bt_host_gap_cfg_conn_req(struct bt_host_gap_cfg_conn_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_cfg_conn_req(const uint16_t conn_interval, const uint16_t conn_latency, const uint16_t conn_timeout, const uint16_t conn_params_reject, const uint16_t conn_params_int_min, const uint16_t conn_params_int_max)
{
    struct bt_host_gap_cfg_conn_req *req;
    struct packet *pkt = alloc_bt_host_gap_cfg_conn_req(&req);
    if (pkt) {
        req->conn_interval = conn_interval;
        req->conn_latency = conn_latency;
        req->conn_timeout = conn_timeout;
        req->conn_params_reject = conn_params_reject;
        req->conn_params_int_min = conn_params_int_min;
        req->conn_params_int_max = conn_params_int_max;
    }
    return pkt;
}

/** configure connection. Response structure.
 */
struct bt_host_gap_cfg_conn_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_cfg_conn_rsp(struct bt_host_gap_cfg_conn_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_cfg_conn_rsp(const int32_t status)
{
    struct bt_host_gap_cfg_conn_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_cfg_conn_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_cfg_smp
 */

#define BT_HOST_GAP_CFG_SMP_REQ 0x07
#define BT_HOST_GAP_CFG_SMP_RSP 0x87

/** configure security (smp). Request structure.
 */
struct bt_host_gap_cfg_smp_req {
    uint8_t io_cap; /**< I/O-capabilities: 0-display_only, 1-display_yes_no, 2-keyboard_only, 3-no_input_no_output, 4-keyboard_display (default: 0) */
    uint8_t oob; /**< OOB exists: 1=True, 0=False (default: 0) */
    uint8_t bondable; /**< bondable: 1=True, 0=False (default: 0) */
    uint8_t mitm; /**< MITM protection: 1=True, 0=False (default: 0) */
    uint8_t sc; /**< Secure connection: 1=True, 0=False (default: 0) */
    uint8_t keypress; /**< Send keypress: 1=True, 0=False (default: 0) */
    uint8_t key_size; /**< Smallest key size (7..16 octets) (default: 16) */
    uint8_t encrypt; /**< Automatically encrypt link at connection setup if key exists: 1=True, 0=False (default: 0) */
};

static inline struct packet *
alloc_bt_host_gap_cfg_smp_req(struct bt_host_gap_cfg_smp_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_cfg_smp_req(const uint8_t io_cap, const uint8_t oob, const uint8_t bondable, const uint8_t mitm, const uint8_t sc, const uint8_t keypress, const uint8_t key_size, const uint8_t encrypt)
{
    struct bt_host_gap_cfg_smp_req *req;
    struct packet *pkt = alloc_bt_host_gap_cfg_smp_req(&req);
    if (pkt) {
        req->io_cap = io_cap;
        req->oob = oob;
        req->bondable = bondable;
        req->mitm = mitm;
        req->sc = sc;
        req->keypress = keypress;
        req->key_size = key_size;
        req->encrypt = encrypt;
    }
    return pkt;
}

/** configure security (smp). Response structure.
 */
struct bt_host_gap_cfg_smp_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_cfg_smp_rsp(struct bt_host_gap_cfg_smp_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_cfg_smp_rsp(const int32_t status)
{
    struct bt_host_gap_cfg_smp_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_cfg_smp_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_cfg_ext
 */

#define BT_HOST_GAP_CFG_EXT_REQ 0x08
#define BT_HOST_GAP_CFG_EXT_RSP 0x88

/** configure extended. Request structure.
 */
struct bt_host_gap_cfg_ext_req {
    uint8_t ext_use; /**< Extended (5.0) usage: 1-legacy, 2-extended, 3-legacy+extended (default: 1) */
    uint8_t adv_pri_phy; /**< Primary PHY: 1-LE 1M, 3-LE Coded (default: 3) */
    uint8_t adv_sec_phy; /**< Secondary PHY:  1-LE 1M, 2-LE 2M, 3-LE Coded (default: 3) */
    uint8_t adv_sid; /**< Advertising SID (0..15) (default: 13) */
    uint8_t conn_phy; /**< Preferred phy(s) during connection - bit0: 1M, bit1: 2M, bit2: CodedS2, bit3: CodedS8, 0=no preference (default: 0x1) */
    uint8_t conn_len; /**< Max length of transmitted data during connection (27..251) (default: 27) */
};

static inline struct packet *
alloc_bt_host_gap_cfg_ext_req(struct bt_host_gap_cfg_ext_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_cfg_ext_req(const uint8_t ext_use, const uint8_t adv_pri_phy, const uint8_t adv_sec_phy, const uint8_t adv_sid, const uint8_t conn_phy, const uint8_t conn_len)
{
    struct bt_host_gap_cfg_ext_req *req;
    struct packet *pkt = alloc_bt_host_gap_cfg_ext_req(&req);
    if (pkt) {
        req->ext_use = ext_use;
        req->adv_pri_phy = adv_pri_phy;
        req->adv_sec_phy = adv_sec_phy;
        req->adv_sid = adv_sid;
        req->conn_phy = conn_phy;
        req->conn_len = conn_len;
    }
    return pkt;
}

/** configure extended. Response structure.
 */
struct bt_host_gap_cfg_ext_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_cfg_ext_rsp(struct bt_host_gap_cfg_ext_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_cfg_ext_rsp(const int32_t status)
{
    struct bt_host_gap_cfg_ext_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_cfg_ext_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_broadcast
 */

#define BT_HOST_GAP_BROADCAST_REQ 0x09
#define BT_HOST_GAP_BROADCAST_RSP 0x89

/** broadcast mode. Request structure.
 */
struct bt_host_gap_broadcast_req {
    uint8_t mode; /**< 0=disable, 1=enable */
    uint8_t own_type; /**< own address type: 0=public, 1=random, 2=resolvable (or public), 3=resolvable (or random) */
    uint8_t peer_type; /**< peer address type: 0=public, 1=random */
    uint8_t peer_addr[6]; /**< peer address */
};

static inline struct packet *
alloc_bt_host_gap_broadcast_req(struct bt_host_gap_broadcast_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_broadcast_req(const uint8_t mode, const uint8_t own_type, const uint8_t peer_type, const uint8_t peer_addr[6])
{
    struct bt_host_gap_broadcast_req *req;
    struct packet *pkt = alloc_bt_host_gap_broadcast_req(&req);
    if (pkt) {
        req->mode = mode;
        req->own_type = own_type;
        req->peer_type = peer_type;
        memcpy(req->peer_addr, peer_addr, sizeof req->peer_addr);
    }
    return pkt;
}

/** broadcast mode. Response structure.
 */
struct bt_host_gap_broadcast_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_broadcast_rsp(struct bt_host_gap_broadcast_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_broadcast_rsp(const int32_t status)
{
    struct bt_host_gap_broadcast_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_broadcast_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_discoverable
 */

#define BT_HOST_GAP_DISCOVERABLE_REQ 0x0a
#define BT_HOST_GAP_DISCOVERABLE_RSP 0x8a

/** discoverable mode. Request structure.
 */
struct bt_host_gap_discoverable_req {
    uint8_t mode; /**< 0=disable, 1=non, 2=limited, 3=general */
    uint8_t own_type; /**< own address type: 0=public, 1=random, 2=resolvable (or public), 3=resolvable (or random) */
    uint8_t peer_type; /**< peer address type: 0=public, 1=random */
    uint8_t peer_addr[6]; /**< peer address */
};

static inline struct packet *
alloc_bt_host_gap_discoverable_req(struct bt_host_gap_discoverable_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_discoverable_req(const uint8_t mode, const uint8_t own_type, const uint8_t peer_type, const uint8_t peer_addr[6])
{
    struct bt_host_gap_discoverable_req *req;
    struct packet *pkt = alloc_bt_host_gap_discoverable_req(&req);
    if (pkt) {
        req->mode = mode;
        req->own_type = own_type;
        req->peer_type = peer_type;
        memcpy(req->peer_addr, peer_addr, sizeof req->peer_addr);
    }
    return pkt;
}

/** discoverable mode. Response structure.
 */
struct bt_host_gap_discoverable_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_discoverable_rsp(struct bt_host_gap_discoverable_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_discoverable_rsp(const int32_t status)
{
    struct bt_host_gap_discoverable_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_discoverable_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_discovery
 */

#define BT_HOST_GAP_DISCOVERY_REQ 0x0b
#define BT_HOST_GAP_DISCOVERY_RSP 0x8b

/** discovery mode. Request structure.
 */
struct bt_host_gap_discovery_req {
    uint8_t mode; /**< 0=disable, 1=limited, 2=general, 3=name */
    uint8_t own_type; /**< own address type: 0=public, 1=random, 2=resolvable (or public), 3=resolvable (or random) */
    uint8_t peer_type; /**< peer address type (only for mode "name"): 0=public, 1=random, 2=public identity, 3=random identity */
    uint8_t peer_addr[6]; /**< peer address (only for mode "name") */
};

static inline struct packet *
alloc_bt_host_gap_discovery_req(struct bt_host_gap_discovery_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_discovery_req(const uint8_t mode, const uint8_t own_type, const uint8_t peer_type, const uint8_t peer_addr[6])
{
    struct bt_host_gap_discovery_req *req;
    struct packet *pkt = alloc_bt_host_gap_discovery_req(&req);
    if (pkt) {
        req->mode = mode;
        req->own_type = own_type;
        req->peer_type = peer_type;
        memcpy(req->peer_addr, peer_addr, sizeof req->peer_addr);
    }
    return pkt;
}

/** discovery mode. Response structure.
 */
struct bt_host_gap_discovery_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_discovery_rsp(struct bt_host_gap_discovery_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_discovery_rsp(const int32_t status)
{
    struct bt_host_gap_discovery_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_discovery_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_connectable
 */

#define BT_HOST_GAP_CONNECTABLE_REQ 0x0c
#define BT_HOST_GAP_CONNECTABLE_RSP 0x8c

/** connectable mode. Request structure.
 */
struct bt_host_gap_connectable_req {
    uint8_t mode; /**< 0=disable, 1=non, 2=direct, 3=undirect */
    uint8_t own_type; /**< own address type: 0=public, 1=random, 2=resolvable (or public), 3=resolvable (or random) */
    uint8_t peer_type; /**< peer address type: 0=public, 1=random */
    uint8_t peer_addr[6]; /**< peer address */
};

static inline struct packet *
alloc_bt_host_gap_connectable_req(struct bt_host_gap_connectable_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_connectable_req(const uint8_t mode, const uint8_t own_type, const uint8_t peer_type, const uint8_t peer_addr[6])
{
    struct bt_host_gap_connectable_req *req;
    struct packet *pkt = alloc_bt_host_gap_connectable_req(&req);
    if (pkt) {
        req->mode = mode;
        req->own_type = own_type;
        req->peer_type = peer_type;
        memcpy(req->peer_addr, peer_addr, sizeof req->peer_addr);
    }
    return pkt;
}

/** connectable mode. Response structure.
 */
struct bt_host_gap_connectable_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_connectable_rsp(struct bt_host_gap_connectable_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_connectable_rsp(const int32_t status)
{
    struct bt_host_gap_connectable_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_connectable_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_connection
 */

#define BT_HOST_GAP_CONNECTION_REQ 0x0d
#define BT_HOST_GAP_CONNECTION_RSP 0x8d

/** connection mode. Request structure.
 */
struct bt_host_gap_connection_req {
    uint8_t mode; /**< 0=disable, 1=auto, 2=general, 3=selective, 4=direct ("auto" and "selective" require a white list) */
    uint8_t own_type; /**< own address type: 0=public, 1=random, 2=resolvable (or public), 3=resolvable (or random) */
    uint8_t peer_type; /**< peer address type: 0=public, 1=random, 2=public identity, 3=random identity */
    uint8_t peer_addr[6]; /**< peer address */
};

static inline struct packet *
alloc_bt_host_gap_connection_req(struct bt_host_gap_connection_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_connection_req(const uint8_t mode, const uint8_t own_type, const uint8_t peer_type, const uint8_t peer_addr[6])
{
    struct bt_host_gap_connection_req *req;
    struct packet *pkt = alloc_bt_host_gap_connection_req(&req);
    if (pkt) {
        req->mode = mode;
        req->own_type = own_type;
        req->peer_type = peer_type;
        memcpy(req->peer_addr, peer_addr, sizeof req->peer_addr);
    }
    return pkt;
}

/** connection mode. Response structure.
 */
struct bt_host_gap_connection_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_connection_rsp(struct bt_host_gap_connection_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_connection_rsp(const int32_t status)
{
    struct bt_host_gap_connection_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_connection_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_terminate
 */

#define BT_HOST_GAP_TERMINATE_REQ 0x0e
#define BT_HOST_GAP_TERMINATE_RSP 0x8e

/** terminate connection. Request structure.
 */
struct bt_host_gap_terminate_req {
    uint8_t handle; /**< connection handle */
};

static inline struct packet *
alloc_bt_host_gap_terminate_req(struct bt_host_gap_terminate_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_terminate_req(const uint8_t handle)
{
    struct bt_host_gap_terminate_req *req;
    struct packet *pkt = alloc_bt_host_gap_terminate_req(&req);
    if (pkt) {
        req->handle = handle;
    }
    return pkt;
}

/** terminate connection. Response structure.
 */
struct bt_host_gap_terminate_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_terminate_rsp(struct bt_host_gap_terminate_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_terminate_rsp(const int32_t status)
{
    struct bt_host_gap_terminate_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_terminate_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_authenticate
 */

#define BT_HOST_GAP_AUTHENTICATE_REQ 0x0f
#define BT_HOST_GAP_AUTHENTICATE_RSP 0x8f

/** authenticate connection with provided settings. Request structure.
 */
struct bt_host_gap_authenticate_req {
    uint8_t handle; /**< connection handle */
    uint8_t oob; /**< OOB protection: 1=True, 0=False */
    uint8_t bondable; /**< bondable: 1=True, 0=False */
    uint8_t mitm; /**< MITM protection: 1=True, 0=False */
    uint8_t sc; /**< Secure connection: 1=True, 0=False */
    uint8_t key128; /**< 128-bits key required: 1=True, 0=False */
};

static inline struct packet *
alloc_bt_host_gap_authenticate_req(struct bt_host_gap_authenticate_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_authenticate_req(const uint8_t handle, const uint8_t oob, const uint8_t bondable, const uint8_t mitm, const uint8_t sc, const uint8_t key128)
{
    struct bt_host_gap_authenticate_req *req;
    struct packet *pkt = alloc_bt_host_gap_authenticate_req(&req);
    if (pkt) {
        req->handle = handle;
        req->oob = oob;
        req->bondable = bondable;
        req->mitm = mitm;
        req->sc = sc;
        req->key128 = key128;
    }
    return pkt;
}

/** authenticate connection with provided settings. Response structure.
 */
struct bt_host_gap_authenticate_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_authenticate_rsp(struct bt_host_gap_authenticate_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_authenticate_rsp(const int32_t status)
{
    struct bt_host_gap_authenticate_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_authenticate_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_connection_update
 */

#define BT_HOST_GAP_CONNECTION_UPDATE_REQ 0x10
#define BT_HOST_GAP_CONNECTION_UPDATE_RSP 0x90

/** update existing connection with new connection parameters. Request structure.
 */
struct bt_host_gap_connection_update_req {
    uint16_t handle; /**< connection handle */
    uint16_t interval_min; /**< in 1.25 ms, range: 0x0006 to 0x0C80 */
    uint16_t interval_max; /**< in 1.25 ms, range: 0x0006 to 0x0C80 */
    uint16_t latency; /**< in intervals, range: 0x0000 to 0x01F3 */
    uint16_t timeout; /**<  in ms, range: 0x000A to 0x0C80 */
};

static inline struct packet *
alloc_bt_host_gap_connection_update_req(struct bt_host_gap_connection_update_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_connection_update_req(const uint16_t handle, const uint16_t interval_min, const uint16_t interval_max, const uint16_t latency, const uint16_t timeout)
{
    struct bt_host_gap_connection_update_req *req;
    struct packet *pkt = alloc_bt_host_gap_connection_update_req(&req);
    if (pkt) {
        req->handle = handle;
        req->interval_min = interval_min;
        req->interval_max = interval_max;
        req->latency = latency;
        req->timeout = timeout;
    }
    return pkt;
}

/** update existing connection with new connection parameters. Response structure.
 */
struct bt_host_gap_connection_update_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_connection_update_rsp(struct bt_host_gap_connection_update_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_connection_update_rsp(const int32_t status)
{
    struct bt_host_gap_connection_update_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_connection_update_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_add_device_to_white_list
 */

#define BT_HOST_GAP_ADD_DEVICE_TO_WHITE_LIST_REQ 0x11
#define BT_HOST_GAP_ADD_DEVICE_TO_WHITE_LIST_RSP 0x91

/** add device to white list. Request structure.
 */
struct bt_host_gap_add_device_to_white_list_req {
    uint8_t addr_type; /**< address type: 0=public, 1=random */
    uint8_t addr[6]; /**< public or random device address */
};

static inline struct packet *
alloc_bt_host_gap_add_device_to_white_list_req(struct bt_host_gap_add_device_to_white_list_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_add_device_to_white_list_req(const uint8_t addr_type, const uint8_t addr[6])
{
    struct bt_host_gap_add_device_to_white_list_req *req;
    struct packet *pkt = alloc_bt_host_gap_add_device_to_white_list_req(&req);
    if (pkt) {
        req->addr_type = addr_type;
        memcpy(req->addr, addr, sizeof req->addr);
    }
    return pkt;
}

/** add device to white list. Response structure.
 */
struct bt_host_gap_add_device_to_white_list_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_add_device_to_white_list_rsp(struct bt_host_gap_add_device_to_white_list_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_add_device_to_white_list_rsp(const int32_t status)
{
    struct bt_host_gap_add_device_to_white_list_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_add_device_to_white_list_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_remove_device_from_white_list
 */

#define BT_HOST_GAP_REMOVE_DEVICE_FROM_WHITE_LIST_REQ 0x12
#define BT_HOST_GAP_REMOVE_DEVICE_FROM_WHITE_LIST_RSP 0x92

/** remove device from white list. Request structure.
 */
struct bt_host_gap_remove_device_from_white_list_req {
    uint8_t addr_type; /**< address type: 0=public, 1=random */
    uint8_t addr[6]; /**< public or random device address */
};

static inline struct packet *
alloc_bt_host_gap_remove_device_from_white_list_req(struct bt_host_gap_remove_device_from_white_list_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_remove_device_from_white_list_req(const uint8_t addr_type, const uint8_t addr[6])
{
    struct bt_host_gap_remove_device_from_white_list_req *req;
    struct packet *pkt = alloc_bt_host_gap_remove_device_from_white_list_req(&req);
    if (pkt) {
        req->addr_type = addr_type;
        memcpy(req->addr, addr, sizeof req->addr);
    }
    return pkt;
}

/** remove device from white list. Response structure.
 */
struct bt_host_gap_remove_device_from_white_list_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_remove_device_from_white_list_rsp(struct bt_host_gap_remove_device_from_white_list_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_remove_device_from_white_list_rsp(const int32_t status)
{
    struct bt_host_gap_remove_device_from_white_list_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_remove_device_from_white_list_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_clear_white_list
 */

#define BT_HOST_GAP_CLEAR_WHITE_LIST_REQ 0x13
#define BT_HOST_GAP_CLEAR_WHITE_LIST_RSP 0x93

/** clear white list. Request structure.
 */
struct bt_host_gap_clear_white_list_req {
};

static inline struct packet *
alloc_bt_host_gap_clear_white_list_req(struct bt_host_gap_clear_white_list_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_clear_white_list_req(void)
{
    struct bt_host_gap_clear_white_list_req *req;
    struct packet *pkt = alloc_bt_host_gap_clear_white_list_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** clear white list. Response structure.
 */
struct bt_host_gap_clear_white_list_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_clear_white_list_rsp(struct bt_host_gap_clear_white_list_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_clear_white_list_rsp(const int32_t status)
{
    struct bt_host_gap_clear_white_list_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_clear_white_list_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_add_device_to_resolving_list
 */

#define BT_HOST_GAP_ADD_DEVICE_TO_RESOLVING_LIST_REQ 0x14
#define BT_HOST_GAP_ADD_DEVICE_TO_RESOLVING_LIST_RSP 0x94

/** add device to resolving list. Request structure.
 */
struct bt_host_gap_add_device_to_resolving_list_req {
    uint8_t addr_type; /**< address type: 0=public, 1=random */
    uint8_t addr[6]; /**< public or random device address */
    uint8_t peer_irk[16]; /**< IRK of the peer device */
    uint8_t local_irk[16]; /**< IRK of the local device */
};

static inline struct packet *
alloc_bt_host_gap_add_device_to_resolving_list_req(struct bt_host_gap_add_device_to_resolving_list_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_add_device_to_resolving_list_req(const uint8_t addr_type, const uint8_t addr[6], const uint8_t peer_irk[16], const uint8_t local_irk[16])
{
    struct bt_host_gap_add_device_to_resolving_list_req *req;
    struct packet *pkt = alloc_bt_host_gap_add_device_to_resolving_list_req(&req);
    if (pkt) {
        req->addr_type = addr_type;
        memcpy(req->addr, addr, sizeof req->addr);
        memcpy(req->peer_irk, peer_irk, sizeof req->peer_irk);
        memcpy(req->local_irk, local_irk, sizeof req->local_irk);
    }
    return pkt;
}

/** add device to resolving list. Response structure.
 */
struct bt_host_gap_add_device_to_resolving_list_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_add_device_to_resolving_list_rsp(struct bt_host_gap_add_device_to_resolving_list_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_add_device_to_resolving_list_rsp(const int32_t status)
{
    struct bt_host_gap_add_device_to_resolving_list_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_add_device_to_resolving_list_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_remove_device_from_resolving_list
 */

#define BT_HOST_GAP_REMOVE_DEVICE_FROM_RESOLVING_LIST_REQ 0x15
#define BT_HOST_GAP_REMOVE_DEVICE_FROM_RESOLVING_LIST_RSP 0x95

/** remove device from resolving list. Request structure.
 */
struct bt_host_gap_remove_device_from_resolving_list_req {
    uint8_t addr_type; /**< address type: 0=public, 1=random */
    uint8_t addr[6]; /**< public or random device address */
};

static inline struct packet *
alloc_bt_host_gap_remove_device_from_resolving_list_req(struct bt_host_gap_remove_device_from_resolving_list_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_remove_device_from_resolving_list_req(const uint8_t addr_type, const uint8_t addr[6])
{
    struct bt_host_gap_remove_device_from_resolving_list_req *req;
    struct packet *pkt = alloc_bt_host_gap_remove_device_from_resolving_list_req(&req);
    if (pkt) {
        req->addr_type = addr_type;
        memcpy(req->addr, addr, sizeof req->addr);
    }
    return pkt;
}

/** remove device from resolving list. Response structure.
 */
struct bt_host_gap_remove_device_from_resolving_list_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_remove_device_from_resolving_list_rsp(struct bt_host_gap_remove_device_from_resolving_list_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_remove_device_from_resolving_list_rsp(const int32_t status)
{
    struct bt_host_gap_remove_device_from_resolving_list_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_remove_device_from_resolving_list_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_clear_resolving_list
 */

#define BT_HOST_GAP_CLEAR_RESOLVING_LIST_REQ 0x16
#define BT_HOST_GAP_CLEAR_RESOLVING_LIST_RSP 0x96

/** clear resolving list. Request structure.
 */
struct bt_host_gap_clear_resolving_list_req {
};

static inline struct packet *
alloc_bt_host_gap_clear_resolving_list_req(struct bt_host_gap_clear_resolving_list_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_clear_resolving_list_req(void)
{
    struct bt_host_gap_clear_resolving_list_req *req;
    struct packet *pkt = alloc_bt_host_gap_clear_resolving_list_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** clear resolving list. Response structure.
 */
struct bt_host_gap_clear_resolving_list_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_clear_resolving_list_rsp(struct bt_host_gap_clear_resolving_list_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_clear_resolving_list_rsp(const int32_t status)
{
    struct bt_host_gap_clear_resolving_list_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_clear_resolving_list_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_set_address_resolution_enable
 */

#define BT_HOST_GAP_SET_ADDRESS_RESOLUTION_ENABLE_REQ 0x17
#define BT_HOST_GAP_SET_ADDRESS_RESOLUTION_ENABLE_RSP 0x97

/** enable address resolution. Request structure.
 */
struct bt_host_gap_set_address_resolution_enable_req {
    uint16_t timeout; /**< Resolvable private address timeout in s (default: 900s) */
    uint8_t enable; /**< Enable: 1=True, 0=False (default: 0) */
};

static inline struct packet *
alloc_bt_host_gap_set_address_resolution_enable_req(struct bt_host_gap_set_address_resolution_enable_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_set_address_resolution_enable_req(const uint16_t timeout, const uint8_t enable)
{
    struct bt_host_gap_set_address_resolution_enable_req *req;
    struct packet *pkt = alloc_bt_host_gap_set_address_resolution_enable_req(&req);
    if (pkt) {
        req->timeout = timeout;
        req->enable = enable;
    }
    return pkt;
}

/** enable address resolution. Response structure.
 */
struct bt_host_gap_set_address_resolution_enable_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gap_set_address_resolution_enable_rsp(struct bt_host_gap_set_address_resolution_enable_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_set_address_resolution_enable_rsp(const int32_t status)
{
    struct bt_host_gap_set_address_resolution_enable_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gap_set_address_resolution_enable_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gap_event
 */

#define BT_HOST_GAP_EVENT_IND 0xd8

/** hci event. Indication structure.
 */
struct bt_host_gap_event_ind {
    uint8_t payload[0]; /**< hci event, see BT specification, Vol 2: Core, part E: HCI, chapter 7.7. */
};

static inline struct packet *
alloc_bt_host_gap_event_ind(struct bt_host_gap_event_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gap_event_ind(const uint8_t payload[0])
{
    struct bt_host_gap_event_ind *ind;
    struct packet *pkt = alloc_bt_host_gap_event_ind(&ind);
    if (pkt) {
        /* payload must be set by the caller */
    }
    return pkt;
}
/*
 * Message smp_passkey
 */

#define BT_HOST_SMP_PASSKEY_REQ 0x19
#define BT_HOST_SMP_PASSKEY_RSP 0x99

/** enter passkey (or oob) for smp pairing. Request structure.
 */
struct bt_host_smp_passkey_req {
    uint32_t key0; /**< 20 bits passkey or OOB0 (bits 0..31) */
    uint32_t oob1; /**< OOB1 (bits 32..63) */
    uint32_t oob2; /**< OOB2 (bits 64..95) */
    uint32_t oob3; /**< OOB3 (bits 96..127) */
};

static inline struct packet *
alloc_bt_host_smp_passkey_req(struct bt_host_smp_passkey_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_smp_passkey_req(const uint32_t key0, const uint32_t oob1, const uint32_t oob2, const uint32_t oob3)
{
    struct bt_host_smp_passkey_req *req;
    struct packet *pkt = alloc_bt_host_smp_passkey_req(&req);
    if (pkt) {
        req->key0 = key0;
        req->oob1 = oob1;
        req->oob2 = oob2;
        req->oob3 = oob3;
    }
    return pkt;
}

/** enter passkey (or oob) for smp pairing. Response structure.
 */
struct bt_host_smp_passkey_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_smp_passkey_rsp(struct bt_host_smp_passkey_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_smp_passkey_rsp(const int32_t status)
{
    struct bt_host_smp_passkey_rsp *rsp;
    struct packet *pkt = alloc_bt_host_smp_passkey_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gatt_exchange_mtu
 */

#define BT_HOST_GATT_EXCHANGE_MTU_REQ 0x1a
#define BT_HOST_GATT_EXCHANGE_MTU_RSP 0x9a

/** This sub-procedure is used by the client to set the ATT_MTU to the maximum possible value that can be supported by both
    devices when the client supports a value greater than the default ATT_MTU for the Attribute protocol. Request structure.
 */
struct bt_host_gatt_exchange_mtu_req {
    uint16_t size; /**< client rx mtu size (23..251) (default: 23) */
};

static inline struct packet *
alloc_bt_host_gatt_exchange_mtu_req(struct bt_host_gatt_exchange_mtu_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_exchange_mtu_req(const uint16_t size)
{
    struct bt_host_gatt_exchange_mtu_req *req;
    struct packet *pkt = alloc_bt_host_gatt_exchange_mtu_req(&req);
    if (pkt) {
        req->size = size;
    }
    return pkt;
}

/** This sub-procedure is used by the client to set the ATT_MTU to the maximum possible value that can be supported by both
    devices when the client supports a value greater than the default ATT_MTU for the Attribute protocol. Response structure.
 */
struct bt_host_gatt_exchange_mtu_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_exchange_mtu_rsp(struct bt_host_gatt_exchange_mtu_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_exchange_mtu_rsp(const int32_t status)
{
    struct bt_host_gatt_exchange_mtu_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_exchange_mtu_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gatt_discover_all_primary_services
 */

#define BT_HOST_GATT_DISCOVER_ALL_PRIMARY_SERVICES_REQ 0x1b
#define BT_HOST_GATT_DISCOVER_ALL_PRIMARY_SERVICES_RSP 0x9b
#define BT_HOST_GATT_DISCOVER_ALL_PRIMARY_SERVICES_IND 0xdb

/** This sub-procedure is used by a client to discover all the primary services on a server. Request structure.
 */
struct bt_host_gatt_discover_all_primary_services_req {
};

static inline struct packet *
alloc_bt_host_gatt_discover_all_primary_services_req(struct bt_host_gatt_discover_all_primary_services_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_all_primary_services_req(void)
{
    struct bt_host_gatt_discover_all_primary_services_req *req;
    struct packet *pkt = alloc_bt_host_gatt_discover_all_primary_services_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** This sub-procedure is used by a client to discover all the primary services on a server. Response structure.
 */
struct bt_host_gatt_discover_all_primary_services_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_discover_all_primary_services_rsp(struct bt_host_gatt_discover_all_primary_services_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_all_primary_services_rsp(const int32_t status)
{
    struct bt_host_gatt_discover_all_primary_services_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_discover_all_primary_services_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** This sub-procedure is used by a client to discover all the primary services on a server. Indication structure.
 */
struct bt_host_gatt_discover_all_primary_services_ind {
    uint16_t attr_handle; /**< handle for the service declaration */
    uint16_t end_handle; /**< handle of the last attribute within the service definition */
    uint16_t size; /**< service uuid size in bytes, 2-uuid16, 16-uuid128 */
    uint8_t uuid[16]; /**< service uuid - 16 or 128 bits */
};

static inline struct packet *
alloc_bt_host_gatt_discover_all_primary_services_ind(struct bt_host_gatt_discover_all_primary_services_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_all_primary_services_ind(const uint16_t attr_handle, const uint16_t end_handle, const uint16_t size, const uint8_t uuid[16])
{
    struct bt_host_gatt_discover_all_primary_services_ind *ind;
    struct packet *pkt = alloc_bt_host_gatt_discover_all_primary_services_ind(&ind);
    if (pkt) {
        ind->attr_handle = attr_handle;
        ind->end_handle = end_handle;
        ind->size = size;
        memcpy(ind->uuid, uuid, sizeof ind->uuid);
    }
    return pkt;
}
/*
 * Message gatt_discover_primary_service_by_service_uuid
 */

#define BT_HOST_GATT_DISCOVER_PRIMARY_SERVICE_BY_SERVICE_UUID_REQ 0x1c
#define BT_HOST_GATT_DISCOVER_PRIMARY_SERVICE_BY_SERVICE_UUID_RSP 0x9c
#define BT_HOST_GATT_DISCOVER_PRIMARY_SERVICE_BY_SERVICE_UUID_IND 0xdc

/** This sub-procedure is used by a client to discover a specific primary service on a server when only the Service UUID is known. Request structure.
 */
struct bt_host_gatt_discover_primary_service_by_service_uuid_req {
    uint16_t size; /**< uuid size in bytes, 2-uuid16, 16-uuid128 */
    uint8_t uuid[16]; /**< uuid - 16 or 128 bits */
};

static inline struct packet *
alloc_bt_host_gatt_discover_primary_service_by_service_uuid_req(struct bt_host_gatt_discover_primary_service_by_service_uuid_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_primary_service_by_service_uuid_req(const uint16_t size, const uint8_t uuid[16])
{
    struct bt_host_gatt_discover_primary_service_by_service_uuid_req *req;
    struct packet *pkt = alloc_bt_host_gatt_discover_primary_service_by_service_uuid_req(&req);
    if (pkt) {
        req->size = size;
        memcpy(req->uuid, uuid, sizeof req->uuid);
    }
    return pkt;
}

/** This sub-procedure is used by a client to discover a specific primary service on a server when only the Service UUID is known. Response structure.
 */
struct bt_host_gatt_discover_primary_service_by_service_uuid_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_discover_primary_service_by_service_uuid_rsp(struct bt_host_gatt_discover_primary_service_by_service_uuid_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_primary_service_by_service_uuid_rsp(const int32_t status)
{
    struct bt_host_gatt_discover_primary_service_by_service_uuid_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_discover_primary_service_by_service_uuid_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** This sub-procedure is used by a client to discover a specific primary service on a server when only the Service UUID is known. Indication structure.
 */
struct bt_host_gatt_discover_primary_service_by_service_uuid_ind {
    uint16_t start_handle; /**< starting handle of the service definition */
    uint16_t end_handle; /**< ending handle of the service definition */
};

static inline struct packet *
alloc_bt_host_gatt_discover_primary_service_by_service_uuid_ind(struct bt_host_gatt_discover_primary_service_by_service_uuid_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_primary_service_by_service_uuid_ind(const uint16_t start_handle, const uint16_t end_handle)
{
    struct bt_host_gatt_discover_primary_service_by_service_uuid_ind *ind;
    struct packet *pkt = alloc_bt_host_gatt_discover_primary_service_by_service_uuid_ind(&ind);
    if (pkt) {
        ind->start_handle = start_handle;
        ind->end_handle = end_handle;
    }
    return pkt;
}
/*
 * Message gatt_find_included_services
 */

#define BT_HOST_GATT_FIND_INCLUDED_SERVICES_REQ 0x1d
#define BT_HOST_GATT_FIND_INCLUDED_SERVICES_RSP 0x9d
#define BT_HOST_GATT_FIND_INCLUDED_SERVICES_IND 0xdd

/** This sub-procedure is used by a client to find include service declarations within a service definition on a server. Request structure.
 */
struct bt_host_gatt_find_included_services_req {
    uint16_t start_handle; /**< starting handle of the specified service */
    uint16_t end_handle; /**< ending handle of the specified service */
};

static inline struct packet *
alloc_bt_host_gatt_find_included_services_req(struct bt_host_gatt_find_included_services_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_find_included_services_req(const uint16_t start_handle, const uint16_t end_handle)
{
    struct bt_host_gatt_find_included_services_req *req;
    struct packet *pkt = alloc_bt_host_gatt_find_included_services_req(&req);
    if (pkt) {
        req->start_handle = start_handle;
        req->end_handle = end_handle;
    }
    return pkt;
}

/** This sub-procedure is used by a client to find include service declarations within a service definition on a server. Response structure.
 */
struct bt_host_gatt_find_included_services_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_find_included_services_rsp(struct bt_host_gatt_find_included_services_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_find_included_services_rsp(const int32_t status)
{
    struct bt_host_gatt_find_included_services_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_find_included_services_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** This sub-procedure is used by a client to find include service declarations within a service definition on a server. Indication structure.
 */
struct bt_host_gatt_find_included_services_ind {
    uint16_t attr_handle; /**< attribute handle */
    uint16_t isa_handle; /**<  handle */
    uint16_t end_handle; /**< end group handle */
    uint16_t size; /**< uuid size in bytes, 2-uuid16, 0-not included (=uuid128) */
    uint16_t uuid; /**< uuid16 (if size is 2, else not valid) */
};

static inline struct packet *
alloc_bt_host_gatt_find_included_services_ind(struct bt_host_gatt_find_included_services_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_find_included_services_ind(const uint16_t attr_handle, const uint16_t isa_handle, const uint16_t end_handle, const uint16_t size, const uint16_t uuid)
{
    struct bt_host_gatt_find_included_services_ind *ind;
    struct packet *pkt = alloc_bt_host_gatt_find_included_services_ind(&ind);
    if (pkt) {
        ind->attr_handle = attr_handle;
        ind->isa_handle = isa_handle;
        ind->end_handle = end_handle;
        ind->size = size;
        ind->uuid = uuid;
    }
    return pkt;
}
/*
 * Message gatt_discover_all_characteristics_of_a_service
 */

#define BT_HOST_GATT_DISCOVER_ALL_CHARACTERISTICS_OF_A_SERVICE_REQ 0x1e
#define BT_HOST_GATT_DISCOVER_ALL_CHARACTERISTICS_OF_A_SERVICE_RSP 0x9e
#define BT_HOST_GATT_DISCOVER_ALL_CHARACTERISTICS_OF_A_SERVICE_IND 0xde

/** This sub-procedure is used by a client to find all the characteristic declarations within a service definition on a server
    when only the service handle range is known. Request structure.
 */
struct bt_host_gatt_discover_all_characteristics_of_a_service_req {
    uint16_t start_handle; /**< starting handle of the specified service */
    uint16_t end_handle; /**< ending handle of the specified service */
};

static inline struct packet *
alloc_bt_host_gatt_discover_all_characteristics_of_a_service_req(struct bt_host_gatt_discover_all_characteristics_of_a_service_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_all_characteristics_of_a_service_req(const uint16_t start_handle, const uint16_t end_handle)
{
    struct bt_host_gatt_discover_all_characteristics_of_a_service_req *req;
    struct packet *pkt = alloc_bt_host_gatt_discover_all_characteristics_of_a_service_req(&req);
    if (pkt) {
        req->start_handle = start_handle;
        req->end_handle = end_handle;
    }
    return pkt;
}

/** This sub-procedure is used by a client to find all the characteristic declarations within a service definition on a server
    when only the service handle range is known. Response structure.
 */
struct bt_host_gatt_discover_all_characteristics_of_a_service_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_discover_all_characteristics_of_a_service_rsp(struct bt_host_gatt_discover_all_characteristics_of_a_service_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_all_characteristics_of_a_service_rsp(const int32_t status)
{
    struct bt_host_gatt_discover_all_characteristics_of_a_service_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_discover_all_characteristics_of_a_service_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** This sub-procedure is used by a client to find all the characteristic declarations within a service definition on a server
    when only the service handle range is known. Indication structure.
 */
struct bt_host_gatt_discover_all_characteristics_of_a_service_ind {
    uint16_t handle; /**< handle for the characteristic declaration */
    uint16_t prop; /**< characteristic properties */
    uint16_t value_handle; /**< characteristic value handle */
    uint16_t size; /**< characteristic uuid size in bytes, 2-uuid16, 16-uuid128 */
    uint8_t uuid[16]; /**< characteristic uuid - 16 or 128 bits */
};

static inline struct packet *
alloc_bt_host_gatt_discover_all_characteristics_of_a_service_ind(struct bt_host_gatt_discover_all_characteristics_of_a_service_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_all_characteristics_of_a_service_ind(const uint16_t handle, const uint16_t prop, const uint16_t value_handle, const uint16_t size, const uint8_t uuid[16])
{
    struct bt_host_gatt_discover_all_characteristics_of_a_service_ind *ind;
    struct packet *pkt = alloc_bt_host_gatt_discover_all_characteristics_of_a_service_ind(&ind);
    if (pkt) {
        ind->handle = handle;
        ind->prop = prop;
        ind->value_handle = value_handle;
        ind->size = size;
        memcpy(ind->uuid, uuid, sizeof ind->uuid);
    }
    return pkt;
}
/*
 * Message gatt_discover_characteristics_by_uuid
 */

#define BT_HOST_GATT_DISCOVER_CHARACTERISTICS_BY_UUID_REQ 0x1f
#define BT_HOST_GATT_DISCOVER_CHARACTERISTICS_BY_UUID_RSP 0x9f
#define BT_HOST_GATT_DISCOVER_CHARACTERISTICS_BY_UUID_IND 0xdf

/** This sub-procedure is used by a client to discover service characteristics on a server when only the service handle ranges are known
    and the characteristic UUID is known. Request structure.
 */
struct bt_host_gatt_discover_characteristics_by_uuid_req {
    uint16_t start_handle; /**< starting handle for the service handle range */
    uint16_t end_handle; /**< ending handle for the service handle range */
    uint16_t size; /**< uuid size in bytes, 2-uuid16, 16-uuid128 */
    uint8_t uuid[16]; /**< uuid - 16 or 128 bits */
};

static inline struct packet *
alloc_bt_host_gatt_discover_characteristics_by_uuid_req(struct bt_host_gatt_discover_characteristics_by_uuid_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_characteristics_by_uuid_req(const uint16_t start_handle, const uint16_t end_handle, const uint16_t size, const uint8_t uuid[16])
{
    struct bt_host_gatt_discover_characteristics_by_uuid_req *req;
    struct packet *pkt = alloc_bt_host_gatt_discover_characteristics_by_uuid_req(&req);
    if (pkt) {
        req->start_handle = start_handle;
        req->end_handle = end_handle;
        req->size = size;
        memcpy(req->uuid, uuid, sizeof req->uuid);
    }
    return pkt;
}

/** This sub-procedure is used by a client to discover service characteristics on a server when only the service handle ranges are known
    and the characteristic UUID is known. Response structure.
 */
struct bt_host_gatt_discover_characteristics_by_uuid_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_discover_characteristics_by_uuid_rsp(struct bt_host_gatt_discover_characteristics_by_uuid_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_characteristics_by_uuid_rsp(const int32_t status)
{
    struct bt_host_gatt_discover_characteristics_by_uuid_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_discover_characteristics_by_uuid_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** This sub-procedure is used by a client to discover service characteristics on a server when only the service handle ranges are known
    and the characteristic UUID is known. Indication structure.
 */
struct bt_host_gatt_discover_characteristics_by_uuid_ind {
    uint16_t attr_handle; /**< handle for the characteristic declaration */
    uint16_t prop; /**< characteristic properties */
    uint16_t value_handle; /**< characteristic value handle */
    uint16_t size; /**< characteristic uuid size in bytes, 2-uuid16, 16-uuid128 */
    uint8_t uuid[16]; /**< characteristic uuid - 16 or 128 bits */
};

static inline struct packet *
alloc_bt_host_gatt_discover_characteristics_by_uuid_ind(struct bt_host_gatt_discover_characteristics_by_uuid_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_characteristics_by_uuid_ind(const uint16_t attr_handle, const uint16_t prop, const uint16_t value_handle, const uint16_t size, const uint8_t uuid[16])
{
    struct bt_host_gatt_discover_characteristics_by_uuid_ind *ind;
    struct packet *pkt = alloc_bt_host_gatt_discover_characteristics_by_uuid_ind(&ind);
    if (pkt) {
        ind->attr_handle = attr_handle;
        ind->prop = prop;
        ind->value_handle = value_handle;
        ind->size = size;
        memcpy(ind->uuid, uuid, sizeof ind->uuid);
    }
    return pkt;
}
/*
 * Message gatt_discover_all_characteristic_descriptors
 */

#define BT_HOST_GATT_DISCOVER_ALL_CHARACTERISTIC_DESCRIPTORS_REQ 0x20
#define BT_HOST_GATT_DISCOVER_ALL_CHARACTERISTIC_DESCRIPTORS_RSP 0xa0
#define BT_HOST_GATT_DISCOVER_ALL_CHARACTERISTIC_DESCRIPTORS_IND 0xe0

/** This sub-procedure is used by a client to find all the characteristic descriptor's Attribute Handles and Attribute Types
    within a characteristic definition when only the characteristic handle range is known. Request structure.
 */
struct bt_host_gatt_discover_all_characteristic_descriptors_req {
    uint16_t start_handle; /**< starting handle of the specified service */
    uint16_t end_handle; /**< ending handle of the specified service */
};

static inline struct packet *
alloc_bt_host_gatt_discover_all_characteristic_descriptors_req(struct bt_host_gatt_discover_all_characteristic_descriptors_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_all_characteristic_descriptors_req(const uint16_t start_handle, const uint16_t end_handle)
{
    struct bt_host_gatt_discover_all_characteristic_descriptors_req *req;
    struct packet *pkt = alloc_bt_host_gatt_discover_all_characteristic_descriptors_req(&req);
    if (pkt) {
        req->start_handle = start_handle;
        req->end_handle = end_handle;
    }
    return pkt;
}

/** This sub-procedure is used by a client to find all the characteristic descriptor's Attribute Handles and Attribute Types
    within a characteristic definition when only the characteristic handle range is known. Response structure.
 */
struct bt_host_gatt_discover_all_characteristic_descriptors_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_discover_all_characteristic_descriptors_rsp(struct bt_host_gatt_discover_all_characteristic_descriptors_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_all_characteristic_descriptors_rsp(const int32_t status)
{
    struct bt_host_gatt_discover_all_characteristic_descriptors_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_discover_all_characteristic_descriptors_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** This sub-procedure is used by a client to find all the characteristic descriptor's Attribute Handles and Attribute Types
    within a characteristic definition when only the characteristic handle range is known. Indication structure.
 */
struct bt_host_gatt_discover_all_characteristic_descriptors_ind {
    uint16_t attr_handle; /**< handle for the characteristic descriptor declaration */
    uint16_t size; /**< characteristic uuid size in bytes, 2-uuid16, 16-uuid128 */
    uint8_t uuid[16]; /**< characteristic uuid - 16 or 128 bits */
};

static inline struct packet *
alloc_bt_host_gatt_discover_all_characteristic_descriptors_ind(struct bt_host_gatt_discover_all_characteristic_descriptors_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_discover_all_characteristic_descriptors_ind(const uint16_t attr_handle, const uint16_t size, const uint8_t uuid[16])
{
    struct bt_host_gatt_discover_all_characteristic_descriptors_ind *ind;
    struct packet *pkt = alloc_bt_host_gatt_discover_all_characteristic_descriptors_ind(&ind);
    if (pkt) {
        ind->attr_handle = attr_handle;
        ind->size = size;
        memcpy(ind->uuid, uuid, sizeof ind->uuid);
    }
    return pkt;
}
/*
 * Message gatt_read_characteristic_value
 */

#define BT_HOST_GATT_READ_CHARACTERISTIC_VALUE_REQ 0x21
#define BT_HOST_GATT_READ_CHARACTERISTIC_VALUE_RSP 0xa1
#define BT_HOST_GATT_READ_CHARACTERISTIC_VALUE_IND 0xe1

/** This sub-procedure is used to read a Characteristic Value from a server when the client knows the Characteristic Value Handle. Request structure.
 */
struct bt_host_gatt_read_characteristic_value_req {
    uint16_t value_handle; /**< value_handle to be read from remote server */
};

static inline struct packet *
alloc_bt_host_gatt_read_characteristic_value_req(struct bt_host_gatt_read_characteristic_value_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_characteristic_value_req(const uint16_t value_handle)
{
    struct bt_host_gatt_read_characteristic_value_req *req;
    struct packet *pkt = alloc_bt_host_gatt_read_characteristic_value_req(&req);
    if (pkt) {
        req->value_handle = value_handle;
    }
    return pkt;
}

/** This sub-procedure is used to read a Characteristic Value from a server when the client knows the Characteristic Value Handle. Response structure.
 */
struct bt_host_gatt_read_characteristic_value_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_read_characteristic_value_rsp(struct bt_host_gatt_read_characteristic_value_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_characteristic_value_rsp(const int32_t status)
{
    struct bt_host_gatt_read_characteristic_value_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_read_characteristic_value_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** This sub-procedure is used to read a Characteristic Value from a server when the client knows the Characteristic Value Handle. Indication structure.
 */
struct bt_host_gatt_read_characteristic_value_ind {
    uint16_t handle; /**< handle for the read characteristic */
    uint16_t size; /**< size of value in bytes */
    uint8_t value[0]; /**< value of read attribute */
};

static inline struct packet *
alloc_bt_host_gatt_read_characteristic_value_ind(struct bt_host_gatt_read_characteristic_value_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_characteristic_value_ind(const uint16_t handle, const uint16_t size, const uint8_t value[0])
{
    struct bt_host_gatt_read_characteristic_value_ind *ind;
    struct packet *pkt = alloc_bt_host_gatt_read_characteristic_value_ind(&ind);
    if (pkt) {
        ind->handle = handle;
        ind->size = size;
        /* value must be set by the caller */
    }
    return pkt;
}
/*
 * Message gatt_read_using_characteristic_uuid
 */

#define BT_HOST_GATT_READ_USING_CHARACTERISTIC_UUID_REQ 0x22
#define BT_HOST_GATT_READ_USING_CHARACTERISTIC_UUID_RSP 0xa2
#define BT_HOST_GATT_READ_USING_CHARACTERISTIC_UUID_IND 0xe2

/** This sub-procedure is used to read a Characteristic Value from a server when the client only knows the characteristic UUID. Request structure.
 */
struct bt_host_gatt_read_using_characteristic_uuid_req {
    uint16_t start_handle; /**< starting handle of the specified service */
    uint16_t end_handle; /**< ending handle of the specified service */
    uint16_t size; /**< uuid size in bytes, 2-uuid16, 16-uuid128 */
    uint8_t uuid[16]; /**< uuid - 16 or 128 bits */
};

static inline struct packet *
alloc_bt_host_gatt_read_using_characteristic_uuid_req(struct bt_host_gatt_read_using_characteristic_uuid_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_using_characteristic_uuid_req(const uint16_t start_handle, const uint16_t end_handle, const uint16_t size, const uint8_t uuid[16])
{
    struct bt_host_gatt_read_using_characteristic_uuid_req *req;
    struct packet *pkt = alloc_bt_host_gatt_read_using_characteristic_uuid_req(&req);
    if (pkt) {
        req->start_handle = start_handle;
        req->end_handle = end_handle;
        req->size = size;
        memcpy(req->uuid, uuid, sizeof req->uuid);
    }
    return pkt;
}

/** This sub-procedure is used to read a Characteristic Value from a server when the client only knows the characteristic UUID. Response structure.
 */
struct bt_host_gatt_read_using_characteristic_uuid_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_read_using_characteristic_uuid_rsp(struct bt_host_gatt_read_using_characteristic_uuid_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_using_characteristic_uuid_rsp(const int32_t status)
{
    struct bt_host_gatt_read_using_characteristic_uuid_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_read_using_characteristic_uuid_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** This sub-procedure is used to read a Characteristic Value from a server when the client only knows the characteristic UUID. Indication structure.
 */
struct bt_host_gatt_read_using_characteristic_uuid_ind {
    uint16_t handle; /**< handle for the read characteristic */
    uint16_t size; /**< size of value in bytes */
    uint8_t value[0]; /**< value of read attribute */
};

static inline struct packet *
alloc_bt_host_gatt_read_using_characteristic_uuid_ind(struct bt_host_gatt_read_using_characteristic_uuid_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_using_characteristic_uuid_ind(const uint16_t handle, const uint16_t size, const uint8_t value[0])
{
    struct bt_host_gatt_read_using_characteristic_uuid_ind *ind;
    struct packet *pkt = alloc_bt_host_gatt_read_using_characteristic_uuid_ind(&ind);
    if (pkt) {
        ind->handle = handle;
        ind->size = size;
        /* value must be set by the caller */
    }
    return pkt;
}
/*
 * Message gatt_read_long_characteristic_value
 */

#define BT_HOST_GATT_READ_LONG_CHARACTERISTIC_VALUE_REQ 0x23
#define BT_HOST_GATT_READ_LONG_CHARACTERISTIC_VALUE_RSP 0xa3
#define BT_HOST_GATT_READ_LONG_CHARACTERISTIC_VALUE_IND 0xe3

/** Same as gatt_read_characteristic_value, but for long values. Request structure.
 */
struct bt_host_gatt_read_long_characteristic_value_req {
    uint16_t value_handle; /**< value_handle to be read from remote server */
    uint16_t value_offset; /**< value_offset to be read */
};

static inline struct packet *
alloc_bt_host_gatt_read_long_characteristic_value_req(struct bt_host_gatt_read_long_characteristic_value_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_long_characteristic_value_req(const uint16_t value_handle, const uint16_t value_offset)
{
    struct bt_host_gatt_read_long_characteristic_value_req *req;
    struct packet *pkt = alloc_bt_host_gatt_read_long_characteristic_value_req(&req);
    if (pkt) {
        req->value_handle = value_handle;
        req->value_offset = value_offset;
    }
    return pkt;
}

/** Same as gatt_read_characteristic_value, but for long values. Response structure.
 */
struct bt_host_gatt_read_long_characteristic_value_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_read_long_characteristic_value_rsp(struct bt_host_gatt_read_long_characteristic_value_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_long_characteristic_value_rsp(const int32_t status)
{
    struct bt_host_gatt_read_long_characteristic_value_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_read_long_characteristic_value_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** Same as gatt_read_characteristic_value, but for long values. Indication structure.
 */
struct bt_host_gatt_read_long_characteristic_value_ind {
    uint16_t handle; /**< handle for the read characteristic */
    uint16_t size; /**< size of value in bytes */
    uint8_t value[0]; /**< value of read attribute */
};

static inline struct packet *
alloc_bt_host_gatt_read_long_characteristic_value_ind(struct bt_host_gatt_read_long_characteristic_value_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_long_characteristic_value_ind(const uint16_t handle, const uint16_t size, const uint8_t value[0])
{
    struct bt_host_gatt_read_long_characteristic_value_ind *ind;
    struct packet *pkt = alloc_bt_host_gatt_read_long_characteristic_value_ind(&ind);
    if (pkt) {
        ind->handle = handle;
        ind->size = size;
        /* value must be set by the caller */
    }
    return pkt;
}
/*
 * Message gatt_read_multiple_characteristic_values
 */

#define BT_HOST_GATT_READ_MULTIPLE_CHARACTERISTIC_VALUES_REQ 0x24
#define BT_HOST_GATT_READ_MULTIPLE_CHARACTERISTIC_VALUES_RSP 0xa4
#define BT_HOST_GATT_READ_MULTIPLE_CHARACTERISTIC_VALUES_IND 0xe4

/** This sub-procedure is used to read multiple Characteristic Values from a server when the client knows the Characteristic Value Handles. Request structure.
 */
struct bt_host_gatt_read_multiple_characteristic_values_req {
    uint16_t nof_handles; /**< number of handles to be read */
    uint8_t handles[0]; /**< handles to be read (two bytes per handle (lsb,msb)) */
};

static inline struct packet *
alloc_bt_host_gatt_read_multiple_characteristic_values_req(struct bt_host_gatt_read_multiple_characteristic_values_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_multiple_characteristic_values_req(const uint16_t nof_handles, const uint8_t handles[0])
{
    struct bt_host_gatt_read_multiple_characteristic_values_req *req;
    struct packet *pkt = alloc_bt_host_gatt_read_multiple_characteristic_values_req(&req);
    if (pkt) {
        req->nof_handles = nof_handles;
        /* handles must be set by the caller */
    }
    return pkt;
}

/** This sub-procedure is used to read multiple Characteristic Values from a server when the client knows the Characteristic Value Handles. Response structure.
 */
struct bt_host_gatt_read_multiple_characteristic_values_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_read_multiple_characteristic_values_rsp(struct bt_host_gatt_read_multiple_characteristic_values_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_multiple_characteristic_values_rsp(const int32_t status)
{
    struct bt_host_gatt_read_multiple_characteristic_values_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_read_multiple_characteristic_values_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** This sub-procedure is used to read multiple Characteristic Values from a server when the client knows the Characteristic Value Handles. Indication structure.
 */
struct bt_host_gatt_read_multiple_characteristic_values_ind {
    uint16_t handle; /**< handle for the read characteristic */
    uint16_t size; /**< size of value in bytes */
    uint8_t value[0]; /**< value of read attribute */
};

static inline struct packet *
alloc_bt_host_gatt_read_multiple_characteristic_values_ind(struct bt_host_gatt_read_multiple_characteristic_values_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_multiple_characteristic_values_ind(const uint16_t handle, const uint16_t size, const uint8_t value[0])
{
    struct bt_host_gatt_read_multiple_characteristic_values_ind *ind;
    struct packet *pkt = alloc_bt_host_gatt_read_multiple_characteristic_values_ind(&ind);
    if (pkt) {
        ind->handle = handle;
        ind->size = size;
        /* value must be set by the caller */
    }
    return pkt;
}
/*
 * Message gatt_read_characteristic_descriptor
 */

#define BT_HOST_GATT_READ_CHARACTERISTIC_DESCRIPTOR_REQ 0x25
#define BT_HOST_GATT_READ_CHARACTERISTIC_DESCRIPTOR_RSP 0xa5
#define BT_HOST_GATT_READ_CHARACTERISTIC_DESCRIPTOR_IND 0xe5

/** This sub-procedure is used to read a characteristic descriptor from a server when the client knows the
    characteristic descriptor declaration's Attribute handle. Request structure.
 */
struct bt_host_gatt_read_characteristic_descriptor_req {
    uint16_t handle; /**< handle for the characteristic descriptor */
};

static inline struct packet *
alloc_bt_host_gatt_read_characteristic_descriptor_req(struct bt_host_gatt_read_characteristic_descriptor_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_characteristic_descriptor_req(const uint16_t handle)
{
    struct bt_host_gatt_read_characteristic_descriptor_req *req;
    struct packet *pkt = alloc_bt_host_gatt_read_characteristic_descriptor_req(&req);
    if (pkt) {
        req->handle = handle;
    }
    return pkt;
}

/** This sub-procedure is used to read a characteristic descriptor from a server when the client knows the
    characteristic descriptor declaration's Attribute handle. Response structure.
 */
struct bt_host_gatt_read_characteristic_descriptor_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_read_characteristic_descriptor_rsp(struct bt_host_gatt_read_characteristic_descriptor_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_characteristic_descriptor_rsp(const int32_t status)
{
    struct bt_host_gatt_read_characteristic_descriptor_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_read_characteristic_descriptor_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** This sub-procedure is used to read a characteristic descriptor from a server when the client knows the
    characteristic descriptor declaration's Attribute handle. Indication structure.
 */
struct bt_host_gatt_read_characteristic_descriptor_ind {
    uint16_t handle; /**< handle for the read characteristic descriptor */
    uint16_t size; /**< size of value in bytes */
    uint8_t value[0]; /**< value of read characteristic descriptor */
};

static inline struct packet *
alloc_bt_host_gatt_read_characteristic_descriptor_ind(struct bt_host_gatt_read_characteristic_descriptor_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_characteristic_descriptor_ind(const uint16_t handle, const uint16_t size, const uint8_t value[0])
{
    struct bt_host_gatt_read_characteristic_descriptor_ind *ind;
    struct packet *pkt = alloc_bt_host_gatt_read_characteristic_descriptor_ind(&ind);
    if (pkt) {
        ind->handle = handle;
        ind->size = size;
        /* value must be set by the caller */
    }
    return pkt;
}
/*
 * Message gatt_read_long_characteristic_descriptor
 */

#define BT_HOST_GATT_READ_LONG_CHARACTERISTIC_DESCRIPTOR_REQ 0x26
#define BT_HOST_GATT_READ_LONG_CHARACTERISTIC_DESCRIPTOR_RSP 0xa6
#define BT_HOST_GATT_READ_LONG_CHARACTERISTIC_DESCRIPTOR_IND 0xe6

/** Same as gatt_read_characteristic_descriptor, but for long values. Request structure.
 */
struct bt_host_gatt_read_long_characteristic_descriptor_req {
    uint16_t handle; /**< handle for the characteristic descriptor */
};

static inline struct packet *
alloc_bt_host_gatt_read_long_characteristic_descriptor_req(struct bt_host_gatt_read_long_characteristic_descriptor_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_long_characteristic_descriptor_req(const uint16_t handle)
{
    struct bt_host_gatt_read_long_characteristic_descriptor_req *req;
    struct packet *pkt = alloc_bt_host_gatt_read_long_characteristic_descriptor_req(&req);
    if (pkt) {
        req->handle = handle;
    }
    return pkt;
}

/** Same as gatt_read_characteristic_descriptor, but for long values. Response structure.
 */
struct bt_host_gatt_read_long_characteristic_descriptor_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_read_long_characteristic_descriptor_rsp(struct bt_host_gatt_read_long_characteristic_descriptor_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_long_characteristic_descriptor_rsp(const int32_t status)
{
    struct bt_host_gatt_read_long_characteristic_descriptor_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_read_long_characteristic_descriptor_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** Same as gatt_read_characteristic_descriptor, but for long values. Indication structure.
 */
struct bt_host_gatt_read_long_characteristic_descriptor_ind {
    uint16_t handle; /**< handle for the read characteristic descriptor */
    uint16_t size; /**< size of value in bytes */
    uint8_t value[0]; /**< value of read characteristic descriptor */
};

static inline struct packet *
alloc_bt_host_gatt_read_long_characteristic_descriptor_ind(struct bt_host_gatt_read_long_characteristic_descriptor_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_read_long_characteristic_descriptor_ind(const uint16_t handle, const uint16_t size, const uint8_t value[0])
{
    struct bt_host_gatt_read_long_characteristic_descriptor_ind *ind;
    struct packet *pkt = alloc_bt_host_gatt_read_long_characteristic_descriptor_ind(&ind);
    if (pkt) {
        ind->handle = handle;
        ind->size = size;
        /* value must be set by the caller */
    }
    return pkt;
}
/*
 * Message gatt_write_without_response
 */

#define BT_HOST_GATT_WRITE_WITHOUT_RESPONSE_REQ 0x27
#define BT_HOST_GATT_WRITE_WITHOUT_RESPONSE_RSP 0xa7

/** This sub-procedure is used to write a Characteristic Value to a server when the client knows the Characteristic
    Value Handle and the client does not need an acknowledgment that the write was successfully performed. Request structure.
 */
struct bt_host_gatt_write_without_response_req {
    uint16_t value_handle; /**< value_handle to be written */
    uint8_t value[0]; /**< value to write */
};

static inline struct packet *
alloc_bt_host_gatt_write_without_response_req(struct bt_host_gatt_write_without_response_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_write_without_response_req(const uint16_t value_handle, const uint8_t value[0])
{
    struct bt_host_gatt_write_without_response_req *req;
    struct packet *pkt = alloc_bt_host_gatt_write_without_response_req(&req);
    if (pkt) {
        req->value_handle = value_handle;
        /* value must be set by the caller */
    }
    return pkt;
}

/** This sub-procedure is used to write a Characteristic Value to a server when the client knows the Characteristic
    Value Handle and the client does not need an acknowledgment that the write was successfully performed. Response structure.
 */
struct bt_host_gatt_write_without_response_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_write_without_response_rsp(struct bt_host_gatt_write_without_response_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_write_without_response_rsp(const int32_t status)
{
    struct bt_host_gatt_write_without_response_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_write_without_response_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gatt_signed_write_without_response
 */

#define BT_HOST_GATT_SIGNED_WRITE_WITHOUT_RESPONSE_REQ 0x28
#define BT_HOST_GATT_SIGNED_WRITE_WITHOUT_RESPONSE_RSP 0xa8

/** This sub-procedure is used to write a Characteristic Value to a server when the client knows the Characteristic
    Value Handle and the ATT bearer is not encrypted. Request structure.
 */
struct bt_host_gatt_signed_write_without_response_req {
    uint16_t value_handle; /**< value_handle to be written */
    uint8_t value[0]; /**< value to write */
};

static inline struct packet *
alloc_bt_host_gatt_signed_write_without_response_req(struct bt_host_gatt_signed_write_without_response_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_signed_write_without_response_req(const uint16_t value_handle, const uint8_t value[0])
{
    struct bt_host_gatt_signed_write_without_response_req *req;
    struct packet *pkt = alloc_bt_host_gatt_signed_write_without_response_req(&req);
    if (pkt) {
        req->value_handle = value_handle;
        /* value must be set by the caller */
    }
    return pkt;
}

/** This sub-procedure is used to write a Characteristic Value to a server when the client knows the Characteristic
    Value Handle and the ATT bearer is not encrypted. Response structure.
 */
struct bt_host_gatt_signed_write_without_response_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_signed_write_without_response_rsp(struct bt_host_gatt_signed_write_without_response_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_signed_write_without_response_rsp(const int32_t status)
{
    struct bt_host_gatt_signed_write_without_response_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_signed_write_without_response_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gatt_write_characteristic_value
 */

#define BT_HOST_GATT_WRITE_CHARACTERISTIC_VALUE_REQ 0x29
#define BT_HOST_GATT_WRITE_CHARACTERISTIC_VALUE_RSP 0xa9

/** This sub-procedure is used to write a Characteristic Value to a server when the client knows the Characteristic Value Handle. Request structure.
 */
struct bt_host_gatt_write_characteristic_value_req {
    uint16_t value_handle; /**< value_handle to be written */
    uint8_t value[0]; /**< value to write */
};

static inline struct packet *
alloc_bt_host_gatt_write_characteristic_value_req(struct bt_host_gatt_write_characteristic_value_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_write_characteristic_value_req(const uint16_t value_handle, const uint8_t value[0])
{
    struct bt_host_gatt_write_characteristic_value_req *req;
    struct packet *pkt = alloc_bt_host_gatt_write_characteristic_value_req(&req);
    if (pkt) {
        req->value_handle = value_handle;
        /* value must be set by the caller */
    }
    return pkt;
}

/** This sub-procedure is used to write a Characteristic Value to a server when the client knows the Characteristic Value Handle. Response structure.
 */
struct bt_host_gatt_write_characteristic_value_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_write_characteristic_value_rsp(struct bt_host_gatt_write_characteristic_value_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_write_characteristic_value_rsp(const int32_t status)
{
    struct bt_host_gatt_write_characteristic_value_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_write_characteristic_value_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gatt_write_characteristic_descriptor
 */

#define BT_HOST_GATT_WRITE_CHARACTERISTIC_DESCRIPTOR_REQ 0x2a
#define BT_HOST_GATT_WRITE_CHARACTERISTIC_DESCRIPTOR_RSP 0xaa

/** This sub-procedure is used to write a characteristic descriptor value to a server when the client knows the characteristic descriptor handle. Request structure.
 */
struct bt_host_gatt_write_characteristic_descriptor_req {
    uint16_t handle; /**< handle for the characteristic descriptor */
    uint8_t value[0]; /**< value to write */
};

static inline struct packet *
alloc_bt_host_gatt_write_characteristic_descriptor_req(struct bt_host_gatt_write_characteristic_descriptor_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_write_characteristic_descriptor_req(const uint16_t handle, const uint8_t value[0])
{
    struct bt_host_gatt_write_characteristic_descriptor_req *req;
    struct packet *pkt = alloc_bt_host_gatt_write_characteristic_descriptor_req(&req);
    if (pkt) {
        req->handle = handle;
        /* value must be set by the caller */
    }
    return pkt;
}

/** This sub-procedure is used to write a characteristic descriptor value to a server when the client knows the characteristic descriptor handle. Response structure.
 */
struct bt_host_gatt_write_characteristic_descriptor_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_write_characteristic_descriptor_rsp(struct bt_host_gatt_write_characteristic_descriptor_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_write_characteristic_descriptor_rsp(const int32_t status)
{
    struct bt_host_gatt_write_characteristic_descriptor_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_write_characteristic_descriptor_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gatt_notification
 */

#define BT_HOST_GATT_NOTIFICATION_REQ 0x2b
#define BT_HOST_GATT_NOTIFICATION_RSP 0xab

/** This sub-procedure is used when a server is configured to notify a Characteristic Value to a client without expecting any Attribute Protocol layer
    acknowledgment that the notification was successfully received. Request structure.
 */
struct bt_host_gatt_notification_req {
    uint8_t value; /**< value in notification */
};

static inline struct packet *
alloc_bt_host_gatt_notification_req(struct bt_host_gatt_notification_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_notification_req(const uint8_t value)
{
    struct bt_host_gatt_notification_req *req;
    struct packet *pkt = alloc_bt_host_gatt_notification_req(&req);
    if (pkt) {
        req->value = value;
    }
    return pkt;
}

/** This sub-procedure is used when a server is configured to notify a Characteristic Value to a client without expecting any Attribute Protocol layer
    acknowledgment that the notification was successfully received. Response structure.
 */
struct bt_host_gatt_notification_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_notification_rsp(struct bt_host_gatt_notification_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_notification_rsp(const int32_t status)
{
    struct bt_host_gatt_notification_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_notification_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gatt_indication
 */

#define BT_HOST_GATT_INDICATION_REQ 0x2c
#define BT_HOST_GATT_INDICATION_RSP 0xac

/** This sub-procedure is used when a server is configured to indicate a Characteristic Value to a client and expects an Attribute Protocol layer
    acknowledgment that the indication was successfully received. Request structure.
 */
struct bt_host_gatt_indication_req {
    uint8_t value; /**< value in indication */
};

static inline struct packet *
alloc_bt_host_gatt_indication_req(struct bt_host_gatt_indication_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_indication_req(const uint8_t value)
{
    struct bt_host_gatt_indication_req *req;
    struct packet *pkt = alloc_bt_host_gatt_indication_req(&req);
    if (pkt) {
        req->value = value;
    }
    return pkt;
}

/** This sub-procedure is used when a server is configured to indicate a Characteristic Value to a client and expects an Attribute Protocol layer
    acknowledgment that the indication was successfully received. Response structure.
 */
struct bt_host_gatt_indication_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_indication_rsp(struct bt_host_gatt_indication_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_indication_rsp(const int32_t status)
{
    struct bt_host_gatt_indication_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_indication_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message gatt_service_changed
 */

#define BT_HOST_GATT_SERVICE_CHANGED_REQ 0x2d
#define BT_HOST_GATT_SERVICE_CHANGED_RSP 0xad

/** This sub-procedure is used to trigger a service changed indication (the client needs to write the CCCD first). Request structure.
 */
struct bt_host_gatt_service_changed_req {
};

static inline struct packet *
alloc_bt_host_gatt_service_changed_req(struct bt_host_gatt_service_changed_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_service_changed_req(void)
{
    struct bt_host_gatt_service_changed_req *req;
    struct packet *pkt = alloc_bt_host_gatt_service_changed_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** This sub-procedure is used to trigger a service changed indication (the client needs to write the CCCD first). Response structure.
 */
struct bt_host_gatt_service_changed_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_gatt_service_changed_rsp(struct bt_host_gatt_service_changed_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_gatt_service_changed_rsp(const int32_t status)
{
    struct bt_host_gatt_service_changed_rsp *rsp;
    struct packet *pkt = alloc_bt_host_gatt_service_changed_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message bt_iperf
 */

#define BT_HOST_BT_IPERF_REQ 0x2e
#define BT_HOST_BT_IPERF_RSP 0xae
#define BT_HOST_BT_IPERF_IND 0xee

/** This sub-procedure is used to read a Characteristic Value from a server when the client only knows the characteristic UUID. Request structure.
 */
struct bt_host_bt_iperf_req {
    uint8_t handle; /**< connection handle */
    uint8_t mode; /**< 0=OFF, 1=RX, 2=TX, 3=RX and TX */
    uint16_t bandwidth; /**< transmitted bandwidth in kbits/s */
    uint16_t time; /**< time for test in seconds (0=unlimited) */
};

static inline struct packet *
alloc_bt_host_bt_iperf_req(struct bt_host_bt_iperf_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_bt_iperf_req(const uint8_t handle, const uint8_t mode, const uint16_t bandwidth, const uint16_t time)
{
    struct bt_host_bt_iperf_req *req;
    struct packet *pkt = alloc_bt_host_bt_iperf_req(&req);
    if (pkt) {
        req->handle = handle;
        req->mode = mode;
        req->bandwidth = bandwidth;
        req->time = time;
    }
    return pkt;
}

/** This sub-procedure is used to read a Characteristic Value from a server when the client only knows the characteristic UUID. Response structure.
 */
struct bt_host_bt_iperf_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_bt_iperf_rsp(struct bt_host_bt_iperf_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_bt_iperf_rsp(const int32_t status)
{
    struct bt_host_bt_iperf_rsp *rsp;
    struct packet *pkt = alloc_bt_host_bt_iperf_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** This sub-procedure is used to read a Characteristic Value from a server when the client only knows the characteristic UUID. Indication structure.
 */
struct bt_host_bt_iperf_ind {
    uint16_t bandwidth; /**< measured bandwidth in kbits/s (65535 means completed) */
    int8_t rssi; /**< rssi in dBm (127 means no value) */
};

static inline struct packet *
alloc_bt_host_bt_iperf_ind(struct bt_host_bt_iperf_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_bt_iperf_ind(const uint16_t bandwidth, const int8_t rssi)
{
    struct bt_host_bt_iperf_ind *ind;
    struct packet *pkt = alloc_bt_host_bt_iperf_ind(&ind);
    if (pkt) {
        ind->bandwidth = bandwidth;
        ind->rssi = rssi;
    }
    return pkt;
}
/*
 * Message smp_bond_list
 */

#define BT_HOST_SMP_BOND_LIST_REQ 0x2f
#define BT_HOST_SMP_BOND_LIST_RSP 0xaf

/** list bond at index in persistent memory. Request structure.
 */
struct bt_host_smp_bond_list_req {
    uint32_t index; /**< index in list (0..4) */
};

static inline struct packet *
alloc_bt_host_smp_bond_list_req(struct bt_host_smp_bond_list_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_smp_bond_list_req(const uint32_t index)
{
    struct bt_host_smp_bond_list_req *req;
    struct packet *pkt = alloc_bt_host_smp_bond_list_req(&req);
    if (pkt) {
        req->index = index;
    }
    return pkt;
}

/** list bond at index in persistent memory. Response structure.
 */
struct bt_host_smp_bond_list_rsp {
    int32_t status; /**< result status, zero is success */
    char name[64]; /**< friendly name or address */
};

static inline struct packet *
alloc_bt_host_smp_bond_list_rsp(struct bt_host_smp_bond_list_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_smp_bond_list_rsp(const int32_t status, const char name[64])
{
    struct bt_host_smp_bond_list_rsp *rsp;
    struct packet *pkt = alloc_bt_host_smp_bond_list_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        strncpy(rsp->name, name, 64);
    }
    return pkt;
}
/*
 * Message smp_bond_remove_one
 */

#define BT_HOST_SMP_BOND_REMOVE_ONE_REQ 0x30
#define BT_HOST_SMP_BOND_REMOVE_ONE_RSP 0xb0

/** remove bond at index in persistent memory. Request structure.
 */
struct bt_host_smp_bond_remove_one_req {
    uint32_t index; /**< index in list (0..4) */
};

static inline struct packet *
alloc_bt_host_smp_bond_remove_one_req(struct bt_host_smp_bond_remove_one_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_smp_bond_remove_one_req(const uint32_t index)
{
    struct bt_host_smp_bond_remove_one_req *req;
    struct packet *pkt = alloc_bt_host_smp_bond_remove_one_req(&req);
    if (pkt) {
        req->index = index;
    }
    return pkt;
}

/** remove bond at index in persistent memory. Response structure.
 */
struct bt_host_smp_bond_remove_one_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_smp_bond_remove_one_rsp(struct bt_host_smp_bond_remove_one_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_smp_bond_remove_one_rsp(const int32_t status)
{
    struct bt_host_smp_bond_remove_one_rsp *rsp;
    struct packet *pkt = alloc_bt_host_smp_bond_remove_one_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message smp_bond_remove_all
 */

#define BT_HOST_SMP_BOND_REMOVE_ALL_REQ 0x31
#define BT_HOST_SMP_BOND_REMOVE_ALL_RSP 0xb1

/** remove all bonds from persistent memory. Request structure.
 */
struct bt_host_smp_bond_remove_all_req {
};

static inline struct packet *
alloc_bt_host_smp_bond_remove_all_req(struct bt_host_smp_bond_remove_all_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_smp_bond_remove_all_req(void)
{
    struct bt_host_smp_bond_remove_all_req *req;
    struct packet *pkt = alloc_bt_host_smp_bond_remove_all_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** remove all bonds from persistent memory. Response structure.
 */
struct bt_host_smp_bond_remove_all_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_host_smp_bond_remove_all_rsp(struct bt_host_smp_bond_remove_all_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_host_smp_bond_remove_all_rsp(const int32_t status)
{
    struct bt_host_smp_bond_remove_all_rsp *rsp;
    struct packet *pkt = alloc_bt_host_smp_bond_remove_all_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/* [[[end]]] */
/** @} */
