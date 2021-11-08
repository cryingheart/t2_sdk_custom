/*
 * Definition of bt_monitor API
 *
 * Monitor interface
 */
#pragma once
#include <string.h>
#include <kernel/hostio.h>

/*
[[[cog
import cog, generate
generate.doxygen(generate.Emitter(cog.out), 'bt_monitor')
]]] */
/**
 * @addtogroup hioapi_bt_monitor
 * @{
 *
 * This interface provides functions for using the standard BT test mode.
 * Example for how to use this interface:
 *
 *     bt_start                      - the interface must be started first (only once), after that RX and TX can be performed
 *
 *     bt_set_channel frequency      - 2402..2480  (default 2402)
 *     bt_set_length length          - 0..255 (default 37)
 *     bt_set_payload payload        - 0=prbs9, 1=0x0f, 2=0x55, 4=0xff, 5=0x00 (default prbs9)
 *     bt_set_phy phy                - 1=1M, 2=2M, 3=CodedS8, 4=CodeS2 (default 1)
 *
 *     bt_test mode                  - 0=RX, 1=TX, 2=OFF
 *
 *     Example:
 *     bt_start
 *     bt_set_payload 2
 *     bt_set_phy 2
 *     bt_test 1                     - TX: freq 2402, length 37, payload 0x55, phy 2M
 *     bt_test 2                     - OFF
 *     bt_set_channel 2480
 *     bt_set_phy 1
 *     bt_test 1                     - TX: freq 2480, length 37, payload 0x55, phy 1M
 *     bt_test 2                     - OFF
 *     bt_test 0                     - RX: freq 2480, phy 1M
 *     bt_test 2                     - OFF
 *     bt_set_phy 4
 *     bt_test 0                     - RX: freq 2480, phy CodedS2
 *
 * <table>
 * <caption>Message summary (group=6)</caption>
 * <tr><th>OPERATION</th><th>REQ</th><th>RSP</th><th>IND</th></tr>
 * <tr><td>bt_start</td><td>0x00 - bt_monitor_bt_start_req</td><td>0x80 - bt_monitor_bt_start_rsp</td><td></td></tr>
 * <tr><td>bt_stop</td><td>0x01 - bt_monitor_bt_stop_req</td><td>0x81 - bt_monitor_bt_stop_rsp</td><td></td></tr>
 * <tr><td>bt_set_channel</td><td>0x02 - bt_monitor_bt_set_channel_req</td><td>0x82 - bt_monitor_bt_set_channel_rsp</td><td></td></tr>
 * <tr><td>bt_set_length</td><td>0x03 - bt_monitor_bt_set_length_req</td><td>0x83 - bt_monitor_bt_set_length_rsp</td><td></td></tr>
 * <tr><td>bt_set_payload</td><td>0x04 - bt_monitor_bt_set_payload_req</td><td>0x84 - bt_monitor_bt_set_payload_rsp</td><td></td></tr>
 * <tr><td>bt_set_phy</td><td>0x05 - bt_monitor_bt_set_phy_req</td><td>0x85 - bt_monitor_bt_set_phy_rsp</td><td></td></tr>
 * <tr><td>bt_set_tx_power</td><td>0x06 - bt_monitor_bt_set_tx_power_req</td><td>0x86 - bt_monitor_bt_set_tx_power_rsp</td><td></td></tr>
 * <tr><td>bt_test</td><td>0x07 - bt_monitor_bt_test_req</td><td>0x87 - bt_monitor_bt_test_rsp</td><td></td></tr>
 * <tr><td>bt_log</td><td>0x08 - bt_monitor_bt_log_req</td><td>0x88 - bt_monitor_bt_log_rsp</td><td></td></tr>
 * <tr><td>bt_xmit_tone</td><td>0x09 - bt_monitor_bt_xmit_tone_req</td><td>0x89 - bt_monitor_bt_xmit_tone_rsp</td><td></td></tr>
 * <tr><td>bt_xmit_stop</td><td>0x0a - bt_monitor_bt_xmit_stop_req</td><td>0x8a - bt_monitor_bt_xmit_stop_rsp</td><td></td></tr>
 * </table>
 */
/* [[[end]]] */

/*
[[[cog
cog.outl('')
generate.header(generate.Emitter(cog.out), 'bt_monitor')
cog.outl('')
]]] */

#define HIO_GROUP_BT_MONITOR 6

/*
 * Message bt_start
 */

#define BT_MONITOR_BT_START_REQ 0x00
#define BT_MONITOR_BT_START_RSP 0x80

/** Start the interface. Request structure.
 */
struct bt_monitor_bt_start_req {
};

static inline struct packet *
alloc_bt_monitor_bt_start_req(struct bt_monitor_bt_start_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_start_req(void)
{
    struct bt_monitor_bt_start_req *req;
    struct packet *pkt = alloc_bt_monitor_bt_start_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Start the interface. Response structure.
 */
struct bt_monitor_bt_start_rsp {
};

static inline struct packet *
alloc_bt_monitor_bt_start_rsp(struct bt_monitor_bt_start_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_start_rsp(void)
{
    struct bt_monitor_bt_start_rsp *rsp;
    struct packet *pkt = alloc_bt_monitor_bt_start_rsp(&rsp);
    if (pkt) {
    }
    return pkt;
}
/*
 * Message bt_stop
 */

#define BT_MONITOR_BT_STOP_REQ 0x01
#define BT_MONITOR_BT_STOP_RSP 0x81

/** Stop the interface. Request structure.
 */
struct bt_monitor_bt_stop_req {
};

static inline struct packet *
alloc_bt_monitor_bt_stop_req(struct bt_monitor_bt_stop_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_stop_req(void)
{
    struct bt_monitor_bt_stop_req *req;
    struct packet *pkt = alloc_bt_monitor_bt_stop_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Stop the interface. Response structure.
 */
struct bt_monitor_bt_stop_rsp {
};

static inline struct packet *
alloc_bt_monitor_bt_stop_rsp(struct bt_monitor_bt_stop_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_stop_rsp(void)
{
    struct bt_monitor_bt_stop_rsp *rsp;
    struct packet *pkt = alloc_bt_monitor_bt_stop_rsp(&rsp);
    if (pkt) {
    }
    return pkt;
}
/*
 * Message bt_set_channel
 */

#define BT_MONITOR_BT_SET_CHANNEL_REQ 0x02
#define BT_MONITOR_BT_SET_CHANNEL_RSP 0x82

/** Set channel (will take affect at the following mode change to RX or TX). Request structure.
 */
struct bt_monitor_bt_set_channel_req {
    uint16_t freq; /**< frequency in MHz (2402..2480) */
};

static inline struct packet *
alloc_bt_monitor_bt_set_channel_req(struct bt_monitor_bt_set_channel_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_set_channel_req(const uint16_t freq)
{
    struct bt_monitor_bt_set_channel_req *req;
    struct packet *pkt = alloc_bt_monitor_bt_set_channel_req(&req);
    if (pkt) {
        req->freq = freq;
    }
    return pkt;
}

/** Set channel (will take affect at the following mode change to RX or TX). Response structure.
 */
struct bt_monitor_bt_set_channel_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_monitor_bt_set_channel_rsp(struct bt_monitor_bt_set_channel_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_set_channel_rsp(const int32_t status)
{
    struct bt_monitor_bt_set_channel_rsp *rsp;
    struct packet *pkt = alloc_bt_monitor_bt_set_channel_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message bt_set_length
 */

#define BT_MONITOR_BT_SET_LENGTH_REQ 0x03
#define BT_MONITOR_BT_SET_LENGTH_RSP 0x83

/** Set payload length (will take affect at the following mode change to TX). Request structure.
 */
struct bt_monitor_bt_set_length_req {
    uint8_t length; /**< length of the payload (1..255) */
};

static inline struct packet *
alloc_bt_monitor_bt_set_length_req(struct bt_monitor_bt_set_length_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_set_length_req(const uint8_t length)
{
    struct bt_monitor_bt_set_length_req *req;
    struct packet *pkt = alloc_bt_monitor_bt_set_length_req(&req);
    if (pkt) {
        req->length = length;
    }
    return pkt;
}

/** Set payload length (will take affect at the following mode change to TX). Response structure.
 */
struct bt_monitor_bt_set_length_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_monitor_bt_set_length_rsp(struct bt_monitor_bt_set_length_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_set_length_rsp(const int32_t status)
{
    struct bt_monitor_bt_set_length_rsp *rsp;
    struct packet *pkt = alloc_bt_monitor_bt_set_length_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message bt_set_payload
 */

#define BT_MONITOR_BT_SET_PAYLOAD_REQ 0x04
#define BT_MONITOR_BT_SET_PAYLOAD_RSP 0x84

/** Set payload pattern (will take affect at the following mode change to TX). Request structure.
 */
struct bt_monitor_bt_set_payload_req {
    uint8_t payload; /**< payload pattern: 0 - prbs9, 1 - 0x0f, 2 - 0x55, 4 - 0xff, 5 - 0x00 */
};

static inline struct packet *
alloc_bt_monitor_bt_set_payload_req(struct bt_monitor_bt_set_payload_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_set_payload_req(const uint8_t payload)
{
    struct bt_monitor_bt_set_payload_req *req;
    struct packet *pkt = alloc_bt_monitor_bt_set_payload_req(&req);
    if (pkt) {
        req->payload = payload;
    }
    return pkt;
}

/** Set payload pattern (will take affect at the following mode change to TX). Response structure.
 */
struct bt_monitor_bt_set_payload_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_monitor_bt_set_payload_rsp(struct bt_monitor_bt_set_payload_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_set_payload_rsp(const int32_t status)
{
    struct bt_monitor_bt_set_payload_rsp *rsp;
    struct packet *pkt = alloc_bt_monitor_bt_set_payload_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message bt_set_phy
 */

#define BT_MONITOR_BT_SET_PHY_REQ 0x05
#define BT_MONITOR_BT_SET_PHY_RSP 0x85

/** Set phy (will take affect at the following mode change to RX or TX). Request structure.
 */
struct bt_monitor_bt_set_phy_req {
    uint8_t phy; /**< phy: 1 - 1M, 2 - 2M, 3 - Coded S8, 4 - Code S2 */
};

static inline struct packet *
alloc_bt_monitor_bt_set_phy_req(struct bt_monitor_bt_set_phy_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_set_phy_req(const uint8_t phy)
{
    struct bt_monitor_bt_set_phy_req *req;
    struct packet *pkt = alloc_bt_monitor_bt_set_phy_req(&req);
    if (pkt) {
        req->phy = phy;
    }
    return pkt;
}

/** Set phy (will take affect at the following mode change to RX or TX). Response structure.
 */
struct bt_monitor_bt_set_phy_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_monitor_bt_set_phy_rsp(struct bt_monitor_bt_set_phy_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_set_phy_rsp(const int32_t status)
{
    struct bt_monitor_bt_set_phy_rsp *rsp;
    struct packet *pkt = alloc_bt_monitor_bt_set_phy_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message bt_set_tx_power
 */

#define BT_MONITOR_BT_SET_TX_POWER_REQ 0x06
#define BT_MONITOR_BT_SET_TX_POWER_RSP 0x86

/** Set TX power (will take affect at the following mode change to TX). Request structure.
 */
struct bt_monitor_bt_set_tx_power_req {
    int8_t power; /**< tx_power in dBm (-127..10) (default: 0) */
};

static inline struct packet *
alloc_bt_monitor_bt_set_tx_power_req(struct bt_monitor_bt_set_tx_power_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_set_tx_power_req(const int8_t power)
{
    struct bt_monitor_bt_set_tx_power_req *req;
    struct packet *pkt = alloc_bt_monitor_bt_set_tx_power_req(&req);
    if (pkt) {
        req->power = power;
    }
    return pkt;
}

/** Set TX power (will take affect at the following mode change to TX). Response structure.
 */
struct bt_monitor_bt_set_tx_power_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_monitor_bt_set_tx_power_rsp(struct bt_monitor_bt_set_tx_power_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_set_tx_power_rsp(const int32_t status)
{
    struct bt_monitor_bt_set_tx_power_rsp *rsp;
    struct packet *pkt = alloc_bt_monitor_bt_set_tx_power_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message bt_test
 */

#define BT_MONITOR_BT_TEST_REQ 0x07
#define BT_MONITOR_BT_TEST_RSP 0x87

/** Set test mode 0=RX, 1=TX, 2=OFF. Request structure.
 */
struct bt_monitor_bt_test_req {
    uint8_t mode; /**< 0 - rx, 1 - tx, 2 - off */
};

static inline struct packet *
alloc_bt_monitor_bt_test_req(struct bt_monitor_bt_test_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_test_req(const uint8_t mode)
{
    struct bt_monitor_bt_test_req *req;
    struct packet *pkt = alloc_bt_monitor_bt_test_req(&req);
    if (pkt) {
        req->mode = mode;
    }
    return pkt;
}

/** Set test mode 0=RX, 1=TX, 2=OFF. Response structure.
 */
struct bt_monitor_bt_test_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t crc_ok; /**< received packets with correct crc */
    uint32_t crc_fail; /**< received packets with incorrect crc */
    uint32_t corr_fail; /**< receptions with no correlation */
};

static inline struct packet *
alloc_bt_monitor_bt_test_rsp(struct bt_monitor_bt_test_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_test_rsp(const int32_t status, const uint32_t crc_ok, const uint32_t crc_fail, const uint32_t corr_fail)
{
    struct bt_monitor_bt_test_rsp *rsp;
    struct packet *pkt = alloc_bt_monitor_bt_test_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->crc_ok = crc_ok;
        rsp->crc_fail = crc_fail;
        rsp->corr_fail = corr_fail;
    }
    return pkt;
}
/*
 * Message bt_log
 */

#define BT_MONITOR_BT_LOG_REQ 0x08
#define BT_MONITOR_BT_LOG_RSP 0x88

/** Enable debug log. Request structure.
 */
struct bt_monitor_bt_log_req {
    uint8_t enable; /**< 1 - enable, 0 - disable */
};

static inline struct packet *
alloc_bt_monitor_bt_log_req(struct bt_monitor_bt_log_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_log_req(const uint8_t enable)
{
    struct bt_monitor_bt_log_req *req;
    struct packet *pkt = alloc_bt_monitor_bt_log_req(&req);
    if (pkt) {
        req->enable = enable;
    }
    return pkt;
}

/** Enable debug log. Response structure.
 */
struct bt_monitor_bt_log_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_monitor_bt_log_rsp(struct bt_monitor_bt_log_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_log_rsp(const int32_t status)
{
    struct bt_monitor_bt_log_rsp *rsp;
    struct packet *pkt = alloc_bt_monitor_bt_log_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message bt_xmit_tone
 */

#define BT_MONITOR_BT_XMIT_TONE_REQ 0x09
#define BT_MONITOR_BT_XMIT_TONE_RSP 0x89

/** Start transmitting an RF tone. Request structure.
 */
struct bt_monitor_bt_xmit_tone_req {
    uint16_t freq; /**< Frequency in MHz for the transmit channel */
    uint16_t dpa_power; /**< DPA TX power */
};

static inline struct packet *
alloc_bt_monitor_bt_xmit_tone_req(struct bt_monitor_bt_xmit_tone_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_xmit_tone_req(const uint16_t freq, const uint16_t dpa_power)
{
    struct bt_monitor_bt_xmit_tone_req *req;
    struct packet *pkt = alloc_bt_monitor_bt_xmit_tone_req(&req);
    if (pkt) {
        req->freq = freq;
        req->dpa_power = dpa_power;
    }
    return pkt;
}

/** Start transmitting an RF tone. Response structure.
 */
struct bt_monitor_bt_xmit_tone_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_monitor_bt_xmit_tone_rsp(struct bt_monitor_bt_xmit_tone_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_xmit_tone_rsp(const int32_t status)
{
    struct bt_monitor_bt_xmit_tone_rsp *rsp;
    struct packet *pkt = alloc_bt_monitor_bt_xmit_tone_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message bt_xmit_stop
 */

#define BT_MONITOR_BT_XMIT_STOP_REQ 0x0a
#define BT_MONITOR_BT_XMIT_STOP_RSP 0x8a

/** Stop an ongoing transmission started with xmit_cop. Request structure.
 */
struct bt_monitor_bt_xmit_stop_req {
};

static inline struct packet *
alloc_bt_monitor_bt_xmit_stop_req(struct bt_monitor_bt_xmit_stop_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_xmit_stop_req(void)
{
    struct bt_monitor_bt_xmit_stop_req *req;
    struct packet *pkt = alloc_bt_monitor_bt_xmit_stop_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Stop an ongoing transmission started with xmit_cop. Response structure.
 */
struct bt_monitor_bt_xmit_stop_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_bt_monitor_bt_xmit_stop_rsp(struct bt_monitor_bt_xmit_stop_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_bt_monitor_bt_xmit_stop_rsp(const int32_t status)
{
    struct bt_monitor_bt_xmit_stop_rsp *rsp;
    struct packet *pkt = alloc_bt_monitor_bt_xmit_stop_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/* [[[end]]] */
/** @} */
