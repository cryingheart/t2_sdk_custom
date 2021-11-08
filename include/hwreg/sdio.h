#pragma once
#include <stdint.h>

struct hw_sdio {
    struct hw_sdio_cmd {
        uint16_t                   cntr;
        uint32_t                   arg;
        uint8_t                    cmd;
        uint8_t                    crc;
        uint8_t                    len;
        uint8_t                    en;
        uint8_t                    auto_rx;
        uint8_t                    evt_pos;
        uint8_t                    evt_pos_en;
#define SDIO_EVT_POS_EN_RX         0x01
#define SDIO_EVT_POS_EN_TX         0x02
    } cmd;
    struct hw_sdio_rsp {
        uint32_t                   arg;
        uint8_t                    cmd;
        uint8_t                    crc;
        uint8_t                    en;
    } rsp;
    struct hw_sdio_data {
        uint8_t                    start;
#define SDIO_START_RX              0x1
#define SDIO_START_TX              0x2
        uint16_t                   cnt;
        uint16_t                   blks;
        uint8_t                    mode;
#define SDIO_MODE_FOUR_WIRE_MODE   0x01
#define SDIO_MODE_RX_CRC           0x02
#define SDIO_MODE_TX_CRC           0x04
#define SDIO_MODE_RX_STATUS        0x08
#define SDIO_MODE_TX_STATUS        0x10
#define SDIO_MODE_READ_WAIT_EN     0x20
        uint8_t                    busy_wait;
    } data;
    struct hw_sdio_ctrl {
        struct hw_sdio_ctrl_clk {
            uint8_t                enable;
            uint8_t                run;
            uint8_t                state;
            uint8_t                pwidth;
            uint8_t                nwidth;
        } clk;
        uint8_t                    abort;
    } ctrl;
    struct hw_sdio_status {
        uint8_t                    cmd_state;
        uint8_t                    data_state;
        uint8_t                    bus_state;
    } status;
};
