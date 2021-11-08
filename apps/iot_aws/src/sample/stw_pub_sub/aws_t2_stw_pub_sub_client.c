#include <kernel/os.h>
#include "string.h"
#include "aws_iot_mqtt_client_interface.h"
#include <wifi/wcm.h>
#include "cert/cert.h"
#include "aws_iot_json_utils.h"
#include "aws_hio.h"


#define INPUT_PARAMETER_SSID "ssid"
#define INPUT_PARAMETER_PASSPHRASE "passphrase"
#define INPUT_PARAMETER_AWS_URL "aws_host"
#define INPUT_PARAMETER_AWS_PORT "aws_port"
#define INPUT_PARAMETER_AWS_PUBLISH_TOPIC "publish_topic"
#define INPUT_PARAMETER_AWS_SUBSCRIBE_TOPIC "subscribe_topic"

OS_APPINFO {.stack_size=4096};


extern void wcm_hio_init(void);
extern unsigned int wcm_hio_set_power_mode(enum wcm_power_mode mode);
extern void sock_hio_init(void);
extern void mdns_hio_init(void);
extern bool lwip_running(void);

static void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data);
static int init_and_connect_aws_iot();
static int start_sending_messages_to_aws(char *ptopic, char *cPayload, int len);
static void on_new_subscribe_message_from_aws(AWS_IoT_Client *pClient,
		char *pTopicName, uint16_t topicNameLen,
		IoT_Publish_Message_Params *pParams, void *pClientData);


static AWS_IoT_Client *gpclient;
char *subscribe_topic;

bool connect_to_aws = false;
uint32_t yield_period = 5000;

char aws_host[128];
uint32_t aws_port;

static void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data) {

	IoT_Error_t rc = FAILURE;

	if (NULL == pClient) {
		return;
	}

	if (aws_iot_is_autoreconnect_enabled(pClient)) {
		os_printf(
				"Auto Reconnect is enabled, Reconnecting attempt will start now\n");
	} else {
		os_printf("Auto Reconnect not enabled. Starting manual reconnect...\n");
		rc = aws_iot_mqtt_attempt_reconnect(pClient);
		if (NETWORK_RECONNECTED == rc) {
			os_printf("Manual Reconnect Successful\n");
		} else {
			os_printf("Manual Reconnect Failed - %d\n", rc);
		}
	}

	return;
}

static int init_and_connect_aws_iot() {

	int rc;
	IoT_Client_Init_Params *mqttInitParams;

	mqttInitParams = os_alloc(sizeof(IoT_Client_Init_Params));
	mqttInitParams->enableAutoReconnect = true;

	mqttInitParams->pHostURL = aws_host;
	mqttInitParams->port = aws_port;

	mqttInitParams->pRootCALocation = root_ca;
	mqttInitParams->pDeviceCertLocation = device_cert;
	mqttInitParams->pDevicePrivateKeyLocation = device_pkey;
	mqttInitParams->mqttCommandTimeout_ms = 20000;
	mqttInitParams->tlsHandshakeTimeout_ms = 5000;
	mqttInitParams->isSSLHostnameVerify = false;
	mqttInitParams->disconnectHandler = disconnect_callback_handler;
	mqttInitParams->disconnectHandlerData = NULL;

	gpclient = os_alloc(sizeof(AWS_IoT_Client));
	rc = aws_iot_mqtt_init(gpclient, mqttInitParams);
	if (SUCCESS != rc) {
		os_printf("aws_iot_mqtt_init returned error : %d\n", rc);
		return rc;
	}

	IoT_Client_Connect_Params *connectParams = os_alloc(
			sizeof(IoT_Client_Connect_Params));
	*connectParams = iotClientConnectParamsDefault;

	connectParams->keepAliveIntervalInSec = 60;
	connectParams->isCleanSession = false;
	connectParams->MQTTVersion = MQTT_3_1_1;
	connectParams->pClientID = "c-sdk-1";
	connectParams->clientIDLen = (uint16_t) strlen("c-sdk-1");
	connectParams->isWillMsgPresent = false;
	connectParams->usernameLen = 0;
	connectParams->passwordLen = 0;

	os_printf(
			"Connecting...heap[%d] max contentlen[%d] sizeof IoT_Publish_Message_Params (%d)\n",
			os_avail_heap(), MBEDTLS_SSL_MAX_CONTENT_LEN,
			sizeof(IoT_Publish_Message_Params));

	rc = aws_iot_mqtt_connect(gpclient, connectParams);
	os_free(mqttInitParams);
	return rc;
}

static int parse_received_message(char *msg_received, int payloadLen) {
	int i, r;
	jsmn_parser p;
	jsmntok_t t[16];

	jsmn_init(&p);
	r = jsmn_parse(&p, msg_received, payloadLen, t, sizeof(t) / sizeof(t[0]));
	if (r < 0) {
		os_printf("Failed to parse JSON: %d\n", r);
		return 1;
	}
	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		os_printf("Object expected\n");
		return 1;
	}
	for (i = 1; i < r; i++) {
		
		if (jsoneq(msg_received, &t[i], "from") == 0) {
			os_printf("- from: %.*s\n", t[i + 1].end - t[i + 1].start,
					msg_received + t[i + 1].start);
			i++;
		} else if (jsoneq(msg_received, &t[i], "to") == 0) {
			os_printf("- to: %.*s\n", t[i + 1].end - t[i + 1].start,
					msg_received + t[i + 1].start);
			i++;
		} else if (jsoneq(msg_received, &t[i], "msg") == 0) {
			os_printf("- message: %.*s\n", t[i + 1].end - t[i + 1].start,
					msg_received + t[i + 1].start);
			i++;
		} else
			i++;
	}
	return 0;
}

static void on_new_subscribe_message_from_aws(AWS_IoT_Client *pClient,
		char *pTopicName, uint16_t topicNameLen,
		IoT_Publish_Message_Params *pParams, void *pClientData) {
	char *msg_received;
	struct pfrag *frag = NULL;

	os_printf("Topic Name: %.*s\n", topicNameLen, pTopicName);

	msg_received = os_alloc(pParams->payloadLen + 1);
	memset(msg_received, 0, pParams->payloadLen + 1);
	memcpy(msg_received, pParams->payload, pParams->payloadLen);
	parse_received_message(msg_received, pParams->payloadLen);


	//combine the topic and msg payload with a \0 separator
	char *combined_msg = os_alloc(topicNameLen + 1 + pParams->payloadLen + 1);
	memset(combined_msg, 0, topicNameLen + 1 + pParams->payloadLen + 1);
	memcpy(combined_msg, pTopicName, topicNameLen);
	memcpy((combined_msg+topicNameLen+1), msg_received, pParams->payloadLen);

	frag = pfrag_alloc(topicNameLen + 1 + pParams->payloadLen + 1);
	memcpy(pfrag_insert_tail(frag,topicNameLen + 1 + pParams->payloadLen + 1), combined_msg, topicNameLen + 1 + pParams->payloadLen + 1);

	struct packet *ind;
	ind = create_aws_subscribe_ind(topicNameLen + 1 + pParams->payloadLen + 1, combined_msg);
	packet_add_frag(ind, frag);

	hio_write_msg(ind, HIO_GROUP_AWS, AWS_SUBSCRIBE_IND, 0);

	os_free(msg_received);
}

static int start_sending_messages_to_aws(char *ptopic, char *cPayload, int len) {
	int rc = 0;

	IoT_Publish_Message_Params *paramsQOS0 = os_alloc(
			sizeof(IoT_Publish_Message_Params));

	paramsQOS0->qos = QOS0;
	paramsQOS0->payload = (void *) cPayload;
	paramsQOS0->isRetained = 0;
	paramsQOS0->payloadLen = len;
	rc = aws_iot_mqtt_publish(gpclient, ptopic, strlen(ptopic), paramsQOS0);
	os_printf("message status[%d] topic[%s] msg[%s]\n", rc, ptopic, cPayload);
	os_free(paramsQOS0);
	return rc;
}

static int get_mqtt_client_state()
{
	//aws_iot_mqtt_internal_flushBuffers( gpclient );
	ClientState clientState;
	clientState = (int)aws_iot_mqtt_get_client_state(gpclient);

	return clientState;
}
/**********************************************************************************************************/
/* aws HIO functions */

static struct packet* aws_connect(void *ctx, struct packet *msg)
{
    int status = 0;
	struct aws_connect_req * conn_req = packet_data(msg);
	memcpy(aws_host, conn_req->aws_host, 128);
	aws_port = conn_req->port;
	os_printf("aws_host: %s\n", aws_host);
	os_printf("port: %d\n", aws_port);
	connect_to_aws = true;
    return hio_response_status(status);
}


static struct packet* aws_publish(void *ctx, struct packet *msg)
{
    int status = 0;
    os_printf("Publish\n");
	struct aws_publish_req * pub_req = packet_data(msg);
	char *ptopic = pub_req->topic;
	int msg_len = pub_req->msg_len;
	char *pub_msg = os_alloc(msg_len+1);
	memset(pub_msg, 0, msg_len+1);
	strncpy(pub_msg, pub_req->msg, msg_len);

	if (!ptopic)
		ptopic = "inno_test/data";

	os_printf("msg: %s\n", pub_msg);
	os_printf("pub_topic: %s\n", pub_req->topic);
	status = start_sending_messages_to_aws(ptopic, pub_msg, msg_len);
	os_free(pub_msg);
    return hio_response_status(status);
}

static struct packet* aws_subscribe(void *ctx, struct packet *msg)
{
    int status = 0;

	struct aws_subscribe_req * sub_req = packet_data(msg);
	
	subscribe_topic = os_alloc(strlen(sub_req->topic));
	strcpy(subscribe_topic, sub_req->topic);
	if (!subscribe_topic)
	{
		subscribe_topic = "inno_test/ctrl";
	}

	status = aws_iot_mqtt_subscribe(gpclient, subscribe_topic,
			strlen(subscribe_topic), QOS0,
			on_new_subscribe_message_from_aws, NULL);

	os_printf("Subscribed to topic [%s] ret[%d] qos[%d] topic len[%d]\n",
			subscribe_topic, status, QOS0, strlen(subscribe_topic));
    return hio_response_status(status);
}

static struct packet* aws_setinterval(void *ctx, struct packet *msg)
{
    int status = 0;

	struct aws_setinterval_req * si_req = packet_data(msg);
	yield_period = si_req->interval;

    return hio_response_status(status);
}

static struct packet* aws_getclientstate(void *ctx, struct packet *msg)
{
    int clientstate = get_mqtt_client_state();
	
	struct packet *rsp;
	rsp = create_aws_getclientstate_rsp(clientstate);

	hio_write_msg(rsp, HIO_GROUP_AWS, AWS_GETCLIENTSTATE_RSP, 0);
	
	return NULL;
}

/**********************************************************************************************************/
/*
 * [[[cog
 * import cog, generate
 * generate.api(generate.Emitter(cog.out), 'aws')
 * ]]] */
 static const struct hio_api aws_api = {
     .group = HIO_GROUP_AWS,
     .num_handlers = 5,
     .handler = {
         aws_connect,
         aws_publish,
         aws_subscribe,
         aws_setinterval,
		 aws_getclientstate,
     }
 };
/* [[[end]]] */
/**********************************************************************************************************/



int main() {

	int rc;

	wcm_hio_init();
    sock_hio_init();
    mdns_hio_init();

	hio_api_init(&aws_api, NULL);

	/* Enable device suspend (deep sleep) via boot argument */
	if (os_get_boot_arg_int("suspend", 0) != 0)
		os_suspend_enable();

	// wait for hio command before connecting
	while(!connect_to_aws) {
		os_msleep(1000);
	}

	rc = init_and_connect_aws_iot();

	if(rc)
	{
		return 0;
	}

	while(true){
		aws_iot_mqtt_yield(gpclient, 100);
		os_msleep(yield_period);
	}

	aws_iot_mqtt_disconnect(gpclient);
	os_free (gpclient);
	return 0;

}