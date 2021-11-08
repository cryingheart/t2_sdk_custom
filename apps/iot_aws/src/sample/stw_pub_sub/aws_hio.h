#include <kernel/hostio.h>
#include <string.h>


#define HIO_GROUP_AWS 15

/*
 * AWS connect
 */

#define AWS_CONNECT_REQ 0x00
#define AWS_CONNECT_RSP 0x80

struct aws_connect_req {
    char aws_host[128];
    uint16_t port;
};

static inline struct packet *
alloc_aws_connect_req(struct aws_connect_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_aws_connect_req(void)
{
    struct aws_connect_req *req;
    struct packet *pkt = alloc_aws_connect_req(&req);
    if (pkt) {
    }
    return pkt;
}

struct aws_connect_rsp {
    uint32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_aws_connect_rsp(struct aws_connect_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_aws_connect_rsp(const uint32_t status)
{
    struct aws_connect_rsp *rsp;
    struct packet *pkt = alloc_aws_connect_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/*
 * AWS publish
 */

#define AWS_PUBLISH_REQ 0x01
#define AWS_PUBLISH_RSP 0x81

struct aws_publish_req {
    char topic[128]; // char array for publish topic
    int msg_len; // length of msg
    char msg[0]; // char array for msg
};

static inline struct packet *
alloc_aws_publish_req(struct aws_publish_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_aws_publish_req(void)
{
    struct aws_publish_req *req;
    struct packet *pkt = alloc_aws_publish_req(&req);
    if (pkt) {
    }
    return pkt;
}

struct aws_publish_rsp {
    uint32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_aws_publish_rsp(struct aws_publish_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_aws_publish_rsp(const uint32_t status)
{
    struct aws_publish_rsp *rsp;
    struct packet *pkt = alloc_aws_publish_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/*
 * AWS subscribe
 */

#define AWS_SUBSCRIBE_REQ 0x02
#define AWS_SUBSCRIBE_RSP 0x82
#define AWS_SUBSCRIBE_IND 0xC2

struct aws_subscribe_req {
    char topic[128]; // char array for subscribe topic
};

static inline struct packet *
alloc_aws_subscribe_req(struct aws_subscribe_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_aws_subscribe_req(void)
{
    struct aws_subscribe_req *req;
    struct packet *pkt = alloc_aws_subscribe_req(&req);
    if (pkt) {
    }
    return pkt;
}

struct aws_subscribe_rsp {
    uint32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_aws_subscribe_rsp(struct aws_subscribe_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_aws_subscribe_rsp(const uint32_t status)
{
    struct aws_subscribe_rsp *rsp;
    struct packet *pkt = alloc_aws_subscribe_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

struct aws_subscribe_ind {
    uint32_t msg_len;
    char msg[0];       
};

static inline struct packet *
alloc_aws_subscribe_ind(struct aws_subscribe_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_aws_subscribe_ind(uint32_t msg_len, char *msg)
{
    struct aws_subscribe_ind *ind;
    struct packet *pkt = alloc_aws_subscribe_ind(&ind);
    if (pkt) {
        ind->msg_len = msg_len;
    }
    return pkt;
}

/*
 * AWS setinterval
 */

#define AWS_SETINTERVAL_REQ 0x03
#define AWS_SETINTERVAL_RSP 0x83

struct aws_setinterval_req {
    uint32_t interval; // mqtt yield interval in ms
};

static inline struct packet *
alloc_aws_setinterval_req(struct aws_setinterval_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_aws_setinterval_req(void)
{
    struct aws_setinterval_req *req;
    struct packet *pkt = alloc_aws_setinterval_req(&req);
    if (pkt) {
    }
    return pkt;
}

struct aws_setinterval_rsp {
    uint32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_aws_setinterval_rsp(struct aws_setinterval_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_aws_setinterval_rsp(const uint32_t status)
{
    struct aws_setinterval_rsp *rsp;
    struct packet *pkt = alloc_aws_setinterval_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/*
 * AWS getclientstate
 */

#define AWS_GETCLIENTSTATE_REQ 0x04
#define AWS_GETCLIENTSTATE_RSP 0x84

struct aws_getclientstate_req {
};

static inline struct packet *
alloc_aws_getclientstate_req(struct aws_getclientstate_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_aws_getclientstate_req(void)
{
    struct aws_getclientstate_req *req;
    struct packet *pkt = alloc_aws_getclientstate_req(&req);
    if (pkt) {
    }
    return pkt;
}

struct aws_getclientstate_rsp {
    uint32_t status; /**< result status, zero is success */
    uint32_t clientstate;
};

static inline struct packet *
alloc_aws_getclientstate_rsp(struct aws_getclientstate_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_aws_getclientstate_rsp(const uint32_t clientstate)
{
    struct aws_getclientstate_rsp *rsp;
    struct packet *pkt = alloc_aws_getclientstate_rsp(&rsp);
    if (pkt) {
        rsp->status = 0;
        rsp->clientstate = clientstate;
    }
    return pkt;
}
