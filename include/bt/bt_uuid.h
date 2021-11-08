#pragma once
#include <stdint.h>

typedef struct {
    uint8_t data[16];
} uint128_t;

typedef struct {
#define SZ_UUID16     2
#define SZ_UUID32     4
#define SZ_UUID128    16
    uint8_t size;
    union {
        uint16_t  u16;
        uint32_t  u32;
        uint128_t u128;
    } value;
} bt_uuid_t;

/* GATT Services */
#define UUID_GATT_S_GENERIC_ACCESS            0x1800
#define UUID_GATT_S_GENERIC_ATTRIBUTE         0x1801
#define UUID_GATT_S_DEVICE_INFORMATION        0x180A
#define UUID_GATT_S_HEART_RATE                0x180D
#define UUID_GATT_S_BATTERY                   0x180F
/* GATT Declarations */
#define UUID_GATT_D_PRIMARY_SERVICE           0x2800
#define UUID_GATT_D_SECONDARY_SERVICE         0x2801
#define UUID_GATT_D_INCLUDE                   0x2802
#define UUID_GATT_D_CHARACTERISTIC            0x2803
/* GATT Characteristic Descriptors */
#define UUID_GATT_CD_EXTENDED_PROPERERTIES    0x2900
#define UUID_GATT_CD_USER_DESCRIPTION         0x2901
#define UUID_GATT_CD_CLIENT_CONFIGURATION     0x2902
#define UUID_GATT_CD_SERVER_CONFIGURATION     0x2903
#define UUID_GATT_CD_FORMAT                   0x2904
#define UUID_GATT_CD_AGGREGATE_FORMAT         0x2905
/* GATT Characteristic Types */
#define UUID_GATT_CT_DEVICE_NAME              0x2A00
#define UUID_GATT_CT_APPEARANCE               0x2A01
#define UUID_GATT_CT_PERIPHERAL_PRIV_FLAG     0x2A02
#define UUID_GATT_CT_RECONNECTION_ADDRESS     0x2A03
#define UUID_GATT_CT_PERIPHERAL_PREF_CONN     0x2A04
#define UUID_GATT_CT_SERVICE_CHANGED          0x2A05
#define UUID_GATT_CT_TX_POWER_LEVEL           0x2A07
#define UUID_GATT_CT_BATTERY_LEVEL            0x2A19
#define UUID_GATT_CT_MANUFACTURER_NAME        0x2A29
#define UUID_GATT_CT_HEART_RATE_MEASUREMENT   0x2A37
#define UUID_GATT_CT_BODY_SENSOR_LOCATION     0x2A38
#define UUID_GATT_CT_HEART_RATE_CONTROL_POINT 0x2A39
#define UUID_GATT_CT_CENTRAL_ADDR_RESOLUTION  0x2AA6


int bt_uuid_cmp(const bt_uuid_t *uuid1, const bt_uuid_t *uuid2);
void bt_uuid_convert_att(const bt_uuid_t *src, bt_uuid_t *dst);
void bt_uuid16_create(bt_uuid_t *uuid, uint16_t value);
void bt_uuid32_create(bt_uuid_t *uuid, uint32_t value);
void bt_uuid128_create(bt_uuid_t *uuid, uint128_t value);
void bt_uuid_str_to_uuid128(char *uuid, uint128_t *uuid_128);
void bt_uuid_print(bt_uuid_t *uuid);
