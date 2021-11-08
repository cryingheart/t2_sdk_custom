/**
  ******************************************************************************
  * @file   fota.c
  *
  * @brief  example fota application
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
#include <stdint.h>
#include <kernel/os.h>
#include <kernel/spi-mem.h>
#include <kernel/flash.h>
#include <kernel/boot.h>
#include <kernel/callout.h>
#include <kernel/io.h>
#include <kernel/hwreg.h>
#include <kernel/watchdog.h>
#include <sha2.h>
#include <wifi/wcm.h>
#include <block/mtd.h>
#include <sys/mount.h>

//#include "config.h"
#include "fota.h"
#include "fota_ble.h"
#include "fota_utils.h"
#include "fs_utils.h"
#include "part.h"
#include "sector_cache.h"

#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#define FS_ADDR	0x1c0000
#define FS_SIZE	0x40000

#define OTA_PROVISIONING  0
#define OTA_UPGRADE     2

#define MAX_RETRIES     5

OS_APPINFO {.stack_size=10000};

static void reset_device();

struct ota_config *ota_config;
struct ota_config fota_config;
uint32_t debugen_save;

//struct config config;
struct part_config part_config;

static bool connect_tls(const char * server_name, const char * server_port);
static void close_tls(void);

static void my_wcm_notify_cb(void *ctx, struct os_msg *msg);

char* ssid;
char* passphrase;

static mbedtls_net_context server_fd;
static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;
static mbedtls_ssl_context ssl;
static mbedtls_ssl_config conf;
static mbedtls_x509_crt cacert;

static struct wcm_handle *h;
volatile bool ap_connected = false;
bool match = false;

static const char* root_ca_pem;

#define BUF_SIZE 2048
#define REQUEST_BLOCK_SIZE 4096
static char data_buff[REQUEST_BLOCK_SIZE];
static char byte_range_buff[30];

SHA256_CTX sha256_ctx;

void boot_sha256_init(SHA256_CTX *ctx);
void boot_sha256_update(SHA256_CTX *ctx, const void *buf, size_t len);
void boot_sha256_transform(SHA256_CTX *ctx);
void boot_sha256_final(SHA256_CTX *ctx, uint8_t *digest);

static void __irq
handle_crash_event()
{
    //handle crash here
    reset_device();
}

int read_last_boot()
{
	// Open file for reading
	flash_file_open("/root/lastboot", 0);

	// read file
	char data[2];
	flash_file_read(2, data);

    // Close file
    flash_file_close();
	return atoi(data);
}

int
main(void)
{
    os_set_crash_handler(handle_crash_event);
    int rc;
    ota_config = &fota_config;
    os_printf("***T2 FOTA Sample Application***\n");

    os_printf("Mounting file system\n");
    rc = mount_ufs();

    if(rc !=0)
    {
        os_printf("Failed to mount filesystem\n");
        goto fota_error;
    }
    bootpass();
    // parse part file
    memset(&part_config, 0, sizeof(struct part_config));
    rc = read_part_file("/root/part.json", &part_config);

    if(rc !=0)
    {
        os_printf("failed to read part.json\n");
        goto fota_error;
    }

    for(int i = 0; i < part_config.image_count; i++)
    {
        os_printf("Image name: %s v%s\n", part_config.image_config[i].name, part_config.image_config[i].version);
    }
    
	// parse OTA file for flag
	parse_ota_file("/root/fota.json", ota_config);
    os_printf("ota_flag=%d\n", ota_config->ota_flag);
    if(rc !=0)
    {
        os_printf("failed to read fota.json\n");
        ota_config->ota_flag = OTA_PROVISIONING;
    }

	if(ota_config->ota_flag == OTA_PROVISIONING)
	{
        // Stand alone OTA mode. Can be used to recover/download any image from CDN by configuring via BLE
        os_printf("FOTA PROVISIONING Mode\n");
        
        // Start BLE
        bt_gap_init();

        // Common server implements the following services that will be added to our server:
        // Generic access
        // Generic attribute
        // Device information
        common_server_create("InnoOTA", 0, "Innophase Inc");

        // Custom server implements the following services that will be added to our server:
        // Custom service
        custom_server_create();

        start_server(ota_config);
        bootpass();

        os_printf("Please connect via Innophase BLE app!\n");
        // wait for device to be provisioned
        while(!get_provisioningstatus())
            os_msleep(1000);

        // Trigger OTA upgrade process
        ota_config->ota_flag=OTA_UPGRADE;
        modify_ota_file(ota_config);
	}
	else if (ota_config->ota_flag == OTA_UPGRADE)
	{
        /***ToDo: Perform backup of current image and FS***/
        uint32_t total = 0;
        uint8_t tries = 0;
        uint32_t img_sector = 0;
        uint32_t img_sector_count;
        uint32_t image_size = 0;
        uint32_t img_addr = 0;

        os_printf("OTA_UPGRADE Mode\n");
        img_sector_count = ota_config->filesize/0x1000;
        if(ota_config->filesize%0x1000)
            img_sector_count+=1;
        if(avail_sectors(&part_config) < img_sector_count)
        {
            os_printf("Error: Not enough free sectors in Flash!\n");
            goto fota_error;
        }
        img_sector = next_free_sector(&part_config);

        struct spi_mem_device *spi_mem = os_flash_get_spi_dev();
		// Upgrade user application as configured in fota.json

        // init sha context
        boot_sha256_init(&sha256_ctx);

		// connect to AP
        h = wcm_create(NULL);
        wcm_notify_enable(h, my_wcm_notify_cb, NULL);
        os_printf("connecting to AP: %s\n", ota_config->ssid);

        wcm_add_network(h, ota_config->ssid, NULL, ota_config->passphrase);
        wcm_auto_connect(h, true);

        while(!ap_connected)
        {
            os_msleep(1000);
        }
        bootpass();
        os_printf("\n!!!Updating firmware. Do not powerdown device!!!!\n");

        // Connect to CDN
        if(!(connect_tls(ota_config->server, ota_config->server_port)))
        {
            goto fota_error;
        }
        memset(data_buff, 0, REQUEST_BLOCK_SIZE);

        // Do a HEAD request to determine size of file
        char connection_header[30];
        memset(connection_header, 0, 30);
        sprintf(connection_header, "Connection: keep-alive\r\n");
        int len = sprintf(data_buff, "HEAD %s HTTP/1.0\r\nHost: %s\r\n%s\r\n", ota_config->filename, ota_config->server, connection_header);
        os_printf(data_buff);
        while ((rc = mbedtls_ssl_write(&ssl, (unsigned char*)data_buff, len)) <= 0) {
            if (rc != MBEDTLS_ERR_SSL_WANT_READ && rc != MBEDTLS_ERR_SSL_WANT_WRITE) 
            {
                os_printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", rc);
                goto fota_error;
            }
        }
        // Read & Parse the HTTP Response
        rc = mbedtls_ssl_read(&ssl, (unsigned char*)data_buff, REQUEST_BLOCK_SIZE);
        os_printf("RSP:\n%s\n", data_buff);
        char *ret;
        int i = 0;
        int l = strlen("Content-Length: ");
        ret = strstr(data_buff, "Content-Length: ");
        if(ret != NULL)
        {
            while(*(ret+l+i) != '\n')
            {
                i++;
            }
        }
        else
        {
            goto fota_error;
        }
        char length_string[16];
        memset(length_string, 0, 16);
        memcpy(length_string, ret+l, i);
        image_size = atoi(length_string); 
        ota_config->filesize = image_size;

        memset(data_buff, 0, REQUEST_BLOCK_SIZE);
        int request_count = image_size/REQUEST_BLOCK_SIZE;
        if(image_size%REQUEST_BLOCK_SIZE)
        {  
            request_count++;
        }
        int range_start;
        int range_end;

        // Download and Flash image file
        img_addr = img_sector*0x1000;
        sector_cache_init();
        os_printf("Downloading %s v%s to sector %d\n", ota_config->name, ota_config->version, img_sector);
        for(int i = 0; i < request_count; i++)
        {
            //os_printf("Request: %d\n", i);
            memset(data_buff, 0, REQUEST_BLOCK_SIZE);
            memset(byte_range_buff, 0, 30);
            range_start = (i*REQUEST_BLOCK_SIZE);
            range_end = range_start + REQUEST_BLOCK_SIZE-1;
            if(range_end > (image_size-1))
            {
                range_end = image_size;
                sprintf(connection_header, "Connection: close\r\n");
            }
            
            sprintf(byte_range_buff, "bytes=%d-%d", range_start, range_end);
            int len = 0;
            tries = 0;
            int bytes_read = 0;
            while(tries < MAX_RETRIES)
            {
                len = snprintf(data_buff, REQUEST_BLOCK_SIZE, "GET %s HTTP/1.0\r\nRange: %s\r\nHost: %s\r\n%s\r\n", ota_config->filename, byte_range_buff, ota_config->server, connection_header);
                os_printf("\nREQ: \n%s\n", data_buff);
                while ((rc = mbedtls_ssl_write(&ssl, (unsigned char*)data_buff, len)) <= 0) {
                    if (rc != MBEDTLS_ERR_SSL_WANT_READ && rc != MBEDTLS_ERR_SSL_WANT_WRITE) 
                    {
                        os_printf(" failed\n  ! mbedtls_ssl_write returned 0x%x\n\n", rc);
                        goto retry_request;
                    }
                    else
                    {
                        break;
                    }
                }

                // Read & Parse the HTTP Response
                memset(data_buff, 0, REQUEST_BLOCK_SIZE);
                rc = mbedtls_ssl_read(&ssl, (unsigned char*)data_buff, REQUEST_BLOCK_SIZE);
                os_printf("RSP:\n%s\n", data_buff);

                // Read the data
                bytes_read = 0;
                memset(data_buff, 0, REQUEST_BLOCK_SIZE);
                do {
                    rc = mbedtls_ssl_read(&ssl, (unsigned char*)data_buff+bytes_read, REQUEST_BLOCK_SIZE);
                    //os_printf("rc: %d\n", rc);
                    if (rc == MBEDTLS_ERR_SSL_WANT_READ || rc == MBEDTLS_ERR_SSL_WANT_WRITE)
                    {
                        continue;
                    }
                    if (rc == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
                    {
                        os_printf("MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY\n");
                        break;
                    }
                    if (rc < 0) {
                        bytes_read = 0;
                        os_printf("failed\n  ! mbedtls_ssl_read returned 0x%x\n\n", rc);
                        goto retry_request;
                    }
                    if(rc >0)
                    {
                        bytes_read += rc;
                    }
                    if(bytes_read == REQUEST_BLOCK_SIZE)
                    {
                        break;
                    }
                    else if(bytes_read >= REQUEST_BLOCK_SIZE)
                    {
                        os_printf("failed\n  ! mbedtls_ssl_read returned %d\n", rc);
                        rc = -1;
                        break; 
                    }
                }while(1);
                break;

                retry_request:
                tries++;
                    os_printf("retrying...\n");
                    close_tls();
                    if(!(connect_tls(ota_config->server, ota_config->server_port)))
                    {
                        goto fota_error;
                    }
            }
            if(bytes_read > 0)
            {
                total += bytes_read;
                boot_sha256_update(&sha256_ctx, data_buff, bytes_read);
                sector_cache_write(spi_mem, img_addr+(i*REQUEST_BLOCK_SIZE), data_buff, bytes_read);
            }
            else if(rc == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
            {
                //os_printf("tot: %d\n", total);
                break;
            }
            else
            {
                os_printf("Download Failed. Aborting..\n");
                break;
            }
        }
        uint8_t hash[32];

        boot_sha256_final(&sha256_ctx, hash);

        os_printf("img hash: ");
        for(int i = 0; i < 32; i++)
        {
            os_printf("%x", hash[i]);
        }
        os_printf("\n");

        uint8_t fota_hash[32];
        const char *pos = ota_config->hash;
        //convert to hash
        for (size_t count = 0; count < 32; count++) {
            sscanf(pos, "%2hhx", &fota_hash[count]);
            pos += 2;
        }
        os_printf("fota hash: ");
        for(int i = 0; i < 32; i++)
        {
            os_printf("%x", fota_hash[i]);
        }
        os_printf("\n");

        if(total > 0)
        {
            // Image Download Completed
            os_printf("\nDownloaded: %d bytes\n", total);

            // Close connection
            close_tls();
            wcm_auto_connect(h, 0);
            while(ap_connected)
            {
                os_msleep(1000);
            }
            wcm_destroy(h);

            // Validate image
            os_printf("Validating Image\n");
            match = true;
            for(int i = 0; i <32; i++)
            {
                if(hash[i] != fota_hash[i])
                {
                    match = false;
                }
            }
            if (match)
            {
                os_printf("sha256 validation success!\n");
                sector_cache_flush_all(spi_mem);
            }
            else
            {
                os_printf("sha256 validation failed!\n");
                //  invalidate contents of Flash
                spi_sector_erase(spi_mem, img_addr);
                goto fota_error;
            }
            // Read and update partition file
            read_part_file("/root/part.json", &part_config);
            part_config.image_count += 1;
            int app_partition =  part_config.image_count;
            memset(part_config.image_config[app_partition-1].name, 0, 32);
            memcpy(part_config.image_config[app_partition-1].name, ota_config->name, strlen(ota_config->name));
            memset(part_config.image_config[app_partition-1].version, 0, 16);
            memcpy(part_config.image_config[app_partition-1].version, ota_config->version, strlen(ota_config->version));
            part_config.image_config[app_partition-1].sector = img_sector;
            part_config.image_config[app_partition-1].size = ota_config->filesize;
            part_config.image_config[app_partition-1].sector_count = img_sector_count;

            //ToDo: separate args
            part_config.image_config[app_partition-1].args[0] = ota_config->args;
            modify_part_file("/root/part.json", &part_config);

            read_part_file("/root/part.json", &part_config);
            for(int i = 0; i < part_config.image_count; i++)
            {
                os_printf("Image name: %s v%s\n", part_config.image_config[i].name, part_config.image_config[i].version);
            }

            // Clear OTA file
            ota_config->ota_flag=OTA_PROVISIONING;
            os_printf("clearing ota file\n");
            struct ota_config blank_config;
            memset(&blank_config, 0, sizeof(struct ota_config));
            modify_ota_file(&blank_config);
            
            os_printf("updating boot file...\n");
            modify_boot_file(app_partition-1);
            
            goto done;
        }

	}
    done:
    reset_device();
	return 0;
    fota_error:
        os_printf("***FOTA ERROR***\n");
        os_msleep(5000);
        reset_device();
    return 0;
}

static void
reset_device()
{
    uint8_t trip = 0;
    mmio_wr8(1, &hw_wdg->unlock);
    mmio_wr8(1, &hw_wdg->run);
    mmio_wr8_set(PMU_RST_MASK_POR , &hw_pmu->rst_mask[1]);
    mmio_wr8_clr(PMU_RST_MASK_POR , &hw_pmu->rst_mask[2]);
    trip |= WDG_TRIP_RESET;
    mmio_wr8(trip, &hw_wdg->trip);
    mmio_wr16(0, &hw_wdg->preload);
}

static bool __irq
connect_tls(const char * server_name, const char * server_port)
{
    int ret = 1;
    uint32_t flags;
    const char *pers = "ssl_client1";
    bool ok = false;

	//read root_ca_pem from file system
	root_ca_pem = os_alloc(1800);
	memset(root_ca_pem, 0, 1800);
	if(flash_file_open("/root/root_ca_pem", 0) ==0)
	{
		flash_file_read(1800, root_ca_pem);
		flash_file_close();
	}
	int root_ca_pem_size = strlen(root_ca_pem)+1;

    /*
     * 0. Initialize the RNG and the session data
     */
    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    os_printf("Seeding the random number generator...\n");

    mbedtls_entropy_init(&entropy);
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                               (const unsigned char *) pers,
                               strlen(pers))) != 0) {
        pr_err(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
        goto exit_connect_tls;
    }

    os_printf(" ok\n");

    /*
     * 0. Initialize certificates
     */
    os_printf("  . Loading the CA root certificate ...\n");

    ret = mbedtls_x509_crt_parse(&cacert, (const unsigned char *) root_ca_pem,
                          root_ca_pem_size);
    if (ret < 0) {
        pr_err(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        goto exit_connect_tls;
    }

    os_printf(" ok (%d skipped)\n", ret);

    /*
     * 1. Start the connection
     */
    os_printf("  . Connecting to tcp %s:%s...", server_name, server_port);

    if ((ret = mbedtls_net_connect(&server_fd, server_name,
                                         server_port, MBEDTLS_NET_PROTO_TCP)) != 0) {
        pr_err(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
        goto exit_connect_tls;
    }

    os_printf(" ok\n");

    /*
     * 2. Setup stuff
     */
    os_printf("  . Setting up the SSL/TLS structure...");

    if ((ret = mbedtls_ssl_config_defaults(&conf,
                    MBEDTLS_SSL_IS_CLIENT,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        pr_err(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto exit_connect_tls;
    }

    os_printf(" ok\n");

    /* OPTIONAL is not optimal for security,
     * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_conf_max_frag_len(&conf, MBEDTLS_SSL_MAX_FRAG_LEN_2048);

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
        pr_err(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        goto exit_connect_tls;
    }

    if ((ret = mbedtls_ssl_set_hostname(&ssl, server_name)) != 0) {
        pr_err(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
        goto exit_connect_tls;
    }

    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    /*
     * 4. Handshake
     */
    os_printf("  . Performing the SSL/TLS handshake...");

    while((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            pr_err(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
            goto exit_connect_tls;
        }
    }
    os_printf(" ok\n");

    /*
     * 5. Verify the server certificate
     */
    os_printf("  . Verifying peer X.509 certificate...");

    /* In real life, we probably want to bail out when ret != 0 */
    if ((flags = mbedtls_ssl_get_verify_result(&ssl)) != 0) {
        char * vrfy_buf = os_alloc(512);
        if (vrfy_buf != NULL) {
            pr_err(" failed\n");
            mbedtls_x509_crt_verify_info(vrfy_buf, 512, "  ! ", flags);
            pr_err("%s\n", vrfy_buf);
        }
        os_free(vrfy_buf);
    }
    else
        os_printf(" ok\n");

    ok = true;
exit_connect_tls:
    return ok;
}

static void
close_tls(void)
{
    mbedtls_ssl_close_notify(&ssl);

    mbedtls_net_free(&server_fd);
    mbedtls_x509_crt_free(&cacert);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}

static void my_wcm_notify_cb(void *ctx, struct os_msg *msg)
{
    switch(msg->msg_type)
    {

        case(WCM_NOTIFY_MSG_LINK_UP):
            os_printf("wcm_notify_cb to App Layer - WCM_NOTIFY_MSG_LINK_UP\n");
            // set most recently used AP and updated priority in list
            break;

        case(WCM_NOTIFY_MSG_LINK_DOWN):
            os_printf("wcm_notify_cb to App Layer - WCM_NOTIFY_MSG_LINK_DOWN\n");
            break;

        case(WCM_NOTIFY_MSG_ADDRESS):
            os_printf("wcm_notify_cb to App Layer - WCM_NOTIFY_MSG_ADDRESS\n");
            ap_connected = true;
            break;

        case(WCM_NOTIFY_MSG_DISCONNECT_DONE):
            os_printf("wcm_notify_cb to App Layer - WCM_NOTIFY_MSG_DISCONNECT_DONE\n");
            ap_connected = false;
            break;

        default:
            break;
    }
    os_msg_release(msg);
}
