#pragma once
/**
******************************************************************************
* @file websock.h
* @brief This file contains the Public APIs and data structure definitions for 
*   websocket client module. This module conforms to RFC 6455 
*   "The websocket Protocol".
******************************************************************************
* @attention
*
*
* Copyright (c) 2020, InnoPhase, Inc.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

/** @ingroup websocket
 *  @{
 */
#include <kernel/callout.h>
#include "ssl_wrap/inc/ssl_wrap.h"
/**
 *******************************************************************************
 * @ingroup websocket
 * @brief Max timeout in seconds, that can be passed to the websock_recv().
 ******************************************************************************/

#define WEBSOCK_MAX_RECV_TIMEOUT    4294967
/**
 *******************************************************************************
 * @ingroup websocket
 * @brief websocket handle.
 *
 * This is returned by the websock_open(). Shall be passed to all the other 
 * websocket APIs
 ******************************************************************************/
typedef  void*  websock_handle_t;

typedef struct{
    char *name;
    char *val;
}websoc_hndshk_hdr_t;

#define WEBSOCK_MAX_HNDSHK_HDRS 10
/**
 *******************************************************************************
 * @ingroup websocket
 * @brief Configuration parameters for a websocket connection.
 *
 * This structure defines configuration parameters used while opening a 
 * websocket connection.
 ******************************************************************************/
typedef struct {     
    char *hostname;/**<host name or the ip address of the server*/
    int port;/**<port*/
    char *uri;/**<Websocket uri to connect to*/
    int secured; /*secured websocket*/
    ssl_wrap_cfg_t ssl_config;
    int time_out;/**<tcp connection timeout*/
    /**<The headers to be set in the WebSocket Handshake request.
        The format shall be "key:val". Use this to pass headers that are not 
        set implicitly. Refer websock_open() for the list of headers set 
        implicitly*/
    websoc_hndshk_hdr_t hndshk_hdrs[WEBSOCK_MAX_HNDSHK_HDRS];
    int num_hndshk_hdrs;
} websock_config_t;

/**
 *******************************************************************************
 * @ingroup websocket
 * @brief structure defining the message information.
 *
 * This structure defines the WebSocket message. Used while receiving.
 ******************************************************************************/
typedef struct{
    int fin;/**< end of message*/
    int opcode;/**< opcode of the message frame. if < 0, 
                this structure contents are invalid. This can happen if 
                the header is not fully received, in the current call to 
                websock_recv().
                This is not an error, keep calling websock_recv(), unless,
                websock_recv() itself returns < 0*/
    long long payload_len;/*Total length of the payload of the fragment being
                            received.*/
}websock_msg_hdr_t;

/**
 *******************************************************************************
 * @ingroup websocket
 * @brief structure defining WebSOcket callback events.
 *
 * This structure defines the events reported through the callback(event handler). 
 * Used while receiving.
 ******************************************************************************/
typedef enum{
    WEBSOCK_CB_EVENT_DATA,/**<Callback event is Data*/
    WEBSOCK_CB_EVENT_ERROR/**<Callback event is socket error*/
}websock_cb_event_t;

/**
 *******************************************************************************
 * @ingroup websocket
 * @brief websocket event handler.
 *
 * This callback/event handler definition. The event handle of this type need 
 * to be registered for asynchronous reporting of events defined in 
 * websock_cb_event_t.
 ******************************************************************************/

typedef void (*websock_event_handler)(void *c, websock_cb_event_t evnt,
                websock_msg_hdr_t *msg_hdr, char *payload, int len);
/**
 *******************************************************************************
 * @ingroup websocket
 * @brief Open a websocket connection to a websocket capable server.
 *
 * This function opens a TCP socket to the server and performs the initial
 * HTTP-based handshake to upgrade to the websocket protocol.  
 * Both secure (SSL) and non-secure websocket connections are supported.
 *
 * @param cfg - Pointer to the configuration data structure for this connection.
 *
 * @retval Pointer to Data structure of type websock_handle_t. 
 *         This is the pointer to the websocket Connection Handle. 
 *         Once the open is done, the same should be used in all other 
 *         Web Sock API for the specific connection.
 * @note :
 *
 *   Headers set implicitly during connection handshake:
 *
 *   1.  |Host| header field whose value
 *           contains /host/ plus optionally ":" followed by /port/ (when not
 *           using the default port).
 *
 *   2.  |Upgrade| header field whose value
 *           MUST include the "websocket" keyword.
 *           
 *   3.  |Connection| header field whose value
 *           MUST include the "Upgrade" token.
 *
 *   4.  |Sec-websocket-Key|. The value of this header field MUST be a
 *           nonce consisting of a randomly selected 16-byte value that has
 *           been base64-encoded (see Section 4 of [RFC4648]).  The nonce
 *           MUST be selected randomly for each connection
 *   5.  |Sec-websocket-Version|.  The value of this header field MUST be 13.
 *
 *   The |Host| |Upgrade|, |Connection|, |Sec -Websocket-key| and 
 *   |Sec-websocket-Version| headers are implicitly set by the websock_Open() 
 *   during the handshake. 
 *   Any other  headers to be used in connection upgrade request must be set 
 *   by the caller 
 ******************************************************************************/
websock_handle_t 
websock_open(websock_config_t *ws_cfg);

/**
 *******************************************************************************
 * @ingroup websocket
 * @brief Close a websocket connection.
 *
 * This function initiates the websocket close handshake with the server if the
 * connection is in the open state.  Otherwise, it just marks the connection
 * closed.  
 * @param h -  Pointer to connection structure for the websocket being closed
 *
 ******************************************************************************/
void
websock_close(websock_handle_t h);

/**
 *******************************************************************************
 * @ingroup websocket
 * @brief Send a message over a websocket connection.
 *
 * This function send "text" data.
 * 
 * @param h -  Pointer to websocket connection structure
 * @param payload -  message payload.
 * @param len -  length of the message payload.
 * 
 * @retval  >= 0 success
 * @retval  < 0  failure
 ******************************************************************************/
int
websock_send_text(websock_handle_t handle, char *payload, int len);

/**
 *******************************************************************************
 * @ingroup websocket
 * @brief Send a message over a websocket connection.
 *
 * This function send "binary" data.
 * 
 * @param h -  Pointer to websocket connection structure
 * @param payload -  message payload.
 * @param len -  length of the message payload.
 * 
 * @retval  >= 0 success
 * @retval  < 0  failure
 ******************************************************************************/
int
websock_send_binary(websock_handle_t handle, char *payload, int len);

/**
 *******************************************************************************
 * @ingroup websocket
 * @brief Send a PING message over a websocket connection.
 *
 * This function send a ping message.
 * 
 * @param h -  Pointer to websocket connection structure
 * @param payload -  message payload.
 * @param len -  length of the message payload.
 * 
 * @retval  >= 0 success
 * @retval  < 0  failure
 ******************************************************************************/
int
websock_send_ping(websock_handle_t handle, char *payload, int len);

/**
 *******************************************************************************
 * @ingroup websocket
 * @brief Send a PONG message over a websocket connection.
 *
 * This function send a pong message.
 * 
 * @param h -  Pointer to websocket connection structure
 * @param payload -  message payload.
 * @param len -  length of the message payload.
 * 
 * @retval  >= 0 success
 * @retval  < 0  failure
 ******************************************************************************/
int
websock_send_pong(websock_handle_t handle, char *payload, int len);


/**
 *******************************************************************************
 * @ingroup websocket
 * @brief Receive a message over a websocket connection.
 *
 * This function receives websocket messages.  This internally handles websocket 
 * close and ping messages. THis is a blocking call, and blocks for the data for 
 * the specified timeout. 
 * Note that this API need to be kept calling in loop in a separate thread 
 * context.
 * If the applicatin chooses to receive data using this API, it shall not use
 * websock_set_event_handler() for the same connection.
 *
 * @param h -  Pointer to websocket connection structure
 * @param msg_hdr -  Pointer to message header structure, through whch the message 
 *              header information is passed.
 * @param buf - pointer to buffer for copying payload data 
 * @param len - buffer length in bytes
 * @param timeout - time in mSec to block for the data.
 *
 * @retval number of bytes received.
 * @retval < 0 in case of any error
 ******************************************************************************/

int
websock_recv(websock_handle_t handle, websock_msg_hdr_t *msg_hdr, 
             char *buf, int *len, int timeout);

/**
 *******************************************************************************
 * @ingroup websocket
 * @brief set message receive event indication call back.
 * 
 * The websocket is not strictly a request-response 
 * protocol. Its an asynchronous data exchange protocol.The message (data) can 
 * come from server at any time without being requested.
 * So a callback mechanism is provided. A callback/event handler can be set
 * using this API. The event handle is called for the events defined in
 * websock_cb_event_t. 
 
 * @param h -  Pointer to websocket connection structure
 * @param cb -  application call back to be called when data is available on 
                websocket
 * @param cb_context - Any context pointer to be passed along callback
 *
 * @retval None
 ******************************************************************************/
void
websock_set_event_handler(websock_handle_t handle, websock_event_handler cb,
                          void *cb_context);

/** @} */

