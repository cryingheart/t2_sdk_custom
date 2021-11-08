#pragma once
#include <stdint.h>
#include <kernel/message.h>
#include <wifi/wifi.h>

/*
 * Message formats for messages from the wifi layer to user applications.
 */

struct wifi_node_event {
    /** Message header */
    struct os_msg iev_hdr;
    /** MAC address for the node this event is related to */
    uint8_t       iev_addr[6];
    /** Domain specific code related to the msg_type.
      * WIFI_NOTIFY_MSG_LEAVE maps to enum ieee80211_reason_code
      */
    int32_t       iev_code;
};

struct wifi_cipher_event {
    /** Message header */
    struct os_msg iev_hdr;
    /** Source MAC address */
    uint8_t       iev_src[6];
    /** Destination MAC address */
    uint8_t       iev_dst[6];
    /** Cipher type */
    uint8_t       iev_cipher;
    /** Key id/index */
    uint8_t       iev_keyix;
    /** Receive sequence number from key */
    uint64_t      iev_keyrsc;
    /** Receive sequence number from frame */
    uint64_t      iev_rsc;
};

struct wifi_eapol_event {
    /** Message header */
    struct os_msg iev_hdr;
    /** pkt len */
    uint32_t      pkt_len;
    /** pkt bytes */
    uint8_t       pkt[0];
};

#define WIFI_NOTIFY_MSG_LEAVE      100     /* AP disconnected us */
#define WIFI_NOTIFY_MSG_LOST       101     /* Lost tracking of AP */
#define WIFI_NOTIFY_MSG_RESTARTED  102     /* Associated AP restarted */
#define WIFI_NOTIFY_MSG_REPLAY     103     /* sequence counter replay detected */
#define WIFI_NOTIFY_MSG_MICHAEL    104     /* Michael MIC failure detected */
#define WIFI_NOTIFY_MSG_EAPOL      105     /* RX EAPOL */

void wifi_notify_node_leave(struct wifi_node *, int32_t reason);
void wifi_notify_node_lost(struct wifi_node *);
void wifi_notify_node_restarted(struct wifi_node *);
void wifi_notify_replay_failure(struct wifi_net *net,
                                const struct ieee80211_frame *frame,
                                const struct ieee80211_key *key,
                                uint64_t rsc,
                                int tid);
void wifi_notify_michael_failure(struct wifi_net *net,
                                 const struct ieee80211_frame *frame,
                                 const struct ieee80211_key *key);
void wifi_notify_eapol(struct wifi_net *net, void *data, size_t len);
/* copy */
struct wifi_eapol_event* copy_wifi_eapol_event(struct wifi_eapol_event *ev);
struct wifi_node_event*  copy_wifi_node_event(struct wifi_node_event *ev);
