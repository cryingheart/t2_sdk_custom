#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"

#include <kernel/os.h>
#include "string.h"
#include "errno.h"
#include <wifi/wcm.h>
#include "aws_iot_json_utils.h"
#include "fs_utils.h"

#define INPUT_PARAMETER_SSID "ssid"
#define INPUT_PARAMETER_PASSPHRASE "passphrase"
#define INPUT_PARAMETER_AWS_URL "aws_host"
#define INPUT_PARAMETER_AWS_PORT "aws_port"
#define INPUT_PARAMETER_AWS_THING_NAME "thing_name"

#define AWS_IOT_MY_THING_NAME os_get_boot_arg_str(INPUT_PARAMETER_AWS_THING_NAME)

#define ROOMTEMPERATURE_UPPERLIMIT 32
#define ROOMTEMPERATURE_LOWERLIMIT 25
#define STARTING_ROOMTEMPERATURE ROOMTEMPERATURE_LOWERLIMIT

#define MAX_LENGTH_OF_UPDATE_JSON_BUFFER 200

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

static int init_platform();
static int init_and_connect_aws_iot(jsonStruct_t *windowActuator);
static int validate_inputs();

static AWS_IoT_Client *gpclient;
static int deltaChange;

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

static void simulateRoomTemperature(int *pRoomTemperature) {

	if (*pRoomTemperature >= ROOMTEMPERATURE_UPPERLIMIT) {
		deltaChange = -1;
	} else if (*pRoomTemperature <= ROOMTEMPERATURE_LOWERLIMIT) {
		deltaChange = 1;
	}

	*pRoomTemperature += deltaChange;
}

static void ShadowUpdateStatusCallback(const char *pThingName,
		ShadowActions_t action, Shadow_Ack_Status_t status,
		const char *pReceivedJsonDocument, void *pContextData) {

	if (SHADOW_ACK_TIMEOUT == status) {
		os_printf("Update Timeout--\n");
	}

	else if (SHADOW_ACK_REJECTED == status) {
		os_printf("Update Rejected\n");
	}

	else if (SHADOW_ACK_ACCEPTED == status) {
		os_printf("Update Accepted !!\n");
	}
}

static void windowActuate_Callback(const char *pJsonString,
		uint32_t JsonStringDataLen, jsonStruct_t *pContext) {

	if (pContext != NULL) {
		os_printf("Delta - Window state changed to %d\n",
				*(bool *) (pContext->pData));
	}
}

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

	char *JsonDocumentBuffer = os_alloc(MAX_LENGTH_OF_UPDATE_JSON_BUFFER);
	size_t sizeOfJsonDocumentBuffer = MAX_LENGTH_OF_UPDATE_JSON_BUFFER;
	int temperature = 0;

	bool windowOpen = true;
	jsonStruct_t *windowActuator = os_zalloc(sizeof(jsonStruct_t));
	windowActuator->cb = windowActuate_Callback;
	windowActuator->pData = &windowOpen;
	windowActuator->dataLength = sizeof(bool);
	windowActuator->pKey = "windowOpen";
	windowActuator->type = SHADOW_JSON_BOOL;

	jsonStruct_t *temperatureHandler = os_alloc(sizeof(jsonStruct_t));
	temperatureHandler->cb = NULL;
	temperatureHandler->pKey = "temperature";
	temperatureHandler->pData = &temperature;
	temperatureHandler->dataLength = sizeof(int);
	temperatureHandler->type = SHADOW_JSON_INT32;

	if ((rc = validate_inputs()) != 0) {
		os_printf(
				"Basic verification failed. ret:%d. Usage: %s=<wifi network name>"
						" %s=<wifi network password> %s=<aws url> %s=<aws_port, def 8883> %s=<Thing Name>\n",
				rc, INPUT_PARAMETER_SSID, INPUT_PARAMETER_PASSPHRASE,
				INPUT_PARAMETER_AWS_URL, INPUT_PARAMETER_AWS_PORT,
				INPUT_PARAMETER_AWS_THING_NAME);
		return rc;
	}
	rc = init_platform();
	if (rc) {
		os_printf("init platform failed. ret:%d\n", rc);
		return rc;
	}

	/* Enable device suspend (deep sleep) via boot argument */
	if (os_get_boot_arg_int("suspend", 0) != 0)
		os_suspend_enable();

        while(!ap_got_ip) {
        os_msleep(1000);
        }

	rc = init_and_connect_aws_iot(windowActuator);
	os_printf("init_and_connect_aws_iot. ret:%d\n", rc);

	if (!rc) {
		temperature = STARTING_ROOMTEMPERATURE;

		// loop and publish a change in temperature
		while (NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc
				|| SUCCESS == rc) {
			rc = aws_iot_shadow_yield(gpclient, 500);
			if (NETWORK_ATTEMPTING_RECONNECT == rc) {
				os_msleep(100);
				// If the client is attempting to reconnect we will skip the rest of the loop.
				continue;
			}
			simulateRoomTemperature(&temperature);

			rc = aws_iot_shadow_init_json_document(JsonDocumentBuffer,
					sizeOfJsonDocumentBuffer);
			if (SUCCESS == rc) {
				rc = aws_iot_shadow_add_reported(JsonDocumentBuffer,
						sizeOfJsonDocumentBuffer, 2, temperatureHandler,
						windowActuator);
				if (SUCCESS == rc) {
					rc = aws_iot_finalize_json_document(JsonDocumentBuffer,
							sizeOfJsonDocumentBuffer);
					if (SUCCESS == rc) {
						os_printf("Update Shadow: %s\n", JsonDocumentBuffer);
						rc = aws_iot_shadow_update(gpclient, AWS_IOT_MY_THING_NAME,
								JsonDocumentBuffer, ShadowUpdateStatusCallback,
								NULL, 4, true);
					}
				}
			}
			os_msleep(3000);
		}
	}
	if (SUCCESS != rc) {
		os_printf("An error occurred in the loop %d\n", rc);
	}

	os_printf("Disconnecting\n");
	rc = aws_iot_shadow_disconnect(gpclient);

	if (SUCCESS != rc) {
		os_printf("Disconnect error %d\n", rc);
	}

	return rc;
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

static int init_and_connect_aws_iot(jsonStruct_t *windowActuator) {

	int rc;

	ShadowInitParameters_t *sp = os_zalloc(sizeof(ShadowInitParameters_t));
	sp->pHost = (char *)os_get_boot_arg_str(INPUT_PARAMETER_AWS_URL);
	sp->port = os_get_boot_arg_int(INPUT_PARAMETER_AWS_PORT, 8883);
	sp->pClientCRT = aws_device_cert;
	sp->pClientKey = aws_device_pkey;
	sp->pRootCA = aws_root_ca;

	gpclient = os_alloc(sizeof(AWS_IoT_Client));
	rc = aws_iot_shadow_init(gpclient, sp);
	if (SUCCESS != rc) {
		os_printf("Shadow Connection Error ret:%d\n", rc);
		return rc;
	}

	ShadowConnectParameters_t *scp = os_zalloc(
			sizeof(ShadowConnectParameters_t));
	scp->pMyThingName = (char *)AWS_IOT_MY_THING_NAME;
	scp->pMqttClientId = (char *)AWS_IOT_MY_THING_NAME;
	scp->mqttClientIdLen = (uint16_t) strlen(AWS_IOT_MY_THING_NAME);

	os_printf("Shadow Connect\n");
	rc = aws_iot_shadow_connect(gpclient, scp);
	if (SUCCESS != rc) {
		os_printf("Shadow Connection Error ret:%d\n", rc);
		return rc;
	}
	os_printf("Shadow Connected\n");
	rc = aws_iot_shadow_set_autoreconnect_status(gpclient, true);
	if (SUCCESS != rc) {
		os_printf("Unable to set Auto Reconnect to true - %d\n", rc);
		return rc;
	}

	rc = aws_iot_shadow_register_delta(gpclient, windowActuator);

	if (SUCCESS != rc) {
		os_printf("Shadow Register Delta Error ret:%d\n", rc);
	}
	os_free(sp);
	os_free(scp);
	return rc;
}
