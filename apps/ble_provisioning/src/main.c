/**
  *****************************************************************************
  * @file   main.c
  *
  * @brief   The sample application creates a BLE GATT profile
  *          and service which is then used to provision a connection  
  *          to a Wi-Fi network. An android application running a 
  *          custom android application is used to input the Wi-Fi credentials. 
  *****************************************************************************
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
  *****************************************************************************
  */
#include <inttypes.h>
#include <string.h>
#include <kernel/console.h>
#include <bt/cmn.h>
#include <bt/bt_gap.h>
#include <bt/bt_gatt.h>
#include <bt/bt_att.h>

#include "wifi_control.h"

OS_APPINFO {.stack_size=4096};


/* Structure containing server information */
typedef struct
{
    /* Pointer to our custom service */
    struct gatt_service *cust_service;
    /* Pointer to link with client */    
    struct gatt_srv_link *gatt_link;        
} custom_server_t;


/* BLE callback functions */
static void connected_cb(bt_hci_event_t *hci_event);
static void disconnected_cb(bt_hci_event_t *hci_event);
/* Server operation functions */
static void custom_server_create(void);
static void start_server(void);

/* BLE address of this server */
static const bt_address_t SERVER_ADDR = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

/* UUID of our custom service */
static const uint128_t UUID_CUSTOM_SERVICE = {
                {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11}};

/* UUIDs of our custom characteristics */
#define UUID_WIFI_SSID_16        0xAB34
#define UUID_WIFI_PASSCODE_16    0xCD34
#define UUID_WIFI_STATUS_16      0xEF34

/* GAP option object to be passed to GAP functions */
static const gap_ops_t gap_ops = {
    .connected_cb    = connected_cb,
    .disconnected_cb = disconnected_cb,
    .discovery_cb    = NULL,
};

/* Our server information structure */
static custom_server_t srv = {NULL, NULL};

/* Some flags to determine if ssid and passcode has been providioned */
static uint8_t ssid_provisioned = 0;
static uint8_t pass_provisioned = 0;
/* final status, for mssgs other than "waiting" and "timeout" */
static uint8_t status_sent = 0;
/* just for timeout cases */
static uint8_t timeout_sent = 0;

static char ssid[32];
static char pw[32];
static char status[16];
static int wcm_return = 0;
static int waiting_status_count = 0;
/* true when we get IP */
bool status_flag; 
bool link_down_timeout;


#define STATUS_SUCCESS "success"
#define STATUS_SUCCESS_LENGTH 7

#define STATUS_WAITING "waiting"
#define STATUS_WAITING_LENGTH 7

#define STATUS_FAILURE "failure"
#define STATUS_FAILURE_LENGTH 7

#define STATUS_TIMEOUT "timeout"
#define STATUS_TIMEOUT_LENGTH 7

#define MY_APP_THREAD_PRIO 5 /* thread priority */
#define MY_APP_THREAD_STACK_SIZE 1024 /* thread stack size */
//#define CYCLE_TIME 60  /*seconds*/

/* the thread function */
static struct os_thread *my_app_thread;  

static void my_app_thread_func(void* arg)
{
    os_printf("starting app_thread\n");
    while(1)
    {
        os_msleep(1000);
    }
    return;
}


static void my_app_init()
{
    my_app_thread = os_create_thread("my_app_thread", 
                    (void *) my_app_thread_func,
                    NULL, MY_APP_THREAD_PRIO, MY_APP_THREAD_STACK_SIZE);
    if( my_app_thread == NULL ){
        os_printf(" thread creation failed\n");
        return;
    }
    os_join_thread(my_app_thread);
}


/* Callback called when the client connects */
static void connected_cb(bt_hci_event_t *hci_event)
{
    const bt_hci_evt_le_conn_cmpl_t *param = 
      (bt_hci_evt_le_conn_cmpl_t*)&hci_event->parameter;

    os_printf("Client connected\n");

    /* Add link for the connection */
    srv.gatt_link = bt_gap_server_link_add(param->handle);
}


/* Callback called when the client disconnects */
static void disconnected_cb(bt_hci_event_t *hci_event)
{
    os_printf("Client disconnected\n");

    if(srv.gatt_link)
    {
        /* Remove link for the connection */
        bt_gap_server_link_remove(srv.gatt_link);
        srv.gatt_link = NULL;
    }

    if(!ssid_provisioned || !pass_provisioned)
    {
        /* Make server connectable again (will re-enable advertisement) */
        bt_gap_connectable_mode(GAP_CONNECTABLE_MODE_UNDIRECT, 
        bt_hci_addr_type_random, 0, address_zero, &gap_ops);
    }
}


/* Callback called when our custom characteristic is accessed */
static bt_att_error_t ssid_provision_cb(uint8_t bearer, bt_uuid_t *uudid, 
   bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
    return BT_ATT_ERROR_INVALID_OFFSET;

    ssid_provisioned = 1;

    memset(ssid, 0 , 32);
    memcpy(ssid, data, *length);

    return BT_ATT_ERROR_SUCCESS;
}

/* Callback called when our custom characteristic is accessed */
static bt_att_error_t pass_provision_cb(uint8_t bearer, bt_uuid_t *uudid, 
   bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    if(offset != 0)
    return BT_ATT_ERROR_INVALID_OFFSET;

    pass_provisioned = 1;

    memset(pw, 0 , 32);
    memcpy(pw, data, *length);

    return BT_ATT_ERROR_SUCCESS;
}

/* Callback called when our custom characteristic is accessed */
static bt_att_error_t status_provision_cb(uint8_t bearer, bt_uuid_t *uudid,
    bt_gatt_fcn_t rw, uint8_t *length, uint16_t offset, uint8_t *data)
{
    /* Writes to this characteristic not allowed */
    if (rw != BT_GATT_FCN_READ)
    return BT_ATT_ERROR_WRITE_NOT_PERMITTED;

    if(offset != 0)
    return BT_ATT_ERROR_INVALID_OFFSET;

    /* status_flag is 1, means we got the IP */
    if(status_flag)
    {
        memset(status, 0 , 16);
        memcpy(status, STATUS_SUCCESS, STATUS_SUCCESS_LENGTH);
        *length =STATUS_SUCCESS_LENGTH;
        status_sent = 1;
        os_printf("client reading status : success\n");
    }
    else
    {
        /* wcm_return is 0 AND status_flag is 0 --> wait for notifications */
        if(wcm_return == 0)
        {

            if(link_down_timeout == 0) /* linkdown timer has not Timedout */
            {
                os_printf("client reading status:waiting\n");
                *length =STATUS_WAITING_LENGTH;
                waiting_status_count++;
                if (waiting_status_count >= 8) {
                    link_down_timeout = 1;
                    waiting_status_count = 0;
                }
            }
            else /* linkdown timer timedout */
            {
                /* memcpy "timeout" as status, */
                memset(status, 0 , 16);
                memcpy(status, STATUS_TIMEOUT, STATUS_TIMEOUT_LENGTH);
                *length =STATUS_TIMEOUT_LENGTH;
                os_printf("client reading status : timeout\n");
                /* in this case, keep status_sent to zero only and 
                   use timeout_sent instead */
                timeout_sent = 1;
            }
        }
        else /* non zero wcm_return --> failure */
        {
            memset(status, 0 , 16);
            memcpy(status, STATUS_FAILURE, STATUS_FAILURE_LENGTH);
            *length =STATUS_FAILURE_LENGTH;
            status_sent = 1;
            os_printf("client reading status:failure\n");
        }
    }

    memcpy(data, status, *length);

    return BT_ATT_ERROR_SUCCESS;
}


/* Creates a "custom server" i.e. adds our custom service and characteristic */
static void custom_server_create(void)
{
    srv.cust_service = bt_gatt_create_service_128(UUID_CUSTOM_SERVICE);
    bt_gatt_add_char_16(srv.cust_service, UUID_WIFI_SSID_16, ssid_provision_cb,
        GATT_PERM_WRITE, GATT_CHAR_PROP_W);
    bt_gatt_add_char_16(srv.cust_service, UUID_WIFI_PASSCODE_16, 
        pass_provision_cb, GATT_PERM_WRITE, GATT_CHAR_PROP_W);

    bt_gatt_add_char_16(srv.cust_service, UUID_WIFI_STATUS_16, 
        status_provision_cb, GATT_PERM_READ, GATT_CHAR_PROP_R);

    bt_gatt_add_service(srv.cust_service);
}

/* Starts our server */
static void start_server(void)
{
    bt_gap_cfg_adv_t bt_adv_handle;

    bt_adv_handle.fast_period = 10240;
    bt_adv_handle.slow_period = 0;
    bt_adv_handle.fast_interval = 160;
    bt_adv_handle.slow_interval = 480;
    bt_adv_handle.tx_power = 0;
    bt_adv_handle.channel_map = BT_HCI_ADV_CHANNEL_ALL;
    bt_gap_cfg_adv_set(&bt_adv_handle);

    /* Set our BLE address */
    bt_gap_addr_set(bt_hci_addr_type_random, SERVER_ADDR);

    /* Set our BLE address */
    bt_gap_addr_set(bt_hci_addr_type_random, SERVER_ADDR);

    /* Make server connectable (will enable advertisement) */
    bt_gap_connectable_mode(GAP_CONNECTABLE_MODE_UNDIRECT, 
      bt_hci_addr_type_random, addr_type_zero, address_zero, &gap_ops);
}

int main(void)
{
    status_flag = false;
    memset(status, 0 , 16);
    memcpy(status, STATUS_WAITING, STATUS_WAITING_LENGTH);

    bt_gap_init();

    /**
    * Common server implements the following services that 
    * will be added to our server:
    *
    * Generic access
    * Generic attribute
    * Device information
    */
    common_server_create("Inno_Ble_WiFiProvisioning", 0, "Innophase Inc");

    /**
    * 
    * Custom service
    *
    * Custom server implements the following services that will be 
    * added to our server
    */
    
    custom_server_create();

    start_server();

    /* while(1), to be 'continued-in' for restarting prov after failure 
      / timeout or to be 'breaken-out' for the prov success case */
    while(1)
    {
        os_printf("Inno_Ble_WiFiProvisioning started\n");
        while(!ssid_provisioned || !pass_provisioned)
        {
            //os_printf("debug 1 loop \n");
            os_msleep(1000);
        }

        wcm_return = wifi_main(ssid,pw);
        if(wcm_return != 0)
        {
            os_printf("main -- WiFi Connection Failed due to \
                  WCM returning error \n");
            /* continue to provisioning loop again resetting 
              ssid_provisioned pass_provisioned. */
            ssid_provisioned = 0;
            pass_provisioned = 0;
            /* make sure status 'failure' is sent, before starting prov again */
            while(!status_sent)
            {
                os_msleep(1000);
                //os_printf("debug 2 loop \n");
            }
            wifi_destroy(0);

            /* As we are restarting provisioning, reset the housekeeping and 
               status mssg to default value 'waiting' */

            memset(status, 0 , 16);
            memcpy(status, STATUS_WAITING, STATUS_WAITING_LENGTH);
            status_sent = 0;
            timeout_sent = 0;
            link_down_timeout = 0;
            wcm_return = 0;
            //os_printf("debug 3 continue \n");
            continue;
        }
        else
        {
            while(!status_sent)
            {
                /* IF timeout in linkdown happens due to TIMER, we continue 
                to while(1) loop after resetting ssid_provisioned 
                pass_provisioned and doing wifi_destroy etc */
                /* status mssg sent is 'timeout' and timeout_sent is used 
                to confirm that status 'timeout' has been read by client */
                /* linkdown Timeout happened. also status_sent is 0 in 
                timeout case */
                if(link_down_timeout == 1) 
                {
                    os_printf("int main -- WiFi Connection not succesfull \
                              due to LINK DOWN timeout scenario \n");

                    /* make sure status timeout is sent, before starting 
                      prov again */
                    while(!timeout_sent)
                    {
                        os_msleep(1000);
                        //os_printf("debug 4 loop \n");
                    }
                    wifi_destroy(0);

                    /* as we are restarting provisioning, reset the housekeeping 
                      and status mssg to default value 'waiting' */

                    /* can continue to provisioning loop again resetting 
                      ssid_provisioned pass_provisioned and housekeeping 
                      variables */
                    ssid_provisioned = 0;
                    pass_provisioned = 0;
                    link_down_timeout = 0;
                    wcm_return = 0;

                    memset(status, 0 , 16);
                    memcpy(status, STATUS_WAITING, STATUS_WAITING_LENGTH);

                    /* break from while (!status_sent), and later use timeout_sent 
                    to either break from or continue to provisioning while(1) */
                    status_sent = 1;

                    continue;
                }
                else /* if linkdown NOT timedout */
                {
                    os_msleep(1000);
                    //os_printf("debug 5 loop \n");
                }
            }

            if(timeout_sent == 1)
            {
                /* starting provisioning again, so reset remaining 
                housekeeping  */
                timeout_sent = 0;
                status_sent = 0;

                //os_printf("debug 6 continue \n");
                continue;
            }
            else
            {
                //os_printf("debug 7 break \n");
                break;
            }
        }
    }/* while(1) ends here */

    os_printf("status sent to phone app, now calling common_server_destroy \
             and bt_gap_destroy\n");
    common_server_destroy();
    bt_gap_destroy();

    /* start your app here */
    my_app_init();

    while(true)
        os_msleep(1000);

    return 0;
}
