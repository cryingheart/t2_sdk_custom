/*
 * PWM API specificatoin
 */
#pragma once
#include <string.h>
#include <kernel/hostio.h>

/*
[[[cog
import cog, generate
cog.outl('')
generate.header(generate.Emitter(cog.out), 'pwm')
cog.outl('')
]]] */

#define HIO_GROUP_PWM 3

/*
 * Message enable
 */

#define PWM_ENABLE_REQ 0x00
#define PWM_ENABLE_RSP 0x80

struct pwm_enable_req {
    uint16_t rate;
    uint8_t i2s;
    uint8_t flags;
};

static inline struct packet *
alloc_pwm_enable_req(struct pwm_enable_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_pwm_enable_req(const uint16_t rate, const uint8_t i2s, const uint8_t flags)
{
    struct pwm_enable_req *req;
    struct packet *pkt = alloc_pwm_enable_req(&req);
    if (pkt) {
        req->rate = rate;
        req->i2s = i2s;
        req->flags = flags;
    }
    return pkt;
}

struct pwm_enable_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_pwm_enable_rsp(struct pwm_enable_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_pwm_enable_rsp(const int32_t status)
{
    struct pwm_enable_rsp *rsp;
    struct packet *pkt = alloc_pwm_enable_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message disable
 */

#define PWM_DISABLE_REQ 0x01
#define PWM_DISABLE_RSP 0x81

struct pwm_disable_req {
};

static inline struct packet *
alloc_pwm_disable_req(struct pwm_disable_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_pwm_disable_req(void)
{
    struct pwm_disable_req *req;
    struct packet *pkt = alloc_pwm_disable_req(&req);
    if (pkt) {
    }
    return pkt;
}

struct pwm_disable_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_pwm_disable_rsp(struct pwm_disable_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_pwm_disable_rsp(const int32_t status)
{
    struct pwm_disable_rsp *rsp;
    struct packet *pkt = alloc_pwm_disable_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message write
 */

#define PWM_WRITE_REQ 0x02
#define PWM_WRITE_RSP 0x82
#define PWM_WRITE_IND 0xc2

struct pwm_write_req {
    int16_t data[448];
};

static inline struct packet *
alloc_pwm_write_req(struct pwm_write_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_pwm_write_req(const int16_t data[448])
{
    struct pwm_write_req *req;
    struct packet *pkt = alloc_pwm_write_req(&req);
    if (pkt) {
        memcpy(req->data, data, sizeof req->data);
    }
    return pkt;
}

struct pwm_write_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_pwm_write_rsp(struct pwm_write_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_pwm_write_rsp(const int32_t status)
{
    struct pwm_write_rsp *rsp;
    struct packet *pkt = alloc_pwm_write_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

struct pwm_write_ind {
    uint32_t reserved;
};

static inline struct packet *
alloc_pwm_write_ind(struct pwm_write_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_pwm_write_ind(void)
{
    struct pwm_write_ind *ind;
    struct packet *pkt = alloc_pwm_write_ind(&ind);
    if (pkt) {
        ind->reserved = 0;
    }
    return pkt;
}

/* [[[end]]] */
