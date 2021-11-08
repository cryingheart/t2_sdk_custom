#pragma once
#include <stdint.h>

#define HW_TESTBED_INTERFACE_UART  0
#define HW_TESTBED_INTERFACE_SPI   1
#define HW_TESTBED_INTERFACE_WIFI  2
#define HW_TESTBED_INTERFACE_BLUE  3
#define HW_TESTBED_INTERFACE_SDIO  4
#define HW_TESTBED_INTERFACE_WIFI_2 5
#define HW_TESTBED_INTERFACE_SERDES 6
#define HW_TESTBED_INTERFACE_POLAR 7
#define HW_TESTBED_INTERFACE_PWM   8

struct hw_testbed {
    struct hw_testbed_ctrl {
        uint32_t                   exit_now;
        uint8_t                    log;
        uint8_t                    log_full;
        uint32_t                   tc_enable;
        uint32_t                   tc_current;
        uint32_t                   gpio_dir;
        uint32_t                   gpio_out;
        uint32_t                   gpio_in;
        uint32_t                   gpio_delay;
        uint8_t                    adc_interpolation;
    } ctrl;
    struct hw_testbed_interface {
        uint8_t                    mode;
#define TESTBED_MODE_NORMAL        0x0
#define TESTBED_MODE_RECORD        0x1
#define TESTBED_MODE_PLAYBACK      0x2
#define TESTBED_MODE_SINK          0x3
        uint8_t                    file_id;
        uint16_t                   reserved;
        uint8_t                    busy;
    } interface[9];
    struct hw_testbed_agc {
        uint8_t                    pga;
    } agc;
    struct hw_testbed_sdio {
        uint8_t                    enable;
        uint8_t                    master;
        uint8_t                    cmd_rsp;
        uint8_t                    data_en;
        uint8_t                    data_dir;
        uint8_t                    data_mode;
        uint16_t                   num_bytes;
        uint8_t                    cmd_send_head;
        uint32_t                   cmd_send_arg;
        uint32_t                   cmd_rsp_lo;
        uint16_t                   cmd_rsp_hi;
        uint8_t                    busy;
    } sdio;
    uint8_t                        connect_i2c;
    uint8_t                        connect_flash;
    uint8_t                        polar;
    uint8_t                        use_am_am;
    uint8_t                        tdc_enable_noise;
    struct hw_testbed_jtag {
        uint8_t                    connect;
        uint8_t                    command;
#define TESTBED_COMMAND_READ_ID    0x01
#define TESTBED_COMMAND_ENABLE_TAP 0x02
#define TESTBED_COMMAND_POWERUP    0x03
#define TESTBED_COMMAND_HALT       0x04
#define TESTBED_COMMAND_POLL       0x05
#define TESTBED_COMMAND_POLL_AND_WAKEUP 0x06
#define TESTBED_COMMAND_RESET      0x07
#define TESTBED_COMMAND_MBIST      0x08
#define TESTBED_COMMAND_HANDLE_BKPT 0x09
        uint32_t                   arg[4];
        uint32_t                   result[4];
    } jtag;
    struct hw_testbed_spi {
        uint8_t                    connect;
        uint8_t                    cs;
        uint8_t                    rx_enable;
        uint8_t                    tx_full;
        uint8_t                    rx_empty;
        uint8_t                    tx_fifo;
        uint8_t                    rx_fifo;
    } spi;
    uint8_t                        state;
    struct hw_testbed_pdm {
        uint8_t                    connect;
    } pdm;
    struct hw_testbed_hyperbus {
        uint8_t                    connect;
        uint8_t                    use_flash;
    } hyperbus;
};
