#pragma once
/**
******************************************************************************
* @file fota.h
* @brief This file contains the definition of data structures and APIs for
*   the fota module
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

/** @ingroup fota 
 *  @{
 */

#define FOTA_DEBUG  
#ifdef FOTA_DEBUG
#define fota_printf os_printf
#else
#define fota_printf
#endif


#define FOTA_STR_PACKAGE_VER    
#define FOTA_STR_FILES          
/** @internal
 *
 * Data Structure used to store the files information from the fota.config 
 * file 
 */
typedef struct fota_files_info{
    char *type;
    char *name;
    char *version;
    char *protocol;
    char *host_name;
    int port;    
    int secured; /*0- Non secured. 1- secured without validation
                   2 - secured with validation*/
    char *ca_cert;
    char *uri;
    char *url;
    char *hash;
    struct fota_files_info *next;
}fota_files_info_t;

/** @internal
 *
 * Data Structure used to store the image information from the part.json file 
 * file 
 */
typedef struct fota_image_info{
    char *name; 
    char *version;    /**< FOTA version. This is not the Firmware version*/
    int sector;     /**< Starting sector*/
    struct fota_image_info *next;
    int index;      /**<index in the image array in part.json file*/
}fota_image_info_t;

/** @internal
 *
 * FOTA configuration structure 
 */
typedef struct {
    const char *package_version;

    /*Files information*/
    fota_files_info_t *files_info_list;
}fota_config_t;


/**
 *
 * FOTA error definitions
 */
enum {
    FOTA_ERROR_NONE,
    FOTA_ERROR_HTTP_CONNECT = -99,        /**< HTTP error */
    FOTA_ERROR_HTTP_GET,
    FOTA_ERROR_HTTP_RESP_STATUS, /**< HTTP response code is not 200 OK*/
    FOTA_ERROR_IMAGE_AUTH,  /**< Image authentication error */
    FOTA_ERROR_NO_CONFIG_FILE, /**<There shall exist a fota configuration file 
                                fota.config in the FS.This error is returned if 
                                the fota module does not find the file in the FS.*/
    FOTA_ERROR_CONFIG_FILE_LOAD, /**<Error during json decoding*/
    FOTA_ERROR_FLASH_WRITE, /**<Flash write error*/
    FOTA_ERROR_FILE_OPEN,
    FOTA_ERROR_FILE_WR,
    FOTA_ERROR_PART_FILE_OPEN,
    FOTA_ERROR_PART_FILE_LOAD,/**<Error during json decoding*/
    FOTA_ERROR_PART_FILE_UPDATE,
    FOTA_ERROR_PART_FILE_PARSE,
    FOTA_ERROR_MEM_ALLOC, /**<Mempry allocation error*/
    FOTA_ERROR_SET_BOOT_INDEX,
    FOTA_ERROR_SPI_MEM,
    FOTA_ERROR_URL,
    FOTA_ERROR_NO_CA_CERT,
    FOTA_ERROR_IMAGE_INTEGRITY,
    FOTA_ERROR

};/*fota_error_t;*/

typedef struct
{
    char *host_name;
    int port;
}fota_connection_info_t;

typedef enum {
    FOTA_RECV_TYPE_NONE,
    FOTA_RECV_TYPE_CONFIG,
    FOTA_RECV_TYPE_FIRMWARE,
    FOTA_RECV_TYPE_FILE      
}fota_recv_type_t;
/**
 *
 * FOTA module context (handle) 
 */

typedef struct  {
    fota_config_t *cfg;   /**<Fota configuration, parsed from fota.config 
                                    file in the root FS*/
    fota_config_t *cfg_remote;   /**<configuration, parsed from 
                                    remote fota.config file present in the server)*/
    fota_image_info_t *image_info_list; /**<Application partition information 
                                              used during the image donload. 
                                              The information is populated by 
                                              fota_select_mage_area()*/
    void *conn_handle;                 /**<Connection handle*/
    fota_connection_info_t conn_info;
    fota_recv_type_t recv_type;
    char * recv_buff;  /**<Pointer to new configuration 
                                   This will be stored in config file in 
                                   root FS*/
    int recv_index;                               
    int http_resp_status; 
    int error_http_cb;/**< error in http call back*/
    int next_boot_index;           /**<*/
    fota_files_info_t * curr_file_info; /**< to be used in http call back*/
                                             
    json_t * json_part;         /**<root json object for the part.json file*/
    json_t * json_cfg;       /**<root json object for the fota.config file*/
    json_t * json_cfg_remote; /**<root json object for the remote fota.config 
                                    file*/
    struct spi_mem_device * spi_mem;
    unsigned int fw_sector;
    FILE *f;/**< to be used in http call back while downloading the file*/
}fota_handle_t;

typedef struct {
    char *fw_name;
    char *hash_str;
    fota_recv_type_t type;/*As of now this has to be FOTA_RECV_TYPE_FIRMWARE*/
    const char *data; /**< Firmware data*/
    int data_len; /**< data length*/
    
    int more_data;
}fota_fw_info_t;

/******************************************************************************
* Public functions
*
******************************************************************************/
fota_handle_t *
fota_init(void);

void 
fota_deinit(fota_handle_t * f_handle);

int 
fota_update_check(fota_handle_t *f_handle, int *update_available);

int
fota_perform(fota_handle_t *f_handle, int check_for_update,
             int fetch_config);

int 
fota_commit(fota_handle_t *f_handle, int do_reset);

int 
fota_fw_update(fota_handle_t *f_handle, fota_fw_info_t *fw_info);

/** @} */

