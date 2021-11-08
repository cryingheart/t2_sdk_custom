/**
******************************************************************************
* @file http_client.c
* @brief This file is part of the http module implimentation. This file provides
*   the HTTP client APIs. It also implements the helper functions.
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
* @addtogroup http_client
* @{
*
* This module impliments the HTTP client functionality. HTTP is an application
* layer protocol used for data exchange. This protocol follows the client-server
* model. The client sends the "Request" and server send the "Response".
* HTTP over tls(ssl) is refered to as HTTPS. This is the secured connection.
* HTTP is assigned with port 80 wwhere as HTTPS is assigened with 443.
* This module subset of HTTP 1.1 version adhearing to  RFC 7230.
* Supported features:
*  1. GET and POST methods
*  2. Receive Chunked encoded data
*  3. Redirection
*  4. HTTPS (HTTP over ssl/tls)
*/

#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "lwip/netdb.h"
#include "../inc/http_client.h"

#define MAX_RECV_BUF_SIZE                   1460
#define MAX_SEND_BUF_SIZE                   1460
#define MAX_SEGMENT_SIZE                    1460

#define ERROR_BUFSIZ                        128
#define HTTP_CLIENT_HOST_NAME_LEN_MAX       255 /*!< Maximum host name defined 
                                                    in RFC 1035 */
#define HTTP_MAX_CHUNK_SIZE_ASCII_LEN       8/*8 Nibles that an integer can hold*/ 

/* macros for characters */
#define HTTP_IP_ADDR_DELIM      "."
#define HTTP_SPACE              ' '  /* 32 */
#define HTTP_CR                 '\r' /* 13 */
#define HTTP_LF                 '\n' /* 10 */
#define HTTP_HDR_DELIM          ": "
#define HTTP_CR_LF_STR          "\r\n"
#define HTTP_2_CR_LF_STR        "\r\n\r\n"
#define HTTP_VER_STR            "HTTP/1.1"
#define HTTP_SPACE_STR          " "
#define HTTP_CR_STR             "\r"

#define HTTP_CR_LF_STR_LEN      2
#define HTTP_2_CR_LF_STR_LEN    4
#define HTTP_CR_STR_LEN         1
#define HTTP_LF_STR_LEN         1
#define HTTP_BUFF_PAD           4
typedef enum {
    HTTP_GET = 1,
    HTTP_HEAD,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_CONNECT
} http_client_method_t;

typedef enum {
    HTTP_STATE_IDE,
    /*Request send states*/
    HTTP_STATE_REQ_SEND,
    HTTP_STATE_REQ_BODY_SEND,
    /*Response process states*/
    HTTP_STATE_RESP_SL_RECV,
    HTTP_STATE_RESP_HDR_RECV,
    HTTP_STATE_RESP_BODY_RECV,    
    HTTP_STATE_RESP_CHUNK_HDR_RECV,/*Response process - Chunked Encoding*/
    HTTP_STATE_RESP_BODY_RECV_CHUNKED
} http_client_state_t;

typedef struct {
    char *url;/*one time use - need to go to config*/
    char *hostname;/*one time use - need to go to config*/
    int port;/*one time use - need to go to config*/
    short method;/*one time use - need to go to config*/
    int req_hdr_sent_cnt;
    char *body_buff;/*Used for sending the body*/
    unsigned int body_buff_len;
    int more_data;
} http_client_req_t;

typedef struct {
    int time_out;
    char *recv_buf;
    int recvd_len;/*bytes received */
    unsigned int bytes_available;   /*Bytes available in the recv_buf,
                                      not parsed/consumed yet*/
    unsigned int total_len;         /*used in case of Conetent Length,  
                                     Total length of the response */
    unsigned int body_received_len; /*length of the body received till now*/
    unsigned int chunk_len;         /* used in case of Chunked Encoding,
                                     Chunk length curently being received*/
    unsigned int chunk_received_len;/*length of the chunk received till now*/
    int chunked;    /*response is in Chunked-Encoding format*/
    int content_len_present;
    int hdr_cnt;
} http_client_resp_t;

enum {
    HTTP_FLAG_KA = 1,
    HTTP_FLAG_SECURED = HTTP_FLAG_KA << 1,
    HTTP_FLAG_HOST_HDR_PRESENT = HTTP_FLAG_SECURED << 1,
    HTTP_FLAG_CONTENT_LEN_HDR_PRESENT = HTTP_FLAG_HOST_HDR_PRESENT << 1,
    HTTP_FLAG_CONN_UPGRADE_HDR_PRESENT = HTTP_FLAG_CONTENT_LEN_HDR_PRESENT << 1,
    HTTP_FLAG_UPGRADE_WEBSOCK_HDR_PRESENT = HTTP_FLAG_CONN_UPGRADE_HDR_PRESENT  << 1,
};

#define HTTP_FLAG_SET(h, f)         ((h)->flags |= (f))
#define HTTP_FLAG_CLEAR(h, f)        ((h)->flags &= (~f))
#define HTTP_FLAG_IS_SET(h, f)      ((h)->flags & (f))

typedef struct http_clent_handle{
    short state;
    int sock_fd;

    int max_req_cnt;/*The Keep-Alive general header allows the sender to hint
                        about how the connection may be used to set a timeout
                        and a maximum amount of requests*/
    int req_cnt;/*Keepalive related. To check against max_req_cnt*/
    unsigned int flags;
    short status_code;
    ssl_wrap_handle_t ssl_handle;
    char *send_buf;
    char *req_hdrs[HTTP_MAX_REQ_HDRS + 1];/* +1 : NULL terminated table*/
    int req_hdr_index;
    int req_body_len;/*Body total length. body may be sent across multiple
                       calls to post API*/
    char *recv_buf;
    char *resp_hdrs[HTTP_MAX_RESP_HDRS + 1];/* +1 :NULL terminated table*/
    http_client_resp_cb *resp_cb;
    void *resp_cb_ctx;
    char *hostname;/*Used for setting the "Host", header in case its not set*/
} http_clent_handle_c ;

/**
 * Specifies the http method strings.
 */
char * http_method_table[] = {
    "INV",     /* special value for invalid method */
    "GET",
    "HEAD",
    "POST",
    "PUT",
    "DELETE",
    "CONNECT"
};

/**
 * This function reads data from http server
 * @param h Pointer to http connection info object
 * @param len number of bytes to read.
 * @return returns bytes read.
 */
#ifdef HTTP_CLIENT_DEBUG_ENABLE

#define HTTP_DEBUG_RECV_INDEX(r, h) ((r)->recv_buf - (h)->recv_buf)

void
print_buff(char * buff, int len)
{
    int i = 0;
    os_printf("\n");
    while(len--) {
        os_printf("%c", buff[i++]);
    }
    os_printf("\n");
}
#else
#define HTTP_DEBUG_RECV_INDEX(r, h) 
#define os_printf(...)

#define printf_buff(a, b)
#endif

static int
valid_digit(char *ip_str)
{
    while(*ip_str) {
        if(*ip_str >= '0' && *ip_str <= '9')
            ++ip_str;
        else
            return 0;
    }
    return 1;
}


/* return 1 if IP string is valid, else return 0
Max lenth of the string, if its valid ip address is,
    "255.255.255.255" - 15
*/
static int
is_valid_ip(char *ip_str_in)
{
    int num, dots = 0;
    char *ptr;
    char ip_str[16];

    if(ip_str == NULL)
        return 0;
    if(strlen(ip_str_in) > 15)
        return 0;
    strcpy(ip_str, ip_str_in);
    ptr    = strtok(ip_str, HTTP_IP_ADDR_DELIM);
    if(ptr == NULL)
        return 0;
    while(ptr) {
        /* after parsing string, it must contain only digits */
        if(!valid_digit(ptr))
            return 0;
        num = atoi(ptr);
        /* check for valid IP */
        if(num >= 0 && num <= 255) {
            /* parse remaining string */
            ptr = strtok(NULL, HTTP_IP_ADDR_DELIM);
            if(ptr != NULL)
                ++dots;
        } else
            return 0;
    }
    /* valid IP string must contain 3 dots */
    if(dots != 3)
        return 0;
    return 1;
}

/**
 * This function will connect the http client to the server
 * @param ip Pointer to IP address/hostname
 * @param port TCP port number
 * @return returns socket fd if success else -1.
 */
static int
http_client_connect(char *hostname, int port)
{
    int sock_fd;
    int status = -1;
    struct addrinfo hints;
    struct addrinfo *list = NULL, *current;
    struct sockaddr_in srvr_addr;
    char port_str[8] = {0};

    os_printf("\nhttp_client_connect %s : %d", hostname, port);
    if(is_valid_ip(hostname) == 0) {        
        sprintf(port_str, "%d", port);

        /* Do name resolution with both IPv6 and IPv4 */
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        /*Its a Fully Qualified Domain Name. Need to be resolved*/
        os_printf("\nResolving : %s", hostname);
        status = getaddrinfo(hostname, port_str, &hints, &list);
        if(status) {
            os_printf("\nError: Failed to resolve DNS = %s", hostname);
            return -1;
        }

        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(sock_fd < 0) {
            os_printf("\nError : Opening the socket %s", __FUNCTION__);
            return -1;
        }
        /* getaddrinfo() may resolve to multiple IPs. Try to connect to
         * connect to one by one IP. stop immediately after the first
         * successful connect
         */
        os_printf("\n DNS resolve done : status = %d", status);
        current = list;
        while(current) {
            status = connect(sock_fd, current->ai_addr,
                             (uint32_t)current->ai_addrlen);
            os_printf("\n connect status = %d", status);
            if(0 == status){
                break;
            }
            current = current->ai_next;
        }

        freeaddrinfo(list);
        if(status < 0) {
            close(sock_fd);
            sock_fd = -1 ;
        }
    } else {
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(sock_fd == -1) {
            return -1;
        }
        srvr_addr.sin_addr.s_addr = inet_addr(hostname);
        srvr_addr.sin_family = AF_INET;
        srvr_addr.sin_port = htons(port);
        status = connect(sock_fd, (struct sockaddr *)&srvr_addr,
                         sizeof(srvr_addr));
        if(status < 0) {
            os_printf("\nError: Failed to connect to IP: %s", hostname);
            close(sock_fd);
            sock_fd = -1;
            return -1;
        }
    }

    if(sock_fd >= 0) {
        struct timeval timeout;
        timeout.tv_sec = 60;
        timeout.tv_usec = 0;
        lwip_setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    }
    return sock_fd;
}
/**
 * This function creates http client request
 * @param h Pointer to http connection info object
 * @return returns pointer to buffer which contains http request.
 */


/*
<method> <request-URL> <version>\r\n
<headers>\r\n
\r\n
<entity-body>

*/
int
http_client_req_create(http_clent_handle_c *h, http_client_req_t *req,
                       int *req_len, char *send_buf, int max_len)
{
    int len, i;
    
    *req_len = len = 0;   
    send_buf[0] = '\0';
    if(req->req_hdr_sent_cnt >= 0){
        /*Method line has been already sent. Directly send headers*/
        goto send_headers;
    }else{
        req->req_hdr_sent_cnt = 0;/*This will be the start index*/
    }
    /*copy method*/
    strcpy(send_buf, http_method_table[req->method]);
    len = strlen(http_method_table[req->method]);
    send_buf[len++] = HTTP_SPACE;
    send_buf[len] = '\0';

    /*Add URI*/
    strcat(send_buf, req->url);
    len += strlen(req->url);
    send_buf[len++] = HTTP_SPACE;
    send_buf[len] = '\0';

    /*Add http version*/
    strcat(send_buf + 1, HTTP_VER_STR);
    len += strlen(HTTP_VER_STR);
    send_buf[len++] = HTTP_CR;
    send_buf[len++] = HTTP_LF;
    send_buf[len] = '\0';
send_headers:    
    for(i = req->req_hdr_sent_cnt; 
        i < h->req_hdr_index && h->req_hdrs[i]; i++){
        if((len + strlen(h->req_hdrs[i]) + strlen(HTTP_2_CR_LF_STR))
            >= max_len){
            /*All the headers does not fit in the send buffer*/
            req->req_hdr_sent_cnt = i;            
            *req_len = len;
            os_printf("\nNot all headers sent: *req_len = %d", *req_len);
            return 1;
        }
        strcat(send_buf, h->req_hdrs[i]);
        len += strlen(h->req_hdrs[i]) ;
        os_printf("\n len = %d %d", strlen(h->req_hdrs[i]), len);
        send_buf[len++] = HTTP_CR;
        send_buf[len++] = HTTP_LF;
        send_buf[len] = '\0';
    }
    send_buf[len++] = HTTP_CR;
    send_buf[len++] = HTTP_LF;
    send_buf[len] = '\0';
    *req_len = len;
    os_printf("\n *req_len = %d", *req_len);
    return 0;
}

static int
http_client_socket_send(http_clent_handle_c *h, char *buff, int len)
{
    int rval = 0;

    if(HTTP_FLAG_IS_SET(h, HTTP_FLAG_SECURED)) {
        os_printf("\n%s : Secure send", __FUNCTION__);
        rval = ssl_wrap_write(h->ssl_handle, (unsigned char*)buff, len);
    } else {
        os_printf("\n%s : Non Secure send", __FUNCTION__);
        rval = send(h->sock_fd, buff, len, 0);
    }
    return rval;/*bytes sent*/
}
/**
 * This function sends http client request
 * @param h Pointer to http connection info object
 * @return returns pointer to buffer which contains http request.
 */

static int
http_client_req_send(http_clent_handle_c *h, http_client_req_t *req)
{
    char *p;
    int req_len, sent_len;
    size_t rval = 0, ret;
    os_printf("\nhttp_client_req_send");
    if(h->state == HTTP_STATE_REQ_SEND) {
        req->req_hdr_sent_cnt = -1;/*Invalidate the Heders sent count*/
create_req:        
        /*Create request: method line + headers*/
        ret = http_client_req_create(h, req, &req_len, h->send_buf,
                                      MAX_SEND_BUF_SIZE);
        /*Send method line + headers*/
        rval = http_client_socket_send(h, h->send_buf, req_len);
        if(rval < 0) {
            return rval;
        }
        if(ret == 1){
            /*Not all the headers are sent*/
            goto create_req;
        }
        h->state = HTTP_STATE_REQ_BODY_SEND;
    }

    if(h->state == HTTP_STATE_REQ_BODY_SEND && h->req_body_len) {
        sent_len = 0;
        while(sent_len < req->body_buff_len) {
            req_len = (req->body_buff_len > MAX_SEGMENT_SIZE) ?
                      MAX_SEGMENT_SIZE : req->body_buff_len;
            p = req->body_buff + sent_len;
            rval = http_client_socket_send(h, p, req_len);
            if(rval < 0) {
                return rval;
            }
            sent_len += req_len;
        }
        h->req_body_len -= req->body_buff_len;
    }

    return 0;
}

/**
 * This function parse the http response and finds header values
 * @param h Pointer to http connection info object
 * @return if succes returns content length header vale else -1.
 */
static int
http_client_status_line_parse(http_clent_handle_c *h,
                              http_client_resp_t *resp)
{
    char *p, *token, *rest;
    int rval;
    int status_line_len;

    /*First line contains the Method line*/
    p = strstr(resp->recv_buf, HTTP_CR_LF_STR);
    if(p == NULL) {
        /* The method line is not received properly*/
        return 1;
    }
    status_line_len = p - resp->recv_buf;
    p = resp->recv_buf;
    /*See if the response is OK and its version is HTTP1.1*/
    token = strtok_r(p, HTTP_SPACE_STR, &rest);
    rval = strcmp(token, HTTP_VER_STR);
    if(rval) {
        /*Response is not HTTP1.1 */
        return rval;
    }
    token = strtok_r(NULL, HTTP_SPACE_STR, &rest);
    h->status_code = atoi(token);
    /*Update counters*/
    resp->recv_buf += status_line_len + strlen(HTTP_CR_LF_STR);
    resp->bytes_available -= status_line_len + strlen(HTTP_CR_LF_STR); 
    return 0;
}

static bool
is_this_header(char *header, char *this_name, char *this_val)
{
    int len = strlen(this_name);
    char temp[32];
    char *p;

    if(NULL == header)
        return 0;
    memcpy(temp, header, len);
    temp[len] = 0;
    /*Header names are case in-sensitive*/
    if(0 != strcasecmp(temp, this_name)) {
        return 0;
    }

    if(!this_val){
        return 1;
    }
    p = strstr(header, HTTP_SPACE_STR);
    while(*p) { /*ignore more than one space if any*/
        if(*p != ' ') break;
        p++;
    }
    /*Headers values are case in-sensitive*/
    if(0 != strcasecmp(p, this_val)) {
        return 0;
    }
    return 1;
}

static  bool
is_content_len_hdr(char *header)
{
    return is_this_header(header, "Content-Length", NULL);
}
static  bool
is_transfer_enco_chunk_hdr(char *header)
{
    return is_this_header(header, "Transfer-Encoding", "chunked");

}
bool
is_connection_keep_alive_hdr(char *header)
{
    return is_this_header(header, "Connection", "keep-alive");
}

/**
 * This function parse the http response and finds header values
 * @param h Pointer to http connection info object
 * @return if succes returns content length header vale else -1.
 */
static int
http_client_hdrs_parse(http_clent_handle_c *h,
                       http_client_resp_t *resp)
{
    char *end, *token, *next;
    int hdr_len, bytes_parsed = 0;

    /*If the received buffer doesnot contaion full headers section, its 
      either stopped at the begining or at the end (just after /r/n)
      of the header. In case the parsing had stopped at the end of an header
      and first two bytes in the current buffer are /r/n, then the header 
      parsing is complete*/
    if(resp->recv_buf[0] == '\r' && resp->recv_buf[1] == '\n'){
        end = resp->recv_buf;
        goto exit_hdrs_parsing_done;
    }
    /*Headers Section end with "\r\n\r\n". Each Header end with "\r\n"
      Mark if all the headers are present in the available buffer*/
    end = strstr(resp->recv_buf, HTTP_2_CR_LF_STR);
    if(NULL != end){
        end[2] = '\0';
    }
    /*Parse and store Headers*/
    token = resp->recv_buf;
    next = strstr(token, HTTP_CR_LF_STR);
    while(next && (bytes_parsed < resp->bytes_available)) { 
        *next='\0';
        /*Store headers only if storage is available*/
        hdr_len = strlen(token);       
        if(resp->hdr_cnt < HTTP_MAX_RESP_HDRS) {
            h->resp_hdrs[resp->hdr_cnt] = os_alloc(hdr_len + 1);
            strcpy(h->resp_hdrs[resp->hdr_cnt], token);
            resp->hdr_cnt++;
        }
        /*Check if content length header is present*/
        if(is_content_len_hdr(token)) {
            token += strlen("Content-Length:");
            resp->total_len = atol(token);
            resp->content_len_present = 1;
        } else if(is_transfer_enco_chunk_hdr(token)) {
            /*Check if transfer encoding is present*/
            os_printf("\n\tIts chunked encoding!!");
            resp->chunked = 1;
        } else if(is_connection_keep_alive_hdr(token)) {
            /*Check if Connetion is keep alive*/
            h->flags |= HTTP_FLAG_KA;
        }
        bytes_parsed += hdr_len + strlen(HTTP_CR_LF_STR);/* delimeter "\r\n"*/
        next += strlen(HTTP_CR_LF_STR);
        token = next;        
        next = strstr(token, HTTP_CR_LF_STR);        
    }
exit_hdrs_parsing_done:    
    /*Update counters*/
    if(NULL != end){
        /* header section end with "\r\n\r\n", "\r\n" is already accounted
           as part of header parsing. just update for "\r\n"*/
        bytes_parsed += strlen(HTTP_CR_LF_STR);
    }
    resp->recv_buf += bytes_parsed;       
    resp->bytes_available -= bytes_parsed;
    return (NULL == end)?1:0;
}

/* calculate chunck size for the input chunk size string.
 * chunk length being integer, maximum chunk length supported is 
 * 2^31 (2147483647) bytes
 * Returns,
 * -1  -> Error
 *  0  -> Chunk size parse Success
 *  1  -> continue receive, buffer does not have the full chunk header
 */
int http_client_chunk_len_get(http_clent_handle_c *h,
                       http_client_resp_t *resp)
{
    int chunk_size = 0, len = 0, bytes_parsed;
    char ch, *p;

    p = strstr(resp->recv_buf, HTTP_CR_LF_STR);
    if(p == NULL) {
        return 1;
    }
    os_printf("\n%s:Got the chunk header", __FUNCTION__);
    *p = '\0';
    bytes_parsed = (p - resp->recv_buf) +  HTTP_CR_LF_STR_LEN;
    p = resp->recv_buf;
    /* A chunk header may have extensions after the length. Extensions are marked
     * by a semi-colon. Not supporting the Chunk extensions, so end the
     * string at first occurance of semi-colon if any */
    while (*p)
    {
        if (*p == ';'){
            *p = 0;
            break;
        }
        p++;
        len++;
    }
    if(len > HTTP_MAX_CHUNK_SIZE_ASCII_LEN){
        os_printf("\nlen = %d > (%d)HTTP_MAX_CHUNK_SIZE_ASCII_LEN", len,
                  HTTP_MAX_CHUNK_SIZE_ASCII_LEN);
        return -1;
    }
    p = resp->recv_buf;
    while(*p)
    {
        /* Get the next character to examine */
        ch = *p;
        /* Shitf current value left one column (4 bits) to make space for new
         * value */
        chunk_size <<= 4;
        /* Add in the new value */
        if (ch >= '0' && ch <= '9'){
            os_printf("\n\tchunk_size = %x", chunk_size);
            chunk_size += ch - '0';
        }
        else if (ch >= 'a' && ch <= 'f'){
            chunk_size += (ch - 'a') + 10;
        }
        else if (ch >= 'A' && ch <= 'F'){
            chunk_size += (chunk_size - 'A') + 10;
        }
        else
        {
            /* The character does not represent a Hex number.
               Bad ASCII length representation */
            os_printf("\n Error: The character does not represent a Hex number");
            chunk_size = -1;
            break;
        }
        p++;
    }
    if(chunk_size >= 0){
        resp->chunk_len = chunk_size;
    }
    
    os_printf("\n%s:chunk size=%d", __FUNCTION__, resp->chunk_len);
    /*Update counters*/
    resp->recv_buf += bytes_parsed;
    resp->bytes_available -= bytes_parsed;
    return 0;
}

static int
http_client_socket_recv(http_clent_handle_c *h,
                        http_client_resp_t *resp)
{
    int recv_len = 0;
    int buff_len;
    char *buf;

    buff_len = MAX_RECV_BUF_SIZE - resp->bytes_available;
    /*move the available bytes to the begining of the receive buffer*/
    memmove(h->recv_buf, resp->recv_buf, resp->bytes_available);
    buf =  h->recv_buf + resp->bytes_available;
    os_printf("\n%s: Receiving at index %d, buff len = %d", __FUNCTION__,
              resp->bytes_available, buff_len);
    if(HTTP_FLAG_IS_SET(h, HTTP_FLAG_SECURED)) {
        recv_len = ssl_wrap_read_timeout(h->ssl_handle,(unsigned char*)buf,
                                 buff_len, resp->time_out*1000);
        os_printf("\n%s: Received(secure) = %d", __FUNCTION__, recv_len);
    } else { 
        fd_set read_fds;
        struct timeval timeout;
        int readfds;
        /*
         * select() is done to block the read for time_out seconds
         */
        FD_ZERO(&read_fds);
        FD_SET(h->sock_fd, &read_fds);
        timeout.tv_sec = resp->time_out;
        timeout.tv_usec = 1;
        readfds = select(h->sock_fd + 1, &read_fds, NULL, NULL, &timeout);
        if(readfds <= 0) {
            os_printf("\n%s: Error timeout", __FUNCTION__);
            return -1;
        }
        recv_len = recv(h->sock_fd, buf, buff_len, 0);
        os_printf("\n%s: Received = %d", __FUNCTION__, recv_len);
    }
    recv_len += resp->bytes_available; 
    if(recv_len >= 0){
        /*Header parsing and status line parsing heavily depend on string 
          routines, so NULL terminate the resp->recv_buf*/
        h->recv_buf[resp->bytes_available + recv_len] = '\0';
    }
    return recv_len;
}

inline void
resp_info_set(http_clent_handle_c *h, http_client_resp_t *resp,
                http_client_resp_info_t *resp_info, int more_data)
{
    /*
     * Fill response info to be passed to user through callback
     */
    memset(resp_info, 0, sizeof(http_client_resp_info_t));
    resp_info->status_code = h->status_code;
    resp_info->resp_hdrs = &h->resp_hdrs[0];
    resp_info->resp_total_len = resp->content_len_present? resp->total_len:0;
    resp_info->resp_body = resp->recv_buf;
    if(resp->chunked){
        resp_info->resp_len = (resp->chunk_len < resp->bytes_available)?
                                resp->chunk_len : resp->bytes_available; 
    }else{
        resp_info->resp_len = resp->bytes_available;
    }
    resp_info->more_data = more_data;

    os_printf("\n\t%s: total len=%d , resp_len=%d, moredata=%d",__FUNCTION__,
        resp_info->resp_total_len, resp_info->resp_len, resp_info->more_data);
}
static int
http_client_resp_recv(http_clent_handle_c *h,
                      http_client_resp_t *resp)
{
    int rval, bytes_rcvd;
    http_client_resp_info_t resp_info;
    int more_data;
    
    do {
        /*Receive data*/
        bytes_rcvd = http_client_socket_recv(h, resp);
        if(bytes_rcvd < 0) {
            break;
        }        
        resp->recv_buf = h->recv_buf;
        resp->recvd_len = bytes_rcvd;
        resp->bytes_available = bytes_rcvd; 
        /* Parse method line, headers & body based on the state
         */
        if(HTTP_STATE_RESP_SL_RECV == h->state) {
            /*Parse Method line*/
            rval = http_client_status_line_parse(h, resp);
            if(rval > 0)
                continue;
            h->state = HTTP_STATE_RESP_HDR_RECV;
        }
        if(HTTP_STATE_RESP_HDR_RECV == h->state){
            /*Parse Headers*/
            rval = http_client_hdrs_parse(h, resp);
            os_printf("\nhttp_client_hdrs_parse retval = %d", rval);
            if(rval < 0)
                return rval;
            if(rval > 0){
                /*Header parsing is not complete in the available data*/
                continue;                
            }
            /* Done with headers parsing.
             * All 1xx (informational), 204 (no content), and 304 (not modified) 
             * responses MUST NOT include a message-body.
             */
            if((h->status_code > 100 && h->status_code < 200) ||
                h->status_code  == 204 || h->status_code == 304){
                /*Call user call back*/
                resp_info_set(h, resp, &resp_info, 0);
                if(NULL != h->resp_cb)
                    h->resp_cb(h->resp_cb_ctx, &resp_info);
                os_printf("No body shll be present for status code : %d",
                           h->status_code);
                /* Response receive complete*/
                break;
            }
            /*Change the state*/
            h->state = resp->chunked? 
                       HTTP_STATE_RESP_CHUNK_HDR_RECV : HTTP_STATE_RESP_BODY_RECV;
        }
        if( HTTP_STATE_RESP_CHUNK_HDR_RECV == h->state){
            /*Headers Section end with "\r\n\r\n". Each Header end with "\r\n"*/
            /*Parse Headers*/
            rval = http_client_chunk_len_get(h, resp);
            if(rval < 0)
                return rval;
            if(rval > 0){
                /*Header parsing is not complete in the available data*/
                continue;
            }
            if(!resp->chunk_len){
                /*Last chunk received. Done with receiving body*/
                /*Call user call back*/
                resp_info_set(h, resp, &resp_info, 0);
                if(NULL != h->resp_cb)
                    h->resp_cb(h->resp_cb_ctx, &resp_info);
                os_printf("\nLast chunk received. Done with receiving body");
                /* Response receive complete*/
                break;
            }
            h->state = HTTP_STATE_RESP_BODY_RECV_CHUNKED;
        }
        if( HTTP_STATE_RESP_BODY_RECV == h->state){
            /*Receive Body*/
            resp->body_received_len += resp->bytes_available;
            /*
             * Fill the response info to be passed to user through callback
             */
            more_data = (resp->body_received_len >= resp->total_len)?0:1;
            resp_info_set(h, resp, &resp_info, more_data);
            /*Call user call back
             */
            if(NULL != h->resp_cb)
                h->resp_cb(h->resp_cb_ctx, &resp_info);
            
            /*Update counters. All the bytes_available 
              consumed, passed to user call back*/
            resp->recv_buf += resp->bytes_available;
            resp->bytes_available = 0;
            os_printf("\n%s: Rcvd till now: %d total : %d", __FUNCTION__,
                        resp->body_received_len, resp->total_len);
            if(resp->total_len && 
               resp->body_received_len >= resp->total_len) {
                /*Response receive complete.*/
                break;
            }       
        }
        if( HTTP_STATE_RESP_BODY_RECV_CHUNKED == h->state){
            int bytes_consumed;
            /*Receive chunked encoded Body */
            resp_info_set(h, resp, &resp_info, 1);
            /*Call user call back */
            if(NULL != h->resp_cb)
                h->resp_cb(h->resp_cb_ctx, &resp_info);
            /*Update counters*/
            bytes_consumed = (resp->chunk_len < resp->bytes_available)?
                                  resp->chunk_len : resp->bytes_available;
            resp->chunk_len -= bytes_consumed;
            resp->recv_buf += bytes_consumed;
            resp->bytes_available -= bytes_consumed;
            if(!resp->chunk_len){
                h->state = HTTP_STATE_RESP_CHUNK_HDR_RECV;
            }
        }
    } while(bytes_rcvd > 0);

    return (bytes_rcvd < 0) ? -1 : 0;
}
/**
 * This function sends http client request
 * @param h Pointer to http connection info object
 * @return if success returns 0 else -1.
 */
static int
http_client_send_recv(http_clent_handle_c *h,
                      http_client_req_t *req, int time_out)
{
    int rval = 0;
    http_client_resp_t resp;
    int cnt;

    if(NULL == h->send_buf){
        h->send_buf = os_alloc(MAX_SEND_BUF_SIZE);
        if(NULL == h->send_buf) {
            os_printf("\nError: Malloc failure @%s:%d ",__FUNCTION__, __LINE__);
            return -1;
        }
    }
    /*Send request*/
    rval = http_client_req_send(h, req);
    if(rval < 0) {
        return rval;
    }
    if(h->req_body_len) {
        /*More data to be sent. Just reurn. No error*/
        return HTTP_CLIENT_CONTINUE_POST;
    }
    os_printf("\r\nRequest sent..waiting for the response");
    /*Done with sending the request. Receive and process response*/
    h->req_cnt++;
    h->state = HTTP_STATE_RESP_SL_RECV;

    /*Allocate buffer for receiving*/
    memset(&resp, 0, sizeof(http_client_resp_t));
    if(NULL == h->recv_buf){
        h->recv_buf = os_alloc(MAX_RECV_BUF_SIZE+1);/* 1 byte extra for '\0'
                                                      termination */
        if(NULL == h->recv_buf){
            os_printf("Error: Malloc failure @%s:%d", __FUNCTION__, __LINE__);
            return -1;
        }
    }
    resp.recv_buf = h->recv_buf;
    resp.time_out = time_out;
    rval = http_client_resp_recv(h, &resp);
    /* Done with receiving the response. Release response
     * headers
     */
    cnt = 0;
    while(cnt < HTTP_MAX_RESP_HDRS && NULL != h->resp_hdrs[cnt]) {
        os_free(h->resp_hdrs[cnt]);
        h->resp_hdrs[cnt] = NULL;
        cnt++;
    }
    if(rval >= 0 && !HTTP_FLAG_IS_SET(h, HTTP_FLAG_KA)) {
        rval = HTTP_CLIENT_CLOSE_CONNECTION;
    }
    h->state = HTTP_STATE_IDE;
    return rval;
}

static void
http_handle_default_init(http_clent_handle_c  *h)
{
    memset(h, 0, sizeof(http_clent_handle_c));
    h->sock_fd = -1;
}

/*
 * Public Functions
 */
/*
 * Replace the header if already present.
 */
int
http_client_set_req_hdr(http_client_handle_t handle,
                        const char *hdr_name, const char *hdr_val)
{
    int line_len, index, replacing = 0;
    char *line;
    http_clent_handle_c *h = (http_clent_handle_c *)handle;

    if(!h || NULL == hdr_name || NULL == hdr_val ||
            h->req_hdr_index >= HTTP_MAX_REQ_HDRS) {
        return -1;
    }

    /*See if the header, by same name is already present*/
    for(index = 0; index < h->req_hdr_index; index++) {
        if(!strncasecmp(h->req_hdrs[index], hdr_name, strlen(hdr_name))) {
            /*Header present*/
            os_free(h->req_hdrs[index]);
            replacing = 1;
            break;
        }
    }
    line_len = strlen(hdr_name) + strlen(hdr_val) + strlen(HTTP_HDR_DELIM);
    line = os_alloc(line_len + HTTP_BUFF_PAD);
    if(NULL == line) {
        return -1;
    }

    line[0] = '\0';
    strcpy(line, hdr_name);
    strcat(line, HTTP_HDR_DELIM);
    strcat(line, hdr_val);
    line[line_len] = '\0';
    index = replacing ? index : h->req_hdr_index++;
    h->req_hdrs[index] = line;

    if(!strcasecmp(hdr_name, "Host")) {
        HTTP_FLAG_SET(h, HTTP_FLAG_HOST_HDR_PRESENT);
    }
    if(!strcasecmp(hdr_name, "Content-Length")) {
        HTTP_FLAG_SET(h, HTTP_FLAG_CONTENT_LEN_HDR_PRESENT);
        h->req_body_len = atol(hdr_val);
        os_printf("\nRequest body length = %d", h->req_body_len);
    }
    return 0;
}

void
http_client_del_req_hdrs_all(http_client_handle_t handle)
{
    int i;
    http_clent_handle_c *h = (http_clent_handle_c *)handle;

    if(NULL == h)
        return;
    for(i = 0 ; i < HTTP_MAX_REQ_HDRS ; i++) {
        if(h->req_hdrs[i] != NULL) {
            mem_free(h->req_hdrs[i]);
            h->req_hdrs[i] = NULL;
        }
    }
    h->req_hdr_index = 0;    
    HTTP_FLAG_CLEAR(h, HTTP_FLAG_HOST_HDR_PRESENT);
}



int 
http_client_url_to_host(const char *url, char *host, int host_max_len,
                        char *path, int path_max_len, int *port)
{
    const char *p;
    char *next;
    int len, default_port;

    if(NULL == url){
        return 0;
    }
    if(!strncmp(url, "https", strlen("https")) || 
                          !strncmp(url, "wss", strlen("wss"))){
        os_printf("\nhttps or wss");
        default_port = 443;
    }else if (!strncmp(url, "http", strlen("http")) || 
                            !strncmp(url, "ws", strlen("ws"))){
        os_printf("\nhttp or ws");
        default_port = 80;
    }else{
        os_printf("\nWrong URL");
        return -1;
    }
    p = url;    
    next = strstr(p,"//");
    if(!next)
        return -1;
    next += strlen("//");

    p = next;       
    next = strstr(p,"/");
    if(next){
        /*lets copy the path*/
        len = strlen(next);
        if(len >= path_max_len){
            os_printf("\nError: Possible path buffer overflow");
            return -1;
        }
        strcpy(path, next);    
        /*calculate the length of the host path*/
        len = next - p;
    }else{
        /*Default path is "/" */
        len = strlen("/");        
        if(len >= path_max_len){
            os_printf("\nError: Possible path buffer overflow");
            return -1;
        }        
        strcpy(path, "/");
        /*calculate the length of the host path*/
        len = strlen(p);    
    }        
    /*lets copy host*/    
    if(len >= host_max_len){
        os_printf("\nError: Possible host buffer overflow");
        return -1;
    }
    
    strncpy(host, p, len);
    host[len]=0;
    /*see if port is specified*/
    *port = default_port;
    next = strstr(host,":");    
    if(next){
        *next = '\0';    
        os_printf("\r\nport is specified");
        *port = atoi(++next);
    }

    os_printf("\r\nhost = %s, port = %d, path = %s\r\n", host, *port, path);
    return 0;
}

http_client_handle_t
http_client_open(http_client_config_t *cfg)
{
    http_clent_handle_c  *handle;
    int host_name_len = strlen(cfg->hostname);
    if(NULL == cfg)
        return NULL;
    os_printf("\ncfg->hostname = %s", cfg->hostname );
    if(NULL == cfg->hostname || !host_name_len ||
            host_name_len >= HTTP_CLIENT_HOST_NAME_LEN_MAX) {
        os_printf("\nError: Host name not proper");
        return NULL;
    }
    handle = os_alloc(sizeof(http_clent_handle_c));
    if(!handle) {
        return NULL;
    }
    http_handle_default_init(handle);
    if(cfg->secured)
        HTTP_FLAG_SET(handle, HTTP_FLAG_SECURED);

    if(HTTP_FLAG_IS_SET(handle, HTTP_FLAG_SECURED)) {
        os_printf("\nHTTPS (secured)connection");
        handle->ssl_handle = ssl_wrap_connect(cfg->hostname, cfg->port, 
                                              &cfg->ssl_cfg);
        if(NULL == handle->ssl_handle) {
            goto err_exit;
        }
    } else {
        os_printf("\nHTTP connection");
        handle->sock_fd = http_client_connect(cfg->hostname, cfg->port);
        if(handle->sock_fd < 0) {
            goto err_exit;
        }
    }
    /*
     * Connection open is success. Set state
     */
    if(NULL == (handle->hostname = os_alloc(host_name_len + 1))) {
        goto err_exit;
    }
    strcpy(handle->hostname, cfg->hostname);
    handle->state = HTTP_STATE_IDE;
    return (http_client_handle_t)handle;
err_exit:
    if(NULL != handle)
        os_free(handle);
    return NULL;
}

/*Response timeout in seconds. This is the timeout from
                        the last response packet received. Default tiemout
                        is 60 seconds*/
int
http_client_get(http_client_handle_t handle, char *uri,
                http_client_resp_cb cb, void *cb_ctx,
                int time_out)
{
    int status = -1;
    http_client_req_t req;
    http_clent_handle_c *h = (http_clent_handle_c *)handle;

    if(!h || !uri || !strlen(uri)){
        os_printf("\nError: Either handle or the uri passed is not proper");
        return -1;
    }
    h->resp_cb = cb;
    h->resp_cb_ctx = cb_ctx;
    memset(&req, 0, sizeof(http_client_req_t));
    req.url = uri;
    req.method = HTTP_GET;
    if(HTTP_STATE_IDE == h->state) {
        h->state = HTTP_STATE_REQ_SEND;
    }
    if(! HTTP_FLAG_IS_SET(h, HTTP_FLAG_HOST_HDR_PRESENT)) {
        /*Host header is not set. This is the compasory header as per HTTP 1.1*/
        http_client_set_req_hdr(h, "Host", h->hostname);
    }
    status = http_client_send_recv(h, &req, time_out);
    return status;
}

int
http_client_post(http_client_handle_t handle, char *uri,
                 char *buff, int buff_len,
                 http_client_resp_cb cb, void *cb_ctx,
                 int time_out)
{
    int rval;
    http_client_req_t req;
    http_clent_handle_c *h = (http_clent_handle_c *)handle;

    if(!h || !uri || !strlen(uri)){
        os_printf("\nError: Either handle or the uri passed is not proper");
        return -1;
    }
    /*
     * POST is supported only with content length.
     */
    if(!HTTP_FLAG_IS_SET(h, HTTP_FLAG_CONTENT_LEN_HDR_PRESENT)) {
        os_printf("\nError: Content-length header not set");
        return -1;
    }
    h->resp_cb = cb;
    h->resp_cb_ctx = cb_ctx;
    memset(&req, 0, sizeof(http_client_req_t));
    req.url = uri;
    req.body_buff = buff;
    req.body_buff_len = buff_len;
    req.method = HTTP_POST;
    if(HTTP_STATE_IDE == h->state) {
        h->state = HTTP_STATE_REQ_SEND;
    }
    if(! HTTP_FLAG_IS_SET(h, HTTP_FLAG_HOST_HDR_PRESENT)) {
        /*Host header is not set. This is the compasory header as per HTTP 1.1*/
        http_client_set_req_hdr(h, "Host", h->hostname);
    }
    rval = http_client_send_recv(h, &req, time_out);
    return rval;
}

int
http_client_close(http_client_handle_t handle)
{
    http_clent_handle_c *h = (http_clent_handle_c *)handle;
    int cnt = 0;
    while(h->req_hdrs[cnt] != NULL) {
        os_free(h->req_hdrs[cnt]);
        cnt++;
        if(cnt >= HTTP_MAX_REQ_HDRS)
            break;
    }
    cnt = 0;
    while(h->resp_hdrs[cnt] != NULL) {
        os_free(h->resp_hdrs[cnt]);
        cnt++;
        if(cnt >= HTTP_MAX_RESP_HDRS)
            break;
    }
    if(NULL != h->hostname)
        os_free(h->hostname);

    os_free(h->recv_buf);
    os_free(h->send_buf);
    /*Close the socket*/
    if(h->flags & HTTP_FLAG_SECURED)
        ssl_wrap_disconnect(h->ssl_handle);
    else
        close(h->sock_fd);

    /*Fianlly free the handle*/
    os_free(h);
    return 0;
}
/*useful in stw(at commands) and if any user wants to do set sock options*/
int
http_client_sock_fd_get(http_client_handle_t handle)
{
    http_clent_handle_c *h = (http_clent_handle_c *)handle;
    if(NULL == handle)
        return -1;
    return HTTP_FLAG_IS_SET(h, HTTP_FLAG_SECURED)? 
                            ssl_wrap_sock_fd_get(h->ssl_handle) : h->sock_fd;
}

/*Used in WebSocket. The WebSocket after http connection upgrade directly
  send and receive data*/
ssl_wrap_handle_t *
http_client_ssl_context_get(http_client_handle_t handle)
{
    http_clent_handle_c *h = (http_clent_handle_c *)handle;
    if(NULL == handle)
        return NULL;
    return h->ssl_handle;
}

/** @} */

