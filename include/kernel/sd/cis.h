#pragma once
#include <stdint.h>
#include <kernel/compiler.h>

#define CIS_AREA_START 0x001000
#define CIS_AREA_END   0x017fff

#define CIS_ADDRESS_FN_MASK     0x007000
#define CIS_ADDRESS_OFFSET_MASK 0x000fff

#define CISTPL_VERS_1   0x15
#define CISTPL_MANFID   0x20
#define CISTPL_FUNCID   0x21
#define CISTPL_FUNCE    0x22
#define CISTPL_SDIO_STD 0x91
#define CISTPL_END      0xff

/* "standard" interface ids */
#define SDIO_UART       1
#define SDIO_BT_A       2
#define SDIO_BT_B       3
#define SDIO_GPS        4
#define SDIO_WLAN       7

/* "standard" interface types */
#define GPS_SIOREG      0

struct __packed cistpl_vers_1 {
    uint8_t code;
    uint8_t link;
    uint8_t major;
    uint8_t minor;
    /* variable length info follow */
};

struct __packed cistpl_manfid {
    uint8_t code;
    uint8_t link;
    uint16_t manf;
    uint16_t card;
};

struct __packed cistpl_funce_00 {
    uint8_t code;
    uint8_t link;
    uint8_t type;
    uint16_t fn0_blk_size;
    uint8_t max_tran_speed;
    /* more fields follow 3.0 */
};

struct __packed cistpl_funcid {
    uint8_t code;
    uint8_t link;
    uint8_t function;
    uint8_t sysinit;
};

struct __packed cistpl_funce_01 {
    uint8_t code;
    uint8_t link;
    uint8_t type;
    uint8_t function_info;
    uint8_t std_io_rev;
    uint32_t card_psn;
    uint32_t csa_size;
    uint8_t csa_property;
    uint16_t max_blk_size;
    uint32_t ocr;
    uint8_t op_min_pwr;
    uint8_t op_avg_pwr;
    uint8_t op_max_pwr;
    uint8_t sb_min_pwr;
    uint8_t sb_avg_pwr;
    uint8_t sb_max_pwr;
    uint16_t min_bw;
    uint16_t opt_bw;
    uint16_t enable_timeout_val;
    uint16_t sp_avg_pwr_33;
    uint16_t sp_max_pwr_33;
    uint16_t hp_avg_pwr_33;
    uint16_t hp_max_pwr_33;
    uint16_t lp_avg_pwr_33;
    uint16_t lp_max_pwr_33;
};

struct __packed cistpl_gps {
    uint8_t code;
    uint8_t link;
    uint8_t std_id;
    uint8_t std_type;
    uint8_t regid;
    uint8_t regeid;
    uint8_t regofs[3];
    uint8_t inf_maxrate;
    uint8_t inf_dll;
    uint8_t inf_dlm;
};
