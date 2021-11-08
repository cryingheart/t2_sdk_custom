#pragma once
#include <stdint.h>
#include <string.h>
#include <kernel/hostio.h>

/*
[[[cog
import cog, generate
cog.outl('')
generate.header(generate.Emitter(cog.out), 'flash')
cog.outl('')
]]] */

#define HIO_GROUP_FLASH 8

/*
 * Message identify
 */

#define FLASH_IDENTIFY_REQ 0x00
#define FLASH_IDENTIFY_RSP 0x80

/** Check presence of supported serial flash device. Request structure.
 */
struct flash_identify_req {
};

static inline struct packet *
alloc_flash_identify_req(struct flash_identify_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_identify_req(void)
{
    struct flash_identify_req *req;
    struct packet *pkt = alloc_flash_identify_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Check presence of supported serial flash device. Response structure.
 */
struct flash_identify_rsp {
    int32_t status; /**< result status, zero is success */
    uint16_t page_size; /**< Size of flash page */
    uint16_t sector_size; /**< Size of flash erase sector */
    uint32_t num_pages; /**< Device capacity in units of flash pages */
    uint32_t idcode; /**< Flash device manufacturer and device code */
};

static inline struct packet *
alloc_flash_identify_rsp(struct flash_identify_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_identify_rsp(const int32_t status, const uint16_t page_size, const uint16_t sector_size, const uint32_t num_pages, const uint32_t idcode)
{
    struct flash_identify_rsp *rsp;
    struct packet *pkt = alloc_flash_identify_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->page_size = page_size;
        rsp->sector_size = sector_size;
        rsp->num_pages = num_pages;
        rsp->idcode = idcode;
    }
    return pkt;
}
/*
 * Message read
 */

#define FLASH_READ_REQ 0x01
#define FLASH_READ_RSP 0x81

/** Read from flash. Request structure.
 */
struct flash_read_req {
    uint32_t address; /**< Sector address to start reading from */
    uint32_t length; /**< Number of bytes to read */
};

static inline struct packet *
alloc_flash_read_req(struct flash_read_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_read_req(const uint32_t address, const uint32_t length)
{
    struct flash_read_req *req;
    struct packet *pkt = alloc_flash_read_req(&req);
    if (pkt) {
        req->address = address;
        req->length = length;
    }
    return pkt;
}

/** Read from flash. Response structure.
 */
struct flash_read_rsp {
    int32_t status; /**< result status, zero is success */
    uint8_t data[0]; /**< Data read from flash */
};

static inline struct packet *
alloc_flash_read_rsp(struct flash_read_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_read_rsp(const int32_t status, const uint8_t data[0])
{
    struct flash_read_rsp *rsp;
    struct packet *pkt = alloc_flash_read_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        /* data must be set by the caller */
    }
    return pkt;
}
/*
 * Message write
 */

#define FLASH_WRITE_REQ 0x02
#define FLASH_WRITE_RSP 0x82

/** Write to flash. Request structure.
 */
struct flash_write_req {
    uint32_t address; /**< Sector address where write should start */
    uint32_t length; /**< Number of bytes to write */
    uint8_t data[0]; /**< Data to write to flash */
};

static inline struct packet *
alloc_flash_write_req(struct flash_write_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_write_req(const uint32_t address, const uint32_t length, const uint8_t data[0])
{
    struct flash_write_req *req;
    struct packet *pkt = alloc_flash_write_req(&req);
    if (pkt) {
        req->address = address;
        req->length = length;
        /* data must be set by the caller */
    }
    return pkt;
}

/** Write to flash. Response structure.
 */
struct flash_write_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_write_rsp(struct flash_write_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_write_rsp(const int32_t status)
{
    struct flash_write_rsp *rsp;
    struct packet *pkt = alloc_flash_write_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message enroll
 */

#define FLASH_ENROLL_REQ 0x03
#define FLASH_ENROLL_RSP 0x83

/** Enrollment message. Request structure.
 */
struct flash_enroll_req {
    uint8_t secureboot_mode; /**< PUF or efuse */
    uint8_t secret[33]; /**< Secret key to store in PUF */
    uint8_t ecdsa_key[64]; /**< Public key for ECDSA signature validation */
    uint8_t fw_key[32]; /**< Key for FW encryption */
};

static inline struct packet *
alloc_flash_enroll_req(struct flash_enroll_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_enroll_req(const uint8_t secureboot_mode, const uint8_t secret[33], const uint8_t ecdsa_key[64], const uint8_t fw_key[32])
{
    struct flash_enroll_req *req;
    struct packet *pkt = alloc_flash_enroll_req(&req);
    if (pkt) {
        req->secureboot_mode = secureboot_mode;
        memcpy(req->secret, secret, sizeof req->secret);
        memcpy(req->ecdsa_key, ecdsa_key, sizeof req->ecdsa_key);
        memcpy(req->fw_key, fw_key, sizeof req->fw_key);
    }
    return pkt;
}

/** Enrollment message. Response structure.
 */
struct flash_enroll_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_enroll_rsp(struct flash_enroll_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_enroll_rsp(const int32_t status)
{
    struct flash_enroll_rsp *rsp;
    struct packet *pkt = alloc_flash_enroll_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message file_mount
 */

#define FLASH_FILE_MOUNT_REQ 0x04
#define FLASH_FILE_MOUNT_RSP 0x84

/** Mount smartfs filesystem. Request structure.
 */
struct flash_file_mount_req {
    uint32_t address; /**< Sector address where filesystem starts */
    uint32_t size; /**< Size of filesystem */
};

static inline struct packet *
alloc_flash_file_mount_req(struct flash_file_mount_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_mount_req(const uint32_t address, const uint32_t size)
{
    struct flash_file_mount_req *req;
    struct packet *pkt = alloc_flash_file_mount_req(&req);
    if (pkt) {
        req->address = address;
        req->size = size;
    }
    return pkt;
}

/** Mount smartfs filesystem. Response structure.
 */
struct flash_file_mount_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_file_mount_rsp(struct flash_file_mount_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_mount_rsp(const int32_t status)
{
    struct flash_file_mount_rsp *rsp;
    struct packet *pkt = alloc_flash_file_mount_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message file_open
 */

#define FLASH_FILE_OPEN_REQ 0x05
#define FLASH_FILE_OPEN_RSP 0x85

/** Open file. Request structure.
 */
struct flash_file_open_req {
    uint32_t mode; /**< File mode */
    uint8_t path[0]; /**< File path */
};

static inline struct packet *
alloc_flash_file_open_req(struct flash_file_open_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_open_req(const uint32_t mode, const uint8_t path[0])
{
    struct flash_file_open_req *req;
    struct packet *pkt = alloc_flash_file_open_req(&req);
    if (pkt) {
        req->mode = mode;
        /* path must be set by the caller */
    }
    return pkt;
}

/** Open file. Response structure.
 */
struct flash_file_open_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_file_open_rsp(struct flash_file_open_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_open_rsp(const int32_t status)
{
    struct flash_file_open_rsp *rsp;
    struct packet *pkt = alloc_flash_file_open_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message file_remove
 */

#define FLASH_FILE_REMOVE_REQ 0x06
#define FLASH_FILE_REMOVE_RSP 0x86

/** Open file. Request structure.
 */
struct flash_file_remove_req {
    uint32_t path_len; /**< Length of path */
    uint8_t path[0]; /**< File path */
};

static inline struct packet *
alloc_flash_file_remove_req(struct flash_file_remove_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_remove_req(const uint32_t path_len, const uint8_t path[0])
{
    struct flash_file_remove_req *req;
    struct packet *pkt = alloc_flash_file_remove_req(&req);
    if (pkt) {
        req->path_len = path_len;
        /* path must be set by the caller */
    }
    return pkt;
}

/** Open file. Response structure.
 */
struct flash_file_remove_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_file_remove_rsp(struct flash_file_remove_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_remove_rsp(const int32_t status)
{
    struct flash_file_remove_rsp *rsp;
    struct packet *pkt = alloc_flash_file_remove_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message file_read
 */

#define FLASH_FILE_READ_REQ 0x07
#define FLASH_FILE_READ_RSP 0x87

/** Read from file. Request structure.
 */
struct flash_file_read_req {
    uint32_t length; /**< Number of bytes to read */
};

static inline struct packet *
alloc_flash_file_read_req(struct flash_file_read_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_read_req(const uint32_t length)
{
    struct flash_file_read_req *req;
    struct packet *pkt = alloc_flash_file_read_req(&req);
    if (pkt) {
        req->length = length;
    }
    return pkt;
}

/** Read from file. Response structure.
 */
struct flash_file_read_rsp {
    int32_t status; /**< result status, zero is success */
    uint8_t data[0]; /**< Data read from flash */
};

static inline struct packet *
alloc_flash_file_read_rsp(struct flash_file_read_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_read_rsp(const int32_t status, const uint8_t data[0])
{
    struct flash_file_read_rsp *rsp;
    struct packet *pkt = alloc_flash_file_read_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        /* data must be set by the caller */
    }
    return pkt;
}
/*
 * Message file_write
 */

#define FLASH_FILE_WRITE_REQ 0x08
#define FLASH_FILE_WRITE_RSP 0x88

/** Write to file. Request structure.
 */
struct flash_file_write_req {
    uint32_t length; /**< Number of bytes to write */
    uint8_t data[0]; /**< Data to write to flash */
};

static inline struct packet *
alloc_flash_file_write_req(struct flash_file_write_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_write_req(const uint32_t length, const uint8_t data[0])
{
    struct flash_file_write_req *req;
    struct packet *pkt = alloc_flash_file_write_req(&req);
    if (pkt) {
        req->length = length;
        /* data must be set by the caller */
    }
    return pkt;
}

/** Write to file. Response structure.
 */
struct flash_file_write_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_file_write_rsp(struct flash_file_write_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_write_rsp(const int32_t status)
{
    struct flash_file_write_rsp *rsp;
    struct packet *pkt = alloc_flash_file_write_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message file_close
 */

#define FLASH_FILE_CLOSE_REQ 0x09
#define FLASH_FILE_CLOSE_RSP 0x89

/** Close file. Request structure.
 */
struct flash_file_close_req {
};

static inline struct packet *
alloc_flash_file_close_req(struct flash_file_close_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_close_req(void)
{
    struct flash_file_close_req *req;
    struct packet *pkt = alloc_flash_file_close_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Close file. Response structure.
 */
struct flash_file_close_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_file_close_rsp(struct flash_file_close_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_close_rsp(const int32_t status)
{
    struct flash_file_close_rsp *rsp;
    struct packet *pkt = alloc_flash_file_close_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message file_stat
 */

#define FLASH_FILE_STAT_REQ 0x0a
#define FLASH_FILE_STAT_RSP 0x8a

/** Return some info about file. Request structure.
 */
struct flash_file_stat_req {
    uint32_t path_len; /**< Length of path */
    uint8_t path[0]; /**< File path */
};

static inline struct packet *
alloc_flash_file_stat_req(struct flash_file_stat_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_stat_req(const uint32_t path_len, const uint8_t path[0])
{
    struct flash_file_stat_req *req;
    struct packet *pkt = alloc_flash_file_stat_req(&req);
    if (pkt) {
        req->path_len = path_len;
        /* path must be set by the caller */
    }
    return pkt;
}

/** Return some info about file. Response structure.
 */
struct flash_file_stat_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t mode; /**< Some info about file */
};

static inline struct packet *
alloc_flash_file_stat_rsp(struct flash_file_stat_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_stat_rsp(const int32_t status, const uint32_t mode)
{
    struct flash_file_stat_rsp *rsp;
    struct packet *pkt = alloc_flash_file_stat_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->mode = mode;
    }
    return pkt;
}
/*
 * Message file_dir_open
 */

#define FLASH_FILE_DIR_OPEN_REQ 0x0b
#define FLASH_FILE_DIR_OPEN_RSP 0x8b

/** Open a directory. Request structure.
 */
struct flash_file_dir_open_req {
    uint8_t path[0]; /**< File path */
};

static inline struct packet *
alloc_flash_file_dir_open_req(struct flash_file_dir_open_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_dir_open_req(const uint8_t path[0])
{
    struct flash_file_dir_open_req *req;
    struct packet *pkt = alloc_flash_file_dir_open_req(&req);
    if (pkt) {
        /* path must be set by the caller */
    }
    return pkt;
}

/** Open a directory. Response structure.
 */
struct flash_file_dir_open_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_file_dir_open_rsp(struct flash_file_dir_open_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_dir_open_rsp(const int32_t status)
{
    struct flash_file_dir_open_rsp *rsp;
    struct packet *pkt = alloc_flash_file_dir_open_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message file_dir_read
 */

#define FLASH_FILE_DIR_READ_REQ 0x0c
#define FLASH_FILE_DIR_READ_RSP 0x8c

/** Read a entry from the opened directory. Request structure.
 */
struct flash_file_dir_read_req {
};

static inline struct packet *
alloc_flash_file_dir_read_req(struct flash_file_dir_read_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_dir_read_req(void)
{
    struct flash_file_dir_read_req *req;
    struct packet *pkt = alloc_flash_file_dir_read_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Read a entry from the opened directory. Response structure.
 */
struct flash_file_dir_read_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t mode; /**< Some info about file */
    uint8_t data[0]; /**< Data read from flash */
};

static inline struct packet *
alloc_flash_file_dir_read_rsp(struct flash_file_dir_read_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_file_dir_read_rsp(const int32_t status, const uint32_t mode, const uint8_t data[0])
{
    struct flash_file_dir_read_rsp *rsp;
    struct packet *pkt = alloc_flash_file_dir_read_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->mode = mode;
        /* data must be set by the caller */
    }
    return pkt;
}
/*
 * Message hash
 */

#define FLASH_HASH_REQ 0x0d
#define FLASH_HASH_RSP 0x8d

/** calculate hash of flash. Request structure.
 */
struct flash_hash_req {
    uint32_t address; /**< Sector address to start reading from */
    uint32_t length; /**< Number of bytes to read */
};

static inline struct packet *
alloc_flash_hash_req(struct flash_hash_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_hash_req(const uint32_t address, const uint32_t length)
{
    struct flash_hash_req *req;
    struct packet *pkt = alloc_flash_hash_req(&req);
    if (pkt) {
        req->address = address;
        req->length = length;
    }
    return pkt;
}

/** calculate hash of flash. Response structure.
 */
struct flash_hash_rsp {
    int32_t status; /**< result status, zero is success */
    uint8_t data[0]; /**< Calculated hash of flash */
};

static inline struct packet *
alloc_flash_hash_rsp(struct flash_hash_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_hash_rsp(const int32_t status, const uint8_t data[0])
{
    struct flash_hash_rsp *rsp;
    struct packet *pkt = alloc_flash_hash_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        /* data must be set by the caller */
    }
    return pkt;
}
/*
 * Message boot
 */

#define FLASH_BOOT_REQ 0x0e
#define FLASH_BOOT_RSP 0x8e

/** Write to sram. Request structure.
 */
struct flash_boot_req {
    uint32_t address; /**< SRAM address where write should start */
    uint32_t length; /**< Number of bytes to write */
    uint8_t data[0]; /**< Data to write to ram */
};

static inline struct packet *
alloc_flash_boot_req(struct flash_boot_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_boot_req(const uint32_t address, const uint32_t length, const uint8_t data[0])
{
    struct flash_boot_req *req;
    struct packet *pkt = alloc_flash_boot_req(&req);
    if (pkt) {
        req->address = address;
        req->length = length;
        /* data must be set by the caller */
    }
    return pkt;
}

/** Write to sram. Response structure.
 */
struct flash_boot_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_boot_rsp(struct flash_boot_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_boot_rsp(const int32_t status)
{
    struct flash_boot_rsp *rsp;
    struct packet *pkt = alloc_flash_boot_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message flush
 */

#define FLASH_FLUSH_REQ 0x0f
#define FLASH_FLUSH_RSP 0x8f

/** Flushes any unsaved state to flash. Request structure.
 */
struct flash_flush_req {
};

static inline struct packet *
alloc_flash_flush_req(struct flash_flush_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_flush_req(void)
{
    struct flash_flush_req *req;
    struct packet *pkt = alloc_flash_flush_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Flushes any unsaved state to flash. Response structure.
 */
struct flash_flush_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_flush_rsp(struct flash_flush_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_flush_rsp(const int32_t status)
{
    struct flash_flush_rsp *rsp;
    struct packet *pkt = alloc_flash_flush_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message get_hwaddr
 */

#define FLASH_GET_HWADDR_REQ 0x10
#define FLASH_GET_HWADDR_RSP 0x90

/** Retreives the hardware address stored in flash. Request structure.
 */
struct flash_get_hwaddr_req {
};

static inline struct packet *
alloc_flash_get_hwaddr_req(struct flash_get_hwaddr_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_get_hwaddr_req(void)
{
    struct flash_get_hwaddr_req *req;
    struct packet *pkt = alloc_flash_get_hwaddr_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Retreives the hardware address stored in flash. Response structure.
 */
struct flash_get_hwaddr_rsp {
    int32_t status; /**< result status, zero is success */
    uint8_t hwaddr[6]; /**< Hardware address stored in flash. */
};

static inline struct packet *
alloc_flash_get_hwaddr_rsp(struct flash_get_hwaddr_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_get_hwaddr_rsp(const int32_t status, const uint8_t hwaddr[6])
{
    struct flash_get_hwaddr_rsp *rsp;
    struct packet *pkt = alloc_flash_get_hwaddr_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        memcpy(rsp->hwaddr, hwaddr, sizeof rsp->hwaddr);
    }
    return pkt;
}
/*
 * Message set_hwaddr
 */

#define FLASH_SET_HWADDR_REQ 0x11
#define FLASH_SET_HWADDR_RSP 0x91

/** Stores hardware address to flash. Request structure.
 */
struct flash_set_hwaddr_req {
    uint8_t hwaddr[6]; /**< Hardware address to store in flash. */
};

static inline struct packet *
alloc_flash_set_hwaddr_req(struct flash_set_hwaddr_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_set_hwaddr_req(const uint8_t hwaddr[6])
{
    struct flash_set_hwaddr_req *req;
    struct packet *pkt = alloc_flash_set_hwaddr_req(&req);
    if (pkt) {
        memcpy(req->hwaddr, hwaddr, sizeof req->hwaddr);
    }
    return pkt;
}

/** Stores hardware address to flash. Response structure.
 */
struct flash_set_hwaddr_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_set_hwaddr_rsp(struct flash_set_hwaddr_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_set_hwaddr_rsp(const int32_t status)
{
    struct flash_set_hwaddr_rsp *rsp;
    struct packet *pkt = alloc_flash_set_hwaddr_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message get_uuid
 */

#define FLASH_GET_UUID_REQ 0x12
#define FLASH_GET_UUID_RSP 0x92

/** Retreives the UUID stored in flash. Request structure.
 */
struct flash_get_uuid_req {
};

static inline struct packet *
alloc_flash_get_uuid_req(struct flash_get_uuid_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_get_uuid_req(void)
{
    struct flash_get_uuid_req *req;
    struct packet *pkt = alloc_flash_get_uuid_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Retreives the UUID stored in flash. Response structure.
 */
struct flash_get_uuid_rsp {
    int32_t status; /**< result status, zero is success */
    uint8_t uuid[16]; /**< UUID stored in flash. */
};

static inline struct packet *
alloc_flash_get_uuid_rsp(struct flash_get_uuid_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_get_uuid_rsp(const int32_t status, const uint8_t uuid[16])
{
    struct flash_get_uuid_rsp *rsp;
    struct packet *pkt = alloc_flash_get_uuid_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        memcpy(rsp->uuid, uuid, sizeof rsp->uuid);
    }
    return pkt;
}
/*
 * Message set_uuid
 */

#define FLASH_SET_UUID_REQ 0x13
#define FLASH_SET_UUID_RSP 0x93

/** Stores a UUID to flash. Request structure.
 */
struct flash_set_uuid_req {
    uint8_t uuid[16]; /**< UUID to store in flash. */
};

static inline struct packet *
alloc_flash_set_uuid_req(struct flash_set_uuid_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_set_uuid_req(const uint8_t uuid[16])
{
    struct flash_set_uuid_req *req;
    struct packet *pkt = alloc_flash_set_uuid_req(&req);
    if (pkt) {
        memcpy(req->uuid, uuid, sizeof req->uuid);
    }
    return pkt;
}

/** Stores a UUID to flash. Response structure.
 */
struct flash_set_uuid_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_set_uuid_rsp(struct flash_set_uuid_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_set_uuid_rsp(const int32_t status)
{
    struct flash_set_uuid_rsp *rsp;
    struct packet *pkt = alloc_flash_set_uuid_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message load_partition_table
 */

#define FLASH_LOAD_PARTITION_TABLE_REQ 0x14
#define FLASH_LOAD_PARTITION_TABLE_RSP 0x94

/** Loads partition table into RAM. To commit any changes it has to be saved. Request structure.
 */
struct flash_load_partition_table_req {
    uint32_t revert; /**< Forcefully loads table even if modified in memory. */
};

static inline struct packet *
alloc_flash_load_partition_table_req(struct flash_load_partition_table_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_load_partition_table_req(const uint32_t revert)
{
    struct flash_load_partition_table_req *req;
    struct packet *pkt = alloc_flash_load_partition_table_req(&req);
    if (pkt) {
        req->revert = revert;
    }
    return pkt;
}

/** Loads partition table into RAM. To commit any changes it has to be saved. Response structure.
 */
struct flash_load_partition_table_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_load_partition_table_rsp(struct flash_load_partition_table_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_load_partition_table_rsp(const int32_t status)
{
    struct flash_load_partition_table_rsp *rsp;
    struct packet *pkt = alloc_flash_load_partition_table_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message save_partition_table
 */

#define FLASH_SAVE_PARTITION_TABLE_REQ 0x15
#define FLASH_SAVE_PARTITION_TABLE_RSP 0x95

/** Saves a previously loaded partition table, along with any changes. Request structure.
 */
struct flash_save_partition_table_req {
};

static inline struct packet *
alloc_flash_save_partition_table_req(struct flash_save_partition_table_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_save_partition_table_req(void)
{
    struct flash_save_partition_table_req *req;
    struct packet *pkt = alloc_flash_save_partition_table_req(&req);
    if (pkt) {
    }
    return pkt;
}

/** Saves a previously loaded partition table, along with any changes. Response structure.
 */
struct flash_save_partition_table_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_save_partition_table_rsp(struct flash_save_partition_table_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_save_partition_table_rsp(const int32_t status)
{
    struct flash_save_partition_table_rsp *rsp;
    struct packet *pkt = alloc_flash_save_partition_table_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message get_partition
 */

#define FLASH_GET_PARTITION_REQ 0x16
#define FLASH_GET_PARTITION_RSP 0x96

/** Retreives information about single partition. Request structure.
 */
struct flash_get_partition_req {
    uint32_t index; /**< Index to retreive. */
};

static inline struct packet *
alloc_flash_get_partition_req(struct flash_get_partition_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_get_partition_req(const uint32_t index)
{
    struct flash_get_partition_req *req;
    struct packet *pkt = alloc_flash_get_partition_req(&req);
    if (pkt) {
        req->index = index;
    }
    return pkt;
}

/** Retreives information about single partition. Response structure.
 */
struct flash_get_partition_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t type; /**< Partition type */
    uint32_t sect_start; /**< Starting sector number */
    uint32_t sect_count; /**< Sector count */
};

static inline struct packet *
alloc_flash_get_partition_rsp(struct flash_get_partition_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_get_partition_rsp(const int32_t status, const uint32_t type, const uint32_t sect_start, const uint32_t sect_count)
{
    struct flash_get_partition_rsp *rsp;
    struct packet *pkt = alloc_flash_get_partition_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->type = type;
        rsp->sect_start = sect_start;
        rsp->sect_count = sect_count;
    }
    return pkt;
}
/*
 * Message add_partition
 */

#define FLASH_ADD_PARTITION_REQ 0x17
#define FLASH_ADD_PARTITION_RSP 0x97

/** Adds a partition at the given index. Request structure.
 */
struct flash_add_partition_req {
    uint32_t index; /**< Partition index to add. */
    uint32_t type; /**< Partition type */
    uint32_t sect_start; /**< Starting sector number */
    uint32_t sect_count; /**< Sector count */
};

static inline struct packet *
alloc_flash_add_partition_req(struct flash_add_partition_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_add_partition_req(const uint32_t index, const uint32_t type, const uint32_t sect_start, const uint32_t sect_count)
{
    struct flash_add_partition_req *req;
    struct packet *pkt = alloc_flash_add_partition_req(&req);
    if (pkt) {
        req->index = index;
        req->type = type;
        req->sect_start = sect_start;
        req->sect_count = sect_count;
    }
    return pkt;
}

/** Adds a partition at the given index. Response structure.
 */
struct flash_add_partition_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_add_partition_rsp(struct flash_add_partition_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_add_partition_rsp(const int32_t status)
{
    struct flash_add_partition_rsp *rsp;
    struct packet *pkt = alloc_flash_add_partition_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message delete_partition
 */

#define FLASH_DELETE_PARTITION_REQ 0x18
#define FLASH_DELETE_PARTITION_RSP 0x98

/** Marks the partition at the given index as deleted. Request structure.
 */
struct flash_delete_partition_req {
    uint32_t index; /**< Partition index to delete. */
};

static inline struct packet *
alloc_flash_delete_partition_req(struct flash_delete_partition_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_delete_partition_req(const uint32_t index)
{
    struct flash_delete_partition_req *req;
    struct packet *pkt = alloc_flash_delete_partition_req(&req);
    if (pkt) {
        req->index = index;
    }
    return pkt;
}

/** Marks the partition at the given index as deleted. Response structure.
 */
struct flash_delete_partition_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_delete_partition_rsp(struct flash_delete_partition_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_delete_partition_rsp(const int32_t status)
{
    struct flash_delete_partition_rsp *rsp;
    struct packet *pkt = alloc_flash_delete_partition_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message erase_by_sector
 */

#define FLASH_ERASE_BY_SECTOR_REQ 0x19
#define FLASH_ERASE_BY_SECTOR_RSP 0x99

/** Erase the chip by sector number. Request structure.
 */
struct flash_erase_by_sector_req {
    uint32_t address; /**< Sector number between 1 to 511 */
};

static inline struct packet *
alloc_flash_erase_by_sector_req(struct flash_erase_by_sector_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_flash_erase_by_sector_req(const uint32_t address)
{
    struct flash_erase_by_sector_req *req;
    struct packet *pkt = alloc_flash_erase_by_sector_req(&req);
    if (pkt) {
        req->address = address;
    }
    return pkt;
}

/** Erase the chip by sector number. Response structure.
 */
struct flash_erase_by_sector_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_flash_erase_by_sector_rsp(struct flash_erase_by_sector_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_flash_erase_by_sector_rsp(const int32_t status)
{
    struct flash_erase_by_sector_rsp *rsp;
    struct packet *pkt = alloc_flash_erase_by_sector_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/* [[[end]]] */
/** @} */
