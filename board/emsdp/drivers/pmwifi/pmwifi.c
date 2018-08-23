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
#if defined(MID_LWIP) && defined(MID_LWIP_CONTRIB)
#include <stddef.h>
#include <string.h>

#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "arc_exception.h"

#include "board.h"

#ifdef ENABLE_OS
#include "os_hal_inc.h"
#endif

#include "pmwifi.h"

// #define PMWIFI_CHECK_EXP(EXPR, ERROR_CODE)		CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

#if (USE_EMSDP_RSWIFI_0)
/** PMOD WIFI SPI FREQ & CLK MODE SETTINGS */
// #define EMSDP_RSWIFI_0_SPI_FREQ		BOARD_SPI_FREQ

// #define EMSDP_RSWIFI_0_SPI_ID		DW_SPI_0_ID


/*********************WIFI_RS9113*********************/
#if defined(WIFI_RS9113)
#include "rs9113_adapter.h"


/* WF OPERATIONS */
#define EMSDP_RSWIFI_0_TIMER_ID		TIMER_0

#define EMSDP_RSWIFI_0_SPI_CPULOCK_ENABLE

/** install wnic 0 to system */
#define RSWIFI_0_NAME		"RS9113"
#define RSWIFI_0_IFNAME0	'r'
#define RSWIFI_0_IFNAME1	's'
static DEV_WNIC rswifi_0_wnic;
const uint8_t  rswifi_0_mac_addr[WNIC_HDR_LEN] = {
	EMSDP_RSWIFI_0_MAC_ADDR0,
	EMSDP_RSWIFI_0_MAC_ADDR1,
	EMSDP_RSWIFI_0_MAC_ADDR2,
	EMSDP_RSWIFI_0_MAC_ADDR3,
	EMSDP_RSWIFI_0_MAC_ADDR4,
	EMSDP_RSWIFI_0_MAC_ADDR5
};


static int32_t rswifi_0_wnic_get_info(uint32_t cmd, void *rinfo)
{
	return rs9113_wnic_get_info(&rswifi_0_wnic, cmd, rinfo);
}

static int32_t rswifi_0_wnic_control(uint32_t ctrl_cmd, void *param)
{
	return rs9113_wnic_control(&rswifi_0_wnic, ctrl_cmd, param);
}

static int32_t rswifi_0_wnic_init(uint32_t network_type)
{
	return rs9113_wnic_init(&rswifi_0_wnic, network_type);
}

static int32_t rswifi_0_poll_init_status(void)
{
	return rs9113_poll_init_status(&rswifi_0_wnic);
}

static int32_t rswifi_0_poll_busy_status(void)
{
	return rs9113_poll_busy_status(&rswifi_0_wnic);
}

static int32_t rswifi_0_set_network_type(uint32_t type)
{
	return rs9113_set_network_type(&rswifi_0_wnic, type);
}

static int32_t rswifi_0_set_macaddr(uint8_t *mac)
{
	return rs9113_set_macaddr(&rswifi_0_wnic, mac);
}

static int32_t rswifi_0_get_macaddr(uint8_t *mac)
{
	return rs9113_get_macaddr(&rswifi_0_wnic, mac);
}

static int32_t rswifi_0_start_scan(void)
{
	return rs9113_start_scan(&rswifi_0_wnic);
}

static int32_t rswifi_0_stop_scan(void)
{
	return rs9113_stop_scan(&rswifi_0_wnic);
}

static int32_t rswifi_0_poll_scan_status(void)
{
	return rs9113_poll_scan_status(&rswifi_0_wnic);
}

static int32_t rswifi_0_get_scan_result_cnt(void)
{
	return rs9113_get_scan_result_cnt(&rswifi_0_wnic);
}

static int32_t rswifi_0_get_scan_result(uint32_t index, WNIC_SCAN_RESULT *result)
{
	return rs9113_get_scan_result(&rswifi_0_wnic, index, result);
}

static int32_t rswifi_0_wnic_connect(uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key)
{
	return rs9113_wnic_connect(&rswifi_0_wnic, security, ssid, key);
}

static int32_t rswifi_0_poll_conn_status(void)
{
	return rs9113_poll_conn_status(&rswifi_0_wnic);
}

static int32_t rswifi_0_wnic_disconnect(void)
{
	return rs9113_wnic_disconnect(&rswifi_0_wnic);
}

static int32_t rswifi_0_prepare_tx(uint32_t tx_len)
{
	return rs9113_prepare_tx(&rswifi_0_wnic, tx_len);
}

static int32_t rswifi_0_add_tx_data(uint8_t *p_buf, uint32_t len)
{
	return rs9113_add_tx_data(&rswifi_0_wnic, p_buf, len);
}

static int32_t rswifi_0_commit_tx(uint32_t len)
{
	return rs9113_commit_tx(&rswifi_0_wnic, len);
}

static int32_t rswifi_0_prepare_rx(void)
{
	return rs9113_prepare_rx(&rswifi_0_wnic);
}

static int32_t rswifi_0_get_rx_data(uint8_t *p_buf, uint32_t len)
{
	return rs9113_get_rx_data(&rswifi_0_wnic, p_buf, len);
}

static int32_t rswifi_0_accept_rx(void)
{
	return rs9113_accept_rx(&rswifi_0_wnic);
}

static int32_t rswifi_0_config_power_mode(int32_t power_mode)
{
	return rs9113_config_power_mode(&rswifi_0_wnic, power_mode);
}

static int32_t rswifi_0_poll_power_mode(void)
{
	return rs9113_poll_power_mode(&rswifi_0_wnic);
}

static int32_t rswifi_0_wnic_reset(void)
{
	return rs9113_wnic_reset(&rswifi_0_wnic);
}

static void rswifi_0_period_process(void *ptr)
{
	rs9113_period_process(&rswifi_0_wnic, ptr);
}

static void rswifi_0_install(void)
{
	DEV_WNIC *rswifi_wnic_ptr = &rswifi_0_wnic;
	DEV_WNIC_INFO *rswifi_wnic_info_ptr = &(rswifi_0_wnic.wnic_info);
	DEV_WNIC_ON_OPS *rswifi_wnic_on_ops_ptr = &(rswifi_0_wnic.wnic_on_ops);

	memcpy((void *)rswifi_wnic_info_ptr->name, (void *)RSWIFI_0_NAME, strlen(RSWIFI_0_NAME));
	rswifi_wnic_info_ptr->ifname[0] = RSWIFI_0_IFNAME0;
	rswifi_wnic_info_ptr->ifname[1] = RSWIFI_0_IFNAME1;
	memset((void *)&rswifi_wnic_info_ptr->ssid, 0, sizeof(WNIC_CFG_SSID));
	rswifi_wnic_info_ptr->ssid.ssid[0] = '\0';

	rswifi_wnic_info_ptr->ctrl = NULL;
	rswifi_wnic_info_ptr->extra = NULL;
	rswifi_wnic_info_ptr->opn_flg = DEV_CLOSED;
	rswifi_wnic_info_ptr->err_flg = DEV_GOOD;
	rswifi_wnic_info_ptr->network_type = WNIC_NETWORK_TYPE_INFRASTRUCTURE;
	rswifi_wnic_info_ptr->init_status = WNIC_NOT_INITIALIZED;
	rswifi_wnic_info_ptr->conn_status = WNIC_NOT_CONNECTED;
	rswifi_wnic_info_ptr->conn_type = WNIC_CONN_NONE;
	rswifi_wnic_info_ptr->mac_status = WNIC_MAC_NOT_UPDATED;
	rswifi_wnic_info_ptr->scan_status = WNIC_NOT_SCANED;
	rswifi_wnic_info_ptr->scan_results = 0;
	memcpy((void *)rswifi_wnic_info_ptr->mac_addr, (void *)rswifi_0_mac_addr, WNIC_HDR_LEN);
	rswifi_wnic_info_ptr->busy_status = WNIC_IS_FREE;
	rswifi_wnic_info_ptr->tx_frame_cnt = 0;
	rswifi_wnic_info_ptr->tx_speed = 0;
	rswifi_wnic_info_ptr->tx_pending = 0;
	rswifi_wnic_info_ptr->rx_frame_cnt = 0;
	rswifi_wnic_info_ptr->rx_speed = 0;
	rswifi_wnic_info_ptr->rx_pending = 0;
	rswifi_wnic_info_ptr->power_status = WNIC_POWER_OFF;

	rswifi_wnic_on_ops_ptr->on_init_success	= NULL;
	rswifi_wnic_on_ops_ptr->on_init_fail	= NULL;
	rswifi_wnic_on_ops_ptr->on_connected 	= NULL;
	rswifi_wnic_on_ops_ptr->on_disconnected	= NULL;
	rswifi_wnic_on_ops_ptr->on_mac_updated 	= NULL;
	rswifi_wnic_on_ops_ptr->on_scan_finished= NULL;
	rswifi_wnic_on_ops_ptr->on_rxdata_comes	= NULL;
	rswifi_wnic_on_ops_ptr->on_dev_asserted	= NULL;


	rswifi_wnic_ptr->wnic_get_info 			= rswifi_0_wnic_get_info	;
	rswifi_wnic_ptr->wnic_control			= rswifi_0_wnic_control		;
	rswifi_wnic_ptr->wnic_init				= rswifi_0_wnic_init		;
	rswifi_wnic_ptr->poll_init_status		= rswifi_0_poll_init_status	;
	rswifi_wnic_ptr->poll_busy_status		= rswifi_0_poll_busy_status	;
	rswifi_wnic_ptr->set_network_type		= rswifi_0_set_network_type	;
	rswifi_wnic_ptr->set_macaddr			= rswifi_0_set_macaddr		;
	rswifi_wnic_ptr->get_macaddr			= rswifi_0_get_macaddr		;
	rswifi_wnic_ptr->start_scan				= rswifi_0_start_scan		;
	rswifi_wnic_ptr->stop_scan				= rswifi_0_stop_scan		;
	rswifi_wnic_ptr->poll_scan_status		= rswifi_0_poll_scan_status	;
	rswifi_wnic_ptr->get_scan_result_cnt	= rswifi_0_get_scan_result_cnt;
	rswifi_wnic_ptr->get_scan_result 		= rswifi_0_get_scan_result	;
	rswifi_wnic_ptr->wnic_connect			= rswifi_0_wnic_connect		;
	rswifi_wnic_ptr->poll_conn_status		= rswifi_0_poll_conn_status	;
	rswifi_wnic_ptr->wnic_disconnect		= rswifi_0_wnic_disconnect	;
	rswifi_wnic_ptr->prepare_tx				= rswifi_0_prepare_tx		;
	rswifi_wnic_ptr->add_tx_data			= rswifi_0_add_tx_data		;
	rswifi_wnic_ptr->commit_tx				= rswifi_0_commit_tx		;
	rswifi_wnic_ptr->prepare_rx				= rswifi_0_prepare_rx		;
	rswifi_wnic_ptr->get_rx_data			= rswifi_0_get_rx_data		;
	rswifi_wnic_ptr->accept_rx				= rswifi_0_accept_rx		;
	rswifi_wnic_ptr->config_power_mode		= rswifi_0_config_power_mode;
	rswifi_wnic_ptr->poll_power_mode		= rswifi_0_poll_power_mode	;
	rswifi_wnic_ptr->wnic_reset				= rswifi_0_wnic_reset		;
	rswifi_wnic_ptr->period_process			= rswifi_0_period_process	;
}

#else /* WIFI_RSI */
#error "no wifi module selected"
#endif

#endif /* USE_EMSDP_RSWIFI_0 */

/** get one wnic device structure */
DEV_WNIC_PTR wnic_get_dev(int32_t wnic_id)
{
	static uint32_t install_flag = 0;

	/* install device objects */
	if (install_flag == 0) {
		install_flag = 1;
		rswifi_all_install();
	}

	switch (wnic_id) {
#if (USE_EMSDP_RSWIFI_0)
		case EMSDP_RSWIFI_0_ID:
			return &rswifi_0_wnic;
			break;
#endif
		default:
			break;
	}
	return NULL;
}

/**
 * \brief  	install all wnic objects
 * \note 	\b MUST be called during system init
 */
void rswifi_all_install(void)
{
#if (USE_EMSDP_RSWIFI_0)
	rswifi_0_install();
#endif
}

#endif  /* MID_LWIP && MID_LWIP_CONTRIB */
