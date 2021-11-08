/**
 * @addtogroup ble_gap
 * @{
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * SMP error code, see  Bluetooth specification Vol 3, Part H, 3.5.5
 */
typedef enum {
    BT_SMP_ERROR_SUCCESS                        = 0x00,
    BT_SMP_ERROR_PASSKEY_ENTRY_FAILED           = 0x01,
    BT_SMP_ERROR_OOB_NOT_AVAILABLE              = 0x02,
    BT_SMP_ERROR_AUTHENTICATION_REQUIREMENTS    = 0x03,
    BT_SMP_ERROR_CONFIRM_VALUE_FAILED           = 0x04,
    BT_SMP_ERROR_PAIRING_NOT_SUPPORTED          = 0x05,
    BT_SMP_ERROR_ENCRYPTION_KEY_SIZE            = 0x06,
    BT_SMP_ERROR_COMMAND_NOT_SUPPORTED          = 0x07,
    BT_SMP_ERROR_UNSPECIFIED_REASON             = 0x08,
    BT_SMP_ERROR_REPEATED_ATTEMPTS              = 0x09,
    BT_SMP_ERROR_INVALID_PARAMETERS             = 0x0A,
    BT_SMP_ERROR_DHKEY_CHECK_FAILED             = 0x0B,
    BT_SMP_ERROR_NUMERIC_COMPARISON_FAILED      = 0x0C,
    BT_SMP_ERROR_PAIRING_IN_PROGRESS            = 0x0D,
    BT_SMP_ERROR_CROSS_TRANSPORT_KEY_DERIVATION = 0x0E,
    BT_SMP_ERROR_KEY_REJECTED                   = 0x0F,
    BT_SMP_ERROR_INTERNAL_TIMEOUT               = 0x10,
    BT_SMP_ERROR_REMOTE_LOST_BOND               = 0x11,
} bt_smp_error_t;

/**
 * SMP IO capabilities for pairing, see  Bluetooth specification Vol 3, Part H, 3.5.1
 */
typedef enum {
    bt_smp_io_display_only       = 0x00,
    bt_smp_io_display_yes_no     = 0x01,
    bt_smp_io_keyboard_only      = 0x02,
    bt_smp_io_no_input_no_output = 0x03,
    bt_smp_io_keyboard_display   = 0x04,
} bt_smp_io_capability_t;

#define BT_SMP_AUTH_BOND_BIT     0 // 1 - bonding, bit1 reserved
#define BT_SMP_AUTH_MITM_BIT     2 // 1 - authenticated MITM protection
#define BT_SMP_AUTH_SC_BIT       3 // 1 - secure connection
#define BT_SMP_AUTH_KEYPRESS_BIT 4
#define BT_SMP_AUTH_ENCR_BIT     6 // 1 - encrypted (not in auth_req)
#define BT_SMP_AUTH_KEY128_BIT   7 // 1 - 128-bits key, else less (not in auth_req)

/**
 * SMP callback functions
 */
typedef struct {
    /** handle Handle for which passkey is requested */
    void (*passkey_input_cb)(uint8_t handle);
    /** passkey Passkey to be entered at remote device */
    void (*passkey_output_cb)(uint32_t passkey);
    /** handle Handle for the connection, result Outcome of the pairing */
    void (*authenticate_cb)(uint8_t handle, bt_smp_error_t result);
} smp_ops_t;


/**
 * @struct bt_smp_cfg_t
 * Configure security (smp). Used during read of protected parameters.
 */
typedef struct bt_smp_cfg_t {
    /** ops SMP callback functions (default: NULL) */
    const smp_ops_t *ops;
    /** smp_io_cap IO capabilities (default: display_only=0) */
    bt_smp_io_capability_t io_cap;
    /** smp_oob OOB data present (default: 0) */
    uint8_t oob;
    /** bondable Bondable (1=True, 0=False) (default: 0) */
    uint8_t bondable;
    /** mitm MITM protection requested (1=True, 0=False) (default: 0) */
    uint8_t mitm;
    /** sc Secure Connection supported (1=True, 0=False) (default: 0) */
    uint8_t sc;
    /** keypress Generate keypress notifications (1=True, 0=False) (default: 0) */
    uint8_t keypress;
    /** key_size_min Minimal key size (bytes) that is accepted (7..16) (default: 16) */
    uint8_t key_size_min;
    /** encrypt Automatically encrypt link at connection setup if key exists: 1=True, 0=False (default: 0) */
    uint8_t encrypt;
} bt_smp_cfg_t;

void bt_smp_authenticate(const uint8_t handle, const uint8_t oob, const uint8_t bondable, const uint8_t mitm, const uint8_t sc, const uint8_t key128);
void bt_smp_passkey_set(uint8_t handle, uint8_t passkey[16]);

void bt_smp_cfg_get(bt_smp_cfg_t *smp_cfg);
void bt_smp_cfg_init_default(bt_smp_cfg_t *smp_cfg);
bt_smp_error_t bt_smp_cfg_set(const bt_smp_cfg_t *smp_cfg);

bool bt_smp_bond_list(uint8_t idx, char *dest, size_t dest_size);
bool bt_smp_bond_remove_one(uint8_t idx);
void bt_smp_bond_remove_all(void);

/** @} */
