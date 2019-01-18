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
#include "esp8266.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

#define DBG_MORE
#include "embARC_debug.h"

int32_t esp8266_init(ESP8266_DEF_PTR obj, uint32_t baudrate)
{
	obj->p_at = malloc(sizeof(AT_PARSER_DEF));

	if (obj->p_at == NULL) {
		return E_NOMEM;
	}

	memset(obj->p_at, 0, sizeof(AT_PARSER_DEF));
	obj->p_at->uart_id = ESP8266_UART_ID;
	// add any esp8266 init code here
	obj->wifi_connected = false;
	obj->trans_mode = ESP8266_NORMALSEND;
	return at_parser_init(obj->p_at, baudrate);
}

void esp8266_deinit(ESP8266_DEF_PTR obj)
{
	if (obj->wifi_connected) {
		esp8266_wifi_disconnect(obj);
	}

	at_parser_deinit(obj->p_at);
	// add any esp8266 deinit code here
	obj->trans_mode = ESP8266_NORMALSEND;
	free(obj->p_at);
}

int32_t esp8266_test(ESP8266_DEF_PTR obj)
{
	return at_test(obj->p_at);
}

//TODO: MAC address functions
//uint32_t AT_MAC_get();// AT+CIPSTAMAC_CUR?
//uint32_t AT_MAC_set();// AT+CIPSTAMAC_CUR="18:fe:35:98:d3:7b"

int32_t esp8266_wifi_mode_get(ESP8266_DEF_PTR obj, bool flash)
{
	char rcv_buf[512];
	AT_PARSER_DEF_PTR p_at = obj->p_at;
	char *pos;
	uint32_t ret = 0;
	at_send_cmd(p_at, AT_READ, flash?"CWMODE_DEF":"CWMODE_CUR", NULL);

	if (at_get_reply(p_at, rcv_buf, AT_NORMAL_TIMEOUT)==AT_OK) {
		pos = strstr(rcv_buf, flash?"CWMODE_DEF:":"CWMODE_CUR:");

		if (sscanf(pos, flash?"CWMODE_DEF:%d":"CWMODE_CUR:%d", &ret)>0) {
			dbg_printf(DBG_LESS_INFO, flash?"CWMODE_DEF = %d\n":"CWMODE_CUR = %d\n", ret);
			return ret;
		}
	}

	return AT_ERROR;
}

int32_t esp8266_wifi_mode_set(ESP8266_DEF_PTR obj, ESP8266_WIFI_MODE mode, bool flash)
{
	char rcv_buf[512];
	char mode_str[4]= {0};
	sprintf(mode_str, "%d", mode);
	at_send_cmd(obj->p_at, AT_WRITE, flash?"CWMODE_DEF":"CWMODE_CUR", mode_str, NULL);
	return at_get_reply(obj->p_at, rcv_buf, AT_NORMAL_TIMEOUT);
}

/* rcv_buf should be large enough for scan result */
int32_t esp8266_wifi_scan(ESP8266_DEF_PTR obj, char *rcv_buf, char *ssid)
{
	char cmd_str[512]= {0};
	sprintf(cmd_str, "CWLAP=%s", ssid);
	at_send_cmd(obj->p_at, AT_EXECUTE, cmd_str);
	return at_get_reply(obj->p_at, rcv_buf, AT_LONG_TIMEOUT);
}

/*
* obj: ESP8266_DEF_PTR (esp8266 struct pointer object)
* ssid: AP ssid, need to be quoted
* pwd: AP password, need to be quoted
* flash: configuration save (true) or not save (false) in the flash
*
* Return Value:
* AT_OK == 0, success
* AT_ERROR == -1, error occurs
* CWJAP==1, connection timeout
* CWJAP==2, wrong password
* CWJAP==3, cannot find the target AP
* CWJAP==4, connection failed
*/
int32_t esp8266_wifi_connect(ESP8266_DEF_PTR obj, AT_STRING ssid, AT_STRING pwd, bool flash)
{
	char rcv_buf[512];
	AT_PARSER_DEF_PTR p_at = obj->p_at;
	char *pos;
	uint32_t ret = 0;

	if ( esp8266_wifi_mode_set(obj, ESP8266_STA, false) == AT_OK) {
		memset(rcv_buf, 0, sizeof(rcv_buf));
		at_send_cmd(p_at, AT_WRITE, flash?"CWJAP_DEF":"CWJAP_CUR", ssid, pwd, NULL);

		if (at_get_reply(p_at, rcv_buf, AT_EXTRA_TIMEOUT)==AT_OK) {
			pos = strstr(rcv_buf, "CWJAP:");

			//check if any error happen during connection setting up
			if (sscanf(pos, "CWJAP:%d", &ret)>0) {
				dbg_printf(DBG_LESS_INFO, "CWJAP = %d\n", ret);
				return ret;
			} else {
				obj->wifi_connected = true;
				return AT_OK;
			}
		}
	}

	return AT_ERROR;
}

int32_t esp8266_wifi_disconnect(ESP8266_DEF_PTR obj)
{
	char rcv_buf[512];
	AT_PARSER_DEF_PTR p_at = obj->p_at;
	at_send_cmd(p_at, AT_EXECUTE, "CWQAP");

	if (at_get_reply(p_at, rcv_buf, AT_NORMAL_TIMEOUT)== AT_OK) {
		obj->wifi_connected = false;
		return AT_OK;
	} else {
		return AT_ERROR;
	}
}

int32_t esp8266_address_get(ESP8266_DEF_PTR obj)
{
	char rcv_buf[512];
	AT_PARSER_DEF_PTR p_at = obj->p_at;
	at_send_cmd(p_at, AT_EXECUTE, "CIFSR");
	return at_get_reply(p_at, rcv_buf, AT_NORMAL_TIMEOUT);
}

/*
* Return Value:
* AT_ERROR == -1, error occurs
* STATUS==2, The ESP8266 Station is connected to an AP and its IP is obtained
* STATUS==3, The ESP8266 Station has created a TCP or UDP transmission
* STATUS==4, The TCP or UDP transmission of ESP8266 Station is disconnected
* STATUS==5, The ESP8266 Station does NOT connect to an AP
*/
int32_t esp8266_conn_status(ESP8266_DEF_PTR obj)
{
	char rcv_buf[512];
	AT_PARSER_DEF_PTR p_at = obj->p_at;
	char *pos;
	uint32_t ret = 0;
	at_send_cmd(p_at, AT_EXECUTE, "CIPSTATUS");

	if (at_get_reply(p_at, rcv_buf, AT_NORMAL_TIMEOUT)==AT_OK) {
		pos = strstr(rcv_buf, "STATUS:");

		if (sscanf(pos, "STATUS:%d", &ret)>0) {
			dbg_printf(DBG_LESS_INFO, "STATUS = %d\n", ret);
			return ret;
		}
	}

	return AT_ERROR;
}

int32_t esp8266_dns_lookup(ESP8266_DEF_PTR obj, char *domain_ip, AT_STRING domain_name)
{
	char rcv_buf[512];
	AT_PARSER_DEF_PTR p_at = obj->p_at;
	char *pos;
	int len=0;
	at_send_cmd(p_at, AT_WRITE, "CIPDOMAIN", domain_name, NULL);

	if (at_get_reply(p_at, rcv_buf, AT_LONG_TIMEOUT)==AT_OK) {
		dbg_printf(DBG_MORE_INFO, "rcv_buf = %s\n", rcv_buf);
		pos = strstr(rcv_buf, "CIPDOMAIN:");

		if (pos!=NULL) {
			pos += strlen("CIPDOMAIN:");
			dbg_printf(DBG_LESS_INFO, "domain_ip = %s, len = %d\n", pos, strlen(pos));
			len=strstr(pos, "\r\n") - pos;
			strncpy(domain_ip, pos, len);
			domain_ip[len]='\0';
			dbg_printf(DBG_LESS_INFO, "domain_ip = %s, len = %d\n", domain_ip, strlen(domain_ip));
			return AT_OK;
		}
	}

	return AT_ERROR;
}

int32_t esp8266_tcp_connect(ESP8266_DEF_PTR obj, AT_STRING server_IP, uint32_t port)
{
	char rcv_buf[512];
	char IP_Str[32], port_Str[16];
	sprintf(IP_Str, "\"%s\"", server_IP);
	sprintf(port_Str, "%d", port);
	at_send_cmd(obj->p_at, AT_WRITE, "CIPSTART", "\"TCP\"", IP_Str, port_Str, NULL);
	return at_get_reply(obj->p_at, rcv_buf, 800);
}

static int32_t esp8266_tcp_server(ESP8266_DEF_PTR obj, uint32_t mode, uint32_t port)
{
	char rcv_buf[512];
	char mode_buf[5];
	char port_buf[10];

	sprintf(mode_buf, "%d", mode);
	sprintf(port_buf, "%d", port);

	AT_PARSER_DEF_PTR p_at = obj->p_at;
	at_send_cmd(p_at, AT_WRITE, "CIPSERVER", mode_buf, port_buf, NULL);
	return at_get_reply(obj->p_at, rcv_buf, AT_NORMAL_TIMEOUT);
}

int32_t esp8266_tcp_server_open(ESP8266_DEF_PTR obj, uint32_t port)
{
	char rcv_buf[512];
	AT_PARSER_DEF_PTR p_at = obj->p_at;
	at_send_cmd(p_at, AT_WRITE, "CIPMUX", "1", NULL);

	if (at_get_reply(p_at, rcv_buf, AT_LONG_TIMEOUT)==AT_OK) {
		return esp8266_tcp_server(obj, 1, port);
	}
}

int32_t esp8266_tcp_server_close(ESP8266_DEF_PTR obj, uint32_t port)
{
	return esp8266_tcp_server(obj, 0, port);
}

int32_t esp8266_transmission_mode(ESP8266_DEF_PTR obj, ESP8266_TRANS_MODE mode)
{
	char rcv_buf[512];
	uint32_t ret;
	char mode_str[4]= {0};
	sprintf(mode_str, "%d", mode);
	at_send_cmd(obj->p_at, AT_WRITE, "CIPMODE", mode_str, NULL);
	ret = at_get_reply(obj->p_at, rcv_buf, AT_NORMAL_TIMEOUT);

	if (ret == AT_OK) {
		obj->trans_mode = mode;
		return AT_OK;
	} else {
		return AT_ERROR;
	}
}

int32_t esp8266_passthr_start(ESP8266_DEF_PTR obj)
{
	char rcv_buf[512];

	if (obj->wifi_connected) {
		if (esp8266_transmission_mode(obj, ESP8266_PASSTHR)==AT_OK) {
			at_send_cmd(obj->p_at, AT_EXECUTE, "CIPSEND");
			return at_get_reply(obj->p_at, rcv_buf, AT_NORMAL_TIMEOUT);
		}
	}

	return AT_ERROR;
}

int32_t esp8266_passthr_end(ESP8266_DEF_PTR obj)
{
	char passthr_end_symbol[] = "+++";
	//uint32_t ret;
	//board_delay_ms(500, OSP_DELAY_OS_COMPAT_ENABLE);
	//ret = at_write(obj->p_at, passthr_end_symbol, 3);//do not send '\0' as string ending
	at_write(obj->p_at, passthr_end_symbol, 3);//do not send '\0' as string ending
	//board_delay_ms(500, OSP_DELAY_OS_COMPAT_ENABLE);
	//if(ret == 3){
	obj->trans_mode = ESP8266_NORMALSEND;
	return AT_OK;
	//} else {
	//	return AT_ERROR;
	//}
}

int32_t esp8266_passthr_write(ESP8266_DEF_PTR obj, char *buf, uint32_t cnt)
{
	if (obj->wifi_connected && obj->trans_mode == ESP8266_PASSTHR) {
		return at_write(obj->p_at, buf, cnt);
	}

	return AT_ERROR;
}

uint32_t esp8266_normal_write(ESP8266_DEF_PTR obj, char *buf, uint32_t cnt)
{
	char rcv_buf[512];
	char cnt_str[16]= {0};

	if (obj->wifi_connected) {
		if (obj->trans_mode != ESP8266_NORMALSEND) {
			if (esp8266_transmission_mode(obj, ESP8266_NORMALSEND)!=AT_OK) {
				return AT_ERROR;
			} else {
				obj->trans_mode = ESP8266_NORMALSEND;
			}
		}

		sprintf(cnt_str, "%d", cnt);
		at_send_cmd(obj->p_at, AT_WRITE, "CIPSEND", cnt_str, NULL);

		if (at_get_reply(obj->p_at, rcv_buf, AT_NORMAL_TIMEOUT)==AT_OK) {
			at_write(obj->p_at, buf, cnt);
			return at_get_reply(obj->p_at, rcv_buf, AT_NORMAL_TIMEOUT);
		}
	}

	return AT_ERROR;
}

uint32_t esp8266_connect_write(ESP8266_DEF_PTR obj, char *buf, char *connect, uint32_t cnt)
{
	char rcv_buf[512];
	char cnt_str[16]= {0};

	if (obj->wifi_connected) {
		if (obj->trans_mode != ESP8266_NORMALSEND) {
			if (esp8266_transmission_mode(obj, ESP8266_NORMALSEND)!=AT_OK) {
				return AT_ERROR;
			} else {
				obj->trans_mode = ESP8266_NORMALSEND;
			}
		}

		sprintf(cnt_str, "%d", cnt);
		at_send_cmd(obj->p_at, AT_WRITE, "CIPSEND", connect, cnt_str, NULL);

		if (at_get_reply(obj->p_at, rcv_buf, AT_NORMAL_TIMEOUT)==AT_OK) {
			at_write(obj->p_at, buf, cnt);
			return at_get_reply(obj->p_at, rcv_buf, AT_NORMAL_TIMEOUT);
		}
	}

	return AT_ERROR;
}

int32_t esp8266_read(ESP8266_DEF_PTR obj, char *buf, uint32_t timeout)
{
	uint32_t cur_ofs = 0;
	uint32_t read_cnt;
	uint32_t cur_time;

	if (obj->wifi_connected && obj->trans_mode == ESP8266_PASSTHR) {
		cur_time = OSP_GET_CUR_MS();

		do {
			read_cnt = at_read(obj->p_at, &buf[cur_ofs], 1);
			cur_ofs += read_cnt;
		} while ((OSP_GET_CUR_MS()-cur_time) < timeout);

		buf[cur_ofs] = '\0';
		dbg_printf(DBG_LESS_INFO, "[%s]%d: \"%s\" (%d)\r\n", __FUNCTION__, __LINE__, buf, strlen(buf));
		return AT_OK;
	}

	return AT_ERROR;
}

uint32_t esp8266_nread(ESP8266_DEF_PTR obj, char *buf, uint32_t n)
{
	if (obj->wifi_connected) {
		return at_read(obj->p_at, buf, n);
	}

	return AT_ERROR;
}


//at_httpserver
int32_t esp8266_CIPCLOSE(ESP8266_DEF_PTR obj, char *conn_buf)
{
	char rcv_buf[512];
	at_send_cmd(obj->p_at, AT_WRITE, "CIPCLOSE", conn_buf, NULL);
	return at_get_reply(obj->p_at, rcv_buf, AT_EXTRA_TIMEOUT);
}