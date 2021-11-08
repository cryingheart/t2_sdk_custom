#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hio_def.h"
#include "hio_transport.h"
#include <pthread.h>
#include <time.h>
#include "wifi_scan_helper.h"


static void* hio_response_reader(void *p);
static void process_response(uint32_t res_len, char* res_payload);

static int gExit = 0;
static int gClientState = 0;
static uint32_t gWcmHandle = 99999;
static uint8_t wifiConnected = 0;
static uint8_t hioBusy = 0;

#define HIO_GROUP_AWS 15
#define AWS_CONNECT_REQ 0x00
#define AWS_CONNECT_RSP 0x80
#define AWS_PUBLISH_REQ 0x01
#define AWS_PUBLISH_RSP 0x81
#define AWS_SUBSCRIBE_REQ 0x02
#define AWS_SUBSCRIBE_RSP 0x82
#define AWS_SUBSCRIBE_IND 0xC2
#define AWS_SETINTERVAL_REQ 0x03
#define AWS_SETINTERVAL_RSP 0x83
#define AWS_GETCLIENTSTATE_REQ 0x04
#define AWS_GETCLIENTSTATE_RSP 0x84


#define	CLIENT_STATE_INVALID                            0
#define	CLIENT_STATE_INITIALIZED                        1
#define	CLIENT_STATE_CONNECTING                         2
#define	CLIENT_STATE_CONNECTED_IDLE                     3
#define	CLIENT_STATE_CONNECTED_YIELD_IN_PROGRESS        4
#define	CLIENT_STATE_CONNECTED_PUBLISH_IN_PROGRESS      5
#define	CLIENT_STATE_CONNECTED_SUBSCRIBE_IN_PROGRESS    6
#define	CLIENT_STATE_CONNECTED_UNSUBSCRIBE_IN_PROGRESS  7
#define	CLIENT_STATE_CONNECTED_RESUBSCRIBE_IN_PROGRESS  8
#define	CLIENT_STATE_CONNECTED_WAIT_FOR_CB_RETURN       9
#define	CLIENT_STATE_DISCONNECTING                      10
#define	CLIENT_STATE_DISCONNECTED_ERROR                 11
#define	CLIENT_STATE_DISCONNECTED_MANUALLY              12
#define	CLIENT_STATE_PENDING_RECONNECT                  13

struct aws_connect_req {
    char aws_host[128];
    uint16_t port;
};

struct aws_connect_rsp {
    uint32_t status; /**< result status, zero is success */
};

struct aws_publish_req {
    char topic[128]; // char array for publish topic
    int msg_len; // length of msg
    char msg[0]; // char array for msg
};

struct aws_publish_rsp {
    uint32_t status; /**< result status, zero is success */
};

struct aws_subscribe_req {
    char topic[128]; // char array for subscribe topic
};

struct aws_subscribe_rsp {
    uint32_t status; /**< result status, zero is success */
};

struct aws_subscribe_indi {
    uint32_t msg_len;
    char msg[0];
};

struct aws_setinterval_req {
    uint32_t interval; // mqtt yield interval in ms
};

struct aws_setinterval_rsp {
    uint32_t status; /**< result status, zero is success */
};

struct aws_getclientstate_req {
};

struct aws_getclientstate_rsp {
    uint32_t status; /**< result status, zero is success */
    uint32_t clientstate;
};

void main(int argc, char *argv[]) 
{
    int ret;
    struct aws_connect_req req;
    memset(req.aws_host, 0 , 128);
    char* aws_host = "a1r1c7pxk55qta-ats.iot.us-west-2.amazonaws.com";
    strncpy(req.aws_host, aws_host, strlen(aws_host));
    req.port=8883;
    int msg_id = 1;

    struct aws_subscribe_req sub_req;
    memset(sub_req.topic, 0, 128);
    char* sub_topic = "inno_test/ctrl";
    strncpy(sub_req.topic, sub_topic, strlen(sub_topic));

    struct aws_subscribe_req sub_req2;
    memset(sub_req2.topic, 0, 128);
    char* sub_topic2 = "inno_test/recv2";
    strncpy(sub_req2.topic, sub_topic2, strlen(sub_topic2));

    struct aws_setinterval_req setint_req;
    setint_req.interval = 5000;

    struct aws_getclientstate_req gcs_req;

    if(argc < 4)
    {
        printf("Invalid Input format: \nEg: ./aws_stw /dev/ttyUSB2 [ssid] [pass]\n");
        return;
    }

	if (init_hio_transport(argv[1]) < 0) {
		printf("Failed to open serial device %s\n", argv[1]);
		return;
	}

    sleep(2);

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, hio_response_reader, NULL);
    sleep(1);
    
    //create wcm interface
    struct wcm_if_create_req cr_req;
    ret = send_command(HIO_GROUP_WCM, WCM_IF_CREATE_REQ, &cr_req, sizeof(cr_req), msg_id++);

    while(gWcmHandle == 99999);
    sleep(1);
    
    printf("Sending scan req\n");
    struct wcm_if_scan_req scan_req ={0,};
    scan_req.handle = gWcmHandle;
    scan_req.max = 10;
    scan_req.wait = 25;
    scan_req.dwell = 25;
    scan_req.rate = 0x100;
    ret = send_command(HIO_GROUP_WCM, WCM_IF_SCAN_REQ, &scan_req, sizeof(scan_req), msg_id++);
    sleep(5);
    
    //send wcm_if_connect request
    struct wcm_if_connect_req conn_req = {0,};
    conn_req.handle = gWcmHandle;
    strcpy(conn_req.ssid, argv[2]);
    strcpy(conn_req.passphrase, argv[3]);

    printf("ssid: %s\n", conn_req.ssid);
    printf("passphrase: %s\n", conn_req.passphrase);
    ret = send_command(HIO_GROUP_WCM, WCM_IF_CONNECT_REQ, &conn_req, sizeof(conn_req), msg_id++);
    
    while(!wifiConnected);

    //send connect HIO command
    printf("Connecting to aws\n");
    ret = send_command(HIO_GROUP_AWS, AWS_CONNECT_REQ, &req, sizeof(req), msg_id++);
    sleep(15);

    hioBusy = 1;
    ret = send_command(HIO_GROUP_AWS, AWS_GETCLIENTSTATE_REQ, &gcs_req, sizeof(gcs_req), msg_id++);
    while(hioBusy);
    while(gClientState != CLIENT_STATE_CONNECTED_IDLE )
    {
        sleep(1);
        ret = send_command(HIO_GROUP_AWS, AWS_GETCLIENTSTATE_REQ, &gcs_req, sizeof(gcs_req), msg_id++);
    }

    //subscribe to inno_test/ctrl
    printf("Subscribing to to inno_test/ctrl\n");
    hioBusy = 1;
    ret = send_command(HIO_GROUP_AWS, AWS_SUBSCRIBE_REQ, &sub_req, sizeof(sub_req), msg_id++);
    while(hioBusy);
    
    hioBusy = 1;
    ret = send_command(HIO_GROUP_AWS, AWS_GETCLIENTSTATE_REQ, &gcs_req, sizeof(gcs_req), msg_id++);
    while(hioBusy);
    while(gClientState != CLIENT_STATE_CONNECTED_IDLE )
    {
        sleep(1);
        hioBusy = 1;
        ret = send_command(HIO_GROUP_AWS, AWS_GETCLIENTSTATE_REQ, &gcs_req, sizeof(gcs_req), msg_id++);
        while(hioBusy);
    }
    
    //subscribe to inno_test/recv2
    printf("Subscribing to to inno_test/recv2\n");
    hioBusy = 1;
    ret = send_command(HIO_GROUP_AWS, AWS_SUBSCRIBE_REQ, &sub_req2, sizeof(sub_req2), msg_id++);
    while(hioBusy);

    hioBusy = 1;
    ret = send_command(HIO_GROUP_AWS, AWS_GETCLIENTSTATE_REQ, &gcs_req, sizeof(gcs_req), msg_id++);
    while(hioBusy);
    while(gClientState != CLIENT_STATE_CONNECTED_IDLE )
    {
        sleep(1);
        hioBusy = 1;
        ret = send_command(HIO_GROUP_AWS, AWS_GETCLIENTSTATE_REQ, &gcs_req, sizeof(gcs_req), msg_id++);
        while(hioBusy);
    }
    
    printf("Setting yield interval\n");
    //set mgtt_yield interval
    hioBusy = 1;
    ret = send_command(HIO_GROUP_AWS, AWS_SETINTERVAL_REQ, &setint_req, sizeof(setint_req), msg_id++);
    while(hioBusy);

    
    //publish message to inno_test/data and inno_test/atad every 3 seconds
    char* msg = malloc(100);
    while(1){
		snprintf(msg, 100,
							"{\"from\":\"Talaria T2\",\"to\":\"AWS\",\"msg\":\"Howdy Ho\",\"msg_id\":%d}",
							msg_id++);
        int msg_len = strlen(msg);
        struct aws_publish_req *pub_req = malloc(sizeof(struct aws_publish_req) + msg_len);
        memset(pub_req->topic, 0, 32);
        char* pub_topic = "inno_test/data";
        strncpy(pub_req->topic, pub_topic, strlen(pub_topic));
        pub_req->msg_len = msg_len;
        memset(pub_req->msg, 0, msg_len);
        strncpy(pub_req->msg, msg, msg_len);
        hioBusy = 1;
        ret = send_command(HIO_GROUP_AWS, AWS_PUBLISH_REQ, pub_req, sizeof(struct aws_publish_req) + msg_len, msg_id);
        free(pub_req);
        while(hioBusy);

        struct aws_publish_req *pub_req2 = malloc(sizeof(struct aws_publish_req) + msg_len);
        memset(pub_req2->topic, 0, 32);
        char* pub_topic2 = "inno_test/atad";
        strncpy(pub_req2->topic, pub_topic2, strlen(pub_topic2));
        pub_req2->msg_len = msg_len;
        memset(pub_req2->msg, 0, msg_len);
        strncpy(pub_req2->msg, msg, msg_len);
        hioBusy = 1;
        ret = send_command(HIO_GROUP_AWS, AWS_PUBLISH_REQ, pub_req2, sizeof(struct aws_publish_req) + msg_len, msg_id);
        free(pub_req2);
        while(hioBusy);

        sleep(3);
    }
    
    deinit_hio_transport();
}

static void* hio_response_reader(void *p) {
    uint32_t msg_count = 0;
	while (!gExit) {
		int read_count = 0;
        uint32_t res_len;
		char msg_len[4] = {0,};
		//make sure we read atleast two bytes
		while(read_count < 2)
		{
			read_count += read_hio_message(msg_len+read_count, 2-read_count);
		}
		msg_len[2] = 0x00;
		msg_len[3] = 0x00;

        res_len = msg_len[0] & 0x000000FF;
		if (msg_len[1] != 0) {
			res_len = ((uint8_t)msg_len[0]&0xFF) + (msg_len[1]<<8) & 0xFFFF;
		}
        sleep(0.2);
        if(res_len > 0)
        {
            char *res_payload = malloc(res_len+1);
            read_count = 0;
            while(read_count < res_len)
			{
				read_count += read_hio_message((res_payload)+read_count, res_len-read_count);
			}
            process_response(res_len, res_payload);
        }
    }
    return NULL;
}

static void process_response(uint32_t res_len, char* res_payload)
{
    struct hio_res_header res_header = {0,};
    memcpy(&res_header, res_payload, sizeof(struct hio_res_header));
    uint32_t payloadLength = res_len - sizeof(struct hio_res_header);
    if(res_header.group_id == HIO_GROUP_WCM)
    {
        switch(res_header.res_code)
        {
            case WCM_IF_CREATE_RSP:
            {
                struct wcm_if_create_rsp *rsp = malloc(sizeof(struct wcm_if_create_rsp));
                memcpy(rsp, res_payload+sizeof(struct hio_res_header), sizeof(struct wcm_if_create_rsp));
                gWcmHandle = rsp->handle;
                break;
            }
            case WCM_IF_CONNECT_RSP:
            {
                struct wcm_if_connect_rsp *rsp = malloc(sizeof(struct wcm_if_connect_rsp));
                memcpy(rsp, res_payload+sizeof(struct hio_res_header), sizeof(struct wcm_if_connect_rsp));
                if(rsp->status == 0)
                {
                    printf("WCM Connect Success!\n");
                    wifiConnected = 1;
                }
                break;
            }
            case WCM_IF_SCAN_RSP:
            {   struct wcm_if_scan_rsp *scan_rsp = malloc(payloadLength);
                memcpy(scan_rsp, res_payload + sizeof(struct hio_res_header), payloadLength);
                if(scan_rsp->status==0)
                    printf("Found: %d nets\n", scan_rsp->num);
                free(scan_rsp);
                break;
            }
            case WCM_IF_SCAN_IND:
            {
                struct wcm_if_scan_ind *ind = malloc(payloadLength);
                memcpy(ind, res_payload + sizeof(struct hio_res_header), payloadLength);
                int ssidbuff_len = payloadLength - sizeof(struct wcm_if_scan_ind);
                char *ssidbuff[ssidbuff_len];
                memset(ssidbuff, 0, ssidbuff_len);
                memcpy(ssidbuff, (res_payload + sizeof(struct hio_res_header) + sizeof(struct wcm_if_scan_ind)), ssidbuff_len);
                struct wifi_ssid *ssid = malloc(payloadLength -sizeof(struct wcm_if_scan_ind));
                wifi_netinfo_get_ssid(ssidbuff, ssidbuff_len, ssid);
                printf("ssid: %s\n", ssid->ws_ssid);
                free(ind);
	            break;
                
            }
            default:
                printf("Group Code: 0x%X\n", res_header.group_id);
                printf("Response Code: 0x%X\n", res_header.res_code);
                break;
        }
    }
    else if(res_header.group_id == HIO_GROUP_AWS)
    {
        switch(res_header.res_code)
        {
            case AWS_CONNECT_RSP:
            {
                struct aws_connect_rsp *rsp = malloc(sizeof(struct aws_connect_rsp));
                memcpy(rsp, res_payload+sizeof(struct hio_res_header), sizeof(struct aws_connect_rsp));
                if(rsp->status == 0)
                {
                    printf("HIO MQTT Connect command sent\n");
                }
                free(rsp);
                hioBusy = 0;
                break;
            }
            case AWS_PUBLISH_RSP:
            {
                struct aws_publish_rsp *rsp = malloc(sizeof(struct aws_publish_rsp));
                memcpy(rsp, res_payload+sizeof(struct hio_res_header), sizeof(struct aws_publish_rsp));
                if(rsp->status == 0)
                {
                    printf("MQTT Publish Success!\n");
                }
                else
                {
                    printf("MQTT Publish fail: %d\n", rsp->status);
                }
                free(rsp);
                hioBusy = 0;
                break;
            }
            case AWS_SUBSCRIBE_RSP:
            {
                struct aws_subscribe_rsp *rsp = malloc(sizeof(struct aws_subscribe_rsp));
                memcpy(rsp, res_payload+sizeof(struct hio_res_header), sizeof(struct aws_subscribe_rsp));
                if(rsp->status == 0)
                {
                    printf("MQTT Subscribe Success!\n");
                }
                else
                {
                    printf("MQTT Subscribe fail: %d\n", rsp->status);
                }
                hioBusy = 0;
                free(rsp);
                break;
            }
            case AWS_SUBSCRIBE_IND:
            {
                printf("Subscribe Message\n");
                struct aws_subscribe_indi *rsp = malloc(sizeof(struct aws_subscribe_indi) + (res_len-sizeof(struct hio_res_header)));
                memcpy(rsp, res_payload+sizeof(struct hio_res_header), sizeof(struct aws_subscribe_indi) + (res_len-sizeof(struct hio_res_header)));
                printf("Topic: %s\n", rsp->msg);
                //since topic and actual msg is separated by a \0 we can use strlen to get the offset
                int offset = strlen(rsp->msg)+1;
                printf("msg payload:\n %s\n", (rsp->msg)+offset);
                free(rsp);
                break;
            }
            case AWS_SETINTERVAL_RSP:
            {
                struct aws_setinterval_rsp *rsp = malloc(sizeof(struct aws_setinterval_rsp));
                memcpy(rsp, res_payload+sizeof(struct hio_res_header), sizeof(struct aws_setinterval_rsp));
                if(rsp->status == 0)
                {
                    printf("MQTT Yield Interval set!\n");
                }
                else
                {
                    printf("Failed to set MQTT Yield Interval: %d\n", rsp->status);
                }
                free(rsp);
                hioBusy = 0;
                break;
            }
            case AWS_GETCLIENTSTATE_RSP:
            {
                struct aws_getclientstate_rsp *rsp = malloc(sizeof(struct aws_getclientstate_rsp));
                memcpy(rsp, res_payload+sizeof(struct hio_res_header), sizeof(struct aws_getclientstate_rsp));
                if(rsp->status == 0)
                {
                    printf("Client state: %d\n", rsp->clientstate);
                    gClientState = rsp->clientstate;
                }
                else
                {
                    printf("get client state fail\n");
                }
                free(rsp);
                hioBusy = 0;
                break;
            }
            default:
                printf("Response Code: 0x%X\n", res_header.res_code);
                break;
       }
    }
    else
    {
        printf("Group Code: 0x%X\n", res_header.group_id);
        printf("Response Code: 0x%X\n", res_header.res_code);
    }
    
}