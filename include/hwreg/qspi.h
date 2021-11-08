#pragma once
#include <stdint.h>
#include <hwreg/dma.h>

struct hw_qspi {
    struct hw_dma_ctrl             dma_ctrl;
    uint8_t                        cmd[8];
    uint8_t                        start;
    uint16_t                       data_size;
    uint8_t                        cs_sel;
    uint8_t                        busy;
    uint32_t                       last_shifted;
    struct hw_qspi_setup {
        struct hw_qspi_setup_cfg {
            uint8_t                mode;
#define QSPI_MODE_WIDTH_MASK       0x03
#define QSPI_WIDTH_WIDTH_1         0x00
#define QSPI_WIDTH_WIDTH_2         0x01
#define QSPI_WIDTH_WIDTH_4         0x02
#define QSPI_WIDTH_WIDTH_8         0x03
#define QSPI_MODE_DDR              0x04
#define QSPI_MODE_DIRECTION_MASK   0x18
#define QSPI_DIRECTION_OUT         0x00
#define QSPI_DIRECTION_IN          0x08
#define QSPI_DIRECTION_NONE        0x10
#define QSPI_MODE_LAST             0x20
        } cfg[9];
        uint8_t                    speed;
        uint8_t                    mode;
#define QSPI_MODE_DELAYED_SAMPLE   0x01
#define QSPI_MODE_CPOL             0x02
#define QSPI_MODE_CPHA             0x04
#define QSPI_MODE_CS_HI_CLK        0x08
#define QSPI_MODE_DUAL_FLASH       0x10
#define QSPI_MODE_CLK_N            0x20
        uint8_t                    cs_head;
        uint8_t                    cs_tail;
    } setup;
};
