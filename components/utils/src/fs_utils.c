/**
******************************************************************************
* @file fs_utils.c
* @brief This file is part of utilities module.
******************************************************************************
* @attention
*
*
* Copyright (c) 2020, InnoPhase, Inc.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

/**
* @ingroup utils
* @{
*
* This file implements and provides APIs for file system utility functions like
* mounting root fs, getting the file size etc
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/os.h>
#include <kernel/page_alloc.h>
#include <kernel/spi.h>
#include <kernel/spi-mem.h>
#include <kernel/hwreg.h>
#include <kernel/elf.h>
#include <kernel/boot.h>
#include <kernel/uart.h>
#include <kernel/secureboot.h>
#include <kernel/packet.h>
#include <kernel/uart.h>
#include <kernel/flash.h>
#include <block/mtd.h>
#include <sys/mount.h>
#include <json/basic_json/json.h>
#include <dirent.h>
#include "utils/inc/utils.h"
#include "utils/inc/fs_utils.h"
#include "utils/inc/fs_defines.h"
#include "checksum/inc/fletcher32.h"


#define MOUNT_NAME "/root"
#define MEDIA_PART_TYPE_DATA 15

enum{
    JSON_PARSE_STATE_KEY,
    JSON_PARSE_STATE_SEPRATOR,
    JSON_PARSE_STATE_VAL
};

int
utils_mount_rootfs(void)
{
    return os_mount(MEDIA_PART_TYPE_DATA, MOUNT_NAME, NULL);
}

int
utils_umount_rootfs(void)
{
    return umount("/root");
}

int
utils_is_file_present(const char * path)
{
    FILE *f;    
    f = fopen(path, "r");
     if(NULL == f) {
         utils_printf("couldnot open file: %s\n", path);
         return 0;
     }
     fclose(f);
     return 1;
}

int
utils_file_size_get(const char * path)
{
    FILE *f;
    int size;

    f = fopen(path, "r");
    if(NULL == f) {
        utils_printf("couldnot open file: %s\n", path);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    fclose(f);
    return size;
}


char *
utils_file_get(const char * path, int *len)
{

    FILE *f;
    int fileSize, readBytes;
    char *buff = NULL;
    utils_printf("%s : Getting the file %s\n", __FUNCTION__, path);

    fileSize = utils_file_size_get(path);
    if(fileSize <= 0) {
        return NULL;
    }

    f = fopen(path, "r");
    if(f == NULL) {
        utils_printf("Opening the file %s : Failed\n", path);
        return NULL;
    }

    buff = (char *)os_alloc(fileSize + 1);
    if(NULL == buff) {
        fclose(f);
        os_printf("malloc of size %d Failed\n", fileSize + 1);
        return NULL;

    }

    readBytes = fread(buff, 1, fileSize, f);
    utils_printf("Reading the file %s : %d %d \n", path, readBytes, fileSize);
    if(readBytes != fileSize) {
        fclose(f);
        os_free(buff);
        utils_printf("Reading the file %s : Failed\n", path);
        return NULL;
    }
    utils_printf("successfully opened the file %s\n", path);
    fclose(f);
    utils_printf("utils_file_get: Exit\n");
    buff[fileSize] = 0;
    *len = fileSize + 1;
    return buff;
}	

/** @internal
 *
 * Store file in root fs
 */
int 
utils_file_store(const char *file_path, char *buff, int len)
{
    FILE *f;
    int bytes_written;
    int ret = 0;

    /* open file with file name in root FS*/
    f = fopen(file_path, "w");
    if(NULL == f){
        utils_printf("\nError: Failed to open %s", file_path);
        return -1;
    }
    /* write buffer content in to file*/
    bytes_written = fwrite(buff, 1, len, f);
    
    if(bytes_written != len){
        utils_printf("\nError: %s :Failed to write %d bytes. written = %d", 
                    file_path, len, bytes_written);
        ret = -1;
    }
    /* close the file*/
    fclose(f);

    return ret;
}


int
utils_file_clone(const char *dst_path, const char *src_path)
{
    FILE *f_src, *f_dst;
    int size, nread;
    char *buff;

    utils_printf("\n%s: %s to %s", __FUNCTION__, src_path, dst_path);
    f_src = fopen(src_path, "r");
    if(f_src == NULL){
        utils_printf("\nError: Failed opening src = %s\n", src_path);
        return -1;
    }    
    /*Get file size*/
    fseek(f_src, 0, SEEK_END);
    size = ftell(f_src);
    fseek(f_src, 0, SEEK_SET);
    /*Read src file*/
    buff = os_alloc(size + 1);
    if(NULL == buff){
        utils_printf("\nError: malloc failed @%d\n", __LINE__);
        fclose(f_src);
        return -1;
    }
    nread = fread(buff, 1, size, f_src);
    if(nread != size){
        utils_printf("\nError: read error - %s\n", src_path);
        fclose(f_src);
        return -1;
    }
    fclose(f_src);
    buff[size] = 0;
    /*Write in to dst file. "w" - Creates if its not present*/
    f_dst = fopen(dst_path, "w");
    if(f_dst == NULL){
        utils_printf("\nError: Failed opening dst = %s\n", dst_path);
        return -1;
    }    
    fwrite(buff, 1, size, f_dst);
    fclose(f_dst); 

    return 0;
}

/** @internal
 *
 * Select the area in flash where in the firmware image is 
 * downloaded
 */
int 
utils_create_checksum_file(const char *file_path, const char *chksum_file_path)
{
    unsigned int f_chksum;
    char chksum_str[16];
    int rval;
    
    utils_printf("\nutils_create_checksum_file : %s: %s", file_path, 
                chksum_file_path);
    fletcher32_file_checksum_get(file_path, &f_chksum);
    sprintf(chksum_str, "%08x", f_chksum);
    utils_printf("\nnew checksum = %s", chksum_str);
    /* store file*/
    rval = utils_file_store(chksum_file_path, chksum_str, strlen(chksum_str));

    return rval;
}

/* It is used to create a file without any content.*/
int utils_file_touch(const char *path)
{
     FILE *f;

    f = fopen(path, "w");
    if(f == NULL){
        utils_printf("\nError: Failed opening %s\n", path);
        return -1;
    }
    fclose(f);
    return 0;
}

static int
utils_get_boot_index(void)
{
    FILE *f_part;
    struct json_parser json;
    int ch='A';
    int  boot_index = -1;
    unsigned int state = JSON_PARSE_STATE_KEY;

    /*open part.json and parse the file*/
    f_part = fopen(BOOT_FILE_PATH, "r");
    if(NULL== f_part)
        return -1;
    
    json_init(&json);

    while(1){       
        ch = fgetc(f_part);
        if(ch == EOF){
            break;
        }
        
        volatile __auto_type t = json_tokenizer(&json, ch);
        if(t == JSON_END)
            break;
        if(t == JSON_BEGIN_ARRAY) {
            state = JSON_PARSE_STATE_KEY;
            continue;
        }

        if(JSON_PARSE_STATE_KEY == state && t == JSON_STRING) {
            utils_printf("\nkey =  %s\n",json.token);
            state =JSON_PARSE_STATE_SEPRATOR;
            if (strcmp(json.token, "boot_index") == 0) {
                /*index_found = 1;*/
            } 
            continue;
        }

        if(JSON_PARSE_STATE_SEPRATOR == state && t == JSON_MEMBER_SEPARATOR) {
            state = JSON_PARSE_STATE_VAL;
            continue;
        }

        if(state == JSON_PARSE_STATE_VAL && t == JSON_NUMBER) {
            utils_printf("num val : %d\n",atoi(json.token));                
            boot_index = atoi(json.token);
            /*got the boot index*/
            state = JSON_PARSE_STATE_KEY;
            break;
        }
    }

    json_finish(&json);
    fclose(f_part);
    return boot_index;
}


/*This will add "/root/p<image index number>/" to the supplied path.
e.g : if the path is "ca.pem" the current boot index is 0,
absolute path will be "/root/p0/ca.pem" */
int utils_file_absolute_path_get(char *path, char *abs_path_out)
{
    char index_str[16];
    int boot_index = utils_get_boot_index();
    if(boot_index < 0)
        return -1;
    /*parse boot file*/
    sprintf(index_str, "%d", boot_index);
    *abs_path_out = '\0';
    strcat(abs_path_out,"/root/p");
    strcat(abs_path_out,index_str);
    strcat(abs_path_out,"/");
    strcat(abs_path_out,path);
    return 0;
}
/**
 * This API shall be used to store fota_config.json files.
 * This creates Dirty bit file before starting to store the file. 
 * It also creates corresponding checksum file thats used by ssbl for sanity
 * check
 */
int 
utils_update_fota_cfg_file(char *buf, int len)
{
    utils_printf("\n%s", __FUNCTION__);
    /* Create dirty bit file*/
    utils_file_touch(DIRTY_BIT_FILE_PATH);
    /*store file*/
    utils_file_store(FOTA_CFG_FILE_PATH, buf, len);    
    /*Create checksum file*/
    utils_create_checksum_file(FOTA_CFG_FILE_PATH, FOTA_CFG_CHKSUM_FILE_PATH);

    return 0;
}


