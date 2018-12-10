/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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
--------------------------------------------- */
#include "embARC.h"
#include "embARC_debug.h"

#include "board.h"
#include "esp8266.h"

#include <stdio.h>
#include <string.h>

#define WIFI_SSID   "\"embARC\""
#define WIFI_PWD    "\"qazwsxedc\""

static char http_get[] = "GET /";
static char http_IDP[] = "+IPD,";
static char http_html_header[] = "HTTP/1.x 200 OK\r\nContent-type: text/html\r\n\r\n";
static char http_html_body_1[] =
    "<html><head><title>ESP8266_AT_HttpServer</title></head><body><h1>Welcome to this Website</h1>";
static char http_html_body_2[] =
    "<p>This Website is used to test the AT command about HttpServer of ESP8266.</p></body></html>";


static char http_server_buf[2048];

int main(void)
{
	char *conn_buf;

	//ESP8266 Init
	EMBARC_PRINTF("============================ Init ============================\n");

	ESP8266_DEFINE(esp8266);
	esp8266_init(esp8266, UART_BAUDRATE_115200);
	at_test(esp8266->p_at);
	board_delay_ms(100, 1);

	//Set Mode
	EMBARC_PRINTF("============================ Set Mode ============================\n");

	esp8266_wifi_mode_get(esp8266, false);
	board_delay_ms(100, 1);
	esp8266_wifi_mode_set(esp8266, 3, false);
	board_delay_ms(100, 1);

	//Connect WiFi
	EMBARC_PRINTF("============================ Connect WiFi ============================\n");

	do {
		esp8266_wifi_scan(esp8266, http_server_buf, WIFI_SSID);
		EMBARC_PRINTF("Searching for WIFI %s ......\n", WIFI_SSID);
		board_delay_ms(100, 1);
	} while (strstr(http_server_buf, WIFI_SSID)==NULL);

	EMBARC_PRINTF("WIFI %s found! Try to connect\n", WIFI_SSID);

	while (esp8266_wifi_connect(esp8266, WIFI_SSID, WIFI_PWD, false) != AT_OK) {
		EMBARC_PRINTF("WIFI %s connect failed\n", WIFI_SSID);
		board_delay_ms(100, 1);
	}

	EMBARC_PRINTF("WIFI %s connect succeed\n", WIFI_SSID);

	//Creat Server
	EMBARC_PRINTF("============================ Connect Server ============================\n");

	esp8266_tcp_server_open(esp8266, 80);

	//Show IP
	EMBARC_PRINTF("============================ Show IP ============================\n");

	esp8266_address_get(esp8266);
	board_delay_ms(1000, 1);

	while (1) {
		memset(http_server_buf, 0, sizeof(http_server_buf));
		at_read(esp8266->p_at ,http_server_buf ,1000);
		board_delay_ms(200, 1);
		//EMBARC_PRINTF("Alive\n");

		if (strstr(http_server_buf, http_get) != NULL) {
			EMBARC_PRINTF("============================ send ============================\n");

			EMBARC_PRINTF("\nThe message is:\n%s\n", http_server_buf);

			conn_buf = strstr(http_server_buf, http_IDP) + 5;
			*(conn_buf+1) = 0;

			EMBARC_PRINTF("Send Start\n");
			board_delay_ms(10, 1);

			esp8266_connect_write(esp8266, http_html_header, conn_buf, (sizeof(http_html_header)-1));
			board_delay_ms(100, 1);

			esp8266_connect_write(esp8266, http_html_body_1, conn_buf, (sizeof(http_html_body_1)-1));
			board_delay_ms(300, 1);

			esp8266_connect_write(esp8266, http_html_body_2, conn_buf, (sizeof(http_html_body_2)-1));
			board_delay_ms(300, 1);

			esp8266_CIPCLOSE(esp8266, conn_buf);

			EMBARC_PRINTF("Send Finish\n");
		}
	}

	return E_OK;
}