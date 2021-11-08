
#include <errno.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "mqtt_nw.h"


static int 
mqtt_socket_read(MQTTNetwork* n, unsigned char* buffer, int len, int timeout_ms)
{
    fd_set rs;
    struct timeval timeout;
    int rval;

    FD_ZERO(&rs);
    FD_SET(n->socket, &rs);

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    rval = lwip_select(n->socket+1, &rs, NULL, NULL, &timeout);
    if(rval <= 0)
        return rval;
    return lwip_read(n->socket, buffer, len);
}


static int 
mqtt_socket_write(MQTTNetwork* n, unsigned char* buffer, int len, int timeout_ms)
{
    /* TODO timeout */
    return lwip_write(n->socket, buffer, len);
}


static void 
mqtt_socket_disconnect(MQTTNetwork* n)
{
    lwip_close(n->socket);
}

/*APIs*/
void 
MQTTNetworkInit(MQTTNetwork* n)
{
    n->socket = -1;
    n->mqttread = mqtt_socket_read;
    n->mqttwrite = mqtt_socket_write;
    n->disconnect = mqtt_socket_disconnect;
}

int 
MQTTNetworkConnect(MQTTNetwork* n, char* addr, int port)
{
    int s = -1;
    int e = EHOSTUNREACH;
    char service[8];
    struct addrinfo hints;
    struct addrinfo *a, *ai;

    os_printf("\n%s:%s", __FILE__, __FUNCTION__);
    snprintf(service, sizeof(service), "%u", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

    if(lwip_getaddrinfo(addr, service, &hints, &ai) != 0)
        return -e;

    for(a = ai; a != NULL; a = a->ai_next) {
        s = lwip_socket(a->ai_family, a->ai_socktype, a->ai_protocol);
        if(s < 0) {
            e = errno;
            continue;
        }
        if(lwip_connect(s, a->ai_addr, a->ai_addrlen) < 0) {
            e = errno;
            lwip_close(s);
            s = -1;
            continue;
        }
        break;
    }
    lwip_freeaddrinfo(ai);

    if(s < 0)
        return -e;

    n->socket = s;
    return 0;
}

void MQTTNetworkDisconnect(MQTTNetwork *n)
{
    (*n->disconnect)(n);
}
