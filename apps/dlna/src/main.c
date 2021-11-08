#include <kernel/os.h>
#include <kernel/gpio.h>
#include <wifi/wcm.h>
#include <lwip/ip4_addr.h>
#include <lwip/ip6_addr.h>
#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include "upnp_tv_device.h"

#define LED_PIN 14

struct param_t {
    const char *ssid;
    const char *passphrase;
    const char *url;
    const char *host;
    const char *path;
    const char *port;
    const char *secured;
    const char *method;
    const char *ca_cert;
    const char *post_len;
    const char *client_cert;
    const char *client_key;
    
    const char *hdr1_name;
    const char *hdr1_val;
    const char *hdr2_name;
    const char *hdr2_val;
    const char *hdr3_name;
    const char *hdr3_val;

    const char *test_iterations;
    const char *use_ca_bundle;

};

struct param_t param;
struct os_semaphore app_wcm_lock;
int wcm_connect_success = 0;

int led_pin = 1 << LED_PIN;

/**
 * wcm notify callback
 */
void
app_wcm_notify_cb(void *ctx, struct os_msg *msg)
{
    switch(msg->msg_type) {
    case(WCM_NOTIFY_MSG_LINK_UP):
        break;
    case(WCM_NOTIFY_MSG_LINK_DOWN):
        os_sem_post(&app_wcm_lock);
        break;
    case(WCM_NOTIFY_MSG_ADDRESS):
        break;
    case WCM_NOTIFY_MSG_CONNECTED:
        wcm_connect_success = 1;
        os_sem_post(&app_wcm_lock);
        break;
    }
    os_msg_release(msg);
}

bool
app_wcm_connect(struct wcm_handle *wcm_h, char *ssid, char *pw)
{
    wcm_notify_enable(wcm_h, app_wcm_notify_cb, NULL);
    os_printf("\nstart to add network");
    wcm_add_network(wcm_h, ssid, NULL, pw);
    os_printf("\nstart to auto connect");
    wcm_auto_connect(wcm_h, 1);
    os_sem_wait(&app_wcm_lock);
    return false;
}

static void *
TvDeviceCommandLoop( void *args )
{
    while( 1 ) {
        //toggle gpio HIGH
        os_msleep(500);
        os_gpio_set_pin(led_pin);
        //toggle gpio LOW
        os_msleep(500);
        os_gpio_clr_pin(led_pin);
        //size_t avai = os_avail_heap();/////////////////////////
        //os_printf("\%d\n",avai);////////////////////
    }
    return NULL;
}

int
main(void)
{
    struct wcm_handle *wcm_handle;
    char ssid[] = {'G', 'E', 'N', 'I', 'M', 'O', 'U', 'S', 0};//"wangzisheng";
    char passphrase[] = {'$', 'j', 'G', '7', 'M', 'a', 'B', 'T', 0};//"rik123456";
    //char ip[]="10.1.3.77";
    
    char *ip_address = NULL, *desc_doc_name = NULL, *web_dir_path = NULL;
    unsigned int port = 0;
    ip_addr_t *myaddr;
    
    ithread_t *cmdloop_thread;
    
    os_gpio_request(led_pin);
    os_gpio_set_output(led_pin);
    os_gpio_clr_pin(led_pin);
    
   
    /*Connect to WiFi N/w*/
    os_sem_init(&app_wcm_lock, 0);
    wcm_handle = wcm_create(NULL);
    app_wcm_connect(wcm_handle, (char *)ssid, (char *)passphrase);
    
    if(!wcm_connect_success) {
        os_printf("\n[APP]Error: Failed to connect to WiFi N/w");
        goto exit;
    }
    os_printf("\n[APP]Success: Connected to %s N/w", (char *)ssid);
    
    /*get local ip addr*/
    myaddr = netif_ip_addr4(wcm_get_netif(wcm_handle));
    //return (*ip_2_ip4(myaddr)).addr;//(myaddr->u_addr.ip4);
    ip_address = inet_ntoa((*ip_2_ip4(myaddr)).addr);
    //os_printf("\nmy addr is %s\n",ip_address);
 //os_gpio_clr_pin(led_pin);
    TvDeviceStart( ip_address, port, desc_doc_name, web_dir_path,
                   os_printf );
                   
    /*
       start a command loop thread 
     */
    cmdloop_thread = os_create_thread(NULL, TvDeviceCommandLoop, NULL, OS_THREADPRI_LO|OS_CRTHREAD_DETACHED, 512);
    if(cmdloop_thread==NULL) {
        os_printf("\n[APP]Error: Failed to create thread TvDeviceCommandLoop");
        goto exit;
    }

    /* waits for thread function to finish */
    //os_join_thread(cmdloop_thread);
                           

    
 exit:   
    while(1)
    {
        os_msleep(10000);
    }
    //TvDeviceStop();
    return 0;
}
