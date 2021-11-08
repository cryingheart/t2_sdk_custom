/**
  ******************************************************************************
  * @file   fota_ble.c
  *
  * @brief  ble function for fota
  *
  ******************************************************************************
  * @attention
  *
  *
  *  Copyright (c) 2021, InnoPhase, Inc.
  *
  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  *  POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


#include <stdio.h>
#include <string.h>
#include <kernel/os.h>

#include "fota_ble.h"
#include "fota_utils.h"
#include "fs_utils.h"

/* Some flags to determine if ota has been provisioned */
uint8_t ota_provisioned = 0;

static struct ota_config* fota_config;

/* UUID of our custom service */
static const uint128_t UUID_CUSTOM_SERVICE = {
		{0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11}};

/* UUIDs of our custom characteristics */
#define UUID_OTA_SSID_16		0xAB31
#define UUID_OTA_PASS_16		0xAB32
#define UUID_OTA_FLAG_16		0x0131
#define UUID_OTA_SRV1_16		0x0231
#define UUID_OTA_SRV2_16		0x0232
#define UUID_OTA_SRV3_16		0x0233
#define UUID_OTA_SRVPORT_16		0x0239
#define UUID_OTA_NAME_16		0x0331
#define UUID_OTA_VER_16		    0x0332
#define UUID_OTA_FNAME_16		0x0431
#define UUID_OTA_FSIZE_16		0x0432
#define UUID_OTA_USERNAME_16	0x0531
#define UUID_OTA_USERPASS_16	0x0532

#define CON_ADDRESS_TYPE bt_hci_addr_type_public

/* GAP option object to be passed to GAP functions */
static const gap_ops_t gap_ops = {
    .connected_cb    = connected_cb,
    .disconnected_cb = disconnected_cb,
    .discovery_cb    = NULL,
};

/* Our server information structure */
static custom_server_t srv = {NULL, NULL};

/* Callback called when the client connects */
void connected_cb(bt_hci_event_t *hci_event)
{
    const bt_hci_evt_le_conn_cmpl_t *param = (bt_hci_evt_le_conn_cmpl_t*)&hci_event->parameter;

	os_printf("Client connected\n");

	// Add link for the connection
	srv.gatt_link = bt_gap_server_link_add(param->handle);
}


/* Callback called when the client disconnects */
void disconnected_cb(bt_hci_event_t *hci_event)
{
	os_printf("Client disconnected\n");

	if(srv.gatt_link)
	{
		// Remove link for the connection
		bt_gap_server_link_remove(srv.gatt_link);
		srv.gatt_link = NULL;
	}

	// Make server connectable again (will re-enable advertisement)
	//bt_gap_connectable_mode(GAP_CONNECTABLE_MODE_UNDIRECT, bt_hci_addr_type_random, 0, address_zero, &gap_ops);
    bt_gap_connectable_mode(GAP_CONNECTABLE_MODE_UNDIRECT, CON_ADDRESS_TYPE, 0, address_zero, &gap_ops);
}

bt_att_error_t ota_ssid_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
	if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(fota_config->ssid, 0, 32);
    int len = *length;
    if(len > 32)
        len = 32;

    memcpy(fota_config->ssid, data, len);

    return BT_ATT_ERROR_SUCCESS;
}
bt_att_error_t ota_pass_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(fota_config->passphrase, 0, 32);
    int len = *length;
    if(len > 32)
        len = 32;

    memcpy(fota_config->passphrase , data, len);

    return BT_ATT_ERROR_SUCCESS;
}
bt_att_error_t ota_flag_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    fota_config->ota_flag = *data;
    os_printf("provisioned ota_flag val: %d\n", fota_config->ota_flag);
    ota_provisioned = 1;
    return BT_ATT_ERROR_SUCCESS;
}
bt_att_error_t ota_srv1_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    int len = *length;

    memcpy(fota_config->server, data, len);
    return BT_ATT_ERROR_SUCCESS;
}
bt_att_error_t ota_srv2_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    int len = *length;

    memcpy((fota_config->server)+20, data, len);
    return BT_ATT_ERROR_SUCCESS;
}
bt_att_error_t ota_srv3_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    int len = *length;

    memcpy((fota_config->server)+40, data, len);
    return BT_ATT_ERROR_SUCCESS;
}
bt_att_error_t ota_srvport_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(fota_config->server_port, 0, 6);
    int len = *length;
    if(len > 6)
        len = 6;

    memcpy(fota_config->server_port, data, len);
    return BT_ATT_ERROR_SUCCESS;
}
bt_att_error_t ota_name_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(fota_config->name, 0, 32);
    int len = *length;
    if(len > 32)
        len = 32;

    memcpy(fota_config->name , data, len);
    return BT_ATT_ERROR_SUCCESS;
}
bt_att_error_t ota_ver_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(fota_config->version, 0, 32);
    int len = *length;
    if(len > 32)
        len = 32;

    memcpy(fota_config->version , data, len);
    return BT_ATT_ERROR_SUCCESS;
}
bt_att_error_t ota_fname_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(fota_config->filename, 0, 32);
    int len = *length;
    if(len > 32)
        len = 32;

    memcpy(fota_config->filename , data, len);
    return BT_ATT_ERROR_SUCCESS;
}
bt_att_error_t ota_fsize_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    if(*length == 4)
    {
        fota_config->filesize = (*(data) & 0xFF) + (*(data+1)<<8 & 0xFF00) + (*(data+2)<<16 & 0xFF0000) + (*(data+3)<<24  & 0xFF000000);
    }
    else
    {
        fota_config->filesize = 0;
    }
    
    os_printf("Filesize: %d\r\n", fota_config->filesize);
    return BT_ATT_ERROR_SUCCESS;
}
bt_att_error_t ota_username_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(fota_config->username, 0, 32);
    int len = *length;
    if(len > 32)
        len = 32;

    memcpy(fota_config->username, data, len);
    return BT_ATT_ERROR_SUCCESS;
}
bt_att_error_t ota_userpass_prov_cb(uint8_t bearer, bt_uuid_t *uudid, bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(fota_config->password, 0, 32);
    int len = *length;
    if(len > 32)
        len = 32;

    memcpy(fota_config->password , data, len);
    return BT_ATT_ERROR_SUCCESS;
}


/* Creates a "custom server" i.e. adds our custom service and characteristic */
void custom_server_create(void)
{
	srv.cust_service = bt_gatt_create_service_128(UUID_CUSTOM_SERVICE);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_SSID_16, (void*)ota_ssid_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_PASS_16, (void*)ota_pass_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_FLAG_16, (void*)ota_flag_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_SRV1_16, (void*)ota_srv1_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_SRV2_16, (void*)ota_srv2_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_SRV3_16, (void*)ota_srv3_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_SRVPORT_16, (void*)ota_srvport_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_NAME_16, (void*)ota_name_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_VER_16,(void*) ota_ver_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_FNAME_16, (void*)ota_fname_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_FSIZE_16, (void*)ota_fsize_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_USERNAME_16, (void*)ota_username_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_USERPASS_16, (void*)ota_userpass_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
	bt_gatt_add_service(srv.cust_service);
}

void custom_server_destroy(void)
{
    bt_gap_connectable_mode(GAP_CONNECTABLE_MODE_DISABLE, 0, 0, address_zero, NULL);
    bt_gatt_destroy_service(bt_gatt_remove_service(srv.cust_service));
    bt_gap_server_link_remove(srv.gatt_link);
}

/* Starts our server */
void start_server(struct ota_config* ota_config)
{
    fota_config = ota_config;
    bt_gap_cfg_adv_t bt_adv_handle;

    // Configure advertisement
    //bt_gap_cfg_adv(10240, 0, 160, 1600, 0, 0);

    bt_adv_handle.fast_period = 10240;
    bt_adv_handle.slow_period = 0;
    bt_adv_handle.fast_interval = 160;
    //bt_adv_handle.fast_interval_nonconn = 0;  //not documented
    bt_adv_handle.slow_interval = 480;
    bt_adv_handle.tx_power = BT_HCI_LE_TX_POWER_NO_PREFERENCE;
    bt_adv_handle.channel_map = BT_HCI_ADV_CHANNEL_ALL;

    bt_gap_cfg_adv_set(&bt_adv_handle);
    //bt_gap_cfg_adv(10240, 0, 160, 480, BT_HCI_LE_TX_POWER_NO_PREFERENCE, BT_HCI_ADV_CHANNEL_ALL);

	// Set our BLE address
	//bt_gap_addr_set(bt_hci_addr_type_random, SERVER_ADDR);
    //bt_gap_addr_set(CON_ADDRESS_TYPE, SERVER_ADDR);

	// Make server connectable (will enable advertisement)
	//bt_gap_connectable_mode(GAP_CONNECTABLE_MODE_UNDIRECT, bt_hci_addr_type_random, addr_type_zero, address_zero, &gap_ops);
    bt_gap_connectable_mode(GAP_CONNECTABLE_MODE_UNDIRECT, CON_ADDRESS_TYPE, 0, address_zero, &gap_ops);
}

int get_provisioningstatus()
{
    return ota_provisioned;
}
