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
 * \defgroup	EMBARC_APP_FREERTOS_SEC_MATRIXSSL_ECHOSERVER embARC MatrixSSL Secure Echoserver
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_FREERTOS
 * \ingroup	EMBARC_APPS_MID_LWIP
 * \ingroup	EMBARC_APPS_MID_MATRIXSSL
 * \brief	embARC Example for secure echoserver based on FreeRTOS+LwIP and MatrixSSL
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     - Digilent PMOD WIFI(MRF24WG0MA)
 *
 * ### Design Concept
 *     This example is designed as a secure echoserver. The communication is based on TLS.
 *     This example needs to work with echoclient example of MatrixSSL in pair.
 *
 * ### Usage Manual
 *     - Hardware:
 *         - EMSK node1: PMOD WiFi to J5 \ref EMBARC_BOARD_CONNECTION "EMSK"
 *         - EMSK node2: PMOD WiFi to J5 \ref EMBARC_BOARD_CONNECTION "EMSK"
 *     - WiFi Network: *SSID:embARC, PASSWORD:qazwsxedc*
 *     - Run steps:
 *         - This example should work together with the \ref EMBARC_APP_FREERTOS_SEC_MATRIXSSL_FREERTOS_ECHOCLIENT.
 *         - This example(echoserver) should run first, get the IP.
 *         - change the \ref ECHOSERVER_IP "IP of echoserver" in \ref echoclient.c "echoclient source code", build and run echoclient.
 *         - the exchange of information will be shown in terminal as below.
 *
 *     ![ScreenShot of MatrixSSL secure echoserver](pic/images/example/emsk/emsk_matrixssl_freertos_echoserver.jpg)
 *
 * ### Extra Comments
 *     v0.1 support PSK and RSA 1024
 *
 */


/**
 * \file
 * \brief  example of MatrixSSL secure echoserver
 *
 * \ingroup	EMBARC_APP_FREERTOS_SEC_MATRIXSSL_ECHOSERVER
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_SEC_MATRIXSSL_ECHOSERVER
 * @{
 */

#include "embARC.h"
#include "matrixssl/matrixsslApi.h"
#include "core/embARC/matrixssl_network.h"

#ifdef USE_RSA
#include "testkeys/RSA/1024_RSA_KEY.h"
#include "testkeys/RSA/1024_RSA.h"
#include "testkeys/RSA/ALL_RSA_CAS.h"
#endif /* USE_RSA */

#include "testkeys/PSK/psk.h"

#define SVR_COMMAND_SIZE 256
#define SVR_PORT	80	/* echo server port */


#define TSKPRI_ECHOSERVER_TEST	(configMAX_PRIORITIES-2)	  /**< secure echoserver task priority */

static TaskHandle_t echoserver_test_handle = NULL;


Inline void error(const char* msg)
{
	EMBARC_PRINTF("error: %s\r\n", msg);
	if (msg) vTaskSuspend(NULL);
}

static void echoserver_test(void* args)
{
	sslKeys_t *keys = NULL;

	MATRIXSSL_SOCKET server;

	struct sockaddr_in server_addr;

	int shutdown = 0;
	int port = 80;
	// int rc;


	if (matrixSslOpen() < 0) {
		error("matrixSslOpen failed, exiting...");
	}

	if (matrixSslNewKeys(&keys, NULL) < PS_SUCCESS) {
		error("matrixSslNewKeys failed");
	}

	// for (rc = 0; rc < 8; rc++) {
	// 	matrixSslLoadPsk(keys, pskTable[rc].key, sizeof(pskTable[rc].key),
	// 		pskTable[rc].id, sizeof(pskTable[rc].id));
	// }

	if (matrixSslLoadRsaKeysMem(keys, RSA1024, sizeof(RSA1024),
						RSA1024KEY, sizeof(RSA1024KEY), RSACAS,
						sizeof(RSACAS)) < 0) {
		error("matrixSslLoadRsaKeysMem failed");
	}

	server_addr.sin_len = sizeof(server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		error("socket failed");
	}

	if (bind(server, (const struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
		error("tcp bind failed");
	}

	if (listen(server, 5) !=0 ) {
		error("listen error");
	}

    	while (!shutdown) {
  		matrixssl_con	*con;
        	char  command[SVR_COMMAND_SIZE+1];
        	int     echo_size = 0;
      		MATRIXSSL_SOCKET client;
        	int     first_read = 1;
        	int     got_G = 0;

        	struct sockaddr_in client_addr;
        	socklen_t     client_addr_len = sizeof(client_addr);
  		EMBARC_PRINTF("Waiting for client to connect....\r\n");
      		client = accept(server, (struct sockaddr*)&client_addr, &client_addr_len);

        	if (client < 0) error("tcp accept failed");

        	EMBARC_PRINTF("client:%s\r\n", inet_ntoa(client_addr.sin_addr.s_addr));

		con = matrixssl_new(client, keys);
		con->options.versionFlag = SSL_FLAGS_TLS_1_2;
		// con->options.truncHmac = -1;
		// con->options.maxFragLen = -1;

		if (matrixssl_accept(con) != PS_SUCCESS) {
			EMBARC_PRINTF("SSL_accept failed\r\n");
			matrixssl_free(con);
			close(client);
			continue;
		}

		while ( (echo_size = matrixssl_read(con, command, sizeof(command)-1)) > 0) {

			if (first_read == 1) {
				first_read = 0;  /* browser may send 1 byte 'G' to start */
				if (echo_size == 1 && command[0] == 'G') {
					got_G = 1;
					continue;
				}
			}
			else if (got_G == 1 && strncmp(command, "ET /", 4) == 0) {
				strncpy(command, "GET", 4);
				/* fall through to normal GET */
			}

			if ( strncmp(command, "quit", 4) == 0) {
				EMBARC_PRINTF("client sent quit command: shutting down!\r\n");
				shutdown = 1;
				break;
			}
			if ( strncmp(command, "break", 5) == 0) {
				EMBARC_PRINTF("client sent break command: closing session!\r\n");
				break;
			}

			if ( strncmp(command, "GET", 3) == 0) {
				char type[]   = "HTTP/1.0 200 ok\r\nContent-type:"
			                " text/html\r\n\r\n";
				char header[] = "<html><body BGCOLOR=\"#ffffff\">\n<pre>\n";
				char body[]   = "greetings from matrixssl\n";
				char footer[] = "</body></html>\r\n\r\n";

				strncpy(command, type, sizeof(type));
				echo_size = sizeof(type) - 1;

				strncpy(&command[echo_size], header, sizeof(header));
				echo_size += (int)sizeof(header) - 1;
				strncpy(&command[echo_size], body, sizeof(body));
				echo_size += (int)sizeof(body) - 1;
				strncpy(&command[echo_size], footer, sizeof(footer));
				echo_size += (int)sizeof(footer);

				if (matrixssl_write(con, command, echo_size) != echo_size)
					EMBARC_PRINTF("SSL_write failed\r\n");
				break;
			}
			command[echo_size] = 0;
			EMBARC_PRINTF("From client:%s\r\n", command);

			if (matrixssl_write(con, command, echo_size) != echo_size) {
				EMBARC_PRINTF("SSL_write failed\r\n");
				break;
			}
		}

		matrixssl_close(con);
		matrixssl_free(con);
		close(client);
	}

	close(server);
	vTaskSuspend(NULL);
}

int main(void)
{

	EMBARC_PRINTF("freertos lwip matrixssl echo server demo\r\n");
	EMBARC_PRINTF("Please remember to change ECHOSERVER_IP of echoclient demo source code according to the echoserver IP.\r\n");
	EMBARC_PRINTF("And after you have made changes, please rebuild matrixssl freertos echoclient demo and run it.\r\n");

	if (xTaskCreate(echoserver_test, "matrixssl echo server", 4096, (void *)1,
		TSKPRI_ECHOSERVER_TEST, &echoserver_test_handle) != pdPASS) {
		EMBARC_PRINTF("create matrixssl echo server task failed\r\n");
		return -1;
	}
	vTaskSuspend(NULL);

	return 0;
}
/** @} */