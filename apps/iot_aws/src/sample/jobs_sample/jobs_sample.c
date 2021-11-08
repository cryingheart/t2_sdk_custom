/*
 * Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 *
 * This example takes the parameters from the aws_iot_config.h file and establishes
 * a connection to the AWS IoT MQTT Platform. It performs several operations to
 * demonstrate the basic capabilities of the AWS IoT Jobs platform.
 *
 * If all the certs are correct, you should see the list of pending Job Executions
 * printed out by the iot_get_pending_callback_handler. If there are any existing pending
 * job executions each will be processed one at a time in the iot_next_job_callback_handler.
 * After all of the pending jobs have been processed the program will wait for
 * notifications for new pending jobs and process them one at a time as they come in.
 *
 * In the main body you can see how each callback is registered for each corresponding
 * Jobs topic.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include "errno.h"

#include <wifi/wcm.h>
#include "aws_iot_config.h"

#include "aws_iot_json_utils.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_jobs_interface.h"
#include "fs_utils.h"

#define INPUT_PARAMETER_SSID "ssid"
#define INPUT_PARAMETER_PASSPHRASE "passphrase"
#define INPUT_PARAMETER_AWS_URL "aws_host"
#define INPUT_PARAMETER_AWS_PORT "aws_port"
#define INPUT_PARAMETER_AWS_THING_NAME "thing_name"

#define AWS_IOT_MY_THING_NAME os_get_boot_arg_str(INPUT_PARAMETER_AWS_THING_NAME)

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

static int init_platform();
static int validate_inputs();
static int init_and_connect_aws_iot();
static void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data);

static jsmn_parser jsonParser;
static jsmntok_t jsonTokenStruct[MAX_JSON_TOKEN_EXPECTED];
static int32_t tokenCount;
static AWS_IoT_Client *gpclient;

static void iot_get_pending_callback_handler(AWS_IoT_Client *pClient,
		char *topicName, uint16_t topicNameLen,
		IoT_Publish_Message_Params *params, void *pData) {

	os_printf("JOB_GET_PENDING_TOPIC callback\n");
	os_printf("topic: %.*s\n", topicNameLen, topicName);
	os_printf("payload: %.*s\n", (int) params->payloadLen,
			(char *) params->payload);

	jsmn_init(&jsonParser);

	tokenCount = jsmn_parse(&jsonParser, params->payload,
			(int) params->payloadLen, jsonTokenStruct, MAX_JSON_TOKEN_EXPECTED);

	if (tokenCount < 0) {
		os_printf("Failed to parse JSON: %d\n", tokenCount);
		return;
	}

	/* Assume the top-level element is an object */
	if (tokenCount < 1 || jsonTokenStruct[0].type != JSMN_OBJECT) {
		os_printf("Top Level is not an object\n");
		return;
	}

	jsmntok_t *jobs;

	jobs = findToken("inProgressJobs", params->payload, jsonTokenStruct);

	if (jobs) {
		os_printf("inProgressJobs: %.*s\n", jobs->end - jobs->start,
				(char *) params->payload + jobs->start);
	}

	jobs = findToken("queuedJobs", params->payload, jsonTokenStruct);

	if (jobs) {
		os_printf("queuedJobs: %.*s\n", jobs->end - jobs->start,
				(char *) params->payload + jobs->start);
	}
}

static void iot_next_job_callback_handler(AWS_IoT_Client *pClient,
		char *topicName, uint16_t topicNameLen,
		IoT_Publish_Message_Params *params, void *pData) {
	char topicToPublishUpdate[MAX_JOB_TOPIC_LENGTH_BYTES];
	char messageBuffer[200];

	os_printf("JOB_NOTIFY_NEXT_TOPIC / JOB_DESCRIBE_TOPIC($next) callback\n");
	os_printf("topic: %.*s\n", topicNameLen, topicName);
	os_printf("payload: %.*s\n", (int) params->payloadLen,
			(char *) params->payload);

	jsmn_init(&jsonParser);

	tokenCount = jsmn_parse(&jsonParser, params->payload,
			(int) params->payloadLen, jsonTokenStruct, MAX_JSON_TOKEN_EXPECTED);

	if (tokenCount < 0) {
		os_printf("Failed to parse JSON: %d\n", tokenCount);
		return;
	}

	/* Assume the top-level element is an object */
	if (tokenCount < 1 || jsonTokenStruct[0].type != JSMN_OBJECT) {
		os_printf("Top Level is not an object\n");
		return;
	}

	jsmntok_t *tokExecution;

	tokExecution = findToken("execution", params->payload, jsonTokenStruct);

	if (tokExecution) {
		os_printf("execution: %.*s\n", tokExecution->end - tokExecution->start,
				(char *) params->payload + tokExecution->start);

		jsmntok_t *tok;

		tok = findToken("jobId", params->payload, tokExecution);

		if (tok) {
			int rc;
			char jobId[MAX_SIZE_OF_JOB_ID + 1];
			AwsIotJobExecutionUpdateRequest updateRequest;

			rc = parseStringValue(jobId, MAX_SIZE_OF_JOB_ID + 1,
					params->payload, tok);
			if (SUCCESS != rc) {
				os_printf("parseStringValue returned error : %d\n", rc);
				return;
			}

			os_printf("jobId: %s\n", jobId);

			tok = findToken("jobDocument", params->payload, tokExecution);

			/*
			 * Do your job processing here.
			 */

			if (tok) {
				os_printf("jobDocument: %.*s\n", tok->end - tok->start,
						(char *) params->payload + tok->start);
				/* Alternatively if the job still has more steps the status can be set to JOB_EXECUTION_IN_PROGRESS instead */
				updateRequest.status = JOB_EXECUTION_SUCCEEDED;
				updateRequest.statusDetails =
						"{\"exampleDetail\":\"a value appropriate for your successful job\"}";
			} else {
				updateRequest.status = JOB_EXECUTION_FAILED;
				updateRequest.statusDetails =
						"{\"failureDetail\":\"Unable to process job document\"}";
			}

			updateRequest.expectedVersion = 0;
			updateRequest.executionNumber = 0;
			updateRequest.includeJobExecutionState = false;
			updateRequest.includeJobDocument = false;
			updateRequest.clientToken = NULL;

			rc = aws_iot_jobs_send_update(pClient, QOS0, AWS_IOT_MY_THING_NAME,
					jobId, &updateRequest, topicToPublishUpdate,
					sizeof(topicToPublishUpdate), messageBuffer,
					sizeof(messageBuffer));
			if (SUCCESS != rc) {
				os_printf("aws_iot_jobs_send_update returned error : %d\n", rc);
				return;
			}
		}
	} else {
		os_printf("execution property not found, nothing to do\n");
	}
}

static void iot_update_accepted_callback_handler(AWS_IoT_Client *pClient,
		char *topicName, uint16_t topicNameLen,
		IoT_Publish_Message_Params *params, void *pData) {
	IOT_UNUSED(pData);
	IOT_UNUSED(pClient);
	os_printf("JOB_UPDATE_TOPIC / accepted callback\n");
	os_printf("topic: %.*s\n", topicNameLen, topicName);
	os_printf("payload: %.*s\n", (int) params->payloadLen,
			(char *) params->payload);
}

static void iot_update_rejected_callback_handler(AWS_IoT_Client *pClient,
		char *topicName, uint16_t topicNameLen,
		IoT_Publish_Message_Params *params, void *pData) {
	IOT_UNUSED(pData);
	IOT_UNUSED(pClient);
	os_printf("JOB_UPDATE_TOPIC / rejected callback\n");
	os_printf("topic: %.*s", topicNameLen, topicName);
	os_printf("payload: %.*s", (int) params->payloadLen,
			(char *) params->payload);

	/* Do error handling here for when the update was rejected */
}

static void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data) {
	os_printf("MQTT Disconnect\n");
	int rc = FAILURE;

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

int main(int argc, char **argv) {

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
						" %s=<wifi network password> %s=<aws url> %s=<aws_port, def 8883> %s=<Thing NAME>\n",
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

	rc = init_and_connect_aws_iot();
	os_printf("AWS Connection is done ret:%d\n", rc);
	if (!rc) {

		char *topicToSubscribeGetPending = os_alloc(MAX_JOB_TOPIC_LENGTH_BYTES);

		rc = aws_iot_jobs_subscribe_to_job_messages(gpclient, QOS0,
		AWS_IOT_MY_THING_NAME, NULL, JOB_GET_PENDING_TOPIC,
				JOB_WILDCARD_REPLY_TYPE, iot_get_pending_callback_handler, NULL,
				topicToSubscribeGetPending, MAX_JOB_TOPIC_LENGTH_BYTES);

		if (SUCCESS != rc) {
			os_printf("Error subscribing JOB_GET_PENDING_TOPIC: %d\n", rc);
			return rc;
		}
		os_printf("Success subscribing JOB_GET_PENDING_TOPIC: %d\n", rc);

		char *topicToSubscribeNotifyNext = os_alloc(MAX_JOB_TOPIC_LENGTH_BYTES);
		rc = aws_iot_jobs_subscribe_to_job_messages(gpclient, QOS0,
		AWS_IOT_MY_THING_NAME, NULL, JOB_NOTIFY_NEXT_TOPIC, JOB_REQUEST_TYPE,
				iot_next_job_callback_handler, NULL, topicToSubscribeNotifyNext,
				MAX_JOB_TOPIC_LENGTH_BYTES);

		if (SUCCESS != rc) {
			os_printf("Error subscribing JOB_NOTIFY_NEXT_TOPIC: %d\n", rc);
			return rc;
		}
		os_printf("Success subscribing JOB_NOTIFY_NEXT_TOPIC: %d\n", rc);

		char *topicToSubscribeGetNext = os_alloc(MAX_JOB_TOPIC_LENGTH_BYTES);
		rc = aws_iot_jobs_subscribe_to_job_messages(gpclient, QOS0,
		AWS_IOT_MY_THING_NAME, JOB_ID_NEXT, JOB_DESCRIBE_TOPIC,
				JOB_WILDCARD_REPLY_TYPE, iot_next_job_callback_handler, NULL,
				topicToSubscribeGetNext, MAX_JOB_TOPIC_LENGTH_BYTES);

		if (SUCCESS != rc) {
			os_printf("Error subscribing JOB_DESCRIBE_TOPIC ($next): %d\n",
					rc);
			return rc;
		}
		os_printf("Success subscribing JOB_DESCRIBE_TOPIC ($next): %d\n", rc);

		char *topicToSubscribeUpdateAccepted = os_alloc(
		MAX_JOB_TOPIC_LENGTH_BYTES);
		rc = aws_iot_jobs_subscribe_to_job_messages(gpclient, QOS0,
		AWS_IOT_MY_THING_NAME, JOB_ID_WILDCARD, JOB_UPDATE_TOPIC,
				JOB_ACCEPTED_REPLY_TYPE, iot_update_accepted_callback_handler,
				NULL, topicToSubscribeUpdateAccepted,
				MAX_JOB_TOPIC_LENGTH_BYTES);

		if (SUCCESS != rc) {
			os_printf("Error subscribing JOB_UPDATE_TOPIC/accepted: %d\n", rc);
			return rc;
		}
		os_printf("Success subscribing JOB_UPDATE_TOPIC/accepted: %d\n", rc);

		char *topicToSubscribeUpdateRejected = os_alloc(
		MAX_JOB_TOPIC_LENGTH_BYTES);
		rc = aws_iot_jobs_subscribe_to_job_messages(gpclient, QOS0,
		AWS_IOT_MY_THING_NAME, JOB_ID_WILDCARD, JOB_UPDATE_TOPIC,
				JOB_REJECTED_REPLY_TYPE, iot_update_rejected_callback_handler,
				NULL, topicToSubscribeUpdateRejected,
				MAX_JOB_TOPIC_LENGTH_BYTES);

		if (SUCCESS != rc) {
			os_printf("Error subscribing JOB_UPDATE_TOPIC/rejected: %d\n", rc);
			return rc;
		}
		os_printf("Success subscribing JOB_UPDATE_TOPIC/rejected: %d\n", rc);

		char *topicToPublishGetPending = os_alloc(MAX_JOB_TOPIC_LENGTH_BYTES);
		rc = aws_iot_jobs_send_query(gpclient, QOS0, AWS_IOT_MY_THING_NAME,
				NULL, NULL, topicToPublishGetPending,
				MAX_JOB_TOPIC_LENGTH_BYTES, NULL, 0, JOB_GET_PENDING_TOPIC);

		os_printf("aws_iot_jobs_send_query: %d\n", rc);

		AwsIotDescribeJobExecutionRequest *describeRequest = os_alloc(
				sizeof(AwsIotDescribeJobExecutionRequest));
		describeRequest->executionNumber = 0;
		describeRequest->includeJobDocument = true;
		describeRequest->clientToken = NULL;

		char *topicToPublishGetNext = os_alloc(MAX_JOB_TOPIC_LENGTH_BYTES);
		rc = aws_iot_jobs_describe(gpclient, QOS0, AWS_IOT_MY_THING_NAME,
		JOB_ID_NEXT, describeRequest, topicToPublishGetNext,
		MAX_JOB_TOPIC_LENGTH_BYTES, NULL, 0);
		os_printf("Success aws_iot_jobs_describe: %d\n", rc);

		while (1) {
			//Max time the yield function will wait for read messages
			rc = aws_iot_mqtt_yield(gpclient, 5000);
			os_printf("aws_iot_mqtt_yield: %d\n", rc);
		}

	}

	/*clean up heap and other inits*/
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
	connectParams->pClientID = (char *)AWS_IOT_MY_THING_NAME;
	connectParams->clientIDLen = (uint16_t) strlen(AWS_IOT_MY_THING_NAME);
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

