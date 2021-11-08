#include <errno.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "websocket/inc/websock.h"
#include "mqtt/platform/mqtt_nw.h"


extern websock_handle_t
websock_handle_get(int sock_fd);
extern int
websock_sock_fd_get(websock_handle_t handle);

/**
 * Reference:
 *   http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/mqtt-v3.1.1.html
 *   Note: Reffer section 6
 *
 * Here is the excerpt from the above document:
 *  If MQTT is transported over a WebSocket [RFC6455] connection, the following 
 *  conditions apply:
 *    · MQTT Control Packets MUST be sent in WebSocket binary data frames. 
 *      If any other type of data frame is received the recipient MUST close 
 *      the Network Connection [MQTT-6.0.0-1].
 *    · A single WebSocket data frame can contain multiple or partial MQTT 
 *      Control Packets. The receiver MUST NOT assume that MQTT Control Packets 
 *      are aligned on WebSocket frame boundaries [MQTT-6.0.0-2].
 *    · The client MUST include “mqtt” in the list of WebSocket Sub Protocols it 
 *      offers [MQTT-6.0.0-3].  
 *    · The WebSocket Sub Protocol name selected and returned by the server 
 *      MUST be “mqtt” [MQTT-6.0.0-4].
 *    · The WebSocket URI used to connect the client and server has no impact 
 *      on the MQTT protocol.
 *
 * When using MQTT WebSocket protocol, binary mode must be used and sub-protocol 
 * header must be carried during the opening handshake:
 *
 *     Sec-WebSocket-Protocol: mqtt (or mqttv3.1, "mqtt" is preferred)
 */
static int 
mqtt_ws_sock_read(MQTTNetwork* n, unsigned char* buf, int len, int timeout_ms)
{
    websock_handle_t handle;
    websock_msg_hdr_t msg_hdr;
    int rval;
    int timeout = (timeout_ms/1000)+1;
    handle = websock_handle_get(n->socket);
    rval = websock_recv(handle, &msg_hdr, (char *)buf, &len, timeout);
    if(rval < 0)
        os_printf("\n%s : rval = %d", __FUNCTION__, rval);
    if(rval > 0){
        int i;
        for(i = 0; i < rval; i++)
        os_printf("\n\t\t%s : 1st byte = %x", __FUNCTION__, buf[i]);
    }
    return rval;
}

static int 
mqtt_ws_sock_write(MQTTNetwork* n, unsigned char* buf, int len, 
                                int timeout_ms)
{
    websock_handle_t handle;
    int rval;
    handle = websock_handle_get(n->socket);
    rval = websock_send_binary(handle, (char *)buf, len);
    if(rval < 0)
        os_printf("\n%s :  = %d", __FUNCTION__, rval);
    return rval;
}

static void 
mqtt_ws_sock_disconnect(MQTTNetwork* n)
{
    websock_handle_t handle;

    handle = websock_handle_get(n->socket);
    websock_close(handle);
}

void MQTTNetworkInit_Ws(MQTTNetwork* n)
{
    n->socket = -1;
    n->mqttread = mqtt_ws_sock_read;
    n->mqttwrite = mqtt_ws_sock_write;
    n->disconnect = mqtt_ws_sock_disconnect;
}


int MQTTNetworkConnect_Ws(MQTTNetwork* n, websock_config_t * ws_cfg)
{
    websock_handle_t handle;
    int nhdr = ws_cfg->num_hndshk_hdrs;
    
    os_printf("\n%s:%s", __FILE__, __FUNCTION__);

    if(nhdr >= WEBSOCK_MAX_HNDSHK_HDRS){
        os_printf("\nError: num_hndshk_hdrs exceeds max");
        return -1;
    }
    /* When using MQTT WebSocket protocol, sub-protocol header must be 
     * carried during the opening handshake,
     *          Sec-WebSocket-Protocol: mqtt
     */
    ws_cfg->hndshk_hdrs[nhdr].name = "Sec-WebSocket-Protocol";
    ws_cfg->hndshk_hdrs[nhdr].val = "mqtt";
    ws_cfg->num_hndshk_hdrs++;
    
    handle = websock_open(ws_cfg);
    if(NULL == handle)
        return -1;
    n->socket = websock_sock_fd_get(handle);
    return n->socket;
}

void MQTTNetworkDisconnect_Ws(MQTTNetwork *n)
{
    (*n->disconnect)(n);
}

