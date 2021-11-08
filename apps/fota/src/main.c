#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/os.h>
#include <kernel/page_alloc.h>
#include <kernel/spi.h>
#include <kernel/spi-mem.h>
#include <kernel/hwreg.h>
#include <kernel/elf.h>
#include <kernel/boot.h>
#include <kernel/uart.h>
#include <kernel/secureboot.h>
#include <kernel/packet.h>
#include <kernel/uart.h>
#include <kernel/flash.h>
#include <block/mtd.h>
#include <sys/mount.h>
#include <dirent.h>
#include <wifi/wcm.h>
#include <http/inc/http_client.h>

#include "json/jansson/src/jansson.h"
#include "utils/inc/fs_utils.h"
#include "utils/inc/utils.h"
#include "utils/inc/sector_cache.h"
#include "fota/inc/fota.h"


#define APP_NAME        "FOTA application"
#define APP_VERSION     "1.0"

OS_APPINFO {.stack_size = 8192}; 

struct os_semaphore wcm_lock;
int wcm_connected = 0;

void
fota_app_wcm_notifier(void *ctx, struct os_msg *msg)
{
    switch(msg->msg_type) {
    case WCM_NOTIFY_MSG_ADDRESS:
        os_sem_post(&wcm_lock);
        wcm_connected = 1;
        break;
    case WCM_NOTIFY_MSG_LINK_DOWN:
        wcm_connected = 0;
        break;
    default:
        break;
    }
}

int main()
{
    fota_handle_t *f_handle;
    int update_available = 0;    
    struct wcm_handle *wcm_handle;
    int rval;
    
    /* Get Boot args*/
    const char *ssid = os_get_boot_arg_str("ssid");
    const char *passphrase = os_get_boot_arg_str("passphrase") ?: NULL;

    print_app_info(APP_NAME, APP_VERSION);

    /*Verify boot args*/
    if (ssid == NULL || passphrase == NULL) {
        os_printf("\nUsage : <ssid> <passphrase>");
        return 0;
    }
        
    /*Mount root FS*/
    utils_mount_rootfs();

    /*Connect to WiFi N/w*/
    wcm_handle = wcm_create(NULL);
    os_sem_init(&wcm_lock, 0);
    wcm_notify_enable(wcm_handle, fota_app_wcm_notifier, NULL);    
    /* Connect to network */
    os_printf("Connecting to ...%s - %s", ssid, passphrase);
    rval = wcm_add_network(wcm_handle, ssid, NULL, passphrase);
    if(rval < 0) {
        os_printf("Error: wcm_add_network = %d\n", rval);
        return 0;
    }

    rval = wcm_auto_connect(wcm_handle, true);
    if(rval < 0) {
        pr_err("Error: wcm_auto_connect = %d\n", rval);
        return rval;
    }

    /*Wait for the N/w connection to go through*/
    os_sem_wait(&wcm_lock);

    /*Check the N/w connection status*/
    if(!wcm_connected){
        os_printf("\nError: wcm connection failed");
        goto error_exit;
    }
    os_printf("\nN/w Connection done..");

    /*Initialise FOTA */
    f_handle = fota_init();
    if(!f_handle){
        os_printf("\nError: Initialising fota");
        goto error_exit;
    }

    if(FOTA_ERROR_NONE != fota_update_check(f_handle, &update_available)){
        os_printf("\nError: FOTA update check failed");
        goto error_exit;
    }

    if(update_available){
        os_printf("\nPerform Fota");
        if(FOTA_ERROR_NONE != fota_perform(f_handle, 0, 0)){
            os_printf("\nError: FOTA perform failed");
            goto error_exit;

        }
        /*fota commit.  This will reset the system*/
        if(FOTA_ERROR_NONE != fota_commit(f_handle, 1)){
            os_printf("\nError: FOTA perform failed");
            goto error_exit;
        }
    }else{
        os_printf("\nNo new update available");
    }

error_exit:
    /*[TODO]. As per the requirement need to impliment recovery mechanism
     *Like,
     * 1 - Modify the boot boot.json and reset device
     */ 
    os_printf("\nExit");
    while(1);
}
