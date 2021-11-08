#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Serial Flash Discoverable Parameters */
/* as defined in JEDEC JESD216B */

/*
 * SFDP is a standard by JEDEC, designed to eliminate the need for
 * hard-coded parameter tables based on SPI Flash manufacturer IDs.
 * This apparently grew out of something Intel invented for their
 * firmware flash interface, documentation for this seems to be
 * missing from the Internet though. As of 2017 its implementation
 * among flash manufacturers is limited.
 *
 * The SFDP data is read from the flash device using instruction "Read
 * SFDP" (5Ah) followed by three address bytes and one dummy byte
 * (same format as Fast Read). Address space is different from regular
 * flash address space.
 *
 * Data is structured as a global header followed by a sequence of
 * parameter headers. Each parameter header describes a data structure
 * located at a specified address.
 *
 * There are three structures defined by JESD216: Basic Flash
 * Parameters (for some reason referred to as "Basic SPI protocol" in
 * exactly one place in the standard), Sector map, and, 4-Byte Address
 * Instruction Table. A fourth, Replay Protected Monotonic Counters,
 * seems to be defined by Intel.
 *
 * Layout of global and parameter headers:
 * +--------+--------+--------+--------+
 * |   'P'  |   'D'  |   'F'  |   'S'  |
 * +--------+--------+--------+--------+
 * |   FF   |   NPH  | MAJOR  | MINOR  |
 * +--------+--------+--------+--------+
 * | LENGTH | MAJOR  | MINOR  | ID LSB |  ^
 * +--------+--------+--------+--------+  |
 * | ID MSB | Parameter Table Pointer  |  |
 * +--------+--------+--------+--------+  |
 * ...                                   NPH+1
 * +--------+--------+--------+--------+  |
 * | LENGTH | MAJOR  | MINOR  | ID LSB |  |
 * +--------+--------+--------+--------+  |
 * | ID MSB | Parameter Table Pointer  |  |
 * +--------+--------+--------+--------+  v
 *
 * Functions in this files extract various fields from the table.  The
 * parameters are as the word number in the standard (one-indexed).
 * Most functions operate on a single word.
 */

#pragma push_macro("_BFX")
#undef _BFX
#define _BFX(v, a, b) (((v) >> a) & ((1u << b) - 1))

/* header definitions */

/* major/minor versions */
#define SFDP_JESD216_MAJOR  1
#define SFDP_JESD216_MINOR  1
#define SFDP_JESD216A_MINOR 5
#define SFDP_JESD216B_MINOR 6

/* defined parameter tables */
#define SFDP_BASIC_FLASH_PARAMETERS 0xff00
#define SFDP_SECTOR_MAP             0xff81
/* following possibly defined by Intel document "Serial Flash
 * Hardening Product" */
#define SFDP_MONOTONIC_COUNTERS     0xff03
#define SFDP_4_ADDRESS_INSTRUCTION  0xff84

static inline bool
sfdp_header_valid(uint32_t word1)
{
    return word1 == 0x50444653;
}

static inline unsigned int
sfdp_header_major(uint32_t word2)
{
    return _BFX(word2, 8, 8);
}

static inline unsigned int
sfdp_header_minor(uint32_t word2)
{
    return _BFX(word2, 0, 8);
}

static inline bool
sfdp_header_version_check(uint32_t word2)
{
    return sfdp_header_major(word2) == SFDP_JESD216_MAJOR;
}

static inline unsigned int
sfdp_header_num_params(uint32_t word2)
{
    return 1 + _BFX(word2, 16, 8);
}

/* return major version number of table */
static inline unsigned int
sfdp_ptable_major(uint32_t word1)
{
    return _BFX(word1, 16, 8);
}

/* return minor version number of table */
static inline unsigned int
sfdp_ptable_minor(uint32_t word1)
{
    return _BFX(word1, 8, 8);
}

/* return length (in words) of parameter table */
static inline unsigned int
sfdp_ptable_length(uint32_t word1)
{
    return _BFX(word1, 24, 8);
}

/* return address of parameter table */
static inline unsigned int
sfdp_ptable_pointer(uint32_t word2)
{
    return _BFX(word2, 0, 24);
}

static inline unsigned int
sfdp_ptable_id(uint32_t word1, uint32_t word2)
{
    return _BFX(word1, 0, 8) | (_BFX(word2, 24, 8) << 8);
}

/* return true if this is a Basic Flash Paramater Table */
static inline bool
sfdp_pheader_is_bfp(uint32_t word1, uint32_t word2)
{
    return sfdp_ptable_id(word1, word2) == SFDP_BASIC_FLASH_PARAMETERS;
}

/* return true if this is a Sector Map Table */
static inline bool
sfdp_pheader_is_smp(uint32_t word1, uint32_t word2)
{
    return sfdp_ptable_id(word1, word2) == SFDP_SECTOR_MAP;
}

/* return true if this is a 4-byte Address Instruction Table */
static inline bool
sfdp_pheader_is_4ai(uint32_t word1, uint32_t word2)
{
    return sfdp_ptable_id(word1, word2) == SFDP_4_ADDRESS_INSTRUCTION;
}

/* defintions for Basic Flash Parameter Table */

/* legacy: return true if 4k block erase is supported */
static inline bool
sfdp_bfp_4k_block_erase_supported(uint32_t word1)
{
    return _BFX(word1, 0, 2) == 1;
}

/* legacy: return true if write buffer is at least 64 bytes */
static inline bool
sfdp_bfp_64_byte_granularity(uint32_t word1)
{
    return _BFX(word1, 2, 1) == 1;
}

/* legacy: return true if status register is volatile */
static inline bool
sfdp_bfp_block_protect_is_volatile(uint32_t word1)
{
    return _BFX(word1, 3, 1) == 1;
}

/* legacy: return instruction for write enable a volatile status register */
static inline unsigned int
sfdp_bfp_block_protect_instruction(uint32_t word1)
{
    if(_BFX(word1, 4, 1) == 0)
        return 0x50;
    return 0x6;
}

/* return instruction for 4k block erase */
static inline unsigned int
sfdp_bfp_4k_block_erase_instruction(uint32_t word1)
{
    return _BFX(word1, 8, 8);
}

/* return true if 1-1-2 fast read is supported */
static inline bool
sfdp_bfp_fast_read_1_1_2_supported(uint32_t word1)
{
    return _BFX(word1, 16, 1);
}

/* return true if 1-2-2 fast read is supported */
static inline bool
sfdp_bfp_fast_read_1_2_2_supported(uint32_t word1)
{
    return _BFX(word1, 20, 1);
}

/* return true if 1-4-4 fast read is supported */
static inline bool
sfdp_bfp_fast_read_1_4_4_supported(uint32_t word1)
{
    return _BFX(word1, 21, 1);
}

/* return true if 1-1-4 fast read is supported */
static inline bool
sfdp_bfp_fast_read_1_1_4_supported(uint32_t word1)
{
    return _BFX(word1, 22, 1);
}

/* return true if 3-byte addressing is supported */
static inline bool
sfdp_bfp_3_byte_addressing_supported(uint32_t word1)
{
    return _BFX(word1, 18, 1) == 0;
}

/* return true if 4-byte addressing is supported */
static inline bool
sfdp_bfp_4_byte_addressing_supported(uint32_t word1)
{
    return _BFX(word1, 17, 2) == 1 || _BFX(word1, 17, 2) == 2;
}

/* return true if double transfer rate is supported */
static inline bool
sfdp_bfp_dtr_supported(uint32_t word1)
{
    return _BFX(word1, 19, 1);
}

/* return density in bytes */
static inline uint64_t
sfdp_bfp_density(uint32_t word2)
{
    unsigned int log2 = _BFX(word2, 31,1);
    unsigned int n = _BFX(word2, 0, 31);
    if(log2 == 0)
        return n/8+1;
    n -= 3;
    if(n >= 64)
        return ~0ULL;
    return 1ULL << n;
}

/* return 1-4-4 fast read dummy clocks */
static inline uint32_t
sfdp_bfp_fast_read_1_4_4_dummy_clocks(uint32_t word3)
{
    return _BFX(word3, 0, 5);
}

/* return 1-4-4 fast read mode clocks */
static inline uint32_t
sfdp_bfp_fast_read_1_4_4_mode_clocks(uint32_t word3)
{
    return _BFX(word3, 5, 2);
}

/* return 1-4-4 fast read instruction */
static inline uint32_t
sfdp_bfp_fast_read_1_4_4_instruction(uint32_t word3)
{
    return _BFX(word3, 8, 8);
}

/* return 1-1-4 fast read dummy clocks */
static inline uint32_t
sfdp_bfp_fast_read_1_1_4_dummy_clocks(uint32_t word3)
{
    return _BFX(word3, 16, 5);
}

/* return 1-1-4 fast read mode clocks */
static inline uint32_t
sfdp_bfp_fast_read_1_1_4_mode_clocks(uint32_t word3)
{
    return _BFX(word3, 21, 2);
}

/* return 1-1-4 fast read instruction */
static inline uint32_t
sfdp_bfp_fast_read_1_1_4_instruction(uint32_t word3)
{
    return _BFX(word3, 24, 8);
}

/* return 1-1-2 fast read dummy clocks */
static inline uint32_t
sfdp_bfp_fast_read_1_1_2_dummy_clocks(uint32_t word4)
{
    return _BFX(word4, 0, 5);
}

/* return 1-1-2 fast read mode clocks */
static inline uint32_t
sfdp_bfp_fast_read_1_1_2_mode_clocks(uint32_t word4)
{
    return _BFX(word4, 5, 2);
}

/* return 1-1-2 fast read instruction */
static inline uint32_t
sfdp_bfp_fast_read_1_1_2_instruction(uint32_t word4)
{
    return _BFX(word4, 8, 8);
}

/* return 1-2-2 fast read dummy clocks */
static inline uint32_t
sfdp_bfp_fast_read_1_2_2_dummy_clocks(uint32_t word4)
{
    return _BFX(word4, 16, 5);
}

/* return 1-2-2 fast read mode clocks */
static inline uint32_t
sfdp_bfp_fast_read_1_2_2_mode_clocks(uint32_t word4)
{
    return _BFX(word4, 21, 2);
}

/* return 1-2-2 fast read instruction */
static inline uint32_t
sfdp_bfp_fast_read_1_2_2_instruction(uint32_t word4)
{
    return _BFX(word4, 24, 8);
}

/* return true if 2-2-2 fast read is supported */
static inline bool
sfdp_bfp_fast_read_2_2_2_supported(uint32_t word5)
{
    return _BFX(word5, 0, 1);
}

/* return true if 4-4-4 fast read is supported */
static inline bool
sfdp_bfp_fast_read_4_4_4_supported(uint32_t word5)
{
    return _BFX(word5, 4, 1);
}

/* return 2-2-2 fast read dummy clocks */
static inline uint32_t
sfdp_bfp_fast_read_2_2_2_dummy_clocks(uint32_t word6)
{
    return _BFX(word6, 16, 5);
}

/* return 2-2-2 fast read mode clocks */
static inline uint32_t
sfdp_bfp_fast_read_2_2_2_mode_clocks(uint32_t word6)
{
    return _BFX(word6, 21, 2);
}

/* return 2-2-2 fast read instruction */
static inline uint32_t
sfdp_bfp_fast_read_2_2_2_instruction(uint32_t word6)
{
    return _BFX(word6, 24, 8);
}

/* return 4-4-4 fast read dummy clocks */
static inline uint32_t
sfdp_bfp_fast_read_4_4_4_dummy_clocks(uint32_t word7)
{
    return _BFX(word7, 16, 5);
}

/* return 4-4-4 fast read mode clocks */
static inline uint32_t
sfdp_bfp_fast_read_4_4_4_mode_clocks(uint32_t word7)
{
    return _BFX(word7, 21, 2);
}

/* return 4-4-4 fast read instruction */
static inline uint32_t
sfdp_bfp_fast_read_4_4_4_instruction(uint32_t word7)
{
    return _BFX(word7, 24, 8);
}

/* return erase type 1 size in log2(bytes) */
static inline uint32_t
sfdp_bfp_erase_type_1_size(uint32_t word8)
{
    return _BFX(word8, 0, 8);
}

/* return erase type 1 instruction */
static inline uint32_t
sfdp_bfp_erase_type_1_instruction(uint32_t word8)
{
    return _BFX(word8, 8, 8);
}

/* return true if erase type 1 is well defined */
static inline bool
sfdp_bfp_erase_type_1_defined(uint32_t word8)
{
    return sfdp_bfp_erase_type_1_size(word8) != 0
        && sfdp_bfp_erase_type_1_instruction(word8) != 0xff;
}

/* return erase type 2 size in log2(bytes) */
static inline uint32_t
sfdp_bfp_erase_type_2_size(uint32_t word8)
{
    return _BFX(word8, 16, 8);
}

/* return erase type 2 instruction */
static inline uint32_t
sfdp_bfp_erase_type_2_instruction(uint32_t word8)
{
    return _BFX(word8, 24, 8);
}

/* return true if erase type 2 is well defined */
static inline bool
sfdp_bfp_erase_type_2_defined(uint32_t word8)
{
    return sfdp_bfp_erase_type_2_size(word8) != 0
        && sfdp_bfp_erase_type_2_instruction(word8) != 0xff;
}

/* return erase type 3 size in log2(bytes) */
static inline uint32_t
sfdp_bfp_erase_type_3_size(uint32_t word9)
{
    return _BFX(word9, 0, 8);
}

/* return erase type 3 instruction */
static inline uint32_t
sfdp_bfp_erase_type_3_instruction(uint32_t word9)
{
    return _BFX(word9, 8, 8);
}

/* return true if erase type 3 is well defined */
static inline bool
sfdp_bfp_erase_type_3_defined(uint32_t word9)
{
    return sfdp_bfp_erase_type_3_size(word9) != 0
        && sfdp_bfp_erase_type_3_instruction(word9) != 0xff;
}

/* return erase type 4 size in log2(bytes) */
static inline uint32_t
sfdp_bfp_erase_type_4_size(uint32_t word9)
{
    return _BFX(word9, 16, 8);
}

/* return erase type 4 instruction */
static inline uint32_t
sfdp_bfp_erase_type_4_instruction(uint32_t word9)
{
    return _BFX(word9, 24, 8);
}

/* return true if erase type 4 is well defined */
static inline bool
sfdp_bfp_erase_type_4_defined(uint32_t word9)
{
    return sfdp_bfp_erase_type_4_size(word9) != 0
        && sfdp_bfp_erase_type_4_instruction(word9) != 0xff;
}

/* return multiplier describing ratio between max and typical erase
 * time for both erase type n and chip erase */
static inline uint32_t
sfdp_bfp_typical_max_erase_mult(uint32_t word10)
{
    unsigned int count = _BFX(word10, 0, 4);
    return 2*(count+1);
}

/* return typical time for erase type 1 in milliseconds */
static inline uint32_t
sfdp_bfp_erase_type_1_typical_time(uint32_t word10)
{
    unsigned int unit = _BFX(word10, 9, 2);
    unsigned int count = 1 + _BFX(word10, 4, 5);

    if(unit == 0)
        return count;
    if(unit == 1)
        return count * 16;
    if(unit == 2)
        return count * 128;
    return count * 1000;
}

/* return typical time for erase type 2 in milliseconds */
static inline uint32_t
sfdp_bfp_erase_type_2_typical_time(uint32_t word10)
{
    unsigned int unit = _BFX(word10, 16, 2);
    unsigned int count = 1 + _BFX(word10, 11, 5);

    if(unit == 0)
        return count;
    if(unit == 1)
        return count * 16;
    if(unit == 2)
        return count * 128;
    return count * 1000;
}

/* return typical time for erase type 3 in milliseconds */
static inline uint32_t
sfdp_bfp_erase_type_3_typical_time(uint32_t word10)
{
    unsigned int unit = _BFX(word10, 23, 2);
    unsigned int count = 1 + _BFX(word10, 18, 5);

    if(unit == 0)
        return count;
    if(unit == 1)
        return count * 16;
    if(unit == 2)
        return count * 128;
    return count * 1000;
}

/* return typical time for erase type 4 in milliseconds */
static inline uint32_t
sfdp_bfp_erase_type_4_typical_time(uint32_t word10)
{
    unsigned int unit = _BFX(word10, 30, 2);
    unsigned int count = 1 + _BFX(word10 ,25, 5);

    if(unit == 0)
        return count;
    if(unit == 1)
        return count * 16;
    if(unit == 2)
        return count * 128;
    return count * 1000;
}

/* return multiplier describing ratio between max and typical
 * page/byte program time */
static inline uint32_t
sfdp_bfp_typical_max_program_mult(uint32_t word11)
{
    unsigned int count = _BFX(word11, 0, 4);
    return 2*(count+1);
}

/* return page size in log2(bytes) */
static inline size_t
sfdp_bfp_page_size(uint32_t word11)
{
    return _BFX(word11, 4, 4);
}

/* return typical time for page program in microseconds */
static inline uint32_t
sfdp_bfp_page_program_typical_time(uint32_t word11)
{
    unsigned int unit = _BFX(word11, 13, 1);
    unsigned int count = 1 + _BFX(word11, 8, 5);

    if(unit)
        return count * 64;

    return count * 8;
}

/* return typical time for byte programin microseconds, this returns
 * time for first byte */
static inline uint32_t
sfdp_bfp_byte_program_first_typical_time(uint32_t word11)
{
    unsigned unit = _BFX(word11, 18, 1);
    unsigned count = 1 + _BFX(word11, 14, 4);

    if(unit)
        return 8 * count;
    return count;
}

/* return typical time for byte programin microseconds, this returns
 * time each additional byte */
static inline uint32_t
sfdp_bfp_byte_program_additional_typical_time(uint32_t word11)
{
    unsigned unit = _BFX(word11, 23, 1);
    unsigned count = 1 + _BFX(word11, 19, 4);

    if(unit)
        return 8 * count;
    return count;
}
/* return typical time for chip erase in milliseconds */
static inline uint32_t
sfdp_bfp_chip_erase_typical_time(uint32_t word11)
{
    unsigned unit = _BFX(word11, 29, 2);
    unsigned count = 1 + _BFX(word11, 24, 5);

    if(unit == 0)
        return count * 16;
    if(unit == 2)
        return count * 256;
    if(unit == 3)
        return count * 4000;
    return count * 64000;
}

/* operations not allowed during suspended program, highly device
 * specific, see datasheet */
static inline unsigned int
sfdp_bfp_suspend_program_prohibited_ops(uint32_t word12)
{
    return _BFX(word12, 4, 0);
}

/* operations not allowed during suspended erase, highly device
 * specific, see datasheet */
static inline unsigned int
sfdp_bfp_suspend_erase_prohibited_ops(uint32_t word12)
{
    return _BFX(word12, 4, 4);
}

/* time required to make any progress to an in-progress program in
 * microseconds */
static inline unsigned int
sfdp_bfp_suspend_program_interval(uint32_t word12)
{
    return 64 * (1 + _BFX(word12, 9, 4));
}

/* time for device to suspend an in-progress program in microseconds */
static inline unsigned int
sfdp_bfp_suspend_program_max_latency(uint32_t word12)
{
    unsigned unit = _BFX(word12, 18, 2);
    unsigned count = 1 + _BFX(word12, 13, 5);
    if(unit == 0)
        /* units in 128 ns, round up to microseconds */
        return (count + 7) / 8;
    if(unit == 1)
        return count;
    if(unit == 2)
        return count * 8;
    return count * 64;
}

/* time required to make any progress to an in-progress erase in
 * microseconds */
static inline unsigned int
sfdp_bfp_suspend_erase_interval(uint32_t word12)
{
    return 64 * (1 + _BFX(word12, 20, 4));
}

/* time for device to suspend an in-progress erase in microseconds */
static inline unsigned int
sfdp_bfp_suspend_erase_max_latency(uint32_t word12)
{
    unsigned unit = _BFX(word12, 29, 2);
    unsigned count = 1 + _BFX(word12, 24, 5);
    if(unit == 0)
        /* units in 128 ns, round up to microseconds */
        return (count + 7) / 8;
    if(unit == 1)
        return count;
    if(unit == 2)
        return count * 8;
    return count * 64;
}

/* device supports suspend of in-progress erase or program */
static inline bool
sfdp_bfp_suspend_resume_supported(uint32_t word12)
{
    return _BFX(word12, 31, 1) == 0;
}

/* return instruction to resume a suspended program */
static inline unsigned int
sfdp_bfp_program_resume_instruction(uint32_t word13)
{
    return _BFX(word13, 0, 8);
}

/* return instruction to suspend an in-progress program */
static inline unsigned int
sfdp_bfp_program_suspend_instruction(uint32_t word13)
{
    return _BFX(word13, 8, 8);
}

/* return instruction to resume a suspended erase */
static inline unsigned int
sfdp_bfp_erase_resume_instruction(uint32_t word13)
{
    return _BFX(word13, 16, 8);
}

/* return instruction to suspend an in-progress erase */
static inline unsigned int
sfdp_bfp_erase_suspend_instruction(uint32_t word13)
{
    return _BFX(word13, 24, 8);
}

/* return true if deep powerdown is supported */
static inline bool
sfdp_bfp_deep_powerdown_supported(uint32_t word14)
{
    return _BFX(word14, 31, 1) == 0;
}

/* return instruction to enter deep powerdown */
static inline unsigned int
sfdp_bfp_deep_powerdown_enter_instruction(uint32_t word14)
{
    return _BFX(word14, 23, 8);
}

/* return instruction to exit deep powerdown */
static inline unsigned int
sfdp_bfp_deep_powerdown_exit_instruction(uint32_t word14)
{
    return _BFX(word14, 15, 8);
}

/* return max time to exit deep powerdown in microseconds */
static inline unsigned int
sfdp_bfp_deep_powerdown_exit_delay(uint32_t word14)
{
    unsigned unit = _BFX(word14, 13, 2);
    unsigned count = 1 + _BFX(word14, 8, 5);
    if(unit == 0)
        /* units in 128 ns, round up to microseconds */
        return (count + 7) / 8;
    if(unit == 1)
        return count;
    if(unit == 2)
        return count * 8;
    return count * 64;
}

/* return true if status register can be polled with instruction 70 */
static inline unsigned int
sfdp_bfp_status_register_poll_70(uint32_t word14)
{
    return _BFX(word14, 3, 1) == 1;
}

/* return true if status register can be polled with instruction 05 */
static inline unsigned int
sfdp_bfp_status_register_poll_05(uint32_t word14)
{
    return _BFX(word14, 2, 1) == 1;
}

/* return true if HOLD/RESET can be disabled by writing a zero bit bit
 * 4 of the Non-Volatile Extended Configuration Register */
static inline bool
sfdp_bfp_hold_disable_supported(uint32_t word15)
{
    return _BFX(word15, 23, 1);
}

/* return value describing how to enter quad data mode (1-1-4 or
 * 1-4-4) */
static inline unsigned int
sfdp_bfp_quad_enable_requirements(uint32_t word15)
{
    return _BFX(word15, 20, 3);
}

/* return value describing how to enter XIP (0-4-4) mode */
static inline unsigned int
sfdp_bfp_xip_enter_method(uint32_t word15)
{
    return _BFX(word15, 16, 4);
}

/* return value describing how to exit XIP (0-4-4) mode */
static inline unsigned int
sfdp_bfp_xip_exit_method(uint32_t word15)
{
    return _BFX(word15, 10, 6);
}

/* return true if XIP (0-4-4) mode is supported */
static inline unsigned int
sfdp_bfp_xip_supported(uint32_t word15)
{
    return _BFX(word15, 9, 1);
}

/* return value describing how to enable 4-4-4 mode */
static inline unsigned int
sfdp_bfp_4_4_4_enable_method(uint32_t word15)
{
    return _BFX(word15, 4, 5);
}

/* return value describing how to disable 4-4-4 mode */
static inline unsigned int
sfdp_bfp_4_4_4_disable_method(uint32_t word15)
{
    return _BFX(word15, 0, 4);
}

/* return value describing how to enter 4-byte addressing mode */
static inline unsigned int
sfdp_bfp_4_byte_address_enter_methods(uint32_t word16)
{
    return _BFX(word16, 24, 8);
}

/* return value describing how to exit 4-byte addressing mode */
static inline unsigned int
sfdp_bfp_4_byte_address_exit_methods(uint32_t word16)
{
    return _BFX(word16, 14, 10);
}

/* return value describing how to perform soft reset */
static inline unsigned int
sfdp_bfp_soft_reset_methods(uint32_t word16)
{
    return _BFX(word16, 8, 6);
}

/* return value describing how to write to status register 1 */
static inline unsigned int
sfdp_bfp_status_register_1_write_methods(uint32_t word16)
{
    return _BFX(word16, 0, 7);
}

/* defintions for Sector Map Parameter Table */

/* return true if this is a Command Descriptor (as opposed to a Map
 * Descriptor) */
static inline bool
sfdp_smp_descriptor_is_command(uint32_t word)
{
    return _BFX(word, 1, 1) == 0;
}

/* return true if this is the final descriptor of this type */
static inline bool
sfdp_smp_descriptor_is_final(uint32_t word)
{
    return _BFX(word, 1, 1) == 1;
}

/* return CDC read data mask, mask with data from CDC instruction */
static inline unsigned int
sfdp_smp_cdc_read_data_mask(uint32_t word1)
{
    return _BFX(word1, 24, 8);
}

/* return CDC address length as coded value, 0: no address, 1: 3-byte,
 * 2: 4-byte, 3: current setting */
static inline unsigned int
sfdp_smp_cdc_address_length(uint32_t word1)
{
    return _BFX(word1, 22, 2);
}

/* return CDC read latency, 0-14 latency in cycles, 15: currently set
 * latency */
static inline unsigned int
sfdp_smp_cdc_read_latency(uint32_t word1)
{
    return _BFX(word1, 16, 4);
}

/* return CDC read instruction */
static inline unsigned int
sfdp_smp_cdc_instruction(uint32_t word1)
{
    return _BFX(word1, 8, 8);
}

/* return configuration id for this configuration */
static inline unsigned int
sfdp_smp_map_configuration_id(uint32_t header)
{
    return _BFX(header, 8, 8);
}

/* return number of region words following this header */
static inline unsigned int
sfdp_smp_map_region_count(uint32_t header)
{
    return 1 + _BFX(header, 16, 8);
}

/* return region size in bytes, special value 0 == 4 GiBytes */
static inline uint32_t
sfdp_smp_region_size(uint32_t region)
{
    unsigned int count = 1 + _BFX(region, 8, 24);

    return count * 256;
}

static inline bool
sfdp_smp_region_erase_type_1_supported(uint32_t region)
{
    return _BFX(region, 0, 1) == 1;
}

static inline bool
sfdp_smp_region_erase_type_2_supported(uint32_t region)
{
    return _BFX(region, 1, 1) == 1;
}

static inline bool
sfdp_smp_region_erase_type_3_supported(uint32_t region)
{
    return _BFX(region, 2, 1) == 1;
}

static inline bool
sfdp_smp_region_erase_type_4_supported(uint32_t region)
{
    return _BFX(region, 3, 1) == 1;
}

/* 4-address stuff omitted */

#pragma pop_macro("_BFX")

enum {
    /* Device does not have a QE bit. Device detects 1-1-4 and 1-4-4
     * reads based on instruction. DQ3/HOLD# functions as hold during
     * instruction phase. */
    SFDP_QER_MODE_0,

    /* QE is bit 1 of status register 2. It is set via Write Status
     * with two data bytes where bit 1 of the second byte is one. It
     * is cleared via Write Status with two data bytes where bit 1 of
     * the second byte is zero. Writing only one byte to the status
     * register has the side-effect of clearing status register 2,
     * including the QE bit. The 100b code is used if writing one byte
     * to the status register does not modify status register 2. */
    SFDP_QER_MODE_1,

    /* QE is bit 6 of status register 1. It is set via Write Status
     * with one data byte where bit 6 is one. It is cleared via Write
     * Status with one data byte where bit 6 is zero. */
    SFDP_QER_MODE_2,

    /* QE is bit 7 of status register 2. It is set via Write status
     * register 2 instruction 3Eh with one data byte where bit 7 is
     * one. It is cleared via Write status register 2 instruction 3Eh
     * with one data byte where bit 7 is zero. The status register 2
     * is read using instruction 3Fh. */
    SFDP_QER_MODE_3,

    /* QE is bit 1 of status register 2. It is set via Write Status
     * with two data bytes where bit 1 of the second byte is one. It
     * is cleared via Write Status with two data bytes where bit 1 of
     * the second byte is zero. In contrast to the 001b code, writing
     * one byte to the status register does not modify status register
     * 2. */
    SFDP_QER_MODE_4,

    /* QE is bit 1 of the status register 2. Status register 1 is read
     * using Read Status instruction 05h. Status register 2 is read
     * using instruction 35h. QE is set via Write Status instruction
     * 01h with two data bytes where bit 1 of the second byte is
     * one. It is cleared via Write Status with two data bytes where
     * bit 1 of the second byte is zero. */
    SFDP_QER_MODE_5
};
