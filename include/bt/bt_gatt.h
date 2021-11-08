/**
 * @addtogroup ble_gatt
 * @{
 */
#pragma once
#include <stdint.h>
#include <kernel/packet.h>
#include <bt/bt_uuid.h>
#include <bt/bt_att.h>

/**
 * List of errors returned from Bluetooth GATT functions
 */
typedef enum {
    /** Operation succeeded with no error */
    BT_GATT_ERROR_SUCCESS     = 0x00,
    /** Operation could not be performed due to the client being busy */
    BT_GATT_ERROR_CLIENT_BUSY = 0x01,
} bt_gatt_error_t;

/**
 * GATT characteristic properties
 */
#define GATT_CHAR_PROP_BIT_BROADCAST    (1<<0)
#define GATT_CHAR_PROP_BIT_READ         (1<<1)
#define GATT_CHAR_PROP_BIT_WRITE_NO_RSP (1<<2)
#define GATT_CHAR_PROP_BIT_WRITE        (1<<3)
#define GATT_CHAR_PROP_BIT_NOTIFY       (1<<4)
#define GATT_CHAR_PROP_BIT_INDICATE     (1<<5)
#define GATT_CHAR_PROP_BIT_WRITE_SIGNED (1<<6)
#define GATT_CHAR_PROP_BIT_EXT_PROP     (1<<7)
#define GATT_CHAR_PROP_R    (GATT_CHAR_PROP_BIT_READ)
#define GATT_CHAR_PROP_W    (GATT_CHAR_PROP_BIT_WRITE)
#define GATT_CHAR_PROP_WN   (GATT_CHAR_PROP_BIT_WRITE_NO_RSP)
#define GATT_CHAR_PROP_WNS  (GATT_CHAR_PROP_WN | GATT_CHAR_PROP_BIT_WRITE_SIGNED)
#define GATT_CHAR_PROP_WS   (GATT_CHAR_PROP_W  | GATT_CHAR_PROP_BIT_WRITE_SIGNED)
#define GATT_CHAR_PROP_RW   (GATT_CHAR_PROP_R | GATT_CHAR_PROP_W)
#define GATT_CHAR_PROP_RWN  (GATT_CHAR_PROP_R | GATT_CHAR_PROP_WN)
#define GATT_CHAR_PROP_RWNS (GATT_CHAR_PROP_R | GATT_CHAR_PROP_WNS)
#define GATT_CHAR_PROP_RWS  (GATT_CHAR_PROP_R | GATT_CHAR_PROP_WS)
#define GATT_CHAR_PROP_N    (GATT_CHAR_PROP_BIT_NOTIFY)
#define GATT_CHAR_PROP_I    (GATT_CHAR_PROP_BIT_INDICATE)

/**
 * GATT attribute permissions
 */
#define GATT_PERM_READ              (1<<0)
#define GATT_PERM_WRITE             (1<<1)
#define GATT_PERM_ENCRYPTION        (1<<2)
#define GATT_PERM_AUTHENTICATION    (1<<3)
#define GATT_PERM_AUTHORIZATION     (1<<4)
#define GATT_PERM_ENC_KEY_SIZE_128  (1<<5)
#define GATT_PERM_SIGNED            (1<<7)
#define GATT_PERM_SIGNED_MITM       (1<<8)
#define GATT_PERM_RW  (GATT_PERM_READ | GATT_PERM_WRITE)
#define GATT_PERM_REA (GATT_PERM_READ | GATT_PERM_AUTHENTICATION | GATT_PERM_ENCRYPTION)
#define GATT_PERM_RAC (GATT_PERM_READ | GATT_PERM_AUTHENTICATION)
#define GATT_PERM_RAZ (GATT_PERM_READ | GATT_PERM_AUTHORIZATION)
#define GATT_PERM_REK (GATT_PERM_READ | GATT_PERM_ENCRYPTION | GATT_PERM_ENC_KEY_SIZE_128)
#define GATT_PERM_WAC (GATT_PERM_WRITE | GATT_PERM_AUTHENTICATION)
#define GATT_PERM_WAZ (GATT_PERM_WRITE | GATT_PERM_AUTHORIZATION)
#define GATT_PERM_WEK (GATT_PERM_WRITE | GATT_PERM_ENCRYPTION | GATT_PERM_ENC_KEY_SIZE_128)
#define GATT_PERM_WSI (GATT_PERM_WRITE | GATT_PERM_SIGNED)
#define GATT_PERM_RWAC (GATT_PERM_RW | GATT_PERM_AUTHENTICATION)
#define GATT_PERM_RWAZ (GATT_PERM_RW | GATT_PERM_AUTHORIZATION)
#define GATT_PERM_RWEK (GATT_PERM_RW | GATT_PERM_ENCRYPTION | GATT_PERM_ENC_KEY_SIZE_128)
#define GATT_PERM_RWSI (GATT_PERM_RW | GATT_PERM_SIGNED)

/**
 * GATT client characteristic configuration
 */
#define GATT_CCCD_NOTIFICATION_BIT  (1<<0)
#define GATT_CCCD_INDICATION_BIT    (1<<1)

/**
 * Read or write function
 */
typedef enum {
    BT_GATT_FCN_READ  = 0,
    BT_GATT_FCN_WRITE = 1,
} bt_gatt_fcn_t;

typedef bt_att_error_t (*bt_srv_fcn_t)(uint8_t bearer, bt_uuid_t *uuid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data);
typedef void (*bt_srv_cfm_cb_t)(uint8_t bearer);
/* GATT client callbacks */
/** General callback function. */
typedef void (*cb_t)(uint8_t bearer);
/** Callback for bt_gatt_discover_all_primary_services(). */
typedef void (*daps_cb_t)(uint8_t bearer, bt_att_error_t error, uint16_t attribute_handle, uint16_t end_group_handle, bt_uuid_t uuid);
/** Callback for bt_gatt_discover_primary_service_by_service_uuid(). */
typedef void (*dpsbsu_cb_t)(uint8_t bearer, bt_att_error_t error, uint16_t attribute_handle, uint16_t end_group_handle);
/** Callback for bt_gatt_find_included_services(). */
typedef void (*fis_cb_t)(uint8_t bearer, bt_att_error_t error, uint16_t attribute_handle, uint16_t isa_handle, uint16_t end_group_handle, bt_uuid_t uuid);
/** Callback for bt_gatt_discover_characteristics_by_uuid/_all_characteristics_of_a_service(). */
typedef void (*dacoas_cb_t)(uint8_t bearer, bt_att_error_t error, uint16_t handle, uint8_t properties, uint16_t value_handle, bt_uuid_t uuid);
/** Callback for bt_gatt_discover_all_characteristic_descriptors(). */
typedef void (*dacd_cb_t)(uint8_t bearer, bt_att_error_t error, uint16_t handle, bt_uuid_t uuid);
/** Callback for bt_gatt_read_characteristic_value/_using_characteristic_uuid/_multiple_characteristic_values(). */
typedef void (*rcv_cb_t)(uint8_t bearer, bt_att_error_t error, uint16_t handle, uint16_t size, uint8_t *data);
/** Callback for bt_gatt_write(). */
typedef void (*wcv_cb_t)(uint8_t bearer, bt_att_error_t error, uint16_t handle);
/** Callback for bt_gatt_subscribe(). */
typedef void (*n_cb_t)(uint8_t bearer, uint16_t handle, uint8_t length, uint8_t *data);

/**
 * @struct gatt_service
 * Opaque object representing a Service declaration
 */
struct gatt_service;

/**
 * @struct gatt_incl
 * Opaque object representing an Include declaration
 */
struct gatt_incl;

/**
 * @struct gatt_char
 * Opaque object representing a Characteristic declaration
 */
struct gatt_char;

/**
 * @struct gatt_desc
 * Opaque object representing a Descriptor declaration
 */
struct gatt_desc;

/**
 * @struct gatt_srv_link
 * Opaque object representing a GATT server link
 */
struct gatt_srv_link;

/**
 * @struct gatt_cli_link
 * Opaque object representing a GATT client link
 */
struct gatt_cli_link;

/* Server interface */
struct gatt_service * bt_gatt_create_service_16(uint16_t uuid16);
struct gatt_service * bt_gatt_create_service_32(uint32_t uuid32);
struct gatt_service * bt_gatt_create_service_128(uint128_t uuid128);
void bt_gatt_add_service(struct gatt_service *s);
struct gatt_service * bt_gatt_remove_service(struct gatt_service *s);
void bt_gatt_destroy_service(struct gatt_service *s);
struct gatt_incl * bt_gatt_add_incl_16(struct gatt_service *s, uint16_t uuid16, uint16_t isa_handle, uint16_t eg_handle);
struct gatt_incl * bt_gatt_add_incl_32(struct gatt_service *s, uint32_t uuid32, uint16_t isa_handle, uint16_t eg_handle);
struct gatt_incl *  bt_gatt_add_incl_128(struct gatt_service *s, uint128_t uuid128, uint16_t isa_handle, uint16_t eg_handle);
struct gatt_char * bt_gatt_add_char_16(struct gatt_service *s, uint16_t uuid16, bt_srv_fcn_t fcn, uint8_t permission, uint8_t property);
struct gatt_char * bt_gatt_add_char_32(struct gatt_service *s, uint32_t uuid32, bt_srv_fcn_t fcn, uint8_t permission, uint8_t property);
struct gatt_char * bt_gatt_add_char_128(struct gatt_service *s, uint128_t uuid128, bt_srv_fcn_t fcn, uint8_t permission, uint8_t property);
struct gatt_desc * bt_gatt_add_desc_16(struct gatt_char *c, uint16_t uuid16, bt_srv_fcn_t fcn, uint8_t permission, uint8_t property);
struct gatt_desc * bt_gatt_add_desc_32(struct gatt_char *c, uint32_t uuid32, bt_srv_fcn_t fcn, uint8_t permission, uint8_t property);
struct gatt_desc * bt_gatt_add_desc_128(struct gatt_char *c, uint128_t uuid128, bt_srv_fcn_t fcn, uint8_t permission, uint8_t property);
void bt_gatt_notification(struct gatt_srv_link *link, struct gatt_char *chr, uint8_t length, uint8_t *data);
bt_att_error_t bt_gatt_indication(struct gatt_srv_link *link, struct gatt_char *chr, uint8_t length, uint8_t *data, bt_srv_cfm_cb_t cfm_cb);

/* Client interface */
bt_gatt_error_t bt_gatt_exchange_mtu(struct gatt_cli_link *link, uint16_t size, cb_t cb);
bt_gatt_error_t bt_gatt_discover_all_primary_services(struct gatt_cli_link *link, daps_cb_t cb);
bt_gatt_error_t bt_gatt_discover_primary_service_by_service_uuid(struct gatt_cli_link *link, bt_uuid_t uuid, dpsbsu_cb_t cb);
bt_gatt_error_t bt_gatt_find_included_services(struct gatt_cli_link *link, uint16_t start_handle,
                                               uint16_t end_handle, fis_cb_t cb);
bt_gatt_error_t bt_gatt_discover_all_characteristics_of_a_service(struct gatt_cli_link *link, uint16_t start_handle,
                                                                  uint16_t end_handle, dacoas_cb_t cb);
bt_gatt_error_t bt_gatt_discover_characteristics_by_uuid(struct gatt_cli_link *link, uint16_t start_handle,
                                                         uint16_t end_handle, bt_uuid_t uuid, dacoas_cb_t cb);
bt_gatt_error_t bt_gatt_discover_all_characteristic_descriptors(struct gatt_cli_link *link, uint16_t start_handle,
                                                                uint16_t end_handle, dacd_cb_t cb);
bt_gatt_error_t bt_gatt_read_characteristic_value(struct gatt_cli_link *link, uint16_t handle, uint8_t *data, rcv_cb_t cb);
bt_gatt_error_t bt_gatt_read_using_characteristic_uuid(struct gatt_cli_link *link, uint16_t start_handle, uint16_t end_handle,
                                                       bt_uuid_t uuid,  uint8_t *data, rcv_cb_t cb);
bt_gatt_error_t bt_gatt_read_long_characteristic_value(struct gatt_cli_link *link, uint16_t handle, uint8_t *data, uint16_t offset, rcv_cb_t cb);
bt_gatt_error_t bt_gatt_read_multiple_characteristic_values(struct gatt_cli_link *link, uint8_t nof_handles, uint16_t *handles,
                                                            uint8_t *data, rcv_cb_t cb);

#if 0
bt_gatt_error_t bt_gatt_write_without_response(struct gatt_cli_link *link, uint16_t handle, uint8_t size, uint8_t *data);
bt_gatt_error_t bt_gatt_signed_write_without_response(struct gatt_cli_link *link, uint16_t handle, uint8_t size, uint8_t *data);
bt_gatt_error_t bt_gatt_write_characteristic_value(struct gatt_cli_link *link, uint16_t handle, uint8_t size, uint8_t *data, wcv_cb_t cb);
#else
/* One common interface for write_without_response, signed_write_without_response, write_characteristic_value, ... */
bt_gatt_error_t bt_gatt_write(struct gatt_cli_link *link, uint16_t handle, uint8_t size, uint8_t *data, uint8_t signed_write, wcv_cb_t cb);
#endif

/* subscribe for notifications/indications */
bt_gatt_error_t bt_gatt_subscribe(struct gatt_cli_link *link, uint16_t size, uint8_t *data, n_cb_t cb);

#if 0
void bt_gatt_write_long_characteristic_value()
void bt_gatt_reliable_write()
#endif

#define bt_gatt_read_characteristic_descriptor bt_gatt_read_characteristic_value
#define bt_gatt_read_long_characteristic_descriptor bt_gatt_read_long_characteristic_value
#define bt_gatt_write_characteristic_descriptor bt_gatt_write

#if 0
void bt_gatt_write_long_characteristic_descriptor()
#endif

uint16_t bt_gatt_client_rx_mtu_size_get(struct gatt_srv_link *link);

/** @} */
