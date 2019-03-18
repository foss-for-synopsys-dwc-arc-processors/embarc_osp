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

#include <string.h>
#include "embARC.h"
#include "slip_esp.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "string.h"

//#define DEBUG
#include "embARC_debug.h"

#define CONSOLE_PORT	(7777)

#define SLIP_CHECK_EXP(EXPR, ERROR_CODE)	CHECK_EXP(EXPR, err, ERROR_CODE, error_exit)

struct netconn *conn;
ip_addr_t gateway_default;

/**
 * @brief get current system time in millisecond
 * @return  millisecond
 */
static uint32_t slip_timer_read_ms(void)
{
	return OSP_GET_CUR_MS();
}

static void slip_timer_delay(uint32_t ms)
{
#if  defined(OS_FREERTOS)
	vTaskDelay(ms);
#else
	board_delay_ms(ms, OSP_DELAY_OS_COMPAT_ENABLE);
#endif
}


static int32_t slip_console_connect(ip_addr_t *gw)
{
	err_t err;

	conn = netconn_new(NETCONN_TCP);
	netconn_bind(conn, IP_ADDR_ANY, CONSOLE_PORT);
	err = netconn_connect(conn, gw, CONSOLE_PORT);

	return err;
}

static int32_t slip_console_disconnect(void)
{
	return netconn_delete(conn);
}

static int32_t slip_console_receive(char *result, int32_t data_len)
{
	struct netbuf *buf;
	err_t err;
	u16_t len;
	void *data;

	SLIP_CHECK_EXP(conn != NULL, E_SYS);
	err = netconn_recv(conn, &buf);
	SLIP_CHECK_EXP(err == ERR_OK, E_SYS);
	netbuf_data(buf, &data, &len);
	len = len > data_len ? data_len : len;
	strncpy(result, data, len);
	netbuf_delete(buf);
	DBG("\r\n receive(%d): %s\r\n", len, result);

error_exit:
	return err;
}

static int32_t slip_console_send(char *cmd)
{
	err_t err;

	if(conn == NULL) {
		slip_console_connect(&gateway_default);
		SLIP_CHECK_EXP(conn != NULL, E_SYS);
	}

	DBG("\r\n write(%d): %s\r\n", strlen(cmd), cmd);
	err = netconn_write(conn, cmd, strlen(cmd), NETCONN_COPY);
	SLIP_CHECK_EXP(err == ERR_OK, E_SYS);

error_exit:
	return err;

}

int32_t slip_esp_wnic_get_info(DEV_WNIC_PTR slip_esp_wnic, uint32_t cmd, void *rinfo)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

error_exit:
	return err;
}

int32_t slip_esp_wnic_control(DEV_WNIC_PTR slip_esp_wnic, uint32_t ctrl_cmd, void *param)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

error_exit:
	return err;
}

int32_t slip_esp_wnic_init(DEV_WNIC_PTR slip_esp_wnic, uint32_t network_type)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;
	SLIP_ESP_DEF_PTR esp;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_NOT_INITIALIZED) \
	                || (slip_esp_info->init_status == WNIC_INIT_FAILED), E_OPNED);

	SLIP_CHECK_EXP((network_type == WNIC_NETWORK_TYPE_INFRASTRUCTURE) \
	                || (network_type == WNIC_NETWORK_TYPE_ADHOC), E_PAR);
	esp = (SLIP_ESP_DEF_PTR)slip_esp_info->ctrl;
	slip_esp_info->init_status = WNIC_DURING_INITIALIZATION;  /* start initialization process */
	slip_esp_info->network_type = network_type;
 	DBG("init slip_esp\r\n");
	slip_esp_info->power_status = WNIC_POWER_NORMAL;
	esp->init_timer = slip_timer_read_ms() + SLIP_INIT_TIME_MS;
/*
 * setup a TCP connection between embARC board and esp8266(esp_slip_router), should connect to its console
 * connect success means init done
 */
/* esp_slip_router has ip 192.168.240.1 by default */
	IP4_ADDR(&gateway_default, WF_GATEWAY_1, WF_GATEWAY_2, WF_GATEWAY_3, WF_GATEWAY_4);
	err = slip_console_connect(&gateway_default);
	if(err == E_OK){
		slip_console_receive(esp->console_buf, SLIP_CONSOLE_BUF_LEN);
		DBG("init result %s\r\n", esp->console_buf);
		if(strstr(esp->console_buf, "CMD>") != NULL){
			slip_esp_info->init_status = WNIC_INIT_SUCCESSFUL;
		}
	}

error_exit:
	return err;
}

int32_t slip_esp_poll_init_status(DEV_WNIC_PTR slip_esp_wnic)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);

	err = slip_esp_info->init_status;

error_exit:
	return err;
}

int32_t slip_esp_poll_busy_status(DEV_WNIC_PTR slip_esp_wnic)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	err = slip_esp_info->busy_status;

error_exit:
	return err;
}

int32_t slip_esp_set_network_type(DEV_WNIC_PTR slip_esp_wnic, uint32_t type)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	SLIP_CHECK_EXP((type == WNIC_NETWORK_TYPE_INFRASTRUCTURE) \
	                || (type == WNIC_NETWORK_TYPE_ADHOC), E_PAR);

	/* \todo: what do these types mean for esp? */
	slip_esp_info->network_type = type;

error_exit:
	return err;
}

int32_t slip_esp_set_macaddr(DEV_WNIC_PTR slip_esp_wnic, uint8_t *mac)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;
	DEV_WNIC_ON_OPS_PTR slip_esp_on_ops;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	SLIP_CHECK_EXP((mac != NULL), E_PAR);

	/* Note: the mac of esp can not be set */
	slip_esp_on_ops = &(slip_esp_wnic->wnic_on_ops);
	if (slip_esp_on_ops->on_mac_updated != NULL) {
		slip_esp_on_ops->on_mac_updated(slip_esp_wnic);
	}

error_exit:
	return err;
}

int32_t slip_esp_get_macaddr(DEV_WNIC_PTR slip_esp_wnic, uint8_t *mac)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	SLIP_CHECK_EXP((mac != NULL), E_PAR);

	memcpy(mac, slip_esp_info->mac_addr, WNIC_HDR_LEN);
error_exit:
	return err;
}

int32_t slip_esp_start_scan(DEV_WNIC_PTR slip_esp_wnic)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;
	SLIP_ESP_DEF_PTR esp;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	SLIP_CHECK_EXP((slip_esp_info->scan_status != WNIC_DURING_SCAN), E_CLS);
	SLIP_CHECK_EXP((slip_esp_info->conn_status == WNIC_NOT_CONNECTED) \
	                || (slip_esp_info->conn_status == WNIC_CONNECTED), E_CLS);
	SLIP_CHECK_EXP((slip_esp_info->busy_status == WNIC_IS_FREE), E_CLS);


	slip_esp_info->scan_status = WNIC_DURING_SCAN;
	slip_esp_info->busy_status = WNIC_IS_BUSY;
	esp = (SLIP_ESP_DEF_PTR)slip_esp_info->ctrl;
	esp->scan_timer = slip_timer_read_ms() + SLIP_SCAN_TIME_MS;

	slip_console_send("scan\r\n");
	slip_console_receive(esp->console_buf, SLIP_CONSOLE_BUF_LEN);
error_exit:
	return err;
}

int32_t slip_esp_stop_scan(DEV_WNIC_PTR slip_esp_wnic)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;
	SLIP_ESP_DEF_PTR esp;
	DBG("slip_esp_stop_scan\r\n");

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	esp = (SLIP_ESP_DEF_PTR)slip_esp_info->ctrl;

	if(slip_esp_info->scan_status != WNIC_NOT_SCANED){
		slip_esp_info->scan_status = WNIC_NOT_SCANED;
		slip_console_receive(esp->console_buf, SLIP_CONSOLE_BUF_LEN);
	}

	slip_esp_info->busy_status = WNIC_IS_FREE;

	esp->scan_cnt = 0;
	esp->scan_timer = 0;


	/* \todo: cannot stop the scan of esp */

error_exit:
	return err;
}

int32_t slip_esp_poll_scan_status(DEV_WNIC_PTR slip_esp_wnic)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	err = slip_esp_info->scan_status;

error_exit:
	return err;
}

int32_t slip_esp_get_scan_result_cnt(DEV_WNIC_PTR slip_esp_wnic)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	if (slip_esp_info->scan_status == WNIC_SCAN_FINISHED) {
		err = slip_esp_info->scan_results;
	} else {
		err = E_OK;
	}

error_exit:
	return err;
}

int32_t slip_esp_get_scan_result(DEV_WNIC_PTR slip_esp_wnic, uint32_t index, WNIC_SCAN_RESULT *result)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;
	// SLIP_ESP_AP_INFO_DEF_PTR ap;
	SLIP_ESP_DEF_PTR esp;


	// SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	// SLIP_CHECK_EXP(result != NULL, E_PAR);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	SLIP_CHECK_EXP((slip_esp_info->scan_status == WNIC_SCAN_FINISHED), E_CLS);
	SLIP_CHECK_EXP((index < slip_esp_info->scan_results), E_PAR);

	esp = (SLIP_ESP_DEF_PTR)slip_esp_info->ctrl;

	// ap = &esp->scanned_ap[index];
	//TODO: when scan finished, process the returned string and save scanned ap data to scanned_ap[]

	slip_console_receive(esp->console_buf, SLIP_CONSOLE_BUF_LEN);
	EMBARC_PRINTF("ESP scan result:\r\n%s\r\n", esp->console_buf);
	slip_esp_info->scan_status = WNIC_NOT_SCANED;
	// memcpy(result->bssid, ap->bssid, WNIC_BSSID_MAX_LEN);
	// memcpy(result->ssid, ap->ssid, WNIC_SSID_MAX_LEN);
	// result->ssidlen = strlen(ap->ssid);
	// result->ap_config.apConfig = 0;
	// result->rssi = ap->rssi;
	// result->bsstype = 0;
	// result->channel = ap->channel;

error_exit:
	return err;
}

int32_t slip_esp_wnic_connect(DEV_WNIC_PTR slip_esp_wnic, uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;
	SLIP_ESP_DEF_PTR esp;
	int32_t ssid_len;
	char cmd[64];

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	SLIP_CHECK_EXP((ssid != NULL), E_PAR);
	SLIP_CHECK_EXP(!((key == NULL) && (security != AUTH_SECURITY_OPEN)), E_PAR);

	ssid_len = strlen((const char *)ssid);
	SLIP_CHECK_EXP((ssid_len <= WNIC_SSID_MAX_LEN), E_PAR); /* check ssid len */

	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	SLIP_CHECK_EXP((slip_esp_info->busy_status == WNIC_IS_FREE), E_CLS);
	SLIP_CHECK_EXP((slip_esp_info->conn_status == WNIC_NOT_CONNECTED) \
	                || (slip_esp_info->conn_status == WNIC_CONNECTED), E_CLS);

	esp = (SLIP_ESP_DEF_PTR)slip_esp_info->ctrl;
	if (slip_esp_info->conn_status == WNIC_CONNECTED) {
		/* if connected to a different ssid, just disconnect it */
		if (strcmp((const char *)ssid, (const char *)(slip_esp_info->ssid.ssid)) == 0) {
			err = E_OK;
			goto error_exit;
		} else {
			if (slip_esp_wnic_disconnect(slip_esp_wnic) != E_OK) { /* if disconnect failed just return failed */
				err = E_SYS;
			}
		}
	}

	slip_esp_info->busy_status = WNIC_IS_BUSY;
	slip_esp_info->conn_status = WNIC_DURING_CONNECTION;
	esp->conn_timer = slip_timer_read_ms() + SLIP_CON_TIME_MS;
	memcpy(slip_esp_info->ssid.ssid, ssid, ssid_len);
	sprintf(cmd, "set ssid %s\r\n", ssid);
	slip_console_send(cmd);
	slip_console_receive(esp->console_buf, SLIP_CONSOLE_BUF_LEN);

	sprintf(cmd, "set password %s\r\n", key->key);
	slip_console_send(cmd);
	slip_console_receive(esp->console_buf, SLIP_CONSOLE_BUF_LEN);


	sprintf(cmd, "save\r\n");
	slip_console_send(cmd);
	slip_console_receive(esp->console_buf, SLIP_CONSOLE_BUF_LEN);

	sprintf(cmd, "connect\r\n");
	slip_console_send(cmd);
	slip_console_receive(esp->console_buf, SLIP_CONSOLE_BUF_LEN);

error_exit:
	return err;
}

int32_t slip_esp_poll_conn_status(DEV_WNIC_PTR slip_esp_wnic)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	if (slip_esp_info->busy_status == WNIC_IS_BUSY) {
		err = slip_esp_info->conn_status;
		goto error_exit;
	}
	if (slip_esp_info->conn_status == WNIC_CONNECTED) {
		err = WNIC_CONNECTED;
		goto error_exit;
	}
	slip_esp_info->conn_status = err;

error_exit:
	return err;
}

int32_t slip_esp_wnic_disconnect(DEV_WNIC_PTR slip_esp_wnic)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;
	DEV_WNIC_ON_OPS_PTR slip_esp_on_ops;
	SLIP_ESP_DEF_PTR esp;
	char cmd[64];

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	SLIP_CHECK_EXP((slip_esp_info->conn_status != WNIC_DURING_DISCONNECTION), E_CLS);

	esp = (SLIP_ESP_DEF_PTR)slip_esp_info->ctrl;

	if (slip_esp_poll_conn_status(slip_esp_wnic) == WNIC_CONNECTED) {
		slip_esp_info->conn_status = WNIC_DURING_DISCONNECTION;

		sprintf(cmd, "disconnect\r\n");
		slip_console_send(cmd);
		slip_console_receive(esp->console_buf, SLIP_CONSOLE_BUF_LEN);
		slip_esp_info->conn_status = WNIC_NOT_CONNECTED;
		slip_esp_on_ops = &(slip_esp_wnic->wnic_on_ops);
		if (slip_esp_on_ops->on_disconnected != NULL) {
			slip_esp_on_ops->on_disconnected(slip_esp_wnic);
		}
	}

error_exit:
	return err;
}

int32_t slip_esp_prepare_tx(DEV_WNIC_PTR slip_esp_wnic, uint32_t tx_len)
{
	return E_NOSPT;
}

int32_t slip_esp_add_tx_data(DEV_WNIC_PTR slip_esp_wnic, uint8_t *p_buf, uint32_t len)
{
	return E_NOSPT;
}

int32_t slip_esp_commit_tx(DEV_WNIC_PTR slip_esp_wnic, uint32_t len)
{
	return E_NOSPT;
}

int32_t slip_esp_prepare_rx(DEV_WNIC_PTR slip_esp_wnic)
{
	return E_NOSPT;
}

int32_t slip_esp_get_rx_data(DEV_WNIC_PTR slip_esp_wnic, uint8_t *p_buf, uint32_t len)
{
	return E_NOSPT;
}

int32_t slip_esp_accept_rx(DEV_WNIC_PTR slip_esp_wnic)
{
	return E_NOSPT;
}

int32_t slip_esp_config_power_mode(DEV_WNIC_PTR slip_esp_wnic, int32_t power_mode){
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

error_exit:
	return err;
}

int32_t slip_esp_poll_power_mode(DEV_WNIC_PTR slip_esp_wnic)
{
	err_t err = E_OK;
	DEV_WNIC_INFO_PTR slip_esp_info;

	SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	SLIP_CHECK_EXP((slip_esp_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

error_exit:
	return err;
}

int32_t slip_esp_wnic_reset(DEV_WNIC_PTR slip_esp_wnic)
{
	DBG("slip_esp_wnic_reset\r\n");
	err_t err;
	slip_console_send("reset\r\n");
	err = netconn_delete(conn);
	return err;
}

void slip_esp_period_process(DEV_WNIC_PTR slip_esp_wnic, void *ptr)
{
	DEV_WNIC_INFO_PTR slip_esp_info;
	DEV_WNIC_ON_OPS_PTR slip_esp_on_ops;
	SLIP_ESP_DEF_PTR esp;

	//SLIP_CHECK_EXP(slip_esp_wnic != NULL, E_OBJ);
	slip_esp_info = &(slip_esp_wnic->wnic_info);
	//SLIP_CHECK_EXP((slip_esp_info->power_status == WNIC_POWER_NORMAL), E_CLSED);
	slip_esp_on_ops = &(slip_esp_wnic->wnic_on_ops);
	esp = (SLIP_ESP_DEF_PTR)slip_esp_info->ctrl;

	if (slip_esp_info->scan_status == WNIC_DURING_SCAN) {
		if (slip_timer_read_ms() > esp->scan_timer) {
			EMBARC_PRINTF("ESP scan finished\r\n");
			slip_esp_info->scan_results = 1;//esp->scan_cnt;
			slip_esp_info->scan_status = WNIC_SCAN_FINISHED;
			slip_esp_info->busy_status = WNIC_IS_FREE;
			esp->scan_timer = 0;
			// if (slip_esp_on_ops->on_scan_finished != NULL) {
			// 	slip_esp_on_ops->on_scan_finished(slip_esp_wnic);
			// }
		}
	}
	if (slip_esp_info->init_status == WNIC_NOT_INITIALIZED) {
		if (esp->init_try < SLIP_ESP_INIT_RETRY) {
			DBG("esp init\r\n");
			slip_esp_wnic_init(slip_esp_wnic, WNIC_NETWORK_TYPE_INFRASTRUCTURE);
			esp->init_try++;
		}
	}
	else if (slip_esp_info->init_status == WNIC_DURING_INITIALIZATION) {
		if (slip_timer_read_ms() > esp->init_timer) {
			if (esp->init_try < SLIP_ESP_INIT_RETRY) {
				DBG("esp re-init\r\n");
				esp->init_try++;
			} else {
				DBG("esp init failed\r\n");
				esp->init_try = 0;
				slip_esp_info->init_status = WNIC_INIT_FAILED;
				if (slip_esp_on_ops->on_init_fail != NULL) {
					slip_esp_on_ops->on_init_fail(slip_esp_wnic);
				}
			}
		}
	}
	if (slip_esp_info->conn_status == WNIC_DURING_CONNECTION) {
		if (slip_timer_read_ms() > esp->conn_timer) {
			DBG("esp connection timeout check\r\n");
			slip_console_send("show\r\n");
			slip_timer_delay(500);
			slip_console_receive(esp->console_buf, SLIP_CONSOLE_BUF_LEN);
			if(strstr(esp->console_buf, "External IP") != NULL){
				slip_esp_info->conn_status = WNIC_CONNECTED;
				EMBARC_PRINTF("INFO:\r\n%s\r\n", esp->console_buf);
				//TODO: save external IP to SLIPWIFI_IF -> external_addr
				if (slip_esp_on_ops->on_connected != NULL) {
					slip_esp_on_ops->on_connected(slip_esp_wnic);
				}
			} else {
				slip_esp_info->conn_status = WNIC_NOT_CONNECTED;
				EMBARC_PRINTF("esp connection time out\r\n");
				if (slip_esp_on_ops->on_disconnected != NULL) {
					slip_esp_on_ops->on_disconnected(slip_esp_wnic);
				}
			}
			slip_esp_info->busy_status = WNIC_IS_FREE;
		}
	}

error_exit:
	return;
}