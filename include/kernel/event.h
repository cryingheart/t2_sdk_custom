/**
 * @addtogroup event
 * @{
 */
#pragma once
#include <stdbool.h>
#include <kernel/interrupt.h>

#define EVENT_BO_DONE        0
/* AGC */
#define EVENT_AGC_SAT        2
#define EVENT_AGC_RSSI       3
/* OFDM frontend */
#define EVENT_OFDM_SIG_DET_1 4
#define EVENT_OFDM_SIG_DET_2 5
#define EVENT_OFDM_LTF_DET_1 6
#define EVENT_OFDM_LTF_DET_2 7
#define EVENT_OFDM_LTF_DET_3 8
/* OFDM backend */
#define EVENT_OFDM_LSIG_EN   9
#define EVENT_OFDM_HTSIG_EN 10
/* TX-BB */
#define EVENT_TX_DONE       11
/* QPSK frontend */
#define EVENT_QPSK_SYNC     12
/* QPSK backend */
#define EVENT_QPSK_HDR_EN   13
#define EVENT_QPSK_PLCP     14
#define EVENT_QPSK_FAIL     15

#define EVENT_WIFI_QPSK     (BIT(EVENT_QPSK_SYNC)      | \
                             BIT(EVENT_QPSK_HDR_EN)    | \
                             BIT(EVENT_QPSK_PLCP)      | \
                             BIT(EVENT_QPSK_FAIL))

#define EVENT_WIFI_OFDM     (BIT(EVENT_OFDM_SIG_DET_1) | \
                             BIT(EVENT_OFDM_SIG_DET_2) | \
                             BIT(EVENT_OFDM_LTF_DET_1) | \
                             BIT(EVENT_OFDM_LTF_DET_2) | \
                             BIT(EVENT_OFDM_LTF_DET_3) | \
                             BIT(EVENT_OFDM_LSIG_EN)   | \
                             BIT(EVENT_OFDM_HTSIG_EN))

#define EVENT_WIFI_ALL      (EVENT_WIFI_QPSK | EVENT_WIFI_OFDM)

#define EVENT_SW_0          16
#define EVENT_SW_1          17
#define EVENT_SW_2          18
#define EVENT_SW_3          19
#define EVENT_SW_4          20
#define EVENT_SW_5          21
#define EVENT_SW_6          22
#define EVENT_SW_7          23
#define EVENT_SW_8          24
#define EVENT_SW_9          25
#define EVENT_SW_10         26
#define EVENT_SW_11         27
#define EVENT_SW_12         28
#define EVENT_SW_13         29
#define EVENT_SW_14         30
#define EVENT_SW_15         31
#define EVENT_WIFI_RESUME   EVENT_SW_13 /* WiFi resumed by RF COEX */
#define EVENT_RF_CAL        EVENT_SW_14
#define EVENT_WIFI_ABORT    EVENT_SW_15 /* WiFi aborted by RF COEX */

/* BT events from BB to COP */
#define EVENT_BT_LE_TX_EOD  32  /* Streamer completed (safe to change whitening) */
#define EVENT_BT_LE_TX_DONE 33  /* Entire TX is completed. */
#define EVENT_BT_LE_RX_CORR 34  /* Also 15.4 full preamble found */

#define EVENT_SDIO_RX_DONE   35
#define EVENT_SDIO_TX_DONE   36
#define EVENT_SDIO_POSITION  37
#define EVENT_SDIO_DATA_DONE 38

#define EVENT_15_4_RX_CS     39 /* Passed first preamble corr, check status register */
#define EVENT_15_4_RX_SFD    40 /* SFD received, check status register */

/* SDIO software events */
#define EVENT_SW_SDIO_REQUEST  42
#define EVENT_SW_SDIO_RESPONSE 43

/* BT events between COP and ARM */
#define EVENT_BT_COP_STATUS 48
#define EVENT_BT_COP_READY  49

#define EVENT_BLUE_ABORT    50 /* BT aborted by RF COEX */
#define EVENT_BLUE_RESUME   51 /* BT resumed by RF COEX */

#define EVENT_UART_FRAMING  54 /* UART framing error */
#define EVENT_GPIO_0        55
#define EVENT_GPIO_1        56
#define EVENT_GPIO_2        57
#define EVENT_GPIO_3        58

#define EVENT_WIFI_RX_OVERRUN  59
#define EVENT_WIFI_TX_UNDERRUN 60

#define EVENT_COP0_ERROR    61
#define EVENT_COP1_ERROR    62
#define EVENT_COP2_ERROR    63

typedef int (*os_event_handler)(uint32_t vector, void *arg);

void os_attach_event(uint32_t id, os_event_handler handler, void *arg);
void os_enable_event(uint32_t id);
void os_disable_event(uint32_t id);
void os_clear_event(uint32_t id);
void os_set_event(uint32_t id);
bool os_test_event(uint32_t id);

/** @} */
