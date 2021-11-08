/*******************************************************************************
*
*  File: mqtt_nw_tls.c
*
*  Copyright (c) 2020, InnoPhase, Inc.
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
*******************************************************************************/
#include <lwip/netif.h>
#include <lwip/sockets.h>
#include "lwip/netdb.h"
#include <lwip/ip_addr.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "ssl_wrap/inc/ssl_wrap.h"
#include "mbedtls/net_sockets.h"
#include "mqtt/platform/mqtt_nw.h"

int 
mqtt_ssl_sock_read(MQTTNetwork* n, unsigned char* buf, int len,
                   int timeout_ms)
{
    ssl_wrap_handle_t ssl_wrap_handle;
    int ret;
    /*Get the ssl wrap handle*/
    ssl_wrap_handle = ssl_wrap_fd_to_handle(n->socket);
    ret = ssl_wrap_read_timeout(ssl_wrap_handle, buf, len, timeout_ms);    
    return ret;
}

int 
mqtt_ssl_sock_write(MQTTNetwork* n, unsigned char* buf, int len,
                    int timeout_ms)
{
    int ret;
    ssl_wrap_handle_t ssl_wrap_handle;
    /*Get the ssl wrap handle*/
    ssl_wrap_handle = ssl_wrap_fd_to_handle(n->socket);
    ret = ssl_wrap_write(ssl_wrap_handle, buf, len);    
    return ret;
}

void
MQTTNetworkDisconnect_Tls(MQTTNetwork* n)
{
    ssl_wrap_handle_t ssl_wrap_handle;
    /*Get the ssl wrap handle*/
    ssl_wrap_handle = ssl_wrap_fd_to_handle(n->socket);
    return ssl_wrap_disconnect(ssl_wrap_handle);
}

void MQTTNetworkInit_Tls(MQTTNetwork* n)
{
    n->mqttread = mqtt_ssl_sock_read;
    n->mqttwrite = mqtt_ssl_sock_write;
    n->disconnect = MQTTNetworkDisconnect_Tls;   
}

int
MQTTNetworkConnect_Tls(MQTTNetwork *n, char *host, int port, 
                       ssl_wrap_cfg_t *cfg)
{
    ssl_wrap_handle_t ssl_wrap_handle;
    
    os_printf("\n%s:%s", __FILE__, __FUNCTION__);
    ssl_wrap_handle = ssl_wrap_connect(host, port, cfg);
    if(NULL == ssl_wrap_handle)
        return -1;
    n->socket = ssl_wrap_sock_fd_get(ssl_wrap_handle);
    return n->socket;
}

