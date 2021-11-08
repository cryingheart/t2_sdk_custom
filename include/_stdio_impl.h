#pragma once

#include <stdio.h>

#include <kernel/compiler.h>
#include <kernel/bitops.h>
#include <kernel/os.h>

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>

#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <bits/null.h>

enum _io_orientation { _io_byte, _io_wide };

/* file mode flags */
#define _IO_F_READ     BIT(0)
#define _IO_F_WRITE    BIT(1)
#define _IO_F_TRUNCATE BIT(2)
#define _IO_F_APPEND   BIT(3)
#define _IO_F_CREATE   BIT(4)
#define _IO_F_BINARY   BIT(5)

/* dynamic flags */
#define _IO_F_EOF      BIT(8)
#define _IO_F_ERROR    BIT(9)
#define _IO_F_I        BIT(10)
#define _IO_F_O        BIT(11)

/* buffer flags */
#define _IO_F_BUFALLOC BIT(16)
#define _IO_F_BUFSET   BIT(17)

struct _io_file_ops;

typedef int (*_io_writefn_t)(FILE*, const void*, size_t);

struct _io_file {
    const struct _io_file_ops *io_fop;
    _io_writefn_t              io_writefn;
    atomic_t                   io_flags;
    struct os_semaphore        io_lock;
    unsigned char             *io_buf;
    fpos_t                     io_bufpos; /* first byte */
    size_t                     io_bufsize;
    size_t                     io_bufvalid; /* valid bytes in io_buf */
    size_t                     io_bufindex; /* index into io_buf */
    enum _io_orientation       io_orientation;
    unsigned char              io_ungetc_buf[4];
    size_t                     io_ungetc_count;
    unsigned char _Alignas(__BIGGEST_ALIGNMENT__)  io_extra[0];
    /* followed by buffer space */
};

#define _IO_FILE_INIT(_name, _flags, _fop, _writefn) {   \
        .io_fop = _fop,                             \
        .io_writefn = _writefn,                     \
        .io_flags = _flags,                         \
        .io_lock = OS_SEM_INITALIZER((_name).io_lock, 1),   \
        .io_buf = NULL,                             \
        .io_bufpos = 0,                             \
        .io_bufsize = 0,                            \
        .io_bufvalid = 0,                           \
        .io_bufindex = 0,                           \
        .io_orientation = _io_byte,                 \
        .io_ungetc_count = 0                        \
    }

static inline void _io_file_init(struct _io_file *stream,
                                 unsigned int flags,
                                 const struct _io_file_ops *fop,
                                 _io_writefn_t writefn)
{
    stream->io_fop = fop;
    stream->io_writefn = writefn;
    stream->io_flags = flags;
    os_sem_init(&stream->io_lock, 1);

    stream->io_buf = NULL;
    stream->io_bufpos = 0;
    stream->io_bufsize = 0;
    stream->io_bufvalid = 0;
    stream->io_bufindex = 0;
    stream->io_orientation = _io_byte;
    stream->io_ungetc_count = 0;
}

static inline unsigned int _io_test_flags(FILE *stream, unsigned int flags)
{
    return atomic_read(&stream->io_flags) & flags;
}

static inline void _io_set_flags(FILE *stream, unsigned int flags)
{
    atomic_bis(&stream->io_flags, flags);
}

static inline void _io_clear_flags(FILE *stream, unsigned int flags)
{
    atomic_bic(&stream->io_flags, flags);
}


struct _io_file_ops {
    ssize_t (*iof_read)(struct _io_file*, void*, size_t);
    ssize_t (*iof_write)(struct _io_file*, const void*, size_t);
    long    (*iof_seek)(struct _io_file*, long, int);
    int     (*iof_close)(struct _io_file*);
};

/* _stream may be NULL */
static inline void _io_set_errno(FILE *stream, int errnum)
{
    errno = errnum;
}

static inline int _io_write(FILE *stream, const void *data, size_t len)
{
    //assert(_io_test_flags(stream, _IO_F_I) == 0);

    _io_set_flags(stream, _IO_F_O);

    return (*stream->io_writefn)(stream, data, len);
}


/* BEGIN GENERATED PROTOS */

void _clearerr_l(FILE* stream);

int _feof_l(FILE* stream);

int _ferror_l(FILE* stream);

int _fflush_l(FILE* stream);

int _fgetc_l(FILE* stream);

char* _fgets_l(char* restrict s, int n, FILE* restrict stream);

int _fputc_l(int c, FILE* stream);

size_t
_fread_l(void* restrict ptr,
         size_t size,
         size_t nmemb,
         FILE* restrict stream);

int _fseek_l(FILE* stream, long int offset, int whence);

long int _ftell_l(FILE* stream);

size_t
_fwrite_l(const void* restrict ptr,
          size_t size,
          size_t nmemb,
          FILE* restrict stream);

struct _io_file*
_io_file_alloc(const struct _io_file_ops* fop,
               unsigned int flags,
               size_t extra_size,
               size_t buffer_size);

void _io_file_free(struct _io_file* file);

void _io_lock(FILE* fp);

unsigned int _io_parse_mode(const char* mode);

int _io_read(FILE* stream, void* dest, size_t len);

int _io_setbuf(FILE* restrict stream, char* buf, size_t size);

void _io_unlock(FILE* fp);

int _io_write_fbf(FILE* stream, const void* ptr, size_t len);

int _io_write_lbf(FILE* stream, const void* ptr, size_t len);

int _io_write_nbf(FILE* stream, const void* ptr, size_t len);

int _iop_close(struct _io_file* stream);

int _iop_flush(struct _io_file* stream);

ssize_t _iop_read(struct _io_file* stream, void* data, size_t len);

long _iop_seek(struct _io_file* stream, long offset, int whence);

ssize_t
_iop_write(struct _io_file* stream,
           const void* data,
           size_t len);

void _rewind_l(FILE* stream);

int
_setvbuf_l(FILE* restrict stream,
           char* restrict buf,
           int mode,
           size_t size);

int _ungetc_l(int c, FILE* stream);

int
_vfprintf_l(FILE* restrict stream,
            const char* restrict format,
            va_list arg)
    __attribute__((__format__(__printf__, 2, 0)));

int
_vfscanf_l(FILE* restrict stream,
           const char* restrict format,
           va_list arg)
    __attribute__((__format__(__scanf__, 2, 0)));

/* END GENERATED PROTOS */
