#pragma once
#include <stdint.h>
#include <stddef.h>
#include <kernel/sd/memory.h>
#include <kernel/dma.h>

#define PROF_CMD_IDX 0x100
#define PROF_CMD_ARG 0x101
#define PROF_RSP_IDX 0x102
#define PROF_RSP_ARG 0x103

enum sd_request_type {
    sd_request_nop,
    sd_request_attach,
    sd_request_detach,
    sd_request_read,
    sd_request_write,
    sd_request_erase,
    sd_request_terminate
};

struct sd_host_controller {
    struct sd_memory_card  card;
    /* per request fields */
    enum sd_request_type   request;
    void                  *buffer;
    size_t                 size;
    //struct dma_desc       *desc;
    uint32_t               address;
    uint32_t               address2; /* erase only */
};

extern struct sd_host_controller sd_host_controller;

struct card_state;

/* BEGIN GENERATED PROTOS */

void sd_host(void);

void sd_host_attach(void);

void sd_host_erase(void);

void sd_host_read(void);

void sd_host_write(void);

void sd_hw_abort_transaction(void);

void sd_hw_cd_connect(void);

void sd_hw_cd_disconnect(void);

unsigned int sd_hw_clock_cycle(unsigned int count);

uint32_t sd_hw_clock_delay(unsigned ncycles);

void sd_hw_disable_busy_wait(void);

void sd_hw_disable_read_wait(void);

void sd_hw_disable_rx_status(void);

void sd_hw_disable_tx_status(void);

void sd_hw_enable_busy_wait(void);

void sd_hw_enable_read_wait(void);

void sd_hw_enable_rx_status(void);

void sd_hw_enable_tx_status(void);

unsigned int sd_hw_get_clock_freq(void);

void sd_hw_init(void);

void sd_hw_irq_disable(void);

void sd_hw_irq_enable(void);

void sd_hw_send(uint8_t command, uint32_t arg, uint8_t crc);

void sd_hw_set_bus_width(bool four_wire);

void sd_hw_set_clock(bool enable);

void sd_hw_set_clock_freq(unsigned int freq_khz);

void sd_hw_set_data_size(size_t size, size_t count);

void sd_hw_set_rx_notice(size_t offset);

void sd_hw_set_tx_notice(size_t offset);

void sd_hw_start_data_rx(void);

void sd_hw_start_data_tx(void);

void sd_hw_start_rx(void);

void sd_hw_start_tx(void);

void sd_hw_sw_clock(bool enable);

void sd_hw_wait_dat0_busy(void);

void sd_hw_wait_event(unsigned int event_id);

bool
sd_hw_wait_event_timeout(unsigned int event_id,
                         unsigned int timeout);

void
sd_io_read(unsigned int function,
           unsigned int address,
           void* dst,
           size_t block_count,
           size_t size);

uint8_t sd_io_read_byte(unsigned int function, unsigned int address);

void
sd_io_write(unsigned int function,
            unsigned int address,
            const void* src,
            size_t block_count,
            size_t size);

uint8_t
sd_io_write_byte(unsigned int function,
                 unsigned int address,
                 uint8_t data);

bool
sd_mem_erase(struct sd_memory_card* card_info,
             unsigned int start_address,
             unsigned int end_address);

bool
sd_mem_read(struct sd_memory_card* card_info,
            unsigned int address,
            void* dst,
            size_t length);

bool
sd_mem_set_bus_width(struct sd_memory_card* card_info,
                     bool four_wire_mode);

bool
sd_mem_write(struct sd_memory_card* card_info,
             unsigned int address,
             const void* src,
             size_t length);

bool sd_select_card(struct sd_memory_card* card_info);

bool
sd_send_app_cmd(struct sd_memory_card* card_info,
                uint8_t acmd,
                uint32_t arg);

bool sd_send_cid(struct sd_memory_card* card_info);

bool sd_send_csd(struct sd_memory_card* card_info);

bool sd_send_if_cond(struct sd_memory_card* card_info);

bool sd_send_op_cond(struct sd_memory_card* card_info, uint32_t arg);

bool sd_send_relative_addr(struct sd_memory_card* card_info);

bool sd_send_scr(struct sd_memory_card* card_info);

bool sd_send_status(struct sd_memory_card* card_info);

bool
sd_start_transaction(uint8_t cmd,
                     uint32_t arg,
                     bool response,
                     bool data_phase);

bool
sd_start_transaction_r1(struct sd_memory_card* card_info,
                        uint8_t cmd,
                        uint32_t arg);

bool
sd_start_transaction_r1b(struct sd_memory_card* card_info,
                         uint8_t cmd,
                         uint32_t arg);

bool
sd_start_transaction_r2(struct sd_memory_card* card_info,
                        uint8_t cmd,
                        uint32_t arg,
                        uint32_t* resp);

bool
sd_start_transaction_r6(struct sd_memory_card* card_info,
                        uint8_t cmd,
                        uint32_t arg);

bool sd_stop_transmission(struct sd_memory_card* card_info);

void sdio_io_send_op_cond(void);

unsigned int
sdio_read_write_cia(struct card_state* cs,
                    bool write,
                    unsigned int address,
                    unsigned int value);

void sdio_slave_loop(struct card_state* cs);

/* END GENERATED PROTOS */
