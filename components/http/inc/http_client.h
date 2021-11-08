#pragma once
/**
******************************************************************************
* @file http_client.h
* @brief This file contains the definition of data structures and APIs for
*   the http client module
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

/** @ingroup http_client
 *  @{
 */

/*TODO headers shall not be included in the header- or only include mininm files*/
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "ssl_wrap/inc/ssl_wrap.h"


/*
 * Un-comment the macro below to enable debug printfs in the http client module
 */
//#define HTTP_CLIENT_DEBUG_ENABLE

#define HTTP_STATUS_CODE_200_OK     200

#define HTTP_MAX_REQ_HDRS   10
#define HTTP_MAX_RESP_HDRS  32

enum {
    HTTP_CLIENT_ERROR = -1,
    HTTP_CLIENT_ERROR_NONE = 0,
    HTTP_CLIENT_CONTINUE_POST,
    HTTP_CLIENT_CLOSE_CONNECTION
};

typedef struct {
    char *url;
    char *hostname;
    int port;
    int secured; /*https*/
    ssl_wrap_cfg_t ssl_cfg;
    int time_out;
} http_client_config_t;

typedef struct {
    int status_code;/**< HTTP response status code*/
    char **resp_hdrs;/*< Response headers. Array of strings*/
    const char *resp_body; /**< Response body*/
    int resp_len; /**< Resp len, currently availabe in the resp_body*/
    unsigned int resp_total_len;/**< Total length of the response body. If 0,
                            No total length available before hand as the body
                            may be sent using chunked or multipart encoding*/
    int more_data;
} http_client_resp_info_t;

/*This is the call back called when the response is received*/
typedef void http_client_resp_cb(void *ctx, http_client_resp_info_t *resp);

typedef  void*  http_client_handle_t;

http_client_handle_t
http_client_open(http_client_config_t *cfg);


int
http_client_get(http_client_handle_t handle, char *uri,
                http_client_resp_cb cb, void *cb_ctx,
                int time_out);

int
http_client_post(http_client_handle_t handle, char *uri,
                 char *buff, int buff_len,
                 http_client_resp_cb cb, void *cb_ctx,
                 int time_out);

int
http_client_set_req_hdr(http_client_handle_t handle,
                        const char *hdrname, const char *hdrval);

void
http_client_del_req_hdrs_all(http_client_handle_t handle);

int
http_client_close(http_client_handle_t handle);

int
http_client_sock_fd_get(http_client_handle_t handle);

ssl_wrap_handle_t *
http_client_ssl_context_get(http_client_handle_t handle);

int 
http_client_url_to_host(const char *url, char *host, int host_max_len,
                        char *path, int path_max_len, int *port);

/** @} */

