/**
******************************************************************************
* @file websock.c
* @brief This file is part of the WebSocket module implimentation. 
*        This file provides  ]WebSocket client APIs. It also implements the 
*        helper functions.
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

/**
* @addtogroup WebSocket
* @{
*
* This module impliments the WebSocket client functionality. WebSocket is a 
* communication protocol providing full duplex, asynchronous communication 
* between the connected endpoints. WebSocket is designed to leverage the 
* currently existing web infrastructure. It is designed to work over HTTP ports 
* 80 and 443 as well as to supports HTTP proxies and intermediaries. 
* WebSocket is fully compatible with Web (HTTP) servers and uses HTTP upgrade 
* header to switch from HTTP to WebSocket protocol. WebSocket is standardized 
* by IETF as RFC6455.
*/

#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "lwip/netdb.h"
#include "http/inc/http_client.h"
#include "mbedtls/ssl.h"
#include "../inc/websock.h"


/*

Sec-WebSocket-Key:
---------------------
The request MUST include a header field with the name
        |Sec-WebSocket-Key|.  The value of this header field MUST be a
        nonce consisting of a randomly selected 16-byte value that has
        been base64-encoded (see Section 4 of [RFC4648]).  The nonce
        MUST be selected randomly for each connection.


 For this header field, the server has to take the value (as present
   in the header field, e.g., the base64-encoded [RFC4648] version minus
   any leading and trailing whitespace) and concatenate this with the
   Globally Unique Identifier (GUID, [RFC4122]) "258EAFA5-E914-47DA-
   95CA-C5AB0DC85B11" in string form, which is unlikely to be used by
   network endpoints that do not understand the WebSocket Protocol.  A
   SHA-1 hash (160 bits) [FIPS.180-3], base64-encoded (see Section 4 of
   [RFC4648]), of this concatenation is then returned in the server's
   handshake.

Sec-WebSocket-Version :
--------------------------
The request MUST include a header field with the name
        |Sec-WebSocket-Version|.  The value of this header field MUST be
        13.




Fette & Melnikov             Standards Track                    [Page 7]

RFC 6455                 The WebSocket Protocol            December 2011


   Concretely, if as in the example above, the |Sec-WebSocket-Key|
   header field had the value "dGhlIHNhbXBsZSBub25jZQ==", the server
   would concatenate the string "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
   to form the string "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-
   C5AB0DC85B11".  The server would then take the SHA-1 hash of this,
   giving the value 0xb3 0x7a 0x4f 0x2c 0xc0 0x62 0x4f 0x16 0x90 0xf6
   0x46 0x06 0xcf 0x38 0x59 0x45 0xb2 0xbe 0xc4 0xea.  This value is
   then base64-encoded (see Section 4 of [RFC4648]), to give the value
   "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=".  This value would then be echoed in
   the |Sec-WebSocket-Accept| header field.



    0                1               2               3
     0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
    +-+-+-+-+-------+-+-------------+-------------------------------+
    |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
    |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
    |N|V|V|V|       |S|             |   (if payload len==126/127)   |
    | |1|2|3|       |K|             |                               |
    +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
    |     Extended payload length continued, if payload len == 127  |
    + - - - - - - - - - - - - - - - +-------------------------------+
    |                               |Masking-key, if MASK set to 1  |
    +-------------------------------+-------------------------------+
    | Masking-key (continued)       |          Payload Data         |
    +-------------------------------- - - - - - - - - - - - - - - - +
    :                     Payload Data continued ...                :
    + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
    |                     Payload Data continued ...                |
    +---------------------------------------------------------------+


*/

/* WebSocket header field definitions */
#define WS_HDR_FIN_BIT         0x80
#define WS_HDR_MASK_BIT        0x80
#define WS_HDR_OPCODE_MASK     0x0F

/* Internal macros*/
#define WS_MAX_FRAME_SIZE      1460
#define WS_MAX_HEADERLEN       8
#define WS_MAX_FRAME_PAYLOAD   (WS_MAX_FRAME_SIZE - WS_MAX_HEADERLEN)/*1452*/
#define WS_MIN_HDR_LEN 2
#define WS_POLL_TIMEOUT_MS      1000
#define WS_MSG_WEBSOCKET        1100
#define WS_MAX_CONNECTIONS      16
#define WS_MAX_RECV_BUF_LEN     1400

typedef enum
{
    WS_OPCODE_CONTINUE   = 0x0,/**< Used for fragmentation*/
    WS_OPCODE_TEXT       = 0x1,/**< Data frame with content of type Text*/
    WS_OPCODE_BINARY     = 0x2,/**< Data frame with content of type Binary*/
    WS_OPCODE_CLOSE      = 0x8,/**< Close Control frame */
    WS_OPCODE_PING       = 0x9,/**< Ping Control frame */
    WS_OPCODE_PONG       = 0xA,/**< Pong Control frame */    
    WS_OPCODE_INVALID    = 0xF,/**< Pong Control frame */
}ws_opcode_e;

enum {
    WS_STATE_HDR_RECEIVING,
    WS_STATE_EXND_PL_LEN_RECEIVING,
    WS_STATE_PAYLOAD_RECEIVING
};

enum{
    WS_MSG_EVENT_RECV_POLL = 1
};

typedef struct ws_handle{
    struct callout poll_timer;                                            
    http_client_handle_t  http_handle;
    int sock_fd;/*get the socket fd from http using already available API*/
    ssl_wrap_handle_t *ssl_wrap_hndl;/*get the pointer from http using an API*/
    int secured;
    char *buf;
    char *recv_buf;
    websock_event_handler cb;
    void *cb_ctx;
    int state;
    int http_resp_status;
    struct os_semaphore ws_wait_sem;/**<sem to wait for handshake response from server*/
    long long payload_unread_len;/**<the payload len can be 8 bytes long*/
    char frame_hdr[16];/*Frame Header bufer*/
    int hdr_index;/*used during Frame header parsing. index to frame_hdr[]*/
    int hdr_unread_len;/*used during Frame header parsing*/
    int payload_len;/*used during Frame header parsing*/
    websock_msg_hdr_t msg_hdr;/**/
    struct ws_handle *next;/*Create a list, for polling for messages*/
}ws_handle_c;

struct ws_msg {
    struct os_msg hdr;
    int event;
    void *context;/**>Currently not used*/
};

static struct os_thread *ws_thread;
ws_handle_c *ws_handle_list;
struct os_semaphore ws_lock;


long long
ws_ntohll(long long n_num)
{
    long long h_num;
    unsigned int n;
    unsigned char *p = (unsigned char *)&n_num;

    n = (unsigned int) *((unsigned int *)&p[0]);
    h_num = ntohl(n);
    h_num <<= 32;
    n = (unsigned int) *((unsigned int *)&p[4]);
    h_num += ntohl(n);
    return h_num;
}

static void
ws_list_handle_add(ws_handle_c *h)
{
    os_sem_wait(&ws_lock);
    /*insert at the head*/
    h->next = ws_handle_list;
    ws_handle_list = h;
    os_sem_post(&ws_lock);
}

static void
ws_list_handle_remove(ws_handle_c *h)
{
    ws_handle_c *p, *prev;
    
    os_sem_wait(&ws_lock);
    p = prev = ws_handle_list ;
    if(p == h){
        ws_handle_list = h->next;
        goto exit;
    }
    while(p){
        if(p->next == h){
            p->next = h->next;
            break;
        }
        p = p->next;
    }
exit:    
    os_sem_post(&ws_lock);
}

inline void
ws_set_recv_state_to_frame_hdr_receiving(ws_handle_c *h)
{
    h->state = WS_STATE_HDR_RECEIVING;
    h->hdr_unread_len = WS_MIN_HDR_LEN;
    h->state = WS_STATE_HDR_RECEIVING;
    h->hdr_index = 0;
}

inline void
ws_set_msg_hdr_to_invalid(websock_msg_hdr_t *msg_hdr)
{
    msg_hdr->fin = -1;
    msg_hdr->opcode = -1;
    msg_hdr->payload_len = 0;
}

static int
ws_socket_send(ws_handle_c *h, char *buf, int len)
{
    int rval = 0;

    if(h->secured) {
        os_printf("\n%s : Secure send", __FUNCTION__);
        rval = ssl_wrap_write(h->ssl_wrap_hndl, (unsigned char *)buf, len);
    } else {
        os_printf("\n%s : Non Secure send", __FUNCTION__);
        rval = send(h->sock_fd, buf, len, 0);
    }
    
    os_printf("\n%s : rval = %d", __FUNCTION__, rval);
    return rval;/*bytes sent*/
}

static int 
ws_socket_recv(ws_handle_c *h, char *buf, int len, unsigned int timeout_sec)
{
    int ret, rcvd_bytes = 0;
    fd_set rs;
    struct timeval timeout;

    if(h->secured){
        rcvd_bytes = ssl_wrap_read_timeout(h->ssl_wrap_hndl, 
                                           (unsigned char *)buf, len, 
                                           timeout_sec *1000);        
    }else{
        FD_ZERO(&rs);
        FD_SET(h->sock_fd, &rs);
        timeout.tv_sec = timeout_sec;
        timeout.tv_usec = 0;
        
        ret = select(h->sock_fd+1, &rs, NULL, NULL, &timeout);
        if(ret > 0) {
            rcvd_bytes = recv(h->sock_fd, buf, len, 0);
        }           
    }
    os_printf("\nSock recved bytes = %d", rcvd_bytes);
    return rcvd_bytes;
}


void 
ws_close_intrnal(ws_handle_c *h)
{
    callout_stop(&h->poll_timer);
    http_client_close(h->http_handle);
    ws_list_handle_remove(h);
}

/*Websocket message is a series of one or more frames/fragments terminated 
by a frame with FIN bit set in the header

Payload length:  7 bits, 7+16 bits, or 7+64 bits

     The length of the "Payload data", in bytes: if 0-125, that is the
     payload length.  If 126, the following 2 bytes interpreted as a
     16-bit unsigned integer are the payload length.  If 127, the
     following 8 bytes interpreted as a 64-bit unsigned integer (the
     most significant bit MUST be 0) are the payload length.

Note: This implementation supports only max frame/fragment size of 64K. that is
    "Payload len" field value = 126. Hence the WS_MAX_HEADERLEN is 8 
*/
static int
ws_send_internal(ws_handle_c *h, char *buf, int len, ws_opcode_e opcode)
{
    int continuation_frame = 0;
    unsigned int mask, word_align_adjust;
    char *p;
    int i, payload_len, header_len;
    int in_len, rval;

    in_len = len;
    get_random_bytes(&mask, 4);
    if(NULL == h->buf){
        h->buf = os_alloc(WS_MAX_FRAME_SIZE);
        if(NULL == h->buf){
            return -1;
        }
    }
    os_printf("\n%s: len = %d", __FUNCTION__, len);
    do{        
       if(continuation_frame)
            opcode =  WS_OPCODE_CONTINUE;       
        memset(h->buf, 0, WS_MAX_HEADERLEN);
        p = h->buf;
        payload_len = (len > WS_MAX_FRAME_PAYLOAD)?WS_MAX_FRAME_PAYLOAD : len; 
        
        os_printf("\n%s: payload_len = %d", __FUNCTION__, payload_len);
        len -= payload_len;
        os_printf("\n%s 1: len = %d", __FUNCTION__, len);
        /*Encode header*/        
        *p = (!len)? (opcode + WS_HDR_FIN_BIT) : opcode;
        p++;
        if(payload_len < 126)
        {
            *p = (unsigned char)payload_len+ WS_HDR_MASK_BIT;
            p++;
            header_len = 6;
        }
        else if(payload_len < 65535)/*Payload Len = Two bytes -> Max 0xFFFF */
        {        
            *p = (unsigned char)(126 + WS_HDR_MASK_BIT);
            p++;
            *((unsigned short *)p) = ntohs(payload_len);
            p += 2;
            header_len = 8;
        }
        *((unsigned int *)p) = mask;
        p += 4;        
        if(NULL == buf){
            /*No payload. Just send the header*/
            os_printf("\nSending frame. No Payload");
            ws_socket_send(h, h->buf, header_len);
            break;
        }
        /*Copy the payload*/    
        memcpy(p, buf, payload_len);
        buf += payload_len;

        /*Used for padding during masking. The masking is done in multiples 
          of 1 word. so making the send buffer size word aligned. 
          Note that, the padding bytes are not sent to the server
        */
        word_align_adjust = (payload_len & 0x3)?(4 - (payload_len & 0x3)):0;
        /*mask the payload: 
          Octet i of the transformed data ("transformed-octet-i") is the XOR of
          octet i of the original data ("original-octet-i") with octet at index
          i modulo 4 of the masking key ("masking-key-octet-j"):
            j = i MOD 4
            transformed-octet-i = original-octet-i XOR masking-key-octet-j
        */
        for(i = 0; i < (payload_len + word_align_adjust) ; i += 4)
        {
            *((unsigned int *)p) ^= mask;
            p += sizeof(unsigned int);
        }
        /*send frame*/
        os_printf("\nsending len = %d", (payload_len + header_len));
        rval = ws_socket_send(h, h->buf, (payload_len + header_len));
        if(rval < 0){
            os_printf("\nError: ws socket send");
            in_len = rval;
            break;
        }      
        /*Continue sending the data if the payload does not fit in one 
          send buffer of size WS_MAX_FRAME_SIZE*/
        continuation_frame = 1;
    }while(len);        
    os_printf("\n%s: returning sent_len = %d", __FUNCTION__, in_len);
    return in_len;
}

static int
ws_recv_internal(ws_handle_c *h, websock_msg_hdr_t *msg_hdr, 
                 char *buf, int *len, int timeout)
{
    int nread, payload_len, recv_len;
    int in_buf_len = *len;
    
bigin:
    nread = 0;
    ws_set_msg_hdr_to_invalid(msg_hdr);  
    os_printf("\n%s: h->state = %d, len(IN) = %d", __FUNCTION__, h->state, *len);
    if(h->state == WS_STATE_HDR_RECEIVING){
        if(!h->hdr_unread_len)
            h->hdr_unread_len = WS_MIN_HDR_LEN;
        /*non blocking read*/        
        nread = ws_socket_recv(h, &h->frame_hdr[h->hdr_index], 
                               h->hdr_unread_len, timeout);
        if(nread <= 0){
            *len = 0;
            return nread;
        }
        if(nread < h->hdr_unread_len){
            h->hdr_unread_len -= nread;
            h->hdr_index += nread;
            *len = 0;/*In the middle of receiving the header. No payload available*/
            return 0;
        }
        h->hdr_index += nread;
        /*Get the message header information*/
        h->msg_hdr.fin = (h->frame_hdr[0] & WS_HDR_FIN_BIT)?1:0;
        h->msg_hdr.opcode = h->frame_hdr[0] & WS_HDR_OPCODE_MASK;
                
        payload_len = h->frame_hdr[1] & 0x7F;
        os_printf("\nPayload len = %d", payload_len);
        if(payload_len > 125){
            h->payload_len = payload_len;
            h->hdr_unread_len = (payload_len == 126)?2:8; 
            h->state = WS_STATE_EXND_PL_LEN_RECEIVING;
        }else{
            h->payload_unread_len = (long long)payload_len;
            h->msg_hdr.payload_len = payload_len;
            h->state = WS_STATE_PAYLOAD_RECEIVING;
            if(0 == payload_len){
                os_printf("\nNo payload to receive");
                ws_set_recv_state_to_frame_hdr_receiving(h);
                *len = 0;
            }
        }
    }

    if(h->state == WS_STATE_EXND_PL_LEN_RECEIVING){      
        nread = ws_socket_recv(h, &h->frame_hdr[h->hdr_index], 
                               h->hdr_unread_len, timeout);
        if(nread <= 0){
            *len = 0;
            return nread;
        }
        if(nread < h->hdr_unread_len){
            h->hdr_unread_len -= nread;
            *len = 0;/*In the middle of receiving the header. No payload available*/
            return 0;
        }
        payload_len = (long long)h->payload_len;
        if(payload_len ==126){
            payload_len = (long long) ntohs(*((short *)&h->frame_hdr[2]));
        }else{
            payload_len = (long long) *((long long *)&h->frame_hdr[2]);
            payload_len = ws_ntohll(payload_len);
        }
        os_printf("\nExtended payload len = %08x", payload_len);    
        h->payload_unread_len = (long long)payload_len;
        h->msg_hdr.payload_len = payload_len;
        h->state = WS_STATE_PAYLOAD_RECEIVING;
    }
    
    if(h->state == WS_STATE_PAYLOAD_RECEIVING){
exhaust_ping_payload: 
        recv_len = (*len < h->payload_unread_len)? *len : h->payload_unread_len;
        /*non blocking read*/        
        nread = ws_socket_recv(h, buf, recv_len, timeout);
        if(nread <= 0){
            *len = 0;
            return nread;
        }
        h->payload_unread_len -= recv_len;
        /* If a ping packet has payload, it will be received and discarded*/
        if(((h->frame_hdr[0] & WS_HDR_OPCODE_MASK) == WS_OPCODE_PING) &&
            h->payload_unread_len){
            goto exhaust_ping_payload;
        }
        /*Check if all the payload of the fragment/frame is fully received
            */
        if(0 == h->payload_unread_len){
            os_printf("\nDone with receiving frame..");
            ws_set_recv_state_to_frame_hdr_receiving(h);            
        }
        if(WS_OPCODE_PING != msg_hdr->opcode){
            /*Do not change the length of the buffer, as the PING payload 
              if any will be discarded*/
            *len = recv_len;
        }
    }

    if(WS_OPCODE_PING == h->msg_hdr.opcode){
        /*For a PING received, PONG is sent implicietly. The Ping data
          if any is discarded*/
        os_printf("\nPing packet. Send Pong. Go to begining");            
        ws_send_internal(h, NULL, 0, WS_OPCODE_PONG);
        *len = in_buf_len;/*Restore the in buf length*/
        goto bigin;

    }else if(WS_OPCODE_PONG == h->msg_hdr.opcode){
#if 0
        if(pConnHndl->pingSent)
        {   
            GsnOsal_SemRelease(&pConnHndl->waitLock);                   
            pConnHndl->pingSent = 0;
        }
#endif
    }
    /*Copy the message header*/
    memcpy(msg_hdr, &h->msg_hdr, sizeof(h->msg_hdr));
    return nread;/**/    
}

/*returns nfds*/
static int
ws_poll_fds_set(struct pollfd fds[])
{
    ws_handle_c *p;
    int i = 0;
    
    os_sem_wait(&ws_lock);
    p = ws_handle_list ;
    while(p){
        fds[i].fd = p->sock_fd;
        fds[i++].events = POLLIN | POLLERR;
        p = p->next;
    }
    os_sem_post(&ws_lock);
    return i;
}

/*Returns WebSocket handle for the supplied socket fd*/
static ws_handle_c *
ws_fd_to_handle(int sock_fd)
{
    ws_handle_c *p;
  
    os_sem_wait(&ws_lock);
    p = ws_handle_list ;
    while(p){        
        if(sock_fd == p->sock_fd)
            break;
        p = p->next;
    }
    os_sem_post(&ws_lock);
    return p;
}

static struct os_msg*
ws_msg_create(int event, void * context)
{
    struct ws_msg *msg;

    OS_MSG_ALLOC(msg, WS_MSG_WEBSOCKET, 0);
    msg->event = event;
    msg->context = context;
    return &msg->hdr;
}    

/*Note This callback is called in the interrupt context. A time out message is
  posted to the WebSOcket thread. The thread will do the poll for the 
  data on WebSockets open and do the further processing*/
__irq void
ws_poll_timeout_cb(struct callout *co)
{
    /*Send message and offload the work to a thread*/
    os_sendmsg(ws_thread, ws_msg_create(WS_MSG_EVENT_RECV_POLL, NULL));

    /* reschedule to get a periodic behaviour*/
    callout_schedule(co, SYSTIME_MS(WS_POLL_TIMEOUT_MS));
}

static void*
ws_event_thread_entry(void* arg)
{
    ws_handle_c *h;
    int ret, rbytes;
    int nfds, i, len;
    websock_msg_hdr_t msg_hdr;
    os_printf("\n%s", __FUNCTION__);
    for(;;) {
        struct ws_msg *msg;
        /*may have to exhaust all mesages in loop as all messages are of same 
        type*/
        msg = (struct ws_msg*)os_recvmsg_type(WS_MSG_WEBSOCKET);
        
        if(msg->event != WS_MSG_EVENT_RECV_POLL)
            continue;

        /*poll all the fds*/
        struct pollfd fds[WS_MAX_CONNECTIONS];
        nfds = ws_poll_fds_set(fds);
        os_printf("\npolling...");
        ret = poll(fds, nfds, 0);
        
        os_printf("\n%s: message received. nfds = %d, poll ret = %d", __FUNCTION__,
                    nfds, ret);        
    	if (ret == -1) {
    		os_printf("\nError: poll() error!!");
    	}
    	if (0 == ret) {
            continue;
    	}
        for(i = 0; i < nfds; i++){
            if (fds[i].revents & POLLIN){
                h = ws_fd_to_handle(fds[i].fd);
                if(NULL == h->recv_buf){
                    h->recv_buf = os_alloc(WS_MAX_RECV_BUF_LEN);
                    if(NULL == h->recv_buf){
                        os_printf("\nError: malloc failure @%s:%d", __FUNCTION__,
                                    __LINE__);
                        os_msleep(1000);
                        continue;
                    }
                }
                len = WS_MAX_RECV_BUF_LEN;
                do{
                    rbytes = ws_recv_internal(h, &msg_hdr, h->recv_buf, &len, 0);
                    if(rbytes < 0){
                        /*call the call back*/                        
                        /*Call the user call back*/
                        (h->cb)(h->cb_ctx, WEBSOCK_CB_EVENT_ERROR, &msg_hdr, 
                                NULL, len);
                        /*close the connection*/
                        os_printf("\nClosing the connection!!!");
                        ws_close_intrnal(h);
                        break;
                        
                    }
                    if(rbytes >= 0 && msg_hdr.opcode >= 0){
                        /*Call the user call back*/
                        (h->cb)(h->cb_ctx, WEBSOCK_CB_EVENT_DATA, &msg_hdr, 
                                h->recv_buf, len);
                    }
                }while(rbytes > 0);                
            }
            if (fds[i].revents & POLLERR){
    		    os_printf ("\nfd %d has error. Close", fds[i].fd);
            
            }
        }        
    }
    return NULL;
}

static void
ws_init(void)
{
    static int init_done = 0;
    if(init_done)
        return;
    init_done =1;
    os_sem_init(&ws_lock, 1);
    ws_handle_list = NULL;
}

static void
ws_event_thread_int(ws_handle_c *h)
{
    static int thread_init_done = 0;
    if(thread_init_done)
        return;
    thread_init_done = 1;
    ws_thread = os_create_thread("ws_event_thread", ws_event_thread_entry, 
                                  NULL, 2, 4096);

    /*Initialize the callout object*/
    callout_init(&h->poll_timer, ws_poll_timeout_cb);
    /*Schedule a callback in WS_POLL_TIMEOUT_MS */
    callout_schedule(&h->poll_timer, SYSTIME_MS(WS_POLL_TIMEOUT_MS));
}

static void 
ws_http_resp_cb(void *ctx, http_client_resp_info_t *resp)
{
    ws_handle_c *h = (ws_handle_c *)ctx;
    h->http_resp_status = (resp->status_code == 101)?1:0;
}
/*
 * APIs for the internal use line MQTT over websock and stw
 */
int
websock_sock_fd_get(websock_handle_t handle)
{
    ws_handle_c *h = handle;
    return h->sock_fd;
}

websock_handle_t
websock_handle_get(int sock_fd)
{
    return ws_fd_to_handle(sock_fd);
}

/*
 * API implimentation
 */

websock_handle_t
websock_open(websock_config_t *ws_cfg)
{
    http_client_config_t http_cfg;
    int rval, i;
    /*This is initialises only once*/    
    ws_init();
    ws_handle_c *h = os_alloc(sizeof(ws_handle_c));
    if(NULL == h){
       goto error_exit;
    }
    memset(h, 0, sizeof(ws_handle_c));
    h->hdr_unread_len = WS_MIN_HDR_LEN;
    /*open http connection*/
    memset(&http_cfg, 0, sizeof(http_client_config_t));
    http_cfg.hostname = (char *)ws_cfg->hostname;
    http_cfg.port = ws_cfg->port;
    http_cfg.secured = ws_cfg->secured;
    http_cfg.ssl_cfg = ws_cfg->ssl_config;
    
    h->http_handle = http_client_open(&http_cfg);
    if(NULL == h->http_handle) {
        os_printf("\nError:Connecting to the server");
        goto error_exit;
    }

    /*Set compulsary headers*/
    http_client_set_req_hdr(h->http_handle, "Host", ws_cfg->hostname);
    http_client_set_req_hdr(h->http_handle, "Upgrade", "websocket");
    http_client_set_req_hdr(h->http_handle, "Connection", "Upgrade");
    http_client_set_req_hdr(h->http_handle, "Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");
    http_client_set_req_hdr(h->http_handle, "Sec-WebSocket-Version","13");
    /*Set application supplied headers*/
    for(i = 0; i < ws_cfg->num_hndshk_hdrs; i++){
        http_client_set_req_hdr(h->http_handle, ws_cfg->hndshk_hdrs[i].name,
                                ws_cfg->hndshk_hdrs[i].val);    
    }
    /*WebSocket opening handshake*/
    rval = http_client_get(h->http_handle, ws_cfg->uri, ws_http_resp_cb, h, 300);   
    if(rval < 0 || 0 == h->http_resp_status){
        os_printf("\nError: Upgrade to WebSocket failed");
        goto error_exit;
    }
    /*WbSick is now connected. Store the required info*/
    h->secured = ws_cfg->secured;
    if(h->secured){
        h->ssl_wrap_hndl = http_client_ssl_context_get(h->http_handle);
        h->sock_fd = http_client_sock_fd_get(h->http_handle);
        os_printf("\n%s: secure sock_fd = %d %p", __FUNCTION__, h->sock_fd,
            h->ssl_wrap_hndl);
    }else{
        h->sock_fd = http_client_sock_fd_get(h->http_handle);
        os_printf("\n%s: sock_fd = %d", __FUNCTION__, h->sock_fd);
    }
    /*Set the socket to Non-Blocking mode. use poll() for timed block*/
    if(fcntl(h->sock_fd, F_SETFL, O_NONBLOCK) < 0){
        os_printf("\nError: Fialed to se the sock fd in non-blocking mode");
    }
    ws_list_handle_add(h);
    return h;
error_exit:
    if(NULL != h)
        os_free(h);
    return NULL;
}

void 
websock_close(websock_handle_t handle)
{
    ws_handle_c *h = handle;
    ws_close_intrnal(h);
}

/*Websocket message is a series of one or more frames/fragments terminated 
by a frame with FIN bit set in the header

Payload length:  7 bits, 7+16 bits, or 7+64 bits

     The length of the "Payload data", in bytes: if 0-125, that is the
     payload length.  If 126, the following 2 bytes interpreted as a
     16-bit unsigned integer are the payload length.  If 127, the
     following 8 bytes interpreted as a 64-bit unsigned integer (the
     most significant bit MUST be 0) are the payload length.

Note: This implementation supports only max frame/fragment size of 64K. that is
    "Payload len" field value = 126. Hence the WS_MAX_HEADERLEN is 8 
*/
int
websock_send_text(websock_handle_t handle, char *payload, int len)
{
    ws_handle_c *h = (ws_handle_c *)handle;
    return ws_send_internal(h, payload, len, WS_OPCODE_TEXT);
}

int
websock_send_binary(websock_handle_t handle, char *payload, int len)
{
    ws_handle_c *h = (ws_handle_c *)handle;
    return ws_send_internal(h, payload, len, WS_OPCODE_BINARY);
}

int
websock_send_ping(websock_handle_t handle, char *payload, int len)
{
    ws_handle_c *h = (ws_handle_c *)handle;
    return ws_send_internal(h, payload, len, WS_OPCODE_PING);
}

int
websock_send_pong(websock_handle_t handle, char *payload, int len)
{
    ws_handle_c *h = (ws_handle_c *)handle;
    return ws_send_internal(h, payload, len, WS_OPCODE_PONG);
}


/*
    timeout -> in sec
*/
int
websock_recv(websock_handle_t handle, websock_msg_hdr_t *msg_hdr, 
             char *buf, int *len, int timeout)
{
    ws_handle_c *h = (ws_handle_c *)handle;
    return ws_recv_internal(h, msg_hdr, buf, len, timeout);
}

void
websock_set_event_handler(websock_handle_t handle, websock_event_handler cb,
                          void *cb_context)
{
    ws_handle_c *h = handle;
    h->cb = cb;
    h->cb_ctx = cb_context;
    /*The event handling setup (Thread + Timer) is needed only if event handler/s
      This will initialise the event handling set up only once*/
    ws_event_thread_int(h);        
}


