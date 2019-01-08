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

#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "at_parser.h"
#include "board.h"


/** uart id which esp8266 uses */
#ifndef ESP8266_UART_ID	//you can define this at board header file (e.g. emsk.h)
#if defined(BOARD_EMSK) || defined(BOARD_HSDK)
#define ESP8266_UART_ID		DW_UART_0_ID
#elif defined(BOARD_EMSDP)|| defined(BOARD_IOTDK)
#define ESP8266_UART_ID		DFSS_UART_1_ID
#else
#warning esp8266 is not supported on this board!
#endif
#endif

typedef enum{
	ESP8266_STA=1,
	ESP8266_AP,
	ESP8266_AP_STA
}ESP8266_WIFI_MODE;

typedef enum{
	ESP8266_NORMALSEND=0,
	ESP8266_PASSTHR
}ESP8266_TRANS_MODE;

typedef struct{
	AT_PARSER_DEF_PTR p_at;
	//add other members here if needed
	bool wifi_connected;
	ESP8266_TRANS_MODE trans_mode;
}ESP8266_DEF, *ESP8266_DEF_PTR;

#define ESP8266_DEFINE(NAME) \
	ESP8266_DEF __ ## NAME = { \
			.p_at = NULL, \
	}; \
	ESP8266_DEF_PTR NAME = &__ ## NAME

int32_t esp8266_init(ESP8266_DEF_PTR obj, uint32_t baudrate);
void esp8266_deinit(ESP8266_DEF_PTR obj);
int32_t esp8266_test(ESP8266_DEF_PTR obj);

int32_t esp8266_wifi_mode_get(ESP8266_DEF_PTR obj, bool flash);
int32_t esp8266_wifi_mode_set(ESP8266_DEF_PTR obj, ESP8266_WIFI_MODE, bool flash);
int32_t esp8266_wifi_scan(ESP8266_DEF_PTR obj, char *rcv_buf, char *ssid);
int32_t esp8266_wifi_connect(ESP8266_DEF_PTR obj, AT_STRING ssid, AT_STRING pwd, bool flash);
int32_t esp8266_wifi_disconnect(ESP8266_DEF_PTR obj);
int32_t esp8266_address_get(ESP8266_DEF_PTR obj);
int32_t esp8266_conn_status(ESP8266_DEF_PTR obj);

int32_t esp8266_dns_lookup(ESP8266_DEF_PTR obj, char *domain_ip, AT_STRING domain_name);
int32_t esp8266_tcp_connect(ESP8266_DEF_PTR obj, AT_STRING server_IP, uint32_t port);
int32_t esp8266_tcp_server_open(ESP8266_DEF_PTR obj, uint32_t port);
int32_t esp8266_tcp_server_close(ESP8266_DEF_PTR obj, uint32_t port);

int32_t esp8266_transmission_mode(ESP8266_DEF_PTR obj, ESP8266_TRANS_MODE mode);
int32_t esp8266_passthr_start(ESP8266_DEF_PTR obj);
int32_t esp8266_passthr_end(ESP8266_DEF_PTR obj);
int32_t esp8266_passthr_write(ESP8266_DEF_PTR obj, char *buf, uint32_t cnt);
uint32_t esp8266_normal_write(ESP8266_DEF_PTR obj, char *buf, uint32_t cnt);
uint32_t esp8266_connect_write(ESP8266_DEF_PTR obj, char *buf, char *connect, uint32_t cnt);
int32_t esp8266_read(ESP8266_DEF_PTR obj, char *buf, uint32_t timeout);
uint32_t esp8266_nread(ESP8266_DEF_PTR obj, char *buf, uint32_t n);

int32_t esp8266_CIPCLOSE(ESP8266_DEF_PTR obj, char *conn_buf);

#endif