#pragma once
#include <kernel/hostio.h>
#include <string.h>

/*
[[[cog
import cog, generate
generate.doxygen(generate.Emitter(cog.out), 'bt')
]]] */
/**
 * @addtogroup hioapi_bt
 * @{
 *
 * This interface is used to control BT via HCI.
 * Two formats are supported: "hci" - the standard hci format, "explicit" - user friendly with more obvious parameters.
 *
 *     Sending HCI commands
 *     E.g. hci_reset
 *     hci 1,3,12,0               <-- hci: packet indicator "1", opcode "3,12(=3<<4)", length "0"
 *     cmd 3 3 0 0,               <-- explicit: ocf "3", ogf "3", length "0", parameters "0,"
 *
 *     Sending ACL data:
 *     hci 2,0,0,5,0,0,1,2,3,4,   <-- hci: pi "2", handle "0,0", length "5,0", data "0,1,2,3,4,"
 *     dat 0 0 0 5 0,1,2,3,4,     <-- explicit: handle "0", pbf "0", bcf "0, length "5", data "0,1,2,3,4,"
 *
 *     Receiving HCI events and ACL data:
 *     The "hci" format is used until the first HCI command is sent. The format of the first
 *     HCI command will then be used for all HCI events and ACL data.
 *
 * <table>
 * <caption>Message summary (group=4)</caption>
 * <tr><th>OPERATION</th><th>REQ</th><th>RSP</th><th>IND</th></tr>
 * <tr><td>hci</td><td>0x00 - bt_hci_req</td><td>0x80 - bt_hci_rsp</td><td>0xc0 - bt_hci_ind</td></tr>
 * <tr><td>cmd</td><td>0x01 - bt_cmd_req</td><td>0x81 - bt_cmd_rsp</td><td>0xc1 - bt_cmd_ind</td></tr>
 * <tr><td>dat</td><td>0x02 - bt_dat_req</td><td>0x82 - bt_dat_rsp</td><td>0xc2 - bt_dat_ind</td></tr>
 * </table>
 */
/* [[[end]]] */

/*
[[[cog
cog.outl('')
generate.header(generate.Emitter(cog.out), 'bt')
cog.outl('')
]]] */

#define HIO_GROUP_BT 4

/*
 * Message hci
 */

#define BT_HCI_REQ 0x00
#define BT_HCI_RSP 0x80
#define BT_HCI_IND 0xc0

/** Send command/data (req) and receive event/data (rsp, ind) - hci format. Request structure.
 */
struct bt_hci_req {
    uint8_t payload[0]; /**< hci command or acl data */
};

static inline struct packet *
alloc_bt_hci_req(struct bt_hci_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_hci_req(const uint8_t payload[0])
{
    struct bt_hci_req *req;
    struct packet *pkt = alloc_bt_hci_req(&req);
    if (pkt) {
        /* payload must be set by the caller */
    }
    return pkt;
}

/** Send command/data (req) and receive event/data (rsp, ind) - hci format. Response structure.
 */
struct bt_hci_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_hci_rsp(struct bt_hci_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_hci_rsp(const int32_t status)
{
    struct bt_hci_rsp *rsp;
    struct packet *pkt = alloc_bt_hci_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** Send command/data (req) and receive event/data (rsp, ind) - hci format. Indication structure.
 */
struct bt_hci_ind {
    uint8_t payload[0]; /**< hci event  or acl data */
};

static inline struct packet *
alloc_bt_hci_ind(struct bt_hci_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_hci_ind(const uint8_t payload[0])
{
    struct bt_hci_ind *ind;
    struct packet *pkt = alloc_bt_hci_ind(&ind);
    if (pkt) {
        /* payload must be set by the caller */
    }
    return pkt;
}
/*
 * Message cmd
 */

#define BT_CMD_REQ 0x01
#define BT_CMD_RSP 0x81
#define BT_CMD_IND 0xc1

/** Send HCI command (req) and receive HCI event (rsb, ind) - explicit format. Request structure.
 */
struct bt_cmd_req {
    uint8_t ocf; /**< opcode command field */
    uint8_t ogf; /**< opcode group field */
    uint8_t len; /**< parameter length (0..255) */
    uint8_t param[0]; /**< parameters */
};

static inline struct packet *
alloc_bt_cmd_req(struct bt_cmd_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_cmd_req(const uint8_t ocf, const uint8_t ogf, const uint8_t len, const uint8_t param[0])
{
    struct bt_cmd_req *req;
    struct packet *pkt = alloc_bt_cmd_req(&req);
    if (pkt) {
        req->ocf = ocf;
        req->ogf = ogf;
        req->len = len;
        /* param must be set by the caller */
    }
    return pkt;
}

/** Send HCI command (req) and receive HCI event (rsb, ind) - explicit format. Response structure.
 */
struct bt_cmd_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_cmd_rsp(struct bt_cmd_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_cmd_rsp(const int32_t status)
{
    struct bt_cmd_rsp *rsp;
    struct packet *pkt = alloc_bt_cmd_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** Send HCI command (req) and receive HCI event (rsb, ind) - explicit format. Indication structure.
 */
struct bt_cmd_ind {
    uint8_t code; /**< event code */
    uint8_t len; /**< parameter length (0..255) */
    uint8_t param[0]; /**< parameters */
};

static inline struct packet *
alloc_bt_cmd_ind(struct bt_cmd_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_cmd_ind(const uint8_t code, const uint8_t len, const uint8_t param[0])
{
    struct bt_cmd_ind *ind;
    struct packet *pkt = alloc_bt_cmd_ind(&ind);
    if (pkt) {
        ind->code = code;
        ind->len = len;
        /* param must be set by the caller */
    }
    return pkt;
}
/*
 * Message dat
 */

#define BT_DAT_REQ 0x02
#define BT_DAT_RSP 0x82
#define BT_DAT_IND 0xc2

/** Send ACL data (req) and receive ACL data (ind) - explicit format. Request structure.
 */
struct bt_dat_req {
    uint8_t handle; /**< connection handle */
    uint8_t pbf; /**< packet boundary flags: 0-start, 1-cont */
    uint8_t bcf; /**< broadcast flags: 0-point-to-point (no broadcast) */
    uint8_t len; /**< data length (0..255) */
    uint8_t data[0]; /**< data */
};

static inline struct packet *
alloc_bt_dat_req(struct bt_dat_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_dat_req(const uint8_t handle, const uint8_t pbf, const uint8_t bcf, const uint8_t len, const uint8_t data[0])
{
    struct bt_dat_req *req;
    struct packet *pkt = alloc_bt_dat_req(&req);
    if (pkt) {
        req->handle = handle;
        req->pbf = pbf;
        req->bcf = bcf;
        req->len = len;
        /* data must be set by the caller */
    }
    return pkt;
}

/** Send ACL data (req) and receive ACL data (ind) - explicit format. Response structure.
 */
struct bt_dat_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_dat_rsp(struct bt_dat_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_dat_rsp(const int32_t status)
{
    struct bt_dat_rsp *rsp;
    struct packet *pkt = alloc_bt_dat_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** Send ACL data (req) and receive ACL data (ind) - explicit format. Indication structure.
 */
struct bt_dat_ind {
    uint8_t handle; /**< connection handle */
    uint8_t pbf; /**< packet boundary flags: 0-start, 1-cont */
    uint8_t bcf; /**< broadcast flags: 0-point-to-point (no broadcast) */
    uint8_t len; /**< data length (0..255) */
    uint8_t data[0]; /**< data */
};

static inline struct packet *
alloc_bt_dat_ind(struct bt_dat_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_bt_dat_ind(const uint8_t handle, const uint8_t pbf, const uint8_t bcf, const uint8_t len, const uint8_t data[0])
{
    struct bt_dat_ind *ind;
    struct packet *pkt = alloc_bt_dat_ind(&ind);
    if (pkt) {
        ind->handle = handle;
        ind->pbf = pbf;
        ind->bcf = bcf;
        ind->len = len;
        /* data must be set by the caller */
    }
    return pkt;
}

/* [[[end]]] */
/** @} */
