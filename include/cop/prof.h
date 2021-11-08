#pragma once

#define PROF_COP_FRONTEND_STATE         1 /* @PROF 1 "Frontend event" */
#define PROF_COP_RSSI                   2 /* @PROF 2 "RSSI" */
#define PROF_COP_AGC_RESTART            3 /* @PROF 1 "AGC restart" */
#define PROF_COP_RX_START               4 /* @PROF 1 "RX Start" */
#define PROF_COP_RX_INVALID             5 /* @PROF 1 "RX Invalid" */
#define PROF_COP_RX_ABORT               6 /* @PROF 1 "RF Abort" */
#define PROF_COP_RX_PACKET              9 /* @PROF 1 "RX packet address" */
#define PROF_COP_RX_FRAG               10 /* @PROF 1 "RX fragment address" */
#define PROF_COP_RX_CHUNK              11 /* @PROF 1 "RX fragment size" */
#define PROF_COP_RX_FLAGS              12 /* @PROF 1 "RX flags" */
#define PROF_COP_RX_SEQ                13 /* @PROF 1 "Sequence number" */
#define PROF_COP_RX_DURATION           14 /* @PROF 1 "Duration" */
#define PROF_COP_RX_TIMESTAMP          15 /* @PROF 1 "Timestamp" */

#define PROF_RX_AMPDU_TOTAL             16
#define PROF_RX_AMPDU_TAG               17
#define PROF_RX_AMPDU_FRAME             18
#define PROF_RX_AMPDU_PADDING           19
#define PROF_RX_FLUSH_DMA               20
#define PROF_COP_RX_SNR                 21

#define PROF_UNSTABLE_640               24

#define PROF_COP_TX_START               30
#define PROF_COP_TX_CHUNK               31
#define PROF_COP_TX_PREPARE             32
#define PROF_COP_TX_QUEUE               33
#define PROF_COP_TX_ACK_TIMEOUT         34
#define PROF_COP_TX_STATUS              35
#define PROF_COP_TX_FRAME               36
#define PROF_COP_TX_CTS_TIMEOUT         37
#define PROF_COP_CCA_BUSY               38
#define PROF_COP_TX_AMPDU_TOTAL         39

#define PROF_RF_FREQ                    40
#define PROF_RF_USER                    41
#define PROF_RF_MODE                    42
#define PROF_RF_AGC                     43
#define PROF_RF_ABORT                   44
#define PROF_RF_REGISTER                45
#define PROF_RF_DEREGISTER              46
#define PROF_RF_LOCKTIME                47
#define PROF_RF_PLLERR                  48

#define PROF_COP_BACKOFF_START          50
#define PROF_COP_BACKOFF_OCCURRED       51
#define PROF_COP_BACKOFF_RX_IDLE        52
#define PROF_COP_BA_REQUEST             53
