/**
  *****************************************************************************
  * @file   wifi_control.c
  *
  * @brief  The sample application  creates a BLE GATT profile
  * and service which is then used to provision a connection to a Wi-Fi 
  * network. An android application running a custom android application is
  * used to input the Wi-Fi credentials. 
  *
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

#include <kernel/callout.h>
#include <kernel/gpio.h>
#include <kernel/timer.h>

/* WIFI INTERFACE*/
#include "wifi/wifi.h"
#include "wifi/wcm.h"
#include "api/wcm.h"

#define TIMER_TIMEOUT_SEC_IN_MICRO 8000000
/* note-- tested with 30s for testing --> 'linkup cancelling the timer' 
  case, for cases when AP comes ON later after few linkdowns */

/* true when we get IP */
extern bool status_flag; 
extern bool link_down_timeout;

/*timer context data structure*/
typedef struct timer_user_data_t
{
  /* unsigned int timer_created_at; */
  unsigned int timeout;
  unsigned int timer_running;
  os_timer_id_t timer_id;
}timer_user_data;

static timer_user_data *ptimer_user_data;

static struct wcm_handle *h = NULL;


/*Timer call back functions*/
static void on_timer_event_callback(void *user_data);


static void my_wcm_notify_cb(void *ctx, struct os_msg *msg)
{
    switch(msg->msg_type)
    {

        case(WCM_NOTIFY_MSG_LINK_UP):
        os_printf("wcm_notify_cb to App Layer - WCM_NOTIFY_MSG_LINK_UP\n");
        /* if timer active, then the linkdown timer STOPS here */
        if(ptimer_user_data->timer_running == 1)
        {
              os_timer_reset(ptimer_user_data->timer_id);
              ptimer_user_data->timer_running = 0;
              os_printf ("\n Cancelling the Timeout Timer. \
                 current time:[%u] \n", os_systime());
        }
            break;

        case(WCM_NOTIFY_MSG_LINK_DOWN):
        os_printf("wcm_notify_cb to App Layer - WCM_NOTIFY_MSG_LINK_DOWN\n");

        /* a timer starts on linkdown (IF already NOT running), 
          timeout 5 seconds (#define TIMER_TIMEOUT_SEC_IN_MICRO) */
        if(ptimer_user_data->timer_running == 0)
        {
            //ptimer_user_data->timer_created_at = os_systime();
            ptimer_user_data->timeout = os_systime()+
                 (TIMER_TIMEOUT_SEC_IN_MICRO);

            /*setting the timer callback and user data*/
            ptimer_user_data->timer_id = os_timer_allocate(TIMER_BASE_US, 
            TIMER_ANY, on_timer_event_callback, ptimer_user_data);

            /*starting the timer for required timeout. 
                 timeout time is in microseconds*/  
            os_timer_set(ptimer_user_data->timer_id, 
                  ptimer_user_data->timeout);
            ptimer_user_data->timer_running = 1;
            os_printf ("\n Linkdown Timeout Timer started. \
                  current time:[%u] \n", os_systime());
        }

            break;

        case(WCM_NOTIFY_MSG_ADDRESS):
        os_printf("wcm_notify_cb to App Layer - WCM_NOTIFY_MSG_ADDRESS\n");
        status_flag = true;
        /* if AP goes OFF after giving IP so linkdown happens and the IP 
           becomes 0, then also status is becoming 1 -- check later */

        /* if active, then the linkdown timer STOPS here (mostly, timer
          will never be active here, still) */
        if(ptimer_user_data->timer_running == 1)
        {
              os_timer_reset(ptimer_user_data->timer_id);
              ptimer_user_data->timer_running = 0;
              os_printf ("\n Cancelling the Timeout Timer. \
                     current time:[%u] \n", os_systime());
        }
            break;

        default:
            break;
    }
    os_msg_release(msg);
}


int wifi_main(char *ssid, char *pw)
{
    /*creating a timer*/

    /*allocating timer user data*/
    ptimer_user_data =  os_alloc(sizeof(timer_user_data));
    ptimer_user_data->timer_running = 0;

    int status;

    os_printf("\n\rWiFi Details  SSID: %s, PASSWORD: %s\n\r", ssid, pw);

    h = wcm_create(NULL);
    wcm_notify_enable(h, my_wcm_notify_cb, NULL);
    if( h == NULL ){
        os_printf(" failed.\n");
    return -1;
    }

    os_printf("Connecting to WiFi...\n");
    /* async connect to a WiFi network */

    status = wcm_add_network(h, ssid, NULL, pw);
    os_printf("add network status: %d\n", status);


    if(status != 0){

        os_printf("adding network Failed\n");
        /* can fail due to, already busy, no memory, 
          wor badly formatted password */
        return status;

    }

    os_printf("added network successfully, will try connecting..\n");

    status = wcm_auto_connect(h, 1);
    os_printf("connecting to network status: %d\n", status);
    if(status != 0){

        os_printf("trying to connect to network Failed\n");
        /* can fail due to, already busy, no memory */
        return status;
    }


#if 0
    /* RELOCATE below itmes -- will need to change place in new async way */

    /* bad when timer was not created in linkdown? if yes, 
      then release in linkup etc */
    //os_timer_release(ptimer_user_data->timer_id); 
    
    os_free(ptimer_user_data); 
#endif

    return status;
}

/* linkdown timer's callback just sets a flag, to be used by att read cb 
   and app_main to behave accordingly */
static void on_timer_event_callback(void *user_data)
{
    // timer_user_data *ptimer_user_data;
    // ptimer_user_data = user_data;

    os_printf("\n Timeout Event occured.\n");
    link_down_timeout =1;
}


void wifi_destroy(bool state_connected)
{
    if(state_connected) {
        wcm_auto_connect(h, 0);
    }
    wcm_destroy(h);
}
