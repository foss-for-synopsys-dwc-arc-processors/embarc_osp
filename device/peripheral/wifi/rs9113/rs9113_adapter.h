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
#ifndef _RS9113_ADAPTER_H_
#define	_RS9113_ADAPTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dev_wnic.h"

#define WLAN_RSI		//If defined, Select RSI as WLAN module

#ifdef WLAN_RSI

#define RSI_91X			//This option enabes support for RSI 1x1 devices.
#define RSI_DEBUGFS		//This option creates debugfs entries
#define RSI_SDIO		//This option enables the SDIO bus support in rsi drivers.
#ifndef RSI_SDIO
#define RSI_USB			//This option enables the USB bus support in rsi drivers.
#endif
#define RSI_COEX		//This option enables the WLAN BT coex support in rsi drivers

#endif /* WLAN_RSI */

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY   5

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE  500

typedef struct MACADDR_T
{
	union
	{
		uint8_t		u8[6];
		uint16_t	u16[3];
	};
} MACADDR;

#define MACBROADCAST	((MACADDR) {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF})	// RFC 894
#define MACNONE			((MACADDR) {0x00, 0x00, 0x00, 0x00, 0x00, 0x00})	// RFC 894

// WPA and WPA2 KEY, used for both
typedef struct
{
	uint8_t rgbKey[32];
} WPA2KEY;

typedef struct
{
	union
	{
		uint8_t		rgbKey[5];
		int8_t		asciiKey[5];
	} key[4];
} WEP40KEY;

typedef struct
{
	union
	{
		uint8_t		rgbKey[13];
		int8_t		asciiKey[13];
	} key[4];
} WEP104KEY;

extern int32_t rs9113_wnic_get_info(DEV_WNIC *rs9113_wnic, uint32_t cmd, void *rinfo);
extern int32_t rs9113_wnic_control(DEV_WNIC *rs9113_wnic, uint32_t ctrl_cmd, void *param);
extern int32_t rs9113_wnic_init(DEV_WNIC *rs9113_wnic, uint32_t network_type);
extern int32_t rs9113_poll_init_status(DEV_WNIC *rs9113_wnic);
extern int32_t rs9113_poll_busy_status(DEV_WNIC *rs9113_wnic);
extern int32_t rs9113_set_network_type(DEV_WNIC *rs9113_wnic, uint32_t type);
extern int32_t rs9113_set_macaddr(DEV_WNIC *rs9113_wnic, uint8_t *mac);
extern int32_t rs9113_get_macaddr(DEV_WNIC *rs9113_wnic, uint8_t *mac);
extern int32_t rs9113_start_scan(DEV_WNIC *rs9113_wnic);
extern int32_t rs9113_stop_scan(DEV_WNIC *rs9113_wnic);
extern int32_t rs9113_poll_scan_status(DEV_WNIC *rs9113_wnic);
extern int32_t rs9113_get_scan_result_cnt(DEV_WNIC *rs9113_wnic);
extern int32_t rs9113_get_scan_result(DEV_WNIC *rs9113_wnic, uint32_t index, WNIC_SCAN_RESULT *result);
extern int32_t rs9113_wnic_connect(DEV_WNIC *rs9113_wnic, uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key);
extern int32_t rs9113_poll_conn_status(DEV_WNIC *rs9113_wnic);
extern int32_t rs9113_wnic_disconnect(DEV_WNIC *rs9113_wnic);
extern int32_t rs9113_prepare_tx(DEV_WNIC *rs9113_wnic, uint32_t tx_len);
extern int32_t rs9113_add_tx_data(DEV_WNIC *rs9113_wnic, uint8_t *p_buf, uint32_t len);
extern int32_t rs9113_commit_tx(DEV_WNIC *rs9113_wnic, uint32_t len);
extern int32_t rs9113_prepare_rx(DEV_WNIC *rs9113_wnic);
extern int32_t rs9113_get_rx_data(DEV_WNIC *rs9113_wnic, uint8_t *p_buf, uint32_t len);
extern int32_t rs9113_accept_rx(DEV_WNIC *rs9113_wnic);
extern int32_t rs9113_config_power_mode(DEV_WNIC *rs9113_wnic, int32_t power_mode);
extern int32_t rs9113_poll_power_mode(DEV_WNIC *rs9113_wnic);
extern int32_t rs9113_wnic_reset(DEV_WNIC *rs9113_wnic);
extern void rs9113_period_process(DEV_WNIC *rs9113_wnic, void *ptr);

#ifdef __cplusplus
}
#endif

#endif	/* _RS9113_ADAPTER_H_ */