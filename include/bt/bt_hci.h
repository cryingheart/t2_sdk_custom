/**
 * @addtogroup ble_hci
 * @{
 */
#pragma once
#include <stdint.h>
#include <kernel/packet.h>

#define BT_HCI_OGF_FLAGS(parameter) (parameter & 0xFC00)
#define BT_HCI_OGF_LINK_CONTROL     (0x01 << 10)
#define BT_HCI_OGF_LINK_POLICY      (0x02 << 10)
#define BT_HCI_OGF_CONTROL_BASEBAND (0x03 << 10)
#define BT_HCI_OGF_INFORMATIONAL    (0x04 << 10)
#define BT_HCI_OGF_STATUS           (0x05 << 10)
#define BT_HCI_OGF_TESTING          (0x06 << 10)
#define BT_HCI_OGF_LE               (0x08 << 10)
#define BT_HCI_OGF_VENDOR_SPECIFIC  (0x3F << 10)

#define BT_HCI_OCF_FLAGS(parameter) (parameter & 0x03FF)

#define BT_HCI_NOP                                                   0x0000
/* BT_HCI_LINK_CONTROL */
#define BT_HCI_INQUIRY                                               0x0001
#define BT_HCI_INQUIRY_CANCEL                                        0x0002
#define BT_HCI_CREATE_CONNECTION                                     0x0005
#define BT_HCI_DISCONNECT                                            0x0006
#define BT_HCI_CREATE_CONNECTION_CANCEL                              0x0008
#define BT_HCI_ACCEPT_CONNECTION_REQUEST                             0x0009
#define BT_HCI_REJECT_CONNECTION_REQUEST                             0x000A
#define BT_HCI_LINK_KEY_REQUEST_REPLY                                0x000B
#define BT_HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY                       0x000C
#define BT_HCI_PIN_CODE_REQUEST_REPLY                                0x000D
#define BT_HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY                       0x000E
#define BT_HCI_CHANGE_CONNECTION_PACKET_TYPE                         0x000F
#define BT_HCI_AUTHENTICATION_REQUESTED                              0x0011
#define BT_HCI_SET_CONNECTION_ENCRYPTION                             0x0013
#define BT_HCI_CHANGE_CONNECTION_LINK_KEY                            0x0015
#define BT_HCI_REMOTE_NAME_REQUEST                                   0x0019
#define BT_HCI_REMOTE_NAME_REQUEST_CANCEL                            0x001A
#define BT_HCI_READ_REMOTE_SUPPORTED_FEATURES                        0x001B
#define BT_HCI_READ_REMOTE_EXTENDED_FEATURES                         0x001C
#define BT_HCI_READ_REMOTE_VERSION_INFORMATION                       0x001D
#define BT_HCI_READ_CLOCK_OFFSET                                     0x001F
/* BT_HCI_OGF_LINK_POLICY */
#define BT_HCI_SWITCH_ROLE                                           0x000B
#define BT_HCI_READ_LINK_POLICY_SETTINGS                             0x000C
#define BT_HCI_WRITE_LINK_POLICY_SETTINGS                            0x000D
#define BT_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS                     0x000E
#define BT_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS                    0x000F
/* BT_HCI_CONTROL_BASEBAND */
#define BT_HCI_SET_EVENT_MASK                                        0x0001
#define BT_HCI_RESET                                                 0x0003
#define BT_HCI_SET_EVENT_FILTER                                      0x0005
#define BT_HCI_WRITE_LOCAL_NAME                                      0x0013
#define BT_HCI_READ_LOCAL_NAME                                       0x0014
#define BT_HCI_READ_CONNECTION_ACCEPT_TIMEOUT                        0x0015
#define BT_HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT                       0x0016
#define BT_HCI_READ_PAGE_TIMEOUT                                     0x0017
#define BT_HCI_WRITE_PAGE_TIMEOUT                                    0x0018
#define BT_HCI_READ_SCAN_ENABLE                                      0x0019
#define BT_HCI_WRITE_SCAN_ENABLE                                     0x001A
#define BT_HCI_READ_PAGE_SCAN_ACTIVITY                               0x001B
#define BT_HCI_WRITE_PAGE_SCAN_ACTIVITY                              0x001C
#define BT_HCI_READ_INQUIRY_SCAN_ACTIVITY                            0x001D
#define BT_HCI_WRITE_INQUIRY_SCAN_ACTIVITY                           0x001E
#define BT_HCI_READ_CLASS_OF_DEVICE                                  0x0023
#define BT_HCI_WRITE_CLASS_OF_DEVICE                                 0x0024
#define BT_HCI_READ_VOICE_SETTING                                    0x0025
#define BT_HCI_READ_TRANSMIT_POWER_LEVEL                             0x002D
#define BT_HCI_READ_NUMBER_OF_SUPPORTED_IAC                          0x0038
#define BT_HCI_READ_CURRENT_IAC_LAP                                  0x0039
#define BT_HCI_WRITE_CURRENT_IAC_LAP                                 0x003A
#define BT_HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION                   0x003F
#define BT_HCI_READ_INQUIRY_MODE                                     0x0044
#define BT_HCI_WRITE_INQUIRY_MODE                                    0x0045
#define BT_HCI_READ_EXTENDED_INQUIRY_RESPONSE                        0x0051
#define BT_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE                       0x0052
#define BT_HCI_SET_EVENT_MASK_PAGE_2                                 0x0063
#define BT_HCI_READ_AUTHENTICATED_PAYLOAD_TIMEOUT                    0x007B
#define BT_HCI_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT                   0x007C
/* BT_HCI_INFORMATIONAL */
#define BT_HCI_READ_LOCAL_VERSION_INFORMATION                        0x0001
#define BT_HCI_READ_LOCAL_SUPPORTED_COMMANDS                         0x0002
#define BT_HCI_READ_LOCAL_SUPPORTED_FEATURES                         0x0003
#define BT_HCI_READ_LOCAL_EXTENDED_FEATURES                          0x0004
#define BT_HCI_READ_BUFFER_SIZE                                      0x0005
#define BT_HCI_READ_BD_ADDR                                          0x0009
/* BT_HCI_STATUS */
#define BT_HCI_READ_RSSI                                             0x0005
#define BT_HCI_READ_AFH_CHANNEL_MAP                                  0x0006
/* BT_HCI_TESTING */
#define BT_HCI_ENABLE_DEVICE_UNDER_TEST_MODE                         0x0003
/* BT_HCI_LE */
#define BT_HCI_LE_SET_EVENT_MASK                                     0x0001
#define BT_HCI_LE_READ_BUFFER_SIZE                                   0x0002
#define BT_HCI_LE_READ_LOCAL_SUPPORTED_FEATURES                      0x0003
#define BT_HCI_LE_SET_RANDOM_ADDRESS                                 0x0005
#define BT_HCI_LE_SET_ADVERTISING_PARAMETERS                         0x0006
#define BT_HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER                  0x0007
#define BT_HCI_LE_SET_ADVERTISING_DATA                               0x0008
#define BT_HCI_LE_SET_SCAN_RESPONSE_DATA                             0x0009
#define BT_HCI_LE_SET_ADVERTISING_ENABLE                             0x000A
#define BT_HCI_LE_SET_SCAN_PARAMETERS                                0x000B
#define BT_HCI_LE_SET_SCAN_ENABLE                                    0x000C
#define BT_HCI_LE_CREATE_CONNECTION                                  0x000D
#define BT_HCI_LE_CREATE_CONNECTION_CANCEL                           0x000E
#define BT_HCI_LE_READ_WHITE_LIST_SIZE                               0x000F
#define BT_HCI_LE_CLEAR_WHITE_LIST                                   0x0010
#define BT_HCI_LE_ADD_DEVICE_TO_WHITE_LIST                           0x0011
#define BT_HCI_LE_REMOVE_DEVICE_FROM_WHITE_LIST                      0x0012
#define BT_HCI_LE_CONNECTION_UPDATE                                  0x0013
#define BT_HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION                    0x0014
#define BT_HCI_LE_READ_CHANNEL_MAP                                   0x0015
#define BT_HCI_LE_READ_REMOTE_USED_FEATURES                          0x0016
#define BT_HCI_LE_ENCRYPT                                            0x0017
#define BT_HCI_LE_RAND                                               0x0018
#define BT_HCI_LE_START_ENCRYPTION                                   0x0019
#define BT_HCI_LE_LONG_TERM_KEY_REQUEST_REPLY                        0x001A
#define BT_HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY               0x001B
#define BT_HCI_LE_READ_SUPPORTED_STATES                              0x001C
#define BT_HCI_LE_RECEIVER_TEST                                      0x001D
#define BT_HCI_LE_TRANSMITTER_TEST                                   0x001E
#define BT_HCI_LE_END_TEST                                           0x001F
#define BT_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY          0x0020
#define BT_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY 0x0021
#define BT_HCI_LE_SET_DATA_LENGTH                                    0x0022
#define BT_HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH                 0x0023
#define BT_HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH                0x0024
#define BT_HCI_LE_READ_LOCAL_P256_PUBLIC_KEY                         0x0025
#define BT_HCI_LE_GENERATE_DHKEY                                     0x0026
#define BT_HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST                       0x0027
#define BT_HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST                  0x0028
#define BT_HCI_LE_CLEAR_RESOLVING_LIST                               0x0029
#define BT_HCI_LE_READ_RESOLVING_LIST_SIZE                           0x002A
#define BT_HCI_LE_READ_PEER_RESOLVABLE_ADDRESS                       0x002B
#define BT_HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS                      0x002C
#define BT_HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE                      0x002D
#define BT_HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT             0x002E
#define BT_HCI_LE_READ_MAXIMUM_DATA_LENGTH                           0x002F
#define BT_HCI_LE_READ_PHY                                           0x0030
#define BT_HCI_LE_SET_DEFAULT_PHY                                    0x0031
#define BT_HCI_LE_SET_PHY                                            0x0032
#define BT_HCI_LE_ENHANCED_RECEIVER_TEST                             0x0033
#define BT_HCI_LE_ENHANCED_TRANSMITTER_TEST                          0x0034
#define BT_HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS                 0x0035
#define BT_HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETERS                0x0036
#define BT_HCI_LE_SET_EXTENDED_ADVERTISING_DATA                      0x0037
#define BT_HCI_LE_SET_EXTENDED_SCAN_RESPONSE_DATA                    0x0038
#define BT_HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE                    0x0039
#define BT_HCI_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH               0x003A
#define BT_HCI_LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS          0X003B
#define BT_HCI_LE_REMOVE_ADVERTISING_SET                             0X003C
#define BT_HCI_LE_CLEAR_ADVERTISING_SETS                             0X003D
#define BT_HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETERS                0x003E
#define BT_HCI_LE_SET_PERIODIC_ADVERTISING_DATA                      0x003F
#define BT_HCI_LE_SET_PERIODIC_ADVERTISING_ENABLE                    0x0040
#define BT_HCI_LE_SET_EXTENDED_SCAN_PARAMETERS                       0x0041
#define BT_HCI_LE_SET_EXTENDED_SCAN_ENABLE                           0x0042
#define BT_HCI_LE_EXTENDED_CREATE_CONNECTION                         0x0043
#define BT_HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC                   0x0044
#define BT_HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL            0x0045
#define BT_HCI_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC                0x0046
#define BT_HCI_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST             0x0047
#define BT_HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST        0x0048
#define BT_HCI_LE_CLEAR_PERIODIC_ADVERTISER_LIST                     0x0049
#define BT_HCI_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE                 0x004A
#define BT_HCI_LE_READ_TRANSMIT_POWER                                0x004B
#define BT_HCI_LE_READ_RF_PATH_COMPENSATION                          0x004C
#define BT_HCI_LE_WRITE_RF_PATH_COMPENSATION                         0x004D
#define BT_HCI_LE_SET_PRIVACY_MODE                                   0x004E
/* BT_HCI_VENDOR_SPECIFIC */
#define BT_HCI_VS_SET_PUBLIC_ADDRESS                                 0x0001
#define BT_HCI_VS_SET_TX_POWER                                       0x0002
#define BT_HCI_VS_CL_TEST                                            0x0003
#define BT_HCI_VS_DBG                                                0x0004

typedef enum {
    bt_err_success                             = 0x00,
    bt_err_unknown_hci_command                 = 0x01,
    bt_err_unknown_connection_identifier       = 0x02,
    bt_err_hardware_failure                    = 0x03,
    bt_err_page_timeout                        = 0x04,
    bt_err_authentication_failure              = 0x05,
    bt_err_pin_or_key_missing                  = 0x06,
    bt_err_memory_capacity_exceeded            = 0x07,
    bt_err_connection_timeout                  = 0x08,
    bt_err_connection_already_exists           = 0x0B,
    bt_err_command_disallowed                  = 0x0C,
    bt_err_connection_accept_timeout_exceeded  = 0x10,
    bt_err_unsupported_feature_or_parameter_value = 0x11,
    bt_err_invalid_hci_command_parameters      = 0x12,
    bt_err_remote_user_terminated_connection   = 0x13,
    bt_err_connection_terminated_by_local_host = 0x16,
    bt_err_unsupported_remote_feature          = 0x1A,
    bt_err_invalid_ll_parameters               = 0x1E,
    bt_err_unspecified_error                   = 0x1F,
    bt_err_unsupported_ll_parameter_value      = 0x20,
    bt_err_role_change_not_allowed             = 0x21,
    bt_err_ll_response_timeout                 = 0x22,
    bt_err_ll_error_transaction_collision      = 0x23,
    bt_err_ll_pdu_not_allowed                  = 0x24,
    bt_err_instant_passed                      = 0x28,
    bt_err_different_transaction_collision     = 0x2A,
    bt_err_role_switch_pending                 = 0x32,
    bt_err_role_switch_failed                  = 0x35,
    bt_err_extended_inquiry_response_too_large = 0x36,
    bt_err_unacceptable_connection_parameters  = 0x3B,
    bt_err_controller_busy                     = 0x3A,
    bt_err_advertising_timeout                 = 0x3C,
    bt_err_connection_terminated_due_to_mic_failure = 0x3D,
    bt_err_connection_failed_to_be_established      = 0x3E,
    bt_err_unknown_advertising_identifier           = 0x42,
    bt_err_limit_reached                            = 0x43,
    bt_err_operation_cancelled_by_host              = 0x44,
} bt_error_t;

typedef enum {
    BT_HCI_EVENT_INQUIRY_COMPLETE                         = 0x01,
    BT_HCI_EVENT_INQUIRY_RESULT                           = 0x02,
    BT_HCI_EVENT_CONNECTION_COMPLETE                      = 0x03,
    BT_HCI_EVENT_CONNECTION_REQUEST                       = 0x04,
    BT_HCI_EVENT_DISCONNECTION_COMPLETE                   = 0x05,
    BT_HCI_EVENT_AUTHENTICATION_COMPLETE                  = 0x06,
    BT_HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE             = 0x07,
    BT_HCI_EVENT_ENCRYPTION_CHANGE                        = 0x08,
    BT_HCI_EVENT_CHANGE_CONNECTION_LINK_KEY_COMPLETE      = 0x09,
    BT_HCI_EVENT_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE  = 0x0B,
    BT_HCI_EVENT_READ_REMOTE_VERSION_INFORMATION_COMPLETE = 0x0C,
    BT_HCI_EVENT_COMMAND_COMPLETE                         = 0x0E,
    BT_HCI_EVENT_COMMAND_STATUS                           = 0x0F,
    BT_HCI_EVENT_HARDWARE_ERROR                           = 0x10,
    BT_HCI_EVENT_ROLE_CHANGE                              = 0x12,
    BT_HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS              = 0x13,
    BT_HCI_EVENT_PIN_CODE_REQUEST                         = 0x16,
    BT_HCI_EVENT_LINK_KEY_REQUEST                         = 0x17,
    BT_HCI_EVENT_LINK_KEY_NOTIFICATION                    = 0x18,
    BT_HCI_EVENT_DATA_BUFFER_OVERFLOW                     = 0x1A,
    BT_HCI_EVENT_MAX_SLOTS_CHANGE                         = 0x1B,
    BT_HCI_EVENT_READ_CLOCK_OFFSET_COMPLETE               = 0x1C,
    BT_HCI_EVENT_CONNECTION_PACKET_TYPE_CHANGED           = 0x1D,
    BT_HCI_EVENT_PAGE_SCAN_REPETITION_MODE_CHANGE         = 0x20,
    BT_HCI_EVENT_INQUIRY_RESULT_RSSI                      = 0x22,
    BT_HCI_EVENT_READ_REMOTE_EXTENDED_FEATURES_COMPLETE   = 0x23,
    BT_HCI_EVENT_INQUIRY_RESULT_EXTENDED                  = 0x2F,
    BT_HCI_EVENT_ENCRYPTION_KEY_REFRESH_COMPLETE          = 0x30,
    BT_HCI_EVENT_LE_META                                  = 0x3E,
    BT_HCI_EVENT_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED    = 0x57,
} bt_hci_event_code_t;

typedef enum {
    BT_HCI_LE_SUBEVENT_CONNECTION_COMPLETE                   = 0x01,
    BT_HCI_LE_SUBEVENT_ADVERTISING_REPORT                    = 0x02,
    BT_HCI_LE_SUBEVENT_CONNECTION_UPDATE_COMPLETE            = 0x03,
    BT_HCI_LE_SUBEVENT_READ_REMOTE_USED_FEATURES_COMPLETE    = 0x04,
    BT_HCI_LE_SUBEVENT_LONG_TERM_KEY_REQUEST                 = 0x05,
    BT_HCI_LE_SUBEVENT_REMOTE_CONNECTION_PARAMETER_REQUEST   = 0x06,
    BT_HCI_LE_SUBEVENT_DATA_LENGTH_CHANGE                    = 0x07,
    BT_HCI_LE_SUBEVENT_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE   = 0x08,
    BT_HCI_LE_SUBEVENT_GENERATE_DHKEY_COMPLETE               = 0x09,
    BT_HCI_LE_SUBEVENT_ENHANCED_CONNECTION_COMPLETE          = 0x0A,
    BT_HCI_LE_SUBEVENT_DIRECT_ADVERTISING_REPORT             = 0x0B,
    BT_HCI_LE_SUBEVENT_PHY_UPDATE_COMPLETE                   = 0x0C,
    BT_HCI_LE_SUBEVENT_EXTENDED_ADVERTISING_REPORT           = 0x0D,
    BT_HCI_LE_SUBEVENT_PERIODIC_ADVERTISING_SYNC_ESTABLISHED = 0x0E,
    BT_HCI_LE_SUBEVENT_PERIODIC_ADVERTISING_REPORT           = 0x0F,
    BT_HCI_LE_SUBEVENT_PERIODIC_ADVERTISING_SYNC_LOST        = 0x10,
    BT_HCI_LE_SUBEVENT_SCAN_TIMEOUT                          = 0x11,
    BT_HCI_LE_SUBEVENT_ADVERTISING_SET_TERMINATED            = 0x12,
    BT_HCI_LE_SUBEVENT_SCAN_REQUEST_RECEIVED                 = 0x13,
    BT_HCI_LE_SUBEVENT_CHANNEL_SELECTION_ALGORITHM           = 0x14,
} bt_hci_le_subevent_code_t;

#define BT_HCI_EVENT_MASK_DISCONNECTION_COMPLETE                    4
#define BT_HCI_EVENT_MASK_ENCRYPTION_CHANGE                         7
#define BT_HCI_EVENT_MASK_READ_REMOTE_VERSION_INFORMATION_COMPLETE 11
#define BT_HCI_EVENT_MASK_HARDWARE_ERROR                           15
#define BT_HCI_EVENT_MASK_DATA_BUFFER_OVERFLOW                     25
#define BT_HCI_EVENT_MASK_ENCRYPTION_KEY_REFRESH_COMPLETE          47
#define BT_HCI_EVENT_MASK_LE_META                                  61

#define BT_HCI_EVENT_MASK_2_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED  23

#define BT_HCI_EVENT_MASK_LE_CONNECTION_COMPLETE                    0
#define BT_HCI_EVENT_MASK_LE_ADVERTISING_REPORT                     1
#define BT_HCI_EVENT_MASK_LE_CONNECTION_UPDATE_COMPLETE             2
#define BT_HCI_EVENT_MASK_LE_READ_REMOTE_USED_FEATURES_COMPLETE     3
#define BT_HCI_EVENT_MASK_LE_LONG_TERM_KEY_REQUEST                  4
#define BT_HCI_EVENT_MASK_LE_REMOTE_CONNECTION_PARAMETER_REQUEST    5
#define BT_HCI_EVENT_MASK_LE_DATA_LENGTH_CHANGE                     6
#define BT_HCI_EVENT_MASK_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE    7
#define BT_HCI_EVENT_MASK_LE_GENERATE_DHKEY_COMPLETE                8
#define BT_HCI_EVENT_MASK_LE_ENHANCED_CONNECTION_COMPLETE           9
#define BT_HCI_EVENT_MASK_LE_DIRECT_ADVERTISING_REPORT             10
#define BT_HCI_EVENT_MASK_LE_PHY_UPDATE_COMPLETE                   11
#define BT_HCI_EVENT_MASK_LE_EXTENDED_ADVERTISING_REPORT           12
#define BT_HCI_EVENT_MASK_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED 13
#define BT_HCI_EVENT_MASK_LE_PERIODIC_ADVERTISING_REPORT           14
#define BT_HCI_EVENT_MASK_LE_PERIODIC_ADVERTISING_SYNC_LOST        15
#define BT_HCI_EVENT_MASK_LE_SCAN_TIMEOUT                          16
#define BT_HCI_EVENT_MASK_LE_ADVERTISING_SET_TERMINATED            17
#define BT_HCI_EVENT_MASK_LE_SCAN_REQUEST_RECEIVED                 18
#define BT_HCI_EVENT_MASK_LE_CHANNEL_SELECTION_ALGORITHM           19

/* Vol 6, part B 4.6 */
#define BT_HCI_LE_FEATURE_ENCRYPTION                   (1<<( 0-(0*8)))
#define BT_HCI_LE_FEATURE_CONNECTION_PARAMETER_REQUEST (1<<( 1-(0*8)))
#define BT_HCI_LE_FEATURE_EXTENDED_REJECTION           (1<<( 2-(0*8)))
#define BT_HCI_LE_FEATURE_SLAVE_INITIATED_EXCHANGE     (1<<( 3-(0*8)))
#define BT_HCI_LE_FEATURE_PING                         (1<<( 4-(0*8)))
#define BT_HCI_LE_FEATURE_DATA_EXTENSION               (1<<( 5-(0*8)))
#define BT_HCI_LE_FEATURE_PRIVACY                      (1<<( 6-(0*8)))
#define BT_HCI_LE_FEATURE_EXTENDED_SCANNER             (1<<( 7-(0*8)))
#define BT_HCI_LE_FEATURE_2M_PHY                       (1<<( 8-(1*8)))
#define BT_HCI_LE_FEATURE_STABLE_MOD_INDEX_TX          (1<<( 9-(1*8)))
#define BT_HCI_LE_FEATURE_STABLE_MOD_INDEX_RX          (1<<(10-(1*8)))
#define BT_HCI_LE_FEATURE_CODED_PHY                    (1<<(11-(1*8)))
#define BT_HCI_LE_FEATURE_EXTENDED_ADVERTISING         (1<<(12-(1*8)))
#define BT_HCI_LE_FEATURE_PERIODIC_ADVERTISING         (1<<(13-(1*8)))
#define BT_HCI_LE_FEATURE_CHANNEL_ALGORITHM_2          (1<<(14-(1*8)))
#define BT_HCI_LE_FEATURE_POWER_CLASS_1                (1<<(15-(1*8)))
#define BT_HCI_LE_FEATURE_MIN_NUM_USED_CHANNELS_PROC   (1<<(16-(2*8)))

#define BT_HCI_ROLE_MASTER 0
#define BT_HCI_ROLE_SLAVE 1

#define BT_HCI_EVENT_MAX_LENGTH 255
#define BT_HCI_NAME_LENGTH 248

#define BT_HCI_ADV_DATA_LEG_MAX 31
#define BT_HCI_ADV_DATA_EXT_MAX 251

typedef struct {
    uint8_t lap[3];
    uint8_t length;
    uint8_t num_resp;
} __attribute__ ((packed)) bt_hci_cmd_inquiry_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_inquiry_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_inquiry_cancel_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_inquiry_cancel_t;

typedef struct {
    uint8_t address[6];
    uint16_t pkt_type;
    uint8_t sr_mode;
    uint8_t reserved;
    uint16_t clk_offset;
    uint8_t rswitch_allowed;
} __attribute__ ((packed)) bt_hci_cmd_connect_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_connect_t;

typedef struct {
    uint16_t handle;
    uint8_t reason;
} __attribute__ ((packed)) bt_hci_cmd_disconnect_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_disconnect_t;

typedef struct {
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_cmd_connect_cancel_t;

typedef struct {
    uint8_t status;
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_rsp_connect_cancel_t;

typedef struct {
    uint8_t address[6];
    uint8_t role;
} __attribute__ ((packed)) bt_hci_cmd_accept_conn_req_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_accept_conn_req_t;

typedef struct {
    uint8_t address[6];
    uint8_t reason;
} __attribute__ ((packed)) bt_hci_cmd_reject_conn_req_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_reject_conn_req_t;

typedef struct {
    uint8_t address[6];
    uint8_t key[16];
} __attribute__ ((packed)) bt_hci_cmd_link_key_req_reply_t;

typedef struct {
    uint8_t status;
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_rsp_link_key_req_reply_t;

typedef struct {
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_cmd_link_key_req_neg_reply_t;

typedef struct {
    uint8_t status;
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_rsp_link_key_req_neg_reply_t;

typedef struct {
    uint8_t address[6];
    uint8_t length;
    uint8_t pin[16];
} __attribute__ ((packed)) bt_hci_cmd_pin_req_reply_t;

typedef struct {
    uint8_t status;
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_rsp_pin_req_reply_t;

typedef struct {
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_cmd_pin_req_neg_reply_t;

typedef struct {
    uint8_t status;
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_rsp_pin_req_neg_reply_t;

typedef struct {
    uint16_t handle;
    uint16_t pkt_type;
} __attribute__ ((packed)) bt_hci_cmd_change_conn_pkt_type_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_change_conn_pkt_type_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_auth_req_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_auth_req_t;

typedef struct {
    uint16_t handle;
    uint8_t enable;
} __attribute__ ((packed)) bt_hci_cmd_conn_encr_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_conn_encr_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_change_link_key_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_change_link_key_t;

typedef struct {
    uint8_t address[6];
    uint8_t sr_mode;
    uint8_t reserved;
    uint16_t clk_offset;
} __attribute__ ((packed)) bt_hci_cmd_remote_name_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_remote_name_t;

typedef struct {
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_cmd_remote_name_cancel_t;

typedef struct {
    uint8_t status;
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_rsp_remote_name_cancel_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_read_remote_sup_feat_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_read_remote_sup_feat_t;

typedef struct {
    uint16_t handle;
    uint8_t page;
} __attribute__ ((packed)) bt_hci_cmd_read_remote_ext_feat_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_read_remote_ext_feat_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_read_remote_version_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_read_remote_version_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_read_clock_offset_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_read_clock_offset_t;

typedef struct {
    uint8_t address[6];
    uint8_t role;
} __attribute__ ((packed)) bt_hci_cmd_switch_role_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_switch_role_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_read_link_policy_t;

typedef struct {
    uint8_t status;
    uint16_t handle;
    uint16_t policy;
} __attribute__ ((packed)) bt_hci_rsp_read_link_policy_t;

typedef struct {
    uint16_t handle;
    uint16_t policy;
} __attribute__ ((packed)) bt_hci_cmd_write_link_policy_t;

typedef struct {
    uint8_t status;
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_rsp_write_link_policy_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_def_link_policy_t;

typedef struct {
    uint8_t status;
    uint16_t policy;
} __attribute__ ((packed)) bt_hci_rsp_read_def_link_policy_t;

typedef struct {
    uint16_t policy;
} __attribute__ ((packed)) bt_hci_cmd_write_def_link_policy_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_write_def_link_policy_t;

typedef struct {
    uint8_t mask[8];
} __attribute__ ((packed)) bt_hci_cmd_set_event_mask_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_set_event_mask_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_reset_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_reset_t;

typedef struct {
    uint8_t type;
    uint8_t condition_type;
    uint8_t condition[7];
} __attribute__ ((packed)) bt_hci_cmd_set_event_filter_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_set_event_filter_t;

typedef struct {
    uint8_t name[248];
} __attribute__ ((packed)) bt_hci_cmd_write_local_name_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_write_local_name_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_local_name_t;

typedef struct {
    uint8_t status;
    uint8_t name[248];
} __attribute__ ((packed)) bt_hci_rsp_read_local_name_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_conn_accept_to_t;

typedef struct {
    uint8_t status;
    uint16_t timeout;
} __attribute__ ((packed)) bt_hci_rsp_read_conn_accept_to_t;

typedef struct {
    uint16_t timeout;
} __attribute__ ((packed)) bt_hci_cmd_write_conn_accept_to_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_write_conn_accept_to_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_page_to_t;

typedef struct {
    uint8_t status;
    uint16_t timeout;
} __attribute__ ((packed)) bt_hci_rsp_read_page_to_t;

typedef struct {
    uint16_t timeout;
} __attribute__ ((packed)) bt_hci_cmd_write_page_to_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_write_page_to_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_scan_enable_t;

typedef struct {
    uint8_t status;
    uint8_t enable;
} __attribute__ ((packed)) bt_hci_rsp_read_scan_enable_t;

typedef struct {
    uint8_t enable;
} __attribute__ ((packed)) bt_hci_cmd_write_scan_enable_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_write_scan_enable_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_page_scan_activity_t;

typedef struct {
    uint8_t status;
    uint16_t interval;
    uint16_t window;
} __attribute__ ((packed)) bt_hci_rsp_read_page_scan_activity_t;

typedef struct {
    uint16_t interval;
    uint16_t window;
} __attribute__ ((packed)) bt_hci_cmd_write_page_scan_activity_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_write_page_scan_activity_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_inquiry_scan_activity_t;

typedef struct {
    uint8_t status;
    uint16_t interval;
    uint16_t window;
} __attribute__ ((packed)) bt_hci_rsp_read_inquiry_scan_activity_t;

typedef struct {
    uint16_t interval;
    uint16_t window;
} __attribute__ ((packed)) bt_hci_cmd_write_inquiry_scan_activity_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_write_inquiry_scan_activity_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_cod_t;

typedef struct {
    uint8_t status;
    uint8_t cod[3];

} __attribute__ ((packed)) bt_hci_rsp_read_cod_t;

typedef struct {
    uint8_t cod[3];
} __attribute__ ((packed)) bt_hci_cmd_write_cod_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_write_cod_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_voice_setting_t;

typedef struct {
    uint8_t status;
    uint16_t setting;
} __attribute__ ((packed)) bt_hci_rsp_read_voice_setting_t;

typedef struct {
    uint16_t handle;
    uint8_t type;
} __attribute__ ((packed)) bt_hci_cmd_read_tx_power_t;

typedef struct {
    uint8_t status;
    uint16_t handle;
    uint8_t tx_power;
} __attribute__ ((packed)) bt_hci_rsp_read_tx_power_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_nof_iac_t;

typedef struct {
    uint8_t status;
    uint8_t num;
} __attribute__ ((packed)) bt_hci_rsp_read_nof_iac_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_iac_t;

typedef struct {
    uint8_t status;
    uint8_t num;
    uint8_t lap[3];
} __attribute__ ((packed)) bt_hci_rsp_read_iac_t;

typedef struct {
    uint8_t num;
    uint8_t lap[3];
} __attribute__ ((packed)) bt_hci_cmd_write_iac_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_write_iac_t;

typedef struct {
    uint8_t map[10];
} __attribute__ ((packed)) bt_hci_cmd_set_afh_map_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_set_afh_map_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_imode_t;

typedef struct {
    uint8_t status;
    uint8_t mode;
} __attribute__ ((packed)) bt_hci_rsp_read_imode_t;

typedef struct {
    uint8_t mode;
} __attribute__ ((packed)) bt_hci_cmd_write_imode_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_write_imode_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_ext_inq_resp_t;

typedef struct {
    uint8_t status;
    uint8_t fec;
    uint8_t eir[240];
} __attribute__ ((packed)) bt_hci_rsp_read_ext_inq_resp_t;

typedef struct {
    uint8_t fec;
    uint8_t eir[240];
} __attribute__ ((packed)) bt_hci_cmd_write_ext_inq_resp_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_write_ext_inq_resp_t;

typedef struct {
    uint8_t mask[8];
} __attribute__ ((packed)) bt_hci_cmd_set_event_mask_page_2_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_set_event_mask_page_2_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_read_auth_payload_to_t;

typedef struct {
    uint8_t status;
    uint16_t handle;
    uint16_t timeout;
} __attribute__ ((packed)) bt_hci_rsp_read_auth_payload_to_t;

typedef struct {
    uint16_t handle;
    uint16_t timeout;
} __attribute__ ((packed)) bt_hci_cmd_write_auth_payload_to_t;

typedef struct {
    uint8_t status;
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_rsp_write_auth_payload_to_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_local_version_t;

typedef struct {
    uint8_t status;
    uint8_t hci_version;
    uint16_t hci_revision;
    uint8_t lmp_version;
    uint16_t manufacturer;
    uint16_t lmp_subversion;
} __attribute__ ((packed)) bt_hci_rsp_read_local_version_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_local_commands_t;

typedef struct {
    uint8_t status;
    uint8_t commands[64];
} __attribute__ ((packed)) bt_hci_rsp_read_local_commands_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_local_sup_feat_t;

typedef struct {
    uint8_t status;
    uint8_t features[8];
} __attribute__ ((packed)) bt_hci_rsp_read_local_sup_feat_t;

typedef struct {
    uint8_t page;
} __attribute__ ((packed)) bt_hci_cmd_read_local_ext_feat_t;

typedef struct {
    uint8_t status;
    uint8_t page;
    uint8_t max_page;
    uint8_t features[8];
} __attribute__ ((packed)) bt_hci_rsp_read_local_ext_feat_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_buffer_size_t;

typedef struct {
    uint8_t status;
    uint16_t acl_size;
    uint8_t sco_size;
    uint16_t acl_tickets;
    uint16_t sco_tickets;
} __attribute__ ((packed)) bt_hci_rsp_read_buffer_size_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_read_bd_addr_t;

typedef struct {
    uint8_t status;
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_rsp_read_bd_addr_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_read_rssi_t;

typedef struct {
    uint8_t status;
    uint16_t handle;
    int8_t rssi;
} __attribute__ ((packed)) bt_hci_rsp_read_rssi_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_read_afh_map_t;

typedef struct {
    uint8_t status;
    uint16_t handle;
    uint8_t mode;
    uint8_t map[10];
} __attribute__ ((packed)) bt_hci_rsp_read_afh_map_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_enable_dut_mode_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_enable_dut_mode_t;

typedef struct {
    uint8_t mask[8];
} __attribute__ ((packed)) bt_hci_cmd_le_set_event_mask_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_event_mask_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_buffer_size_t;

typedef struct {
    uint8_t status;
    uint16_t size;
    uint8_t tickets;
} __attribute__ ((packed)) bt_hci_rsp_le_read_buffer_size_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_local_features_t;

typedef struct {
    uint8_t status;
    uint8_t features[8];
} __attribute__ ((packed)) bt_hci_rsp_le_read_local_features_t;

typedef struct {
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_cmd_le_set_random_address_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_random_address_t;

typedef struct {
    uint16_t interval_min;
    uint16_t interval_max;
    uint8_t adv_type;
    uint8_t own_addr_type;
    uint8_t peer_addr_type;
    uint8_t peer_addr[6];
    uint8_t channel_map;
    uint8_t filter_policy;
} __attribute__ ((packed)) bt_hci_cmd_le_set_adv_params_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_adv_params_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_adv_tx_power_t;

typedef struct {
    uint8_t status;
    uint8_t tx_power;
} __attribute__ ((packed)) bt_hci_rsp_le_read_adv_tx_power_t;

typedef struct {
    uint8_t length;
    uint8_t data[31];
} __attribute__ ((packed)) bt_hci_cmd_le_set_adv_data_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_adv_data_t;

typedef struct {
    uint8_t length;
    uint8_t data[31];
} __attribute__ ((packed)) bt_hci_cmd_le_set_scan_data_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_scan_data_t;

typedef struct {
    uint8_t enable;
} __attribute__ ((packed)) bt_hci_cmd_le_set_adv_enable_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_adv_enable_t;

typedef struct {
    uint8_t scan_type;
    uint16_t interval;
    uint16_t window;
    uint8_t own_addr_type;
    uint8_t filter_policy;
} __attribute__ ((packed)) bt_hci_cmd_le_set_scan_params_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_scan_params_t;

typedef struct {
    uint8_t enable;
    uint8_t filter_duplicates;
} __attribute__ ((packed)) bt_hci_cmd_le_set_scan_enable_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_scan_enable_t;

typedef struct {
    uint16_t scan_interval;
    uint16_t scan_window;
    uint8_t filter_policy;
    uint8_t peer_addr_type;
    uint8_t peer_addr[6];
    uint8_t own_addr_type;
    uint16_t interval_min;
    uint16_t interval_max;
    uint16_t latency;
    uint16_t timeout;
    uint16_t min_ce;
    uint16_t max_ce;
} __attribute__ ((packed)) bt_hci_cmd_le_create_conn_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_le_create_conn_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_create_conn_cancel_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_create_conn_cancel_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_white_list_size_t;

typedef struct {
    uint8_t status;
    uint8_t size;
} __attribute__ ((packed)) bt_hci_rsp_le_read_white_list_size_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_clear_white_list_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_clear_white_list_t;

typedef struct {
    uint8_t type;
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_cmd_le_add_white_list_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_add_white_list_t;

typedef struct {
    uint8_t type;
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_cmd_le_remove_white_list_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_remove_white_list_t;

typedef struct {
    uint16_t handle;
    uint16_t interval_min;
    uint16_t interval_max;
    uint16_t latency;
    uint16_t timeout;
    uint16_t min_ce;
    uint16_t max_ce;
} __attribute__ ((packed)) bt_hci_cmd_le_conn_update_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_le_conn_update_t;

typedef struct {
    uint8_t channel_map[5];
} __attribute__ ((packed)) bt_hci_cmd_le_set_host_channel_map_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_host_channel_map_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_le_read_channel_map_t;

typedef struct {
    uint8_t status;
    uint16_t handle;
    uint8_t channel_map[5];
} __attribute__ ((packed)) bt_hci_rsp_le_read_channel_map_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_le_read_remote_features_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_le_read_remote_features_t;

typedef struct {
    uint8_t key[16];
    uint8_t data[16];
} __attribute__ ((packed)) bt_hci_cmd_le_encrypt_t;

typedef struct {
    uint8_t status;
    uint8_t data[16];
} __attribute__ ((packed)) bt_hci_rsp_le_encrypt_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_rand_t;

typedef struct {
    uint8_t status;
    uint8_t rand[8];
} __attribute__ ((packed)) bt_hci_rsp_le_rand_t;

typedef struct {
    uint16_t handle;
    uint8_t rand[8];
    uint16_t ediv;
    uint8_t ltk[16];
} __attribute__ ((packed)) bt_hci_cmd_le_start_encrypt_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_le_start_encrypt_t;

typedef struct  {
    uint16_t handle;
    uint8_t  ltk[16];
} __attribute__ ((packed)) bt_hci_cmd_le_long_term_key_req_reply_t;

typedef struct  {
    uint8_t status;
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_rsp_le_long_term_key_req_reply_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_le_long_term_key_req_neg_reply_t;

typedef struct  {
    uint8_t status;
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_rsp_le_long_term_key_req_neg_reply_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_supported_state_t;

typedef struct {
    uint8_t status;
    uint8_t states[8];
} __attribute__ ((packed)) bt_hci_rsp_le_read_supported_state_t;

typedef struct {
    uint8_t channel;
} __attribute__ ((packed)) bt_hci_cmd_le_receiver_test_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_receiver_test_t;

typedef struct {
    uint8_t channel;
    uint8_t length;
    uint8_t payload;
} __attribute__ ((packed)) bt_hci_cmd_le_transmitter_test_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_transmitter_test_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_end_test_t;

typedef struct {
    uint8_t status;
    uint16_t packets;
    //uint16_t packets_crc_fail;
    //uint16_t packets_corr_fail;
} __attribute__ ((packed)) bt_hci_rsp_le_end_test_t;

typedef struct {
    uint16_t handle;
    uint16_t interval_min;
    uint16_t interval_max;
    uint16_t latency;
    uint16_t timeout;
    uint16_t min_ce;
    uint16_t max_ce;
} __attribute__ ((packed)) bt_hci_cmd_le_remote_conn_param_req_reply_t;

typedef struct {
    uint8_t status;
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_rsp_le_remote_conn_param_req_reply_t;

typedef struct {
    uint16_t handle;
    uint8_t reason;
} __attribute__ ((packed)) bt_hci_cmd_le_remote_conn_param_req_neg_reply_t;

typedef struct {
    uint8_t status;
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_rsp_le_remote_conn_param_req_neg_reply_t;

typedef struct {
    uint16_t handle;
    uint16_t tx_octets;
    uint16_t tx_time;
} __attribute__ ((packed)) bt_hci_cmd_le_set_data_length_t;

typedef struct {
    uint8_t status;
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_rsp_le_set_data_length_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_default_data_length_t;

typedef struct {
    uint8_t status;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
} __attribute__ ((packed)) bt_hci_rsp_le_read_default_data_length_t;

typedef struct {
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
} __attribute__ ((packed)) bt_hci_cmd_le_write_default_data_length_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_write_default_data_length_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_local_p256_public_key_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_le_read_local_p256_public_key_t;

typedef struct {
    uint8_t key[64];
} __attribute__ ((packed)) bt_hci_cmd_le_generate_dhkey_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_le_generate_dhkey_t;

typedef struct {
    uint8_t id_addr_type;
    uint8_t id_addr[6];
    uint8_t peer_irk[16];
    uint8_t local_irk[16];
} __attribute__ ((packed)) bt_hci_cmd_le_add_resolving_list_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_add_resolving_list_t;

typedef struct {
    uint8_t id_addr_type;
    uint8_t id_addr[6];
} __attribute__ ((packed)) bt_hci_cmd_le_remove_resolving_list_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_remove_resolving_list_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_clear_resolving_list_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_clear_resolving_list_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_resolving_list_size_t;

typedef struct {
    uint8_t status;
    uint8_t size;
} __attribute__ ((packed)) bt_hci_rsp_le_read_resolving_list_size_t;

typedef struct {
    uint8_t id_addr_type;
    uint8_t id_addr[6];
} __attribute__ ((packed)) bt_hci_cmd_le_read_resolvable_addr_t;

typedef struct {
    uint8_t status;
    uint8_t resolvable_addr[6];
} __attribute__ ((packed)) bt_hci_rsp_le_read_resolvable_addr_t;

typedef struct {
    uint8_t enable;
} __attribute__ ((packed)) bt_hci_cmd_le_set_address_resolution_enable_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_address_resolution_enable_t;

typedef struct {
    uint16_t timeout;
} __attribute__ ((packed)) bt_hci_cmd_le_set_resolvable_address_to_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_resolvable_address_to_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_maximum_data_length_t;

typedef struct {
    uint8_t status;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
    uint16_t max_rx_octets;
    uint16_t max_rx_time;
} __attribute__ ((packed)) bt_hci_rsp_le_read_maximum_data_length_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_cmd_le_read_phy_t;

typedef struct {
    uint8_t status;
    uint16_t handle;
    uint8_t tx_phy;
    uint8_t rx_phy;
} __attribute__ ((packed)) bt_hci_rsp_le_read_phy_t;

typedef struct {
    uint8_t all_phys;
    uint8_t tx_phys;
    uint8_t rx_phys;
} __attribute__ ((packed)) bt_hci_cmd_le_set_default_phy_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_default_phy_t;

typedef struct {
    uint16_t handle;
    uint8_t  all_phys;
    uint8_t  tx_phys;
    uint8_t  rx_phys;
    uint16_t phy_options;
} __attribute__ ((packed)) bt_hci_cmd_le_set_phy_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_le_set_phy_t;

typedef struct {
    uint8_t channel;
    uint8_t phy;
    uint8_t mod_index;
} __attribute__ ((packed)) bt_hci_cmd_le_enh_receiver_test_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_enh_receiver_test_t;

typedef struct {
    uint8_t channel;
    uint8_t length;
    uint8_t payload;
    uint8_t phy;
} __attribute__ ((packed)) bt_hci_cmd_le_enh_transmitter_test_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_enh_transmitter_test_t;

typedef struct {
    uint8_t adv_handle;
    uint8_t address[6];
} __attribute__ ((packed)) bt_hci_cmd_le_set_adv_set_random_addr_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_adv_set_random_addr_t;

typedef struct {
    uint8_t adv_handle;
    uint16_t properties;
    uint8_t pri_interval_min[3];
    uint8_t pri_interval_max[3];
    uint8_t pri_channel_map;
    uint8_t own_addr_type;
    uint8_t peer_addr_type;
    uint8_t peer_addr[6];
    uint8_t filter_policy;
    uint8_t tx_power;
    uint8_t pri_phy;
    uint8_t sec_max_skip;
    uint8_t sec_phy;
    uint8_t sid;
    uint8_t srn_enable;
} __attribute__ ((packed)) bt_hci_cmd_le_set_ext_adv_params_t;

typedef struct {
    uint8_t status;
    uint8_t tx_power;
} __attribute__ ((packed)) bt_hci_rsp_le_set_ext_adv_params_t;

typedef struct {
    uint8_t adv_handle;
    uint8_t operation;
    uint8_t fragment;
    uint8_t length;
    uint8_t data[0];
} __attribute__ ((packed)) bt_hci_cmd_le_set_ext_adv_data_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_ext_adv_data_t;

typedef struct {
    uint8_t adv_handle;
    uint8_t operation;
    uint8_t fragment;
    uint8_t length;
    uint8_t data[0];
} __attribute__ ((packed)) bt_hci_cmd_le_set_ext_scan_data_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_ext_scan_data_t;

typedef struct {
    uint8_t enable;
    uint8_t sets;
    struct {
        uint8_t adv_handle;
        uint16_t duration;
        uint8_t events;
    } __attribute__ ((packed)) i[1];
} __attribute__ ((packed)) bt_hci_cmd_le_set_ext_adv_enable_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_ext_adv_enable_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_max_adv_data_length_t;

typedef struct {
    uint8_t status;
    uint16_t length;
} __attribute__ ((packed)) bt_hci_rsp_le_read_max_adv_data_length_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_nof_supported_adv_sets_t;

typedef struct {
    uint8_t status;
    uint8_t sets;
} __attribute__ ((packed)) bt_hci_rsp_le_read_nof_supported_adv_sets_t;

typedef struct {
    uint8_t adv_handle;
} __attribute__ ((packed)) bt_hci_cmd_le_remove_adv_set_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_remove_adv_set_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_clear_adv_sets_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_clear_adv_sets_t;

typedef struct {
    uint8_t adv_handle;
    uint16_t interval_min;
    uint16_t interval_max;
    uint16_t properties;
} __attribute__ ((packed)) bt_hci_cmd_le_set_per_adv_params_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_per_adv_params_t;

typedef struct {
    uint8_t adv_handle;
    uint8_t operation;
    uint8_t length;
    uint8_t data[0];
} __attribute__ ((packed)) bt_hci_cmd_le_set_per_adv_data_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_per_adv_data_t;

typedef struct {
    uint8_t enable;
    uint8_t adv_handle;
} __attribute__ ((packed)) bt_hci_cmd_le_set_per_adv_enable_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_per_adv_enable_t;

typedef struct {
    uint8_t own_addr_type;
    uint8_t filter_policy;
    uint8_t phys;
    struct {
        uint8_t scan_type;
        uint16_t interval;
        uint16_t window;
    } __attribute__ ((packed)) i[2]; // two bits so far defined
} __attribute__ ((packed)) bt_hci_cmd_le_set_ext_scan_params_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_ext_scan_params_t;

typedef struct {
    uint8_t enable;
    uint8_t filter_duplicates;
    uint16_t duration;
    uint16_t period;
} __attribute__ ((packed)) bt_hci_cmd_le_set_ext_scan_enable_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_ext_scan_enable_t;

typedef struct {
    uint8_t filter_policy;
    uint8_t own_addr_type;
    uint8_t peer_addr_type;
    uint8_t peer_addr[6];
    uint8_t phys;
    struct {
        uint16_t scan_interval;
        uint16_t scan_window;
        uint16_t interval_min;
        uint16_t interval_max;
        uint16_t latency;
        uint16_t timeout;
        uint16_t min_ce;
        uint16_t max_ce;
    } __attribute__ ((packed)) i[3]; // three bits so far defined
} __attribute__ ((packed)) bt_hci_cmd_le_ext_create_conn_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_le_ext_create_conn_t;

typedef struct {
    uint8_t filter_policy;
    uint8_t sid;
    uint8_t type;
    uint8_t address[6];
    uint16_t skip;
    uint16_t timeout;
    uint8_t unused;
} __attribute__ ((packed)) bt_hci_cmd_le_per_adv_create_sync_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_rsp_le_per_adv_create_sync_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_per_adv_create_sync_cancel_t;

typedef struct {
    uint8_t  status;
} __attribute__ ((packed)) bt_hci_rsp_le_per_adv_create_sync_cancel_t;

typedef struct {
    uint8_t sync_handle;
} __attribute__ ((packed)) bt_hci_cmd_le_per_adv_terminate_sync_t;

typedef struct {
    uint8_t  status;
} __attribute__ ((packed)) bt_hci_rsp_le_per_adv_terminate_sync_t;

typedef struct {
    uint8_t type;
    uint8_t address[6];
    uint8_t sid;
} __attribute__ ((packed)) bt_hci_cmd_le_add_per_adv_list_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_add_per_adv_list_t;

typedef struct {
    uint8_t type;
    uint8_t address[6];
    uint8_t sid;
} __attribute__ ((packed)) bt_hci_cmd_le_remove_per_adv_list_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_remove_per_adv_list_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_clear_per_adv_list_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_clear_per_adv_list_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_per_adv_list_size_t;

typedef struct {
    uint8_t status;
    uint8_t size;
} __attribute__ ((packed)) bt_hci_rsp_le_read_per_adv_list_size_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_tx_power_t;

typedef struct {
    uint8_t status;
    uint8_t power_min;
    uint8_t power_max;
} __attribute__ ((packed)) bt_hci_rsp_le_read_tx_power_t;

typedef struct {
} __attribute__ ((packed)) bt_hci_cmd_le_read_rf_path_compensation_t;

typedef struct {
    uint8_t status;
    uint8_t tx_path;
    uint8_t rx_path;
} __attribute__ ((packed)) bt_hci_rsp_le_read_rf_path_compensation_t;

typedef struct {
    uint8_t tx_path;
    uint8_t rx_path;
} __attribute__ ((packed)) bt_hci_cmd_le_write_rf_path_compensation_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_write_rf_path_compensation_t;

typedef struct {
    uint8_t type;
    uint8_t address[6];
    uint8_t mode;
} __attribute__ ((packed)) bt_hci_cmd_le_set_privacy_mode_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_le_set_privacy_mode_t;

typedef struct {
    int8_t power;
} __attribute__ ((packed)) bt_hci_cmd_vs_set_tx_power_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_vs_set_tx_power_t;

typedef struct {
    uint8_t state;
    uint8_t pkt_type;
    uint8_t no_whitening;
    uint8_t encr;
    uint8_t edr;
} __attribute__ ((packed)) bt_hci_cmd_vs_cl_test_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_vs_cl_test_t;

#define BT_VS_DBG_FLAG_HCIC (1<<0) // HCI commands
#define BT_VS_DBG_FLAG_HCIE (1<<1) // HCI events
#define BT_VS_DBG_FLAG_HCID (1<<2) // HCI data
#define BT_VS_DBG_FLAG_LL   (1<<3) // LL protocol
#define BT_VS_DBG_FLAG_LM   (1<<4) // LM protocol
#define BT_VS_DBG_FLAG_JOB  (1<<8) // PIP log: COP jobs and status
#define BT_VS_DBG_FLAG_HOP  (1<<9) // PIP log: frequency hops (requires BT_VS_DBG_FLAG_JOB)
#define BT_VS_DBG_FLAG_UNLIM_EV (1<<10) // PIP log: unlimited event length

typedef struct {
    uint32_t flags;
} __attribute__ ((packed)) bt_hci_cmd_vs_dbg_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_rsp_vs_dbg_t;

typedef struct {
    uint8_t status;
} __attribute__ ((packed)) bt_hci_evt_inq_cmpl_t;

typedef struct {
    uint8_t num_resp;
    uint8_t address[6];
    uint8_t sr_mode;
    uint16_t reserved;
    uint8_t cod[3];
    uint16_t clk_offset;
} __attribute__ ((packed)) bt_hci_evt_inq_result_t;

typedef struct {
    uint8_t num_resp;
    uint8_t address[6];
    uint8_t sr_mode;
    uint8_t reserved;
    uint8_t cod[3];
    uint16_t clk_offset;
    int8_t rssi;
} __attribute__ ((packed)) bt_hci_evt_inq_result_rssi_t;

typedef struct {
    uint8_t num_resp;
    uint8_t address[6];
    uint8_t sr_mode;
    uint8_t reserved;
    uint8_t cod[3];
    uint16_t clk_offset;
    int8_t rssi;
    uint8_t eir[240];
} __attribute__ ((packed)) bt_hci_evt_inq_result_extended_t;

typedef struct {
    uint8_t  status;
    uint16_t handle;
    uint8_t  addr[6];
    uint8_t  link_type;
    uint8_t  encryption;
} __attribute__ ((packed)) bt_hci_evt_conn_cmpl_t;

typedef struct {
    uint8_t addr[6];
    uint8_t cod[3];
    uint8_t link_type;
} __attribute__ ((packed)) bt_hci_evt_conn_req_t;

typedef struct {
    uint8_t  status;
    uint16_t handle;
    uint8_t  reason;
} __attribute__ ((packed)) bt_hci_evt_disc_cmpl_t;

typedef struct {
    uint8_t  status;
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_evt_auth_cmpl_t;

typedef struct {
    uint8_t  status;
    uint8_t  addr[6];
    uint8_t  name[248];
} __attribute__ ((packed)) bt_hci_evt_remote_name_req_cmpl_t;

typedef struct {
    uint8_t  status;
    uint16_t handle;
    uint8_t  enabled;
} __attribute__ ((packed)) bt_hci_evt_encr_change_t;

typedef struct {
    uint8_t  status;
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_evt_change_link_key_t;

typedef struct {
    uint8_t  status;
    uint16_t handle;
    uint8_t  features[8];
} __attribute__ ((packed)) bt_hci_evt_remote_sup_feat_cmpl_t;

typedef struct {
    uint8_t  status;
    uint16_t handle;
    uint8_t  page;
    uint8_t  max_page;
    uint8_t  features[8];
} __attribute__ ((packed)) bt_hci_evt_remote_ext_feat_cmpl_t;

typedef struct {
    uint8_t  status;
    uint16_t handle;
    uint8_t  version;
    uint16_t manufacturer;
    uint16_t subversion;
} __attribute__ ((packed)) bt_hci_evt_remote_version_cmpl_t;

typedef struct {
    uint8_t  nof_cmd;
    uint16_t opcode;
    uint8_t return_param[0];
} __attribute__ ((packed)) bt_hci_evt_cmd_cmpl_t;

typedef struct {
    uint8_t  status;
    uint8_t  nof_cmd;
    uint16_t opcode;
} __attribute__ ((packed)) bt_hci_evt_cmd_stat_t;

typedef struct {
    uint8_t  hardware_code;
} __attribute__ ((packed)) bt_hci_evt_hw_error_t;

typedef struct {
    uint8_t status;
    uint8_t addr[6];
    uint8_t role;
} __attribute__ ((packed)) bt_hci_evt_role_change_t;

typedef struct {
    uint8_t  nof_handles;
    uint16_t handle;
    uint16_t nof_packets;
} __attribute__ ((packed)) bt_hci_evt_num_cmpl_packets_t;

typedef struct {
    uint8_t addr[6];
} __attribute__ ((packed)) bt_hci_evt_pin_req_t;

typedef struct {
    uint8_t addr[6];
} __attribute__ ((packed)) bt_hci_evt_link_key_req_t;

typedef struct {
    uint8_t addr[6];
    uint8_t key[16];
    uint8_t type;
} __attribute__ ((packed)) bt_hci_evt_link_notif_t;

typedef struct {
    uint8_t  link_type;
} __attribute__ ((packed)) bt_hci_evt_data_buffer_overflow_t;

typedef struct {
    uint16_t handle;
    uint8_t max_slots;
} __attribute__ ((packed)) bt_hci_evt_max_slots_change_t;

typedef struct {
    uint8_t  status;
    uint16_t handle;
    uint16_t clock_offset;
} __attribute__ ((packed)) bt_hci_evt_clock_offset_cmpl_t;

typedef struct {
    uint8_t  status;
    uint16_t handle;
    uint16_t pkt_type;
} __attribute__ ((packed)) bt_hci_evt_conn_pkt_type_changed_t;

typedef struct {
    uint8_t address[6];
    uint8_t mode;
} __attribute__ ((packed)) bt_hci_evt_pscan_rep_mode_change_t;

typedef struct {
    uint8_t  status;
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_evt_encr_key_refresh_cmpl_t;

typedef struct {
    uint16_t handle;
} __attribute__ ((packed)) bt_hci_evt_auth_payl_to_expired_t;

typedef struct {
    uint8_t  subevent_code;
    uint8_t  status;
    uint16_t handle;
    uint8_t  role;
    uint8_t  addr_type;
    uint8_t  addr[6];
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
    uint8_t  accuracy;
} __attribute__ ((packed)) bt_hci_evt_le_conn_cmpl_t;

typedef struct {
    uint8_t subevent_code;
    uint8_t num_reports;
    struct {
        uint8_t event_type;
        uint8_t addr_type;
        uint8_t addr[6];
        uint8_t length;
        uint8_t data[0];
#if 0
        /* rssi follows data, but would make this structure not
         * expressable in C */
        uint8_t rssi;
#endif
    } __attribute__ ((packed));
} __attribute__ ((packed)) bt_hci_evt_le_adv_report_t;

typedef struct {
    uint8_t  subevent_code;
    uint8_t  status;
    uint16_t handle;
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
} __attribute__ ((packed)) bt_hci_evt_le_conn_upd_cmpl_t;

typedef struct {
    uint8_t  subevent_code;
    uint8_t  status;
    uint16_t handle;
    uint8_t  features[8];
} __attribute__ ((packed)) bt_hci_evt_le_remote_features_cmpl_t;

typedef struct {
    uint8_t  subevent_code;
    uint16_t handle;
    uint64_t rand;
    uint16_t ediv;
} __attribute__ ((packed)) bt_hci_evt_le_long_term_key_req_t;

typedef struct {
    uint8_t  subevent_code;
    uint16_t handle;
    uint16_t interval_min;
    uint16_t interval_max;
    uint16_t latency;
    uint16_t timeout;
} __attribute__ ((packed)) bt_hci_evt_le_remote_conn_param_req_t;

typedef struct {
    uint8_t  subevent_code;
    uint16_t handle;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
    uint16_t max_rx_octets;
    uint16_t max_rx_time;
} __attribute__ ((packed)) bt_hci_evt_le_data_length_change_t;

typedef struct {
    uint8_t subevent_code;
    uint8_t status;
    uint8_t key[64];
} __attribute__ ((packed)) bt_hci_evt_le_read_local_p256_public_key_complete_t;

typedef struct {
    uint8_t subevent_code;
    uint8_t status;
    uint8_t dhkey[32];
} __attribute__ ((packed)) bt_hci_evt_le_generate_dhkey_complete_t;

typedef struct {
    uint8_t  subevent_code;
    uint8_t  status;
    uint16_t handle;
    uint8_t  role;
    uint8_t  addr_type;
    uint8_t  addr[6];
    uint8_t  local_res_priv_addr[6];
    uint8_t  peer_res_priv_addr[6];
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
    uint8_t  accuracy;
} __attribute__ ((packed)) bt_hci_evt_le_enh_conn_cmpl_t;

typedef struct {
    uint8_t subevent_code;
    uint8_t num_reports;
    struct {
        uint8_t event_type;
        uint8_t addr_type;
        uint8_t addr[6];
        uint8_t direct_addr_type;
        uint8_t direct_addr[6];
        uint8_t rssi;
    } __attribute__ ((packed));
} __attribute__ ((packed)) bt_hci_evt_le_direct_adv_report_t;

typedef struct {
    uint8_t  subevent_code;
    uint8_t  status;
    uint16_t handle;
    uint8_t  tx_phy;
    uint8_t  rx_phy;
} __attribute__ ((packed)) bt_hci_evt_le_phy_upd_cmpl_t;

typedef struct {
    uint8_t subevent_code;
    uint8_t num_reports;
    struct {
        uint16_t event_type;
        uint8_t  addr_type;
        uint8_t  addr[6];
        uint8_t  pri_phy;
        uint8_t  sec_phy;
        uint8_t  sid;
        uint8_t  tx_power;
        uint8_t  rssi;
	uint16_t periodic_interval;
        uint8_t  direct_addr_type;
        uint8_t  direct_addr[6];
	uint8_t  length;
	uint8_t  data[0];
    } __attribute__ ((packed));
} __attribute__ ((packed)) bt_hci_evt_le_ext_adv_report_t;

typedef struct {
    uint8_t  subevent_code;
    uint8_t  status;
    uint16_t sync_handle;
    uint8_t  sid;
    uint8_t  addr_type;
    uint8_t  addr[6];
    uint8_t  phy;
    uint16_t interval;
    uint8_t  accuracy;
} __attribute__ ((packed)) bt_hci_evt_le_per_adv_sync_establ_t;

typedef struct {
    uint8_t  subevent_code;
    uint16_t sync_handle;
    uint8_t  tx_power;
    uint8_t  rssi;
    uint8_t  unused;
    uint8_t  status;
    uint8_t  length;
    uint8_t  data[0];
} __attribute__ ((packed)) bt_hci_evt_le_per_adv_report_t;

typedef struct {
    uint8_t  subevent_code;
    uint16_t sync_handle;
} __attribute__ ((packed)) bt_hci_evt_le_per_adv_sync_lost_t;

typedef struct {
    uint8_t  subevent_code;
} __attribute__ ((packed)) bt_hci_evt_le_scan_timeout_t;

typedef struct {
    uint8_t  subevent_code;
    uint8_t  status;
    uint8_t  adv_handle;
    uint16_t conn_handle;
    uint8_t  nof_adv_events;
} __attribute__ ((packed)) bt_hci_evt_le_adv_set_terminated_t;

typedef struct {
    uint8_t subevent_code;
    uint8_t adv_handle;
    uint8_t addr_type;
    uint8_t addr[6];
} __attribute__ ((packed)) bt_hci_evt_le_scan_req_received_t;

typedef struct {
    uint8_t  subevent_code;
    uint16_t conn_handle;
    uint8_t  algorithm;
} __attribute__ ((packed)) bt_hci_evt_le_chan_select_alorithm_t;

typedef uint8_t bt_address_t[6];

typedef enum {
    bt_hci_adv_type_ind           = 0x0,
    bt_hci_adv_type_direct_ind_hi = 0x1,
    bt_hci_adv_type_scan_ind      = 0x2,
    bt_hci_adv_type_nonconn_ind   = 0x3,
    bt_hci_adv_type_direct_ind_lo = 0x4
} bt_hci_adv_type_t;

#define BT_HCI_ADV_EV_PROP_CONNECTABLE_BIT   (1<<0)
#define BT_HCI_ADV_EV_PROP_SCANNABLE_BIT     (1<<1)
#define BT_HCI_ADV_EV_PROP_DIRECTED_BIT      (1<<2)
#define BT_HCI_ADV_EV_PROP_HIGH_DUTY_BIT     (1<<3)
#define BT_HCI_ADV_EV_PROP_USE_LEGACY_BIT    (1<<4)
#define BT_HCI_ADV_EV_PROP_OMIT_OWN_ADDR_BIT (1<<5)
#define BT_HCI_ADV_EV_PROP_INCL_TX_POWER_BIT (1<<6)

#define BT_HCI_ADV_EXT_EVT_CONN_BIT (1<<0)
#define BT_HCI_ADV_EXT_EVT_SCAN_BIT (1<<1)
#define BT_HCI_ADV_EXT_EVT_DIR_BIT  (1<<2)
#define BT_HCI_ADV_EXT_EVT_SRSP_BIT (1<<3)
#define BT_HCI_ADV_EXT_EVT_LEG_BIT  (1<<4)
#define BT_HCI_ADV_EXT_EVT_COMPLETE  (0<<5)
#define BT_HCI_ADV_EXT_EVT_MORE_DATA (1<<5)
#define BT_HCI_ADV_EXT_EVT_TRUNCATED (2<<5)

typedef enum {
    bt_hci_adv_ext_legacy_none_undir      = 0x10,
    bt_hci_adv_ext_legacy_scan_undir      = 0x12,
    bt_hci_adv_ext_legacy_scan_rsp        = 0x1a,
    bt_hci_adv_ext_legacy_conn_scan_undir = 0x13,
    bt_hci_adv_ext_legacy_conn_scan_rsp   = 0x1b,
    bt_hci_adv_ext_legacy_conn_dir        = 0x15,
    bt_hci_adv_ext_legacy_conn_dir_high   = 0x1d,
} bt_hci_adv_ext_legacy_t;

typedef enum {
    bt_hci_adv_filter_policy_scan_any_connect_any     = 0x0,
    bt_hci_adv_filter_policy_scan_white_connect_any   = 0x1,
    bt_hci_adv_filter_policy_scan_any_connect_white   = 0x2,
    bt_hci_adv_filter_policy_scan_white_connect_white = 0x3
} bt_hci_adv_filter_policy_t;
#define BT_HCI_ADV_FILTER_POLICY_SCAN_WHITE_BIT     (1<<0)
#define BT_HCI_ADV_FILTER_POLICY_CONNECT_WHITE_BIT  (1<<1)

typedef enum {
    /* Used for Own and Peer */
    bt_hci_addr_type_public            = 0x0,
    bt_hci_addr_type_random            = 0x1,
    bt_hci_addr_type_resolvable_public = 0x2,
    bt_hci_addr_type_resolvable_random = 0x3
} bt_hci_addr_type_t;
#define BT_HCI_ADDR_TYPE_BIT            (1<<0)
#define BT_HCI_ADDR_TYPE_RESOLVABLE_BIT (1<<1)

#define BT_HCI_ADV_CHANNEL_37_BIT 0
#define BT_HCI_ADV_CHANNEL_38_BIT 1
#define BT_HCI_ADV_CHANNEL_39_BIT 2
#define BT_HCI_ADV_CHANNEL_ALL    0x07

typedef enum {
    bt_hci_scan_type_passive = 0x0,
    bt_hci_scan_type_active  = 0x1
} bt_hci_scan_type_t;

typedef enum {
    bt_hci_scan_filter_policy_all        = 0x0, /* all, but not direct to other */
    bt_hci_scan_filter_policy_white      = 0x1, /* all in white list, but not direct to other */
    bt_hci_scan_filter_policy_all_plus   = 0x2, /* as "all" plus direct resolvable */
    bt_hci_scan_filter_policy_white_plus = 0x3  /* as "white" plus direct resolvable */
} bt_hci_scan_filter_policy_t;
#define BT_HCI_SCAN_FILTER_POLICY_WHITE_BIT      (1<<0)
#define BT_HCI_SCAN_FILTER_POLICY_RESOLVABLE_BIT (1<<1)

typedef enum {
    bt_hci_scan_filter_duplicates_disabled = 0x0,
    bt_hci_scan_filter_duplicates_enabled  = 0x1,
    bt_hci_scan_filter_duplicates_period   = 0x2,
} bt_hci_scan_filter_duplicates_t;

typedef enum {
    bt_hci_initiator_filter_policy_peer  = 0x0, /* use peer_address */
    bt_hci_initiator_filter_policy_white = 0x1, /* use white list */
} bt_hci_initiator_filter_policy_t;

#define BT_HCI_CONN_INTERVAL_MIN       12 /* slots */

/* Separate chunks for LE and Classic */
#define BT_HCI_LE_DATA_CHUNK_SIZE        251
#define BT_HCI_LE_DATA_CHUNK_NUM          14
#define BT_HCI_CL_ACL_CHUNK_SIZE         255
#define BT_HCI_CL_ACL_CHUNK_NUM            6
#define BT_HCI_CL_SCO_CHUNK_SIZE           0
#define BT_HCI_CL_SCO_CHUNK_NUM            0
#define BT_HCI_NUM_COMMAND_PACKETS_ZERO 0
#define BT_HCI_NUM_COMMAND_PACKETS_ONE  1

#define BT_HCI_DATA_PBF_START_H2C 0x0 // LE&CL - Host to Controller
#define BT_HCI_DATA_PBF_CONT      0x1 // LE&CL - both directions
#define BT_HCI_DATA_PBF_START_C2H 0x2 // LE    - Controller to Host
                                      // CL    - both directions
#define BT_HCI_DATA_PBF_COMPL     0x3 // LE&CL - not allowed

#define BT_HCI_DATA_BCF_POINT_TO_POINT  0x0
#define BT_HCI_DATA_BCF_ACTIVE_SLAVE    0x1

#define BT_AUTHENTICATED_PAYLOAD_TIMEOUT_DEFAULT 3000 /* in 10 ms = 30s (0x0BB8) */

#define BT_HCI_DATA_LENGTH_OCTETS_MAX 0xfb
#define BT_HCI_DATA_LENGTH_OCTETS_MIN 0x1b
#define BT_HCI_DATA_LENGTH_TIME_MAX 0x0848
#define BT_HCI_DATA_LENGTH_TIME_MIN 0x0148
#define BT_HCI_DATA_LENGTH_TEST_MAX   0xff

typedef enum {
    BT_HCI_LE_TM_PATTERN_PRBS9    = 0x0,
    BT_HCI_LE_TM_PATTERN_11110000 = 0x1,
    BT_HCI_LE_TM_PATTERN_10101010 = 0x2,
    BT_HCI_LE_TM_PATTERN_PRBS15   = 0x3,
    BT_HCI_LE_TM_PATTERN_11111111 = 0x4,
    BT_HCI_LE_TM_PATTERN_00000000 = 0x5,
} bt_hci_le_tm_pattern_t;

typedef enum {
    BT_HCI_EVENT_TYPE_ADV_IND          = 0x0,
    BT_HCI_EVENT_TYPE_ADV_DIRECT_IND   = 0x1,
    BT_HCI_EVENT_TYPE_ADV_NONCONN_IND  = 0x2,
    BT_HCI_EVENT_TYPE_SCAN_REQ         = 0x3,
    BT_HCI_EVENT_TYPE_SCAN_RSP         = 0x4,
    BT_HCI_EVENT_TYPE_CONNECT_IND      = 0x5,
    BT_HCI_EVENT_TYPE_SCAN_IND         = 0x6
} bt_hci_event_type_adv_t;

typedef enum {
    bt_hci_adv_ext_data_intermediate       = 0x0,
    bt_hci_adv_ext_data_first              = 0x1,
    bt_hci_adv_ext_data_last               = 0x2,
    bt_hci_adv_ext_data_complete           = 0x3,
    bt_hci_adv_ext_data_unchanged          = 0x4,
} bt_hci_adv_ext_data_operation_t;

typedef enum {
    bt_hci_adv_ext_data_frag_allowed    = 0x0,
    bt_hci_adv_ext_data_frag_disallowed = 0x1,
} bt_hci_adv_ext_data_frag_t;

typedef enum {
    bt_hci_per_adv_data_complete   = 0x0,
    bt_hci_per_adv_data_incomplete = 0x1,
    bt_hci_per_adv_data_truncated  = 0x2,
} bt_hci_per_adv_data_status_t;

typedef enum {
    bt_hci_privacy_mode_network = 0x0,
    bt_hci_privacy_mode_device  = 0x1,
} bt_hci_privacy_mode_t;

#define BT_HCI_LE_PHY_ALL_TX_BIT   (1<<0)
#define BT_HCI_LE_PHY_ALL_RX_BIT   (1<<1)
#define BT_HCI_LE_PHY_1M_BIT       (1<<0)
#define BT_HCI_LE_PHY_2M_BIT       (1<<1)
#define BT_HCI_LE_PHY_CODED_BIT    (1<<2)
#define BT_HCI_LE_PHY_ALL          (BT_HCI_LE_PHY_1M_BIT | BT_HCI_LE_PHY_2M_BIT | BT_HCI_LE_PHY_CODED_BIT)
#define BT_HCI_LE_PHY_OPTIONS_NP   0 /* no preference */
#define BT_HCI_LE_PHY_OPTIONS_S2   1
#define BT_HCI_LE_PHY_OPTIONS_S8   2

#define BT_HCI_LE_PHY_1M    1
#define BT_HCI_LE_PHY_2M    2
#define BT_HCI_LE_PHY_CODED 3

#define BT_HCI_LE_TX_POWER_NO_PREFERENCE 0x7F

#define BT_HCI_SCAN_I_BIT (1<<0)
#define BT_HCI_SCAN_P_BIT (1<<1)
#define BT_HCI_SCAN_INTERVAL_DEFAULT 0x0800
#define BT_HCI_SCAN_WINDOW_DEFAULT 0x0012 /* 0x0010 should be enough */

#define BT_HCI_LINK_TYPE_SCO 0
#define BT_HCI_LINK_TYPE_ACL 1
/* HCI API
 * See BT spec. Vol 4, Part A, chapter 2 - UART transport layer (indicator)
 * See BT spec. Vol 2, Part E, chapter 5.4. */
#define BT_HCI_PACKET_INDICATOR_CMD  0x1
#define BT_HCI_PACKET_INDICATOR_DATA 0x2
#define BT_HCI_PACKET_INDICATOR_EVT  0x4

#define BT_HCI_PAGE_TIMEOUT_DEFAULT              0x2000 // slots
#define BT_HCI_CONNECTION_ACCEPT_TIMEOUT_DEFAULT 0x1FA0 // slots

#define BT_HCI_PKT_TYPE_2DH1_NOT 0x0002
#define BT_HCI_PKT_TYPE_3DH1_NOT 0x0004
#define BT_HCI_PKT_TYPE_DM1      0x0008
#define BT_HCI_PKT_TYPE_DH1      0x0010
#define BT_HCI_PKT_TYPE_2DH3_NOT 0x0100
#define BT_HCI_PKT_TYPE_3DH3_NOT 0x0200
#define BT_HCI_PKT_TYPE_DM3      0x0400
#define BT_HCI_PKT_TYPE_DH3      0x0800
#define BT_HCI_PKT_TYPE_2DH5_NOT 0x1000
#define BT_HCI_PKT_TYPE_3DH5_NOT 0x2000
#define BT_HCI_PKT_TYPE_DM5      0x4000
#define BT_HCI_PKT_TYPE_DH5      0x8000
#define BT_HCI_PKT_TYPE_BR_ALL   0xcc18
#define BT_HCI_PKT_TYPE_EDR_NOT  0x3306

typedef enum {
    bt_hci_filter_type_clear           = 0x0,
    bt_hci_filter_type_inquiry         = 0x1,
    bt_hci_filter_type_connection      = 0x2,
} bt_hci_filter_type_t;

typedef enum {
    bt_hci_condition_type_all          = 0x0,
    bt_hci_condition_type_cod          = 0x1,
    bt_hci_condition_type_addr         = 0x2,
} bt_hci_condition_type_t;

typedef enum {
    bt_hci_auto_accept_off             = 0x1,
    bt_hci_auto_accept_on              = 0x2,
    bt_hci_auto_accept_on_switch       = 0x3,
} bt_hci_auto_accept_flag_t;

typedef enum {
    bt_hci_inq_mode_default            = 0x0,
    bt_hci_inq_mode_rssi               = 0x1,
    bt_hci_inq_mode_extended           = 0x2,
} bt_hci_inq_mode_t;

typedef enum {
    bt_hci_key_combination             = 0x0,
    bt_hci_key_local_unit              = 0x1,
    bt_hci_key_remote_unit             = 0x2,
    bt_hci_key_combination_debug       = 0x3,
    bt_hci_key_combination_unauth_192  = 0x4,
    bt_hci_key_combination_auth_192    = 0x5,
    bt_hci_key_combination_changed     = 0x6,
    bt_hci_key_combination_unauth_256  = 0x7,
    bt_hci_key_combination_auth_256    = 0x8,
} bt_hci_key_type_t;

#define BT_HCI_POLICY_SWITCH 0x0001

/**
 * HCI command
 */
typedef struct bt_hci_command_t {
    /** Indicator (=0x1 Command) */
    uint8_t indicator;
    /** Opcode - OCF (bits 0..11) and OGF (bits 12..15) */
    uint16_t opcode;
    /** Length of parameter */
    uint8_t length;
    /** Parameters */
    uint8_t parameter[255];
} __attribute__ ((packed)) bt_hci_command_t;

/**
 * HCI event
 */
typedef struct bt_hci_event_t {
    /** Indicator (=0x4 Event) */
    uint8_t indicator;
    /** Event code */
    uint8_t code;
    /** Length of parameter */
    uint8_t length;
    /** Parameters */
    uint8_t parameter[255];
} __attribute__ ((packed)) bt_hci_event_t;
#define sizeof_hci_event_hdr (1+1+1)

/**
 * HCI data
 */
typedef struct bt_hci_data_t {
    /** Indicator (=0x2 Data) */
    uint8_t indicator;
    /** Connection handle (bits 0..11), PB flags (bits 12..13), BC flags (bits 14..15)  */
    uint16_t handle;
    /** Length of data */
    uint16_t length;
    /** Data */
    uint8_t data[0];
} __attribute__ ((packed)) bt_hci_data_t;

static inline uint8_t bt_hci_handle_hdl(uint16_t handle) {return (handle & 0xfff);}
static inline uint8_t bt_hci_handle_pbf(uint16_t handle) {return ((handle>>12) & 0x3);}
static inline uint8_t bt_hci_handle_bcf(uint16_t handle) {return (handle>>14);}
static inline uint16_t bt_hci_handle_set(uint16_t handle, uint8_t pbf, uint8_t bcf) {return (handle | (pbf<<12) | (bcf<<14));}
static inline uint8_t bt_hci_pkt_type_edr(uint16_t pkt) {return ((((pkt) & BT_HCI_PKT_TYPE_EDR_NOT) ^ BT_HCI_PKT_TYPE_EDR_NOT) != 0);}

struct os_workqueue *bt_hci_workqueue(void);

/**
 * HCI event callback
 */
typedef void (*bt_hci_event_cb_t)(struct pfrag *evt);
void hci_event_cb(struct pfrag *cmd);

/**
 * HCI data callback
 */
typedef void (*bt_hci_data_cb_t)(struct pfrag *data);
void hci_data_cb(struct pfrag *cmd);

void bt_hci_start(bt_hci_event_cb_t event_cb, bt_hci_data_cb_t data_cb);
void bt_hci_stop(void);
void bt_hci_command(struct pfrag *cmd);
void bt_hci_data_tx(struct pfrag *data);

/**
 * BT debug API
 */
#define BT_DBG_TX     0
#define BT_DBG_RX     1
#define BT_DBG_TX_ACK 2
void dbg_hci_command(bt_hci_command_t *hci_command);
void dbg_hci_event(bt_hci_event_t *hci_event);
void dbg_hci_data(uint8_t direction, struct pfrag *pf);

/**
 * BT xmit_tone
 */
struct bt_txgen {
    struct rf_txpower *txpower;
    uint16_t freq;
    bool running;
};
void __entrypoint cop_bt_xmit_tone(const struct bt_txgen *param, const uint32_t dpa_power);

/** @} */
