#include <kernel/os.h>
#include "string.h"
#include "errno.h"
#include "aws_iot_mqtt_client_interface.h"
#include <wifi/wcm.h>
#include "aws_iot_json_utils.h"
#include "fs_utils.h"

#define INPUT_PARAMETER_SSID "ssid"
#define INPUT_PARAMETER_PASSPHRASE "passphrase"
#define INPUT_PARAMETER_AWS_URL "aws_host"
#define INPUT_PARAMETER_AWS_PORT "aws_port"
#define INPUT_PARAMETER_AWS_THING_NAME "thing_name"
#define INPUT_PARAMETER_AWS_PUBLISH_TOPIC "publish_topic"
#define INPUT_PARAMETER_AWS_SUBSCRIBE_TOPIC "subscribe_topic"

#define FS_ADDR	0x1c0000
#define FS_SIZE	0x40000

// read the boot args
const char *ssid;
const char *passphrase;
const char *hwaddr;

static struct wcm_handle *h = NULL;
static bool ap_link_up = false;
static bool ap_got_ip = false;

OS_APPINFO {.stack_size=4096};

/**
 * WCM notification callback function pointer
 */
static void wcm_notify_callback(void *ctx, struct os_msg *msg)
{
    switch(msg->msg_type)
    {
        case(WCM_NOTIFY_MSG_LINK_UP):
            os_printf("wcm_notify_cb to App Layer - WCM_NOTIFY_MSG_LINK_UP\n");
            ap_link_up = true;
            break;

        case(WCM_NOTIFY_MSG_LINK_DOWN):
            os_printf("wcm_notify_cb to App Layer - WCM_NOTIFY_MSG_LINK_DOWN\n");
            ap_link_up = false;
            ap_got_ip = false;
            break;

        case(WCM_NOTIFY_MSG_ADDRESS):
            os_printf("wcm_notify_cb to App Layer - WCM_NOTIFY_MSG_ADDRESS\n");
            ap_got_ip = true;
            break;

        case(WCM_NOTIFY_MSG_DISCONNECT_DONE):
            os_printf("wcm_notify_cb to App Layer - WCM_NOTIFY_MSG_DISCONNECT_DONE\n");
            ap_got_ip = false;
            break;

        case(WCM_NOTIFY_MSG_CONNECTED):
            os_printf("wcm_notify_cb to App Layer - WCM_NOTIFY_MSG_CONNECTED\n");
            break;

        default:
            break;
    }
    os_msg_release(msg);
}

/**
 * Calls WCM APIs to asynchronously connect to a WiFi network.
 * @param ssid Pointer to string with the SSID of the desired network.
 * @param passphrase The passphrase of the desired network.
 *
 * @return Returns zero on success, negative error code from 
 * errorno.h in case of an error.
 */
int wifi_main(const char *ssid, const char *pw)
{
    int status;
    os_printf("\n\rWiFi Details  SSID: %s, PASSWORD: %s\n\r", ssid, pw);

    h = wcm_create(NULL);
    if( h == NULL ){
        os_printf(" wcm_notify_enable failed.\n");
        return -ENOMEM;
    }
    os_msleep(2000);

    wcm_notify_enable(h, wcm_notify_callback, NULL);

    /* async connect to a WiFi network */
    os_printf("Connecting to WiFi...\n");
    status = wcm_add_network(h, ssid, NULL, pw);
    os_printf("add network status: %d\n", status);
    if(status != 0){
        os_printf("adding network Failed\n");
        /* can fail due to, already busy, no memory, or badly formatted password */
        return status;
    }

    os_printf("added network successfully, will try connecting..\n");
    status = wcm_auto_connect(h, 1);
    os_printf("connecting to network status: %d\n", status);
    if(status != 0){
        os_printf("trying to connect to network Failed\n");
        /* can fail due to, already busy, no memory */
        return status;
    }
    return status;
}

/**
 * Disconnect and cleanup a WiFi Connection Manager interface.
 * @param state_connected connection state
 */
void wifi_destroy(bool state_connected)
{
    int status;
    if(state_connected){
        status = wcm_auto_connect(h, 0);
        if(status != 0){
            os_printf("trying to disconnect to network Failed\n");
        }
    }
    wcm_destroy(h);
}

static void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data);
static int init_platform();
static int validate_inputs();
static int init_and_connect_aws_iot();
static int start_sending_messages_to_aws(char *ptopic, char *cPayload, int len);
static void on_new_subscribe_message_from_aws(AWS_IoT_Client *pClient,
		char *pTopicName, uint16_t topicNameLen,
		IoT_Publish_Message_Params *pParams, void *pClientData);

static AWS_IoT_Client *gpclient;
char *subscribe_topic;


char *aws_root_ca;
char *aws_device_pkey;
char *aws_device_cert;

int read_certs()
{
	int root_ca_len = 0;
	int pkey_len = 0;
	int cert_len = 0;

	aws_root_ca = utils_file_get("/root/certs/aws/app/aws_root_ca", &root_ca_len);
	//os_printf("read_certs() : root ca file size is %d bytes\n", root_ca_len);
	if(aws_root_ca == NULL)
	{
		return -1;
	}

	aws_device_pkey = utils_file_get("/root/certs/aws/app/aws_device_pkey", &pkey_len);
	//os_printf("read_certs() : device private key file zize is %d bytes\n", pkey_len);
	if(aws_device_pkey == NULL)
	{
		return -1;
	}

	aws_device_cert = utils_file_get("/root/certs/aws/app/aws_device_cert", &cert_len);
	//os_printf("read_certs() : device cert file size is %d bytes\n", cert_len);
	if(aws_device_cert == NULL)
	{
		return -1;
	}

	return 0;
}

void free_certs()
{
	os_free(aws_root_ca);
	os_free(aws_device_pkey);
	os_free(aws_device_cert);
}

int main() {

	int rc;

	os_printf("Mounting file system\n");
	rc = utils_mount_rootfs();

	rc = read_certs();
	if(rc < 0) {
		os_printf("Reading the certs and key in rooFS failed, cant proceed. \n"
				"Please program needed files in rootFS...\n");
		return rc;
	}

	os_printf("read_certs() success\n");

	if ((rc = validate_inputs()) != 0) {
		os_printf(
				"basic verification failed. ret:%d. Usage: %s=<wifi network name>"
						" %s=<wifi network password> %s=<aws url> %s=<aws_port, def 8883> %s=<Thing Name>\n",
				rc,
				INPUT_PARAMETER_SSID, INPUT_PARAMETER_PASSPHRASE,
				INPUT_PARAMETER_AWS_URL, INPUT_PARAMETER_AWS_PORT,
				INPUT_PARAMETER_AWS_THING_NAME);
		return rc;
	}
	rc = init_platform();
	if (rc) {
		os_printf("init platform failed. ret:%d\n", rc);
		return rc;
	}

        while(!ap_got_ip) {
        os_msleep(1000);
        }

	rc = init_and_connect_aws_iot();

	/* Enable device suspend (deep sleep) via boot argument */
	if (os_get_boot_arg_int("suspend", 0) != 0)
		os_suspend_enable();

	/*subscribe to control topic*/
	if (!rc) {
		subscribe_topic = (char *)os_get_boot_arg_str(
		INPUT_PARAMETER_AWS_SUBSCRIBE_TOPIC);
		if (!subscribe_topic)
			subscribe_topic = "inno_test/ctrl";
		rc = aws_iot_mqtt_subscribe(gpclient, subscribe_topic,
				strlen(subscribe_topic), QOS0,
				on_new_subscribe_message_from_aws, NULL);
		os_printf("Subscribed to topic [%s] ret[%d] qos[%d] topic len[%d]\n",
				subscribe_topic, rc, QOS0, strlen(subscribe_topic));
	}
	if (!rc) {
		char msg_payload[100];
		int len;
		int message_id = 0;
		char *ptopic = (char *)os_get_boot_arg_str(INPUT_PARAMETER_AWS_PUBLISH_TOPIC);
		if (!ptopic)
			ptopic = "inno_test/data";
		while (1) {
			len =
					snprintf(msg_payload, sizeof(msg_payload),
							"{\"from\":\"Talaria T2\",\"to\":\"AWS\",\"msg\":\"Howdy Ho\",\"msg_id\":%d}",
							++message_id);
			start_sending_messages_to_aws(ptopic, msg_payload, len);
			aws_iot_mqtt_yield(gpclient, 100);

			os_msleep(10000);
		}
	}
	aws_iot_mqtt_disconnect(gpclient);
	os_free (gpclient);
	return 0;
}

static int init_platform() {

    int ret;
    ssid = os_get_boot_arg_str(INPUT_PARAMETER_SSID);
    passphrase = os_get_boot_arg_str(INPUT_PARAMETER_PASSPHRASE);
	ret = wifi_main(ssid,passphrase);
    if(ret != 0) {
        os_printf("main -- WiFi Connection Failed due to WCM returning error \n");
        wifi_destroy(0);
    }
    return ret;
}

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

static int validate_inputs() {

	if (NULL == os_get_boot_arg_str(INPUT_PARAMETER_SSID)) {
		os_printf("[%s] missing\n", INPUT_PARAMETER_SSID);
		return -1;
	}
	if (NULL == os_get_boot_arg_str(INPUT_PARAMETER_AWS_URL)) {
		os_printf("[%s] missing\n", INPUT_PARAMETER_AWS_URL);
		return -2;
	}
	if (NULL == os_get_boot_arg_str(INPUT_PARAMETER_AWS_THING_NAME)) {
		os_printf("[%s] missing\n", INPUT_PARAMETER_AWS_THING_NAME);
		return -3;
	}
	return 0;
}

static int init_and_connect_aws_iot() {

	int rc;
	IoT_Client_Init_Params *mqttInitParams;

	mqttInitParams = os_alloc(sizeof(IoT_Client_Init_Params));
	mqttInitParams->enableAutoReconnect = true;

	mqttInitParams->pHostURL = (char *)os_get_boot_arg_str(INPUT_PARAMETER_AWS_URL);
	mqttInitParams->port = os_get_boot_arg_int(INPUT_PARAMETER_AWS_PORT, 8883);

	mqttInitParams->pRootCALocation = aws_root_ca;
	mqttInitParams->pDeviceCertLocation = aws_device_cert;
	mqttInitParams->pDevicePrivateKeyLocation = aws_device_pkey;
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
	msg_received = os_alloc(pParams->payloadLen + 1);
	memcpy(msg_received, pParams->payload, pParams->payloadLen);
	parse_received_message(msg_received, pParams->payloadLen);
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
