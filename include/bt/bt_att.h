#pragma once
#include <stdint.h>

// Vol 3, Part F: Attribute Protocol (ATT)
// Section 3.4.1.1 Error codes
typedef enum {
    BT_ATT_ERROR_SUCCESS                          = 0x00,
    BT_ATT_ERROR_INVALID_HANDLE                   = 0x01,
    BT_ATT_ERROR_READ_NOT_PERMITTED               = 0x02,
    BT_ATT_ERROR_WRITE_NOT_PERMITTED              = 0x03,
    BT_ATT_ERROR_INVALID_PDU                      = 0x04,
    BT_ATT_ERROR_INSUFFICIENT_AUTHENTICATION      = 0x05,
    BT_ATT_ERROR_REQ_NOT_SUPPORTED                = 0x06,
    BT_ATT_ERROR_INVALID_OFFSET                   = 0x07,
    BT_ATT_ERROR_INSUFFICIENT_AUTHORIZATION       = 0x08,
    BT_ATT_ERROR_PREPARE_QUEUE_FULL               = 0x09,
    BT_ATT_ERROR_ATTRIBUTE_NOT_FOUND              = 0x0A,
    BT_ATT_ERROR_ATTRIBUTE_NOT_LONG               = 0x0B,
    BT_ATT_ERROR_INSUFFICIENT_ENCRYPTION_KEY_SIZE = 0x0C,
    BT_ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH   = 0x0D,
    BT_ATT_ERROR_UNLIKELY_ERROR                   = 0x0E,
    BT_ATT_ERROR_INSUFFICIENT_ENCRYPTION          = 0x0F,
    BT_ATT_ERROR_UNSUPPORTED_GROUP_TYPE           = 0x10,
    BT_ATT_ERROR_INSUFFICIENT_RESOURCE            = 0x11,
    BT_ATT_ERROR_INDICATION_BUSY                  = 0x12,
    /* 0x13-0x7F - Reserved */
    BT_ATT_ERROR_APPLICATION_ERROR                = 0x80,
    /* 0x80-0x9F - Application error */
    /* 0xA0-0xDF - Reserved */
    /* 0xE0-0xFF - Common Profile and Service Error Codes */
} bt_att_error_t;
