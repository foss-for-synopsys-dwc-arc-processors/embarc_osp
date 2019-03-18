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
--------------------------------------------- */

#ifndef _RW009_H_
#define _RW009_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dev_wnic.h"

#define RW009_SCAN_TIME_MS		(3000)	/* 3000 ms for scan */
#define RW009_SCAN_AP_NUM		(5)

#define RW009_INIT_TIME_MS		(3000)  /* 3000 ms for init time out */
#define RW009_INIT_RETRY		(3)     /* retry times for init */
#define RW009_CON_TIME_MS		(10000) /* 10000 ms for connection time out */

#define RW009_SPI_MAX_DATA_LEN		(1520) /* one ethernet frame */

#define RW009_SSID_NAME_LENGTH_MAX	(32)

struct rw009_spi_data_packet {
	uint32_t len;
	uint32_t type;
	char buffer[RW009_SPI_MAX_DATA_LEN];
};

typedef enum {
	RW009_MODE_STATION = 0,
	RW009_MODE_SOFTAP = 1,
} RW009_MODE_DEF;

typedef struct _rw009_ap_info {
	char        ssid[RW009_SSID_NAME_LENGTH_MAX];
	uint8_t     bssid[8]; 	/* 6byte + 2byte PAD. */
	int32_t     rssi;      	/* Receive Signal Strength Indication in dBm. */
	uint32_t    max_data_rate; /* Maximum data rate in kilobits/s */
	uint32_t    security;      /* Security type  */
	uint32_t    channel;       /* Radio channel that the AP beacon was received on   */
} RW009_AP_INFO_DEF, * RW009_AP_INFO_DEF_PTR;

typedef struct _rw009_t {
	uint8_t spi_master;
	uint8_t spi_cs;
	uint32_t spi_freq;

	uint8_t gpio_int_busy;
	uint8_t pin_int_busy;
	DEV_GPIO_HANDLER gpio_int_handler;

	uint8_t gpio_rst;
	uint8_t pin_rst;

	uint8_t gpio_cs;
	uint8_t pin_cs;

	struct rw009_spi_data_packet snd_packet;
	uint32_t snd_cnt;
	struct rw009_spi_data_packet rcv_packet;
	uint32_t rcv_cnt;

	uint32_t last_cmd;
	uint32_t rw009_to_send; /* flag of rw009 has data to send */

	uint32_t scan_timer;
	uint32_t scan_cnt;
	RW009_AP_INFO_DEF scanned_ap[RW009_SCAN_AP_NUM];

	uint32_t init_timer;	/* init timer */
	uint32_t init_try;
	uint32_t conn_timer;	/* con timer */
} RW009_DEF, * RW009_DEF_PTR;

extern int32_t rw009_wnic_get_info(DEV_WNIC_PTR rw009_wnic, uint32_t cmd, void *rinfo);
extern int32_t rw009_wnic_control(DEV_WNIC_PTR rw009_wnic, uint32_t ctrl_cmd, void *param);
extern int32_t rw009_wnic_init(DEV_WNIC_PTR rw009_wnic, uint32_t network_type);
extern int32_t rw009_poll_init_status(DEV_WNIC_PTR rw009_wnic);
extern int32_t rw009_poll_busy_status(DEV_WNIC_PTR rw009_wnic);
extern int32_t rw009_set_network_type(DEV_WNIC_PTR rw009_wnic, uint32_t type);
extern int32_t rw009_set_macaddr(DEV_WNIC_PTR rw009_wnic, uint8_t *mac);
extern int32_t rw009_get_macaddr(DEV_WNIC_PTR rw009_wnic, uint8_t *mac);
extern int32_t rw009_start_scan(DEV_WNIC_PTR rw009_wnic);
extern int32_t rw009_stop_scan(DEV_WNIC_PTR rw009_wnic);
extern int32_t rw009_poll_scan_status(DEV_WNIC_PTR rw009_wnic);
extern int32_t rw009_get_scan_result_cnt(DEV_WNIC_PTR rw009_wnic);
extern int32_t rw009_get_scan_result(DEV_WNIC_PTR rw009_wnic, uint32_t index, WNIC_SCAN_RESULT *result);
extern int32_t rw009_wnic_connect(DEV_WNIC_PTR rw009_wnic, uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key);
extern int32_t rw009_poll_conn_status(DEV_WNIC_PTR rw009_wnic);
extern int32_t rw009_wnic_disconnect(DEV_WNIC_PTR rw009_wnic);
extern int32_t rw009_prepare_tx(DEV_WNIC_PTR rw009_wnic, uint32_t tx_len);
extern int32_t rw009_add_tx_data(DEV_WNIC_PTR rw009_wnic, uint8_t *p_buf, uint32_t len);
extern int32_t rw009_commit_tx(DEV_WNIC_PTR rw009_wnic, uint32_t len);
extern int32_t rw009_prepare_rx(DEV_WNIC_PTR rw009_wnic);
extern int32_t rw009_get_rx_data(DEV_WNIC_PTR rw009_wnic, uint8_t *p_buf, uint32_t len);
extern int32_t rw009_accept_rx(DEV_WNIC_PTR rw009_wnic);
extern int32_t rw009_config_power_mode(DEV_WNIC_PTR rw009_wnic, int32_t power_mode);
extern int32_t rw009_poll_power_mode(DEV_WNIC_PTR rw009_wnic);
extern int32_t rw009_wnic_reset(DEV_WNIC_PTR rw009_wnic);
extern void rw009_period_process(DEV_WNIC_PTR rw009_wnic, void *ptr);
extern void rw009_isr(DEV_WNIC_PTR rw009_wnic);

#ifdef __cplusplus
}
#endif

#endif /* _RW009_H_ */
