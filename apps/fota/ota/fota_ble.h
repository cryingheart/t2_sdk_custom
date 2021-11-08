#include <bt/cmn.h>
#include <bt/bt_gap.h>
#include <bt/bt_gatt.h>
#include <bt/bt_att.h>

#include "fota.h"

/* Structure containing server information */
typedef struct
{
	struct gatt_service *cust_service;		// Pointer to our custom service
	struct gatt_srv_link *gatt_link;		// Pointer to link with client
} custom_server_t;

/* BLE callback functions */
void connected_cb(bt_hci_event_t *hci_event);
void disconnected_cb(bt_hci_event_t *hci_event);
bt_att_error_t ota_ssid_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data);
bt_att_error_t ota_pass_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data);
bt_att_error_t ota_flag_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data);
bt_att_error_t ota_srv1_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data);
bt_att_error_t ota_srv2_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data);
bt_att_error_t ota_srv3_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data);
bt_att_error_t ota_srvport_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data);
bt_att_error_t ota_name_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data);
bt_att_error_t ota_ver_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data);
bt_att_error_t ota_fname_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data);

/* Server operation functions */
void custom_server_create(void);
void custom_server_destroy(void);
void start_server(struct ota_config* ota_config);

int get_provisioningstatus();