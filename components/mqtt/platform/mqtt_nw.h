#pragma once

#include <kernel/os.h>
#include <string.h>
#include "ssl_wrap/inc/ssl_wrap.h"
#include "websocket/inc/websock.h"

typedef struct MQTTNetwork MQTTNetwork;

struct MQTTNetwork
{
    int socket;
    int (*mqttread) (MQTTNetwork*, unsigned char*, int, int);
    int (*mqttwrite) (MQTTNetwork*, unsigned char*, int, int);
    void (*disconnect) (MQTTNetwork*);
};

/*Non secured MQTT connection*/
void MQTTNetworkInit(MQTTNetwork*);

int MQTTNetworkConnect(MQTTNetwork*, char*, int);

void MQTTNetworkDisconnect(MQTTNetwork*);

/*Secured MQTT connection*/
void MQTTNetworkInit_Tls(MQTTNetwork*);

int
MQTTNetworkConnect_Tls(MQTTNetwork *n, char *host, int port, 
                       ssl_wrap_cfg_t *cfg);

void MQTTNetworkDisconnect_Tls(MQTTNetwork*);

/*MQTT over WebSocket connection (Secured and Non secured*/
void MQTTNetworkInit_Ws(MQTTNetwork*);

int MQTTNetworkConnect_Ws(MQTTNetwork* n, websock_config_t * ws_cfg);

void MQTTNetworkDisconnect_Ws(MQTTNetwork*);
