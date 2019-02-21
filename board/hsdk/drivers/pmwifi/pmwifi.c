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
#if defined(MID_LWIP) && defined(MID_LWIP_CONTRIB)
#include <stddef.h>
#include <string.h>

#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "arc_exception.h"

#include "hsdk/hsdk.h"

#ifdef ENABLE_OS
#include "os_hal_inc.h"
#endif

#include "pmwifi.h"

#define PMWIFI_CHECK_EXP(EXPR, ERROR_CODE)		CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

#if (USE_HSDK_PMWIFI_0)
/** PMOD WIFI SPI FREQ & CLK MODE SETTINGS */
#define HSDK_PMWIFI_0_SPI_FREQ		BOARD_SPI_FREQ

/* WF OPERATIONS */
#define HSDK_PMWIFI_0_SPI_ID		BOARD_WIFI_SPI_ID

#define HSDK_PMWIFI_0_SPI_LINE		BOARD_WIFI_SPI_LINE
#define HSDK_PMWIFI_0_GPIO_ID		(DEV_GPIO_PORT_A)

#define HSDK_PMWIFI_0_INT_PIN		(DEV_GPIO_PIN_22)
#define HSDK_PMWIFI_0_RST_PIN		(DEV_GPIO_PIN_23)
#define HSDK_PMWIFI_0_WP_PIN		(DEV_GPIO_PIN_NC)
#define HSDK_PMWIFI_0_HIB_PIN		(DEV_GPIO_PIN_NC)

#define HSDK_PMWIFI_0_PIN_GAP		(HSDK_PMWIFI_0_INT_PIN-WF_GPIO_INT_PIN_OFS)

/*********************PMOD WIFI -> MRF24WG*********************/
#if defined(WIFI_MRF24G)
#include "wf_dev_hal.h"
#include "mrf24g_adapter.h"

#define HSDK_PMWIFI_0_TIMER_ID		TIMER_0

/** Test SPI Frequency to be 15MHz with both lock enable or disable */
//#define HSDK_PMWIFI_0_SPI_CPULOCK_ENABLE

/* gpio ops */
static int32_t pmwifi_0_gpio_open(uint32_t pin_mask, uint32_t pin_dir)
{
	int32_t ercd = E_OK;
	uint32_t mask;
	DEV_GPIO *pmwifi_gpio_ptr;

	pmwifi_gpio_ptr = gpio_get_dev(HSDK_PMWIFI_0_GPIO_ID);
	PMWIFI_CHECK_EXP(pmwifi_gpio_ptr != NULL, E_OBJ);

	pin_mask = ((pin_mask & WF_GPIO_PIN_MASK_ALL)<<HSDK_PMWIFI_0_PIN_GAP);
	mask = (pin_dir<<HSDK_PMWIFI_0_PIN_GAP) & pin_mask;
	ercd = pmwifi_gpio_ptr->gpio_open(mask);
	if (ercd == E_OPNED) {
		/* direction */
		pmwifi_gpio_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, (void *)mask);
		mask = (~(pin_dir<<HSDK_PMWIFI_0_PIN_GAP)) & pin_mask;
		pmwifi_gpio_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)mask);
	}
	/* Default all set to pull */
	ercd = pmwifi_gpio_ptr->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)pin_mask);

error_exit:
	return ercd;
}
static int32_t pmwifi_0_gpio_close(uint32_t pin_mask)
{
	static uint32_t close_mask = 0;
	int32_t ercd = E_OK;
	DEV_GPIO *pmwifi_gpio_ptr;

	pmwifi_gpio_ptr = gpio_get_dev(HSDK_PMWIFI_0_GPIO_ID);
	PMWIFI_CHECK_EXP(pmwifi_gpio_ptr != NULL, E_OBJ);

	close_mask |= pin_mask;
	close_mask &= WF_GPIO_PIN_MASK_ALL;

	if (close_mask == WF_GPIO_PIN_MASK_ALL) { /* only close it when all pin need to be closed */
		ercd = pmwifi_gpio_ptr->gpio_close();
		close_mask = 0;
	}

error_exit:
	return ercd;
}
static int32_t pmwifi_0_gpio_read(uint32_t pin_mask, uint32_t *value)
{
	int32_t ercd = E_OK;
	DEV_GPIO *pmwifi_gpio_ptr;

	pmwifi_gpio_ptr = gpio_get_dev(HSDK_PMWIFI_0_GPIO_ID);
	PMWIFI_CHECK_EXP(pmwifi_gpio_ptr != NULL, E_OBJ);

	pin_mask = ((pin_mask & WF_GPIO_PIN_MASK_ALL)<<HSDK_PMWIFI_0_PIN_GAP);
	ercd = pmwifi_gpio_ptr->gpio_read(value, pin_mask);
	*value = (*value) >> HSDK_PMWIFI_0_PIN_GAP;

error_exit:
	return ercd;
}
static int32_t pmwifi_0_gpio_write(uint32_t pin_mask, uint32_t value)
{
	int32_t ercd = E_OK;
	DEV_GPIO *pmwifi_gpio_ptr;

	pmwifi_gpio_ptr = gpio_get_dev(HSDK_PMWIFI_0_GPIO_ID);
	PMWIFI_CHECK_EXP(pmwifi_gpio_ptr != NULL, E_OBJ);

	pin_mask = ((pin_mask & WF_GPIO_PIN_MASK_ALL)<<HSDK_PMWIFI_0_PIN_GAP);
	value = value << HSDK_PMWIFI_0_PIN_GAP;
	ercd = pmwifi_gpio_ptr->gpio_write(value, pin_mask);

error_exit:
	return ercd;
}
static int32_t pmwifi_0_gpio_eint_install(DEV_GPIO_HANDLER gpio_handler)
{
	int32_t ercd = E_OK;
	DEV_GPIO *pmwifi_gpio_ptr;
	DEV_GPIO_BIT_ISR bit_isr;
	DEV_GPIO_INT_CFG int_cfg;

	pmwifi_gpio_ptr = gpio_get_dev(HSDK_PMWIFI_0_GPIO_ID);
	PMWIFI_CHECK_EXP(pmwifi_gpio_ptr != NULL, E_OBJ);

	int_cfg.int_bit_mask = (1<<HSDK_PMWIFI_0_INT_PIN);
	int_cfg.int_bit_type = GPIO_INT_BIT_LEVEL_TRIG(HSDK_PMWIFI_0_INT_PIN);
	int_cfg.int_bit_polarity = GPIO_INT_BIT_POL_ACT_LOW(HSDK_PMWIFI_0_INT_PIN);
	int_cfg.int_bit_debounce = GPIO_INT_BIT_DIS_DEBOUNCE(HSDK_PMWIFI_0_INT_PIN);
	ercd = pmwifi_gpio_ptr->gpio_control(GPIO_CMD_SET_BIT_INT_CFG, (void *)(&int_cfg));
	bit_isr.int_bit_ofs = HSDK_PMWIFI_0_INT_PIN;
	bit_isr.int_bit_handler = gpio_handler;
	ercd = pmwifi_gpio_ptr->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&bit_isr));

error_exit:
	return ercd;
}
static int32_t pmwifi_0_gpio_eint_is_enabled(void)
{
	int32_t ercd = E_OK;
	uint32_t mask;
	uint32_t mthd;
	DEV_GPIO *pmwifi_gpio_ptr;

	pmwifi_gpio_ptr = gpio_get_dev(HSDK_PMWIFI_0_GPIO_ID);
	PMWIFI_CHECK_EXP(pmwifi_gpio_ptr != NULL, E_OBJ);

	mask = (1 << HSDK_PMWIFI_0_INT_PIN);
	ercd = pmwifi_gpio_ptr->gpio_control(GPIO_CMD_GET_BIT_MTHD, (void *)(&mthd));
	if (ercd == E_OK) {
		mthd &= mask;
		ercd = ((mthd==0) ? WF_EINT_DISABLE:WF_EINT_ENABLE);
	}

error_exit:
	return ercd;
}
static int32_t pmwifi_0_gpio_eint_control(int32_t state)
{
	int32_t ercd = E_OK;
	uint32_t mask;
	DEV_GPIO *pmwifi_gpio_ptr;

	pmwifi_gpio_ptr = gpio_get_dev(HSDK_PMWIFI_0_GPIO_ID);
	PMWIFI_CHECK_EXP(pmwifi_gpio_ptr != NULL, E_OBJ);

	mask = (1 << HSDK_PMWIFI_0_INT_PIN);
	if (state == WF_EINT_ENABLE) {
		ercd = pmwifi_gpio_ptr->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)mask);
	} else {
		ercd = pmwifi_gpio_ptr->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)mask);
	}

error_exit:
	return ercd;
}
/* wifi gpio operation object */
static WF_GPIO_OPS pmwifi_0_gpio_ops = {
	pmwifi_0_gpio_open,
	pmwifi_0_gpio_close,
	pmwifi_0_gpio_read,
	pmwifi_0_gpio_write,
	pmwifi_0_gpio_eint_install,
	pmwifi_0_gpio_eint_is_enabled,
	pmwifi_0_gpio_eint_control
};
/* timer ops */
static int32_t pmwifi_0_timer_init(uint32_t freq)
{
	return E_OK;
}
static uint32_t pmwifi_0_timer_read_ms(void)
{
	return OSP_GET_CUR_MS();
}
static void pmwifi_0_timer_delay(uint32_t ms)
{
#ifdef OS_FREERTOS
	vTaskDelay(ms);
#else
	board_delay_ms(ms, OSP_DELAY_OS_COMPAT_ENABLE);
#endif
}

/* wifi timer operations object */
static WF_TIMER_OPS pmwifi_0_timer_ops = {
	pmwifi_0_timer_init,
	pmwifi_0_timer_read_ms,
	pmwifi_0_timer_delay
};
/* spi ops */
static int32_t pmwifi_0_spi_open(uint32_t freq, uint32_t clk_mode)
{
	int32_t ercd = E_OK;
	DEV_SPI *pmwifi_spi_ptr;

	pmwifi_spi_ptr = spi_get_dev(HSDK_PMWIFI_0_SPI_ID);
	PMWIFI_CHECK_EXP(pmwifi_spi_ptr != NULL, E_OBJ);

	ercd = pmwifi_spi_ptr->spi_open(DEV_MASTER_MODE, freq);
	if ((ercd == E_OPNED) || (ercd == E_OK)) {
		ercd = pmwifi_spi_ptr->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(clk_mode));
	}
error_exit:
	return ercd;
}

static int32_t pmwifi_0_spi_close(void)
{
	int32_t ercd = E_OK;
	DEV_SPI *pmwifi_spi_ptr;

	pmwifi_spi_ptr = spi_get_dev(HSDK_PMWIFI_0_SPI_ID);
	PMWIFI_CHECK_EXP(pmwifi_spi_ptr != NULL, E_OBJ);

	ercd = pmwifi_spi_ptr->spi_close();

error_exit:
	return ercd;
}

static int32_t pmwifi_0_spi_cs(int32_t cs)
{
	int32_t ercd = E_OK;
	DEV_SPI *pmwifi_spi_ptr;
	uint32_t cs_line = HSDK_PMWIFI_0_SPI_LINE;

	pmwifi_spi_ptr = spi_get_dev(HSDK_PMWIFI_0_SPI_ID);
	PMWIFI_CHECK_EXP(pmwifi_spi_ptr != NULL, E_OBJ);

	if (cs == WF_CHIP_SELECTED) {
		ercd = pmwifi_spi_ptr->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs_line));
	} else {
		ercd = pmwifi_spi_ptr->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs_line));
	}

error_exit:
	return ercd;
}

static int32_t pmwifi_0_spi_transfer(DEV_SPI_TRANSFER *xfer)
{
	int32_t ercd = E_OK;
	DEV_SPI *pmwifi_spi_ptr;

#ifdef HSDK_PMWIFI_0_SPI_CPULOCK_ENABLE
	unsigned int cs_cpu_status;
#endif
	pmwifi_spi_ptr = spi_get_dev(HSDK_PMWIFI_0_SPI_ID);
	PMWIFI_CHECK_EXP(pmwifi_spi_ptr != NULL, E_OBJ);
	PMWIFI_CHECK_EXP(xfer != NULL, E_PAR);

#ifdef HSDK_PMWIFI_0_SPI_CPULOCK_ENABLE
	cs_cpu_status = cpu_lock_save();
#endif
	ercd = pmwifi_spi_ptr->spi_control(SPI_CMD_TRANSFER_POLLING, (void *)(xfer));

#ifdef HSDK_PMWIFI_0_SPI_CPULOCK_ENABLE
	cpu_unlock_restore(cs_cpu_status);
#endif
error_exit:
	return ercd;
}

/* wifi spi operations object */
static WF_SPI_OPS pmwifi_0_spi_ops = {
	pmwifi_0_spi_open,
	pmwifi_0_spi_close,
	pmwifi_0_spi_cs,
	pmwifi_0_spi_transfer,
	HSDK_PMWIFI_0_SPI_FREQ,
	SPI_CPOL_0_CPHA_0
};

static WF_OPS pmwifi_0_ops = {
	&pmwifi_0_gpio_ops,
	&pmwifi_0_timer_ops,
	&pmwifi_0_spi_ops
};

/** install wnic 0 to system */
#define PMWIFI_0_NAME		"MRF24G"
#define PMWIFI_0_IFNAME0	'w'
#define PMWIFI_0_IFNAME1	'0'
static DEV_WNIC pmwifi_0_wnic;
const uint8_t  pmwifi_0_mac_addr[WNIC_HDR_LEN] = {
	HSDK_PMWIFI_0_MAC_ADDR0,
	HSDK_PMWIFI_0_MAC_ADDR1,
	HSDK_PMWIFI_0_MAC_ADDR2,
	HSDK_PMWIFI_0_MAC_ADDR3,
	HSDK_PMWIFI_0_MAC_ADDR4,
	HSDK_PMWIFI_0_MAC_ADDR5
};

static int32_t pmwifi_0_wnic_get_info(uint32_t cmd, void *rinfo)
{
	return mrf24g_wnic_get_info(&pmwifi_0_wnic, cmd, rinfo);
}

static int32_t pmwifi_0_wnic_control(uint32_t ctrl_cmd, void *param)
{
	return mrf24g_wnic_control(&pmwifi_0_wnic, ctrl_cmd, param);
}

static int32_t pmwifi_0_wnic_init(uint32_t network_type)
{
	return mrf24g_wnic_init(&pmwifi_0_wnic, network_type);
}

static int32_t pmwifi_0_poll_init_status(void)
{
	return mrf24g_poll_init_status(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_poll_busy_status(void)
{
	return mrf24g_poll_busy_status(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_set_network_type(uint32_t type)
{
	return mrf24g_set_network_type(&pmwifi_0_wnic, type);
}

static int32_t pmwifi_0_set_macaddr(uint8_t *mac)
{
	return mrf24g_set_macaddr(&pmwifi_0_wnic, mac);
}
static int32_t pmwifi_0_get_macaddr(uint8_t *mac)
{
	return mrf24g_get_macaddr(&pmwifi_0_wnic, mac);
}

static int32_t pmwifi_0_start_scan(void)
{
	return mrf24g_start_scan(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_stop_scan(void)
{
	return mrf24g_stop_scan(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_poll_scan_status(void)
{
	return mrf24g_poll_scan_status(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_get_scan_result_cnt(void)
{
	return mrf24g_get_scan_result_cnt(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_get_scan_result(uint32_t index, WNIC_SCAN_RESULT *result)
{
	return mrf24g_get_scan_result(&pmwifi_0_wnic, index, result);
}

static int32_t pmwifi_0_wnic_connect(uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key)
{
	return mrf24g_wnic_connect(&pmwifi_0_wnic, security, ssid, key);
}

static int32_t pmwifi_0_poll_conn_status(void)
{
	return mrf24g_poll_conn_status(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_wnic_disconnect(void)
{
	return mrf24g_wnic_disconnect(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_prepare_tx(uint32_t tx_len)
{
	return mrf24g_prepare_tx(&pmwifi_0_wnic, tx_len);
}

static int32_t pmwifi_0_add_tx_data(uint8_t *p_buf, uint32_t len)
{
	return mrf24g_add_tx_data(&pmwifi_0_wnic, p_buf, len);
}

static int32_t pmwifi_0_commit_tx(uint32_t len)
{
	return mrf24g_commit_tx(&pmwifi_0_wnic, len);
}

static int32_t pmwifi_0_prepare_rx(void)
{
	return mrf24g_prepare_rx(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_get_rx_data(uint8_t *p_buf, uint32_t len)
{
	return mrf24g_get_rx_data(&pmwifi_0_wnic, p_buf, len);
}

static int32_t pmwifi_0_accept_rx(void)
{
	return mrf24g_accept_rx(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_config_power_mode(int32_t power_mode)
{
	return mrf24g_config_power_mode(&pmwifi_0_wnic, power_mode);
}

static int32_t pmwifi_0_poll_power_mode(void)
{
	return mrf24g_poll_power_mode(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_wnic_reset(void)
{
	return mrf24g_wnic_reset(&pmwifi_0_wnic);
}

static void pmwifi_0_period_process(void *ptr)
{
	mrf24g_period_process(&pmwifi_0_wnic, ptr);
}

static void pmwifi_0_install(void)
{
	DEV_WNIC *pmwifi_wnic_ptr = &pmwifi_0_wnic;
	WF_OPS *pmwifi_ops_ptr = &pmwifi_0_ops;
	DEV_WNIC_INFO *pmwifi_wnic_info_ptr = &(pmwifi_0_wnic.wnic_info);
	DEV_WNIC_ON_OPS *pmwifi_wnic_on_ops_ptr = &(pmwifi_0_wnic.wnic_on_ops);

	memcpy((void *)pmwifi_wnic_info_ptr->name, (void *)PMWIFI_0_NAME, strlen(PMWIFI_0_NAME));
	pmwifi_wnic_info_ptr->ifname[0] = PMWIFI_0_IFNAME0;
	pmwifi_wnic_info_ptr->ifname[1] = PMWIFI_0_IFNAME1;
	memset((void *)&pmwifi_wnic_info_ptr->ssid, 0, sizeof(WNIC_CFG_SSID));
	pmwifi_wnic_info_ptr->ssid.ssid[0] = '\0';

	pmwifi_wnic_info_ptr->ctrl = (void *)pmwifi_ops_ptr;
	pmwifi_wnic_info_ptr->extra = NULL;
	pmwifi_wnic_info_ptr->opn_flg = DEV_CLOSED;
	pmwifi_wnic_info_ptr->err_flg = DEV_GOOD;
	pmwifi_wnic_info_ptr->network_type = WNIC_NETWORK_TYPE_INFRASTRUCTURE;
	pmwifi_wnic_info_ptr->init_status = WNIC_NOT_INITIALIZED;
	pmwifi_wnic_info_ptr->conn_status = WNIC_NOT_CONNECTED;
	pmwifi_wnic_info_ptr->conn_type = WNIC_CONN_NONE;
	pmwifi_wnic_info_ptr->mac_status = WNIC_MAC_NOT_UPDATED;
	pmwifi_wnic_info_ptr->scan_status = WNIC_NOT_SCANED;
	pmwifi_wnic_info_ptr->scan_results = 0;
	memcpy((void *)pmwifi_wnic_info_ptr->mac_addr, (void *)pmwifi_0_mac_addr, WNIC_HDR_LEN);
	pmwifi_wnic_info_ptr->busy_status = WNIC_IS_FREE;
	pmwifi_wnic_info_ptr->tx_frame_cnt = 0;
	pmwifi_wnic_info_ptr->tx_speed = 0;
	pmwifi_wnic_info_ptr->tx_pending = 0;
	pmwifi_wnic_info_ptr->rx_frame_cnt = 0;
	pmwifi_wnic_info_ptr->rx_speed = 0;
	pmwifi_wnic_info_ptr->rx_pending = 0;
	pmwifi_wnic_info_ptr->power_status = WNIC_POWER_OFF;

	pmwifi_wnic_on_ops_ptr->on_init_success	= NULL;
	pmwifi_wnic_on_ops_ptr->on_init_fail	= NULL;
	pmwifi_wnic_on_ops_ptr->on_connected 	= NULL;
	pmwifi_wnic_on_ops_ptr->on_disconnected	= NULL;
	pmwifi_wnic_on_ops_ptr->on_mac_updated 	= NULL;
	pmwifi_wnic_on_ops_ptr->on_scan_finished= NULL;
	pmwifi_wnic_on_ops_ptr->on_rxdata_comes	= NULL;
	pmwifi_wnic_on_ops_ptr->on_dev_asserted	= NULL;


	pmwifi_wnic_ptr->wnic_get_info 		= pmwifi_0_wnic_get_info	;
	pmwifi_wnic_ptr->wnic_control		= pmwifi_0_wnic_control		;
	pmwifi_wnic_ptr->wnic_init		= pmwifi_0_wnic_init		;
	pmwifi_wnic_ptr->poll_init_status	= pmwifi_0_poll_init_status	;
	pmwifi_wnic_ptr->poll_busy_status	= pmwifi_0_poll_busy_status	;
	pmwifi_wnic_ptr->set_network_type	= pmwifi_0_set_network_type	;
	pmwifi_wnic_ptr->set_macaddr		= pmwifi_0_set_macaddr		;
	pmwifi_wnic_ptr->get_macaddr		= pmwifi_0_get_macaddr		;
	pmwifi_wnic_ptr->start_scan		= pmwifi_0_start_scan		;
	pmwifi_wnic_ptr->stop_scan		= pmwifi_0_stop_scan		;
	pmwifi_wnic_ptr->poll_scan_status	= pmwifi_0_poll_scan_status	;
	pmwifi_wnic_ptr->get_scan_result_cnt	= pmwifi_0_get_scan_result_cnt	;
	pmwifi_wnic_ptr->get_scan_result 	= pmwifi_0_get_scan_result	;
	pmwifi_wnic_ptr->wnic_connect		= pmwifi_0_wnic_connect		;
	pmwifi_wnic_ptr->poll_conn_status	= pmwifi_0_poll_conn_status	;
	pmwifi_wnic_ptr->wnic_disconnect	= pmwifi_0_wnic_disconnect	;
	pmwifi_wnic_ptr->prepare_tx		= pmwifi_0_prepare_tx		;
	pmwifi_wnic_ptr->add_tx_data		= pmwifi_0_add_tx_data		;
	pmwifi_wnic_ptr->commit_tx		= pmwifi_0_commit_tx		;
	pmwifi_wnic_ptr->prepare_rx		= pmwifi_0_prepare_rx		;
	pmwifi_wnic_ptr->get_rx_data		= pmwifi_0_get_rx_data		;
	pmwifi_wnic_ptr->accept_rx		= pmwifi_0_accept_rx		;
	pmwifi_wnic_ptr->config_power_mode	= pmwifi_0_config_power_mode	;
	pmwifi_wnic_ptr->poll_power_mode	= pmwifi_0_poll_power_mode	;
	pmwifi_wnic_ptr->wnic_reset		= pmwifi_0_wnic_reset		;
	pmwifi_wnic_ptr->period_process		= pmwifi_0_period_process	;
}

WF_OPS_PTR get_wf_ops(void)
{
	return &pmwifi_0_ops;
}

/*********************PMOD WIFI -> RW009*********************/
#elif defined(WIFI_RW009)

#include "rw009.h"

/** install wnic 0 to system */
#define PMWIFI_0_NAME		"RT-RW009"
#define PMWIFI_0_IFNAME0	'w'
#define PMWIFI_0_IFNAME1	'0'

static DEV_WNIC pmwifi_0_wnic;
static RW009_DEF rw009;

static int32_t pmwifi_0_wnic_get_info(uint32_t cmd, void *rinfo)
{
	return rw009_wnic_get_info(&pmwifi_0_wnic, cmd, rinfo);
}

static int32_t pmwifi_0_wnic_control(uint32_t ctrl_cmd, void *param)
{
	return rw009_wnic_control(&pmwifi_0_wnic, ctrl_cmd, param);
}

static int32_t pmwifi_0_wnic_init(uint32_t network_type)
{
	return rw009_wnic_init(&pmwifi_0_wnic, network_type);
}

static int32_t pmwifi_0_poll_init_status(void)
{
	return rw009_poll_init_status(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_poll_busy_status(void)
{
	return rw009_poll_busy_status(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_set_network_type(uint32_t type)
{
	return rw009_set_network_type(&pmwifi_0_wnic, type);
}

static int32_t pmwifi_0_set_macaddr(uint8_t *mac)
{
	return rw009_set_macaddr(&pmwifi_0_wnic, mac);
}

static int32_t pmwifi_0_get_macaddr(uint8_t *mac)
{
	return rw009_get_macaddr(&pmwifi_0_wnic, mac);
}

static int32_t pmwifi_0_start_scan(void)
{
	return rw009_start_scan(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_stop_scan(void)
{
	return rw009_stop_scan(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_poll_scan_status(void)
{
	return rw009_poll_scan_status(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_get_scan_result_cnt(void)
{
	return rw009_get_scan_result_cnt(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_get_scan_result(uint32_t index, WNIC_SCAN_RESULT *result)
{
	return rw009_get_scan_result(&pmwifi_0_wnic, index, result);
}

static int32_t pmwifi_0_wnic_connect(uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key)
{
	return rw009_wnic_connect(&pmwifi_0_wnic, security, ssid, key);
}

static int32_t pmwifi_0_poll_conn_status(void)
{
	return rw009_poll_conn_status(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_wnic_disconnect(void)
{
	return rw009_wnic_disconnect(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_prepare_tx(uint32_t tx_len)
{
	return rw009_prepare_tx(&pmwifi_0_wnic, tx_len);
}

static int32_t pmwifi_0_add_tx_data(uint8_t *p_buf, uint32_t len)
{
	return rw009_add_tx_data(&pmwifi_0_wnic, p_buf, len);
}

static int32_t pmwifi_0_commit_tx(uint32_t len)
{
	return rw009_commit_tx(&pmwifi_0_wnic, len);
}

static int32_t pmwifi_0_prepare_rx(void)
{
	return rw009_prepare_rx(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_get_rx_data(uint8_t *p_buf, uint32_t len)
{
	return rw009_get_rx_data(&pmwifi_0_wnic, p_buf, len);
}

static int32_t pmwifi_0_accept_rx(void)
{
	return rw009_accept_rx(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_config_power_mode(int32_t power_mode)
{
	return rw009_config_power_mode(&pmwifi_0_wnic, power_mode);
}

static int32_t pmwifi_0_poll_power_mode(void)
{
	return rw009_poll_power_mode(&pmwifi_0_wnic);
}

static int32_t pmwifi_0_wnic_reset(void)
{
	return rw009_wnic_reset(&pmwifi_0_wnic);
}

static void pmwifi_0_period_process(void *ptr)
{
	rw009_period_process(&pmwifi_0_wnic, ptr);
}

static void pmwifi_0_int_handler(void *ptr)
{
	rw009_isr(&pmwifi_0_wnic);
}

static void pmwifi_0_install(void)
{
	DEV_WNIC *pmwifi_wnic_ptr = &pmwifi_0_wnic;
	DEV_WNIC_INFO *pmwifi_wnic_info_ptr = &(pmwifi_0_wnic.wnic_info);
	DEV_WNIC_ON_OPS *pmwifi_wnic_on_ops_ptr = &(pmwifi_0_wnic.wnic_on_ops);

	memcpy((void *)pmwifi_wnic_info_ptr->name, (void *)PMWIFI_0_NAME, strlen(PMWIFI_0_NAME));
	pmwifi_wnic_info_ptr->ifname[0] = PMWIFI_0_IFNAME0;
	pmwifi_wnic_info_ptr->ifname[1] = PMWIFI_0_IFNAME1;
	memset((void *)&pmwifi_wnic_info_ptr->ssid, 0, sizeof(WNIC_CFG_SSID));
	pmwifi_wnic_info_ptr->ssid.ssid[0] = '\0';

	pmwifi_wnic_info_ptr->ctrl = NULL;
	pmwifi_wnic_info_ptr->extra = NULL;
	pmwifi_wnic_info_ptr->opn_flg = DEV_CLOSED;
	pmwifi_wnic_info_ptr->err_flg = DEV_GOOD;
	pmwifi_wnic_info_ptr->network_type = WNIC_NETWORK_TYPE_INFRASTRUCTURE;
	pmwifi_wnic_info_ptr->init_status = WNIC_NOT_INITIALIZED;
	pmwifi_wnic_info_ptr->conn_status = WNIC_NOT_CONNECTED;
	pmwifi_wnic_info_ptr->conn_type = WNIC_CONN_NONE;
	pmwifi_wnic_info_ptr->mac_status = WNIC_MAC_NOT_UPDATED;
	pmwifi_wnic_info_ptr->scan_status = WNIC_NOT_SCANED;
	pmwifi_wnic_info_ptr->scan_results = 0;
	pmwifi_wnic_info_ptr->busy_status = WNIC_IS_FREE;
	pmwifi_wnic_info_ptr->tx_frame_cnt = 0;
	pmwifi_wnic_info_ptr->tx_speed = 0;
	pmwifi_wnic_info_ptr->tx_pending = 0;
	pmwifi_wnic_info_ptr->rx_frame_cnt = 0;
	pmwifi_wnic_info_ptr->rx_speed = 0;
	pmwifi_wnic_info_ptr->rx_pending = 0;
	pmwifi_wnic_info_ptr->power_status = WNIC_POWER_OFF;

	rw009_ptr->spi_master = HSDK_PMWIFI_0_SPI_ID;
	rw009_ptr->spi_cs = HSDK_PMWIFI_0_SPI_LINE;
	rw009_ptr->spi_freq = HSDK_PMWIFI_0_SPI_FREQ;
	rw009_ptr->gpio_int_busy = HSDK_PMWIFI_0_GPIO_ID;
	rw009_ptr->pin_int_busy = HSDK_PMWIFI_0_INT_PIN;
	rw009_ptr->gpio_int_handler = pmwifi_0_int_handler;
	rw009_ptr->gpio_rst = HSDK_PMWIFI_0_GPIO_ID;
	rw009_ptr->pin_rst = HSDK_PMWIFI_0_RST_PIN;

	rw009_ptr->gpio_cs = HSDK_PMWIFI_0_GPIO_ID;
	rw009_ptr->pin_cs = HSDK_PMWIFI_0_WP_PIN;

	pmwifi_wnic_on_ops_ptr->on_init_success	= NULL;
	pmwifi_wnic_on_ops_ptr->on_init_fail	= NULL;
	pmwifi_wnic_on_ops_ptr->on_connected 	= NULL;
	pmwifi_wnic_on_ops_ptr->on_disconnected	= NULL;
	pmwifi_wnic_on_ops_ptr->on_mac_updated 	= NULL;
	pmwifi_wnic_on_ops_ptr->on_scan_finished= NULL;
	pmwifi_wnic_on_ops_ptr->on_rxdata_comes	= NULL;
	pmwifi_wnic_on_ops_ptr->on_dev_asserted	= NULL;


	pmwifi_wnic_ptr->wnic_get_info 		= pmwifi_0_wnic_get_info	;
	pmwifi_wnic_ptr->wnic_control		= pmwifi_0_wnic_control		;
	pmwifi_wnic_ptr->wnic_init		= pmwifi_0_wnic_init		;
	pmwifi_wnic_ptr->poll_init_status	= pmwifi_0_poll_init_status	;
	pmwifi_wnic_ptr->poll_busy_status	= pmwifi_0_poll_busy_status	;
	pmwifi_wnic_ptr->set_network_type	= pmwifi_0_set_network_type	;
	pmwifi_wnic_ptr->set_macaddr		= pmwifi_0_set_macaddr		;
	pmwifi_wnic_ptr->get_macaddr		= pmwifi_0_get_macaddr		;
	pmwifi_wnic_ptr->start_scan		= pmwifi_0_start_scan		;
	pmwifi_wnic_ptr->stop_scan		= pmwifi_0_stop_scan		;
	pmwifi_wnic_ptr->poll_scan_status	= pmwifi_0_poll_scan_status	;
	pmwifi_wnic_ptr->get_scan_result_cnt	= pmwifi_0_get_scan_result_cnt	;
	pmwifi_wnic_ptr->get_scan_result 	= pmwifi_0_get_scan_result	;
	pmwifi_wnic_ptr->wnic_connect		= pmwifi_0_wnic_connect		;
	pmwifi_wnic_ptr->poll_conn_status	= pmwifi_0_poll_conn_status	;
	pmwifi_wnic_ptr->wnic_disconnect	= pmwifi_0_wnic_disconnect	;
	pmwifi_wnic_ptr->prepare_tx		= pmwifi_0_prepare_tx		;
	pmwifi_wnic_ptr->add_tx_data		= pmwifi_0_add_tx_data		;
	pmwifi_wnic_ptr->commit_tx		= pmwifi_0_commit_tx		;
	pmwifi_wnic_ptr->prepare_rx		= pmwifi_0_prepare_rx		;
	pmwifi_wnic_ptr->get_rx_data		= pmwifi_0_get_rx_data		;
	pmwifi_wnic_ptr->accept_rx		= pmwifi_0_accept_rx		;
	pmwifi_wnic_ptr->config_power_mode	= pmwifi_0_config_power_mode	;
	pmwifi_wnic_ptr->poll_power_mode	= pmwifi_0_poll_power_mode	;
	pmwifi_wnic_ptr->wnic_reset		= pmwifi_0_wnic_reset		;
	pmwifi_wnic_ptr->period_process		= pmwifi_0_period_process	;
}
#else /* PMWIFI_RW009 */
#error "no wifi module selected"
#endif

#endif /* USE_HSDK_PMWIFI_0 */

/** get one wnic device structure */
DEV_WNIC_PTR wnic_get_dev(int32_t wnic_id)
{
	static uint32_t install_flag = 0;

	/* install device objects */
	if (install_flag == 0) {
		install_flag = 1;
		pmwifi_all_install();
	}

	switch (wnic_id) {
#if (USE_HSDK_PMWIFI_0)
		case HSDK_PMWIFI_0_ID:
			return &pmwifi_0_wnic;
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
void pmwifi_all_install(void)
{
	/* use PMOD_B as pmwifi */
	if (io_pmod_config(PMOD_B, PMOD_SPI, 1) != E_OK) {
		while(1);
	}
#if (USE_HSDK_PMWIFI_0)
	pmwifi_0_install();
#endif
}
#endif  /* MID_LWIP && MID_LWIP_CONTRIB */
