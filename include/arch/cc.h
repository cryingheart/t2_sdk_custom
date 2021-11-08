/**--------------------------------------------------
 * Copyright (c) 2017, InnophaseInc
 * All rights reserved.
 *
 * @authors: agonga@InnophaseInc.com,
 *
 * @description: arch/cc.h lwip2x port file
 *--------------------------------------------------*/

#ifndef LWIP_ARCH_CC_H
#define LWIP_ARCH_CC_H

/* Include some files for defining library routines */
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>
#include <kernel/endian.h>


#define LWIP_TIMEVAL_PRIVATE 0

/** Define the byte order of the system.
 * Needed for conversion of network data to host byte order.
 * Allowed values: LITTLE_ENDIAN and BIG_ENDIAN
 */
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

/* Define generic types used in lwIP */
typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   int     u32_t;
typedef signed     int     s32_t;

/* Define (sn)printf formatters for these lwIP types */
#define X8_F  "02x"
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

/* If only we could use C99 and get %zu */
#if defined(__x86_64__)
#define SZT_F "lu"
#else
#define SZT_F "u"
#endif

/* Compiler hints for packing structures */
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

/* prototypes for printf() and abort() */
#include <stdio.h>
#include <stdlib.h>
#include <kernel/os.h>
/* Plaform specific diagnostic output */
#define LWIP_PLATFORM_DIAG(x)   do {os_printf x;} while(0)

#define LWIP_PLATFORM_ASSERT(x) os_error2(OS_ERR_ASSERTION_FAILED, 0)

#define LWIP_RAND() ((u32_t)get_random_uint(0xFFFF))

struct sio_status_s;
typedef struct sio_status_s sio_status_t;
#define sio_fd_t sio_status_t*
#define __sio_fd_t_defined

/* Expand to myself to get the inline function from <errno.h> */
#define set_errno set_errno

#define lwip_htons(x) htobe16(x)
#define lwip_htonl(x) htobe32(x)

#define LWIP_CHKSUM   lwip_arm_chksum

#endif  //LWIP_ARCH_CC_H
