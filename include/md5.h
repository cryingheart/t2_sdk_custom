#pragma once
#include <stddef.h>

/*
 **********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
 **                                                                  **
 ** License to copy and use this software is granted provided that   **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message     **
 ** Digest Algorithm" in all material mentioning or referencing this **
 ** software or this function.                                       **
 **                                                                  **
 ** License is also granted to make and use derivative works         **
 ** provided that such works are identified as "derived from the RSA **
 ** Data Security, Inc. MD5 Message Digest Algorithm" in all         **
 ** material mentioning or referencing the derived work.             **
 **                                                                  **
 ** RSA Data Security, Inc. makes no representations concerning      **
 ** either the merchantability of this software or the suitability   **
 ** of this software for any particular purpose.  It is provided "as **
 ** is" without express or implied warranty of any kind.             **
 **                                                                  **
 ** These notices must be retained in any copies of any part of this **
 ** documentation and/or software.                                   **
 **********************************************************************
 */

#include <stdint.h>

#define MD5_DIGEST_LENGTH 16U

/* Data structure for MD5 (Message Digest) computation */
typedef struct {
    uint64_t      i;            /* number of _bits_ handled mod 2^64 */
    uint32_t      buf[4];       /* scratch buffer */
    unsigned char in[64];       /* input buffer */
} MD5_CTX;

void md5_init(MD5_CTX *mdContext);
void md5_update(MD5_CTX *mdContext, const void *buf, size_t inLen);
void md5_final(MD5_CTX *mdContext, unsigned char digest[MD5_DIGEST_LENGTH]);
