/** @file */
/**
 * @defgroup hioapi Host Interface Protocol Specification
 * @{
 *
 * The Host Interface Protocol is a frame based protocol between a host and the
 * Talaria TWO device. This protocol can be used over UART or SPI transports.
 * The framing differs slightly for SPI. Details on the additional framing
 * mandated by the @Talaria slave SPI interface is described in detail in the
 * document <em>"Talaria TWO - Host SPI Interface"</em>.
 *
 * There are three types of messages. A request is sent from the host to the
 * device. Most requests are followed by a response message from the device to
 * the host -- but there are a few exceptions. An indication is also sent from
 * the device to the host, but differs in that they convey asynchronous
 * information, such as WiFi scan results or device wake-up notifications.
 *
 * All messages in the following sections are described using C structures.
 * Some messages does not carry and additional parameters to the group and
 * message ID. These requests will have an empty struct definition in the
 * documentation.
 *
 * Messages should be encoded as binary representations of these structures as
 * they would appear to a little endian ARM CPU.
 *
 * All requests are preceded by a header:
 *
 *     struct hio_msghdr {
 *         uint8_t  group;
 *         uint8_t  msgid;
 *         uint16_t trxid;
 *     };
 *
 * The \a group and \a msgid is the message identifier, and \a trxid is a
 * number that can be used by the host to sequence requests (it's just returned
 * in response messages).
 *
 * The message header and body is preceded by a two byte (little endian) length
 * of the header and body (excluding the length).
 *
 * @xmlonly <asciiart title="Host inteface message framing">

   |------ 2 -------|-- 1 ---|-- 1 ---|------ 2 -------|

   +----------------+--------+--------+----------------+------------- - - - -------------+
   |     LENGTH     | GROUP  | MSGID  | TRXID          | PAYLOAD                         |
   +----------------+--------+--------+----------------+------------- - - - -------------+

                    |----------------------------- (LENGTH) -----------------------------|

 * </asciiart> @endxmlonly
 *
 * The normal operation is to first issue a @ref hio_query_req message. The
 * response to this will include a table of which groups and messages are
 * supported by the device (in the form of @ref hio_query_rsp). It also
 * includes the max request size the device can handle.
 *
 * The host may then configure interrupt and wakeup pins with the @ref
 * hio_config_req message.
 *
 * Interrupts can be configured with the hio_config_req message.  Interrupts
 * can be either edge or level triggered. Edge triggered interrupts will toggle
 * the interupt pin for each new message being queued on the device, while
 * level triggered interrupts will drive the pin high when there are queued
 * messages.
 *
 * When using level triggered interrupts the hio_irqack_req message is
 * used to clear the interrupt condition. This is not necessary for edge
 * type interrupts.
 *
 * When the host has no more pending requests, it may inform the device that it
 * is safe to go into power down mode. This is done with hio_sleep_req message;
 * there is no response to this message. Sending this message doesn't mean that
 * the device will immediately go into power save, but after issuing this
 * request the host must wake the device up via the wakeup pin.
 *
 * Wake-up of the device is performed by pulling the configured wake-up pin.
 * The level to signal device wakeup is also configurable (wakeup_level in the
 * @ref hio_config_req). The recommended setting to use is a low signal to
 * signal wakeup.
 *
 * @defgroup hioapi_hio        Control
 * @defgroup hioapi_wcm        WIFI Connection Manager
 * @defgroup hioapi_bt         Bluetooth
 * @defgroup hioapi_sock       Sockets
 * @defgroup hioapi_tls        TLS Sockets
 * @defgroup hioapi_bt_host    Bluetooth GAP/GATT
 * @defgroup hioapi_mdns       mDNS
 * @defgroup hioapi_debug      Debug
 * @internal
 * @defgroup hioapi_monitor    Monitor
 * @defgroup hioapi_bt_monitor Bluetooth Monitor
 * @endinternal
 *
 * @}
 */
#pragma once
#include <stdint.h>
#include <string.h>
#include <kernel/hostio.h>

/*
[[[cog
import cog, generate
generate.doxygen(generate.Emitter(cog.out), 'hio')
]]] */
/**
 * @addtogroup hioapi_hio
 * @{
 *
 * This message group deals with control type messages, such as
 * capability discovery, and device configuration.
 *
 * <table>
 * <caption>Message summary (group=0)</caption>
 * <tr><th>OPERATION</th><th>REQ</th><th>RSP</th><th>IND</th></tr>
 * <tr><td>query</td><td>0x00 - hio_query_req</td><td>0x80 - hio_query_rsp</td><td></td></tr>
 * <tr><td>config</td><td>0x01 - hio_config_req</td><td>0x81 - hio_config_rsp</td><td></td></tr>
 * <tr><td>sleep</td><td>0x02 - hio_sleep_req</td><td></td><td></td></tr>
 * <tr><td>irqack</td><td>0x03 - hio_irqack_req</td><td></td><td></td></tr>
 * <tr><td>ping</td><td>0x04 - hio_ping_req</td><td>0x84 - hio_ping_rsp</td><td></td></tr>
 * <tr><td>wakeup</td><td></td><td></td><td>0xc5 - hio_wakeup_ind</td></tr>
 * <tr><td>console</td><td></td><td></td><td>0xc6 - hio_console_ind</td></tr>
 * </table>
 */
/* [[[end]]] */

/*
[[[cog
cog.outl('')
generate.header(generate.Emitter(cog.out), 'hio')
cog.outl('')
]]] */

#define HIO_GROUP_HIO 0

/*
 * Message query
 */

#define HIO_QUERY_REQ 0x00
#define HIO_QUERY_RSP 0x80

/** Query device about available procedures. Request structure.
 */
struct hio_query_req {
};

static inline struct packet *
alloc_hio_query_req(struct hio_query_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_hio_query_req(void)
{
    struct hio_query_req *req;
    struct packet *pkt = alloc_hio_query_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Query device about available procedures. Response structure.
 */
struct hio_query_rsp {
    int32_t status; /**< result status, zero is success */
    uint8_t version; /**< protocol version */
    uint8_t ngroups; /**< length of nmsg vector */
    uint16_t maxsize; /**< max request size */
    char fw_rev[10]; /**< Firware revision string */
    char patch_rev[10]; /**< ROM patch revision string */
    uint16_t nmsg[0]; /**< supported hostio interfaces, coded as 256*group + num_api */
};

static inline struct packet *
alloc_hio_query_rsp(struct hio_query_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_hio_query_rsp(const int32_t status, const uint8_t version, const uint8_t ngroups, const uint16_t maxsize, const char fw_rev[10], const char patch_rev[10], const uint16_t nmsg[0])
{
    struct hio_query_rsp *rsp;
    struct packet *pkt = alloc_hio_query_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->version = version;
        rsp->ngroups = ngroups;
        rsp->maxsize = maxsize;
        strncpy(rsp->fw_rev, fw_rev, 10);
        strncpy(rsp->patch_rev, patch_rev, 10);
        /* nmsg must be set by the caller */
    }
    return pkt;
}
/*
 * Message config
 */

#define HIO_CONFIG_REQ 0x01
#define HIO_CONFIG_RSP 0x81

/** Configure wake-up and interrupt pins. Request structure.
 */
struct hio_config_req {
    uint8_t wakeup_pin; /**< GPIO pin to use for wakeup, zero to disable */
    uint8_t wakeup_level; /**< wakeup level high(1) or low(0) */
    uint8_t irq_pin; /**< GPIO pin to use for interrupt */
    uint8_t irq_mode; /**< Interrupt mode to use: none(0), high(1), or toggle(2) */
};

static inline struct packet *
alloc_hio_config_req(struct hio_config_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_hio_config_req(const uint8_t wakeup_pin, const uint8_t wakeup_level, const uint8_t irq_pin, const uint8_t irq_mode)
{
    struct hio_config_req *req;
    struct packet *pkt = alloc_hio_config_req(&req);
    if (pkt) {
        req->wakeup_pin = wakeup_pin;
        req->wakeup_level = wakeup_level;
        req->irq_pin = irq_pin;
        req->irq_mode = irq_mode;
    }
    return pkt;
}

/** Configure wake-up and interrupt pins. Response structure.
 */
struct hio_config_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_hio_config_rsp(struct hio_config_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_hio_config_rsp(const int32_t status)
{
    struct hio_config_rsp *rsp;
    struct packet *pkt = alloc_hio_config_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message sleep
 */

#define HIO_SLEEP_REQ 0x02

/** Indicate that device may go to sleep. Request structure.
 */
struct hio_sleep_req {
};

static inline struct packet *
alloc_hio_sleep_req(struct hio_sleep_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_hio_sleep_req(void)
{
    struct hio_sleep_req *req;
    struct packet *pkt = alloc_hio_sleep_req(&req);
    if (pkt) {
    }
    return pkt;
}
/*
 * Message irqack
 */

#define HIO_IRQACK_REQ 0x03

/** Acknowledge level interrupt. Request structure.
 */
struct hio_irqack_req {
};

static inline struct packet *
alloc_hio_irqack_req(struct hio_irqack_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_hio_irqack_req(void)
{
    struct hio_irqack_req *req;
    struct packet *pkt = alloc_hio_irqack_req(&req);
    if (pkt) {
    }
    return pkt;
}
/*
 * Message ping
 */

#define HIO_PING_REQ 0x04
#define HIO_PING_RSP 0x84

/** Solicit response from device. Request structure.
 */
struct hio_ping_req {
    uint8_t payload[0]; /**< Ping message content. */
};

static inline struct packet *
alloc_hio_ping_req(struct hio_ping_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_hio_ping_req(const uint8_t payload[0])
{
    struct hio_ping_req *req;
    struct packet *pkt = alloc_hio_ping_req(&req);
    if (pkt) {
        /* payload must be set by the caller */
    }
    return pkt;
}

/** Solicit response from device. Response structure.
 */
struct hio_ping_rsp {
    uint8_t payload[0]; /**< Ping response content. */
};

static inline struct packet *
alloc_hio_ping_rsp(struct hio_ping_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_hio_ping_rsp(const uint8_t payload[0])
{
    struct hio_ping_rsp *rsp;
    struct packet *pkt = alloc_hio_ping_rsp(&rsp);
    if (pkt) {
        /* payload must be set by the caller */
    }
    return pkt;
}
/*
 * Message wakeup
 */

#define HIO_WAKEUP_IND 0xc5

/** Informs host that device is awake. Keep wakeup_pin set until wakeup_ind is received. Indication structure.
 */
struct hio_wakeup_ind {
    uint32_t reserved;
};

static inline struct packet *
alloc_hio_wakeup_ind(struct hio_wakeup_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_hio_wakeup_ind(void)
{
    struct hio_wakeup_ind *ind;
    struct packet *pkt = alloc_hio_wakeup_ind(&ind);
    if (pkt) {
        ind->reserved = 0;
    }
    return pkt;
}
/*
 * Message console
 */

#define HIO_CONSOLE_IND 0xc6

/** Debug console output. Indication structure.
 */
struct hio_console_ind {
    char str[0]; /**< console message */
};

static inline struct packet *
alloc_hio_console_ind(struct hio_console_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_hio_console_ind(const char str[0])
{
    struct hio_console_ind *ind;
    struct packet *pkt = alloc_hio_console_ind(&ind);
    if (pkt) {
        strncpy(ind->str, str, 0);
    }
    return pkt;
}

/* [[[end]]] */
/** @} */
