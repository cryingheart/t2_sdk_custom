/**
  ******************************************************************************
  * @file   fs_utils.c
  *
  * @brief  filesystem utility functions
  *
  ******************************************************************************
  * @attention
  *
  *
  *  Copyright (c) 2021, InnoPhase, Inc.
  *
  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  *  POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <string.h>
#include <kernel/os.h>
#include <kernel/flash.h>
#include <sys/stat.h>
#include <dirent.h>

#include "fs_utils.h"

FILE *flash_fp;
DIR  *flash_dp;

int mount_ufs()
{
    return os_mount(MEDIA_PART_TYPE_DATA, MOUNT_NAME, NULL);
}

int __irq flash_file_open(const char* path, uint8_t mode)
{
    if (mode == 0)
    {
        struct stat buffer;
        int file_exists = stat(path,&buffer);
        if(file_exists == 0)
        {
            flash_fp = fopen(path, "r");
            return 0;
        }
        return -1;
    }
    else
    {
        flash_fp = fopen(path, "w");
    }

    return 0;
}

void __irq flash_file_close()
{
    fclose(flash_fp);
}

int __irq flash_file_read(uint32_t length, char* data)
{
    memset(data, 0 , length);
    return fread(data, 1, length, flash_fp);
}

void __irq flash_file_gets(uint32_t length, char* data)
{
    memset(data, 0 , length);
    fgets(data, length, flash_fp);
}

void __irq flash_file_write(uint32_t length, char* data)
{
    uint32_t __unused sz_out = fwrite(data, 1, length, flash_fp);       
}
