/* NuttX 7.30 */
/****************************************************************************
 * include/dirent.h
 *
 *   Copyright (C) 2007-2009 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __INCLUDE_DIRENT_H
#define __INCLUDE_DIRENT_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <vfs/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <limits.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* File type code for the d_type field in dirent struct.
 * Note that because of the simplified filesystem organization
 * of NuttX, an inode can be BOTH a file and a directory
 */

#define DTYPE_UNKNOWN   0         /* The file type could not be determined */
#define DTYPE_FILE      (1 << 0)  /* Bit 0: Regular file */
#define DTYPE_CHR       (1 << 1)  /* Bit 1: Character device */
#define DTYPE_BLK       (1 << 2)  /* Bit 2: Block device */
#define DTYPE_DIRECTORY (1 << 3)  /* Bit 3: Directory */
#define DTYPE_LINK      (1 << 4)  /* Bit 4: Symbolic link */

#define DIRENT_ISFILE(dtype)      (((dtype) & DTYPE_FILE) != 0)
#define DIRENT_ISCHR(dtype)       (((dtype) & DTYPE_CHR) != 0)
#define DIRENT_ISBLK(dtype)       (((dtype) & DTYPE_BLK) != 0)
#define DIRENT_ISDIRECTORY(dtype) (((dtype) & DTYPE_DIRECTORY) != 0)
#define DIRENT_ISLINK(dtype)      (((dtype) & DTYPE_LINK) != 0)

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

/* The POSIX specification requires that the caller of readdir_r provide
 * storage "large enough for a dirent with the d_name member and an array
 * of char containing at least {NAME_MAX} plus one elements.
 */

struct dirent
{
  uint8_t   d_type;             /* type of file */
  char      d_name[NAME_MAX+1]; /* filename */
};

struct fs_dirent_s;
typedef struct fs_dirent_s DIR;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/


/* POSIX-like File System Interfaces */

int        closedir(DIR *dirp);
DIR*       opendir(const char *path);
struct dirent *readdir(DIR *dirp);
void       rewinddir(DIR *dirp);
void       seekdir(DIR *dirp, off_t loc);
off_t      telldir(DIR *dirp);

#endif /* __INCLUDE_DIRENT_H */
