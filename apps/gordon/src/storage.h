/*
 * Wireless Connection Manager API
 */
#pragma once
#include <string.h>
#include <kernel/hostio.h>

/*
[[[cog
import cog, generate
generate.doxygen(generate.Emitter(cog.out), 'storage')
]]] */
/**
 * @addtogroup hioapi_storage
 * @{
 *
 * This is an interface to the filesystem on flash storage.
 * It is designed for robustness rather then efficiency.
 *
 * <ul>
 *   <li> read files </li>
 *   <li> write files </li>
 *   <li> list entry in a directory </li>
 * </ul>
 *
 * <table>
 * <caption>Message summary (group=14)</caption>
 * <tr><th>OPERATION</th><th>REQ</th><th>RSP</th><th>IND</th></tr>
 * <tr><td>file_read</td><td>0x00 - storage_file_read_req</td><td>0x80 - storage_file_read_rsp</td><td></td></tr>
 * <tr><td>file_write</td><td>0x01 - storage_file_write_req</td><td>0x81 - storage_file_write_rsp</td><td></td></tr>
 * <tr><td>file_unlink</td><td>0x02 - storage_file_unlink_req</td><td>0x82 - storage_file_unlink_rsp</td><td></td></tr>
 * <tr><td>file_mkdir</td><td>0x03 - storage_file_mkdir_req</td><td>0x83 - storage_file_mkdir_rsp</td><td></td></tr>
 * <tr><td>file_rmdir</td><td>0x04 - storage_file_rmdir_req</td><td>0x84 - storage_file_rmdir_rsp</td><td></td></tr>
 * <tr><td>file_rename</td><td>0x05 - storage_file_rename_req</td><td>0x85 - storage_file_rename_rsp</td><td></td></tr>
 * <tr><td>file_stat</td><td>0x06 - storage_file_stat_req</td><td>0x86 - storage_file_stat_rsp</td><td></td></tr>
 * <tr><td>file_dir_read</td><td>0x07 - storage_file_dir_read_req</td><td>0x87 - storage_file_dir_read_rsp</td><td></td></tr>
 * </table>
 */
/* [[[end]]] */

/*
[[[cog
cog.outl('')
generate.header(generate.Emitter(cog.out), 'storage')
cog.outl('')
]]] */

#define HIO_GROUP_STORAGE 14

/*
 * Message file_read
 */

#define STORAGE_FILE_READ_REQ 0x00
#define STORAGE_FILE_READ_RSP 0x80

/** Read from file as a stateles operation, leaving no filehandle open. Request structure.
 */
struct storage_file_read_req {
    uint32_t offset; /**< Where to start reading from */
    uint32_t length; /**< Number of bytes to read */
    char path[68]; /**< File path */
};

static inline struct packet *
alloc_storage_file_read_req(struct storage_file_read_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_read_req(const uint32_t offset, const uint32_t length, const char path[68])
{
    struct storage_file_read_req *req;
    struct packet *pkt = alloc_storage_file_read_req(&req);
    if (pkt) {
        req->offset = offset;
        req->length = length;
        strncpy(req->path, path, 68);
    }
    return pkt;
}

/** Read from file as a stateles operation, leaving no filehandle open. Response structure.
 */
struct storage_file_read_rsp {
    int32_t status; /**< result status, zero is success */
    uint8_t data[0]; /**< Data read */
};

static inline struct packet *
alloc_storage_file_read_rsp(struct storage_file_read_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_read_rsp(const int32_t status, const uint8_t data[0])
{
    struct storage_file_read_rsp *rsp;
    struct packet *pkt = alloc_storage_file_read_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        /* data must be set by the caller */
    }
    return pkt;
}
/*
 * Message file_write
 */

#define STORAGE_FILE_WRITE_REQ 0x01
#define STORAGE_FILE_WRITE_RSP 0x81

/** Write to file as a stateless operation, leaving no filehandle open. Request structure.
 */
struct storage_file_write_req {
    uint32_t mode; /**< File mode (1=wb, 2=ab */
    uint32_t offset; /**< Where to write */
    char path[68]; /**< File path */
    uint8_t data[0]; /**< Data to write */
};

static inline struct packet *
alloc_storage_file_write_req(struct storage_file_write_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_write_req(const uint32_t mode, const uint32_t offset, const char path[68], const uint8_t data[0])
{
    struct storage_file_write_req *req;
    struct packet *pkt = alloc_storage_file_write_req(&req);
    if (pkt) {
        req->mode = mode;
        req->offset = offset;
        strncpy(req->path, path, 68);
        /* data must be set by the caller */
    }
    return pkt;
}

/** Write to file as a stateless operation, leaving no filehandle open. Response structure.
 */
struct storage_file_write_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t length; /**< Number of bytes written */
};

static inline struct packet *
alloc_storage_file_write_rsp(struct storage_file_write_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_write_rsp(const int32_t status, const uint32_t length)
{
    struct storage_file_write_rsp *rsp;
    struct packet *pkt = alloc_storage_file_write_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->length = length;
    }
    return pkt;
}
/*
 * Message file_unlink
 */

#define STORAGE_FILE_UNLINK_REQ 0x02
#define STORAGE_FILE_UNLINK_RSP 0x82

/** Remove a file. Request structure.
 */
struct storage_file_unlink_req {
    char path[68]; /**< File path */
};

static inline struct packet *
alloc_storage_file_unlink_req(struct storage_file_unlink_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_unlink_req(const char path[68])
{
    struct storage_file_unlink_req *req;
    struct packet *pkt = alloc_storage_file_unlink_req(&req);
    if (pkt) {
        strncpy(req->path, path, 68);
    }
    return pkt;
}

/** Remove a file. Response structure.
 */
struct storage_file_unlink_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_storage_file_unlink_rsp(struct storage_file_unlink_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_unlink_rsp(const int32_t status)
{
    struct storage_file_unlink_rsp *rsp;
    struct packet *pkt = alloc_storage_file_unlink_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message file_mkdir
 */

#define STORAGE_FILE_MKDIR_REQ 0x03
#define STORAGE_FILE_MKDIR_RSP 0x83

/** Create a directory. Request structure.
 */
struct storage_file_mkdir_req {
    char path[68]; /**< File path */
};

static inline struct packet *
alloc_storage_file_mkdir_req(struct storage_file_mkdir_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_mkdir_req(const char path[68])
{
    struct storage_file_mkdir_req *req;
    struct packet *pkt = alloc_storage_file_mkdir_req(&req);
    if (pkt) {
        strncpy(req->path, path, 68);
    }
    return pkt;
}

/** Create a directory. Response structure.
 */
struct storage_file_mkdir_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_storage_file_mkdir_rsp(struct storage_file_mkdir_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_mkdir_rsp(const int32_t status)
{
    struct storage_file_mkdir_rsp *rsp;
    struct packet *pkt = alloc_storage_file_mkdir_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message file_rmdir
 */

#define STORAGE_FILE_RMDIR_REQ 0x04
#define STORAGE_FILE_RMDIR_RSP 0x84

/** Remove a directory. Request structure.
 */
struct storage_file_rmdir_req {
    char path[68]; /**< File path */
};

static inline struct packet *
alloc_storage_file_rmdir_req(struct storage_file_rmdir_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_rmdir_req(const char path[68])
{
    struct storage_file_rmdir_req *req;
    struct packet *pkt = alloc_storage_file_rmdir_req(&req);
    if (pkt) {
        strncpy(req->path, path, 68);
    }
    return pkt;
}

/** Remove a directory. Response structure.
 */
struct storage_file_rmdir_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_storage_file_rmdir_rsp(struct storage_file_rmdir_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_rmdir_rsp(const int32_t status)
{
    struct storage_file_rmdir_rsp *rsp;
    struct packet *pkt = alloc_storage_file_rmdir_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message file_rename
 */

#define STORAGE_FILE_RENAME_REQ 0x05
#define STORAGE_FILE_RENAME_RSP 0x85

/** Rename a file. Request structure.
 */
struct storage_file_rename_req {
    char oldpath[68]; /**< File path */
    char newpath[68]; /**< File path */
};

static inline struct packet *
alloc_storage_file_rename_req(struct storage_file_rename_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_rename_req(const char oldpath[68], const char newpath[68])
{
    struct storage_file_rename_req *req;
    struct packet *pkt = alloc_storage_file_rename_req(&req);
    if (pkt) {
        strncpy(req->oldpath, oldpath, 68);
        strncpy(req->newpath, newpath, 68);
    }
    return pkt;
}

/** Rename a file. Response structure.
 */
struct storage_file_rename_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_storage_file_rename_rsp(struct storage_file_rename_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_rename_rsp(const int32_t status)
{
    struct storage_file_rename_rsp *rsp;
    struct packet *pkt = alloc_storage_file_rename_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}
/*
 * Message file_stat
 */

#define STORAGE_FILE_STAT_REQ 0x06
#define STORAGE_FILE_STAT_RSP 0x86

/** Return file or file system status. Request structure.
 */
struct storage_file_stat_req {
    char path[68]; /**< File path */
};

static inline struct packet *
alloc_storage_file_stat_req(struct storage_file_stat_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_stat_req(const char path[68])
{
    struct storage_file_stat_req *req;
    struct packet *pkt = alloc_storage_file_stat_req(&req);
    if (pkt) {
        strncpy(req->path, path, 68);
    }
    return pkt;
}

/** Return file or file system status. Response structure.
 */
struct storage_file_stat_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t mode; /**< Some info about file */
    uint32_t size; /**< size of the file */
};

static inline struct packet *
alloc_storage_file_stat_rsp(struct storage_file_stat_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_stat_rsp(const int32_t status, const uint32_t mode, const uint32_t size)
{
    struct storage_file_stat_rsp *rsp;
    struct packet *pkt = alloc_storage_file_stat_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->mode = mode;
        rsp->size = size;
    }
    return pkt;
}
/*
 * Message file_dir_read
 */

#define STORAGE_FILE_DIR_READ_REQ 0x07
#define STORAGE_FILE_DIR_READ_RSP 0x87

/** Read an entry from a directory. Request structure.
 */
struct storage_file_dir_read_req {
    uint32_t offset; /**< index */
    char path[68]; /**< File path */
};

static inline struct packet *
alloc_storage_file_dir_read_req(struct storage_file_dir_read_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_dir_read_req(const uint32_t offset, const char path[68])
{
    struct storage_file_dir_read_req *req;
    struct packet *pkt = alloc_storage_file_dir_read_req(&req);
    if (pkt) {
        req->offset = offset;
        strncpy(req->path, path, 68);
    }
    return pkt;
}

/** Read an entry from a directory. Response structure.
 */
struct storage_file_dir_read_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t mode; /**< Some info about file */
    uint32_t size; /**< size of the file */
    uint8_t data[0]; /**< name of file or dir */
};

static inline struct packet *
alloc_storage_file_dir_read_rsp(struct storage_file_dir_read_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_storage_file_dir_read_rsp(const int32_t status, const uint32_t mode, const uint32_t size, const uint8_t data[0])
{
    struct storage_file_dir_read_rsp *rsp;
    struct packet *pkt = alloc_storage_file_dir_read_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->mode = mode;
        rsp->size = size;
        /* data must be set by the caller */
    }
    return pkt;
}

/* [[[end]]] */
/** @} */
