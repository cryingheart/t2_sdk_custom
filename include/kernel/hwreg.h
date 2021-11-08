#pragma once
#include <kernel/compiler.h>
#include <hwreg/version.h>
#include <hwreg/event_handler.h>
#include <hwreg/timer.h>
#include <hwreg/afe.h>
#include <hwreg/backoff.h>
#include <hwreg/gpio.h>
#include <hwreg/rnd.h>
#include <hwreg/uart.h>
#include <hwreg/frequency.h>
#include <hwreg/watchdog.h>
#include <hwreg/testbed.h>
#include <hwreg/cipher.h>
#include <hwreg/clone.h>
#include <hwreg/rst_clk.h>
#include <hwreg/wifi_rx_ofdm.h>
#include <hwreg/wifi_rx_11b.h>
#include <hwreg/mac_interface.h>
#include <hwreg/frame_info.h>
#include <hwreg/tx_bb.h>
#include <hwreg/frontend.h>
#include <hwreg/frontend_rx.h>
#include <hwreg/mmu.h>
#include <hwreg/pmu.h>
#include <hwreg/wfe_handler.h>
#include <hwreg/support.h>
#include <hwreg/txrx_dma.h>
#include <hwreg/fpga.h>
#include <hwreg/tx_dummy.h>
#include <hwreg/qspi.h>
#include <hwreg/ble.h>
#include <hwreg/i2c.h>
#include <hwreg/spi_slv.h>
#include <hwreg/sdio.h>
#include <hwreg/pwm.h>
#include <hwreg/pdm.h>
#include <hwreg/core.h>
#include <hwreg/dpll.h>
//#include <hwreg/afe.h>
#include <hwreg/tap.h>
#include <hwreg/rxtdc.h>

#define PERIPH_BASE           0x00fc0000
#define PSTREAM_BASE          0x00fd0000
#define PERIPH_VERSION        (PERIPH_BASE | 0x0000)
#define PERIPH_PMU            (PERIPH_BASE | 0x0100)
#define PERIPH_WFE            (PERIPH_BASE | 0x0200)
#define PERIPH_EVH            (PERIPH_BASE | 0x0300)
#define PERIPH_TIMER          (PERIPH_BASE | 0x0400)
#define PERIPH_BOFF           (PERIPH_BASE | 0x0500)
#define PERIPH_GPIO           (PERIPH_BASE | 0x0600)
#define PERIPH_RND            (PERIPH_BASE | 0x0700)
#define PERIPH_QSPI           (PERIPH_BASE | 0x0800)
#define PERIPH_UART           (PERIPH_BASE | 0x0900)
#define PERIPH_FREQ           (PERIPH_BASE | 0x0a00)
#define PERIPH_WDG            (PERIPH_BASE | 0x0b00)
#define PERIPH_TESTBED        (PERIPH_BASE | 0x0c00)
#define PERIPH_SUPPORT        (PERIPH_BASE | 0x0d00)
#define PERIPH_TXRXDMA        (PERIPH_BASE | 0x0e00)
#define PERIPH_SPISLV         (PERIPH_BASE | 0x0f00)
#define PERIPH_SDIO           (PERIPH_BASE | 0x1000)
#define PERIPH_CIPHER         (PERIPH_BASE | 0x1100)
#define PERIPH_CLONE_WIFI     (PERIPH_BASE | 0x1200)
#define PERIPH_CLONE_BT       (PERIPH_BASE | 0x1300)
#define PERIPH_CLONE_SDIO     (PERIPH_BASE | 0x1400)
#define PERIPH_I2C            (PERIPH_BASE | 0x1500)
#define PERIPH_RXTDC          (PERIPH_BASE | 0x1600)
#define PERIPH_PWM            (PERIPH_BASE | 0x1700)
#define PERIPH_MMU            (PERIPH_BASE | 0x1800)
#define PERIPH_AFE            (PERIPH_BASE | 0x1a00)
#define PERIPH_CORE           (PERIPH_BASE | 0x1c00)
#define PERIPH_PDM            (PERIPH_BASE | 0x1d00)
#define PERIPH_RSTCLK         (PERIPH_BASE | 0x2000)
#define PERIPH_WIFI_RX_OFDM   (PERIPH_BASE | 0x2100)
#define PERIPH_MAC_INTERFACE  (PERIPH_BASE | 0x2200)
#define PERIPH_FRAME_INFO     (PERIPH_BASE | 0x2300)
#define PERIPH_TXBB           (PERIPH_BASE | 0x2400)
#define PERIPH_BLE            (PERIPH_BASE | 0x2500)
#define PERIPH_WIFI_RX_11B    (PERIPH_BASE | 0x2600)
#define PERIPH_FRONTEND_RX    (PERIPH_BASE | 0x2a00)
#define PERIPH_FRONTEND       (PERIPH_BASE | 0x2d00)
#define PERIPH_DPLL           (PSTREAM_BASE| 0x0000)
#define PERIPH_FPGA           (PERIPH_BASE | 0x4000)
#define PERIPH_TX_DUMMY       (PERIPH_BASE | 0x4100)
#define PERIPH_TAP            (PERIPH_BASE | 0x4400)

#ifndef HW_REG
#define HW_REG(type, name, address) \
    static volatile __unused type *name = (volatile type*)(address)
#endif

/* MAC */
HW_REG(struct hw_version,       hw_ver,       PERIPH_VERSION);
HW_REG(struct hw_pmu,           hw_pmu,       PERIPH_PMU);
HW_REG(struct hw_wfe_handler,   hw_wfe,       PERIPH_WFE);
HW_REG(struct hw_event_handler, hw_evh,       PERIPH_EVH);
HW_REG(struct hw_timer,         hw_timer,     PERIPH_TIMER);
HW_REG(struct hw_backoff,       hw_boff,      PERIPH_BOFF);
HW_REG(struct hw_gpio,          hw_gpio,      PERIPH_GPIO);
HW_REG(struct hw_rnd,           hw_rnd,       PERIPH_RND);
HW_REG(struct hw_qspi,          hw_qspi,      PERIPH_QSPI);
HW_REG(struct hw_uart,          hw_uart,      PERIPH_UART);
HW_REG(struct hw_tap,           hw_tap,       PERIPH_TAP);
HW_REG(struct hw_frequency,     hw_freq,      PERIPH_FREQ);
HW_REG(struct hw_wdg,           hw_wdg,       PERIPH_WDG);
HW_REG(struct hw_testbed,       hw_tb,        PERIPH_TESTBED);
HW_REG(struct hw_support,       hw_sup,       PERIPH_SUPPORT);
HW_REG(struct hw_txrx_dma,      hw_trxdma,    PERIPH_TXRXDMA);
HW_REG(struct hw_spi_slv,       hw_sspi,      PERIPH_SPISLV);
HW_REG(struct hw_sdio,          hw_sdio,      PERIPH_SDIO);
HW_REG(struct hw_cipher,        hw_cipher,    PERIPH_CIPHER);
HW_REG(struct hw_clone,         hw_clone,     PERIPH_CLONE_WIFI);
HW_REG(struct hw_clone,         hw_clone_bt,  PERIPH_CLONE_BT);
HW_REG(struct hw_clone,         hw_clone_sdio,PERIPH_CLONE_SDIO);
HW_REG(struct hw_i2c,           hw_i2c,       PERIPH_I2C);
HW_REG(struct hw_dpll,          hw_dpll,      PERIPH_DPLL);
HW_REG(struct hw_pwm,           hw_pwm,       PERIPH_PWM);
HW_REG(struct hw_pdm,           hw_pdm,       PERIPH_PDM);
HW_REG(struct hw_mmu,           hw_mmu,       PERIPH_MMU);
HW_REG(struct hw_afe,           hw_afe,       PERIPH_AFE);
HW_REG(struct hw_core,          hw_core,      PERIPH_CORE);
HW_REG(struct hw_rxtdc,         hw_rxtdc,     PERIPH_RXTDC);
/* WIFI-PHY */
HW_REG(struct hw_rst_clk,       hw_rstclk,      PERIPH_RSTCLK);
HW_REG(struct hw_wifi_rx_ofdm,  hw_bbrx_ofdm,   PERIPH_WIFI_RX_OFDM);
HW_REG(struct hw_mac_interface, hw_macif,       PERIPH_MAC_INTERFACE);
HW_REG(struct hw_frame_info,    hw_frame,       PERIPH_FRAME_INFO);
HW_REG(struct hw_tx_bb,         hw_txbb,        PERIPH_TXBB);
HW_REG(struct hw_ble,           hw_ble,         PERIPH_BLE);
HW_REG(struct hw_wifi_rx_11b,   hw_bbrx_11b,    PERIPH_WIFI_RX_11B);
HW_REG(struct hw_frontend_rx,   hw_frontend_rx, PERIPH_FRONTEND_RX);
HW_REG(struct hw_frontend,      hw_frontend,    PERIPH_FRONTEND);
HW_REG(struct hw_fpga,          hw_fpga,        PERIPH_FPGA);
HW_REG(struct hw_tx_dummy,      hw_tx_dummy,    PERIPH_TX_DUMMY);
