/**
******************************************************************************
* @file fota.c
* @brief This file is part of the fota module implimentation. This file provides
*   the fota APIs. It also implements the helper functions.
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
* @addtogroup fota
* @{
*
* This module impliments the FOTA functionality. FOTA is the Firmware 
* management technology that helps to perform wireless firmware upgrade on 
* the device. 
*/

#include <kernel/os.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <kernel/os.h>
#include <kernel/spi-mem.h>
#include <kernel/flash.h>
#include <kernel/boot.h>
#include <kernel/callout.h>
#include <kernel/io.h>
#include <kernel/hwreg.h>
#include <kernel/watchdog.h>
#include <wifi/wcm.h>
#include <block/mtd.h>
#include <sys/mount.h>
#include <unistd.h>
#include <sha2.h>

#include "http/inc/http_client.h"
#include "json/jansson/src/jansson.h"
#include "utils/inc/sector_cache.h"
#include "utils/inc/utils.h"
#include "utils/inc/fs_utils.h"
#include "utils/inc/fs_defines.h"
#include "../inc/fota.h"

/******************************************************************************
* Private definitions
*
******************************************************************************/
#define FOTA_MAX_URL_LEN            256    
#define FOTA_RECV_TIMEOUT_S         10
#define FOTA_INVALID_VERSION        -1
#define MAX_IMAGE_INFO_FOR_AN_APP   2

#define FOTA_MAX_RECV_BUFF_SIZE     SECTORSIZE

extern void boot_sha256_init(SHA256_CTX *ctx);
extern void boot_sha256_update(SHA256_CTX *ctx, const void *buf, size_t len);
extern void boot_sha256_transform(SHA256_CTX *ctx);
extern void boot_sha256_final(SHA256_CTX *ctx, uint8_t *digest);

SHA256_CTX sha256_ctx;

/******************************************************************************
* Private functions
*
******************************************************************************/
//#define FOTA_DEBUG

#ifdef FOTA_DEBUG
void fota_debug_print_files_info(fota_files_info_t *files_info_p)
{
    os_printf("\n%s\n\t%s\n\t%s\n\t%s\n\t%d\n\t%d\n\t%s\n\t%s\n\t%s", 
                files_info_p->type, 
                files_info_p->name,files_info_p->protocol,
                files_info_p->host_name, files_info_p->port ,
                files_info_p->secured, files_info_p->uri,
                files_info_p->url, files_info_p->version);
}

void fota_debug_print_image_info(fota_image_info_t *image_info_p)
{    
    os_printf("\n%s :%d",
                image_info_p->version, image_info_p->sector);
}

void fota_debug_print_file_info_list( fota_files_info_t *files_info_list)
{
    fota_files_info_t *p = files_info_list;
    os_printf("\n%s:", __FUNCTION__);
    while(p){
        os_printf("\n%s\n\t%s", p->type, p->name);
        p = p->next;
    }
}

void fota_debug_dump_file(const char * path)
{    
    FILE *f;
    int val = 'A';

    f = fopen(path, "r");
    if(NULL == f){
        os_printf("%s: Couldnot open file: %s\n", __FUNCTION__, path);
        return ;
    }
    os_printf("\n %s file len = %d content:\n", path, utils_file_size_get(path));
    while(1){
        val = fgetc(f);
        if(val == EOF)
            break;
        os_printf("%c", val);
    }
    os_printf("\n\n **** End of File ****\n");
    fclose(f);
}

#else
inline void 
fota_debug_print_files_info(fota_files_info_t *files_info_p)
{    
}

inline void 
fota_debug_print_image_info(fota_image_info_t *image_info_p)
{    
}
inline void
fota_debug_print_file_info_list( fota_files_info_t *files_info_list)
{
}
inline void fota_debug_dump_file(const char * path)
{
}
#endif

void *
fota_calloc(int size)
{
    void *p;
    p = os_alloc(size);
    if(!p)
        return NULL;
    memset(p, 0, size);
    return p;
}
/*This will add "/root/p<image index number>/" to the supplied path.
e.g : if the path is "ca.pem" the current boot index is 0,
absolute path will be "/root/p0/ca.pem" */
int fota_file_path_at_index_get(char *path, char *abs_path_out, 
                                    int image_index)
{
    char index_str[16];
    
    /*parse boot file*/
    sprintf(index_str, "%d", image_index);
    *abs_path_out = '\0';
    strcat(abs_path_out,"/root/p");
    strcat(abs_path_out,index_str);
    strcat(abs_path_out,"/");
    strcat(abs_path_out,path);
    return 0;
}

/** @internal
 *
 * Update a json object
 */
static int 
fota_json_object_update(json_t *root, char *key, void *value, json_type type)
{
    json_t *obj, *val_obj = NULL;
    if((obj =  json_object()) == NULL){
        return -1;
    }
    switch(type){
        case JSON_STRING:            
            val_obj = json_string((const char *)value);
            break;
        case JSON_INTEGER:
            val_obj = json_integer(*((int *)value));
            break;
        default:
            break;
    }
    /*TODO unref va_obj or obj or both??*/
    if(json_object_set_new(obj, key, val_obj) < 0){
        return -1;
    }
    
    if(json_object_update_existing(root, obj) < 0){
        return -1;
    }

    return 0;
}

/** @internal
 *
 * This function need to be called for each of the file/buffer to be parsed
 * @param file_path Path of the json file in root FS
 * @param json_str  String having the jason data to be parsed
 * @note Only file or string can parsed. Not both. if file_path is present that 
 *      takes the presidence.
 */
static json_t *
fota_json_init(const char *file_path, char *json_str)
{
    char *jstr = NULL;
    FILE *f;
    int file_size;
    int read_len;
    json_t *root;
    json_error_t error;

    if(NULL == file_path && NULL == json_str)
        return NULL;

    jstr = json_str;
    if(NULL != file_path){
        f = fopen(file_path, "r");
        os_printf("\n%s: %s  f = %p", __FUNCTION__, file_path, f);
        if(f == NULL){            
            return NULL;
        }
        
        /* Get file size*/
        file_size = utils_file_size_get(file_path);
        if(file_size <= 0){
            return NULL;
        }
        
        jstr = os_alloc(file_size + 1);
        if(NULL == jstr){
            os_printf("Error: malloc failed @ %d", __LINE__);
            return NULL;
        }
            
        /* read file in to a buffer*/
        read_len = fread(jstr, 1, file_size, f);
        if(read_len != file_size){
            os_printf("Error: %s - file read failed", file_path);
            os_free(jstr);
            return NULL;
        }
        
        /* [TODO]decrypt the file if encryption is enabled*/

        jstr[file_size] = '\0';
    }
        
    if(NULL == (root = json_loads(jstr, 0, &error))){
        os_printf("Error: json load failed @ %d: %s\n", error.line, error.text);        
    }
    if(NULL != file_path){
        /*jstr is mallock'ed in case of in put is a file */
        os_free(jstr);
    }
    return root;
}

/** @internal
 *
 * Parse fota.config file
 */
static char *fota_json_str_get(json_t * root, char *key)
{
    json_t *obj;
    const char *val;
    
    obj = json_object_get(root, key);
    if(!obj){
        os_printf("\nError: key not found");
        return NULL;
    }
    val = json_string_value(obj);
    os_printf("\n%s = %s", key, val);
    return (char *)val;
}
static int fota_json_int_get(json_t * root, char *key)
{
    json_t *obj;
    json_int_t val;
    
    obj = json_object_get(root, key);
    if(!obj){
        os_printf("\nError: key not found");
        return 0;
    }
    val = json_integer_value(obj);
    os_printf("\n%s = %d", key, (int)val);
    return (int)val;
}

static fota_config_t  *fota_parse_config_file(json_t * root)
{
    json_t *obj_array, *obj;
    int i, size;
    fota_files_info_t *files_info_p, *prev = NULL;
    
    fota_config_t *fota_config_p = fota_calloc(sizeof(fota_config_t));
    if(NULL== fota_config_p){
        os_printf("\n%s:%d Error malloc failure", __FUNCTION__, __LINE__);
        goto error_exit;
    }
    fota_config_p ->files_info_list = NULL;
    fota_config_p->package_version = fota_json_str_get(root, "package_version");
    os_printf("\nPackage version = %s", fota_config_p->package_version);    
    obj_array = json_object_get(root, "files");
    if(!obj_array){
        os_printf("\nError: fota.config - files obj not found");
        goto error_exit;
    }

    size = json_array_size(obj_array);
    for(i =0; i < size; i++){
        files_info_p = fota_calloc(sizeof(fota_files_info_t));
        if(NULL== files_info_p){
            os_printf("\n%s:%d Error malloc failure", __FUNCTION__, __LINE__);
            goto error_exit;
        }
        files_info_p->next = NULL;      
        
        /*Get object at index i*/
        obj = json_array_get(obj_array, i);
        files_info_p->type = fota_json_str_get(obj, "type");
        files_info_p->name = fota_json_str_get(obj, "name");
        files_info_p->version = fota_json_str_get(obj, "version");
        files_info_p->protocol = fota_json_str_get(obj, "protocol");
        files_info_p->host_name = fota_json_str_get(obj, "hostname");
        files_info_p->port = fota_json_int_get(obj, "port");
        files_info_p->secured = fota_json_int_get(obj, "secured");
        files_info_p->uri = fota_json_str_get(obj, "uri");
        files_info_p->url = fota_json_str_get(obj, "url");
        files_info_p->hash = fota_json_str_get(obj, "hash");

        fota_debug_print_files_info(files_info_p);
        /*Make a list of files info*/
        if(fota_config_p->files_info_list == NULL){
           fota_config_p->files_info_list = files_info_p;
        }else{            
            prev->next = files_info_p;
        }
        prev = files_info_p; 
    }
    
    return fota_config_p;
error_exit:
    /*Error parsing the config file. Release resources allocated if any*/
    if(fota_config_p){
        files_info_p = fota_config_p ->files_info_list;
        while(files_info_p){
            prev = files_info_p;
            files_info_p = files_info_p->next;
            os_free(prev);            
        }
        os_free(fota_config_p);
    }
    return NULL;
}

/** @internal
 *
 * Parse part.json file
 */
static int 
fota_parse_part_file(json_t * root, char *app_name,
                     fota_image_info_t image_info_table[])
{
    json_t *obj_array, *obj, *obj_name;
    int i, size, j;

    /*First invalidate all the entries*/
    for(i = 0; i < MAX_IMAGE_INFO_FOR_AN_APP; i++)
        image_info_table[i].index = -1;

    /*start parsing*/
    obj_array = json_object_get(root, "image");
    if(!obj_array){
        os_printf("\nError: part.json - image obj not found");
        return FOTA_ERROR_PART_FILE_PARSE;
    }    
    os_printf("\nImage array size = %d", json_array_size(obj_array));

    size = json_array_size(obj_array);
    for(i =0, j=0; i < size; i++){
        fota_image_info_t *image_info_p;

        /*Get object at index i*/
        obj = json_array_get(obj_array, i);

        obj_name = json_object_get(obj,"name");
        os_printf("\nname = %s", json_string_value(obj_name));
        if(strcmp(json_string_value(obj_name), app_name) != 0){
            continue;
        }
        
        image_info_p = &image_info_table[j];        
        image_info_p->version = fota_json_str_get(obj, "version"); 
        image_info_p->sector = fota_json_int_get(obj, "start_sector");
        fota_debug_print_image_info(image_info_p);
        image_info_p->index = i;
        j++;
        if(j > MAX_IMAGE_INFO_FOR_AN_APP){
            return FOTA_ERROR_PART_FILE_PARSE;
        }
    
    }
    if(j < 2){
        os_printf("\nError: Index = %d. At least 2 image info shall be present",
                    j);
        return FOTA_ERROR_PART_FILE_PARSE;
    }

    return FOTA_ERROR_NONE;
}

/** @internal
 *
 * Set the boot index to load updated firmware on next boot
 */
static int
fota_set_boot_index(fota_handle_t *f_handle)
{
    /*read boot.json file*/
    json_t * root;
    char *json_str;
    int rval;
    
    root = fota_json_init(BOOT_FILE_PATH, NULL);
    if(!root)
        return FOTA_ERROR_SET_BOOT_INDEX;

    os_printf("\nSetting next boot index = %d", f_handle->next_boot_index);
    
    if(fota_json_object_update(root, "image", &f_handle->next_boot_index, 
                                JSON_INTEGER) < 0){
        return FOTA_ERROR_SET_BOOT_INDEX;
    }

    json_str = json_dumps(root,JSON_INDENT(4));

    /* Create dirty bit file*/
    utils_file_touch(DIRTY_BIT_FILE_PATH);
    /* store file*/
    rval = utils_file_store(BOOT_FILE_PATH, json_str, strlen(json_str));
    /* Create checksum file*/
    utils_create_checksum_file(BOOT_FILE_PATH, BOOT_CHKSUM_FILE_PATH);
    return rval;
}

/** @internal
 *
 * Select the area in flash where in the firmware image is 
 * downloaded
 */
static int 
fota_update_config_file(fota_handle_t *f_handle)
{
    char *json_str;
    int rval = 0;
    if(NULL == f_handle->json_cfg_remote){
        return 0;
    }
    json_str = json_dumps(f_handle->json_cfg_remote,JSON_INDENT(4));
    
    /* Create dirty bit file*/
    utils_file_touch(DIRTY_BIT_FILE_PATH);
    /* store file*/
    rval = utils_file_store(FOTA_CFG_FILE_PATH, json_str, strlen(json_str));
    /* Create checksum file*/
    utils_create_checksum_file(FOTA_CFG_FILE_PATH, FOTA_CFG_CHKSUM_FILE_PATH);

    return rval;
}

/** @internal
 *
 * Update part.json file. This will update the version number of the
 * image info for the newly donloaded firmware.
 */
static int 
fota_update_part_file(fota_handle_t *f_handle)
{
    json_t *obj_array, *obj;
    char *json_str;
    fota_image_info_t *fota_image_info;
    int rval = 0;
    
    obj_array = json_object_get(f_handle->json_part, "image");
    if(!obj_array){
        return FOTA_ERROR_PART_FILE_UPDATE;
    }    

    fota_image_info = f_handle->image_info_list;    
    while(fota_image_info){
        obj = json_array_get(obj_array, fota_image_info->index);
        fota_json_object_update(obj, "version" ,fota_image_info->version, 
                                JSON_STRING);
        fota_image_info = fota_image_info ->next;
    }        
    json_str = json_dumps(f_handle->json_part,JSON_INDENT(4));
    /* Create dirty bit file*/
    utils_file_touch(DIRTY_BIT_FILE_PATH);
    /*store file*/
    utils_file_store(PART_FILE_PATH, json_str, strlen(json_str));    
    /*Create checksum file*/
    utils_create_checksum_file(PART_FILE_PATH, PART_CHKSUM_FILE_PATH);
    return rval;
}

/** @internal
 *
 * Select the area in flash where in the firmware image is 
 * downloaded
 */
static fota_image_info_t *
fota_select_image_area(fota_handle_t *f_handle, char *app_name)
{
    int rval;
    fota_image_info_t image_info_table[MAX_IMAGE_INFO_FOR_AN_APP];
    fota_image_info_t *image_info_p = NULL;
    int i, index_low;
    char *ver_low;
    
    rval = fota_parse_part_file(f_handle->json_part, app_name, 
                            image_info_table);
    if(rval < 0){
        return NULL;
    }

    ver_low = image_info_table[0].version;
    index_low = 0;
    for(i =1 ; i < MAX_IMAGE_INFO_FOR_AN_APP && 
                    image_info_table[i].index >= 0; i++){
        if(utils_num_str_cmp(ver_low, image_info_table[i].version) > 0){
            ver_low = image_info_table[i].version;
            index_low = i;
        }
    }

    /*Image info with lowest version shall be replaced*/
    image_info_p = fota_calloc(sizeof(fota_image_info_t ));
    if(!image_info_p){
        return NULL;
    }   
    *image_info_p = image_info_table[index_low];
    image_info_p->next = f_handle->image_info_list;
    f_handle->image_info_list = image_info_p;
    os_printf("\nimage_info_l->index = %d", image_info_p->index);

    /*return image info with lowest version*/
    return image_info_p;
}


fota_files_info_t * 
fota_cfg_file_info_get(fota_files_info_t *f_info_list)
{
    while(f_info_list){
        if(!strcmp(f_info_list->type, "configuration"))
            break;
        f_info_list = f_info_list->next;
    }
    return f_info_list;
}

/** @internal
 * Is a connection request made to same server?
 */
bool 
is_connected_to_same_server(fota_handle_t *f_handle, char *host, int port)
{
    if(!strcmp(host, f_handle->conn_info.host_name) &&
        f_handle->conn_info.port == port){
        return 1;
    }
    return 0;
}
inline static int
fota_get_path_from_url(char *url_in, char **path_out)
{    
    char *host = NULL, *path = NULL, *url=NULL;
    int port;

    os_printf("\n%s: URL provided: %s", __FUNCTION__, url_in);
    host= os_alloc(FOTA_MAX_URL_LEN);
    path= os_alloc(FOTA_MAX_URL_LEN);
    url= os_alloc(FOTA_MAX_URL_LEN);
    strcpy(url, url_in);
    http_client_url_to_host(url, host, FOTA_MAX_URL_LEN , path,
                            FOTA_MAX_URL_LEN, &port);
    *path_out = path;
    os_free(host);
    os_free(url);    
    return 0;
}

int
fota_get_host_from_url(char *url_in, char **host_out, int *port_out)
{
    char *host = NULL, *path = NULL, *url=NULL;
    int port;
    
    os_printf("\n%s: URL provided: %s", __FUNCTION__, url_in);
    host= os_alloc(FOTA_MAX_URL_LEN);
    path= os_alloc(FOTA_MAX_URL_LEN);
    url= os_alloc(FOTA_MAX_URL_LEN);
    strcpy(url, url_in);
    http_client_url_to_host(url, host, FOTA_MAX_URL_LEN , path,
                            FOTA_MAX_URL_LEN, &port);
    *host_out = host;
    *port_out = port;
    
    os_free(path);
    os_free(url);  
    return 0;
}

static int 
fota_http_connect(fota_handle_t *f_handle, fota_files_info_t *file_info)
{
    int rval;
    /* Connect to HTTP server*/
    http_client_config_t cfg = {0};
    http_client_handle_t  http_handle;
    char *host = NULL;
    int port;

    memset(&cfg, 0, sizeof(http_client_config_t));
    if(file_info->secured){
        cfg.secured = 1;
        if(file_info->secured == 1){        
            cfg.ssl_cfg.auth_mode = SSL_WRAP_VERIFY_NONE;
        }else{
            cfg.ssl_cfg.auth_mode = SSL_WRAP_VERIFY_REQUIRED;
            cfg.ssl_cfg.ca_cert.buf = utils_file_get(file_info->ca_cert, 
                                                     &cfg.ssl_cfg.ca_cert.len);
            if(NULL == cfg.ssl_cfg.ca_cert.buf){
                rval = FOTA_ERROR_NO_CA_CERT;
                goto error_exit;
            }
        }
    }
            
    if(file_info->url && strlen(file_info->url)){
        if(strlen(file_info->url) >= 256){
            rval = FOTA_ERROR_URL;
            goto error_exit;
        }
        fota_get_host_from_url(file_info->url, &host, &port);
        cfg.hostname = host;
        cfg.port = port;
        os_printf("\n%s: URL provided: %s", __FUNCTION__, file_info->url);        
    }else{
        cfg.hostname = file_info->host_name;
    }
    if(file_info->port){
        cfg.port = file_info->port;
    }        
    os_printf("\n%s:host=%s port=%d", __FUNCTION__, cfg.hostname, 
              cfg.port);    
    /*Connect to HTTP server*/
    os_printf("\nCalling http_client_open()");
    http_handle = http_client_open(&cfg);
    if(NULL == http_handle){
        os_printf("\nError: HTTP connection failed");
        rval = FOTA_ERROR_HTTP_CONNECT;
        goto error_exit;
    }
    /* Store the connection f_handle*/
    f_handle->conn_handle = (void *)http_handle;
    f_handle->conn_info.host_name = file_info->host_name;
    f_handle->conn_info.port = file_info->port;

    rval = FOTA_ERROR_NONE;
error_exit:
    if(!host)os_free(host);
    return rval;
}

int is_absolute_path(const char *path)
{
    return !strncmp(path, "/root", strlen("/root"));
}
static void 
fota_http_close(fota_handle_t *f_handle)
{
    /* Close HTTP connection*/
    http_client_close(f_handle->conn_handle);

    f_handle->conn_handle = NULL;
    memset(&f_handle->conn_info, 0, sizeof(fota_connection_info_t));
}

static void 
fota_http_cb(void * ctx, http_client_resp_info_t *resp)
{
    static int total_rcvd_len = 0;
    int copy_len, available;
#ifdef FOTA_DEBUG
    static int dot_cnt = 0;
#endif
    char abs_path[128];
    fota_files_info_t * f_info;    
    fota_handle_t *f_handle;

    if(NULL == ctx || NULL == resp){
        return;
    }
    /*Get the fota handle*/
    f_handle = (fota_handle_t *)ctx;
    /*Store status code*/
    f_handle->http_resp_status = resp->status_code;    
    if(resp->status_code != 200){
        os_printf("\n Error: HTTP response. code = %d", resp->status_code);
        return;
    }

    switch(f_handle->recv_type){
        case FOTA_RECV_TYPE_CONFIG:
            if(NULL == f_handle->recv_buff){
                /*First need to get the content length using HEAD method??- TODO*/
                f_handle->recv_buff = os_alloc(FOTA_MAX_RECV_BUFF_SIZE);
                if(NULL == f_handle->recv_buff){
                    /*TODO in http if cb returns error, abort connection*/
                    return;
                }
                f_handle->recv_index = 0;
            }
            memcpy((f_handle->recv_buff+f_handle->recv_index), resp->resp_body, 
                    resp->resp_len);
            f_handle->recv_index += resp->resp_len;
            f_handle->recv_buff[f_handle->recv_index] = '\0';

            break;
        case FOTA_RECV_TYPE_FIRMWARE:
#ifdef FOTA_DEBUG             
            os_printf(".");
            if(dot_cnt == 80){dot_cnt = 0;os_printf("\n");}
            dot_cnt++;
            total_rcvd_len += resp->resp_len;
            os_printf("\n\t%s: resp->resp_len = %d, resp->resp_total_len = %d total_rcvd_len= %d\n", 
                       __FUNCTION__, resp->resp_len, resp->resp_total_len, total_rcvd_len);
#endif
            if(NULL == f_handle->recv_buff){
                /*First need to get the content length using HEAD method??- TODO*/
                f_handle->recv_buff = os_alloc(FOTA_MAX_RECV_BUFF_SIZE);
                if(NULL == f_handle->recv_buff){
                    /*TODO in http if cb returns error, abort connection*/
                    return;
                }
                f_handle->recv_index = 0;
            }
            available = (FOTA_MAX_RECV_BUFF_SIZE - f_handle->recv_index); 
            copy_len = (available > resp->resp_len)? resp->resp_len: available;
            memcpy((f_handle->recv_buff + f_handle->recv_index), resp->resp_body, 
                    copy_len);
            f_handle->recv_index += copy_len;
            resp->resp_len -= copy_len;
            if(f_handle->recv_index >= FOTA_MAX_RECV_BUFF_SIZE ||
                (!resp->more_data && !resp->resp_len)){
                
                boot_sha256_update(&sha256_ctx, f_handle->recv_buff, 
                                   f_handle->recv_index);
                sector_cache_write(f_handle->spi_mem, 
                                   f_handle->fw_sector * SECTORSIZE, 
                                   f_handle->recv_buff , f_handle->recv_index);
                f_handle->fw_sector++;
                f_handle->recv_index = 0;                        
            }
            if(resp->resp_len && !resp->more_data){
                /*The response body is not fully copied in to recv_buff in the 
                  earlier copy as there was not enough space*/
                memcpy(f_handle->recv_buff, resp->resp_body + copy_len, 
                        resp->resp_len);
                boot_sha256_update(&sha256_ctx, f_handle->recv_buff, 
                                   resp->resp_len);                
                sector_cache_write(f_handle->spi_mem, 
                                   f_handle->fw_sector * SECTORSIZE, 
                                   f_handle->recv_buff , resp->resp_len);
                f_handle->fw_sector++;
                f_handle->recv_index = 0;
            }else if(resp->resp_len){
                memcpy(f_handle->recv_buff, resp->resp_body + copy_len, 
                        resp->resp_len);
                f_handle->recv_index = resp->resp_len;
            }
            
            if(!resp->more_data){
                /*No more data to be received. flush the cache*/
                os_printf("\nsector_cache_flush_all");
                sector_cache_flush_all(f_handle->spi_mem);
                /*input buffer is no more needed*/
                if(NULL != f_handle->recv_buff){
                    os_free(f_handle->recv_buff);
                    f_handle->recv_buff = NULL;
                }
            }
            break;
        case FOTA_RECV_TYPE_FILE:
             if(NULL == f_handle->f){
                char *f_path = NULL;
                /*open file*/                
                f_info = f_handle->curr_file_info;
                /*if the file path starts with /root, it will be stotred in the 
                  specified path. else an absolute path is derived based on the 
                  boot index of the firmware being updated*/
                os_printf("\ndownload file_name = %s", f_info->name);  
                f_path = f_info->name;
                if(0== is_absolute_path(f_info->name)){
                    fota_file_path_at_index_get(f_info->name, abs_path, 
                                                f_handle->next_boot_index);
                    f_path = abs_path;
                    os_printf("\nabsolute path = %s", abs_path);
                }
                f_handle->f = fopen(f_path, "w");
                if(NULL == f_handle->f){
                    os_printf("\n Could not open the file %s", f_path);
                    f_handle->error_http_cb = 1;
                    return;
                }                
            }
            fwrite(resp->resp_body, 1, resp->resp_len, f_handle->f);
            if(!resp->more_data){
                os_printf("\n closing the file");
                fclose(f_handle->f);
            }
            /*TODO - integrity check*/
            break;     
        case FOTA_RECV_TYPE_NONE:     
        default:
            break;
    }
}

inline static void
fota_string_to_hash(char *hash_str, unsigned char *hash_out)
{
    for (size_t count = 0; count < 32; count++) {
        sscanf(hash_str, "%2hhx", hash_out);
        hash_str += 2;
        hash_out++;
    }
}
/** @internal
 *
 * Download firmware and store in Flash
 */
static int 
fota_firmware_download(fota_handle_t *f_handle,  fota_files_info_t *file_info)
{
    int rval = FOTA_ERROR_NONE;
    int ret;
    fota_image_info_t *image_info;   
    char *path=NULL;
    uint8_t hash[32];

    /* Select Flash area using fota_select_image_area() */
    image_info = fota_select_image_area(f_handle, file_info->name);
    if(NULL == image_info){
        rval = FOTA_ERROR_PART_FILE_LOAD;
        goto error_exit;
    }
    os_printf("\nDownload the new f/w @ sector = %d\n", image_info->sector);
    /*Initialize the starting sector of the flash in to which the new firmaware
        will be downloaded*/
    f_handle->fw_sector = image_info->sector;
    /*Connect to server*/
    ret = fota_http_connect(f_handle, file_info);
    if(ret){
        rval = FOTA_ERROR_HTTP_CONNECT;
        goto error_exit;
    }    
    /*Download image*/
    if(file_info->url && strlen(file_info->url)){
        fota_get_path_from_url(file_info->url, &path);
    }else{
        path = file_info->uri;
    }
    /* Get the latest config file from remote server*/
    http_client_set_req_hdr(f_handle->conn_handle, "Host", 
                            f_handle->conn_info.host_name);
    f_handle->recv_type = FOTA_RECV_TYPE_FIRMWARE;
    ret = http_client_get(f_handle->conn_handle, path, fota_http_cb, 
                            f_handle, FOTA_RECV_TIMEOUT_S);
    f_handle->recv_type = FOTA_RECV_TYPE_NONE;
    if(ret < 0){
        rval = FOTA_ERROR_HTTP_GET;
        goto error_exit;
    }
    if(f_handle->http_resp_status != HTTP_STATUS_CODE_200_OK){
        os_printf("\nError : HTTP resp status = %d", f_handle->http_resp_status);
        rval = FOTA_ERROR_HTTP_RESP_STATUS;
        goto error_exit;
    }
    /* Download complete here*/
    os_printf("\nFw download complete");
    
    /* Check Image integrity*/    
    boot_sha256_final(&sha256_ctx, hash);
    os_printf("\nimage hash: ");
    for(int i = 0; i < 32; i++)   
        os_printf("%x", hash[i]);    
    if(file_info->hash && strlen(file_info->hash)){
        unsigned char hash_from_cfg[32];
        fota_string_to_hash(file_info->hash, hash_from_cfg);
        if(0 != memcmp(hash, hash_from_cfg, 32)){
            os_printf("\nError: Image Integrity check failed");
            rval = FOTA_ERROR_HTTP_RESP_STATUS;
            goto error_exit;
        }
    }
        
    /*All Ok*/
    image_info->version = file_info->version;
    f_handle->next_boot_index = image_info->index;
    os_printf("\n!!! New version = %s, next index = %d", image_info->version,
              f_handle->next_boot_index);
    rval = FOTA_ERROR_NONE;
error_exit:
    if(file_info->url && !path)os_free(path);
    /*Close http connection*/
    fota_http_close(f_handle);
    return rval;
}
/** @internal
 *
 * Download file and store in rootfs
 */
static int 
fota_file_download(fota_handle_t *f_handle, fota_files_info_t *file_info)
{   
    int ret;
    int rval = FOTA_ERROR_NONE;
    char *path = NULL;
    os_printf("\n%s", __FUNCTION__);    
    ret = fota_http_connect(f_handle, file_info);
       if(ret){
           rval = FOTA_ERROR_HTTP_CONNECT;
           goto error_exit;
       }
     if(file_info->url && strlen(file_info->url)){
        fota_get_path_from_url(file_info->url, &path);
    }else{
        path = file_info->uri;
    }
    /* Get the latest config file from remote server*/
    http_client_set_req_hdr(f_handle->conn_handle, "Host", 
                           f_handle->conn_info.host_name);
    f_handle->recv_type = FOTA_RECV_TYPE_FILE;
    f_handle->curr_file_info = file_info;
    f_handle->error_http_cb = 0;
    ret = http_client_get(f_handle->conn_handle, path, fota_http_cb, 
                           f_handle, FOTA_RECV_TIMEOUT_S);
    f_handle->recv_type = FOTA_RECV_TYPE_NONE;
    if(ret < 0){
       rval = FOTA_ERROR_HTTP_GET;
       goto error_exit;
    }
    if(f_handle->http_resp_status != HTTP_STATUS_CODE_200_OK ||
        f_handle->error_http_cb){
        os_printf("\nError : HTTP resp status = %d, error_http_cb = %d", 
                  f_handle->http_resp_status, f_handle->error_http_cb);
        rval = FOTA_ERROR_HTTP_RESP_STATUS;
        goto error_exit;
    }

   rval = FOTA_ERROR_NONE;
error_exit:
    if(file_info->url && !path)os_free(path);
    /*Close http connection*/
    fota_http_close(f_handle);
    return rval;
}

static int 
fota_update_check_internal(fota_handle_t *f_handle, int *update_available)
{
    int rval = FOTA_ERROR_NONE;
    int ret;
    fota_files_info_t *f_info;

    *update_available = 0;    
    f_info = fota_cfg_file_info_get(f_handle->cfg->files_info_list);
    if(NULL == f_info){
        os_printf("\nError: Configuration not preset in fota.config file");
        rval = FOTA_ERROR_NO_CONFIG_FILE;
        goto error_exit;
    }


    /* Get the latest config file from remote server*/
    http_client_set_req_hdr(f_handle->conn_handle, "Host", 
                            f_handle->conn_info.host_name);
    f_handle->recv_type = FOTA_RECV_TYPE_CONFIG;
    ret = http_client_get(f_handle->conn_handle, f_info->uri, fota_http_cb, 
                            f_handle, FOTA_RECV_TIMEOUT_S);
    f_handle->recv_type = FOTA_RECV_TYPE_NONE;
    if(ret < 0){
        rval = FOTA_ERROR_HTTP_GET;
        goto error_exit;
    }
    
    /* Parse fota configuration file just donwloaded */
    f_handle->json_cfg_remote = fota_json_init(NULL, f_handle->recv_buff);
    /*input buffer is no more needed*/
    if(NULL != f_handle->recv_buff){
        os_free(f_handle->recv_buff);
        f_handle->recv_buff = NULL;
    }
    if(NULL == f_handle->json_cfg_remote){
        os_printf("\nError: loading the remote config file");
        rval = FOTA_ERROR_PART_FILE_LOAD;
        goto error_exit;
    }
    os_printf("\nParsing Remote FOTA config file***");
    f_handle->cfg_remote = fota_parse_config_file(f_handle->json_cfg_remote);
    if(NULL == f_handle->cfg_remote){
        os_printf("\n Error parsingConfig file");
        goto error_exit;
    }
    if(utils_num_str_cmp(f_handle->cfg_remote->package_version,
                        f_handle->cfg->package_version) > 0){
        *update_available = 1;  
    }
error_exit:
    return rval;
}

/******************************************************************************
* Public functions
*
******************************************************************************/
/** 
 * Initialize fota
 *
 * Initialise the fota module.This shall be called before any other fota APIs 
 *      are used.
 *
 * @returns  Pointer to data structure fota_handle_t
 */
fota_handle_t *fota_init(void)
{
    fota_handle_t *f_handle;
    f_handle = fota_calloc(sizeof(fota_handle_t));
    if(NULL == f_handle){
        os_printf("\n Error: malloc failed @ %d", __LINE__);
        return NULL; 
    }

    /*flash initialisation*/
    f_handle->spi_mem = os_flash_get_spi_dev();
    if(NULL == f_handle->spi_mem){
        os_printf("\n Error: Get spi dev error");
        os_free(f_handle);
        return NULL;
    }
    sector_cache_init();
    
    /* Parse fota.config in the file system*/
    f_handle->json_cfg = fota_json_init(FOTA_CFG_FILE_PATH, NULL);
    if(NULL == f_handle->json_cfg){
        os_printf("\n Error: loading the config file");
        os_free(f_handle);
        return NULL;
    }
    os_printf("\nParsing rootfs FOTA config file***");
    f_handle->cfg = fota_parse_config_file(f_handle->json_cfg);
    if(NULL == f_handle->cfg){
        os_printf("\n Error: parsing Config file");
        os_free(f_handle);
        return NULL;
    }
    
    boot_sha256_init(&sha256_ctx);
    os_printf("\n Fota Init Success: %x", (int)f_handle);
    return f_handle;
}

/** 
 * Deinitialize fota
 *
 * This will release all the resources allocated during fota_init() and othe 
 *      fota API calls,
 * @param f_hadle Pointer to fota handle that was returned by fota_init()
 */
void fota_deinit(fota_handle_t * f_handle)
{    
    fota_image_info_t *img_p, *prev_img;    
    fota_files_info_t *p, *prev;

    if(NULL == f_handle)
        return;
    os_free(f_handle->recv_buff);

    img_p =  f_handle->image_info_list;
    while(img_p){
        prev_img = img_p;
        img_p = img_p->next;
        os_free(prev_img);
    }    
    if(f_handle->cfg){
        p = f_handle->cfg->files_info_list;
        while(p){
            prev = p;
            p = p->next;
            os_free(prev);
        }
        os_free(f_handle->cfg);
    }
    if(f_handle->cfg_remote){
                
        p = f_handle->cfg_remote->files_info_list;
        while(p){            
            prev = p;
            p = p->next;
            os_free(prev);
        }
        os_free(f_handle->cfg_remote);
    }
    json_decref(f_handle->json_part);
    json_decref(f_handle->json_cfg);
    json_decref(f_handle->json_cfg_remote);
    /* free f_handle*/
    os_free(f_handle);
}

/** 
 * Check for update
 *
 * Checks if a newer version of the formware package is available for update
 * @param f_hadle Pointer to fota handle that was returned by fota_init()
 * @param update_available Pointer to integer. If the new update is available
 *      or not is passed through this parameter.
 * @returns fota_error_t error code
 */
int 
fota_update_check(fota_handle_t *f_handle, int *update_available)
{
    int ret;
    fota_files_info_t *f_info;
    
    *update_available = 0; 
    
    f_info = fota_cfg_file_info_get(f_handle->cfg->files_info_list);
    if(NULL == f_info){
        os_printf("\nError: Configuration not preset in fota.config file");
        ret = FOTA_ERROR_NO_CONFIG_FILE;
        goto error_exit;
    }

    ret = fota_http_connect(f_handle, f_info);
    if(ret){
        return ret;
    }
    ret = fota_update_check_internal(f_handle, update_available);
    /*Close connection*/
    fota_http_close(f_handle);
 error_exit:    
    return ret;
}

/** 
 * Perform fota update
 *
 * Perform the fota update. This can check for the update, if update is available,
 * download the formware specified in the fota.config file.
 *
 * @param f_hadle Pointer to fota handle that was returned by fota_init()
 * @param check_for_update Check if an update is available before doing the FOTA.
 *           If this flag is set to 1, foat.config file will be fetched 
 *           from cloud and the package version of the latest file is comapred
 *           against the fota.config file present in the root FS. 
 *           The FOTA is done only if the latest version is greater than 
 *           the one preset on the device. 
 *           No check done if this flag is set to 0. 
 * @param fetch_config  Fetch latest fota.config file from the cloud before doing 
 *           FOTA. However no version comaparison is done as in case of
 *           check_for_update. Note that if check_for_update is set to 1, 
 *           fetching the latest fota.config is done automatically and 
 *           this flag is not needed. 
 *           This flag can be set only if availability of newer package 
 *           is confirned using some other mechanism (like, MQTT) and 
 *           FOTA need to be done as per the latest fota.config in the cloud. 
 * @return fota_error_t error code            
 */
int 
fota_perform(fota_handle_t *f_handle, int check_for_update,
             int fetch_config)
{
    fota_files_info_t *files_info;
    int ret = FOTA_ERROR_NONE;
    int update_available;
        
    fota_debug_print_file_info_list(f_handle->cfg->files_info_list);
    if(check_for_update)
    {
        ret = fota_update_check_internal(f_handle, &update_available);
        if(!ret || !update_available){
            return ret;
        }
    }
    /* Loop through the files list*/
    files_info = f_handle->cfg_remote->files_info_list;
    while(files_info){
        os_printf("\n type = %s", files_info->type);    
        if(!strcmp (files_info->type, "configuration")){
            files_info = files_info->next;
            continue;
        }else if(!strcmp(files_info->type, "firmware")){
            if(NULL == f_handle->json_part){
                /*parse part file, only if the firmware need to be updated*/
                f_handle->json_part = fota_json_init(PART_FILE_PATH, NULL);// TODO this is at wrong place. need to be 
                                                                            //in side select image function. also make sure that its not done again and again
                if(NULL == f_handle->json_part){
                    os_printf("\n Error loading the part file");
                    return FOTA_ERROR_PART_FILE_LOAD;
                }
            }
            ret = fota_firmware_download(f_handle, files_info);
            if(ret){
                break;
            }
        }else if(!strcmp(files_info->type, "file")){
            ret = fota_file_download(f_handle, files_info);
            if(!ret){
                break;
            }
        }
        files_info = files_info->next;
    }        
        
    return ret;
}

/**
 * Commit FOTA update
 *
 * After the FOTA update is done call this function to set the newly updated 
 * firmwre as the default. This need to be called after fota_perform() is 
 * success.
 *
 * @param f_hadle Pointer to fota handle that was returned by fota_init()
 * @param do_reset if set to 1, perform reset
 * @returns fota_error_t error code
 */
int 
fota_commit(fota_handle_t *f_handle, int do_reset)
{
    int rval;
    /*Mark that fota was in progress*/
    os_printf("\n%s", __FUNCTION__);
    if(utils_file_touch(FOTA_IN_PROGRESS_FILE_PATH) < 0){
        return -1;
    }

    if(FOTA_ERROR_NONE != (rval = fota_update_config_file(f_handle))){
        os_printf("\nError: updating config file failed");
        return rval;
    }
    if(FOTA_ERROR_NONE != (rval = fota_update_part_file(f_handle))){
        os_printf("\nError: updating part file failed");
        return rval;
    }
    
    if(FOTA_ERROR_NONE != (rval = fota_set_boot_index(f_handle))){
        os_printf("\nError: updating boot.json failed");
        return rval;
    }
#if 0
    fota_debug_dump_file(PART_FILE_PATH);
    fota_debug_dump_file(FOTA_CFG_FILE_PATH);
    fota_debug_dump_file(BOOT_FILE_PATH);
#endif    
    /*Fota is success*/    
    unlink(FOTA_IN_PROGRESS_FILE_PATH);
    /*Reboot the device*/
    if(do_reset == 1) {
        reset_device();
    }
    /*control shll not come here. the statement below is just for avaiding
     compilation warning*/
    return rval;
}

/* This API will just up date the FW. Downloading the FW need to be done by the 
 * user application. call this repeatedly untill full download is complete.
*/
int 
fota_fw_update(fota_handle_t *f_handle, fota_fw_info_t *fw_info)
{
    static int total_rcvd_len = 0;
    int copy_len, available;
#ifdef FOTA_DEBUG
    static int dot_cnt = 0;
#endif
    uint8_t hash[32];
    int rval = FOTA_ERROR_NONE;
    fota_image_info_t * image_info = NULL;
    
    if(NULL == f_handle || NULL == fw_info){
        return FOTA_ERROR;
    }
    
#ifdef FOTA_DEBUG     
    os_printf(".");
    if(dot_cnt == 80){dot_cnt = 0;os_printf("\n");}
    dot_cnt++;
    total_rcvd_len += fw_info->data_len;
    os_printf("\n\t%s: fw_info->data_len = %d, total_rcvd_len= %d\n", 
               __FUNCTION__, fw_info->data_len, total_rcvd_len);
#endif
    /*parse part.json file*/
    if(!f_handle->json_part){
        os_printf("\n%s: parsing part.json file (json_init)", __FUNCTION__);
        f_handle->json_part = fota_json_init(PART_FILE_PATH, NULL);
        if(NULL == f_handle->json_part){
            return FOTA_ERROR_PART_FILE_LOAD;
        }
    }
    if(!f_handle->fw_sector){
        /* Select Flash area using fota_select_image_area() */
        os_printf("\n%s: Selecting the image area", __FUNCTION__);
        image_info = fota_select_image_area(f_handle, fw_info->fw_name);
        if(NULL == image_info){
            return FOTA_ERROR_PART_FILE_LOAD;
        }
        /*Initialize the starting sector of the flash in to which the new firmaware
          will be downloaded*/
        f_handle->fw_sector = image_info->sector;
        f_handle->next_boot_index = image_info->index;
        os_printf("\n%s: starting Download @sector= %d", __FUNCTION__, 
                  f_handle->fw_sector);
    }

    if(NULL == f_handle->recv_buff){
        /*First need to get the content length using HEAD method??- TODO*/
        f_handle->recv_buff = os_alloc(FOTA_MAX_RECV_BUFF_SIZE);
        if(NULL == f_handle->recv_buff){
            /*TODO in http if cb returns error, abort connection*/
            return FOTA_ERROR;
        }
        f_handle->recv_index = 0;
    }
    available = (FOTA_MAX_RECV_BUFF_SIZE - f_handle->recv_index); 
    copy_len = (available > fw_info->data_len)? 
                                    fw_info->data_len: available;
    memcpy((f_handle->recv_buff + f_handle->recv_index), fw_info->data, 
            copy_len);
    f_handle->recv_index += copy_len;
    fw_info->data_len -= copy_len;
    if(f_handle->recv_index >= FOTA_MAX_RECV_BUFF_SIZE ||
        (!fw_info->more_data && !fw_info->data_len)){
        
        boot_sha256_update(&sha256_ctx, f_handle->recv_buff, 
                           f_handle->recv_index);
        os_printf("\n@ln=%d Writing to flash @sector=%d, numbytes = %d", 
                  __LINE__, f_handle->fw_sector, f_handle->recv_index);
        sector_cache_write(f_handle->spi_mem, 
                           f_handle->fw_sector * SECTORSIZE, 
                           f_handle->recv_buff , f_handle->recv_index);
        f_handle->fw_sector++;
        f_handle->recv_index = 0;                        
    }
    if(fw_info->data_len && !fw_info->more_data){
        /*The response body is not fully copied in to recv_buff in the 
          earlier copy as there was not enough space*/
        memcpy(f_handle->recv_buff, fw_info->data + copy_len, 
                fw_info->data_len);
        boot_sha256_update(&sha256_ctx, f_handle->recv_buff, 
                           fw_info->data_len);
        
        os_printf("\n@ln=%d Writing to flash @sector=%d, numbytes = %d", __LINE__, f_handle->fw_sector,
            fw_info->data_len);
        sector_cache_write(f_handle->spi_mem, 
                           f_handle->fw_sector * SECTORSIZE, 
                           f_handle->recv_buff , fw_info->data_len);
        f_handle->fw_sector++;
        f_handle->recv_index = 0;
    }else if(fw_info->data_len){
        memcpy(f_handle->recv_buff, fw_info->data + copy_len, 
                fw_info->data_len);
        f_handle->recv_index = fw_info->data_len;
    }
    
    if(!fw_info->more_data){
        /*No more data to be received. flush the cache*/
        os_printf("\nsector_cache_flush_all");
        sector_cache_flush_all(f_handle->spi_mem);
        /*input buffer is no more needed*/
        if(NULL != f_handle->recv_buff){
            os_free(f_handle->recv_buff);
            f_handle->recv_buff = NULL;
        }

        /* Check Image integrity*/    
        boot_sha256_final(&sha256_ctx, hash);
        os_printf("\nimage hash: ");
        for(int i = 0; i < 32; i++)   
            os_printf("%x", hash[i]);    
        if(fw_info->hash_str && strlen(fw_info->hash_str)){
            os_printf("Verifying the image integrity\n");
            unsigned char hash_from_cfg[32];
            fota_string_to_hash(fw_info->hash_str, hash_from_cfg);
            if(0 != memcmp(hash, hash_from_cfg, 32)){
                os_printf("\nError: Image Integrity check failed");
                rval = FOTA_ERROR_IMAGE_INTEGRITY;
                goto error_exit;
            } else {
                os_printf("Image integrity verified. OK.\n");
            }
        }
    }
    return FOTA_ERROR_NONE;
error_exit:
    return rval;
}

/** @} */

