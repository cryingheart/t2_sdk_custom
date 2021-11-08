#pragma once
#include <stdint.h>
#include <kernel/packet.h>
#include <kernel/rfdrv.h>

#define F15_4_MLME_RECEIVER_TEST    0x0000
#define F15_4_MLME_TRANSMITTER_TEST 0x0001
#define F15_4_MLME_END_TEST         0x0002

typedef struct {
    uint16_t opcode;
    uint8_t length;
    uint8_t parameter[255];
} __attribute__ ((packed)) f15_4_mlme_t;

void f15_4_mlme(struct pfrag *pf);
void f15_4_mac_start(void);
void f15_4_mac_stop(void);

