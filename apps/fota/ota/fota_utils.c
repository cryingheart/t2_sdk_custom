/**
  ******************************************************************************
  * @file   fota_utils.c
  *
  * @brief  fota utility functions
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
#include <stdlib.h>
#include <string.h>
#include <kernel/os.h>
#include <kernel/boot.h>

#include "json_utils.h"
//#include "config.h"
#include "fs_utils.h"
#include "fota_utils.h"
#include "part.h"

/**
 * Parses a partition file in filesystem into a part_config struct
 * @param conffile filename of partition file
 * @param part_config destination part_config struct
 */
int
read_part_file(const char *conffile, struct part_config *part_config)
{
    //os_printf("read_part_file\n");
    FILE *f;
    struct big_json_parser json;
    int ch;
    void *dest=NULL;
    int  in_image = -1;
    int param_cnt = 0;
    bool param = false;
    bool common_boot_args = false;
    int common_boot_args_offset = 0;
    int key = 0;

    f = fopen(conffile, "r");
    if(f == NULL)
        return 1;

    big_json_init(&json);

    while(1) {
        ch = fgetc(f);
        volatile __auto_type t = big_json_tokenizer(&json, ch);

        switch (t) {
        case JSON_END:
            big_json_finish(&json);
            fclose(f);
            return 0;
            break;

        case JSON_LITERAL:
            dest = NULL;
            param = false;
            common_boot_args = false;
            key = 0;

            if (strcmp(json.token, "baud") == 0)
                dest = &part_config->baud;

            if (strcmp(json.token, "common") == 0)
            {
                common_boot_args = true;
                dest = &part_config->common;
            }
            if (strcmp(json.token, "timeout") == 0)
                dest = &part_config->timeout;


            if (strcmp(json.token, "verbose") == 0)
                dest = &part_config->verbose;

            if (strcmp(json.token, "image_count") == 0)
                dest = &part_config->image_count;

            if (strcmp(json.token, "insecure") == 0)
                dest = &part_config->insecure;

            if (strcmp(json.token, "fail_check") == 0)
                dest = &part_config->fail_check;

            if (strcmp(json.token, "image") == 0)
                in_image = 0;

            if (strcmp(json.token, "name") == 0)
                dest = &part_config->image_config[in_image].name;

            if (strcmp(json.token, "version") == 0)
                dest = &part_config->image_config[in_image].version;

            if (strcmp(json.token, "status") == 0)
                dest = &part_config->image_config[in_image].status;

            if (strcmp(json.token, "param") == 0)
                param = true;

            if (strcmp(json.token, "sector") == 0)
                dest = &part_config->image_config[in_image].sector;

            if (strcmp(json.token, "size") == 0)
                dest = &part_config->image_config[in_image].size;

            if (strcmp(json.token, "sector_count") == 0)
                dest = &part_config->image_config[in_image].sector_count;

            if (strcmp(json.token, "ecdsa_pub") == 0)
                key = 64;

            break;

        case JSON_END_OBJECT:
            if (in_image>=0)
            {
                in_image++;
                param_cnt = 0;
            }
            break;

        case JSON_END_ARRAY:
            in_image = -1;
            break;

        case JSON_NUMBER:
            if (dest)
                *(int*)dest = atoi(json.token);
            if (key--)
                part_config->image_config[in_image].ecdsa_pub[63-key] = strtol(json.token, NULL, 0);
            break;

        case JSON_STRING:
            if (dest)
            {
                if(common_boot_args)
                {
                    snprintf(dest+common_boot_args_offset, 128, "\"%s\", ", json.token);
                    common_boot_args_offset += strlen(json.token) + 4;
                }
                else
                {
                    strcpy(dest, json.token);
                }
            }
            if (param) {
                part_config->image_config[in_image].args[param_cnt] = os_alloc(strlen(json.token));
                strcpy(part_config->image_config[in_image].args[param_cnt], json.token);
                param_cnt++;
            }
            break;

        default:
            break;
        }
    }

    return 0;
}

/**
 * Parse boot config file and returns the read value
 */
int parse_boot_file(const char *filename)
{
    FILE *f;
    struct big_json_parser json;
    int ch;
    void *dest=NULL;

    f = fopen(filename, "r");
    if(f == NULL)
        return 1;

    big_json_init(&json);

    while(1) {
        ch = fgetc(f);
        volatile __auto_type t = big_json_tokenizer(&json, ch);

        switch (t) {
        case JSON_END:
            big_json_finish(&json);
            fclose(f);
            return -1;
            break;

        case JSON_LITERAL:
            dest = NULL;

            if (strcmp(json.token, "image") == 0)
            {
                dest = (void*)1;
            }
            break;

        case JSON_NUMBER:
            if (dest) {
                fclose(f);
                return atoi(json.token);
            }
            break;

        default:
            break;
        }
    }

    return 0;
}

/**
 * Parses a ota cponfig file in filesystem into a ota_config struct
 * @param filename filename of partition file
 * @param fota_config destination ota_config struct
 */
int parse_ota_file(const char *filename, struct ota_config* fota_config)
{
    FILE *f;
    struct big_json_parser json;
    int ch;
    void *dest=NULL;

    f = fopen(filename, "r");
    if(f == NULL)
        return 1;

    big_json_init(&json);

    while(1) {
        ch = fgetc(f);
        volatile __auto_type t = big_json_tokenizer(&json, ch);
        switch (t) {
        case JSON_END:
            big_json_finish(&json);
            fclose(f);
            return -1;
            break;

        case JSON_LITERAL:
            dest = NULL;
            if (strcmp(json.token, "ssid") == 0)
			{
                dest = &fota_config->ssid;
			}
            else if (strcmp(json.token, "passphrase") == 0)
			{
                dest = &fota_config->passphrase;
			}
            else if (strcmp(json.token, "ota_flag") == 0)
			{
                dest = &fota_config->ota_flag;
			}
			else if (strcmp(json.token, "server") == 0)
			{
                dest = &fota_config->server;
			}
            else if (strcmp(json.token, "server_port") == 0)
			{
                dest = &fota_config->server_port;
			}
			else if (strcmp(json.token, "name") == 0)
			{
                dest = &fota_config->name;
			}
			else if (strcmp(json.token, "version") == 0)
			{
                dest = &fota_config->version;
			}
            else if (strcmp(json.token, "filename") == 0)
			{
                dest = &fota_config->filename;
			}
            else if (strcmp(json.token, "filesize") == 0)
			{
                dest = &fota_config->filesize;
			}
            else if (strcmp(json.token, "username") == 0)
			{
                dest = &fota_config->username;
			}
            else if (strcmp(json.token, "password") == 0)
			{
                dest = &fota_config->password;
			}
            else if (strcmp(json.token, "hash") == 0)
			{
                dest = &fota_config->hash;
			}
            else if (strcmp(json.token, "args") == 0)
			{
                dest = &fota_config->args;
			}
            else if (strcmp(json.token, "param_count") == 0)
			{
                dest = &fota_config->param_count;
			}
            break;
        case JSON_NUMBER:
            if (dest)
                *(int*)dest = atoi(json.token);
            break;
        case JSON_STRING:
            if (dest)
            {
                strcpy(dest, json.token);
            }
            break;
        default:
            break;
        }
        
    }
    return 0;
}

int get_image_status(int image_index)
{
    struct part_config part_config;
    memset(&part_config, 0, sizeof(struct part_config));
    read_part_file("/root/part.json", &part_config);
    return part_config.image_config[image_index].status;
}

char* get_image_version(int image_index)
{
    struct part_config part_config;
    memset(&part_config, 0, sizeof(struct part_config));
    read_part_file("/root/part.json", &part_config);
    return part_config.image_config[image_index].version;
}

/**
 * Modifies an existing partition config file
 * @param conffile filename of partition file
 * @param part_config source part_config struct
 */
void modify_part_file(const char *conffile, struct part_config* part_config)
{
    char part_json[4096];
    char images_json[2048];
    memset(part_json, 0, 4096);
    memset(images_json, 0, 2048);

    int images_json_pos = 0;
    for(int i =0; i < part_config->image_count; i++)
    {
        int images_jason_len = snprintf(images_json+images_json_pos, 2048,
"    {\n\
    name    : \"%s\",\n\
    version    : \"%s\",\n\
    sector : %d,\n\
    size : %d,\n\
    sector_count : %d,\n", 
        part_config->image_config[i].name, part_config->image_config[i].version,
        part_config->image_config[i].sector, part_config->image_config[i].size,
        part_config->image_config[i].sector_count);

        images_json_pos+=images_jason_len;

        bool no_args = false;
        int arg_index = 0;
        while(!no_args)
        {
            if(part_config->image_config[i].args[arg_index] != NULL)
            {
                images_jason_len = snprintf(images_json+images_json_pos, 2048,
"    param : \"%s\"\n", part_config->image_config[i].args[arg_index]);
                images_json_pos+=images_jason_len;
            }
            else
            {
                break;
            }
            arg_index++;
        }

        images_jason_len = snprintf(images_json+images_json_pos, 1024, 
"    },\n");

        images_json_pos+=images_jason_len;
    }
    snprintf(part_json, 4096,
"{\n\
  baud    : %d,\n\
  timeout : %d,\n\
  common  : [%s],\n\
  verbose : %d,\n\
  image_count : %d\n\
  image   :\
    [\n\
%s\
    ],\n\
  insecure    : %d,\n\
  fail_check  : %d,\n\
}\n", part_config->baud, part_config->timeout, part_config->common, 2, part_config->image_count,
        images_json, 0, part_config->fail_check);

    // Open file for writing
    flash_file_open(conffile, 1);
    // Write to file
    flash_file_write(strlen(part_json), part_json);
    // Close file
    flash_file_close();
}

char boot_json[16];

/**
 * Modifies the boot config file
 * @param image_num boot image index
 */
void modify_boot_file(uint8_t image_num)
{
    memset(boot_json, 0, 16);
    snprintf(boot_json, 16, "{image : %d}\n", image_num);

    // Open file for writing
    flash_file_open("/root/boot.json", 1);
    // Write to file
    flash_file_write(strlen(boot_json), boot_json);
    // Close file
    flash_file_close();
}

char ota_json[1024];

/**
 * Modifies an existing ota config file
 * @param fota_config source ota_config struct
 */
void modify_ota_file(struct ota_config* fota_config)
{
    memset(ota_json, 0, 1024);
    snprintf(ota_json, 1024,
"{\n\
    ssid        : \"%s\",\n\
    passphrase  : \"%s\",\n\
    ota_flag    : %d,\n\
    server      : \"%s\",\n\
    server_port : \"%s\",\n\
    name        : \"%s\",\n\
    version     : \"%s\",\n\
    filename    : \"%s\",\n\
    filesize    : %d,\n\
    hash        : \"%s\",\n\
    username    : \"%s\",\n\
    password    : \"%s\",\n\
    param_count    : \"%d\",\n\
    args    : \"%s\"\n\
}\n", fota_config->ssid, fota_config->passphrase, fota_config->ota_flag, 
    fota_config->server, fota_config->server_port, fota_config->name, 
    fota_config->version, fota_config->filename, fota_config->filesize, 
    fota_config->hash, fota_config->username, fota_config->password,
    fota_config->param_count, fota_config->args);

    // Open file for writing
    flash_file_open("/root/fota.json", 1);
    // Write to file
    flash_file_write(strlen(ota_json), ota_json);
    // Close file
    flash_file_close();
}

char lastboot[4];
/**
 * Saves the index of the last succesfully booted image
 */
void set_lastboot()
{
    memset(lastboot, 0 ,4);
    sprintf(lastboot, "%d\n", parse_boot_file("/root/boot.json"));
    // Open file for writing
    flash_file_open("/root/lastboot", 1);
    // Write to file
    flash_file_write(strlen(lastboot), lastboot);
    // Close file
    flash_file_close();
}

/**
 * Sets bootfail counter to 0
 */
void bootpass()
{    
    // Open file for writing
    flash_file_open("/root/bootfail", 1);
    // Write to file
    flash_file_write(2, "0\n");
    // Close file
    flash_file_close();
    set_lastboot();
}

static int bootfail_cnt;
/**
 * increments bootfail counter
 */
void inc_bootfail()
{
    FILE *f;
    f = fopen("/root/bootfail", "w");
    if(f)
    {
        bootfail_cnt = 0;
        fscanf(f, "%d", &bootfail_cnt);
        bootfail_cnt++;
        fprintf(f, "%d\n", bootfail_cnt);
        fclose(f);
    }
}
