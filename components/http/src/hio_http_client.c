/*
 * HTTP API specification
 */
#include <stdlib.h>
#include <stdio.h>
#include <kernel/os.h>
#include <wifi/wcm.h>
#include <errno.h>

#include <string.h>
#include <kernel/hostio.h>
#include <lwip/err.h>
#include <lwip/inet.h>
#include <lwip/api.h>
#include <lwip/tcpip.h>


#include <wifi/wifi.h>
#include <lwip/netdb.h>


#include <lwip/netif.h>

#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "../inc/http_client.h"

#pragma GCC diagnostic push
/* Suppress the "-Wstringop-truncation" warning. It was created by cogg logic.! */
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#include "hio_http_client.h"
#pragma GCC diagnostic pop

#define URL_HOST "httpbin.org"
#define URL_PATH "/ip"

#define SERVER_HTTP_PORT_NUM    80
#define SERVER_HTTPS_PORT_NUM  443
#define HTTP_SUCCESS  0
#define HTTP_FAILURE -1
#define HTTP_CLIENT_MAX     10

enum hio_http_events {
    HIO_HTTP_EVT_CREATE_REQ      = -8000,
    HIO_HTTP_EVT_CREATE_RESP,
    HIO_HTTP_EVT_SEND_REQ,
    HIO_HTTP_EVT_SEND_RESP,
    HIO_HTTP_EVT_SET_HDR_REQ,
    HIO_HTTP_EVT_SET_HDR_RESP,
    HIO_HTTP_EVT_DEL_HDR_REQ,
    HIO_HTTP_EVT_DEL_HDR_RESP,
    HIO_HTTP_EVT_STORE_CERT_REQ,
    HIO_HTTP_EVT_STORE_CERT_RESP,
    HIO_HTTP_EVT_DEL_CERT_REQ,
    HIO_HTTP_EVT_DEL_CERT_RESP,
    HIO_HTTP_EVT_CLOSE_REQ,
    HIO_HTTP_EVT_CLOSE_RESP
};


typedef enum {
    HIO_HTTP_HDR_INVAL,       /* special value for invalid header */
    HIO_HTTP_HDR_ALLOW,
    HIO_HTTP_HDR_AUTHORIZATION,
    HIO_HTTP_HDR_CONNECTION,
    HIO_HTTP_HDR_CONTENT_ENCODING,
    HIO_HTTP_HDR_CONTENT_LENGTH,
    HIO_HTTP_HDR_CONTENT_RANGE,
    HIO_HTTP_HDR_CONTENT_TYPE,
    HIO_HTTP_HDR_COOKIE,
    HIO_HTTP_HDR_COOKIE2,
    HIO_HTTP_HDR_DATE,
    HIO_HTTP_HDR_EXPIRES,
    HIO_HTTP_HDR_FROM,
    HIO_HTTP_HDR_HOST,
    HIO_HTTP_HDR_IF_MODIFIED_SINCE,
    HIO_HTTP_HDR_LAST_MODIFIED,
    HIO_HTTP_HDR_LOCATION,
    HIO_HTTP_HDR_PRAGMA,
    HIO_HTTP_HDR_RANGE,
    HIO_HTTP_HDR_REFERER,
    HIO_HTTP_HDR_SERVER,
    HIO_HTTP_HDR_SET_COOKIE,
    HIO_HTTP_HDR_TRANSFER_ENCODING,
    HIO_HTTP_HDR_USER_AGENT,
    HIO_HTTP_HDR_WWW_AUTHENTICATE,
    HIO_HTTP_HDR_COUNT,
    HIO_HTTP_HDR_CUSTOM       /* Value indicating the start of custom headers */
} httphdrtype;

static struct os_thread *hio_http_thread = NULL;
static bool hio_http_thread_init = false;

/**
 * Specifies the http header names.
 */
const char * httphdrnames[] = {
    "Inval",                    /* special value for invalid header */
    "Allow",
    "Authorization",
    "Connection",
    "Content-Encoding",
    "Content-Length",/*5*/
    "Content-Range",
    "Content-Type",
    "Cookie",
    "Cookie2",
    "Date",
    "Expires",
    "From",
    "Host",
    "If-Modified-Since",
    "Last-Modified",
    "Location",
    "Pragma",
    "Range",
    "Referer",
    "Server",
    "Set-Cookie",
    "Transfer-Encoding",
    "User-Agent",
    "Www-Authenticate",
};

char *httphdrvalues[HIO_HTTP_HDR_COUNT];

typedef struct cert {
    char *name;
    int len;
    char *data;
    struct cert *next;
} cert_t;

cert_t *certList = NULL;
void *handle_table[HTTP_CLIENT_MAX ];
int http_client_start_done = 0;

struct hio_http_evt_msg {
    struct os_msg hdr;
    int event;
    struct packet *packet;
};

enum {
    HIO_HTTP_EVT_MSG = 9060,
};

/*
 * Helper functions
 */
static struct os_msg*
hio_http_event_msg(int event, struct packet *pkt)
{
    struct hio_http_evt_msg *msg;

    OS_MSG_ALLOC(msg, HIO_HTTP_EVT_MSG, 0);
    msg->event = event;
    msg->packet = pkt;
    return &msg->hdr;
}

static struct os_msg *
hio_http_event_msg_rsp(int msg_type, struct packet *pkt)
{
    struct hio_http_evt_msg *msg;

    OS_MSG_ALLOC(msg, msg_type, 0);
    msg->event  = msg_type;
    msg->packet = pkt;
    return &msg->hdr;
}

static void
hio_http_send_rsp(struct os_thread *sender, int msg_type, struct packet *pkt)
{
    os_sendmsg(sender, hio_http_event_msg_rsp(msg_type, pkt));
}

static struct packet*
hio_http_recv_rsp(int msg_type)
{
    struct os_msg *msg = os_recvmsg_type(msg_type);
    struct hio_http_evt_msg *hio_rsp = container_of(msg, struct hio_http_evt_msg, hdr);

    struct packet *pkt = NULL;
    if(hio_rsp != NULL) {
        pkt = hio_rsp->packet;
        os_free(hio_rsp);
    }

    return pkt;
}

static cert_t *
http_hio_client_get_cert(char *cert_name)
{
    cert_t *p = certList;
    while(p) {
        if(!strcmp(cert_name, p->name)) {
            return p;
        }
    }
    return p;
}

static int
http_hio_client_get_conid(void)
{
    int i;
    for(i = 0; i < HTTP_CLIENT_MAX; i++) {
        if(handle_table[i] == NULL)
            return i;
    }

    return -1;
}
static int
http_hio_client_handle_to_conid(void *handle)
{
    int i;
    for(i = 0; i < HTTP_CLIENT_MAX; i++) {
        if(handle_table[i] == handle)
            return i;
    }

    return -1;
}

static void
event_http_hio_client_start(void *sender, struct packet *msg)
{
    struct packet *rsp_pkt;
    int conid = -1, i, secured = 0;
    bool status = HTTP_SUCCESS;
    http_client_config_t cfg = {0};
    cert_t *cert;
    struct http_hio_client_start_req *req = packet_data(msg);

    os_printf("\n%s", __FUNCTION__);
    if(!http_client_start_done) {
        for(i = 0; i < HTTP_CLIENT_MAX; i++)
            handle_table[i] = NULL;
        memset(httphdrvalues, 0, sizeof(httphdrvalues));
        http_client_start_done = 1;

    }
    os_printf("\n %x %s %d", (int)req->certName, req->certName, strlen(req->certName));
    if(NULL != req->certName && strlen(req->certName)) {
        secured = 1;
        cert = http_hio_client_get_cert(req->certName);
        if(NULL == cert) {
            status = HTTP_FAILURE;
            goto error_exit;
        }
    }

    conid = http_hio_client_get_conid();
    os_printf("\n conId =  %d", conid);
    if(conid < 0) {
        status = HTTP_FAILURE;
        goto error_exit;
    }

    cfg.hostname = req->serverName;
    cfg.port = req->port;
    cfg.secured = secured;
    if(secured) {
        cfg.ssl_cfg.ca_cert.buf = cert->data;
        cfg.ssl_cfg.ca_cert.len = cert->len;
    }

    handle_table[conid] = http_client_open(&cfg);
    os_printf("\n handle_table[conid] = %x", (int)handle_table[conid]);
    if(NULL == handle_table[conid]) {
        conid =  -1;
    }
error_exit:
    rsp_pkt = create_http_hio_client_start_rsp(status, conid);
    hio_http_send_rsp(sender, HIO_HTTP_EVT_CREATE_RESP, rsp_pkt);

}

static void
hio_http_cli_resp_cb(void *ctx, http_client_resp_info_t *resp)
{
    struct packet *pkt;

    int clientId = http_hio_client_handle_to_conid(ctx);

    pkt = create_http_hio_resp_notify_ind(HTTP_SUCCESS, clientId,
                                          resp->status_code, !resp->more_data, resp->resp_len, NULL);
    if(pkt == NULL) {
        os_printf("\n pakt alloc error");
        return;
    }
    packet_append(pkt, resp->resp_body, resp->resp_len);

    os_printf("\n Sending Packet, size=: %d\n ", packet_size(pkt));
    hio_write_msg(pkt, HIO_GROUP_HTTP, HTTP_HIO_RESP_NOTIFY_IND, 0);

    return;
}

static void
event_http_hio_client_send_req(void *sender, struct packet *msg)
{
    struct packet *rsp_pkt;
    int rval, i;
    bool status = HTTP_SUCCESS;
    http_client_handle_t h_handle;
    struct http_hio_client_send_req_req *req = packet_data(msg);

    h_handle = handle_table[req->clientID];

    for(i = 0 ; i < HIO_HTTP_HDR_COUNT ; i++) {
        if(NULL != httphdrvalues[i]) {
            os_printf("\n %s : %s %s", __FUNCTION__,
                      httphdrnames[i], httphdrvalues[i]);
            http_client_set_req_hdr(h_handle, httphdrnames[i], httphdrvalues[i]);
        }
    }

    if(req->method == 1) {
        rval = http_client_get(h_handle, req->uriReq,
                               hio_http_cli_resp_cb, h_handle, 10);
        if(rval < 0)
            status = HTTP_FAILURE;
    } else if(req->method == 2) {
        rval = http_client_post(h_handle, req->uriReq, req->dataToSend,
                                req->dataLen,
                                hio_http_cli_resp_cb, h_handle, 10);
        if(rval < 0)
            status = HTTP_FAILURE;
    }

    os_printf("\nSending req resp over HIO.... status = %d", status);
    rsp_pkt = create_http_hio_client_send_req_rsp(status);
    hio_http_send_rsp(sender, HIO_HTTP_EVT_SEND_RESP, rsp_pkt);

}

static void
event_http_hio_set_client_hdr(void *sender, struct packet *msg)
{
    struct packet *rsp_pkt;
    bool status = HTTP_SUCCESS;
    int id;
    struct http_hio_set_client_hdr_req *req = packet_data(msg);

    id = req->HeaderId;
    os_printf("\n %s: %d %s", __FUNCTION__, id, httphdrvalues[id]);
    if(id > HIO_HTTP_HDR_INVAL && id <  HIO_HTTP_HDR_COUNT &&
            strlen(req->headerVal)) {
        if(httphdrvalues[id])
            os_free(httphdrvalues[id]);
        httphdrvalues[id] = os_alloc(strlen(req->headerVal) + 1);
        if(NULL == httphdrvalues[id]) {
            status = HTTP_FAILURE;
            goto error_exi;
        }
        strcpy(httphdrvalues[id], req->headerVal);
    }
error_exi:
    rsp_pkt = create_http_hio_set_client_hdr_rsp(status);
    hio_http_send_rsp(sender, HIO_HTTP_EVT_SET_HDR_RESP, rsp_pkt);
}


static void
event_http_hio_del_client_hdr(void *sender, struct packet *msg)
{
    struct packet *rsp_pkt;
    bool status = HTTP_FAILURE;
    int id;
    struct http_hio_del_client_hdr_req *req = packet_data(msg);
    id = req->HeaderId;

    if(id > HIO_HTTP_HDR_INVAL && id <  HIO_HTTP_HDR_COUNT) {
        if(NULL != httphdrvalues[id]) {
            os_free(httphdrvalues[id]);
            httphdrvalues[id] = NULL;
        }
        status = HTTP_SUCCESS;
    }
    rsp_pkt = create_http_hio_del_client_hdr_rsp(status);
    hio_http_send_rsp(sender, HIO_HTTP_EVT_DEL_HDR_RESP, rsp_pkt);
}
static void
http_hio_print_cert_list(void)
{

    cert_t *p = certList;
    os_printf("\n %s Entry", __FUNCTION__);
    while(p) {
        os_printf("\n name= %s Len = %d ",
                  p->name, p->len);
        p = p->next;
    }
    os_printf("\n %s Exit", __FUNCTION__);
}

static void
event_http_hio_store_cert(void *sender, struct packet *msg)
{
    static size_t copiedBytes = 0;    
    static cert_t *cert_receiving = NULL;
    
    struct packet *rsp_pkt;
    bool status = HTTP_SUCCESS;
    cert_t *cert;
    struct http_hio_store_cert_req *req = packet_data(msg);
    if(copiedBytes == 0){        
        if(!(cert = os_alloc(sizeof(cert_t)))) {
            status = HTTP_FAILURE;
            goto error_exit;
        }
        memset(cert, 0, sizeof(cert_t));
        if(!(cert->name = os_alloc(strlen(req->certName + 1)))) {
            status = HTTP_FAILURE;
            goto error_exit;
        }
        strcpy(cert->name, req->certName);

        if(!(cert->data = os_alloc(req->certTotalLen))) {
            status = HTTP_FAILURE;
            goto error_exit;
        }

        cert_receiving = cert;
    }else{        
        cert = cert_receiving;
        if(NULL == cert){
            os_printf("\n Error cert_receiving");
            goto error_exit;
        }
    }
    memcpy(cert->data + copiedBytes, req->certData, req->certChunkLen);
    cert->len = req->certTotalLen;
    copiedBytes += req->certChunkLen;
    if(copiedBytes >= req->certTotalLen) {
        printf("\nInserting the cert in to list");
        copiedBytes  = 0;
        cert_receiving = NULL;
        if(NULL == certList)
            certList = cert;
        else {
            cert->next = certList;
            certList = cert;
        }

        http_hio_print_cert_list();
    }
error_exit:
    rsp_pkt = create_http_hio_store_cert_rsp(status);
    hio_http_send_rsp(sender, HIO_HTTP_EVT_STORE_CERT_RESP, rsp_pkt);
}

static void
event_http_hio_delete_cert(void *sender, struct packet *msg)
{
    struct packet *rsp_pkt;
    bool status = 0;
    cert_t *p, *prev;
    struct http_hio_delete_cert_req *req = packet_data(msg);

    p = prev = certList;
    while(p) {
        if(!strcmp(req->certName, p->name)) {
            if(p == certList)
                certList = p->next;
            else {
                prev->next = p->next;
            }
            os_free(p->name);
            os_free(p->data);
            os_free(p);
            break;
        }
        prev = p;
        p = p->next;
    }
    http_hio_print_cert_list();
    rsp_pkt = create_http_hio_delete_cert_rsp(status);
    hio_http_send_rsp(sender, HIO_HTTP_EVT_DEL_CERT_RESP, rsp_pkt);
}


static void
event_http_hio_close(void *sender, struct packet *msg)
{
    struct packet *rsp_pkt;
    bool status = 0;
    http_client_handle_t h_handle;
    struct http_hio_close_req *req = packet_data(msg);
    os_printf("\n%s %d %x", __FUNCTION__, req->clientId, (int)handle_table[req->clientId]);
    h_handle = handle_table[req->clientId];
    http_client_close(h_handle);

    rsp_pkt = create_http_hio_close_rsp(status);
    hio_http_send_rsp(sender, HIO_HTTP_EVT_CLOSE_RESP, rsp_pkt);
}

/*
 * HOST INTERFACE FUNCTIONS
 */
static struct packet *
http_hio_client_start(void *ctx, struct packet *msg)
{
    int event = HIO_HTTP_EVT_CREATE_REQ;
    os_sendmsg(hio_http_thread, hio_http_event_msg(event, msg));

    return hio_http_recv_rsp(HIO_HTTP_EVT_CREATE_RESP);
}

static struct packet *
http_hio_client_send_req(void *ctx, struct packet *msg)
{
    int event = HIO_HTTP_EVT_SEND_REQ;
    os_sendmsg(hio_http_thread, hio_http_event_msg(event, msg));

    return hio_http_recv_rsp(HIO_HTTP_EVT_SEND_RESP);
}

static struct packet *
http_hio_set_client_hdr(void *ctx, struct packet *msg)
{
    int event = HIO_HTTP_EVT_SET_HDR_REQ;
    os_sendmsg(hio_http_thread, hio_http_event_msg(event, msg));

    return hio_http_recv_rsp(HIO_HTTP_EVT_SET_HDR_RESP);
}

static struct packet *
http_hio_del_client_hdr(void *ctx, struct packet *msg)
{
    int event = HIO_HTTP_EVT_DEL_HDR_REQ;
    os_sendmsg(hio_http_thread, hio_http_event_msg(event, msg));

    return hio_http_recv_rsp(HIO_HTTP_EVT_DEL_HDR_RESP);
}

static struct packet *
http_hio_store_cert(void *ctx, struct packet *msg)
{
    int event = HIO_HTTP_EVT_STORE_CERT_REQ;
    os_sendmsg(hio_http_thread, hio_http_event_msg(event, msg));

    return hio_http_recv_rsp(HIO_HTTP_EVT_STORE_CERT_RESP);
}

static struct packet *
http_hio_delete_cert(void *ctx, struct packet *msg)
{
    int event = HIO_HTTP_EVT_DEL_CERT_REQ;
    os_sendmsg(hio_http_thread, hio_http_event_msg(event, msg));

    return hio_http_recv_rsp(HIO_HTTP_EVT_DEL_CERT_RESP);
}


static struct packet *
http_hio_close(void *ctx, struct packet *msg)
{
    int event = HIO_HTTP_EVT_CLOSE_REQ;
    os_printf("\n%s %d", __FUNCTION__, event);
    os_sendmsg(hio_http_thread, hio_http_event_msg(event, msg));

    return hio_http_recv_rsp(HIO_HTTP_EVT_CLOSE_RESP);
}

/*
[[[cog
import cog, generate
generate.api(generate.Emitter(cog.out), 'http')
]]] */
static const struct hio_api http_api = {
    .group = 20,
    .num_handlers = 8,
    .handler = {
        http_hio_client_start,
        http_hio_client_send_req,
        http_hio_set_client_hdr,
        http_hio_del_client_hdr,
        NULL /* hio_resp_notify */,
        http_hio_store_cert,
        http_hio_delete_cert,
        http_hio_close,
    }
};

static void*
hio_http_thread_entry(void* arg)
{

    for(;;) {
        struct hio_http_evt_msg *hio_req;
        hio_req = (struct hio_http_evt_msg*)os_recvmsg_type(HIO_HTTP_EVT_MSG);
        int event  = hio_req->event;
        int msg_type = hio_req->hdr.msg_type;
        struct packet *pkt = hio_req->packet;
        struct os_thread *sender = os_sender(&hio_req->hdr);

        os_msg_release(&hio_req->hdr);
        os_printf("\n%s event = %d", __FUNCTION__, event);
        if(msg_type != HIO_HTTP_EVT_MSG) {
            continue;
        } else if(event == HIO_HTTP_EVT_CREATE_REQ) {
            event_http_hio_client_start(sender, pkt);
        } else if(event == HIO_HTTP_EVT_SEND_REQ) {
            event_http_hio_client_send_req(sender, pkt);
        } else if(event == HIO_HTTP_EVT_SET_HDR_REQ) {
            event_http_hio_set_client_hdr(sender, pkt);
        } else if(event == HIO_HTTP_EVT_DEL_HDR_REQ) {
            event_http_hio_del_client_hdr(sender, pkt);
        } else if(event == HIO_HTTP_EVT_STORE_CERT_REQ) {
            event_http_hio_store_cert(sender, pkt);
        } else if(event == HIO_HTTP_EVT_DEL_CERT_REQ) {
            event_http_hio_delete_cert(sender, pkt);
        } else if(event == HIO_HTTP_EVT_CLOSE_REQ) {
            event_http_hio_close(sender, pkt);
        }
    }

    return NULL;
}

static void
hio_http_create_thread(void)
{
    if(hio_http_thread_init)
        return;
    hio_http_thread_init = true;
    hio_http_thread = os_create_thread("htls", hio_http_thread_entry,
                                       NULL, 2, 4096);
}

void
hio_http_client_init(void)
{
    hio_api_init(&http_api, NULL);

    hio_http_create_thread();
}
/* [[[end]]] */
