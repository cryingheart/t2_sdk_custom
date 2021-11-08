/*
 * 802.11 protocol definitions.
 *
 * Copyright (c) 2001 Atsushi Onoe
 * Copyright (c) 2002-2009 Sam Leffler, Errno Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @addtogroup wifi_defs
 * @{
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <kernel/compiler.h>

#define	IEEE80211_ADDR_LEN	6       /* size of 802.11 address */
#define IEEE80211_RSNA_PSK_LEN 32   /* size of Pre Shared Key */

#define IEEE80211_MAX_MSDU         2304
#define IEEE80211_MAX_A_MSDU_SHORT 3839
#define IEEE80211_MAX_A_MSDU_LONG  7935

#define IEEE80211_A_PSDU_MAX_LENGTH_B 4095
#define IEEE80211_A_PSDU_MAX_LENGTH_G 4095
#define IEEE80211_A_PSDU_MAX_LENGTH_N 65535

/**
 * IEEE 802.11 frame header
 */
struct ieee80211_frame {
    uint16_t i_fc;
    uint16_t i_dur;
    uint8_t  i_addr1[IEEE80211_ADDR_LEN];
    uint8_t  i_addr2[IEEE80211_ADDR_LEN];
    uint8_t  i_addr3[IEEE80211_ADDR_LEN];
    uint16_t i_seq;
} __packed;

/**
 * IEEE 802.11 Authentication frame body
 */
struct ieee80211_auth_frame_body {
    uint16_t algo;
    uint16_t seqno;
    uint16_t status;
} __packed;

/**
 * IEEE 802.11 Deauthentication frame body
 */
struct ieee80211_deauth_frame_body {
    uint16_t reason_code;
    uint8_t ies[0];
} __packed;

/**
 * IEEE 802.11 Association Request frame body
 */
struct ieee80211_assoc_req_frame_body {
    uint16_t capability;
    uint16_t listen_interval;
    uint8_t ies[0];
} __packed;

/**
 * IEEE 802.11 Association Response frame body
 */
struct ieee80211_assoc_resp_frame_body {
    uint16_t capability;
    uint16_t status_code;
    uint16_t aid;
    uint8_t ies[0];
} __packed;

/**
 * IEEE 802.11 QoS frame header
 */
struct ieee80211_qosframe {
    uint16_t i_fc;
    uint16_t i_dur;
    uint8_t  i_addr1[IEEE80211_ADDR_LEN];
    uint8_t  i_addr2[IEEE80211_ADDR_LEN];
    uint8_t  i_addr3[IEEE80211_ADDR_LEN];
    uint16_t i_seq;
    uint16_t i_qos;
} __packed;

/**
 * IEEE 802.11 frame header (/w four address fields)
 */
struct ieee80211_frame_addr4 {
    uint16_t i_fc;
    uint16_t i_dur;
    uint8_t  i_addr1[IEEE80211_ADDR_LEN];
    uint8_t  i_addr2[IEEE80211_ADDR_LEN];
    uint8_t  i_addr3[IEEE80211_ADDR_LEN];
    uint16_t i_seq;
    uint8_t  i_addr4[IEEE80211_ADDR_LEN];
} __packed;

/**
 * IEEE 802.11 QoS frame header (/w four address fields)
 */
struct ieee80211_qosframe_addr4 {
    uint16_t i_fc;
    uint16_t i_dur;
    uint8_t  i_addr1[IEEE80211_ADDR_LEN];
    uint8_t  i_addr2[IEEE80211_ADDR_LEN];
    uint8_t  i_addr3[IEEE80211_ADDR_LEN];
    uint16_t i_seq;
    uint8_t  i_addr4[IEEE80211_ADDR_LEN];
    uint16_t i_qos;
} __packed;

#define	IEEE80211_FC0_VERSION_MASK		0x0003
#define	IEEE80211_FC0_VERSION_SHIFT		0
#define	IEEE80211_FC0_VERSION_0			0x0000
#define	IEEE80211_FC0_TYPE_MASK			0x000c
#define	IEEE80211_FC0_TYPE_SHIFT		2
#define	IEEE80211_FC0_TYPE_MGT			0x0000
#define	IEEE80211_FC0_TYPE_CTL			0x0004
#define	IEEE80211_FC0_TYPE_DATA			0x0008
#define	IEEE80211_FC0_SUBTYPE_MASK		0x00f0
#define	IEEE80211_FC0_SUBTYPE_SHIFT		4
/* for TYPE_MGT */
#define	IEEE80211_FC0_SUBTYPE_ASSOC_REQ		0x0000
#define	IEEE80211_FC0_SUBTYPE_ASSOC_RESP	0x0010
#define	IEEE80211_FC0_SUBTYPE_REASSOC_REQ	0x0020
#define	IEEE80211_FC0_SUBTYPE_REASSOC_RESP	0x0030
#define	IEEE80211_FC0_SUBTYPE_PROBE_REQ		0x0040
#define	IEEE80211_FC0_SUBTYPE_PROBE_RESP	0x0050
#define	IEEE80211_FC0_SUBTYPE_TIMING_ADV	0x0060
#define	IEEE80211_FC0_SUBTYPE_BEACON		0x0080
#define	IEEE80211_FC0_SUBTYPE_ATIM		0x0090
#define	IEEE80211_FC0_SUBTYPE_DISASSOC		0x00a0
#define	IEEE80211_FC0_SUBTYPE_AUTH		0x00b0
#define	IEEE80211_FC0_SUBTYPE_DEAUTH		0x00c0
#define	IEEE80211_FC0_SUBTYPE_ACTION		0x00d0
#define	IEEE80211_FC0_SUBTYPE_ACTION_NOACK	0x00e0
/* for TYPE_CTL */
#define	IEEE80211_FC0_SUBTYPE_BAR		0x0080
#define	IEEE80211_FC0_SUBTYPE_BA		0x0090
#define	IEEE80211_FC0_SUBTYPE_PS_POLL		0x00a0
#define	IEEE80211_FC0_SUBTYPE_RTS		0x00b0
#define	IEEE80211_FC0_SUBTYPE_CTS		0x00c0
#define	IEEE80211_FC0_SUBTYPE_ACK		0x00d0
#define	IEEE80211_FC0_SUBTYPE_CF_END		0x00e0
#define	IEEE80211_FC0_SUBTYPE_CF_END_ACK	0x00f0
/* for TYPE_DATA (bit combination) */
#define	IEEE80211_FC0_SUBTYPE_DATA		0x0000
#define	IEEE80211_FC0_SUBTYPE_CF_ACK		0x0010
#define	IEEE80211_FC0_SUBTYPE_CF_POLL		0x0020
#define	IEEE80211_FC0_SUBTYPE_CF_ACPL		0x0030
#define	IEEE80211_FC0_SUBTYPE_NULL		0x0040
#define	IEEE80211_FC0_SUBTYPE_CFACK		0x0050
#define	IEEE80211_FC0_SUBTYPE_CFPOLL		0x0060
#define	IEEE80211_FC0_SUBTYPE_CF_ACK_CF_ACK	0x0070
#define	IEEE80211_FC0_SUBTYPE_QOS		0x0080
#define	IEEE80211_FC0_SUBTYPE_QOS_NULL		0x00c0

#define	IEEE80211_FC1_DIR_MASK			0x0300
#define	IEEE80211_FC1_DIR_NODS			0x0000  /* STA->STA */
#define	IEEE80211_FC1_DIR_TODS			0x0100  /* STA->AP  */
#define	IEEE80211_FC1_DIR_FROMDS		0x0200  /* AP ->STA */
#define	IEEE80211_FC1_DIR_DSTODS		0x0300  /* AP ->AP  */
#define	IEEE80211_FC1_MORE_FRAG			0x0400
#define	IEEE80211_FC1_RETRY			0x0800
#define	IEEE80211_FC1_PWR_MGT			0x1000
#define	IEEE80211_FC1_MORE_DATA			0x2000
#define	IEEE80211_FC1_PROTECTED			0x4000
#define	IEEE80211_FC1_ORDER			0x8000

/* Bitfield definitions for i_seq */
#define	IEEE80211_SEQ_FRAG_MASK			0x000f
#define	IEEE80211_SEQ_FRAG_SHIFT		0
#define	IEEE80211_SEQ_SEQ_MASK			0xfff0
#define	IEEE80211_SEQ_SEQ_SHIFT			4
#define	IEEE80211_SEQ_RANGE			4096

/* Bitfield definitions for i_qos */
#define	IEEE80211_QOS_TXOP			0xff00
#define	IEEE80211_QOS_TXOP_S			8
#define	IEEE80211_QOS_AMSDU			0x0080
#define	IEEE80211_QOS_AMSDU_S			7
#define	IEEE80211_QOS_ACKPOLICY			0x0060
#define	IEEE80211_QOS_ACKPOLICY_S		5
#define	IEEE80211_QOS_EOSP			0x0010  /* EndOfService Period */
#define	IEEE80211_QOS_EOSP_S			4
#define	IEEE80211_QOS_TID			0x000f

enum ieee80211_ack_policy {
    ACK_POLICY_NORMAL   = 0,    /* Normal Ack or Implicit Block Ack Request. */
    ACK_POLICY_NOACK    = 1,    /* No Ack */
    ACK_POLICY_PSMP     = 2,    /* No explicit acknowledgment or PSMP Ack. */
    ACK_POLICY_BLOCKACK = 3     /* Block Ack */
};

#define AMPDU_DELIMITER_LENGTH         4
#define AMPDU_DELIMITER_SIGNATURE      0x4e
#define AMPDU_DELIMITER_SIGNATURE_MASK 0xff000000
#define AMPDU_DELIMITER_CRC_MASK       0x00ff0000
#define AMPDU_DELIMITER_LENGTH_MASK    0x0000fff0
#define AMPDU_NULL_DELIMITER           0x4e140000

static inline int
ieee80211_seq_add(int seq, int add)
{
    return (seq + add) & (IEEE80211_SEQ_RANGE-1);
}

static inline int
ieee80211_seq_sub(int seq, int sub)
{
    return (seq + IEEE80211_SEQ_RANGE - sub) & (IEEE80211_SEQ_RANGE-1);
}

static inline int
ieee80211_seq_inc(int seq)
{
    return ieee80211_seq_add(seq, 1);
}

static inline bool
ieee80211_seq_before(int a, int b)
{
    return ieee80211_seq_sub(b, a+1) < IEEE80211_SEQ_RANGE/2-1;
}

static inline bool
ieee80211_seq_after(int a, int b)
{
    return ieee80211_seq_before(b,a);
}

/**
 * Returns true if \a addr is a multicast/broadcast address
 */
static inline bool
ieee80211_is_multicast(const uint8_t *addr)
{
    return addr[0] & 0x01;
}

/**
 * Returns true if \a wh is a frame of \a type
 */
static inline bool
ieee80211_is_type(const struct ieee80211_frame *wh, uint16_t type)
{
    return (wh->i_fc & IEEE80211_FC0_TYPE_MASK) == type;
}

/**
 * Returns true if the type and subtype of the frame \a wh matches \a type and
 * \a subtype.
 */
static inline bool
ieee80211_is_subtype(const struct ieee80211_frame *wh, uint16_t type, uint16_t subtype)
{
    return (wh->i_fc & (IEEE80211_FC0_TYPE_MASK | IEEE80211_FC0_SUBTYPE_MASK)) == (type | subtype);
}

/**
 * Returns true if the frame \a wh is of type DATA or QoS-DATA
 */
static inline bool
ieee80211_is_data(const struct ieee80211_frame *wh)
{
    uint16_t mask = IEEE80211_FC0_TYPE_MASK | (IEEE80211_FC0_SUBTYPE_MASK & ~IEEE80211_FC0_SUBTYPE_QOS);
    return (wh->i_fc & mask) == (IEEE80211_FC0_TYPE_DATA | IEEE80211_FC0_SUBTYPE_DATA);
}

/**
 * Returns true if \a wh is a Null-DATA or QoS-Null-DATA frame
 */
static inline bool
ieee80211_is_null(const struct ieee80211_frame *wh)
{
    uint16_t mask = IEEE80211_FC0_TYPE_MASK | (IEEE80211_FC0_SUBTYPE_MASK & ~IEEE80211_FC0_SUBTYPE_QOS);
    return (wh->i_fc & mask) == (IEEE80211_FC0_TYPE_DATA | IEEE80211_FC0_SUBTYPE_NULL);
}

/**
 * Returns true if \a wh is a frame with a sequence number that is relevant.
 * Will return true for all frames except CONTROL, Null-DATA or QoS-Null-DATA
 * frames.
 */
static inline bool
ieee80211_has_seq(const struct ieee80211_frame *wh)
{
    return !ieee80211_is_type(wh, IEEE80211_FC0_TYPE_CTL) && !ieee80211_is_null(wh);
}

/**
 * Returns true if \a wh is a frame sent outside a BSS.
 */
static inline bool
ieee80211_is_nods(const struct ieee80211_frame *wh)
{
    return (wh->i_fc & IEEE80211_FC1_DIR_MASK) == IEEE80211_FC1_DIR_NODS;
}

/**
 * Returns true if \a wh is a frame sent to an AP.
 */
static inline bool
ieee80211_is_tods(const struct ieee80211_frame *wh)
{
    return (wh->i_fc & IEEE80211_FC1_DIR_MASK) == IEEE80211_FC1_DIR_TODS;
}

/**
 * Returns true if \a wh is a frame sent by an AP.
 */
static inline bool
ieee80211_is_fromds(const struct ieee80211_frame *wh)
{
    return (wh->i_fc & IEEE80211_FC1_DIR_MASK) == IEEE80211_FC1_DIR_FROMDS;
}

/**
 * Returns true if \a wh is a DS-to-DS frame
 */
static inline bool
ieee80211_is_dstods(const struct ieee80211_frame *wh)
{
    return (wh->i_fc & IEEE80211_FC1_DIR_MASK) == IEEE80211_FC1_DIR_DSTODS;
}

/**
 * Returns true if \a wh is a QoS-DATA or QoS-Null-Data frame
 */
static inline bool
ieee80211_is_qos(const struct ieee80211_frame *wh)
{
    uint16_t mask = IEEE80211_FC0_TYPE_MASK | IEEE80211_FC0_SUBTYPE_QOS;
    return (wh->i_fc & mask) == (IEEE80211_FC0_TYPE_DATA | IEEE80211_FC0_SUBTYPE_QOS);
}

/*
 * 802.11n Management Action Frames
 */

/**
 * Action management frame body
 */
struct ieee80211_action {
    uint8_t ia_category;
    uint8_t ia_action;
} __packed;

/* action category codes */
#define	IEEE80211_ACTION_SPECTRUM_MANAGEMENT   0
#define	IEEE80211_ACTION_QOS                   1
#define	IEEE80211_ACTION_DLS                   2
#define	IEEE80211_ACTION_BLOCK_ACK             3
#define	IEEE80211_ACTION_PUBLIC                4
#define	IEEE80211_ACTION_RADIO_MEASUREMENT     5
#define	IEEE80211_ACTION_FAST_BSS_TRANSITION   6
#define	IEEE80211_ACTION_HT                    7
#define	IEEE80211_ACTION_SA_QUERY              8
#define	IEEE80211_ACTION_PROTECTED_PA_DUAL     9 /* Protected Dual of Public Action */
#define	IEEE80211_ACTION_WNM                   10
#define	IEEE80211_ACTION_UNPROTECTED_WNM       11
#define	IEEE80211_ACTION_TDLS                  12
#define	IEEE80211_ACTION_MESH                  13
#define	IEEE80211_ACTION_MULTIHOP              14
#define	IEEE80211_ACTION_SELF_PROTECTED        15
#define	IEEE80211_ACTION_DMG                   16
#define	IEEE80211_ACTION_WFA                   17
#define	IEEE80211_ACTION_FAST_SESSION_TRANSFER 18
#define	IEEE80211_ACTION_ROBUST_AV_STREAMING   19
#define	IEEE80211_ACTION_UNPROTECTED_DMG       20
#define	IEEE80211_ACTION_VHT                   21
/* 22-125 reserved */
#define	IEEE80211_ACTION_VENDOR_PROTECTED      126
#define	IEEE80211_ACTION_VENDOR                127
/* 128-255 error */

#define	IEEE80211_ACTION_HT_TXCHWIDTH	0       /* recommended xmit chan width */
#define	IEEE80211_ACTION_HT_MIMOPWRSAVE	1       /* MIMO power save */

/*
 * Management Notification Frame
 */
struct ieee80211_mnf {
    uint8_t mnf_category;
    uint8_t mnf_action;
    uint8_t mnf_dialog;
    uint8_t mnf_status;
} __packed;

#define	MNF_SETUP_REQ	0
#define	MNF_SETUP_RESP	1
#define	MNF_TEARDOWN	2

/* HT - recommended transmission channel width */
struct ieee80211_action_ht_txchwidth {
    struct ieee80211_action at_header;
    uint8_t at_chwidth;
} __packed;

#define	IEEE80211_A_HT_TXCHWIDTH_20	0
#define	IEEE80211_A_HT_TXCHWIDTH_2040	1

/* HT - MIMO Power Save (NB: D2.04) */
struct ieee80211_action_ht_mimopowersave {
    struct ieee80211_action am_header;
    uint8_t am_control;
} __packed;

#define	IEEE80211_A_HT_MIMOPWRSAVE_ENA		0x01    /* PS enabled */
#define	IEEE80211_A_HT_MIMOPWRSAVE_MODE		0x02
#define	IEEE80211_A_HT_MIMOPWRSAVE_MODE_S	1
#define	IEEE80211_A_HT_MIMOPWRSAVE_DYNAMIC	0x02    /* Dynamic Mode */
#define	IEEE80211_A_HT_MIMOPWRSAVE_STATIC	0x00    /* no SM packets */
/* bits 2-7 reserved */

/* Block Ack actions */
#define IEEE80211_ACTION_BA_ADDBA_REQUEST       0       /* ADDBA request */
#define IEEE80211_ACTION_BA_ADDBA_RESPONSE      1       /* ADDBA response */
#define IEEE80211_ACTION_BA_DELBA	        2       /* DELBA */

/* Block Ack Parameter Set */
#define	IEEE80211_BAPS_BUFSIZ	0xffc0  /* buffer size */
#define	IEEE80211_BAPS_BUFSIZ_S	6
#define	IEEE80211_BAPS_TID	0x003c  /* TID */
#define	IEEE80211_BAPS_TID_S	2
#define	IEEE80211_BAPS_POLICY	0x0002  /* block ack policy */
#define	IEEE80211_BAPS_POLICY_S	1
#define	IEEE80211_BAPS_AMSDU	0x0001  /* A-MSDU supported */

#define	IEEE80211_BAPS_POLICY_DELAYED	(0<<IEEE80211_BAPS_POLICY_S)
#define	IEEE80211_BAPS_POLICY_IMMEDIATE	(1<<IEEE80211_BAPS_POLICY_S)

/* Block Ack Sequence Control */
#define	IEEE80211_BASEQ_START	0xfff0  /* starting seqnum */
#define	IEEE80211_BASEQ_START_S	4
#define	IEEE80211_BASEQ_FRAG	0x000f  /* fragment number */
#define	IEEE80211_BASEQ_FRAG_S	0

/* Delete Block Ack Parameter Set */
#define	IEEE80211_DELBAPS_TID	0xf000  /* TID */
#define	IEEE80211_DELBAPS_TID_S	12
#define	IEEE80211_DELBAPS_INIT	0x0800  /* initiator */
#define	IEEE80211_DELBAPS_INIT_S 11

/* BA - ADDBA request */
struct ieee80211_action_ba_addbarequest {
    uint8_t  rq_dialogtoken;
    uint16_t rq_baparamset;
    uint16_t rq_batimeout;      /* in TUs */
    uint16_t rq_baseqctl;
} __packed;

/* BA - ADDBA response */
struct ieee80211_action_ba_addbaresponse {
    uint8_t  rs_dialogtoken;
    uint16_t rs_statuscode;
    uint16_t rs_baparamset;
    uint16_t rs_batimeout;      /* in TUs */
} __packed;

/* BA - DELBA */
struct ieee80211_action_ba_delba {
    uint16_t dl_baparamset;
    uint16_t dl_reasoncode;
} __packed;

/* BAR Control */
#define	IEEE80211_BAR_TID	0xf000  /* TID */
#define	IEEE80211_BAR_TID_S	12
#define	IEEE80211_BAR_COMP	0x0004  /* Compressed Bitmap */
#define	IEEE80211_BAR_MTID	0x0002  /* Multi-TID */
#define	IEEE80211_BAR_NOACK	0x0001  /* No-Ack policy */

/* BAR Starting Sequence Control */
#define	IEEE80211_BAR_SEQ_START	0xfff0  /* starting seqnum */
#define	IEEE80211_BAR_SEQ_START_S	4

#define IEEE80211_BA_CTRL_ACK_POLICY 0x0001
#define IEEE80211_BA_CTRL_MULTI_TID  0x0002
#define IEEE80211_BA_CTRL_COMPRESSED 0x0004
#define IEEE80211_BA_CTRL_GCR        0x0008
#define IEEE80211_BA_CTRL_TID        0xf000
#define IEEE80211_BA_CTRL_VARIANT (IEEE80211_BA_CTRL_MULTI_TID \
                                   | IEEE80211_BA_CTRL_COMPRESSED \
                                   | IEEE80211_BA_CTRL_GCR)

enum block_ack_variant {
    IEEE80211_BA_BASIC          = 0,
    IEEE80211_BA_EXT_COMPRESSED = 1,
    IEEE80211_BA_COMPRESSED     = 2,
    IEEE80211_BA_MULTI_TID      = 3,
    IEEE80211_BA_GCR            = 6
};


enum ieee80211_sa_query_action {
    IEEE80211_SA_QUERY_REQUEST  = 0,
    IEEE80211_SA_QUERY_RESPONSE = 1
};

/*
 * Control frames.
 */

struct ieee80211_frame_min {
    uint16_t i_fc;
    uint16_t i_dur;
    uint8_t  i_addr1[IEEE80211_ADDR_LEN];
    uint8_t  i_addr2[IEEE80211_ADDR_LEN];
    /* FCS */
} __packed;

struct ieee80211_frame_rts {
    uint16_t i_fc;
    uint16_t i_dur;
    uint8_t  i_ra[IEEE80211_ADDR_LEN];
    uint8_t  i_ta[IEEE80211_ADDR_LEN];
    /* FCS */
} __packed;

struct ieee80211_frame_cts {
    uint16_t i_fc;
    uint16_t i_dur;
    uint8_t  i_ra[IEEE80211_ADDR_LEN];
    /* FCS */
} __packed;

struct ieee80211_frame_ack {
    uint16_t i_fc;
    uint16_t i_dur;
    uint8_t  i_ra[IEEE80211_ADDR_LEN];
    /* FCS */
} __packed;

struct ieee80211_frame_pspoll {
    uint16_t i_fc;
    uint16_t i_aid;
    uint8_t  i_bssid[IEEE80211_ADDR_LEN];
    uint8_t  i_ta[IEEE80211_ADDR_LEN];
    /* FCS */
} __packed;

struct ieee80211_frame_bar {
    uint16_t i_fc;
    uint16_t i_dur;
    uint8_t  i_ra[IEEE80211_ADDR_LEN];
    uint8_t  i_ta[IEEE80211_ADDR_LEN];
    uint16_t i_ctl;
    uint16_t i_seq;
    /* FCS */
} __packed;

struct ieee80211_frame_ba_basic {
    uint16_t i_fc;
    uint16_t i_dur;
    uint8_t  i_ra[IEEE80211_ADDR_LEN];
    uint8_t  i_ta[IEEE80211_ADDR_LEN];
    uint16_t i_ctl;
    uint16_t i_seq;
    uint8_t  i_bitmap[128];
    /* FCS */
} __packed;

struct ieee80211_frame_ba_comp {
    uint16_t i_fc;
    uint16_t i_dur;
    uint8_t  i_ra[IEEE80211_ADDR_LEN];
    uint8_t  i_ta[IEEE80211_ADDR_LEN];
    uint16_t i_ctl;
    uint16_t i_seq;
    uint8_t  i_bitmap[8];
    /* FCS */
} __packed;

#define	IEEE80211_BA_COMP_LEN \
    (sizeof(struct ieee80211_frame_ba_comp) + IEEE80211_CRC_LEN)

/**
 * Beacon management frame body.
 */
struct ieee80211_beacon {
    uint64_t timestamp;
    uint16_t interval;
    uint16_t capability;
} __packed;

#define IEEE80211_CAPINFO_ESS                BIT(0)
#define IEEE80211_CAPINFO_IBSS               BIT(1)
#define IEEE80211_CAPINFO_CF_POLLABLE        BIT(2)
#define IEEE80211_CAPINFO_CF_POLLREQ         BIT(3)
#define IEEE80211_CAPINFO_PRIVACY            BIT(4)
#define IEEE80211_CAPINFO_SHORT_PREAMBLE     BIT(5)
/* bits 6 and 7 are reserved */
#define IEEE80211_CAPINFO_SPECTRUM_MGMT      BIT(8)
#define IEEE80211_CAPINFO_QOS                BIT(9)
#define IEEE80211_CAPINFO_SHORT_SLOTTIME     BIT(10)
#define IEEE80211_CAPINFO_APSD               BIT(11)
#define IEEE80211_CAPINFO_RADIO_MEASUREMENT  BIT(12)
/* bit 13 is reserved */
#define IEEE80211_CAPINFO_DELAYED_BLOCKACK   BIT(14)
#define IEEE80211_CAPINFO_IMMEDIATE_BLOCKACK BIT(15)

struct ieee80211_ie_hdr {
    uint8_t id;                 /* enum ieee80211_element_id */
    uint8_t len;
    uint8_t data[0];
};

struct ieee80211_ie_ext_hdr {
    uint8_t id;                 /* enum ieee80211_element_id */
    uint8_t len;
    uint8_t ext;                /* enum ieee80211_element_id_extension */
    uint8_t data[0];
};

/*
 * Management information element payloads.
 */

enum ieee80211_element_id {
    IEEE80211_EID_SSID                            = 0,
    IEEE80211_EID_RATES                           = 1,
    IEEE80211_EID_DS_PARAMS                       = 3,
    IEEE80211_EID_CF_PARAMS                       = 4,
    IEEE80211_EID_TIM                             = 5,
    IEEE80211_EID_IBSS_PARAMS                     = 6,
    IEEE80211_EID_COUNTRY                         = 7,
    IEEE80211_EID_REQUEST                         = 10,
    IEEE80211_EID_BSS_LOAD                        = 11,
    IEEE80211_EID_EDCA_PARAM_SET                  = 12,
    IEEE80211_EID_TSPEC                           = 13,
    IEEE80211_EID_TCLAS                           = 14,
    IEEE80211_EID_SCHEDULE                        = 15,
    IEEE80211_EID_CHALLENGE                       = 16,
    IEEE80211_EID_PWR_CONSTRAINT                  = 32,
    IEEE80211_EID_PWR_CAPABILITY                  = 33,
    IEEE80211_EID_TPC_REQUEST                     = 34,
    IEEE80211_EID_TPC_REPORT                      = 35,
    IEEE80211_EID_SUPPORTED_CHANNELS              = 36,
    IEEE80211_EID_CHANNEL_SWITCH                  = 37,
    IEEE80211_EID_MEASURE_REQUEST                 = 38,
    IEEE80211_EID_MEASURE_REPORT                  = 39,
    IEEE80211_EID_QUIET                           = 40,
    IEEE80211_EID_IBSS_DFS                        = 41,
    IEEE80211_EID_ERP_INFO                        = 42,
    IEEE80211_EID_TS_DELAY                        = 43,
    IEEE80211_EID_TCLAS_PROCESSING                = 44,
    IEEE80211_EID_HT_CAP                          = 45,
    IEEE80211_EID_QOS                             = 46,
    IEEE80211_EID_RSN                             = 48,
    IEEE80211_EID_EXT_RATES                       = 50,
    IEEE80211_EID_AP_CHANNEL_REPORT               = 51,
    IEEE80211_EID_NEIGHBOR_REPORT                 = 52,
    IEEE80211_EID_RCPI                            = 53,
    IEEE80211_EID_MOBILITY_DOMAIN                 = 54,
    IEEE80211_EID_FAST_BSS_TRANSITION             = 55,
    IEEE80211_EID_TIMEOUT_INTERVAL                = 56,
    IEEE80211_EID_RIC_DATA                        = 57,
    IEEE80211_EID_DSE_REGISTERED_LOCATION         = 58,
    IEEE80211_EID_SUPPORTED_OPERATING_CLASSES     = 59,
    IEEE80211_EID_EXT_CHANSWITCH_ANN              = 60,
    IEEE80211_EID_HT_OPERATION                    = 61,
    IEEE80211_EID_SECONDARY_CHANNEL_OFFSET        = 62,
    IEEE80211_EID_BSS_AVERAGE_ACCESS_DELAY        = 63,
    IEEE80211_EID_ANTENNA                         = 64,
    IEEE80211_EID_RSNI                            = 65,
    IEEE80211_EID_MEASUREMENT_PILOT_TRANSMISSION  = 66,
    IEEE80211_EID_BSS_AVAILABLE_ADM_CAPA          = 67,
    IEEE80211_EID_BSS_AC_ACCESS_DELAY             = 68,
    IEEE80211_EID_TIME_ADVERTISEMENT              = 69,
    IEEE80211_EID_RRM_ENABLED_CAPABILITIES        = 70,
    IEEE80211_EID_MULTIPLE_BSSID                  = 71,
    IEEE80211_EID_20_40_BSS_COEXISTENCE           = 72,
    IEEE80211_EID_20_40_BSS_INTOLERANT            = 73,
    IEEE80211_EID_OVERLAPPING_BSS_SCAN_PARAMS     = 74,
    IEEE80211_EID_RIC_DESCRIPTOR                  = 75,
    IEEE80211_EID_MME                             = 76,
    IEEE80211_EID_EVENT_REQUEST                   = 78,
    IEEE80211_EID_EVENT_REPORT                    = 79,
    IEEE80211_EID_DIAGNOSTIC_REQUEST              = 80,
    IEEE80211_EID_DIAGNOSTIC_REPORT               = 81,
    IEEE80211_EID_LOCATION_PARAMETERS             = 82,
    IEEE80211_EID_NONTRANSMITTED_BSSID_CAPA       = 83,
    IEEE80211_EID_SSID_LIST                       = 84,
    IEEE80211_EID_MLTIPLE_BSSID_INDEX             = 85,
    IEEE80211_EID_FMS_DESCRIPTOR                  = 86,
    IEEE80211_EID_FMS_REQUEST                     = 87,
    IEEE80211_EID_FMS_RESPONSE                    = 88,
    IEEE80211_EID_QOS_TRAFFIC_CAPABILITY          = 89,
    IEEE80211_EID_BSS_MAX_IDLE_PERIOD             = 90,
    IEEE80211_EID_TFS_REQ                         = 91,
    IEEE80211_EID_TFS_RESP                        = 92,
    IEEE80211_EID_WNMSLEEP                        = 93,
    IEEE80211_EID_TIM_BROADCAST_REQUEST           = 94,
    IEEE80211_EID_TIM_BROADCAST_RESPONSE          = 95,
    IEEE80211_EID_COLLOCATED_INTERFERENCE_REPORT  = 96,
    IEEE80211_EID_CHANNEL_USAGE                   = 97,
    IEEE80211_EID_TIME_ZONE                       = 98,
    IEEE80211_EID_DMS_REQUEST                     = 99,
    IEEE80211_EID_DMS_RESPONSE                    = 100,
    IEEE80211_EID_LINK_ID                         = 101,
    IEEE80211_EID_WAKEUP_SCHEDULE                 = 102,
    IEEE80211_EID_CHANNEL_SWITCH_TIMING           = 104,
    IEEE80211_EID_PTI_CONTROL                     = 105,
    IEEE80211_EID_TPU_BUFFER_STATUS               = 106,
    IEEE80211_EID_INTERWORKING                    = 107,
    IEEE80211_EID_ADV_PROTO                       = 108,
    IEEE80211_EID_EXPEDITED_BANDWIDTH_REQ         = 109,
    IEEE80211_EID_QOS_MAP_SET                     = 110,
    IEEE80211_EID_ROAMING_CONSORTIUM              = 111,
    IEEE80211_EID_EMERGENCY_ALERT_ID              = 112,
    IEEE80211_EID_MESH_CONFIG                     = 113,
    IEEE80211_EID_MESH_ID                         = 114,
    IEEE80211_EID_MESH_LINK_METRIC_REPORT         = 115,
    IEEE80211_EID_CONGESTION_NOTIFICATION         = 116,
    IEEE80211_EID_PEER_MGMT                       = 117,
    IEEE80211_EID_MESH_CHANNEL_SWITCH_PARAMETERS  = 118,
    IEEE80211_EID_MESH_AWAKE_WINDOW               = 119,
    IEEE80211_EID_BEACON_TIMING                   = 120,
    IEEE80211_EID_MCCAOP_SETUP_REQUEST            = 121,
    IEEE80211_EID_MCCAOP_SETUP_REPLY              = 122,
    IEEE80211_EID_MCCAOP_ADVERTISEMENT            = 123,
    IEEE80211_EID_MCCAOP_TEARDOWN                 = 124,
    IEEE80211_EID_GANN                            = 125,
    IEEE80211_EID_RANN                            = 126,
    IEEE80211_EID_EXT_CAPAB                       = 127,
    IEEE80211_EID_PREQ                            = 130,
    IEEE80211_EID_PREP                            = 131,
    IEEE80211_EID_PERR                            = 132,
    IEEE80211_EID_PXU                             = 137,
    IEEE80211_EID_PXUC                            = 138,
    IEEE80211_EID_AMPE                            = 139,
    IEEE80211_EID_MIC                             = 140,
    IEEE80211_EID_DESTINATION_URI                 = 141,
    IEEE80211_EID_U_APSD_COEX                     = 142,
    IEEE80211_EID_DMG_WAKEUP_SCHEDULE             = 143,
    IEEE80211_EID_EXTENDED_SCHEDULE               = 144,
    IEEE80211_EID_STA_AVAILABILITY                = 145,
    IEEE80211_EID_DMG_TSPEC                       = 146,
    IEEE80211_EID_NEXT_DMG_ATI                    = 147,
    IEEE80211_EID_DMG_CAPABILITIES                = 148,
    IEEE80211_EID_DMG_OPERATION                   = 151,
    IEEE80211_EID_DMG_BSS_PARAMETER_CHANGE        = 152,
    IEEE80211_EID_DMG_BEAM_REFINEMENT             = 153,
    IEEE80211_EID_CHANNEL_MEASUREMENT_FEEDBACK    = 154,
    IEEE80211_EID_CCKM                            = 156,
    IEEE80211_EID_AWAKE_WINDOW                    = 157,
    IEEE80211_EID_MULTI_BAND                      = 158,
    IEEE80211_EID_ADDBA_EXTENSION                 = 159,
    IEEE80211_EID_NEXTPCP_LIST                    = 160,
    IEEE80211_EID_PCP_HANDOVER                    = 161,
    IEEE80211_EID_DMG_LINK_MARGIN                 = 162,
    IEEE80211_EID_SWITCHING_STREAM                = 163,
    IEEE80211_EID_SESSION_TRANSITION              = 164,
    IEEE80211_EID_DYNAMIC_TONE_PAIRING_REPORT     = 165,
    IEEE80211_EID_CLUSTER_REPORT                  = 166,
    IEEE80211_EID_REPLAY_CAPABILITIES             = 167,
    IEEE80211_EID_RELAY_TRANSFER_PARAM_SET        = 168,
    IEEE80211_EID_BEAMLINK_MAINTENANCE            = 169,
    IEEE80211_EID_MULTIPLE_MAC_SUBLAYERS          = 170,
    IEEE80211_EID_U_PID                           = 171,
    IEEE80211_EID_DMG_LINK_ADAPTATION_ACK         = 172,
    IEEE80211_EID_MCCAOP_ADVERTISEMENT_OVERVIEW   = 174,
    IEEE80211_EID_QUIET_PERIOD_REQUEST            = 175,
    IEEE80211_EID_QUIET_PERIOD_RESPONSE           = 177,
    IEEE80211_EID_QMF_POLICY                      = 181,
    IEEE80211_EID_ECAPC_POLICY                    = 182,
    IEEE80211_EID_CLUSTER_TIME_OFFSET             = 183,
    IEEE80211_EID_INTRA_ACCESS_CATEGORY_PRIORITY  = 184,
    IEEE80211_EID_SCS_DESCRIPTOR                  = 185,
    IEEE80211_EID_QLOAD_REPORT                    = 186,
    IEEE80211_EID_HCCA_TXOP_UPDATE_COUNT          = 187,
    IEEE80211_EID_HIGHER_LAYER_STREAM_ID          = 188,
    IEEE80211_EID_GCR_GROUP_ADDRESS               = 189,
    IEEE80211_EID_ANTENNA_SECTOR_ID_PATTERN       = 190,
    IEEE80211_EID_VHT_CAP                         = 191,
    IEEE80211_EID_VHT_OPERATION                   = 192,
    IEEE80211_EID_VHT_EXTENDED_BSS_LOAD           = 193,
    IEEE80211_EID_VHT_WIDE_BW_CHSWITCH            = 194,
    IEEE80211_EID_VHT_TRANSMIT_POWER_ENVELOPE     = 195,
    IEEE80211_EID_VHT_CHANNEL_SWITCH_WRAPPER      = 196,
    IEEE80211_EID_VHT_AID                         = 197,
    IEEE80211_EID_VHT_QUIET_CHANNEL               = 198,
    IEEE80211_EID_VHT_OPERATING_MODE_NOTIFICATION = 199,
    IEEE80211_EID_UPSIM                           = 200,
    IEEE80211_EID_REDUCED_NEIGHBOR_REPORT         = 201,
    IEEE80211_EID_TVHT_OPERATION                  = 202,
    IEEE80211_EID_DEVICE_LOCATION                 = 204,
    IEEE80211_EID_WHITE_SPACE_MAP                 = 205,
    IEEE80211_EID_FTM_PARAMETERS                  = 206,
    IEEE80211_EID_VENDOR_SPECIFIC                 = 221,
    IEEE80211_EID_CAG_NUMBER                      = 237,
    IEEE80211_EID_AP_CSN                          = 239,
    IEEE80211_EID_FILS_INDICATION                 = 240,
    IEEE80211_EID_DILS                            = 241,
    IEEE80211_EID_FRAGMENT                        = 242,
    IEEE80211_EID_EXTENSION                       = 255,
};

enum ieee80211_element_id_extension {
    IEEE80211_EIDE_FTM_SYNCHRONIZATION_INFORMATION = 9,
    IEEE80211_EIDE_EXTENDED_REQUEST                = 10,
    IEEE80211_EIDE_ESTIMATED_SERVICE_PARAMETERS    = 11,
    IEEE80211_EIDE_FUTURE_CHANNEL_GUIDANCE         = 14,
    IEEE80211_EIDE_PASSWORD_IDENTIFIER             = 33,
};

#define	IEEE80211_NWID_LEN   32

struct ieee80211_ssid_ie {
    uint8_t ie;                 /* M80211_ELEMID_SSID */
    uint8_t len;
    uint8_t ssid[0];
} __packed;

struct ieee80211_tim_ie {
    uint8_t tim_ie;             /* IEEE80211_EID_TIM */
    uint8_t tim_len;
    uint8_t tim_count;          /* DTIM count */
    uint8_t tim_period;         /* DTIM period */
    uint8_t tim_ctrl;           /* bitmap control */
    uint8_t tim_bitmap[1];      /* variable-length bitmap */
} __packed;

struct ieee80211_country_ie {
    uint8_t ie;                 /* IEEE80211_ELEMID_COUNTRY */
    uint8_t len;
    uint8_t cc[3];              /* ISO CC+(I)ndoor/(O)utdoor */
    struct {
        uint8_t schan;          /* starting channel */
        uint8_t nchan;          /* number channels */
        uint8_t maxtxpwr;       /* tx power cap */
    } __packed band[1];         /* sub bands (NB: var size) */
} __packed;

#define IEEE80211_COUNTRY_MAX_BANDS     84      /* max possible bands */
#define IEEE80211_COUNTRY_MAX_SIZE \
    (sizeof(struct ieee80211_country_ie) + 3*(IEEE80211_COUNTRY_MAX_BANDS-1))

/* maximum channel number supported by this element; the rest of the
 * elements are Operating/Subbband Sequence */
#define IEEE80211_COUNTRY_MAX_SUBBAND_CHANNEL 200

struct ieee80211_bss_max_idle_period {
    uint8_t ie;                 /* IEEE80211_EID_BSS_MAX_IDLE_PERIOD */
    uint8_t len;
    uint16_t max_idle_period;
    uint8_t idle_options;
} __packed;

/*
 * 802.11i/WPA information element (maximally sized).
 */
struct ieee80211_ie_wpa {
    uint8_t  wpa_id;            /* IEEE80211_ELEMID_VENDOR */
    uint8_t  wpa_len;           /* length in bytes */
    uint8_t  wpa_oui[3];        /* 0x00, 0x50, 0xf2 */
    uint8_t  wpa_type;          /* OUI type */
    uint16_t wpa_version;       /* spec revision */
    uint32_t wpa_mcipher[1];    /* multicast/group key cipher */
    uint16_t wpa_uciphercnt;    /* # pairwise key ciphers */
    uint32_t wpa_uciphers[8];   /* ciphers */
    uint16_t wpa_authselcnt;    /* authentication selector cnt */
    uint32_t wpa_authsels[8];   /* selectors */
    uint16_t wpa_caps;          /* 802.11i capabilities */
    uint16_t wpa_pmkidcnt;      /* 802.11i pmkid count */
    uint16_t wpa_pmkids[8];     /* 802.11i pmkids */
} __packed;

/*
 * 802.11n HT Capability IE
 */
struct ieee80211_ie_htcap {
    uint8_t  hc_id;             /* element ID */
    uint8_t  hc_len;            /* length in bytes */
    uint16_t hc_cap;            /* HT caps (see below) */
    uint8_t  hc_param;          /* A-MPDU parameters */
    uint8_t  hc_mcsset[16];     /* supported MCS set */
    uint16_t hc_extcap;         /* extended HT capabilities */
    uint32_t hc_txbf;           /* txbf capabilities */
    uint8_t  hc_antenna;        /* antenna capabilities */
} __packed;

/* HT capability flags (ht_cap) */
#define	IEEE80211_HTCAP_LDPC		0x0001  /* LDPC supported */
#define	IEEE80211_HTCAP_CHWIDTH40	0x0002  /* 20/40 supported */
#define	IEEE80211_HTCAP_SMPS		0x000c  /* SM Power Save mode */
#define	IEEE80211_HTCAP_SMPS_OFF	0x000c  /* disabled */
#define	IEEE80211_HTCAP_SMPS_DYNAMIC	0x0004  /* send RTS first */
/* NB: SMPS value 2 is reserved */
#define	IEEE80211_HTCAP_SMPS_ENA	0x0000  /* enabled (static mode) */
#define	IEEE80211_HTCAP_GREENFIELD	0x0010  /* Greenfield supported */
#define	IEEE80211_HTCAP_SHORTGI20	0x0020  /* Short GI in 20MHz */
#define	IEEE80211_HTCAP_SHORTGI40	0x0040  /* Short GI in 40MHz */
#define	IEEE80211_HTCAP_TXSTBC		0x0080  /* STBC tx ok */
#define	IEEE80211_HTCAP_RXSTBC		0x0300  /* STBC rx support */
#define	IEEE80211_HTCAP_RXSTBC_S	8
#define	IEEE80211_HTCAP_RXSTBC_1STREAM	0x0100  /* 1 spatial stream */
#define	IEEE80211_HTCAP_RXSTBC_2STREAM	0x0200  /* 1-2 spatial streams */
#define	IEEE80211_HTCAP_RXSTBC_3STREAM	0x0300  /* 1-3 spatial streams */
#define	IEEE80211_HTCAP_DELAYED_BA	0x0400  /* HT-Delayed block ack */
#define	IEEE80211_HTCAP_MAXAMSDU	0x0800  /* max A-MSDU length */
#define	IEEE80211_HTCAP_MAXAMSDU_7935	0x0800  /* 7935 octets */
#define	IEEE80211_HTCAP_MAXAMSDU_3839	0x0000  /* 3839 octets */
#define	IEEE80211_HTCAP_DSSSCCK40	0x1000  /* DSSS/CCK in 40MHz */
#define	IEEE80211_HTCAP_40INTOLERANT	0x4000  /* 40MHz intolerant */
#define	IEEE80211_HTCAP_LSIGTXOPPROT	0x8000  /* L-SIG TXOP prot */

/* HT A-MPDU parameters (hc_param) */
#define	IEEE80211_HTCAP_MAXRXAMPDU	0x03    /* max rx A-MPDU factor */
#define	IEEE80211_HTCAP_MAXRXAMPDU_S	0
#define	IEEE80211_HTCAP_MAXRXAMPDU_8K	0
#define	IEEE80211_HTCAP_MAXRXAMPDU_16K	1
#define	IEEE80211_HTCAP_MAXRXAMPDU_32K	2
#define	IEEE80211_HTCAP_MAXRXAMPDU_64K	3
#define	IEEE80211_HTCAP_MPDUDENSITY	0x1c    /* min MPDU start spacing */
#define	IEEE80211_HTCAP_MPDUDENSITY_S	2
#define	IEEE80211_HTCAP_MPDUDENSITY_NA	0       /* no time restriction */
#define	IEEE80211_HTCAP_MPDUDENSITY_025	1       /* 1/4 us */
#define	IEEE80211_HTCAP_MPDUDENSITY_05	2       /* 1/2 us */
#define	IEEE80211_HTCAP_MPDUDENSITY_1	3       /* 1 us */
#define	IEEE80211_HTCAP_MPDUDENSITY_2	4       /* 2 us */
#define	IEEE80211_HTCAP_MPDUDENSITY_4	5       /* 4 us */
#define	IEEE80211_HTCAP_MPDUDENSITY_8	6       /* 8 us */
#define	IEEE80211_HTCAP_MPDUDENSITY_16	7       /* 16 us */

/* HT extended capabilities (hc_extcap) */
#define	IEEE80211_HTCAP_PCO		0x0001  /* PCO capable */
#define	IEEE80211_HTCAP_PCOTRANS	0x0006  /* PCO transition time */
#define	IEEE80211_HTCAP_PCOTRANS_S	1
#define	IEEE80211_HTCAP_PCOTRANS_04	0x0002  /* 400 us */
#define	IEEE80211_HTCAP_PCOTRANS_15	0x0004  /* 1.5 ms */
#define	IEEE80211_HTCAP_PCOTRANS_5	0x0006  /* 5 ms */
/* bits 3-7 reserved */
#define	IEEE80211_HTCAP_MCSFBACK	0x0300  /* MCS feedback */
#define	IEEE80211_HTCAP_MCSFBACK_S	8
#define	IEEE80211_HTCAP_MCSFBACK_NONE	0x0000  /* nothing provided */
#define	IEEE80211_HTCAP_MCSFBACK_UNSOL	0x0200  /* unsolicited feedback */
#define	IEEE80211_HTCAP_MCSFBACK_MRQ	0x0300  /* " "+respond to MRQ */
#define	IEEE80211_HTCAP_HTC		0x0400  /* +HTC support */
#define	IEEE80211_HTCAP_RDR		0x0800  /* reverse direction responder */
/* bits 12-15 reserved */

/*
 * 802.11n HT Operation IE
 */
struct ieee80211_ie_htoper {
    uint8_t ho_id;              /* element ID */
    uint8_t ho_len;             /* length in bytes */
    uint8_t ho_primary_channel; /* primary channel */
    uint8_t ho_op_info[5];      /* operation information */
    uint8_t ho_basicmcsset[16]; /* basic MCS set */
} __packed;

/* byte1 */
#define	IEEE80211_HTOPER_2NDCHAN	0x03    /* secondary/ext chan offset */
#define	IEEE80211_HTOPER_2NDCHAN_S	0
#define	IEEE80211_HTOPER_2NDCHAN_NONE	0x00    /* no secondary/ext channel */
#define	IEEE80211_HTOPER_2NDCHAN_ABOVE	0x01    /* above private channel */
/* NB: 2 is reserved */
#define	IEEE80211_HTOPER_2NDCHAN_BELOW	0x03    /* below primary channel */
#define	IEEE80211_HTOPER_TXWIDTH	0x04    /* tx channel width */
#define	IEEE80211_HTOPER_TXWIDTH_20	0x00    /* 20MHz width */
#define	IEEE80211_HTOPER_TXWIDTH_2040	0x04    /* any supported width */
#define	IEEE80211_HTOPER_RIFSMODE	0x08    /* Reduced IFS (RIFS) use */
#define	IEEE80211_HTOPER_RIFSMODE_PROH	0x00    /* RIFS use prohibited */
#define	IEEE80211_HTOPER_RIFSMODE_PERM	0x08    /* RIFS use permitted */
#define	IEEE80211_HTOPER_PMSPONLY	0x10    /* PSMP required to associate */
#define	IEEE80211_HTOPER_SIGRAN		0xe0    /* shortest Service Interval */
#define	IEEE80211_HTOPER_SIGRAN_S	5
#define	IEEE80211_HTOPER_SIGRAN_5	0x00    /* 5 ms */
/* XXX add rest */

/* bytes 2+3 */
#define	IEEE80211_HTOPER_OPMODE		0x03    /* operating mode */
#define	IEEE80211_HTOPER_OPMODE_S	0
#define	IEEE80211_HTOPER_OPMODE_PURE	0x00    /* no protection */
#define	IEEE80211_HTOPER_OPMODE_PROTOPT	0x01    /* protection optional */
#define	IEEE80211_HTOPER_OPMODE_HT20PR	0x02    /* protection for HT20 sta's */
#define	IEEE80211_HTOPER_OPMODE_MIXED	0x03    /* protection for legacy sta's */
#define	IEEE80211_HTOPER_NONGF_PRESENT	0x04    /* non-GF sta's present */
#define	IEEE80211_HTOPER_TXBL		0x08    /* transmit burst limit */
#define	IEEE80211_HTOPER_NONHT_PRESENT	0x10    /* non-HT sta's present */
/* bits 5-15 reserved */

/* bytes 4+5 */
#define	IEEE80211_HTOPER_2NDARYBEACON	0x01
#define	IEEE80211_HTOPER_LSIGTXOPPROT	0x02
#define	IEEE80211_HTOPER_PCO_ACTIVE	0x04
#define	IEEE80211_HTOPER_40MHZPHASE	0x08

/* byte5 */
#define	IEEE80211_HTOPER_BASIC_STBCMCS	0x7f
#define	IEEE80211_HTOPER_BASIC_STBCMCS_S 0
#define	IEEE80211_HTOPER_DUALPROTECTED	0x80

/*
 * 802.11h Quiet Time Element.
 */
struct ieee80211_quiet_ie {
    uint8_t quiet_ie;           /* IEEE80211_ELEMID_QUIET */
    uint8_t len;
    uint8_t tbttcount;          /* quiet start */
    uint8_t period;             /* beacon intervals between quiets */
    uint16_t duration;          /* TUs of each quiet */
    uint16_t offset;            /* TUs of from TBTT of quiet start */
} __packed;

/*
 * 802.11h Channel Switch Announcement (CSA).
 */
struct ieee80211_csa_ie {
    uint8_t csa_ie;             /* IEEE80211_ELEMID_CHANSWITCHANN */
    uint8_t csa_len;
    uint8_t csa_mode;           /* Channel Switch Mode */
    uint8_t csa_newchan;        /* New Channel Number */
    uint8_t csa_count;          /* Channel Switch Count */
} __packed;

struct ieee80211_mme_ie {
    uint8_t  mme_id;
    uint8_t  mme_len;
    uint16_t mme_key_id;
    uint8_t  mme_ipn[6];
    uint8_t  mme_mic[16];       /* 8 or 16 bytes */
} __packed;

#define IEEE80211_MME_KEY_ID_MASK 0x0fff

#define QOS_INFO_AP_EDCA_UPDATE_COUNT BITMASK2(3,0) /* Mask for update count */
#define QOS_INFO_AP_11E_QACK          BIT(4) /* reserved in WMM */
#define QOS_INFO_AP_11E_QUEUE_REQ     BIT(5) /* reserved in WMM */
#define QOS_INFO_AP_11E_TXOP_REQ      BIT(6) /* reserved in WMM */
#define QOS_INFO_AP_WMM_UAPSD         BIT(7) /* reserved in 11e */

#define QOS_INFO_AP_11E_MASK (QOS_INFO_AP_EDCA_UPDATE_COUNT | QOS_INFO_AP_11E_QACK \
                              | QOS_INFO_AP_11E_QUEUE_REQ | QOS_INFO_AP_11E_TXOP_REQ)
#define QOS_INFO_AP_WMM_MASK (QOS_INFO_AP_EDCA_UPDATE_COUNT | QOS_INFO_AP_WMM_UAPSD)

#define QOS_INFO_STA_UAPSD_VO         BIT(0)
#define QOS_INFO_STA_UAPSD_VI         BIT(1)
#define QOS_INFO_STA_UAPSD_BK         BIT(2)
#define QOS_INFO_STA_UAPSD_BE         BIT(3)
#define QOS_INFO_STA_QACK             BIT(4) /* reserved in WMM */
#define QOS_INFO_STA_SP_LEN           BITMASK2(6,5) /* Mask for SP-len */
#define QOS_INFO_STA_SP_LEN_ALL 0
#define QOS_INFO_STA_SP_LEN_2   1
#define QOS_INFO_STA_SP_LEN_4   2
#define QOS_INFO_STA_SP_LEN_6   3
#define QOS_INFO_STA_MORE_DATA_ACK    BIT(7) /* reserved in WMM */

struct ieee80211_qos_capability {
    uint8_t qos_id;             /* IEEE80211_EID_QOS */
    uint8_t qos_len;            /* length in bytes */
    uint8_t qos_info;           /* QoS info */
} __packed;

/*
 * WME/802.11e information element.
 */
struct ieee80211_wme_info {
    uint8_t wme_id;             /* IEEE80211_ELEMID_VENDOR */
    uint8_t wme_len;            /* length in bytes */
    uint8_t wme_oui[3];         /* 0x00, 0x50, 0xf2 */
    uint8_t wme_type;           /* OUI type */
    uint8_t wme_subtype;        /* OUI subtype */
    uint8_t wme_version;        /* spec revision */
    uint8_t wme_info;           /* QoS info */
} __packed;

/*
 * WME/802.11e Tspec Element
 */
struct ieee80211_wme_tspec {
    uint8_t ts_id;
    uint8_t ts_len;
    uint8_t ts_oui[3];
    uint8_t ts_oui_type;
    uint8_t ts_oui_subtype;
    uint8_t ts_version;
    uint8_t ts_tsinfo[3];
    uint8_t ts_nom_msdu[2];
    uint8_t ts_max_msdu[2];
    uint8_t ts_min_svc[4];
    uint8_t ts_max_svc[4];
    uint8_t ts_inactv_intv[4];
    uint8_t ts_susp_intv[4];
    uint8_t ts_start_svc[4];
    uint8_t ts_min_rate[4];
    uint8_t ts_mean_rate[4];
    uint8_t ts_max_burst[4];
    uint8_t ts_min_phy[4];
    uint8_t ts_peak_rate[4];
    uint8_t ts_delay[4];
    uint8_t ts_surplus[2];
    uint8_t ts_medium_time[2];
} __packed;

/*
 * WME AC parameter field
 */
#define WME_NUM_AC		4       /* 4 AC categories */
#define	WME_NUM_TID		16      /* 16 tids */

/* this is used in various places */
#define IEEE80211_TID_SIZE      (WME_NUM_TID+1) /* WME TID's +1 for non-QoS */
#define IEEE80211_NONQOS_TID    WME_NUM_TID     /* index for non-QoS sta */

#define WME_AC_TO_TID(_ac) (       \
	((_ac) == WME_AC_VO) ? 6 : \
	((_ac) == WME_AC_VI) ? 5 : \
	((_ac) == WME_AC_BK) ? 1 : \
	0)

union __attribute__ ((__transparent_union__)) edca_parameter {
    struct {
        uint32_t aifsn:4;
        uint32_t acm:1;
        uint32_t aci:2;
        uint32_t reserved:1;
        uint32_t ecwmin:4;
        uint32_t ecwmax:4;
        uint32_t txop_limit:16;
    };
    uint32_t record;
};

union __attribute__ ((__transparent_union__)) edca_qos_info {
    struct {
        uint8_t update_count:4;
        uint8_t qack:1;
        uint8_t qrequest:1;
        uint8_t txop_request:1;
        uint8_t wmm_uapsd:1;
    };
    uint8_t qos_info;
};

#define EDCA_PARAMETER(_aci, _acm, _aifsn, _ecwmin, _ecwmax, _txop_limit) ({ \
            union edca_parameter __p = {                                \
                .aifsn = (_aifsn),                                      \
                .acm = (_acm),                                          \
                .aci = (_aci),                                          \
                .ecwmin = (_ecwmin),                                    \
                .ecwmax = (_ecwmax),                                    \
                .txop_limit = (_txop_limit)                             \
            };                                                          \
            __p;                                                        \
        })

#define EDCA_DEFAULT_BE \
    EDCA_PARAMETER(ACI_BE, 0, 3, A_ECWMIN, A_ECWMAX, 0)
#define EDCA_DEFAULT_BK \
    EDCA_PARAMETER(ACI_BK, 0, 7, A_ECWMIN, A_ECWMAX, 0)
#define EDCA_DEFAULT_VI \
    EDCA_PARAMETER(ACI_VI, 0, 2, A_ECWMIN-1, A_ECWMIN, IEEE80211_US_TO_TXOP(3008))
#define EDCA_DEFAULT_VO \
    EDCA_PARAMETER(ACI_VO, 0, 2, A_ECWMIN-2, A_ECWMIN-1, IEEE80211_US_TO_TXOP(1504))

/*
 * WME Parameter Element
 */
struct ieee80211_wme_param {
    uint8_t param_id;
    uint8_t param_len;
    uint8_t param_oui[3];
    uint8_t param_oui_type;
    uint8_t param_oui_subtype;
    uint8_t param_version;
    uint8_t param_qosInfo;
#define	WME_QOSINFO_COUNT	0x0f    /* Mask for param count field */
    uint8_t param_reserved;
    union edca_parameter params_acParams[WME_NUM_AC];
} __packed;

struct ieee80211_edca_param {
    uint8_t              edca_id;
    uint8_t              edca_len;
    uint8_t              edca_qos_info;
    uint8_t              edca_reserved;
    union edca_parameter edca_param[WME_NUM_AC];
} __packed;

/*
 * Note the min acceptable CSA count is used to guard against
 * malicious CSA injection in station mode.  Defining this value
 * as other than 0 violates the 11h spec.
 */
#define	IEEE80211_CSA_COUNT_MIN	2
#define	IEEE80211_CSA_COUNT_MAX	255

/* rate set entries are in .5 Mb/s units, and potentially marked as basic */
#define	IEEE80211_RATE_BASIC		0x80
#define	IEEE80211_RATE_VAL		0x7f
#define	IEEE80211_RV(v)			((v) & IEEE80211_RATE_VAL)

#define IEEE80211_BSS_MEMBERSHIP_HT  127
#define IEEE80211_BSS_MEMBERSHIP_VHT 126

struct ieee80211_erp_ie {
    uint8_t erp_id;
    uint8_t erp_len;
    uint8_t erp_parameters;
} __packed;

/* ERP information element flags */
#define	IEEE80211_ERP_NON_ERP_PRESENT	0x01
#define	IEEE80211_ERP_USE_PROTECTION	0x02
#define	IEEE80211_ERP_LONG_PREAMBLE	0x04

#define	ATH_OUI			0x7f0300        /* Atheros OUI */
#define	ATH_OUI_TYPE		0x01    /* Atheros protocol ie */

/* NB: Atheros allocated the OUI for this purpose ~2005 but beware ... */
#define	TDMA_OUI		ATH_OUI
#define	TDMA_OUI_TYPE		0x02    /* TDMA protocol ie */

#define	BCM_OUI			0x4c9000        /* Broadcom OUI */
#define	BCM_OUI_HTCAP		51      /* pre-draft HTCAP ie */
#define	BCM_OUI_HTINFO		52      /* pre-draft HTINFO ie */

#define	WPA_OUI			0xf25000
#define	WPA_OUI_TYPE		0x01
#define	WPA_VERSION		1       /* current supported version */

#define	WPA_CSE_NULL		0x00
#define	WPA_CSE_WEP40		0x01
#define	WPA_CSE_TKIP		0x02
#define	WPA_CSE_CCMP		0x04
#define	WPA_CSE_WEP104		0x05

#define	WPA_ASE_NONE		0x00
#define	WPA_ASE_8021X_UNSPEC	0x01
#define	WPA_ASE_8021X_PSK	0x02

#define	WPS_OUI_TYPE		0x04

#define	RSN_OUI			0xac0f00
#define	RSN_VERSION		1       /* current supported version */

#define	RSN_CSE_NULL		0x00
#define	RSN_CSE_WEP40		0x01
#define	RSN_CSE_TKIP		0x02
#define	RSN_CSE_WRAP		0x03
#define	RSN_CSE_CCMP		0x04
#define	RSN_CSE_WEP104		0x05

#define	RSN_ASE_NONE		0x00
#define	RSN_ASE_8021X_UNSPEC	0x01
#define	RSN_ASE_8021X_PSK	0x02

#define	RSN_CAP_PREAUTH		0x01

#define	WME_OUI			0xf25000
#define	WME_OUI_TYPE		0x02
#define	WME_INFO_OUI_SUBTYPE	0x00
#define	WME_PARAM_OUI_SUBTYPE	0x01
#define	WME_VERSION		1

/* WME stream classes */
enum edca_aci {
    ACI_BE = 0,              /* best effort */
    ACI_BK = 1,              /* background */
    ACI_VI = 2,              /* video */
    ACI_VO = 3               /* voice */
};

/* backwards compat */
#define WME_AC_BE ACI_BE
#define WME_AC_BK ACI_BK
#define WME_AC_VI ACI_VI
#define WME_AC_VO ACI_VO

/* convert ACI to priority order (and back) */
#define ACI_PRIO_CONV(_aci)  ((_aci) ^ ((_aci) >> 1) ^ 1)

/*
 * AUTH management packets
 *
 *	octet algo[2]
 *	octet seq[2]
 *	octet status[2]
 *	octet chal.id
 *	octet chal.length
 *	octet chal.text[253]		NB: 1-253 bytes
 */

/* challenge length for shared key auth */
#define IEEE80211_CHALLENGE_LEN		128

enum ieee80211_auth_alg {
    IEEE80211_AUTH_ALG_OPEN   = 0,
    IEEE80211_AUTH_ALG_SHARED = 1,
    IEEE80211_AUTH_ALG_FT     = 2,
    IEEE80211_AUTH_ALG_SAE    = 3
};

enum {
    IEEE80211_AUTH_OPEN_REQUEST     = 1,
    IEEE80211_AUTH_OPEN_RESPONSE    = 2,
};

enum {
    IEEE80211_AUTH_SHARED_REQUEST   = 1,
    IEEE80211_AUTH_SHARED_CHALLENGE = 2,
    IEEE80211_AUTH_SHARED_RESPONSE  = 3,
    IEEE80211_AUTH_SHARED_PASS      = 4,
};

/**
 * Reason codes.
 *
 * Reason codes are used in management frames to indicate why an action took
 * place (e.g. on disassociation).
 *
 * Codes from IEEE Std 802.11-2016, Table 9-45
 */
enum ieee80211_reason_code {
    /*[[[cog
    from bits import enumpub
    for x in enumpub.IEEE80211ReasonCode:
        cog.outl(f'IEEE80211_REASON_{x.name:<40} = {x.value},')
    ]]]*/
    IEEE80211_REASON_UNSPECIFIED                              = 1,
    IEEE80211_REASON_PREV_AUTH_NOT_VALID                      = 2,
    IEEE80211_REASON_DEAUTH_LEAVING                           = 3,
    IEEE80211_REASON_DISASSOC_DUE_TO_INACTIVITY               = 4,
    IEEE80211_REASON_DISASSOC_AP_BUSY                         = 5,
    IEEE80211_REASON_CLASS2_FRAME_FROM_NONAUTH_STA            = 6,
    IEEE80211_REASON_CLASS3_FRAME_FROM_NONASSOC_STA           = 7,
    IEEE80211_REASON_DISASSOC_STA_HAS_LEFT                    = 8,
    IEEE80211_REASON_STA_REQ_ASSOC_WITHOUT_AUTH               = 9,
    IEEE80211_REASON_PWR_CAPABILITY_NOT_VALID                 = 10,
    IEEE80211_REASON_SUPPORTED_CHANNEL_NOT_VALID              = 11,
    IEEE80211_REASON_BSS_TRANSITION_DISASSOC                  = 12,
    IEEE80211_REASON_INVALID_IE                               = 13,
    IEEE80211_REASON_MICHAEL_MIC_FAILURE                      = 14,
    IEEE80211_REASON_HANDSHAKE_4WAY_TIMEOUT                   = 15,
    IEEE80211_REASON_GROUP_KEY_UPDATE_TIMEOUT                 = 16,
    IEEE80211_REASON_IE_IN_4WAY_DIFFERS                       = 17,
    IEEE80211_REASON_GROUP_CIPHER_NOT_VALID                   = 18,
    IEEE80211_REASON_PAIRWISE_CIPHER_NOT_VALID                = 19,
    IEEE80211_REASON_AKMP_NOT_VALID                           = 20,
    IEEE80211_REASON_UNSUPPORTED_RSN_IE_VERSION               = 21,
    IEEE80211_REASON_INVALID_RSN_IE_CAPAB                     = 22,
    IEEE80211_REASON_IEEE_802_1X_AUTH_FAILED                  = 23,
    IEEE80211_REASON_CIPHER_SUITE_REJECTED                    = 24,
    IEEE80211_REASON_TDLS_TEARDOWN_UNREACHABLE                = 25,
    IEEE80211_REASON_TDLS_TEARDOWN_UNSPECIFIED                = 26,
    IEEE80211_REASON_SSP_REQUESTED_DISASSOC                   = 27,
    IEEE80211_REASON_NO_SSP_ROAMING_AGREEMENT                 = 28,
    IEEE80211_REASON_BAD_CIPHER_OR_AKM                        = 29,
    IEEE80211_REASON_NOT_AUTHORIZED_THIS_LOCATION             = 30,
    IEEE80211_REASON_SERVICE_CHANGE_PRECLUDES_TS              = 31,
    IEEE80211_REASON_UNSPECIFIED_QOS_REASON                   = 32,
    IEEE80211_REASON_NOT_ENOUGH_BANDWIDTH                     = 33,
    IEEE80211_REASON_DISASSOC_LOW_ACK                         = 34,
    IEEE80211_REASON_EXCEEDED_TXOP                            = 35,
    IEEE80211_REASON_STA_LEAVING                              = 36,
    IEEE80211_REASON_END_TS_BA_DLS                            = 37,
    IEEE80211_REASON_UNKNOWN_TS_BA                            = 38,
    IEEE80211_REASON_TIMEOUT                                  = 39,
    IEEE80211_REASON_PEERKEY_MISMATCH                         = 45,
    IEEE80211_REASON_AUTHORIZED_ACCESS_LIMIT_REACHED          = 46,
    IEEE80211_REASON_EXTERNAL_SERVICE_REQUIREMENTS            = 47,
    IEEE80211_REASON_INVALID_FT_ACTION_FRAME_COUNT            = 48,
    IEEE80211_REASON_INVALID_PMKID                            = 49,
    IEEE80211_REASON_INVALID_MDE                              = 50,
    IEEE80211_REASON_INVALID_FTE                              = 51,
    IEEE80211_REASON_MESH_PEERING_CANCELLED                   = 52,
    IEEE80211_REASON_MESH_MAX_PEERS                           = 53,
    IEEE80211_REASON_MESH_CONFIG_POLICY_VIOLATION             = 54,
    IEEE80211_REASON_MESH_CLOSE_RCVD                          = 55,
    IEEE80211_REASON_MESH_MAX_RETRIES                         = 56,
    IEEE80211_REASON_MESH_CONFIRM_TIMEOUT                     = 57,
    IEEE80211_REASON_MESH_INVALID_GTK                         = 58,
    IEEE80211_REASON_MESH_INCONSISTENT_PARAMS                 = 59,
    IEEE80211_REASON_MESH_INVALID_SECURITY_CAP                = 60,
    IEEE80211_REASON_MESH_PATH_ERROR_NO_PROXY_INFO            = 61,
    IEEE80211_REASON_MESH_PATH_ERROR_NO_FORWARDING_INFO       = 62,
    IEEE80211_REASON_MESH_PATH_ERROR_DEST_UNREACHABLE         = 63,
    IEEE80211_REASON_MAC_ADDRESS_ALREADY_EXISTS_IN_MBSS       = 64,
    IEEE80211_REASON_MESH_CHANNEL_SWITCH_REGULATORY_REQ       = 65,
    IEEE80211_REASON_MESH_CHANNEL_SWITCH_UNSPECIFIED          = 66,
    //[[[end]]]
};

/**
 * Status codes.
 *
 * Status codes are used in management frames to indicate the result
 * of an operation.
 *
 * Codes from IEEE Std 802.11-2016, Table 9-46.
 */
enum ieee80211_status_code {
    /*[[[cog
    from bits import enumpub
    for x in enumpub.IEEE80211StatusCode:
        cog.outl(f'IEEE80211_STATUS_{x.name:<40} = {x.value},')
    ]]]*/
    IEEE80211_STATUS_SUCCESS                                  = 0,
    IEEE80211_STATUS_UNSPECIFIED_FAILURE                      = 1,
    IEEE80211_STATUS_TDLS_WAKEUP_ALTERNATE                    = 2,
    IEEE80211_STATUS_TDLS_WAKEUP_REJECT                       = 3,
    IEEE80211_STATUS_SECURITY_DISABLED                        = 5,
    IEEE80211_STATUS_UNACCEPTABLE_LIFETIME                    = 6,
    IEEE80211_STATUS_NOT_IN_SAME_BSS                          = 7,
    IEEE80211_STATUS_CAPS_UNSUPPORTED                         = 10,
    IEEE80211_STATUS_REASSOC_NO_ASSOC                         = 11,
    IEEE80211_STATUS_ASSOC_DENIED_UNSPEC                      = 12,
    IEEE80211_STATUS_NOT_SUPPORTED_AUTH_ALG                   = 13,
    IEEE80211_STATUS_UNKNOWN_AUTH_TRANSACTION                 = 14,
    IEEE80211_STATUS_CHALLENGE_FAIL                           = 15,
    IEEE80211_STATUS_AUTH_TIMEOUT                             = 16,
    IEEE80211_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA              = 17,
    IEEE80211_STATUS_ASSOC_DENIED_RATES                       = 18,
    IEEE80211_STATUS_ASSOC_DENIED_NOSHORT                     = 19,
    IEEE80211_STATUS_SPEC_MGMT_REQUIRED                       = 22,
    IEEE80211_STATUS_PWR_CAPABILITY_NOT_VALID                 = 23,
    IEEE80211_STATUS_SUPPORTED_CHANNEL_NOT_VALID              = 24,
    IEEE80211_STATUS_ASSOC_DENIED_NO_SHORT_SLOT_TIME          = 25,
    IEEE80211_STATUS_ASSOC_DENIED_NO_HT                       = 27,
    IEEE80211_STATUS_R0KH_UNREACHABLE                         = 28,
    IEEE80211_STATUS_ASSOC_DENIED_NO_PCO                      = 29,
    IEEE80211_STATUS_ASSOC_REJECTED_TEMPORARILY               = 30,
    IEEE80211_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION       = 31,
    IEEE80211_STATUS_UNSPECIFIED_QOS_FAILURE                  = 32,
    IEEE80211_STATUS_DENIED_INSUFFICIENT_BANDWIDTH            = 33,
    IEEE80211_STATUS_DENIED_POOR_CHANNEL_CONDITIONS           = 34,
    IEEE80211_STATUS_DENIED_QOS_NOT_SUPPORTED                 = 35,
    IEEE80211_STATUS_REQUEST_DECLINED                         = 37,
    IEEE80211_STATUS_INVALID_PARAMETERS                       = 38,
    IEEE80211_STATUS_REJECTED_WITH_SUGGESTED_CHANGES          = 39,
    IEEE80211_STATUS_INVALID_IE                               = 40,
    IEEE80211_STATUS_GROUP_CIPHER_NOT_VALID                   = 41,
    IEEE80211_STATUS_PAIRWISE_CIPHER_NOT_VALID                = 42,
    IEEE80211_STATUS_AKMP_NOT_VALID                           = 43,
    IEEE80211_STATUS_UNSUPPORTED_RSN_IE_VERSION               = 44,
    IEEE80211_STATUS_INVALID_RSN_IE_CAPAB                     = 45,
    IEEE80211_STATUS_CIPHER_REJECTED_PER_POLICY               = 46,
    IEEE80211_STATUS_TS_NOT_CREATED                           = 47,
    IEEE80211_STATUS_DIRECT_LINK_NOT_ALLOWED                  = 48,
    IEEE80211_STATUS_DEST_STA_NOT_PRESENT                     = 49,
    IEEE80211_STATUS_DEST_STA_NOT_QOS_STA                     = 50,
    IEEE80211_STATUS_ASSOC_DENIED_LISTEN_INT_TOO_LARGE        = 51,
    IEEE80211_STATUS_INVALID_FT_ACTION_FRAME_COUNT            = 52,
    IEEE80211_STATUS_INVALID_PMKID                            = 53,
    IEEE80211_STATUS_INVALID_MDIE                             = 54,
    IEEE80211_STATUS_INVALID_FTIE                             = 55,
    IEEE80211_STATUS_REQUESTED_TCLAS_NOT_SUPPORTED            = 56,
    IEEE80211_STATUS_INSUFFICIENT_TCLAS_PROCESSING_RESOURCES  = 57,
    IEEE80211_STATUS_TRY_ANOTHER_BSS                          = 58,
    IEEE80211_STATUS_GAS_ADV_PROTO_NOT_SUPPORTED              = 59,
    IEEE80211_STATUS_NO_OUTSTANDING_GAS_REQ                   = 60,
    IEEE80211_STATUS_GAS_RESP_NOT_RECEIVED                    = 61,
    IEEE80211_STATUS_STA_TIMED_OUT_WAITING_FOR_GAS_RESP       = 62,
    IEEE80211_STATUS_GAS_RESP_LARGER_THAN_LIMIT               = 63,
    IEEE80211_STATUS_REQ_REFUSED_HOME                         = 64,
    IEEE80211_STATUS_ADV_SRV_UNREACHABLE                      = 65,
    IEEE80211_STATUS_REQ_REFUSED_SSPN                         = 67,
    IEEE80211_STATUS_REQ_REFUSED_UNAUTH_ACCESS                = 68,
    IEEE80211_STATUS_INVALID_RSNIE                            = 72,
    IEEE80211_STATUS_U_APSD_COEX_NOT_SUPPORTED                = 73,
    IEEE80211_STATUS_U_APSD_COEX_MODE_NOT_SUPPORTED           = 74,
    IEEE80211_STATUS_BAD_INTERVAL_WITH_U_APSD_COEX            = 75,
    IEEE80211_STATUS_ANTI_CLOGGING_TOKEN_REQ                  = 76,
    IEEE80211_STATUS_FINITE_CYCLIC_GROUP_NOT_SUPPORTED        = 77,
    IEEE80211_STATUS_CANNOT_FIND_ALT_TBTT                     = 78,
    IEEE80211_STATUS_TRANSMISSION_FAILURE                     = 79,
    IEEE80211_STATUS_REQ_TCLAS_NOT_SUPPORTED                  = 80,
    IEEE80211_STATUS_TCLAS_RESOURCES_EXCHAUSTED               = 81,
    IEEE80211_STATUS_REJECTED_WITH_SUGGESTED_BSS_TRANSITION   = 82,
    IEEE80211_STATUS_REJECT_WITH_SCHEDULE                     = 83,
    IEEE80211_STATUS_REJECT_NO_WAKEUP_SPECIFIED               = 84,
    IEEE80211_STATUS_SUCCESS_POWER_SAVE_MODE                  = 85,
    IEEE80211_STATUS_PENDING_ADMITTING_FST_SESSION            = 86,
    IEEE80211_STATUS_PERFORMING_FST_NOW                       = 87,
    IEEE80211_STATUS_PENDING_GAP_IN_BA_WINDOW                 = 88,
    IEEE80211_STATUS_REJECT_U_PID_SETTING                     = 89,
    IEEE80211_STATUS_REFUSED_EXTERNAL_REASON                  = 92,
    IEEE80211_STATUS_REFUSED_AP_OUT_OF_MEMORY                 = 93,
    IEEE80211_STATUS_REJECTED_EMERGENCY_SERVICE_NOT_SUPPORTED = 94,
    IEEE80211_STATUS_QUERY_RESP_OUTSTANDING                   = 95,
    IEEE80211_STATUS_REJECT_DSE_BAND                          = 96,
    IEEE80211_STATUS_TCLAS_PROCESSING_TERMINATED              = 97,
    IEEE80211_STATUS_TS_SCHEDULE_CONFLICT                     = 98,
    IEEE80211_STATUS_DENIED_WITH_SUGGESTED_BAND_AND_CHANNEL   = 99,
    IEEE80211_STATUS_MCCAOP_RESERVATION_CONFLICT              = 100,
    IEEE80211_STATUS_MAF_LIMIT_EXCEEDED                       = 101,
    IEEE80211_STATUS_MCCA_TRACK_LIMIT_EXCEEDED                = 102,
    IEEE80211_STATUS_DENIED_DUE_TO_SPECTRUM_MANAGEMENT        = 103,
    IEEE80211_STATUS_ASSOC_DENIED_NO_VHT                      = 104,
    IEEE80211_STATUS_ENABLEMENT_DENIED                        = 105,
    IEEE80211_STATUS_RESTRICTION_FROM_AUTHORIZED_GDB          = 106,
    IEEE80211_STATUS_AUTHORIZATION_DEENABLED                  = 107,
    IEEE80211_STATUS_FILS_AUTHENTICATION_FAILURE              = 112,
    IEEE80211_STATUS_UNKNOWN_AUTHENTICATION_SERVER            = 113,
    IEEE80211_STATUS_UNKNOWN_PASSWORD_IDENTIFIER              = 123,
    //[[[end]]]
};

#define	IEEE80211_WEP_KEYLEN		5       /* 40bit */
#define	IEEE80211_WEP_IVLEN		3       /* 24bit */
#define	IEEE80211_WEP_KIDLEN		1       /* 1 octet */
#define	IEEE80211_WEP_CRCLEN		4       /* CRC-32 */
#define	IEEE80211_WEP_TOTLEN		(IEEE80211_WEP_IVLEN + \
					 IEEE80211_WEP_KIDLEN + \
					 IEEE80211_WEP_CRCLEN)
#define	IEEE80211_WEP_NKID		6       /* number of key ids */

/*
 * 802.11i defines an extended IV for use with non-WEP ciphers.
 * When the EXTIV bit is set in the key id byte an additional
 * 4 bytes immediately follow the IV for TKIP.  For CCMP the
 * EXTIV bit is likewise set but the 8 bytes represent the
 * CCMP header rather than IV+extended-IV.
 */
#define	IEEE80211_WEP_EXTIV		0x20
#define	IEEE80211_WEP_EXTIVLEN		4       /* extended IV length */
#define	IEEE80211_WEP_MICLEN		8       /* trailing MIC */

#define IEEE80211_CRC_LEN               4
#define IEEE80211_CRC_INIT              ~0u
#define IEEE80211_CRC_POLYNOMIAL        0xedb88320u     /* crc-32 polynomial reversed */
#define IEEE80211_CRC_GOOD              0xdebb20e3u     /* expected crc-32 result */

#define	IEEE80211_ACK_LEN \
	(sizeof(struct ieee80211_frame_ack) + IEEE80211_CRC_LEN)
#define	IEEE80211_CTS_LEN \
	(sizeof(struct ieee80211_frame_cts) + IEEE80211_CRC_LEN)
#define	IEEE80211_RTS_LEN \
	(sizeof(struct ieee80211_frame_rts) + IEEE80211_CRC_LEN)
#define	IEEE80211_MIN_LEN \
	(sizeof(struct ieee80211_frame_min) + IEEE80211_CRC_LEN)

/* when the duration/ID field is used as ID, the two MSB are set to
 * one */
#define IEEE80211_AID_MSB       (3 << 14)

/* removes the two MSB from the value */
#define	IEEE80211_AID(b)	((b) &~ IEEE80211_AID_MSB)

typedef union __attribute__ ((__transparent_union__)) ieee80211_element {
    const uint8_t *ptr;
    const struct ieee80211_ie_hdr *hdr;
    const struct ieee80211_bss_max_idle_period *max_idle;
    const struct ieee80211_country_ie *country;
    const struct ieee80211_csa_ie *csa;
    const struct ieee80211_edca_param *edca_param;
    const struct ieee80211_erp_ie *erp;
    const struct ieee80211_ie_htcap *htcap;
    const struct ieee80211_ie_htoper *htoper;
    const struct ieee80211_ie_wpa *wpa;
    const struct ieee80211_mme_ie *mme;
    const struct ieee80211_qos_capability *qos_cap;
    const struct ieee80211_quiet_ie *quiet;
    const struct ieee80211_ssid_ie *ssid;
    const struct ieee80211_tim_ie *tim;
    const struct ieee80211_wme_info *wmm_info;
    const struct ieee80211_wme_param *wmm_param;
    const struct ieee80211_wme_tspec *wmm_tspec;
} ieee80211_element_t;

struct ieee80211_amsdu_subframe {
    uint8_t  da[IEEE80211_ADDR_LEN];
    uint8_t  sa[IEEE80211_ADDR_LEN];
    uint16_t length;
};

/** @} */
