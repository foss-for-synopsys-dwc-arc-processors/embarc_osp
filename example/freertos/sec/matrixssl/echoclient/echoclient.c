/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \version 2017.03
 * \date 2016-04-22
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	EMBARC_APP_FREERTOS_SEC_MATRIXSSL_ECHOCLIENT embARC MatrixSSL Secure Echoclient
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_FREERTOS
 * \ingroup	EMBARC_APPS_MID_LWIP
 * \ingroup	EMBARC_APPS_MID_MATRIXSSL
 * \brief	embARC Example for secure echoclient based on FreeRTOS+LwIP and MatrixSSL
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     - Digilent PMOD WIFI(MRF24WG0MA)
 *
 * ### Design Concept
 *     This example is designed as a secure echoclient. The communication is based on TLS.
 *     This example needs to work with echoserver example of matrixssl in pair.
 *
 * ### Usage Manual
 *     - Hardware:
 *         - EMSK node1: PMOD WiFi to J5 on \ref EMBARC_BOARD_CONNECTION "EMSK"
 *         - EMSK node2: PMOD WiFi to J5 on \ref EMBARC_BOARD_CONNECTION "EMSK"
 *     - WiFi Network: *SSID:embARC, PASSWORD:qazwsxedc*
 *     - Run steps:
 *         - This example should run together with the \ref EMBARC_APP_MATRIXSSL_FREERTOS_ECHOSERVER.
 *         - echoserver should run first, get the IP from the uart output message such as "ipaddr 192.168.43.252".
 *         - change the \ref ECHOSERVER_IP "IP of echoserver" in \ref echoclient.c "echoclient source code", build and run this example(echoclient).
 *         - the exchange of information will be shown in terminal as below.
 *
 *     ![ScreenShot of matrixssl secure echoserver](pic/images/example/emsk/emsk_matrixssl_freertos_echoclient.jpg)
 *
 * ### Extra Comments
 *     v0.1 support PSK and RSA 1024
 *
 */


/**
 * \file
 * \brief  example of matrixssl secure echoclient
 *
 * \ingroup	EMBARC_APP_FREERTOS_SEC_MATRIXSSL_ECHOCLIENT
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_SEC_MATRIXSSL_ECHOCLIENT
 * @{
 */

#include "embARC.h"
#include "embARC_debug.h"

#include "matrixssl/matrixsslApi.h"
#include "core/embARC/matrixssl_network.h"

#ifdef USE_RSA
#include "testkeys/RSA/1024_RSA_KEY.h"
#include "testkeys/RSA/1024_RSA.h"
#include "testkeys/RSA/ALL_RSA_CAS.h"
#endif /* USE_RSA */

#include "testkeys/PSK/psk.h"

#define TSKPRI_ECHOCLIENT_TEST	(configMAX_PRIORITIES-2) /**< echo client task priority */

#define ECHOSERVER_IP		"192.168.43.252"		/**< echo server ip, need to be modified according to your server IP */
#define ECHOSERVER_PORT		80

static TaskHandle_t echoclient_test_handle = NULL;

static char *msg = "Hello I am client";

static unsigned char data_buf[256];

Inline void error(const char* msg)
{
	EMBARC_PRINTF("error: %s\r\n", msg);
	if (msg) vTaskSuspend(NULL);
}

static void echoclient_test(void* args)
{
	sslKeys_t *keys = NULL;

	MATRIXSSL_SOCKET server;

	matrixssl_con *con;


	struct sockaddr_in server_addr;

	int size = strlen(msg);
	int port = ECHOSERVER_PORT;


	if (matrixSslOpen() < 0) {
		error("matrixSslOpen failed, exiting...\n");
	}

	if (matrixSslNewKeys(&keys, NULL) < PS_SUCCESS) {
		error("matrixSslNewKeys failed\n");
	}

	// for (rc = 0; rc < 8; rc++) {
	// 	matrixSslLoadPsk(keys, pskTable[rc].key, sizeof(pskTable[rc].key),
	// 		pskTable[rc].id, sizeof(pskTable[rc].id));
	// }

	if (matrixSslLoadRsaKeysMem(keys, RSA1024, sizeof(RSA1024),
						RSA1024KEY, sizeof(RSA1024KEY), RSACAS,
						sizeof(RSACAS)) < 0) {
		error("matrixSslLoadRsaKeysMem failed\n");
	}

	server_addr.sin_len = sizeof(server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ECHOSERVER_IP);



	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		error("socket failed");
	}

	EMBARC_PRINTF("Try to connect ECHOSERVER_IP:%s\r\n", ECHOSERVER_IP);
	if (connect(server, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
		error("connect failed, please check if you have modified the ECHOSERVER_IP in the source code!");
	}

	con = matrixssl_new(server, keys);
	con->options.versionFlag = SSL_FLAGS_TLS_1_2;

	if(matrixssl_connect(con) != PS_SUCCESS) {
		matrixssl_free(con);
		close(server);
		error("SSL connect failed");
	}

	if (matrixssl_write(con, msg, size) != size) {
		EMBARC_PRINTF("write failed:%d\r\n", size);
	}

	if (matrixssl_read(con, data_buf, 256) <= 0) {
		EMBARC_PRINTF("read failed\r\n");
	}

	EMBARC_PRINTF("echo:%s\r\n", data_buf);


	matrixssl_close(con);
	matrixssl_free(con);
	close(server);

	vTaskSuspend(NULL);
}

int main(void)
{

	if (xTaskCreate(echoclient_test, "matrixssl echo client", 4096, (void *)1,
		TSKPRI_ECHOCLIENT_TEST, &echoclient_test_handle) != pdPASS) {
		EMBARC_PRINTF("create echoclient task failed\r\n");
		return -1;
	}
	vTaskSuspend(NULL);
	return 0;
}
/** @} */