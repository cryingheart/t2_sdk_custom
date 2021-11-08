/**
 * @addtogroup rfdrv
 * @{
 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/rfparam.h>

/* convert deciBel to vigeciBel (.5 dB) */
#define DBM_TO_VBM(_dbm) ((int)(2 * (_dbm)))

/* convert vigeciBel to deciBel */
#define VBM_TO_DBM(_vbm) (((_vbm) - ((_vbm) < 0)) / 2)

/* opaque structure defined by each backend */
struct rf_txpower;

/**
 * User specific RF settings
 */
struct rf_user {
    /** rfdrv internal */
    uint8_t   id; /* cross-reference */
    /** prio for the rfdrv_request() */
    uint8_t   prio;
    /** rfdrv internal */
    uint8_t   abort_event;
    /** rfdrv internal */
    rf_mode_t mode;
    /** channel in MHz */
    uint32_t  channel;
    /** rfdrv internal */
    void    (*agc_setup)(uint32_t ch_freq);
    /** rfdrv internal */
    rf_type_t type;
    /** rfdrv internal */
    uint8_t   resume_event;
    /** rfdrv internal */
    const struct rf_txpower *txpower;
    /** rfdrv internal */
    struct recal_temp recal;
};

static inline void rfdrv_set_prio(struct rf_user *user, unsigned int prio)
{
    user->prio = prio;
}

static inline void rfdrv_set_channel(struct rf_user *user, unsigned int channel)
{
    user->channel = channel;
}

enum rf_phy_mode {
    RF_PHY_MODE_BLE,
    RF_PHY_MODE_WIFI_QPSK,
    RF_PHY_MODE_WIFI_OFDM
};

enum rf_phy_prio {
    RF_PHY_PRIO_BLE_LOW     = 0,
    RF_PHY_PRIO_WIFI_IDLE   = 1, /* Listening, but no packet yet detected */
    RF_PHY_PRIO_BLE_MEDIUM  = 2, /* Any BLE RX or TX */
    RF_PHY_PRIO_BLE_HIGH    = 3,
    RF_PHY_PRIO_WIFI_BUSY   = 4, /* RX or TX of packet ongoing, do not abort */
};

/** @internal */
struct rf_ops {
    /**
     * Power-on initialization of the RF driver.
     */
    int      (*init)(int temperature);
    /**
     * Called before device is going into suspend.
     */
    void     (*suspend)(void);
    /**
     * Called when leaving suspended state (device has been reset).
     */
    void     (*resume)(void);
    /**
     * Perform RF calibration procedure (if needed).
     */
    int     (*calibrate)(uint8_t*, uint8_t*);
    /**
     * Set the RF hardware in the specified mode.
     */
    void     (*set_mode)(struct rf_user *user);
    /**
     * Prepare TX parts that has a long startup time.
     */
    void     (*tx_prepare)(struct rf_user *user);
    /**
     * Enable/disable phase only mode (power saved when enabled).
     */
    void     (*rx_phase_only)(uint8_t enable);
    /**
     * Backend callback for rfdrv_set_tx_power
     */
    size_t rf_txpower_size;
    void (*rf_txpower_init)(struct rf_txpower *txp, enum rf_phy_mode mode);
    void (*rf_txpower_prepare)(struct rf_txpower *txp, int tx_power_dbm, int antenna_gain_vbi);
    void (*rf_tx_setup)(const struct rf_txpower *txp);
    /**
     * rfdrv frontend tells temperature to backend
     */
    void (*update_temp)(int32_t temp);
};

int rfdrv_init(void);
void rfdrv_update_ct(const uint8_t ct);
void rfdrv_force_recal(void);
int rfdrv_cal(uint32_t idx, rf_type_t type);
void rfdrv_register(struct rf_user *user, rf_type_t type); /* Do the heavy stuff after power up */
void rfdrv_deregister(const struct rf_user *user); /* Rf can be set in deep sleep. */
int  rfdrv_request(struct rf_user *user);
void rfdrv_release(struct rf_user *user);
int  rfdrv_set_mode(struct rf_user *user, rf_mode_t mode);
void rfdrv_set_mode_tx(struct rf_user *user);
int8_t rfdrv_get_rssi(const struct rf_user *user);
void rfdrv_agc_lock(const struct rf_user *user, uint8_t phase_only);
void rfdrv_agc_free(const struct rf_user *user);
void rfdrv_agc_sat_max_lock(const struct rf_user *user);
int rfdrv_tx_prepare(struct rf_user *user);
void rfdrv_agc_permute(const struct rf_user *user);

void rfdrv_agc_center_target(void);
void rfdrv_agc_reset_target(void);

void rfdrv_set_tx_power(struct rf_user *user, const struct rf_txpower *txp);

struct rf_txpower* rfdrv_txpower_new(enum rf_phy_mode mode, int tx_power_dbm);
void rfdrv_txpower_delete(struct rf_txpower *txp);
void rfdrv_txpower_prepare(struct rf_txpower *txp, int tx_power_dbm);
int rfdrv_antenna_gain_set(const char *dbi); /* E.g. "2.15" */

/** @} */
