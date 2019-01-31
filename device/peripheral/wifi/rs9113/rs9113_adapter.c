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
#include "string.h"
#include "stdlib.h"
#include "dev_spi.h"
#include "dev_wnic.h"
#include "embARC_error.h"
//TODO: move SPI related part to emdk\drivers\pmwifi
/* include board.h for SPI wiring and frequency */
#include "board.h"

#include "rs9113_adapter.h"
#include "rsi_driver.h"

#define DBG_LESS
#include "embARC_debug.h"
#define RS9113_CHECK_EXP(EXPR, ERROR_CODE)	CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

typedef struct {
	uint8_t *buf;
	uint32_t len;
	uint32_t cur_index;
} rs9113_pbuf_t, *rs9113_pbuf_t_ptr;

static rs9113_pbuf_t rs9113_pbuf;
static void *driver_task_handle = NULL;
/*
 * 3680 is a suitable value for WLAN only, take reference to document and
 * rsi_driver_init() for other cases
 */
#define RSI_WLAN_CTRL_BUFFER_SIZE  3680

#define RSI_BUFFER_USE_MALLOC 0
#if RSI_BUFFER_USE_MALLOC
static uint8_t *rs_buffer;
#else
static uint8_t rs_buffer[RSI_WLAN_CTRL_BUFFER_SIZE];
#endif /*RSI_BUFFER_USE_MALLOC*/

static int32_t _rs9113_spi_open(uint32_t freq, uint32_t clk_mode)
{
	int32_t ercd = E_OK;
	DEV_SPI_PTR rsi_spi_ptr;

	rsi_spi_ptr = spi_get_dev(BOARD_RSI_SPI_ID);
	RS9113_CHECK_EXP(rsi_spi_ptr != NULL, E_OBJ);

	dbg_printf(DBG_MORE_INFO, "rsi_spi_open DEV_MASTER_MODE= 0x%x (%d),freq= 0x%x (%d) \r\n",
	           DEV_MASTER_MODE, DEV_MASTER_MODE, freq, freq);

	ercd = rsi_spi_ptr->spi_open(DEV_MASTER_MODE, freq);

	if ((ercd == E_OPNED) || (ercd == E_OK)) {
		ercd = rsi_spi_ptr->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(clk_mode));
	}

error_exit:
	return ercd;
}

static int32_t _rs9113_spi_close(void)
{
	int32_t ercd = E_OK;
	DEV_SPI_PTR rsi_spi_ptr;

	rsi_spi_ptr = spi_get_dev(BOARD_RSI_SPI_ID);
	RS9113_CHECK_EXP(rsi_spi_ptr != NULL, E_OBJ);

	ercd = rsi_spi_ptr->spi_close();

error_exit:
	return ercd;
}

static int32_t _rs9113_wnic_deinit(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);

	rs9113_info = &(rs9113_wnic->wnic_info);
	ercd = rsi_wireless_deinit();

#if RSI_BUFFER_USE_MALLOC

	if (rs_buffer != NULL) {
		free(rs_buffer);
	}

#else
#endif
	_rs9113_spi_close();
	rs9113_info->init_status = WNIC_NOT_INITIALIZED;
	rs9113_info->power_status = WNIC_POWER_OFF;

error_exit:
	return ercd;
}

int32_t rs9113_wnic_get_info(DEV_WNIC_PTR rs9113_wnic, uint32_t cmd, void *rinfo)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

error_exit:
	return ercd;
}

int32_t rs9113_wnic_control(DEV_WNIC_PTR rs9113_wnic, uint32_t ctrl_cmd, void *param)
{
	return E_OK;
}

int32_t rs9113_wnic_init(DEV_WNIC_PTR rs9113_wnic, uint32_t network_type)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;
	DEV_WNIC_ON_OPS *rs9113_on_ops;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_NOT_INITIALIZED) \
	                 || (rs9113_info->init_status==WNIC_INIT_FAILED), E_OPNED);

	RS9113_CHECK_EXP((network_type==WNIC_NETWORK_TYPE_INFRASTRUCTURE) \
	                 || (network_type==WNIC_NETWORK_TYPE_ADHOC), E_PAR);

	rs9113_info->init_status = WNIC_DURING_INITIALIZATION;  /* start initialization process */
	rs9113_info->network_type = network_type; /* set network type used next */

	ercd = _rs9113_spi_open(10000000, SPI_CLK_MODE_0);
	dbg_printf(DBG_MORE_INFO, "_rs9113_spi_open return 0x%x\r\n", ercd);
	RS9113_CHECK_EXP(ercd == E_OK, E_SYS);
	//! Driver initialization
#if RSI_BUFFER_USE_MALLOC
	rs_buffer = malloc(RSI_WLAN_CTRL_BUFFER_SIZE);
#else
#endif
	RS9113_CHECK_EXP(rs_buffer != NULL, E_NOMEM);
	ercd = rsi_driver_init(rs_buffer, RSI_WLAN_CTRL_BUFFER_SIZE);
	dbg_printf(DBG_MORE_INFO, "rsi_driver_init return 0x%x\r\n", ercd);
	RS9113_CHECK_EXP((ercd >= 0) && (ercd <= RSI_WLAN_CTRL_BUFFER_SIZE), E_SYS);

	//! RS9113 intialisation
	ercd = rsi_device_init(RSI_LOAD_IMAGE_I_FW);
	dbg_printf(DBG_MORE_INFO, "rsi_device_init return 0x%x\r\n", ercd);
	RS9113_CHECK_EXP(ercd == E_OK, E_SYS);

	//! Task created for Driver task
	if (driver_task_handle == NULL) {
		rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
		 (uint8_t *)"driver_task", RSI_DRIVER_TASK_STACK_SIZE, NULL,
		  RSI_DRIVER_TASK_PRIORITY, &driver_task_handle);
	}

	//a short delay to wait the device get ready
	rsi_delay_ms(10);
	//TODO: set opermode and coex_mode for rsi_wireless_init(uint16_t opermode, uint16_t coex_mode);
	ercd = rsi_wireless_init(0, 0);
	dbg_printf(DBG_MORE_INFO, "rsi_wireless_init return 0x%x\r\n", ercd);
	RS9113_CHECK_EXP(ercd == E_OK, E_SYS);
	//register rs9113 on receive callback function
	rsi_wlan_register_callbacks(RSI_WLAN_DATA_RECEIVE_NOTIFY_CB, (void *)rs9113_info->ctrl);

	rs9113_info->power_status = WNIC_POWER_NORMAL;
	rs9113_info->init_status = WNIC_INIT_SUCCESSFUL;  /* stop initialization process */
	rs9113_on_ops = &(rs9113_wnic->wnic_on_ops);

	if (rs9113_on_ops->on_init_success != NULL) {
		rs9113_on_ops->on_init_success(rs9113_wnic);
	}

error_exit:
	return ercd;
}

int32_t rs9113_poll_init_status(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);

	ercd = rs9113_info->init_status;

error_exit:
	return ercd;
}

int32_t rs9113_poll_busy_status(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	ercd = rs9113_info->busy_status;

error_exit:
	return ercd;
}

int32_t rs9113_set_network_type(DEV_WNIC_PTR rs9113_wnic, uint32_t type)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	RS9113_CHECK_EXP((type==WNIC_NETWORK_TYPE_INFRASTRUCTURE) \
	                 || (type==WNIC_NETWORK_TYPE_ADHOC), E_PAR);

	rs9113_info->network_type = type;

error_exit:
	return ercd;
}

int32_t rs9113_set_macaddr(DEV_WNIC_PTR rs9113_wnic, uint8_t *mac)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;
	DEV_WNIC_ON_OPS *rs9113_on_ops;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	RS9113_CHECK_EXP((mac!=NULL), E_PAR);

	rsi_wlan_set(RSI_SET_MAC_ADDRESS, (uint8_t *) mac, WNIC_HDR_LEN);
	memcpy(rs9113_info->mac_addr, mac, WNIC_HDR_LEN);
	rs9113_info->mac_status = WNIC_MAC_UPDATED;

	rs9113_on_ops = &(rs9113_wnic->wnic_on_ops);

	if (rs9113_on_ops->on_mac_updated != NULL) {
		rs9113_on_ops->on_mac_updated(rs9113_wnic);
	}

error_exit:
	return ercd;
}

int32_t rs9113_get_macaddr(DEV_WNIC_PTR rs9113_wnic, uint8_t *mac)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	RS9113_CHECK_EXP((mac!=NULL), E_PAR);

	if (rs9113_info->mac_status == WNIC_MAC_NOT_UPDATED) {
		ercd = rsi_wlan_get(RSI_MAC_ADDRESS, rs9113_info->mac_addr, WNIC_HDR_LEN);
		dbg_printf(DBG_MORE_INFO, "rsi_wlan_get return %d\r\n", ercd);
		rs9113_info->mac_status = WNIC_MAC_UPDATED;
	}

	memcpy(mac, rs9113_info->mac_addr, WNIC_HDR_LEN);
error_exit:
	return ercd;
}

static rsi_rsp_scan_t scan_result;

int32_t rs9113_start_scan(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	//TODO: implement scan functions by using rsi_wlan_scan()
	rs9113_info->scan_status = WNIC_DURING_SCAN;
	rs9113_info->busy_status = WNIC_IS_BUSY;
	ercd = rsi_wlan_scan(NULL, 0, &scan_result, sizeof(rsi_rsp_scan_t));
	dbg_printf(DBG_MORE_INFO, "rsi_wlan_scan return %d\r\n", ercd);
	rs9113_info->scan_results = scan_result.scan_count[0];
	rs9113_info->scan_status = WNIC_SCAN_FINISHED;
	rs9113_info->busy_status = WNIC_IS_FREE;
error_exit:
	return ercd;
}

int32_t rs9113_stop_scan(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	rs9113_info->scan_status = WNIC_NOT_SCANED;
	rs9113_info->busy_status = WNIC_IS_FREE;


error_exit:
	return ercd;
}

int32_t rs9113_poll_scan_status(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	ercd = rs9113_info->scan_status;
error_exit:
	return ercd;
}

int32_t rs9113_get_scan_result_cnt(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	if (rs9113_info->scan_status == WNIC_SCAN_FINISHED) {
		ercd = rs9113_info->scan_results;
	} else {
		ercd = 0;
	}

error_exit:
	return ercd;
}


int32_t rs9113_get_scan_result(DEV_WNIC_PTR rs9113_wnic, uint32_t index, WNIC_SCAN_RESULT *result)
{
	static rsi_scan_info_t *p_scan_result =  (rsi_scan_info_t *)(&(scan_result.scan_info));
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;
	rsi_scan_info_t *p_scan_result_temp;
	WNIC_AP_CONFIG apConfig_result;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	RS9113_CHECK_EXP(result!=NULL, E_PAR);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	RS9113_CHECK_EXP((rs9113_info->scan_status==WNIC_SCAN_FINISHED), E_CLS);
	RS9113_CHECK_EXP((index < rs9113_info->scan_results), E_PAR);

	p_scan_result_temp = p_scan_result + index;

	memcpy(result->bssid, p_scan_result_temp->bssid, WNIC_BSSID_MAX_LEN);
	memcpy(result->ssid, p_scan_result_temp->ssid, WNIC_SSID_MAX_LEN);
	result->ssid[WNIC_SSID_MAX_LEN-1] = '\0'; /* make end of ssid */
	result->ssidlen = strlen((char *)result->ssid);

	switch (p_scan_result_temp->security_mode) {
		case 0://open
			apConfig_result.ap_cfg.privacy = 0;

		case 1://WPA
			apConfig_result.ap_cfg.wpa = 1;
			apConfig_result.ap_cfg.privacy = 1;
			break;

		case 2://WPA2
			apConfig_result.ap_cfg.wpa2 = 1;
			apConfig_result.ap_cfg.wpa = 1;
			apConfig_result.ap_cfg.privacy = 1;
			break;

		case 3://WEP
			apConfig_result.ap_cfg.privacy = 1;
			break;

		case 4://WAP Enterprise
		case 5://WPA2 Enterprise
		default:
			break;
	}

	result->ap_config.apConfig = apConfig_result.apConfig;
	result->rssi = p_scan_result_temp->rssi_val;
	result->bsstype = p_scan_result_temp->network_type;
	result->channel = p_scan_result_temp->rf_channel;

error_exit:
	return ercd;
}

int32_t rs9113_wnic_connect(DEV_WNIC_PTR rs9113_wnic, uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;
	int32_t ssid_len;
	DEV_WNIC_ON_OPS *rs9113_on_ops;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	RS9113_CHECK_EXP((ssid != NULL), E_PAR);
	RS9113_CHECK_EXP(!((key == NULL) && (security != AUTH_SECURITY_OPEN)), E_PAR);

	ssid_len = strlen((const char *)ssid);
	RS9113_CHECK_EXP((ssid_len <= WNIC_SSID_MAX_LEN), E_PAR); /* check ssid len */

	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	RS9113_CHECK_EXP((rs9113_info->busy_status==WNIC_IS_FREE), E_CLS);
	RS9113_CHECK_EXP((rs9113_info->conn_status==WNIC_NOT_CONNECTED) \
	                 || (rs9113_info->conn_status==WNIC_CONNECTED), E_CLS);

	if (rs9113_info->conn_status == WNIC_CONNECTED) {
		/* if connected to a different ssid, just disconnect it */
		if (strcmp((const char *)ssid, (const char *)(rs9113_info->ssid.ssid)) == 0) {
			ercd = E_OK;
			goto error_exit;
		} else {
			if (rs9113_wnic_disconnect(rs9113_wnic) != E_OK) { /* if disconnect failed just return failed */
				ercd = E_SYS;
			}
		}
	} else {
		rs9113_wnic_disconnect(rs9113_wnic);
	}

	rs9113_info->busy_status = WNIC_IS_BUSY;

	rs9113_info->conn_status = WNIC_DURING_CONNECTION;
	memcpy(rs9113_info->ssid.ssid, ssid, ssid_len);
	rs9113_info->ssid.ssid_len = ssid_len;

	rsi_security_mode_t rsi_security_type;
	void *rsi_secret_key;

	switch (security) {
		case AUTH_SECURITY_OPEN:
			rsi_security_type = RSI_OPEN;
			rsi_secret_key = NULL;
			break;

		// case AUTH_SECURITY_WEP_40:
		// case AUTH_SECURITY_WEP_104:
		// 	rsi_security_type = RSI_WEP;
		// 	rsi_secret_key = &wep;
		// 	//wep.key
		// 	break;

		case AUTH_SECURITY_WPA_WITH_PASS_PHRASE:
			rsi_security_type = RSI_WPA;
			rsi_secret_key = (void *)
			                 key->key;//PSK string terminated with NULL. Length of PSK should be greater than equal to 8 and less than 64 bytes.
			break;

		case AUTH_SECURITY_WPA2_WITH_PASS_PHRASE:
			rsi_security_type = RSI_WPA2;
			rsi_secret_key = (void *)
			                 key->key;//PSK string terminated with NULL. Length of PSK should be greater than equal to 8 and less than 64 bytes.
			break;

		case AUTH_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
			rsi_security_type = RSI_WPA_WPA2_MIXED;
			rsi_secret_key = (void *)
			                 key->key;//PSK string terminated with NULL. Length of PSK should be greater than equal to 8 and less than 64 bytes.
			break;

		case AUTH_SECURITY_WPA_WITH_KEY:
			rsi_security_type = RSI_WPA_PMK;
			rsi_secret_key = (void *)key->key;//PMK string, should be 32 bytes in length.
			break;

		case AUTH_SECURITY_WPA2_WITH_KEY:
			rsi_security_type = RSI_WPA2_PMK;
			rsi_secret_key = (void *)key->key;//PMK string, should be 32 bytes in length.
			break;

		case AUTH_SECURITY_WPS_PUSH_BUTTON:
			rsi_security_type = RSI_WPS_PUSH_BUTTON;
			rsi_secret_key = NULL;//NULL string indicate to generate push button event
			break;

		case AUTH_SECURITY_WPS_PIN:
			rsi_security_type = RSI_WPS_PIN;
			rsi_secret_key = (void *)key->key;//8 bytes WPS PIN
			break;

		case AUTH_SECURITY_WPA_AUTO_WITH_KEY:

		//unsupported, need specify WPA/WPA2 PMK
		default:
			dbg_printf(DBG_LESS_INFO, "RS9113 does not support security type %d \r\n", security);
			rs9113_info->conn_status = WNIC_NOT_CONNECTED;
			rs9113_info->busy_status = WNIC_IS_FREE;
			ercd = E_PAR;
			goto error_exit;
			break;
	}

	ercd = rsi_wlan_connect((int8_t *)ssid, rsi_security_type, rsi_secret_key);
	dbg_printf(DBG_MORE_INFO, "rsi_wlan_connect return 0x%x when connect ssid %s with security type %d\r\n", ercd, ssid,
	           rsi_security_type);
	rs9113_info->busy_status = WNIC_IS_FREE;

	if (ercd != E_OK) {
		rs9113_info->conn_status = WNIC_NOT_CONNECTED;
		dbg_printf(DBG_LESS_INFO, "RS9113 wlan connect failed (return 0x%x )\r\n", ercd);
		return ercd;
	}

	//register rs9113 on receive callback function
	rsi_wlan_register_callbacks(RSI_WLAN_DATA_RECEIVE_NOTIFY_CB, (void *)rs9113_info->ctrl);
	rs9113_info->mac_status = WNIC_MAC_NOT_UPDATED;
	rs9113_info->conn_status = WNIC_CONNECTED;
	rs9113_on_ops = &(rs9113_wnic->wnic_on_ops);

	if (rs9113_on_ops->on_init_success != NULL) {//update MAC address again
		rs9113_on_ops->on_init_success(rs9113_wnic);
	}

	if (rs9113_on_ops->on_connected != NULL) {
		rs9113_on_ops->on_connected(rs9113_wnic);
	}

error_exit:
	return ercd;
}

int32_t rs9113_poll_conn_status(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;
	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	if ((rs9113_info->busy_status == WNIC_IS_BUSY) || (rs9113_info->conn_status == WNIC_CONNECTED)){
		ercd = rs9113_info->conn_status;
		goto error_exit;
	}

	ercd = rsi_wlan_get_status();
	dbg_printf(DBG_MORE_INFO, "rsi_wlan_get_status ret %d\r\n", ercd);

error_exit:
	return ercd;
}

int32_t rs9113_wnic_disconnect(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;
	DEV_WNIC_ON_OPS *rs9113_on_ops;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	RS9113_CHECK_EXP((rs9113_info->conn_status!=WNIC_DURING_DISCONNECTION), E_CLS);

	if (rs9113_poll_conn_status(rs9113_wnic) == WNIC_CONNECTED) {
		rs9113_info->conn_status = WNIC_DURING_DISCONNECTION;
		ercd = rsi_wlan_disconnect();
		dbg_printf(DBG_MORE_INFO, "rsi_wlan_disconnect ret %d\r\n", ercd);
		rs9113_info->conn_status = WNIC_NOT_CONNECTED;
		rs9113_on_ops = &(rs9113_wnic->wnic_on_ops);

		if (rs9113_on_ops->on_disconnected != NULL) {
			rs9113_on_ops->on_disconnected(rs9113_wnic);
		}
	}

error_exit:
	return ercd;
}

int32_t rs9113_prepare_tx(DEV_WNIC_PTR rs9113_wnic, uint32_t tx_len)
{
	int32_t ercd = E_OK;
	rs9113_pbuf.buf = malloc(tx_len);

	if (rs9113_pbuf.buf != NULL) {
		rs9113_pbuf.len = tx_len;
	} else {
		rs9113_pbuf.len = 0;
		ercd = E_NOMEM;
	}

	rs9113_pbuf.cur_index = 0;

error_exit:
	dbg_printf(DBG_MORE_INFO, "rs9113_prepare_tx ret %d\r\n", ercd);
	return ercd;
}

int32_t rs9113_add_tx_data(DEV_WNIC_PTR rs9113_wnic, uint8_t *p_buf, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	RS9113_CHECK_EXP(p_buf!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	if (rs9113_pbuf.cur_index + len > rs9113_pbuf.len) {
		len = rs9113_pbuf.len - rs9113_pbuf.cur_index;
	}

	uint8_t *dest = rs9113_pbuf.buf + rs9113_pbuf.cur_index;
	memcpy(dest, p_buf, len);
	rs9113_pbuf.cur_index += len;
	ercd = len;

error_exit:
	dbg_printf(DBG_MORE_INFO, "rs9113_add_tx_data ret %d\r\n", ercd);
	return ercd;
}

int32_t rs9113_commit_tx(DEV_WNIC_PTR rs9113_wnic, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	RS9113_CHECK_EXP(rs9113_pbuf.buf!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	ercd = rsi_wlan_send_data(rs9113_pbuf.buf, len);
	dbg_printf(DBG_MORE_INFO, "rsi_wlan_send_data ret %d\r\n", ercd);
	RS9113_CHECK_EXP(ercd == E_OK, ercd);
	ercd = len;

error_exit:
	free(rs9113_pbuf.buf);

	if (rs9113_pbuf.buf != NULL) {
		rs9113_pbuf.buf = NULL;
	}

	return ercd;
}

int32_t rs9113_prepare_rx(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;

error_exit:
	return ercd;
}

int32_t rs9113_get_rx_data(DEV_WNIC_PTR rs9113_wnic, uint8_t *p_buf, uint32_t len)
{
	int32_t ercd = E_OK;

error_exit:
	return ercd;
}

int32_t rs9113_accept_rx(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;

error_exit:
	return ercd;
}

int32_t rs9113_config_power_mode(DEV_WNIC_PTR rs9113_wnic, int32_t power_mode)
{
	int32_t ercd = E_OK;

error_exit:
	return ercd;
}

int32_t rs9113_poll_power_mode(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;

error_exit:
	return ercd;
}

int32_t rs9113_wnic_reset(DEV_WNIC_PTR rs9113_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *rs9113_info;

	RS9113_CHECK_EXP(rs9113_wnic!=NULL, E_OBJ);
	rs9113_info = &(rs9113_wnic->wnic_info);
	RS9113_CHECK_EXP((rs9113_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	ercd = rsi_wireless_deinit();
	RS9113_CHECK_EXP(ercd == E_OK, ercd);
	ercd = rsi_wireless_init(0, 0);

error_exit:
	return ercd;
}

void rs9113_period_process(DEV_WNIC_PTR rs9113_wnic, void *ptr)
{
	return;
}
