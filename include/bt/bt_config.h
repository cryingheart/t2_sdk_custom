#pragma once
#include <stdint.h>

/* Configuration of BT host */
#define BT_CFG_NOF_CONNECTIONS 4 // links in connection state (central or peripheral)
static const uint8_t BT_CFG_SMP_IR[16] = {
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
static const uint8_t BT_CFG_SMP_ER[16] = {
    0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,
    0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef};

#define BT_CFG_NOF_BONDS 5 // number of possible bonds stored in persistent memory

/* Configuration of BT controller */
#define BT_CFG_LPO_ACCURACY 0 /* sleep clock accuracy (0: 251-500, 1: 151-250, 2: 101-150,
                               * 3: 76-100, 4: 51-75, 5: 31-50, 6: 21-30, 7: 0-20) */
#define BT_CFG_LM_HANDLES 3 // links in PSCAN, ISCAN, PAGE, INQUIRY, CONNECTED (master or slave)

#define BT_CFG_LE_WHITE_LIST_SIZE BT_CFG_NOF_BONDS
#define BT_CFG_LE_RESOLVING_LIST_SIZE BT_CFG_NOF_BONDS
#define BT_CFG_LE_UNRESOLVED_LIST_SIZE 20 // could be a high value!
#define BT_CFG_LE_PERIODIC_ADVERTISER_LIST_SIZE 3 /* HCI/CCO/BV-17-C */

#define BT_CFG_LE_ADVERTISING_FRAGS_MAX 1 /* LL/DDI/ADV/BV-29..32-C */
#define BT_CFG_LE_ADVERTISING_FRAG_SIZE_MAX 245
#define BT_CFG_LE_ADVERTISING_SETS 3

#define BT_CFG_LE_CONN_LINKS BT_CFG_NOF_CONNECTIONS
#define BT_CFG_LE_SYNC_LINKS 1 /* HCI/DDI/BI-04-C requires 2. */

/* Version information */
#define BT_HCI_VERSION 9 /* 9=5.0, 10=5.1 */
#define BT_LL_VERSION BT_HCI_VERSION
#define BT_HCI_FW_VERSION ((/*year*/19<<8) + (/*week*/49<<0))
#define BT_LL_FW_VERSION BT_HCI_FW_VERSION
#define BT_MANUFACTURER 0x0873 /* Innophase Incorporated */

static const uint8_t bt_cod[3] = {0x14/*Minor:loudspeaker*/, 0x04/*Major:audio/video*/, 0x20/*Class:audio*/};
