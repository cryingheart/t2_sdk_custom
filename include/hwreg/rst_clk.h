#pragma once
#include <stdint.h>

#define HW_RST_CLK_RC_OFDM_RX_TOP  0
#define HW_RST_CLK_RC_OFDM_RX_CIRC 1
#define HW_RST_CLK_RC_OFDM_RX_FRONTEND 2
#define HW_RST_CLK_RC_OFDM_RX_BACKEND 3
#define HW_RST_CLK_RC_OFDM_RX_T2F  4
#define HW_RST_CLK_RC_MAC_INTERFACE 5
#define HW_RST_CLK_RC_WIFI_TX      6
#define HW_RST_CLK_RC_BLE          7
#define HW_RST_CLK_RC_WIFI_RX_11B  8
#define HW_RST_CLK_RC_FRONTEND_TX  9
#define HW_RST_CLK_RC_FRONTEND_RX  10
#define HW_RST_CLK_RC_SDL          11
#define HW_RST_CLK_NO_CLK          12
#define HW_RST_CLK_R_FIFO          12
#define HW_RST_CLK_NO_RST          13

struct hw_rst_clk {
    struct hw_rst_clk_rstclk {
        uint32_t                   set;
        uint32_t                   clr;
    } rstclk;
};
