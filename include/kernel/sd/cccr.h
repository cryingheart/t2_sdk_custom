#pragma once
#include <kernel/cdefs.h>

#define CCCR_REV            0
#define CCCR_REV_FORMAT_1_0 0x00
#define CCCR_REV_FORMAT_1_1 0x01
#define CCCR_REV_FORMAT_2_0 0x02
#define CCCR_REV_FORMAT_3_0 0x03
#define CCCR_REV_SDIO_1_0   0x00
#define CCCR_REV_SDIO_1_1   0x10
#define CCCR_REV_SDIO_2_0   0x30
#define CCCR_REV_SDIO_3_0   0x40

#define CCCR_SD             1
#define CCCR_SD_PHY_1_01    0
#define CCCR_SD_PHY_1_10    1
#define CCCR_SD_PHY_2_0     2
#define CCCR_SD_PHY_3_0     3

#define CCCR_IOEx           2
#define CCCR_IORx           3
#define CCCR_IENx           4
#define CCCR_IEN_IENM       BIT(0)

#define CCCR_INTx           5
#define CCCR_IOA            6
#define CCCR_IOA_ASx_MASK   0x7
#define CCCR_IOA_RES        BIT(3)

#define CCCR_IF             7
#define CCCR_IF_WIDTH_MASK  0x3
#define CCCR_IF_WIDTH_1BIT  0
#define CCCR_IF_WIDTH_4BIT  2
#define CCCR_IF_CD_DISABLE  BIT(7)

#define CCCR_CAP            8
#define CCCR_CAP_SDC        BIT(0)
#define CCCR_CAP_4BLS       BIT(7)

#define CCCR_CIS_0          9
#define CCCR_CIS_1          10
#define CCCR_CIS_2          11

#define CCCR_BLOCK_SIZE_0   16
#define CCCR_BLOCK_SIZE_1   17

#define CCCR_INTEXT         0x16
#define CCCR_INTEXT_SAI     BIT(0)
#define CCCR_INTEXT_EAI     BIT(1)
