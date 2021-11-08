/*
[[[cog
import cog, generate
cog.outl('')
generate.header(generate.Emitter(cog.out), 'http')
cog.outl('')
]]] */

#define HIO_GROUP_HTTP 20

/*
 * Message hio_client_start
 */

#define HTTP_HIO_CLIENT_START_REQ 0x00
#define HTTP_HIO_CLIENT_START_RSP 0x80

/** http client start. Request structure.
 */
struct http_hio_client_start_req {
    char serverName[64]; /**< Remote Server Name/ IP address */
    uint32_t port; /**< Server Port */
    char certName[32]; /**< Cert Name */
};

static inline struct packet *
alloc_http_hio_client_start_req(struct http_hio_client_start_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_client_start_req(const char serverName[64], const uint32_t port, const char certName[32])
{
    struct http_hio_client_start_req *req;
    struct packet *pkt = alloc_http_hio_client_start_req(&req);
    if (pkt) {
        strncpy(req->serverName, serverName, 64);
        req->port = port;
        strncpy(req->certName, certName, 32);
    }
    return pkt;
}

/** http client start. Response structure.
 */
struct http_hio_client_start_rsp {
    uint32_t status; /**< result status, zero is success */
    uint32_t ClientID; /**< HTTP client identifier */
};

static inline struct packet *
alloc_http_hio_client_start_rsp(struct http_hio_client_start_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_client_start_rsp(const uint32_t status, const uint32_t ClientID)
{
    struct http_hio_client_start_rsp *rsp;
    struct packet *pkt = alloc_http_hio_client_start_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->ClientID = ClientID;
    }
    return pkt;
}
/*
 * Message hio_client_send_req
 */

#define HTTP_HIO_CLIENT_SEND_REQ_REQ 0x01
#define HTTP_HIO_CLIENT_SEND_REQ_RSP 0x81

/** http client send request. Request structure.
 */
struct http_hio_client_send_req_req {
    uint32_t clientID; /**< HTTP client identifier */
    uint32_t method; /**< Method: 1-Get 2-Post  */
    char uriReq[64]; /**< URI of the file to Get/Post  */
    uint32_t dataLen; /**< Number of bytes to send  */
    char dataToSend[0]; /**< Data to Send  */
};

static inline struct packet *
alloc_http_hio_client_send_req_req(struct http_hio_client_send_req_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_client_send_req_req(const uint32_t clientID, const uint32_t method, const char uriReq[64], const uint32_t dataLen, const char dataToSend[0])
{
    struct http_hio_client_send_req_req *req;
    struct packet *pkt = alloc_http_hio_client_send_req_req(&req);
    if (pkt) {
        req->clientID = clientID;
        req->method = method;
        strncpy(req->uriReq, uriReq, 64);
        req->dataLen = dataLen;
        strncpy(req->dataToSend, dataToSend, 0);
    }
    return pkt;
}

/** http client send request. Response structure.
 */
struct http_hio_client_send_req_rsp {
    uint32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_http_hio_client_send_req_rsp(struct http_hio_client_send_req_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_client_send_req_rsp(const uint32_t status)
{
    struct http_hio_client_send_req_rsp *rsp;
    struct packet *pkt = alloc_http_hio_client_send_req_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message hio_set_client_hdr
 */

#define HTTP_HIO_SET_CLIENT_HDR_REQ 0x02
#define HTTP_HIO_SET_CLIENT_HDR_RSP 0x82

/** Set http client header. Request structure.
 */
struct http_hio_set_client_hdr_req {
    uint32_t HeaderId; /**< HTTP header Id */
    char headerVal[32]; /**< HTTP header Value */
};

static inline struct packet *
alloc_http_hio_set_client_hdr_req(struct http_hio_set_client_hdr_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_set_client_hdr_req(const uint32_t HeaderId, const char headerVal[32])
{
    struct http_hio_set_client_hdr_req *req;
    struct packet *pkt = alloc_http_hio_set_client_hdr_req(&req);
    if (pkt) {
        req->HeaderId = HeaderId;
        strncpy(req->headerVal, headerVal, 32);
    }
    return pkt;
}

/** Set http client header. Response structure.
 */
struct http_hio_set_client_hdr_rsp {
    uint32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_http_hio_set_client_hdr_rsp(struct http_hio_set_client_hdr_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_set_client_hdr_rsp(const uint32_t status)
{
    struct http_hio_set_client_hdr_rsp *rsp;
    struct packet *pkt = alloc_http_hio_set_client_hdr_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message hio_del_client_hdr
 */

#define HTTP_HIO_DEL_CLIENT_HDR_REQ 0x03
#define HTTP_HIO_DEL_CLIENT_HDR_RSP 0x83

/** Delete http client header. Request structure.
 */
struct http_hio_del_client_hdr_req {
    uint32_t HeaderId; /**< http header Id */
};

static inline struct packet *
alloc_http_hio_del_client_hdr_req(struct http_hio_del_client_hdr_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_del_client_hdr_req(const uint32_t HeaderId)
{
    struct http_hio_del_client_hdr_req *req;
    struct packet *pkt = alloc_http_hio_del_client_hdr_req(&req);
    if (pkt) {
        req->HeaderId = HeaderId;
    }
    return pkt;
}

/** Delete http client header. Response structure.
 */
struct http_hio_del_client_hdr_rsp {
    uint32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_http_hio_del_client_hdr_rsp(struct http_hio_del_client_hdr_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_del_client_hdr_rsp(const uint32_t status)
{
    struct http_hio_del_client_hdr_rsp *rsp;
    struct packet *pkt = alloc_http_hio_del_client_hdr_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message hio_resp_notify
 */

#define HTTP_HIO_RESP_NOTIFY_IND 0xc4

/** http response notification. Indication structure.
 */
struct http_hio_resp_notify_ind {
    uint32_t status; /**< result status, zero is success */
    uint32_t clientID; /**< HTTP client identifier */
    uint32_t respStatus; /**< HTTP Response Status */
    uint32_t endOfResp; /**< HTTP Response end */
    uint32_t respLen; /**< HTTP Response Len */
    char respData[0]; /**< Response Notification data */
};

static inline struct packet *
alloc_http_hio_resp_notify_ind(struct http_hio_resp_notify_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_resp_notify_ind(const uint32_t status, const uint32_t clientID, const uint32_t respStatus, const uint32_t endOfResp, const uint32_t respLen, const char respData[0])
{
    struct http_hio_resp_notify_ind *ind;
    struct packet *pkt = alloc_http_hio_resp_notify_ind(&ind);
    if (pkt) {
        ind->status = status;
        ind->clientID = clientID;
        ind->respStatus = respStatus;
        ind->endOfResp = endOfResp;
        ind->respLen = respLen;
        strncpy(ind->respData, respData, 0);
    }
    return pkt;
}
/*
 * Message hio_store_cert
 */

#define HTTP_HIO_STORE_CERT_REQ 0x05
#define HTTP_HIO_STORE_CERT_RSP 0x85

/** Store cert. Request structure.
 */
struct http_hio_store_cert_req {
    char certName[64]; /**< Certificate Name */
    uint32_t certTotalLen; /**< Certificate Total Length */
    uint32_t certChunkLen; /**< Length of the current chunk */
    char certData[0]; /**< Certificate Data */
};

static inline struct packet *
alloc_http_hio_store_cert_req(struct http_hio_store_cert_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_store_cert_req(const char certName[64], const uint32_t certTotalLen, const uint32_t certChunkLen, const char certData[0])
{
    struct http_hio_store_cert_req *req;
    struct packet *pkt = alloc_http_hio_store_cert_req(&req);
    if (pkt) {
        strncpy(req->certName, certName, 64);
        req->certTotalLen = certTotalLen;
        req->certChunkLen = certChunkLen;
        strncpy(req->certData, certData, 0);
    }
    return pkt;
}

/** Store cert. Response structure.
 */
struct http_hio_store_cert_rsp {
    uint32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_http_hio_store_cert_rsp(struct http_hio_store_cert_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_store_cert_rsp(const uint32_t status)
{
    struct http_hio_store_cert_rsp *rsp;
    struct packet *pkt = alloc_http_hio_store_cert_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message hio_delete_cert
 */

#define HTTP_HIO_DELETE_CERT_REQ 0x06
#define HTTP_HIO_DELETE_CERT_RSP 0x86

/** Delete cert. Request structure.
 */
struct http_hio_delete_cert_req {
    char certName[64]; /**< Certificate Name */
};

static inline struct packet *
alloc_http_hio_delete_cert_req(struct http_hio_delete_cert_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_delete_cert_req(const char certName[64])
{
    struct http_hio_delete_cert_req *req;
    struct packet *pkt = alloc_http_hio_delete_cert_req(&req);
    if (pkt) {
        strncpy(req->certName, certName, 64);
    }
    return pkt;
}

/** Delete cert. Response structure.
 */
struct http_hio_delete_cert_rsp {
    uint32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_http_hio_delete_cert_rsp(struct http_hio_delete_cert_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_delete_cert_rsp(const uint32_t status)
{
    struct http_hio_delete_cert_rsp *rsp;
    struct packet *pkt = alloc_http_hio_delete_cert_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message hio_close
 */

#define HTTP_HIO_CLOSE_REQ 0x07
#define HTTP_HIO_CLOSE_RSP 0x87

/** close the connection. Request structure.
 */
struct http_hio_close_req {
    uint32_t clientId; /**< clientId */
};

static inline struct packet *
alloc_http_hio_close_req(struct http_hio_close_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_close_req(const uint32_t clientId)
{
    struct http_hio_close_req *req;
    struct packet *pkt = alloc_http_hio_close_req(&req);
    if (pkt) {
        req->clientId = clientId;
    }
    return pkt;
}

/** close the connection. Response structure.
 */
struct http_hio_close_rsp {
    uint32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_http_hio_close_rsp(struct http_hio_close_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_http_hio_close_rsp(const uint32_t status)
{
    struct http_hio_close_rsp *rsp;
    struct packet *pkt = alloc_http_hio_close_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/* [[[end]]] */
