/*
 * Definition of monitor API
 *
 * Monitor interface
 */
#pragma once
#include <string.h>
#include <kernel/hostio.h>

/*
[[[cog
import cog, generate
generate.doxygen(generate.Emitter(cog.out), 'monitor')
]]] */
/**
 * @addtogroup hioapi_monitor
 * @{
 *
 * This set of interface functions provides means to perform low-level testing of
 * the device. The interface contains functions for testing both the receive and
 * transmit paths. This interface is using a WIFI monitor mode interface (an
 * interface in promiscous mode that passes all received frames up for
 * processing).
 *
 * <table>
 * <caption>Message summary (group=1)</caption>
 * <tr><th>OPERATION</th><th>REQ</th><th>RSP</th><th>IND</th></tr>
 * <tr><td>start</td><td>0x00 - monitor_start_req</td><td>0x80 - monitor_start_rsp</td><td></td></tr>
 * <tr><td>shutdown</td><td>0x01 - monitor_shutdown_req</td><td>0x81 - monitor_shutdown_rsp</td><td></td></tr>
 * <tr><td>cal</td><td>0x02 - monitor_cal_req</td><td>0x82 - monitor_cal_rsp</td><td></td></tr>
 * <tr><td>get_cal</td><td>0x03 - monitor_get_cal_req</td><td>0x83 - monitor_get_cal_rsp</td><td></td></tr>
 * <tr><td>set_channel</td><td>0x04 - monitor_set_channel_req</td><td>0x84 - monitor_set_channel_rsp</td><td></td></tr>
 * <tr><td>set_filter</td><td>0x05 - monitor_set_filter_req</td><td>0x85 - monitor_set_filter_rsp</td><td></td></tr>
 * <tr><td>set_txpower</td><td>0x06 - monitor_set_txpower_req</td><td>0x86 - monitor_set_txpower_rsp</td><td></td></tr>
 * <tr><td>set_loopback</td><td>0x07 - monitor_set_loopback_req</td><td>0x87 - monitor_set_loopback_rsp</td><td></td></tr>
 * <tr><td>set_noise</td><td>0x08 - monitor_set_noise_req</td><td>0x88 - monitor_set_noise_rsp</td><td></td></tr>
 * <tr><td>get_rxstats</td><td>0x09 - monitor_get_rxstats_req</td><td>0x89 - monitor_get_rxstats_rsp</td><td></td></tr>
 * <tr><td>clr_rxstats</td><td>0x0a - monitor_clr_rxstats_req</td><td>0x8a - monitor_clr_rxstats_rsp</td><td></td></tr>
 * <tr><td>xmit_frame</td><td>0x0b - monitor_xmit_frame_req</td><td>0x8b - monitor_xmit_frame_rsp</td><td>0xcb - monitor_xmit_frame_ind</td></tr>
 * <tr><td>xmit_cop</td><td>0x0c - monitor_xmit_cop_req</td><td>0x8c - monitor_xmit_cop_rsp</td><td></td></tr>
 * <tr><td>xmit_stop</td><td>0x0d - monitor_xmit_stop_req</td><td>0x8d - monitor_xmit_stop_rsp</td><td></td></tr>
 * <tr><td>rx_frame</td><td></td><td></td><td>0xce - monitor_rx_frame_ind</td></tr>
 * <tr><td>get_temp</td><td>0x0f - monitor_get_temp_req</td><td>0x8f - monitor_get_temp_rsp</td><td></td></tr>
 * <tr><td>set_ct</td><td>0x10 - monitor_set_ct_req</td><td>0x90 - monitor_set_ct_rsp</td><td></td></tr>
 * <tr><td>xmit_tone</td><td>0x11 - monitor_xmit_tone_req</td><td>0x91 - monitor_xmit_tone_rsp</td><td></td></tr>
 * <tr><td>set_regulatory_domain</td><td>0x12 - monitor_set_regulatory_domain_req</td><td>0x92 - monitor_set_regulatory_domain_rsp</td><td></td></tr>
 * <tr><td>set_antenna_gain</td><td>0x13 - monitor_set_antenna_gain_req</td><td>0x93 - monitor_set_antenna_gain_rsp</td><td></td></tr>
 * </table>
 */
/* [[[end]]] */

/*
[[[cog
cog.outl('')
generate.header(generate.Emitter(cog.out), 'monitor')
cog.outl('')
]]] */

#define HIO_GROUP_MONITOR 1

/*
 * Message start
 */

#define MONITOR_START_REQ 0x00
#define MONITOR_START_RSP 0x80

/** Create a monitor mode interface. Request structure.
 */
struct monitor_start_req {
};

static inline struct packet *
alloc_monitor_start_req(struct monitor_start_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_start_req(void)
{
    struct monitor_start_req *req;
    struct packet *pkt = alloc_monitor_start_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Create a monitor mode interface. Response structure.
 */
struct monitor_start_rsp {
    int32_t status; /**< result status, zero is success */
    uint8_t hwaddr[6]; /**< The MAC address used by the interface */
    uint16_t reserved; /**< Reserved. Always zero */
};

static inline struct packet *
alloc_monitor_start_rsp(struct monitor_start_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_start_rsp(const int32_t status, const uint8_t hwaddr[6])
{
    struct monitor_start_rsp *rsp;
    struct packet *pkt = alloc_monitor_start_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        memcpy(rsp->hwaddr, hwaddr, sizeof rsp->hwaddr);
        rsp->reserved = 0;
    }
    return pkt;
}
/*
 * Message shutdown
 */

#define MONITOR_SHUTDOWN_REQ 0x01
#define MONITOR_SHUTDOWN_RSP 0x81

/** Destroy the monitor mode interface. Request structure.
 */
struct monitor_shutdown_req {
};

static inline struct packet *
alloc_monitor_shutdown_req(struct monitor_shutdown_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_shutdown_req(void)
{
    struct monitor_shutdown_req *req;
    struct packet *pkt = alloc_monitor_shutdown_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Destroy the monitor mode interface. Response structure.
 */
struct monitor_shutdown_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_shutdown_rsp(struct monitor_shutdown_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_shutdown_rsp(const int32_t status)
{
    struct monitor_shutdown_rsp *rsp;
    struct packet *pkt = alloc_monitor_shutdown_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message cal
 */

#define MONITOR_CAL_REQ 0x02
#define MONITOR_CAL_RSP 0x82

/** Calibrate radio. Request structure.
 */
struct monitor_cal_req {
    uint32_t cal_freq_ndev; /**< 1: perform freq calibration, 0: perform dev calibration */
    uint32_t freq; /**< Frequency to calibrate */
    uint32_t type; /**< Type (0: wifi, 1:blue) */
};

static inline struct packet *
alloc_monitor_cal_req(struct monitor_cal_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_cal_req(const uint32_t cal_freq_ndev, const uint32_t freq, const uint32_t type)
{
    struct monitor_cal_req *req;
    struct packet *pkt = alloc_monitor_cal_req(&req);
    if (pkt) {
        req->cal_freq_ndev = cal_freq_ndev;
        req->freq = freq;
        req->type = type;
    }
    return pkt;
}

/** Calibrate radio. Response structure.
 */
struct monitor_cal_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_cal_rsp(struct monitor_cal_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_cal_rsp(const int32_t status)
{
    struct monitor_cal_rsp *rsp;
    struct packet *pkt = alloc_monitor_cal_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message get_cal
 */

#define MONITOR_GET_CAL_REQ 0x03
#define MONITOR_GET_CAL_RSP 0x83

/** Read the RF calibration parameters for a specific frequency. Request structure.
 */
struct monitor_get_cal_req {
    uint32_t freq; /**< Frequency in MHz */
    uint32_t type; /**< Type (0: wifi, 1: blue) */
};

static inline struct packet *
alloc_monitor_get_cal_req(struct monitor_get_cal_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_get_cal_req(const uint32_t freq, const uint32_t type)
{
    struct monitor_get_cal_req *req;
    struct packet *pkt = alloc_monitor_get_cal_req(&req);
    if (pkt) {
        req->freq = freq;
        req->type = type;
    }
    return pkt;
}

/** Read the RF calibration parameters for a specific frequency. Response structure.
 */
struct monitor_get_cal_rsp {
    int32_t status; /**< result status, zero is success */
    uint16_t freq; /**< Frequency from the request */
    int16_t temp_freq; /**< Temperature stamp (freq table) */
    uint8_t lna2_cap[3]; /**< LNA2 caps */
    uint8_t lna2_cs[4]; /**< LNA current sources */
    uint8_t lna3_cap[3]; /**< LNA3 caps */
    uint8_t lna3_cs[4]; /**< LNA3 current sources */
    uint8_t cc; /**< LNA current compensation value */
    uint8_t ilo_cap[4]; /**< RXILO tuning caps */
    uint8_t ilo_delay[2]; /**< RXILO delay */
    uint8_t dpll_dco[3]; /**< DCO Bias */
    int16_t dpll_gtrk; /**< DPLL TRK gain */
    int16_t dpll_gdac; /**< DPLL DAC gain */
    int16_t dpll_gpwl[9]; /**< DPLL piece-wise linear coeffs */
    int16_t temp_dev; /**< Temperature stamp (dev table) */
    uint8_t tdc_fast; /**< TDC fast delay line delay */
    uint8_t tdc_slow; /**< TDC slow delay line delay */
    uint8_t tdc_delay; /**< Coarse RXTDC alignment */
    uint8_t tdc_vset; /**< TDC LDO vset */
    int16_t dc_offset; /**< RX PADC dc offset compensation */
    uint8_t en_chg; /**< RX PADC kickback enable */
};

static inline struct packet *
alloc_monitor_get_cal_rsp(struct monitor_get_cal_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_get_cal_rsp(const int32_t status, const uint16_t freq, const int16_t temp_freq, const uint8_t lna2_cap[3], const uint8_t lna2_cs[4], const uint8_t lna3_cap[3], const uint8_t lna3_cs[4], const uint8_t cc, const uint8_t ilo_cap[4], const uint8_t ilo_delay[2], const uint8_t dpll_dco[3], const int16_t dpll_gtrk, const int16_t dpll_gdac, const int16_t dpll_gpwl[9], const int16_t temp_dev, const uint8_t tdc_fast, const uint8_t tdc_slow, const uint8_t tdc_delay, const uint8_t tdc_vset, const int16_t dc_offset, const uint8_t en_chg)
{
    struct monitor_get_cal_rsp *rsp;
    struct packet *pkt = alloc_monitor_get_cal_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->freq = freq;
        rsp->temp_freq = temp_freq;
        memcpy(rsp->lna2_cap, lna2_cap, sizeof rsp->lna2_cap);
        memcpy(rsp->lna2_cs, lna2_cs, sizeof rsp->lna2_cs);
        memcpy(rsp->lna3_cap, lna3_cap, sizeof rsp->lna3_cap);
        memcpy(rsp->lna3_cs, lna3_cs, sizeof rsp->lna3_cs);
        rsp->cc = cc;
        memcpy(rsp->ilo_cap, ilo_cap, sizeof rsp->ilo_cap);
        memcpy(rsp->ilo_delay, ilo_delay, sizeof rsp->ilo_delay);
        memcpy(rsp->dpll_dco, dpll_dco, sizeof rsp->dpll_dco);
        rsp->dpll_gtrk = dpll_gtrk;
        rsp->dpll_gdac = dpll_gdac;
        memcpy(rsp->dpll_gpwl, dpll_gpwl, sizeof rsp->dpll_gpwl);
        rsp->temp_dev = temp_dev;
        rsp->tdc_fast = tdc_fast;
        rsp->tdc_slow = tdc_slow;
        rsp->tdc_delay = tdc_delay;
        rsp->tdc_vset = tdc_vset;
        rsp->dc_offset = dc_offset;
        rsp->en_chg = en_chg;
    }
    return pkt;
}
/*
 * Message set_channel
 */

#define MONITOR_SET_CHANNEL_REQ 0x04
#define MONITOR_SET_CHANNEL_RSP 0x84

/** Switch the current channel. Request structure.
 */
struct monitor_set_channel_req {
    uint32_t freq; /**< Switch to the specified channel (1..14) */
};

static inline struct packet *
alloc_monitor_set_channel_req(struct monitor_set_channel_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_channel_req(const uint32_t freq)
{
    struct monitor_set_channel_req *req;
    struct packet *pkt = alloc_monitor_set_channel_req(&req);
    if (pkt) {
        req->freq = freq;
    }
    return pkt;
}

/** Switch the current channel. Response structure.
 */
struct monitor_set_channel_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_set_channel_rsp(struct monitor_set_channel_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_channel_rsp(const int32_t status)
{
    struct monitor_set_channel_rsp *rsp;
    struct packet *pkt = alloc_monitor_set_channel_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message set_filter
 */

#define MONITOR_SET_FILTER_REQ 0x05
#define MONITOR_SET_FILTER_RSP 0x85

/** Set a filter on which frames should be passed up to the host via rx_frame_ind messages. Request structure.
 */
struct monitor_set_filter_req {
    uint16_t level; /**< Select frames based on FCS and address match (0=no frames, 1=FCS OK and unicast with address match, 2=same as (1) plus multicast, 3=FCS OK, 4=All frames) */
    uint16_t mgmt; /**< Select types of management frames to pass up (one bit per subtype) */
    uint16_t ctrl; /**< Select types of control frames to pass up (one bit per subtype) */
    uint16_t data; /**< Select types of data frames to pass up (one bit per subtype) */
    uint16_t limit; /**< Limit the number of bytes of header and payload octets */
    uint16_t reserved; /**< Reserved. Must be zero */
};

static inline struct packet *
alloc_monitor_set_filter_req(struct monitor_set_filter_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_filter_req(const uint16_t level, const uint16_t mgmt, const uint16_t ctrl, const uint16_t data, const uint16_t limit)
{
    struct monitor_set_filter_req *req;
    struct packet *pkt = alloc_monitor_set_filter_req(&req);
    if (pkt) {
        req->level = level;
        req->mgmt = mgmt;
        req->ctrl = ctrl;
        req->data = data;
        req->limit = limit;
        req->reserved = 0;
    }
    return pkt;
}

/** Set a filter on which frames should be passed up to the host via rx_frame_ind messages. Response structure.
 */
struct monitor_set_filter_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_set_filter_rsp(struct monitor_set_filter_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_filter_rsp(const int32_t status)
{
    struct monitor_set_filter_rsp *rsp;
    struct packet *pkt = alloc_monitor_set_filter_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message set_txpower
 */

#define MONITOR_SET_TXPOWER_REQ 0x06
#define MONITOR_SET_TXPOWER_RSP 0x86

/** Set the transmit power. Request structure.
 */
struct monitor_set_txpower_req {
    int8_t tx_power; /**< TX power in vBm (half-dB) (-20..40(max)) */
    uint8_t reserved1; /**< Reserved. Must be zero */
    uint16_t reserved2; /**< Reserved. Must be zero */
};

static inline struct packet *
alloc_monitor_set_txpower_req(struct monitor_set_txpower_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_txpower_req(const int8_t tx_power)
{
    struct monitor_set_txpower_req *req;
    struct packet *pkt = alloc_monitor_set_txpower_req(&req);
    if (pkt) {
        req->tx_power = tx_power;
        req->reserved1 = 0;
        req->reserved2 = 0;
    }
    return pkt;
}

/** Set the transmit power. Response structure.
 */
struct monitor_set_txpower_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_set_txpower_rsp(struct monitor_set_txpower_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_txpower_rsp(const int32_t status)
{
    struct monitor_set_txpower_rsp *rsp;
    struct packet *pkt = alloc_monitor_set_txpower_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message set_loopback
 */

#define MONITOR_SET_LOOPBACK_REQ 0x07
#define MONITOR_SET_LOOPBACK_RSP 0x87

/** Enable digital baseband loopback. For internal testing use only. Request structure.
 */
struct monitor_set_loopback_req {
    uint8_t rxloop; /**< Internal use. */
    uint8_t rxsrc; /**< Internal use. */
};

static inline struct packet *
alloc_monitor_set_loopback_req(struct monitor_set_loopback_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_loopback_req(const uint8_t rxloop, const uint8_t rxsrc)
{
    struct monitor_set_loopback_req *req;
    struct packet *pkt = alloc_monitor_set_loopback_req(&req);
    if (pkt) {
        req->rxloop = rxloop;
        req->rxsrc = rxsrc;
    }
    return pkt;
}

/** Enable digital baseband loopback. For internal testing use only. Response structure.
 */
struct monitor_set_loopback_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_set_loopback_rsp(struct monitor_set_loopback_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_loopback_rsp(const int32_t status)
{
    struct monitor_set_loopback_rsp *rsp;
    struct packet *pkt = alloc_monitor_set_loopback_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message set_noise
 */

#define MONITOR_SET_NOISE_REQ 0x08
#define MONITOR_SET_NOISE_RSP 0x88

/** Enable mixing of Gaussian white noice on the digital receiver input. For internal testing use only. Request structure.
 */
struct monitor_set_noise_req {
    uint16_t noise; /**< Internal use. */
};

static inline struct packet *
alloc_monitor_set_noise_req(struct monitor_set_noise_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_noise_req(const uint16_t noise)
{
    struct monitor_set_noise_req *req;
    struct packet *pkt = alloc_monitor_set_noise_req(&req);
    if (pkt) {
        req->noise = noise;
    }
    return pkt;
}

/** Enable mixing of Gaussian white noice on the digital receiver input. For internal testing use only. Response structure.
 */
struct monitor_set_noise_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_set_noise_rsp(struct monitor_set_noise_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_noise_rsp(const int32_t status)
{
    struct monitor_set_noise_rsp *rsp;
    struct packet *pkt = alloc_monitor_set_noise_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message get_rxstats
 */

#define MONITOR_GET_RXSTATS_REQ 0x09
#define MONITOR_GET_RXSTATS_RSP 0x89

/** Report statistics on the number of received frames. Request structure.
 */
struct monitor_get_rxstats_req {
    uint16_t rate; /**< Request counters for this rate. The rate format (0=11b, 1=11g or 2=11n) is encoded in bits 9..8, and the rate index is encoded in bits 7..0. E.g. OFDM rate 24.0 Mbps is encoded as hexadecial number 0x104. */
};

static inline struct packet *
alloc_monitor_get_rxstats_req(struct monitor_get_rxstats_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_get_rxstats_req(const uint16_t rate)
{
    struct monitor_get_rxstats_req *req;
    struct packet *pkt = alloc_monitor_get_rxstats_req(&req);
    if (pkt) {
        req->rate = rate;
    }
    return pkt;
}

/** Report statistics on the number of received frames. Response structure.
 */
struct monitor_get_rxstats_rsp {
    uint32_t num_pkt; /**< Total number of received frames */
    uint32_t num_fcs_fail; /**< Number of frames received with FCS error */
    int16_t snr_avg; /**< Average SNR (in dB) for all received frames */
    int16_t snr_min; /**< Minimum SNR (in dB) for all received frames */
};

static inline struct packet *
alloc_monitor_get_rxstats_rsp(struct monitor_get_rxstats_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_get_rxstats_rsp(const uint32_t num_pkt, const uint32_t num_fcs_fail, const int16_t snr_avg, const int16_t snr_min)
{
    struct monitor_get_rxstats_rsp *rsp;
    struct packet *pkt = alloc_monitor_get_rxstats_rsp(&rsp);
    if (pkt) {
        rsp->num_pkt = num_pkt;
        rsp->num_fcs_fail = num_fcs_fail;
        rsp->snr_avg = snr_avg;
        rsp->snr_min = snr_min;
    }
    return pkt;
}
/*
 * Message clr_rxstats
 */

#define MONITOR_CLR_RXSTATS_REQ 0x0a
#define MONITOR_CLR_RXSTATS_RSP 0x8a

/** Clear all counters for received frames. Counters for all rates are cleared at once. Request structure.
 */
struct monitor_clr_rxstats_req {
};

static inline struct packet *
alloc_monitor_clr_rxstats_req(struct monitor_clr_rxstats_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_clr_rxstats_req(void)
{
    struct monitor_clr_rxstats_req *req;
    struct packet *pkt = alloc_monitor_clr_rxstats_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Clear all counters for received frames. Counters for all rates are cleared at once. Response structure.
 */
struct monitor_clr_rxstats_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_clr_rxstats_rsp(struct monitor_clr_rxstats_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_clr_rxstats_rsp(const int32_t status)
{
    struct monitor_clr_rxstats_rsp *rsp;
    struct packet *pkt = alloc_monitor_clr_rxstats_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message xmit_frame
 */

#define MONITOR_XMIT_FRAME_REQ 0x0b
#define MONITOR_XMIT_FRAME_RSP 0x8b
#define MONITOR_XMIT_FRAME_IND 0xcb

/** Transmit a raw frame using normal backoff procedure. Request structure.
 */
struct monitor_xmit_frame_req {
    uint16_t flags; /**< Frame TX flags (for details see documentation for wifi_tx_flags in the reference manual) */
    uint16_t txrate; /**< Rate to use for transmission */
    uint16_t maxtries; /**< Number of retries for this transmission */
    uint16_t length; /**< Length of frame */
    uint8_t frame[0]; /**< Frame contents (including 802.11 MAC header) */
};

static inline struct packet *
alloc_monitor_xmit_frame_req(struct monitor_xmit_frame_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_xmit_frame_req(const uint16_t flags, const uint16_t txrate, const uint16_t maxtries, const uint16_t length, const uint8_t frame[0])
{
    struct monitor_xmit_frame_req *req;
    struct packet *pkt = alloc_monitor_xmit_frame_req(&req);
    if (pkt) {
        req->flags = flags;
        req->txrate = txrate;
        req->maxtries = maxtries;
        req->length = length;
        /* frame must be set by the caller */
    }
    return pkt;
}

/** Transmit a raw frame using normal backoff procedure. Response structure.
 */
struct monitor_xmit_frame_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t cookie; /**< Cookie that identifies this frame transmision */
};

static inline struct packet *
alloc_monitor_xmit_frame_rsp(struct monitor_xmit_frame_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_xmit_frame_rsp(const int32_t status, const uint32_t cookie)
{
    struct monitor_xmit_frame_rsp *rsp;
    struct packet *pkt = alloc_monitor_xmit_frame_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->cookie = cookie;
    }
    return pkt;
}

/** Transmit a raw frame using normal backoff procedure. Indication structure.
 */
struct monitor_xmit_frame_ind {
    uint32_t cookie; /**< Cooke that identifies the frame */
    uint16_t numtries; /**< Number of attempts to transmit */
    uint16_t reserved; /**< Reserved. Always zero */
    uint16_t flags; /**< Frame TX flags (for details see documentation for wifi_tx_flags in the reference manual) */
    uint16_t duration; /**< Duration in microseconds for this frame */
    uint32_t txtime; /**< The MAC level timestamp when this frame hit the antenna */
    uint32_t acktime; /**< Internal use. */
};

static inline struct packet *
alloc_monitor_xmit_frame_ind(struct monitor_xmit_frame_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_monitor_xmit_frame_ind(const uint32_t cookie, const uint16_t numtries, const uint16_t flags, const uint16_t duration, const uint32_t txtime, const uint32_t acktime)
{
    struct monitor_xmit_frame_ind *ind;
    struct packet *pkt = alloc_monitor_xmit_frame_ind(&ind);
    if (pkt) {
        ind->cookie = cookie;
        ind->numtries = numtries;
        ind->reserved = 0;
        ind->flags = flags;
        ind->duration = duration;
        ind->txtime = txtime;
        ind->acktime = acktime;
    }
    return pkt;
}
/*
 * Message xmit_cop
 */

#define MONITOR_XMIT_COP_REQ 0x0c
#define MONITOR_XMIT_COP_RSP 0x8c

/** Transmit a series of frames without normal backoff procedure. Typically for shielded room testing. Request structure.
 */
struct monitor_xmit_cop_req {
    uint32_t repeat; /**< Number of frames to transmit */
    uint32_t interval; /**< Interframe gap in microseconds */
    uint16_t rate; /**< Transmission rate */
    uint16_t len; /**< Length of each frame */
    uint8_t mode; /**< Payload generation mode (0=pseudo random sequence, 1=byte sequence 0,1,2,3..., 2=all zero) */
    int8_t power; /**< Transmission power specified in vBm (half-dB) in the range -20..40 */
    uint16_t freq; /**< Frequency in MHz for the transmit channel (zero for digital loopback) */
};

static inline struct packet *
alloc_monitor_xmit_cop_req(struct monitor_xmit_cop_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_xmit_cop_req(const uint32_t repeat, const uint32_t interval, const uint16_t rate, const uint16_t len, const uint8_t mode, const int8_t power, const uint16_t freq)
{
    struct monitor_xmit_cop_req *req;
    struct packet *pkt = alloc_monitor_xmit_cop_req(&req);
    if (pkt) {
        req->repeat = repeat;
        req->interval = interval;
        req->rate = rate;
        req->len = len;
        req->mode = mode;
        req->power = power;
        req->freq = freq;
    }
    return pkt;
}

/** Transmit a series of frames without normal backoff procedure. Typically for shielded room testing. Response structure.
 */
struct monitor_xmit_cop_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_xmit_cop_rsp(struct monitor_xmit_cop_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_xmit_cop_rsp(const int32_t status)
{
    struct monitor_xmit_cop_rsp *rsp;
    struct packet *pkt = alloc_monitor_xmit_cop_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message xmit_stop
 */

#define MONITOR_XMIT_STOP_REQ 0x0d
#define MONITOR_XMIT_STOP_RSP 0x8d

/** Stop an ongoing transmission started with xmit_cop. Request structure.
 */
struct monitor_xmit_stop_req {
};

static inline struct packet *
alloc_monitor_xmit_stop_req(struct monitor_xmit_stop_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_xmit_stop_req(void)
{
    struct monitor_xmit_stop_req *req;
    struct packet *pkt = alloc_monitor_xmit_stop_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Stop an ongoing transmission started with xmit_cop. Response structure.
 */
struct monitor_xmit_stop_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_xmit_stop_rsp(struct monitor_xmit_stop_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_xmit_stop_rsp(const int32_t status)
{
    struct monitor_xmit_stop_rsp *rsp;
    struct packet *pkt = alloc_monitor_xmit_stop_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message rx_frame
 */

#define MONITOR_RX_FRAME_IND 0xce

/** Asynchronous message sent by the device when a frame is received that match the filter criteria. Indication structure.
 */
struct monitor_rx_frame_ind {
    uint64_t timestamp; /**< MAC timestamp when the frame was received */
    uint32_t flags; /**< Frame RX flags (for details see documentation for wifi_rx_flags in the reference manual) */
    uint16_t length; /**< Actual length of frame (the number of octets in the frame field may be less due to a limited size from a previous set_filter */
    uint16_t duration; /**< The frame duration in microseconds */
    uint16_t rate; /**< Frame rate */
    int16_t rssi; /**< RSSI in dBm */
    int16_t snr; /**< SNR in dB */
    int16_t reserved; /**< Reserved. Always zero */
    uint8_t frame[0]; /**< The first octets (starting with the 802.11 MAC level header) of the frame */
};

static inline struct packet *
alloc_monitor_rx_frame_ind(struct monitor_rx_frame_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_monitor_rx_frame_ind(const uint64_t timestamp, const uint32_t flags, const uint16_t length, const uint16_t duration, const uint16_t rate, const int16_t rssi, const int16_t snr, const uint8_t frame[0])
{
    struct monitor_rx_frame_ind *ind;
    struct packet *pkt = alloc_monitor_rx_frame_ind(&ind);
    if (pkt) {
        ind->timestamp = timestamp;
        ind->flags = flags;
        ind->length = length;
        ind->duration = duration;
        ind->rate = rate;
        ind->rssi = rssi;
        ind->snr = snr;
        ind->reserved = 0;
        /* frame must be set by the caller */
    }
    return pkt;
}
/*
 * Message get_temp
 */

#define MONITOR_GET_TEMP_REQ 0x0f
#define MONITOR_GET_TEMP_RSP 0x8f

/** Read current temperature from hkadc. Request structure.
 */
struct monitor_get_temp_req {
};

static inline struct packet *
alloc_monitor_get_temp_req(struct monitor_get_temp_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_get_temp_req(void)
{
    struct monitor_get_temp_req *req;
    struct packet *pkt = alloc_monitor_get_temp_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Read current temperature from hkadc. Response structure.
 */
struct monitor_get_temp_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t temp; /**< The inner temperature (in integer Celsius) of the chip core */
};

static inline struct packet *
alloc_monitor_get_temp_rsp(struct monitor_get_temp_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_get_temp_rsp(const int32_t status, const uint32_t temp)
{
    struct monitor_get_temp_rsp *rsp;
    struct packet *pkt = alloc_monitor_get_temp_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->temp = temp;
    }
    return pkt;
}
/*
 * Message set_ct
 */

#define MONITOR_SET_CT_REQ 0x10
#define MONITOR_SET_CT_RSP 0x90

/** Set the CT value (applied immediately). Request structure.
 */
struct monitor_set_ct_req {
    uint8_t ct; /**< CT value */
};

static inline struct packet *
alloc_monitor_set_ct_req(struct monitor_set_ct_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_ct_req(const uint8_t ct)
{
    struct monitor_set_ct_req *req;
    struct packet *pkt = alloc_monitor_set_ct_req(&req);
    if (pkt) {
        req->ct = ct;
    }
    return pkt;
}

/** Set the CT value (applied immediately). Response structure.
 */
struct monitor_set_ct_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_set_ct_rsp(struct monitor_set_ct_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_ct_rsp(const int32_t status)
{
    struct monitor_set_ct_rsp *rsp;
    struct packet *pkt = alloc_monitor_set_ct_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message xmit_tone
 */

#define MONITOR_XMIT_TONE_REQ 0x11
#define MONITOR_XMIT_TONE_RSP 0x91

/** Start transmitting an RF tone. Request structure.
 */
struct monitor_xmit_tone_req {
    uint16_t freq; /**< Frequency in MHz for the transmit channel */
    uint16_t dpa_power; /**< DPA TX power */
};

static inline struct packet *
alloc_monitor_xmit_tone_req(struct monitor_xmit_tone_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_xmit_tone_req(const uint16_t freq, const uint16_t dpa_power)
{
    struct monitor_xmit_tone_req *req;
    struct packet *pkt = alloc_monitor_xmit_tone_req(&req);
    if (pkt) {
        req->freq = freq;
        req->dpa_power = dpa_power;
    }
    return pkt;
}

/** Start transmitting an RF tone. Response structure.
 */
struct monitor_xmit_tone_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_xmit_tone_rsp(struct monitor_xmit_tone_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_xmit_tone_rsp(const int32_t status)
{
    struct monitor_xmit_tone_rsp *rsp;
    struct packet *pkt = alloc_monitor_xmit_tone_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message set_regulatory_domain
 */

#define MONITOR_SET_REGULATORY_DOMAIN_REQ 0x12
#define MONITOR_SET_REGULATORY_DOMAIN_RSP 0x92

/** Set regulatory domain, with channels and power limits. Request structure.
 */
struct monitor_set_regulatory_domain_req {
    char domain[64]; /**< FCC, ETSI, TELEC, KCC, SRCC (or e.g. 1..11@20) */
};

static inline struct packet *
alloc_monitor_set_regulatory_domain_req(struct monitor_set_regulatory_domain_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_regulatory_domain_req(const char domain[64])
{
    struct monitor_set_regulatory_domain_req *req;
    struct packet *pkt = alloc_monitor_set_regulatory_domain_req(&req);
    if (pkt) {
        strncpy(req->domain, domain, 64);
    }
    return pkt;
}

/** Set regulatory domain, with channels and power limits. Response structure.
 */
struct monitor_set_regulatory_domain_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_set_regulatory_domain_rsp(struct monitor_set_regulatory_domain_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_regulatory_domain_rsp(const int32_t status)
{
    struct monitor_set_regulatory_domain_rsp *rsp;
    struct packet *pkt = alloc_monitor_set_regulatory_domain_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message set_antenna_gain
 */

#define MONITOR_SET_ANTENNA_GAIN_REQ 0x13
#define MONITOR_SET_ANTENNA_GAIN_RSP 0x93

/** Set the antenna gain in dBi. Request structure.
 */
struct monitor_set_antenna_gain_req {
    char gain[8]; /**< Antenna gain in dBi, e.g. 2.15 */
};

static inline struct packet *
alloc_monitor_set_antenna_gain_req(struct monitor_set_antenna_gain_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_antenna_gain_req(const char gain[8])
{
    struct monitor_set_antenna_gain_req *req;
    struct packet *pkt = alloc_monitor_set_antenna_gain_req(&req);
    if (pkt) {
        strncpy(req->gain, gain, 8);
    }
    return pkt;
}

/** Set the antenna gain in dBi. Response structure.
 */
struct monitor_set_antenna_gain_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_monitor_set_antenna_gain_rsp(struct monitor_set_antenna_gain_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_monitor_set_antenna_gain_rsp(const int32_t status)
{
    struct monitor_set_antenna_gain_rsp *rsp;
    struct packet *pkt = alloc_monitor_set_antenna_gain_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/* [[[end]]] */
/** @} */
