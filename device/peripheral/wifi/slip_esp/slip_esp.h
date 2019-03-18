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

#ifndef _SLIP_ESP_H_
#define _SLIP_ESP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dev_wnic.h"

#define SLIP_SCAN_TIME_MS		(4000)	/* 3000 ms for scan */
#define SLIP_ESP_SCAN_AP_NUM		(10)

#define SLIP_INIT_TIME_MS		(3000)  /* 3000 ms for init time out */
#define SLIP_ESP_INIT_RETRY		(3)     /* retry times for init */
#define SLIP_CON_TIME_MS		(10000) /* 10000 ms for connection time out */

#define SLIP_ESP_SSID_NAME_LENGTH_MAX	(32)

#define SLIP_CONSOLE_BUF_LEN		(1024)

typedef enum {
	SLIP_ESP_MODE_STATION = 0,
	SLIP_ESP_MODE_SOFTAP = 1,
} SLIP_ESP_MODE_DEF;

typedef struct _slip_esp_ap_info {
	char        ssid[SLIP_ESP_SSID_NAME_LENGTH_MAX];
	uint8_t     bssid[8]; 	/* 6byte + 2byte PAD. */
	int32_t     rssi;      	/* Receive Signal Strength Indication in dBm. */
	uint32_t    max_data_rate; /* Maximum data rate in kilobits/s */
	uint32_t    security;      /* Security type  */
	uint32_t    channel;       /* Radio channel that the AP beacon was received on   */
} SLIP_ESP_AP_INFO_DEF, * SLIP_ESP_AP_INFO_DEF_PTR;

typedef struct _slip_esp_t {

	uint32_t snd_cnt;
	uint32_t rcv_cnt;

	uint32_t scan_timer;
	uint32_t scan_cnt;
	SLIP_ESP_AP_INFO_DEF scanned_ap[SLIP_ESP_SCAN_AP_NUM];

	uint32_t init_timer;	/* init timer */
	uint32_t init_try;
	uint32_t conn_timer;	/* con timer */
	char 	console_buf[SLIP_CONSOLE_BUF_LEN];
} SLIP_ESP_DEF, * SLIP_ESP_DEF_PTR;

extern int32_t slip_esp_wnic_get_info(DEV_WNIC_PTR slip_esp_wnic, uint32_t cmd, void *rinfo);
extern int32_t slip_esp_wnic_control(DEV_WNIC_PTR slip_esp_wnic, uint32_t ctrl_cmd, void *param);
extern int32_t slip_esp_wnic_init(DEV_WNIC_PTR slip_esp_wnic, uint32_t network_type);
extern int32_t slip_esp_poll_init_status(DEV_WNIC_PTR slip_esp_wnic);
extern int32_t slip_esp_poll_busy_status(DEV_WNIC_PTR slip_esp_wnic);
extern int32_t slip_esp_set_network_type(DEV_WNIC_PTR slip_esp_wnic, uint32_t type);
extern int32_t slip_esp_set_macaddr(DEV_WNIC_PTR slip_esp_wnic, uint8_t *mac);
extern int32_t slip_esp_get_macaddr(DEV_WNIC_PTR slip_esp_wnic, uint8_t *mac);
extern int32_t slip_esp_start_scan(DEV_WNIC_PTR slip_esp_wnic);
extern int32_t slip_esp_stop_scan(DEV_WNIC_PTR slip_esp_wnic);
extern int32_t slip_esp_poll_scan_status(DEV_WNIC_PTR slip_esp_wnic);
extern int32_t slip_esp_get_scan_result_cnt(DEV_WNIC_PTR slip_esp_wnic);
extern int32_t slip_esp_get_scan_result(DEV_WNIC_PTR slip_esp_wnic, uint32_t index, WNIC_SCAN_RESULT *result);
extern int32_t slip_esp_wnic_connect(DEV_WNIC_PTR slip_esp_wnic, uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key);
extern int32_t slip_esp_poll_conn_status(DEV_WNIC_PTR slip_esp_wnic);
extern int32_t slip_esp_wnic_disconnect(DEV_WNIC_PTR slip_esp_wnic);
extern int32_t slip_esp_prepare_tx(DEV_WNIC_PTR slip_esp_wnic, uint32_t tx_len);
extern int32_t slip_esp_add_tx_data(DEV_WNIC_PTR slip_esp_wnic, uint8_t *p_buf, uint32_t len);
extern int32_t slip_esp_commit_tx(DEV_WNIC_PTR slip_esp_wnic, uint32_t len);
extern int32_t slip_esp_prepare_rx(DEV_WNIC_PTR slip_esp_wnic);
extern int32_t slip_esp_get_rx_data(DEV_WNIC_PTR slip_esp_wnic, uint8_t *p_buf, uint32_t len);
extern int32_t slip_esp_accept_rx(DEV_WNIC_PTR slip_esp_wnic);
extern int32_t slip_esp_config_power_mode(DEV_WNIC_PTR slip_esp_wnic, int32_t power_mode);
extern int32_t slip_esp_poll_power_mode(DEV_WNIC_PTR slip_esp_wnic);
extern int32_t slip_esp_wnic_reset(DEV_WNIC_PTR slip_esp_wnic);
extern void slip_esp_period_process(DEV_WNIC_PTR slip_esp_wnic, void *ptr);
//extern void slip_esp_isr(DEV_WNIC_PTR slip_esp_wnic);

#ifdef __cplusplus
}
#endif

#endif /* _SLIP_ESP_H_ */
