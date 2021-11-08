#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <kernel/sd/cid.h>
#include <kernel/sd/csd.h>
#include <kernel/sd/csr.h>
#include <kernel/sd/scr.h>
#include <kernel/sd/cccr.h>
#include <kernel/sd/cis.h>
#include <kernel/sd/commands.h>
#include <kernel/sd/io.h>
#include <kernel/sd/memory.h>
#include <kernel/sd/block.h>
#include <kernel/sd/cop.h>

/* BEGIN GENERATED PROTOS */

void sd_attach(void);

void sd_detach(void);

void sd_disable(void);

void sd_dump_cid(struct sd_memory_card* card);

void sd_dump_csd(struct sd_memory_card* card);

bool sd_enable(void);

void sd_erase(unsigned int start_block, unsigned int end_block);

void sd_nop(void);

void sd_read(unsigned int block_number, void* buf, size_t len);

void sd_write(unsigned int block_number, const void* buf, size_t len);

/* END GENERATED PROTOS */
