#pragma once
#include <kernel/bitops.h>

enum sd_card_state {
    sd_state_idle  = 0,
    sd_state_ready = 1,
    sd_state_ident = 2,
    sd_state_stby  = 3,
    sd_state_tran  = 4,
    sd_state_data  = 5,
    sd_state_rcv   = 6,
    sd_state_prg   = 7,
    sd_state_dis   = 8
};

#define SD_DIRECTION_TX 0x40
#define SD_CMD(_n) (SD_DIRECTION_TX | (_n))
#define SD_ACMD(_n) SD_CMD(_n)

#define SD_GO_IDLE_STATE        SD_CMD(0)
#define SD_ALL_SEND_CID         SD_CMD(2)
#define SD_SEND_RELATIVE_ADDR   SD_CMD(3)
#define SD_IO_SEND_OP_COND      SD_CMD(5)
#define SD_SELECT_CARD          SD_CMD(7)
#define SD_SEND_IF_COND         SD_CMD(8)
#define SD_SEND_CSD             SD_CMD(9)
#define SD_VOLTAGE_SWITCH       SD_CMD(11)
#define SD_STOP_TRANSMISSION    SD_CMD(12)
#define SD_SEND_STATUS          SD_CMD(13)
#define SD_GO_INACTIVE_STATE    SD_CMD(15)
#define SD_READ_SINGLE_BLOCK    SD_CMD(17)
#define SD_READ_MULTIPLE_BLOCK  SD_CMD(18)
#define SD_SET_BLOCK_COUNT      SD_CMD(23)
#define SD_WRITE_BLOCK          SD_CMD(24)
#define SD_WRITE_MULTIPLE_BLOCK SD_CMD(25)
#define SD_ERASE_WR_BLK_START   SD_CMD(32)
#define SD_ERASE_WR_BLK_END     SD_CMD(33)
#define SD_ERASE                SD_CMD(38)
#define SD_IO_RW_DIRECT         SD_CMD(52)
#define SD_IO_RW_EXTENDED       SD_CMD(53)
#define SD_APP_CMD              SD_CMD(55)

#define SD_SET_BUS_WIDTH        SD_ACMD(6)
#define SD_SEND_OP_COND         SD_ACMD(41)
#define SD_SET_CLR_CARD_DETECT  SD_ACMD(42)
#define SD_SEND_SCR             SD_ACMD(51)


/* arguments for CMD8 */
#define SD_SEND_IF_COND_PATTERN 0xaa
#define SD_SEND_IF_COND_3V3     BIT(8)
#define SD_SEND_IF_COND_LV      BIT(9)

/* arguments for ACMD41 */
#define SD_SEND_OP_COND_3V3   BIT(20)
#define SD_SEND_OP_COND_S18   BIT(24)
#define SD_SEND_OP_COND_XPC   BIT(28)
#define SD_SEND_OP_COND_xCS   BIT(30)
#define SD_SEND_OP_COND_READY BIT(31) // called BUSY in spec, but is really not

#define SD_IO_RW_DIRECT_RW        BIT(31)
#define SD_IO_RW_DIRECT_FN_MASK   (BIT(30+1)-BIT(28))
#define SD_IO_RW_DIRECT_RAW       BIT(27)
#define SD_IO_RW_DIRECT_ADDR_MASK (BIT(25+1)-BIT(9))
#define SD_IO_RW_DIRECT_DATA_MASK (BIT(7+1)-BIT(0))

#define SD_IO_RW_DIRECT_ARG(_rw, _fn, _raw, _addr, _data)       \
    (((_rw) ? SD_IO_RW_DIRECT_RW : 0)                           \
     | bfi((_fn), SD_IO_RW_DIRECT_FN_MASK)                      \
     | ((_raw) ? SD_IO_RW_DIRECT_RAW : 0)                       \
     | bfi((_addr), SD_IO_RW_DIRECT_ADDR_MASK)                  \
     | bfi((_data), SD_IO_RW_DIRECT_DATA_MASK))

#define SD_IO_RW_EXTENDED_RW         BIT(31)
#define SD_IO_RW_EXTENDED_FN_MASK    (BIT(30+1)-BIT(28))
#define SD_IO_RW_EXTENDED_BLOCK      BIT(27)
#define SD_IO_RW_EXTENDED_INCR       BIT(26)
#define SD_IO_RW_EXTENDED_ADDR_MASK  (BIT(25+1)-BIT(9))
#define SD_IO_RW_EXTENDED_COUNT_MASK (BIT(8+1)-BIT(0))

#define SD_IO_RW_EXTENDED_ARG(_rw, _fn, _block, _incr, _addr, _count)   \
    (((_rw) ? SD_IO_RW_EXTENDED_RW : 0)                                 \
     | bfi((_fn), SD_IO_RW_EXTENDED_FN_MASK)                            \
     | ((_block) ? SD_IO_RW_EXTENDED_BLOCK : 0)                         \
     | ((_incr) ? SD_IO_RW_EXTENDED_INCR : 0)                           \
     | bfi((_addr), SD_IO_RW_EXTENDED_ADDR_MASK)                        \
     | bfi((_count), SD_IO_RW_EXTENDED_COUNT_MASK))
