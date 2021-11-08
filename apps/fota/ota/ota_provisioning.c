/**
  ******************************************************************************
  * @file   ota_provisioning.c
  *
  * @brief  sample application of provisiong ota_config over ble
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
#include <kernel/spi-mem.h>
#include <kernel/boot.h>
#include <kernel/io.h>
#include <kernel/hwreg.h>
#include <kernel/watchdog.h>
#include <block/mtd.h>
#include <sys/mount.h>
#include <dirent.h>
#include <wifi/wcm.h>

#include <wifi/wcm.h>
#include <bt/cmn.h>
#include <bt/bt_gap.h>
#include <bt/bt_gatt.h>
#include <bt/bt_att.h>

#include "json.h"
#include "config.h"
#include "ota.h"

#define SECTOR_SIZE        0x1000
#define PAGE_SIZE   256
#define PARTITIONSIZE 32768
#define MOUNT_NAME "/root"
#define MOUNT_ADDR 0x8000

#define OTA_IMG_SLOT    4
#define OTA_STANDALONE  0
#define OTA_BACKUP      1  
#define OTA_UPGRADE     2
#define OTA_LATEST     3

OS_APPINFO {.stack_size=6000};

void trigger_ota_update();

int do_mount(struct spi_mem_device * device, off_t start_block, off_t nblocks, const char *mountpt);
void init_spi(void);

int parse_boot_file(const char *filename);
int parse_ota_file(const char *filename);
void modify_boot_file(uint8_t image_num);
void modify_ota_file(struct ota_config config);

void reset_device();

static struct spi_bus        bus;
static struct spi_mem_device spi_mem;
/* mount filesystem */
static bool   smartfs_mounted;
struct mtd_dev_s *mtd_raw, *mtd_p1;

struct ota_config ota_config;

struct mtd_dev_s *mtd_raw;

char *boot_arg = (char*)0x80000;
struct boot_args boot_args;

struct config config;

FILE *flash_fp;
DIR  *flash_dp;

/* Structure containing server information */
typedef struct
{
	struct gatt_service *cust_service;		// Pointer to our custom service
	struct gatt_srv_link *gatt_link;		// Pointer to link with client
} custom_server_t;


/* BLE callback functions */
static void connected_cb(bt_hci_event_t *hci_event);
static void disconnected_cb(bt_hci_event_t *hci_event);
static bt_att_error_t ota_ssid_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data);
static bt_att_error_t ota_pass_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data);
static bt_att_error_t ota_flag_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data);
static bt_att_error_t ota_srv_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data);
static bt_att_error_t ota_name_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data);
static bt_att_error_t ota_ver_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data);
static bt_att_error_t ota_fname_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data);
static bt_att_error_t ota_slot_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data);

/* Server operation functions */
static void custom_server_create(void);
static void start_server(void);
static void custom_server_destroy(void);

/* BLE address of this server */
static const bt_address_t SERVER_ADDR = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

/* UUID of our custom service */
static const uint128_t UUID_CUSTOM_SERVICE = {
		{0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11}};

/* UUIDs of our custom characteristics */
#define UUID_OTA_SSID_16		0xAB34
#define UUID_OTA_PASS_16		0xCD34
#define UUID_OTA_FLAG_16		0x0134
#define UUID_OTA_SRV_16		    0x0234
#define UUID_OTA_NAME_16		0x0334
#define UUID_OTA_VER_16		    0x0434
#define UUID_OTA_FNAME_16		0x0534
#define UUID_OTA_SLOT_16		0x0634

/* GAP option object to be passed to GAP functions */
static const gap_ops_t gap_ops = {
    .connected_cb    = connected_cb,
    .disconnected_cb = disconnected_cb,
    .discovery_cb    = NULL,
};

/* Our server information structure */
static custom_server_t srv = {NULL, NULL};

/* Some flags to determine if ota has been providioned */
static uint8_t ota_provisioned = 0;


static char* ssid;
static char* passphrase;

/* Callback called when the client connects */
static void connected_cb(bt_hci_event_t *hci_event)
{
    const bt_hci_evt_le_conn_cmpl_t *param = (bt_hci_evt_le_conn_cmpl_t*)&hci_event->parameter;

	os_printf("Client connected\n");

	// Add link for the connection
	srv.gatt_link = bt_gap_server_link_add(param->handle);
}


/* Callback called when the client disconnects */
static void disconnected_cb(bt_hci_event_t *hci_event)
{
	os_printf("Client disconnected\n");

	if(srv.gatt_link)
	{
		// Remove link for the connection
		bt_gap_server_link_remove(srv.gatt_link);
		srv.gatt_link = NULL;
	}

	// Make server connectable again (will re-enable advertisement)
	bt_gap_connectable_mode(GAP_CONNECTABLE_MODE_UNDIRECT, bt_hci_addr_type_random, 0, address_zero, &gap_ops);
}

static bt_att_error_t ota_ssid_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data)
{
	if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(ota_config.ssid, 0, 32);
    int ssid_len = *length;
    if(ssid_len > 32)
        ssid_len = 31;

    memcpy(ota_config.ssid, data, ssid_len);

    return BT_ATT_ERROR_SUCCESS;
}
static bt_att_error_t ota_pass_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(ota_config.passphrase, 0, 32);
    int pass_len = *length;
    if(pass_len > 32)
        pass_len = 31;

    memcpy(ota_config.passphrase , data, pass_len);

    return BT_ATT_ERROR_SUCCESS;
}
static bt_att_error_t ota_flag_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    ota_config.ota_flag = *data;
    os_printf("provisioned ota_flag val: %d\n", ota_config.ota_flag);
    ota_provisioned = 1;
    return BT_ATT_ERROR_SUCCESS;
}
static bt_att_error_t ota_srv_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(ota_config.server, 0, 32);
    int srv_len = *length;
    if(srv_len > 32)
        srv_len = 31;

    memcpy(ota_config.server , data, srv_len);
    return BT_ATT_ERROR_SUCCESS;
}
static bt_att_error_t ota_name_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(ota_config.name, 0, 32);
    int name_len = *length;
    if(name_len > 32)
        name_len = 31;

    memcpy(ota_config.name , data, name_len);
    return BT_ATT_ERROR_SUCCESS;
}
static bt_att_error_t ota_ver_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(ota_config.version, 0, 32);
    int ver_len = *length;
    if(ver_len > 32)
        ver_len = 31;

    memcpy(ota_config.version , data, ver_len);
    return BT_ATT_ERROR_SUCCESS;
}
static bt_att_error_t ota_fname_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    memset(ota_config.filename, 0, 32);
    int fname_len = *length;
    if(fname_len > 32)
        fname_len = 31;

    memcpy(ota_config.filename , data, fname_len);
    return BT_ATT_ERROR_SUCCESS;
}
static bt_att_error_t ota_slot_prov_cb(uint8_t bearer, bt_gatt_fcn_t rw, uint8_t *length, uint8_t offset, uint8_t *data)
{
    if(offset != 0)
        return BT_ATT_ERROR_INVALID_OFFSET;

    ota_config.slot = *data;
    return BT_ATT_ERROR_SUCCESS;
}


/* Creates a "custom server" i.e. adds our custom service and characteristic */
static void custom_server_create(void)
{
	srv.cust_service = bt_gatt_create_service_128(UUID_CUSTOM_SERVICE);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_SSID_16, ota_ssid_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_PASS_16, ota_pass_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_FLAG_16, ota_flag_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_SRV_16, ota_srv_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_NAME_16, ota_name_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_VER_16, ota_ver_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_FNAME_16, ota_fname_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_OTA_SLOT_16, ota_slot_prov_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);
	bt_gatt_add_service(srv.cust_service);
}

static void custom_server_destroy(void)
{
    bt_gap_connectable_mode(GAP_CONNECTABLE_MODE_DISABLE, 0, 0, address_zero, NULL);
    bt_gatt_destroy_service(bt_gatt_remove_service(srv.cust_service));
    bt_gap_server_link_remove(srv.gatt_link);
}

/* Starts our server */
static void start_server(void)
{
	// Configure advertisement
	bt_gap_cfg_adv(10240, 0, 160, 1600, 0);

	// Set our BLE address
	bt_gap_addr_set(bt_hci_addr_type_random, SERVER_ADDR);

	// Make server connectable (will enable advertisement)
	bt_gap_connectable_mode(GAP_CONNECTABLE_MODE_UNDIRECT, bt_hci_addr_type_random, addr_type_zero, address_zero, &gap_ops);
}


int main(void)
{
    os_printf("T2 OTA Provisioning App\n");
    init_spi();
	int image_num = parse_boot_file("/root/boot.json");

    ssid = os_alloc(32);
    passphrase = os_alloc(32);

	// parse ota file
	parse_ota_file("/root/fota.json");
    os_printf("ota_flag=%d\n", ota_config.ota_flag);

	bt_gap_init();

	// Common server implements the following services that will be added to our server:
	// Generic access
	// Generic attribute
	// Device information
	common_server_create("InnoOTA", 0, "Innophase Inc");

	// Custom server implements the following services that will be added to our server:
	// Custom service
	custom_server_create();

	start_server();

    while(!ota_provisioned)
	    os_msleep(1000);

    // housekeeping
	custom_server_destroy();
	common_server_destroy();
	bt_gap_destroy();
	os_wait_usec(100000);

    
    os_printf(" OTA update triggered!");
    trigger_ota_update();

	while(true)
		os_msleep(1000);

	return 0;
}

void trigger_ota_update()
{
    modify_ota_file(ota_config);
    int app_partition = 1;
    modify_boot_file(app_partition);
}

int do_mount(struct spi_mem_device * device, off_t start_block, off_t nblocks, const char *mountpt)
{
    if (smartfs_mounted)
        return 0;

    mtd_raw = mtd_spi_open_mem(device);
    if(mtd_raw == NULL) {
        os_printf("mtd_spi_open failed\n");
        return -1;
    }

    mtd_p1 = mtd_partition(mtd_raw, start_block, nblocks);
    if(mtd_p1 == NULL) {
        os_printf("mtd_partition failed\n");
        return -1;
    }

    int retval = smart_initialize(0, mtd_p1, "test");

    if(retval != 0) {
        os_printf("smart_initialize = %d\n", retval);
        return -1;
    }

    retval = file_mount("/dev/smart0test", mountpt, "smartfs", 0, NULL);

    if(retval != 0) {
        os_printf("mount = %d\n", retval);
        return -1;
    }

    smartfs_mounted = true;
    return 0;
}

void init_spi(void)
{

    spi_bus_init(&bus);
    spi_mem_init(&spi_mem, &bus, 0);
    spi_set_freq(&bus, 40000000);
   
    do_mount(&spi_mem, MOUNT_ADDR>>8, PARTITIONSIZE>>8, "/root");
}

FILE *flash_fp;

static void
flash_file_open(const char* path, uint8_t mode)
{
    if (mode == 0)
        flash_fp = fopen(path, "r");
    else
        flash_fp = fopen(path, "w");
}

static void
flash_file_close()
{
    fclose(flash_fp);
}

static void
flash_file_read(uint32_t length)
{
    char *file_data = os_alloc(length+1);
    memset(file_data, 0 , length+1);

    uint32_t actual = fread(file_data, 1, length, flash_fp);

    os_printf("Read: %s\n", file_data);
}

static void
flash_file_write(uint32_t length, uint8_t* data)
{
    uint32_t __unused sz_out = fwrite(data, 1, length, flash_fp);
}

static void
flash_file_remove(const char* path)
{
    file_unlink(path);
}

void add_boot_arg(char* arg, size_t len)
{
    *--boot_arg = 0;
    boot_arg -= len;
    boot_args.argc++;
    memcpy(boot_arg, arg, len);
}

int
parse_boot_file(const char *filename)
{
    FILE *f;
    struct json_parser json;
    int ch;
    void *dest=NULL;

    f = fopen(filename, "r");
    if(f == NULL)
        return 1;

    json_init(&json);

    while(1) {
        ch = fgetc(f);
        volatile __auto_type t = json_tokenizer(&json, ch);

        switch (t) {
        case JSON_END:
            json_finish(&json);
            fclose(f);
            return -1;
            break;

        case JSON_LITERAL:
            dest = NULL;

            if (strcmp(json.token, "image") == 0)
            {
                dest = (void*)1;
            }
            break;

        case JSON_NUMBER:
            if (dest) {
                fclose(f);
                return atoi(json.token);
            }
            break;

        default:
            break;
        }
    }

    return 0;
}

int parse_ota_file(const char *filename)
{
    FILE *f;
    struct json_parser json;
    int ch;
    void *dest=NULL;

    f = fopen(filename, "r");
    if(f == NULL)
        return 1;

    json_init(&json);

    while(1) {
        ch = fgetc(f);
        volatile __auto_type t = json_tokenizer(&json, ch);
        switch (t) {
        case JSON_END:
            json_finish(&json);
            fclose(f);
            return -1;
            break;

        case JSON_LITERAL:
            dest = NULL;
            if (strcmp(json.token, "ssid") == 0)
			{
                dest = &ota_config.ssid;
			}
            if (strcmp(json.token, "passphrase") == 0)
			{
                dest = &ota_config.passphrase;
			}
            if (strcmp(json.token, "ota_flag") == 0)
			{
                dest = &ota_config.ota_flag;
			}
			if (strcmp(json.token, "server") == 0)
			{
                dest = &ota_config.server;
			}
			if (strcmp(json.token, "name") == 0)
			{
                dest = &ota_config.name;
			}
			if (strcmp(json.token, "version") == 0)
			{
                dest = &ota_config.version;
			}
            if (strcmp(json.token, "filename") == 0)
			{
                dest = &ota_config.filename;
			}
            if (strcmp(json.token, "slot") == 0)
			{
                dest = &ota_config.slot;
			}
            break;
        case JSON_NUMBER:
            if (dest)
                *(int*)dest = atoi(json.token);
            break;
        case JSON_STRING:
            if (dest)
            {
                strcpy(dest, json.token);
            }
            break;
        default:
            break;
        }
        
    }
    return 0;
}

void modify_boot_file(uint8_t image_num)
{
    char boot_json[128];
    memset(boot_json, 0, 128);
    int length = snprintf(boot_json, 128,
"{\n\
    image : %d\n\
}\n", image_num);

    // Open file for writing
    flash_file_open("/root/boot.json", 1);
    // Write to file
    flash_file_write(length, boot_json);
    // Close file
    flash_file_close();
}

void modify_ota_file(struct ota_config config)
{
    char ota_json[512];
    memset(ota_json, 0, 512);
    int length = snprintf(ota_json, 512,
"{\n\
    ssid        : \"%s\",\n\
    passphrase  : \"%s\",\n\
    ota_flag    : %d,\n\
    server      : \"%s\",\n\
    name        : \"%s\",\n\
    version     : \"%s\",\n\
    filename    : \"%s\",\n\
    slot        : %d,\n\
}\n", ota_config.ssid, ota_config.passphrase, ota_config.ota_flag, ota_config.server, ota_config.name, ota_config.version, ota_config.filename, ota_config.slot);

    // Open file for writing
    flash_file_open("/root/fota.json", 1);
    // Write to file
    flash_file_write(length, ota_json);
    // Close file
    flash_file_close();

}

void reset_device()
{
    uint8_t trip;
    uint32_t value;
    mmio_wr8(1, &hw_wdg->unlock);
    mmio_wr8(1, &hw_wdg->run);
    mmio_wr8_set(PMU_RST_MASK_POR , &hw_pmu->rst_mask[1]);
    mmio_wr8_clr(PMU_RST_MASK_POR , &hw_pmu->rst_mask[2]);
    trip |= WDG_TRIP_RESET;
    mmio_wr8(trip, &hw_wdg->trip);
    mmio_wr16(0, &hw_wdg->preload);
}
