#pragma once


#define EPERM                1    /* Operation not permitted */
#define ENOENT               2    /* No such file or directory */
#define ESRCH                3    /* No such process */
#define EINTR                4    /* Interrupted system call */
#define EIO                  5    /* Input/output error */
#define ENXIO                6    /* Device not configured */
#define E2BIG                7    /* Argument list too long */
#define ENOEXEC              8    /* Exec format error */
#define EBADF                9    /* Bad file descriptor */
#define ECHILD               10   /* No child processes */
#define EDEADLK              11   /* Resource deadlock avoided */
#define ENOMEM               12   /* Cannot allocate memory */
#define EACCES               13   /* Permission denied */
#define EFAULT               14   /* Bad address */
#define ENOTBLK              15   /* Block device required */
#define EBUSY                16   /* Device busy */
#define EEXIST               17   /* File exists */
#define EXDEV                18   /* Cross-device link */
#define ENODEV               19   /* Operation not supported by device */
#define ENOTDIR              20   /* Not a directory */
#define EISDIR               21   /* Is a directory */
#define EINVAL               22   /* Invalid argument */
#define ENFILE               23   /* Too many open files in system */
#define EMFILE               24   /* Too many open files */
#define ENOTTY               25   /* Inappropriate ioctl for device */
#define ETXTBSY              26   /* Text file busy */
#define EFBIG                27   /* File too large */
#define ENOSPC               28   /* No space left on device */
#define ESPIPE               29   /* Illegal seek */
#define EROFS                30   /* Read-only file system */
#define EMLINK               31   /* Too many links */
#define EPIPE                32   /* Broken pipe */
#define EDOM                 33   /* Numerical argument out of domain */
#define ERANGE               34   /* Result too large or too small */
#define EAGAIN               35   /* Resource temporarily unavailable */
#define EWOULDBLOCK EAGAIN
#define EINPROGRESS          36   /* Operation now in progress */
#define EALREADY             37   /* Operation already in progress */
#define ENOTSOCK             38   /* Socket operation on non-socket */
#define EDESTADDRREQ         39   /* Destination address required */
#define EMSGSIZE             40   /* Message too long */
#define EPROTOTYPE           41   /* Protocol wrong type for socket */
#define ENOPROTOOPT          42   /* Protocol option not available */
#define EPROTONOSUPPORT      43   /* Protocol not supported */
#define EOPNOTSUPP           45   /* Operation not supported */

#define EAFNOSUPPORT         47   /* Address family not supported by protocol family */
#define EADDRINUSE           48   /* Address already in use */
#define EADDRNOTAVAIL        49   /* Can't assign requested address */
#define ENETDOWN             50   /* Network is down */
#define ENETUNREACH          51   /* Network is unreachable */
#define ENETRESET            52   /* Network dropped connection on reset */
#define ECONNABORTED         53   /* Software caused connection abort */
#define ECONNRESET           54   /* Connection reset by peer */
#define ENOBUFS              55   /* No buffer space available */
#define EISCONN              56   /* Socket is already connected */
#define ENOTCONN             57   /* Socket is not connected */
#define ETIMEDOUT            60   /* Operation timed out */
#define ECONNREFUSED         61   /* Connection refused */
#define ELOOP                62   /* Too many levels of symbolic links */
#define ENAMETOOLONG         63   /* File name too long */

#define EHOSTUNREACH         65   /* No route to host */
#define ENOTEMPTY            66   /* Directory not empty */

#define EDQUOT               69   /* Disc quota exceeded */
#define ESTALE               70   /* Stale NFS file handle */

#define ENOLCK               77   /* No locks available */
#define ENOSYS               78   /* Function not implemented */

#define EIDRM                82   /* Identifier removed */
#define ENOMSG               83   /* No message of desired type */
#define EOVERFLOW            84   /* Value too large to be stored in data type */
#define EILSEQ               85   /* Illegal byte sequence */
#define ENOTSUP              86   /* Not supported */
#define ECANCELED            87   /* Operation canceled */
#define EBADMSG              88   /* Bad or Corrupt message */
#define ENODATA              89   /* No message available */
#define ENOSR                90   /* No STREAM resources */
#define ENOSTR               91   /* Not a STREAM */
#define ETIME                92   /* STREAM ioctl timeout */

#define EMULTIHOP            94   /* Multihop attempted */
#define ENOLINK              95   /* Link has been severed */
#define EPROTO               96   /* Protocol error */

#define EOWNERDEAD           97   /* Previous owner died */
#define ENOTRECOVERABLE      98   /* State not recoverable */

int* _get_errno_ptr(void);
#define errno (*_get_errno_ptr())

static inline void set_errno(int e)
{
    errno = e;
}
