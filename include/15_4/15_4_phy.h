#pragma once
#include <stdint.h>
#include <kernel/packet.h>
#include <kernel/rfdrv.h>

#define RF_15_4_TX_POWER           10 // FIXME
#define RF_PLL_LOCK_TIME           70
#define RF_DELAY_TX_RAMP           4
#define RF_DELAY_TX_DATA           3
#define RF_DELAY_RX_DATA           2

#define F15_4_PREAMBLE             0x00000000
#define F15_4_ACCESS_ADDRESS       0x3675ee07
#define F15_4_SFD                  0xa7

/* 5.2.1.9 FCS field */
#define F15_4_FCS_INIT             0x0000
#define F15_4_FCS_POLYNOM          0x8408
#define F15_4_FCS_SIZE             2

#define F15_4_PHY_PHR_LENGTH_MASK  0x7f
#define F15_4_MAX_PHY_PACKET_SIZE  127
#define F15_4_TURN_AROUND_TIME     12 /* symbols, RX->TX, TX->RX */
#define F15_4_LIFS_PERIOD          40 /* symbols */
#define F15_4_SIFS_PERIOD          12 /* symbols */

typedef enum {
    F15_4_PHY_STATE_IDLE = 0x00,
    F15_4_PHY_STATE_TEST_TX,
    F15_4_PHY_STATE_TEST_RX,
} f15_4_phy_state_t;

typedef enum {
    F15_4_PHY_STATUS_CORR_FAIL    = 0x00,
    F15_4_PHY_STATUS_FCS_FAIL     = 0x01,
    F15_4_PHY_STATUS_FCS_OK       = 0x02,
    F15_4_PHY_STATUS_RX_ABORT     = 0x03,

    F15_4_PHY_STATUS_TX_ABORT     = 0x09,
    F15_4_PHY_STATUS_TX_DONE      = 0x0A,
    F15_4_PHY_STATUS_ERROR,
} f15_4_phy_status_t;

/* Interface between MAC (ARM) and PHY (COP). */
typedef struct {
    struct rf_user rf;

    /* GENERAL */
    f15_4_phy_state_t state;
    f15_4_phy_status_t status;

    uint32_t access_address;
    uint32_t channel;

    /* RX */
    struct fragq rxq;
    uint32_t rx_time;
    uint8_t rssi;

    /* TX */
    struct fragq txq;

    /* COP internal */
} f15_4_phy_t;

extern uint32_t f15_4_cop_stack[32];
extern void cop_15_4_init(void);
extern void cop_15_4_start(f15_4_phy_t *f15_4_phy);
extern f15_4_phy_t *f15_4_phy;
extern struct rf_user f15_4_rf_data;
