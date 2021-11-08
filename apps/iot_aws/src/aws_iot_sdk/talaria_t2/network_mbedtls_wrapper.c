/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>
#include <t2_time.h>
#include <network_interface.h>

#include "aws_iot_error.h"
#include "aws_iot_log.h"
#include "t2_network.h"

/* This is the value used for ssl read timeout */
#define IOT_SSL_READ_TIMEOUT 10

/* This defines the value of the debug buffer that gets allocated.
 * The value can be altered based on memory constraints
 */

int mbedtls_ctr_drbg_random_ex(void *p_rng, unsigned char *output,
		size_t output_len) {
	mbedtls_ctr_drbg_random(p_rng, output, output_len);
	return 0;
}

static int _iot_tls_verify_cert(void *data, mbedtls_x509_crt *crt, int depth,
		uint32_t *flags) {
	char *buf = os_alloc(2048);
	((void) data);

	mbedtls_x509_crt_info(buf, 2048 - 1, "", crt);

	if ((*flags) == 0) {
		os_printf("  This certificate has no flags\n");
	} else {
		os_printf(buf, *flags);
		os_printf("%s\n", buf);
	}
	os_free(buf);
	return 0;
}

void _iot_tls_set_connect_params(Network *pNetwork, char *pRootCALocation,
		char *pDeviceCertLocation, char *pDevicePrivateKeyLocation,
		char *pDestinationURL, uint16_t destinationPort, uint32_t timeout_ms,
		bool ServerVerificationFlag) {
	pNetwork->tlsConnectParams.DestinationPort = destinationPort;
	pNetwork->tlsConnectParams.pDestinationURL = pDestinationURL;
	pNetwork->tlsConnectParams.pDeviceCertLocation = pDeviceCertLocation;
	pNetwork->tlsConnectParams.pDevicePrivateKeyLocation =
			pDevicePrivateKeyLocation;
	pNetwork->tlsConnectParams.pRootCALocation = pRootCALocation;
	pNetwork->tlsConnectParams.timeout_ms = timeout_ms;
	pNetwork->tlsConnectParams.ServerVerificationFlag = ServerVerificationFlag;
}

IoT_Error_t iot_tls_init(Network *pNetwork, char *pRootCALocation,
		char *pDeviceCertLocation, char *pDevicePrivateKeyLocation,
		char *pDestinationURL, uint16_t destinationPort, uint32_t timeout_ms,
		bool ServerVerificationFlag) {
	_iot_tls_set_connect_params(pNetwork, pRootCALocation, pDeviceCertLocation,
			pDevicePrivateKeyLocation, pDestinationURL, destinationPort,
			timeout_ms, ServerVerificationFlag);

	pNetwork->connect = iot_tls_connect;
	pNetwork->read = iot_tls_read;
	pNetwork->write = iot_tls_write;
	pNetwork->disconnect = iot_tls_disconnect;
	pNetwork->isConnected = iot_tls_is_connected;
	pNetwork->destroy = iot_tls_destroy;

	pNetwork->tlsDataParams.flags = 0;

	return SUCCESS;
}

IoT_Error_t iot_tls_is_connected(Network *pNetwork) {
	/* Use this to add implementation which can check for physical layer disconnect */
	return NETWORK_PHYSICAL_LAYER_CONNECTED;
}

IoT_Error_t iot_tls_connect(Network *pNetwork, TLSConnectParams *params) {
	int ret = 0;
	const char *pers = "aws_iot_tls_wrapper";
	TLSDataParams *tlsDataParams = NULL;
	char portBuffer[6];

#ifdef ENABLE_IOT_DEBUG
	unsigned char buf[MBEDTLS_DEBUG_BUFFER_SIZE];
#endif

	if (NULL == pNetwork) {
		return NULL_VALUE_ERROR;
	}

	if (NULL != params) {
		_iot_tls_set_connect_params(pNetwork, params->pRootCALocation,
				params->pDeviceCertLocation, params->pDevicePrivateKeyLocation,
				params->pDestinationURL, params->DestinationPort,
				params->timeout_ms, params->ServerVerificationFlag);
	}

	tlsDataParams = &(pNetwork->tlsDataParams);

	mbedtls_net_init(&(tlsDataParams->server_fd));
	mbedtls_ssl_init(&(tlsDataParams->ssl));
	mbedtls_ssl_config_init(&(tlsDataParams->conf));
	mbedtls_ctr_drbg_init(&(tlsDataParams->ctr_drbg));
	mbedtls_x509_crt_init(&(tlsDataParams->cacert));
	mbedtls_x509_crt_init(&(tlsDataParams->clicert));
	mbedtls_pk_init(&(tlsDataParams->pkey));

	mbedtls_entropy_init(&(tlsDataParams->entropy));
	if ((ret = mbedtls_ctr_drbg_seed(&(tlsDataParams->ctr_drbg),
			mbedtls_entropy_func, &(tlsDataParams->entropy),
			(const unsigned char *) pers, strlen(pers))) != 0) {
		os_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned -0x%x\n", -ret);
		return NETWORK_MBEDTLS_ERR_CTR_DRBG_ENTROPY_SOURCE_FAILED;
	}

	ret = mbedtls_x509_crt_parse(&(tlsDataParams->cacert),
			(const unsigned char*) pNetwork->tlsConnectParams.pRootCALocation,
			strlen(pNetwork->tlsConnectParams.pRootCALocation) + 1);

	if (ret < 0) {
		os_printf(
				" failed\n  !  mbedtls_x509_crt_parse returned -0x%x while parsing root cert\n\n",
				-ret);
		return NETWORK_X509_ROOT_CRT_PARSE_ERROR;
	}

	os_printf(
			"\n Root Done[%d]Loading the client cert. and key. size TLSDataParams:%d\n",
			ret, sizeof(TLSDataParams));

	ret =
			mbedtls_x509_crt_parse(&(tlsDataParams->clicert),
					(const unsigned char*) pNetwork->tlsConnectParams.pDeviceCertLocation,
					strlen(pNetwork->tlsConnectParams.pDeviceCertLocation) + 1);

	os_printf("\n Loading the client cert done.... ret[%d]", ret);
	if (ret != 0) {
		IOT_ERROR(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x while parsing device cert\n\n", -ret);
		return NETWORK_X509_DEVICE_CRT_PARSE_ERROR;
	}

	ret =
			mbedtls_pk_parse_key(&(tlsDataParams->pkey),
					(const unsigned char*) pNetwork->tlsConnectParams.pDevicePrivateKeyLocation,
					strlen(pNetwork->tlsConnectParams.pDevicePrivateKeyLocation)
							+ 1, NULL, 0);

	os_printf("\n Client pkey loaded[%d]\n", ret);
	if (ret != 0) {
		return NETWORK_PK_PRIVATE_KEY_PARSE_ERROR;
	}
	snprintf(portBuffer, 6, "%d", pNetwork->tlsConnectParams.DestinationPort);
	os_printf("  . Connecting to %s/%s...",
			pNetwork->tlsConnectParams.pDestinationURL, portBuffer);
	if ((ret = mbedtls_net_connect(&(tlsDataParams->server_fd),
			pNetwork->tlsConnectParams.pDestinationURL, portBuffer,
			MBEDTLS_NET_PROTO_TCP)) != 0) {
		os_printf(
				" failed\n  ! mbedtls_net_connect returned [-0x%x] uRL[%s] port[%s]\n\n",
				-ret, pNetwork->tlsConnectParams.pDestinationURL, portBuffer);
		switch (ret) {
		case MBEDTLS_ERR_NET_SOCKET_FAILED:
			return NETWORK_ERR_NET_SOCKET_FAILED;
		case MBEDTLS_ERR_NET_UNKNOWN_HOST:
			return NETWORK_ERR_NET_UNKNOWN_HOST;
		case MBEDTLS_ERR_NET_CONNECT_FAILED:
		default:
			return NETWORK_ERR_NET_CONNECT_FAILED;
		};
	}

	ret = mbedtls_net_set_block(&(tlsDataParams->server_fd));
	if (ret != 0) {
		os_printf(" failed\n  ! net_set_(non)block() returned -0x%x\n\n", -ret);
		return SSL_CONNECTION_ERROR;
	}
	os_printf(" ok\n");

	os_printf("  . Setting up the SSL/TLS structure...");
	if ((ret = mbedtls_ssl_config_defaults(&(tlsDataParams->conf),
			MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
			MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
		os_printf(" failed\n  ! mbedtls_ssl_config_defaults returned -0x%x\n\n",
				-ret);
		return SSL_CONNECTION_ERROR;
	}
	mbedtls_ssl_conf_verify(&(tlsDataParams->conf), _iot_tls_verify_cert, NULL);
	if (pNetwork->tlsConnectParams.ServerVerificationFlag == true) {
		mbedtls_ssl_conf_authmode(&(tlsDataParams->conf),
				MBEDTLS_SSL_VERIFY_REQUIRED);
	} else {
		os_printf("verification is optional\n");
		mbedtls_ssl_conf_authmode(&(tlsDataParams->conf),
				MBEDTLS_SSL_VERIFY_OPTIONAL);
	}
	mbedtls_ssl_conf_rng(&(tlsDataParams->conf), mbedtls_ctr_drbg_random_ex,
			&(tlsDataParams->ctr_drbg));
	mbedtls_ssl_conf_ca_chain(&(tlsDataParams->conf), &(tlsDataParams->cacert),
			NULL);
	if ((ret = mbedtls_ssl_conf_own_cert(&(tlsDataParams->conf),
			&(tlsDataParams->clicert), &(tlsDataParams->pkey))) != 0) {
		os_printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n",
				ret);
		return SSL_CONNECTION_ERROR;
	}

	mbedtls_ssl_conf_read_timeout(&(tlsDataParams->conf),
			2 * pNetwork->tlsConnectParams.timeout_ms);

	/* Use the AWS IoT ALPN extension for MQTT if port 443 is requested. */
	/*if (443 == pNetwork->tlsConnectParams.DestinationPort) {
	 if ((ret = mbedtls_ssl_conf_alpn_protocols(&(tlsDataParams->conf),
	 alpnProtocols)) != 0) {
	 IOT_ERROR(" failed\n  ! mbedtls_ssl_conf_alpn_protocols returned -0x%x\n\n", -ret);
	 return SSL_CONNECTION_ERROR;
	 }
	 }*/

	/* Assign the resulting configuration to the SSL context. */
	if ((ret = mbedtls_ssl_setup(&(tlsDataParams->ssl), &(tlsDataParams->conf)))
			!= 0) {
		os_printf(" failed\n  ! mbedtls_ssl_setup returned -0x%x\n\n", -ret);
		return SSL_CONNECTION_ERROR;
	}
	if ((ret = mbedtls_ssl_set_hostname(&(tlsDataParams->ssl),
			pNetwork->tlsConnectParams.pDestinationURL)) != 0) {
		IOT_ERROR(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
		return SSL_CONNECTION_ERROR;
	}
	mbedtls_ssl_set_bio(&(tlsDataParams->ssl), &(tlsDataParams->server_fd),
			mbedtls_net_send, NULL, mbedtls_net_recv_timeout);

	while ((ret = mbedtls_ssl_handshake(&(tlsDataParams->ssl))) != 0) {
		if (ret != MBEDTLS_ERR_SSL_WANT_READ
				&& ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
			os_printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n",
					-ret);
			if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
				os_printf("    Unable to verify the server's certificate. "
						"Either it is invalid,\n"
						"    or you didn't set ca_file or ca_path "
						"to an appropriate value.\n"
						"    Alternatively, you may want to use "
						"auth_mode=optional for testing purposes.\n");
			}
			return SSL_CONNECTION_ERROR;
		}
	}
	os_printf("SSL/TLS handshake. DONE ..ret:%d\n", ret);

	os_printf(" ok\n    [ Protocol is %s ]\n    [ Ciphersuite is %s ]\n",
			mbedtls_ssl_get_version(&(tlsDataParams->ssl)),
			mbedtls_ssl_get_ciphersuite(&(tlsDataParams->ssl)));
	if ((ret = mbedtls_ssl_get_record_expansion(&(tlsDataParams->ssl))) >= 0) {
		os_printf("    [ Record expansion is %d ]\n", ret);
	} else {
		os_printf("    [ Record expansion is unknown (compression) ]\n");
	}

	os_printf(". Verifying peer X.509 certificate...\n");

	if (pNetwork->tlsConnectParams.ServerVerificationFlag == true) {
		char *vrfy_buf = os_alloc(512);
		if ((tlsDataParams->flags = mbedtls_ssl_get_verify_result(
				&(tlsDataParams->ssl))) != 0) {
			os_printf(" failed\n");
			mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ",
					tlsDataParams->flags);
			os_printf("%s\n", vrfy_buf);
			ret = SSL_CONNECTION_ERROR;
		} else {
			os_printf(" ok\n");
			ret = SUCCESS;
		}
		os_free(vrfy_buf);
	} else {
		IOT_DEBUG(" Server Verification skipped\n");
		ret = SUCCESS;
	}

	ret = SUCCESS;

#ifdef ENABLE_IOT_DEBUG
	if (mbedtls_ssl_get_peer_cert(&(tlsDataParams->ssl)) != NULL) {
		IOT_DEBUG("  . Peer certificate information    ...");
		mbedtls_x509_crt_info((char *) buf, sizeof(buf) - 1, "      ", mbedtls_ssl_get_peer_cert(&(tlsDataParams->ssl)));
		IOT_DEBUG("%s\n", buf);
	}
#endif

	mbedtls_ssl_conf_read_timeout(&(tlsDataParams->conf), IOT_SSL_READ_TIMEOUT);

	return (IoT_Error_t) ret;
}

IoT_Error_t iot_tls_write(Network *pNetwork, unsigned char *pMsg, size_t len,
		Timer *timer, size_t *written_len) {
	size_t written_so_far;
	bool isErrorFlag = false;
	int frags;
	int ret = 0;
	TLSDataParams *tlsDataParams = &(pNetwork->tlsDataParams);

	for (written_so_far = 0, frags = 0;
			written_so_far < len && !has_timer_expired(timer); written_so_far +=
					ret, frags++) {
		while (!has_timer_expired(timer)
				&& (ret = mbedtls_ssl_write(&(tlsDataParams->ssl),
						pMsg + written_so_far, len - written_so_far)) <= 0) {
			if (ret != MBEDTLS_ERR_SSL_WANT_READ
					&& ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
				os_printf(" failed\n  ! mbedtls_ssl_write returned -0x%x\n\n",
						-ret);
				/* All other negative return values indicate connection needs to be reset.
				 * Will be caught in ping request so ignored here */
				isErrorFlag = true;
				break;
			}
		}
		if (isErrorFlag) {
			break;
		}
	}

	*written_len = written_so_far;

	if (isErrorFlag) {
		return NETWORK_SSL_WRITE_ERROR;
	} else if (has_timer_expired(timer) && written_so_far != len) {
		return NETWORK_SSL_WRITE_TIMEOUT_ERROR;
	}

	return SUCCESS;
}

IoT_Error_t iot_tls_read(Network *pNetwork, unsigned char *pMsg, size_t len,
		Timer *timer, size_t *read_len) {
	mbedtls_ssl_context *ssl = &(pNetwork->tlsDataParams.ssl);
	size_t rxLen = 0;
	int ret;
	while (len > 0) {
		// This read will timeout after IOT_SSL_READ_TIMEOUT if there's no data to be read
		ret = mbedtls_ssl_read(ssl, pMsg, len);
		if (ret > 0) {
			rxLen += ret;
			pMsg += ret;
			len -= ret;
		} else if (ret == 0
				|| (ret != MBEDTLS_ERR_SSL_WANT_READ
						&& ret != MBEDTLS_ERR_SSL_WANT_WRITE
						&& ret != MBEDTLS_ERR_SSL_TIMEOUT)) {
			return NETWORK_SSL_READ_ERROR;
		}

		// Evaluate timeout after the read to make sure read is done at least once
		if (has_timer_expired(timer)) {
			break;
		}
	}

	if (len == 0) {
		*read_len = rxLen;
		return SUCCESS;
	}

	if (rxLen == 0) {
		return NETWORK_SSL_NOTHING_TO_READ;
	} else {
		return NETWORK_SSL_READ_TIMEOUT_ERROR;
	}
}

IoT_Error_t iot_tls_disconnect(Network *pNetwork) {
	mbedtls_ssl_context *ssl = &(pNetwork->tlsDataParams.ssl);
	int ret = 0;
	do {
		ret = mbedtls_ssl_close_notify(ssl);
	} while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);

	/* All other negative return values indicate connection needs to be reset.
	 * No further action required since this is disconnect call */

	return SUCCESS;
}

IoT_Error_t iot_tls_destroy(Network *pNetwork) {
	TLSDataParams *tlsDataParams = &(pNetwork->tlsDataParams);

	mbedtls_net_free(&(tlsDataParams->server_fd));

	mbedtls_x509_crt_free(&(tlsDataParams->clicert));
	mbedtls_x509_crt_free(&(tlsDataParams->cacert));
	mbedtls_pk_free(&(tlsDataParams->pkey));
	mbedtls_ssl_free(&(tlsDataParams->ssl));
	mbedtls_ssl_config_free(&(tlsDataParams->conf));
	mbedtls_ctr_drbg_free(&(tlsDataParams->ctr_drbg));
	mbedtls_entropy_free(&(tlsDataParams->entropy));

	return SUCCESS;
}

#ifdef __cplusplus
}
#endif
