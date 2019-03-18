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

#include <string.h>
#include "embARC.h"
#include "rw009.h"

#define DEBUG
#include "embARC_debug.h"


/* data format of rw009 */
/*
 *
 * Master:	REQ(16bytes)	+ DUMMY 	+ DATA to transfer
 * RW009:	DUMMY		+ RESP(16 bytes)+ DATA to transfer
 *
 * DATA: len(4bytes) + type(4bytes) + buffer
 * type: eth_data, cmd, resp, status
 * eth_data: ethernet datapacket to IP stack
 * cmd & resp:cmd(4bytes) + len(4bytes) + params
 *
 */

/* Note: all the following data structures are for little-endian */
/* Master starts the transfer with a request, rw009 will response */
#define RW009_CHECK_EXP(EXPR, ERROR_CODE)	CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

struct rw009_spi_request {
	uint32_t flag;
	uint32_t len; // master to slave data len.
	uint32_t magic1;
	uint32_t magic2;
};

#define RW009_REQ_MAGIC1		(0x67452301)
#define RW009_REQ_MAGIC2		(0xEFCDAB89)
#define RW009_REQ_FLAG_MRDY		(0x01) /* master ready to transfer */

struct rw009_spi_response {
	uint32_t flag;
	uint32_t len; // slave to master data len.
	uint32_t magic1;
	uint32_t magic2;
};

#define RW009_RESP_FLAG_SRDY		(0x01)	/* rw009 ready to transfer */
#define RW009_RESP_MAGIC1		(0x98BADCFE)
#define RW009_RESP_MAGIC2		(0x10325476)

#define RW009_CMD_PACKET_HRD_LEN	(8)

#define RW009_PASSWORD_LENGTH_MAX	(64)

#define RW009_CMD_INIT			(128)
#define RW009_CMD_SCAN			(129)
#define RW009_CMD_JOIN			(130)
#define RW009_CMD_EASY_JOIN		(131)
#define RW009_CMD_RSSI			(132)
#define RW009_CMD_SOFTAP		(133)
#define RW009_CMD_NULL			(0xff)

#define RW009_DATA_PACKET_HDR_LEN	(8)
#define RW009_CMD_PACKET_HDR_LEN	(8)

typedef enum {
	RW009_TYPE_ETH_DATA = 0,
	RW009_TYPE_CMD,
	RW009_TYPE_RESP,
	RW009_TYPE_STATUS
} RW009_DATA_TYPE_DEF;

typedef struct _rw009_cmd_init {
	uint32_t mode;
} RW009_CMD_INIT_DEF;

typedef struct _rw009_resp_init {
	uint8_t mac[8];     /* 6byte + 2byte PAD. */
	uint8_t sn[24];     /* serial no. of rw 009 */
	char version[16];   /* firmware version */
} RW009_RESP_INIT_DEF;

typedef struct _rw009_cmd_easy_join {
	char ssid[RW009_SSID_NAME_LENGTH_MAX];
	char passwd[RW009_PASSWORD_LENGTH_MAX];
} RW009_CMD_EASY_JOIN_DEF;

typedef struct _rw009_cmd_join {
	uint8_t bssid[8]; /* 6byte + 2byte PAD. */
	char passwd[RW009_PASSWORD_LENGTH_MAX];
} RW009_CMD_JOIN_DEF;

typedef struct _rw009_cmd_rssi {
	uint8_t bssid[8]; /* 6byte + 2byte PAD. */
} RW009_CMD_RSSI_DEF;

typedef struct _rw009_cmd_softap {
	char ssid[RW009_SSID_NAME_LENGTH_MAX];
	char passwd[RW009_PASSWORD_LENGTH_MAX];

	uint32_t security;   /* Security type. */
	uint32_t channel;    /* Radio channel that the AP beacon was received on   */
} RW009_CMD_SOFTAP_DEF;

typedef struct _rw009_resp_join {
	RW009_AP_INFO_DEF ap_info;
} RW009_RESP_JOIN_DEF;

typedef struct rw009_cmd {
	uint32_t cmd;
	uint32_t len;

	/** command body */
	union {
		RW009_CMD_INIT_DEF init;
		RW009_CMD_EASY_JOIN_DEF easy_join;
		RW009_CMD_JOIN_DEF join;
		RW009_CMD_RSSI_DEF rssi;
		RW009_CMD_SOFTAP_DEF softap;
	} params;
} RW009_CMD_DEF, * RW009_CMD_DEF_PTR;

struct rw009_resp {
	uint32_t cmd;
	uint32_t len;

	int32_t result; /* result for CMD */

	/** resp Body */
	union {
		RW009_RESP_INIT_DEF init;
		RW009_AP_INFO_DEF ap_info;
	} resp;
};

/**
 * @brief get current system time in millisecond
 * @return  millisecond
 */
static uint32_t rw009_timer_read_ms(void)
{
	return OSP_GET_CUR_MS();
}

static void rw009_timer_delay(uint32_t ms)
{
#if  defined(OS_FREERTOS)
	vTaskDelay(ms);
#else
	board_delay_ms(ms, OSP_DELAY_OS_COMPAT_ENABLE);
#endif
}

static int32_t rw009_spi_open(RW009_DEF_PTR rw009)
{
	DEV_SPI_PTR spi;
	int32_t ercd = E_OK;

	spi = spi_get_dev(rw009->spi_master);
	RW009_CHECK_EXP(spi != NULL, E_OBJ);

	ercd = spi->spi_open(DEV_MASTER_MODE, rw009->spi_freq);

	if (ercd == E_OPNED) {
		ercd = E_OK;
	}

error_exit:
	return ercd;
}

static int32_t rw009_gpio_open(RW009_DEF_PTR rw009)
{
	int32_t ercd = E_OK;
	uint32_t mask;
	DEV_GPIO_BIT_ISR bit_isr;
	DEV_GPIO_INT_CFG int_cfg;
	DEV_GPIO_PTR gpio;

	gpio = gpio_get_dev(rw009->gpio_rst);
	RW009_CHECK_EXP(gpio != NULL, E_OBJ);

	ercd = gpio->gpio_open(1 << rw009->pin_rst);

	if (ercd == E_OPNED) {
		mask = 1 << rw009->pin_rst;
		gpio->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, (void *)mask);
	}

	gpio = gpio_get_dev(rw009->gpio_cs);
	RW009_CHECK_EXP(gpio != NULL, E_OBJ);

	ercd = gpio->gpio_open(1 << rw009->pin_cs);

	if (ercd == E_OPNED) {
		mask = 1 << rw009->pin_cs;
		gpio->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, (void *)mask);
	}

	gpio = gpio_get_dev(rw009->gpio_int_busy);
	RW009_CHECK_EXP(gpio != NULL, E_OBJ);

	/* pin_int_busy is input */
	ercd = gpio->gpio_open(0);

	mask = 1 << rw009->pin_int_busy;
	if (ercd == E_OPNED) {
		gpio->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)mask);
	}

	int_cfg.int_bit_mask = mask;
	int_cfg.int_bit_type = GPIO_INT_BITS_EDGE_TRIG(mask);
	int_cfg.int_bit_polarity = GPIO_INT_BITS_POL_FALL_EDGE(mask);
	int_cfg.int_bit_debounce = GPIO_INT_BITS_DIS_DEBOUNCE(mask);
	gpio->gpio_control(GPIO_CMD_SET_BIT_INT_CFG, (void *)(&int_cfg));
	bit_isr.int_bit_ofs = rw009->pin_int_busy;
	bit_isr.int_bit_handler = rw009->gpio_int_handler;
	gpio->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&bit_isr));

	ercd = gpio->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)(mask));

error_exit:
	return ercd;
}

/**
 * @brief enable rw009 interrupt
 */
static void rw009_int_enable(RW009_DEF_PTR rw009)
{
	DEV_GPIO_PTR gpio = gpio_get_dev(rw009->gpio_int_busy);

	gpio->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)(1 << rw009->pin_int_busy));
}

/**
 * @brief disable rw009 interrupt
 */
static void rw009_int_disable(RW009_DEF_PTR rw009)
{
	DEV_GPIO_PTR gpio = gpio_get_dev(rw009->gpio_int_busy);

	gpio->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)(1 << rw009->pin_int_busy));
}

/**
 * @brief poll rw009 whether is busy
 * @return 1 busy, 0 idle
 */
static int32_t rw009_is_busy(RW009_DEF_PTR rw009)
{
	uint32_t val;
	DEV_GPIO_PTR gpio = gpio_get_dev(rw009->gpio_int_busy);

	gpio->gpio_read(&val, 1 << rw009->pin_int_busy);

	if (val) {
		return 0;
	}

	return 1;
}

/**
 * @brief reset rw009
 */
static void rw009_reset(RW009_DEF_PTR rw009)
{
#define RW009_RESET_ASSERT_MS	100
#define RW009_RESET_DELAY_MS	200
	DEV_GPIO_PTR gpio = gpio_get_dev(rw009->gpio_rst);

	gpio->gpio_write(~(1 << rw009->pin_rst), 1 << rw009->pin_rst);
	rw009_timer_delay(RW009_RESET_ASSERT_MS);
	gpio->gpio_write(1 << rw009->pin_rst, 1 << rw009->pin_rst);
	rw009_timer_delay(RW009_RESET_DELAY_MS);
}

/**
 * @brief chip select rw009
 */
static void rw009_select(RW009_DEF_PTR rw009)
{
	DEV_SPI_PTR spi = spi_get_dev(rw009->spi_master);
	DEV_GPIO_PTR gpio = gpio_get_dev(rw009->gpio_cs);

	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)rw009->spi_cs));
	gpio->gpio_write(~(1 << rw009->pin_cs), 1 << rw009->pin_cs);
}

/**
 * @brief chip de-select rw009
 */
static void rw009_deselect(RW009_DEF_PTR rw009)
{
	DEV_SPI_PTR spi = spi_get_dev(rw009->spi_master);
	DEV_GPIO_PTR gpio = gpio_get_dev(rw009->gpio_cs);

	gpio->gpio_write(1 << rw009->pin_cs, 1 << rw009->pin_cs);
	spi->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID((uint32_t)rw009->spi_cs));
}

/**
 * @brief cmd response handler
 *
 * @param rw009_wnic rw009 object
 * @param rw009_resp response from rw009

 * @return error code
 */
static int32_t cmd_resp_handler(DEV_WNIC *rw009_wnic, struct rw009_resp* resp)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	DEV_WNIC_ON_OPS_PTR rw009_on_ops;
	RW009_DEF_PTR rw009;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	rw009_on_ops = &(rw009_wnic->wnic_on_ops);

	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

	if (resp->cmd != rw009->last_cmd) {
		DBG("current command is %d, cmd and response do not match!\r\n", resp->cmd);
	}

	switch (resp->cmd) {
		case RW009_CMD_INIT:
			RW009_CHECK_EXP((rw009_info->init_status == WNIC_DURING_INITIALIZATION), E_PAR);

			DBG("response for RW009_CMD_INIT\r\n");
			DBG("sn:%s\r\n", resp->resp.init.sn);
			DBG("ver:%s\r\n", resp->resp.init.version);
			memcpy(rw009_info->mac_addr, resp->resp.init.mac, WNIC_HDR_LEN);
			rw009_info->init_status = WNIC_INIT_SUCCESSFUL;
			rw009->init_try = 0;

			if (rw009_on_ops->on_init_success != NULL) {
				rw009_on_ops->on_init_success(rw009_wnic);
			}
			break;
		case RW009_CMD_SCAN:
			RW009_CHECK_EXP((rw009_info->scan_status == WNIC_DURING_SCAN), E_PAR);
			DBG("response for rw009_cmd_scan\n");
			if (rw009->scan_cnt < RW009_SCAN_AP_NUM) {
				memcpy(&(rw009->scanned_ap[rw009->scan_cnt++]), &resp->resp.ap_info, sizeof(RW009_AP_INFO_DEF));
			}
			break;
		case RW009_CMD_JOIN:
			if (rw009_info->conn_status != WNIC_CONNECTED) {
				break;
			}
		case RW009_CMD_EASY_JOIN:
			RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_PAR);
			RW009_CHECK_EXP((rw009_info->conn_status == WNIC_DURING_CONNECTION) || \
			                (rw009_info->conn_status == WNIC_CONNECTED), E_CLS);
			rw009_info->busy_status = WNIC_IS_FREE;
			DBG("response for RW009_CMD_JOIN or easy_join(cmd:%d, resp:%d)\r\n", resp->cmd, resp->result);
			if (resp->result == 0) {
				/* connected */
				DBG("rw009 connection success\r\n");
				if (rw009_info->conn_status != WNIC_CONNECTED) {
					rw009_info->conn_status = WNIC_CONNECTED;
					if (rw009_on_ops->on_connected != NULL) {
						rw009_on_ops->on_connected(rw009_wnic);
					}
				}
			} else {
				if (! (((resp->cmd == RW009_CMD_JOIN) && (resp->result == 12)) || \
					(resp->cmd == RW009_CMD_EASY_JOIN)) ) {
					break;
				}
				if ((rw009_info->conn_status == WNIC_CONNECTED) && \
					(resp->cmd == RW009_CMD_EASY_JOIN)) {
					break;
				}
				DBG("rw009 connection failure\r\n");
				if (rw009_info->conn_status == WNIC_CONNECTED) {
					rw009_info->conn_status = WNIC_NOT_CONNECTED;
					if (rw009_on_ops->on_disconnected != NULL) {
						rw009_on_ops->on_disconnected(rw009_wnic);
					}
				} else {
					rw009_info->conn_status = WNIC_NOT_CONNECTED;
				}
			}
			break;
		case RW009_CMD_RSSI:
			RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_PAR);
			DBG("response for RW009_CMD_RSSI\r\n");
			/* \todo use to poll the status of connection ? */
			break;
		default:
			DBG("unknown cmd response\r\n");
			ercd = E_PAR;
			break;
	}

error_exit:
	return ercd;
}

/**
 * @brief transfer function between rw009 and host
 *
 * @param rw009_wnic rw009 object
 * @param out output packet
 * @param rw009_spi_data_packet input packet
 * @return < 0 error
 */
static int32_t rw009_xfer(DEV_WNIC *rw009_wnic, struct rw009_spi_data_packet* out, struct rw009_spi_data_packet* in)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	DEV_WNIC_ON_OPS_PTR rw009_on_ops;
	RW009_DEF_PTR rw009;
	DEV_SPI_PTR spi;

	struct rw009_spi_request req;
	struct rw009_spi_response resp;
	struct dev_spi_transfer xfer;
	uint32_t to_send = 0;


	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	rw009_on_ops = &(rw009_wnic->wnic_on_ops);

	if (out == NULL || in == NULL) {
		return -1;
	}

	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;
	spi = spi_get_dev(rw009->spi_master);

	rw009_int_disable(rw009);

	while (rw009_is_busy(rw009)); /* wait if rw009 is busy */

	req.magic1 = RW009_REQ_MAGIC1;
	req.magic2 = RW009_REQ_MAGIC2;
	req.flag = RW009_REQ_FLAG_MRDY;
	if (out->len) {
		req.len = out->len + RW009_DATA_PACKET_HDR_LEN;
	} else {
		req.len = 0;
	}

	spi->spi_control(SPI_CMD_MST_SET_FREQ, CONV2VOID(rw009->spi_freq));
	spi->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(SPI_CLK_MODE_0));

	/* send cmd request */
	rw009_select(rw009);
	spi->spi_write(&req, sizeof(req));
	rw009_deselect(rw009);

	while (rw009_is_busy(rw009));

	rw009_select(rw009);

	/* receive cmd request response */
	spi->spi_read(&resp, sizeof(resp));

	if ((resp.magic1 != RW009_RESP_MAGIC1) || (resp.magic2 != RW009_RESP_MAGIC2)) {
		DBG("bad resp magic, abort!\r\n");
		return E_SYS;
	}

	if (resp.flag & RW009_RESP_FLAG_SRDY) {
		//DBG("slave is ready to receive\n");
		to_send = req.len;
	}

	if (resp.len > RW009_SPI_MAX_DATA_LEN) {
		resp.len = 0;
	}

	DEV_SPI_XFER_SET_TXBUF(&xfer, out, 0, to_send);
	DEV_SPI_XFER_SET_RXBUF(&xfer, in, 0, resp.len);
	DEV_SPI_XFER_SET_NEXT(&xfer, NULL);

	spi->spi_control(SPI_CMD_TRANSFER_POLLING, &xfer);

	rw009_deselect(rw009);

	if (resp.len > 0) {
		if (in->type == RW009_TYPE_ETH_DATA) {
			//DBG("eth data:%d\n", resp.len);
			/* ethernet data */
			if (rw009_info->init_status == WNIC_INIT_SUCCESSFUL) {
				if (rw009_on_ops->on_rxdata_comes != NULL) {
					rw009_on_ops->on_rxdata_comes(rw009_wnic);
				}
			}
			/* eth packet it recevied */
		} else if (in->type == RW009_TYPE_RESP) {
			/* cmd respone received */
			cmd_resp_handler(rw009_wnic, (struct rw009_resp *)in->buffer);
		} else {
			DBG("unsupported data type\r\n");
			ercd = E_SYS;
		}
	}
	rw009_int_enable(rw009);
	//DBG("master send %d bytes, rw009 send %d bytes\r\n", to_send, resp.len);
error_exit:
	return ercd;
}

/**
 * @brief send cmd to rw009
 *
 * @param rw009_wnic rw009 object
 * @param cmd command for rw009
 * @param args arguments of command
 * @return < 0 failed
 */
static int32_t rw009_cmd(DEV_WNIC *rw009_wnic, uint32_t cmd, void *args)
{
	uint32_t cmd_len;
#ifdef RW009_SPI_CPULOCK_ENABLE
	uint32_t cpu_status;
#endif
	RW009_CMD_DEF_PTR cmd_packet;
	struct rw009_spi_data_packet* out;
	RW009_DEF_PTR rw009;

	cmd_len = 0;
	rw009 = (RW009_DEF_PTR)(rw009_wnic->wnic_info.ctrl);

	switch (cmd) {
		case  RW009_CMD_INIT:
			cmd_len = sizeof(RW009_CMD_INIT_DEF);
			rw009->init_timer = rw009_timer_read_ms() + RW009_INIT_TIME_MS;
			break;
		case RW009_CMD_SCAN:
			cmd_len = 0;
			break;
		case RW009_CMD_JOIN:
			cmd_len = sizeof(RW009_CMD_JOIN_DEF);
			rw009->conn_timer = rw009_timer_read_ms() + RW009_CON_TIME_MS;
			break;
		case RW009_CMD_EASY_JOIN:
			cmd_len = sizeof(RW009_CMD_EASY_JOIN_DEF);
			rw009->conn_timer = rw009_timer_read_ms() + RW009_CON_TIME_MS;
			break;
		case RW009_CMD_SOFTAP:
			cmd_len = sizeof(RW009_CMD_SOFTAP_DEF);
			break;
		case RW009_CMD_RSSI:
			cmd_len = sizeof(RW009_CMD_RSSI_DEF);
			break;
		default:
			cmd_len = 0xff;
			break;
	}

	if (cmd_len == 0xff) {
		DBG("cmd type error\r\n");
		return -1;
	}

#ifdef RW009_SPI_CPULOCK_ENABLE
	cpu_status = cpu_lock_save();
#endif

	out = &rw009->snd_packet;
	cmd_packet = (RW009_CMD_DEF_PTR)out->buffer;

	cmd_packet->cmd = cmd;
	cmd_packet->len = cmd_len;
	memcpy(&cmd_packet->params, args, cmd_len);
	rw009->last_cmd = cmd;
	out->type = RW009_TYPE_CMD;
	out->len = RW009_CMD_PACKET_HDR_LEN + cmd_len;
	rw009_xfer(rw009_wnic, out, &rw009->rcv_packet);
	out->len = 0;
	out->type = 0;

#ifdef RW009_SPI_CPULOCK_ENABLE
	cpu_unlock_restore(cpu_status);
#endif
	return 0;
}

/* the following is rw009 implementation for WNIC interface */
int32_t rw009_wnic_get_info(DEV_WNIC *rw009_wnic, uint32_t cmd, void *rinfo)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

error_exit:
	return ercd;
}

int32_t rw009_wnic_control(DEV_WNIC *rw009_wnic, uint32_t ctrl_cmd, void *param)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

error_exit:
	return ercd;
}

int32_t rw009_wnic_init(DEV_WNIC *rw009_wnic, uint32_t network_type)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	RW009_CMD_INIT_DEF cmd;
	RW009_DEF_PTR rw009;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_NOT_INITIALIZED) \
	                || (rw009_info->init_status == WNIC_INIT_FAILED), E_OPNED);

	RW009_CHECK_EXP((network_type == WNIC_NETWORK_TYPE_INFRASTRUCTURE) \
	                || (network_type == WNIC_NETWORK_TYPE_ADHOC), E_PAR);

	rw009_info->init_status = WNIC_DURING_INITIALIZATION;  /* start initialization process */
	rw009_info->network_type = network_type; /* set network type used next */

	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

	DBG("init rw009\r\n");

	if ((ercd = rw009_gpio_open(rw009)) != E_OK) {
		DBG("rw009 gpio open failed\r\n");
		return ercd;
	}

	if ((ercd = rw009_spi_open(rw009)) != E_OK) {
		DBG("rw009 spi open failed\r\n");
		return ercd;
	}

	rw009_reset(rw009);
	rw009_deselect(rw009);

	cmd.mode = RW009_MODE_STATION;
	rw009_cmd(rw009_wnic, RW009_CMD_INIT, (void *)&cmd);


	rw009_info->power_status = WNIC_POWER_NORMAL;

error_exit:
	return ercd;
}

int32_t rw009_poll_init_status(DEV_WNIC *rw009_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);

	ercd = rw009_info->init_status;

error_exit:
	return ercd;
}

int32_t rw009_poll_busy_status(DEV_WNIC *rw009_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	ercd = rw009_info->busy_status;

error_exit:
	return ercd;
}

int32_t rw009_set_network_type(DEV_WNIC *rw009_wnic, uint32_t type)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	RW009_CHECK_EXP((type == WNIC_NETWORK_TYPE_INFRASTRUCTURE) \
	                || (type == WNIC_NETWORK_TYPE_ADHOC), E_PAR);

	/* \todo: set network type: SoftAP is supported in rw009 */
	rw009_info->network_type = type;

error_exit:
	return ercd;
}

int32_t rw009_set_macaddr(DEV_WNIC *rw009_wnic, uint8_t *mac)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	DEV_WNIC_ON_OPS_PTR rw009_on_ops;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	RW009_CHECK_EXP((mac != NULL), E_PAR);

	/* Note: the mac of rw009 can not be set */
	rw009_on_ops = &(rw009_wnic->wnic_on_ops);
	if (rw009_on_ops->on_mac_updated != NULL) {
		rw009_on_ops->on_mac_updated(rw009_wnic);
	}

error_exit:
	return ercd;
}

int32_t rw009_get_macaddr(DEV_WNIC *rw009_wnic, uint8_t *mac)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	RW009_CHECK_EXP((mac != NULL), E_PAR);

	memcpy(mac, rw009_info->mac_addr, WNIC_HDR_LEN);
error_exit:
	return ercd;
}

int32_t rw009_start_scan(DEV_WNIC *rw009_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	RW009_DEF_PTR rw009;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	RW009_CHECK_EXP((rw009_info->scan_status != WNIC_DURING_SCAN), E_CLS);
	RW009_CHECK_EXP((rw009_info->conn_status == WNIC_NOT_CONNECTED) \
	                || (rw009_info->conn_status == WNIC_CONNECTED), E_CLS);
	RW009_CHECK_EXP((rw009_info->busy_status == WNIC_IS_FREE), E_CLS);


	rw009_info->scan_status = WNIC_DURING_SCAN;
	rw009_info->busy_status = WNIC_IS_BUSY;
	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

	rw009->scan_timer = rw009_timer_read_ms() + RW009_SCAN_TIME_MS;
	rw009_cmd(rw009_wnic, RW009_CMD_SCAN, NULL);

error_exit:
	return ercd;
}

int32_t rw009_stop_scan(DEV_WNIC *rw009_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	RW009_DEF_PTR rw009;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	rw009_info->scan_status = WNIC_NOT_SCANED;
	rw009_info->busy_status = WNIC_IS_FREE;
	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

	rw009->scan_cnt = 0;
	rw009->scan_timer = 0;

	/* \todo cannot stop the scan of rw009 */

error_exit:
	return ercd;
}

int32_t rw009_poll_scan_status(DEV_WNIC *rw009_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	ercd = rw009_info->scan_status;

error_exit:
	return ercd;
}

int32_t rw009_get_scan_result_cnt(DEV_WNIC *rw009_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	if (rw009_info->scan_status == WNIC_SCAN_FINISHED) {
		ercd = rw009_info->scan_results;
	} else {
		ercd = 0;
	}

error_exit:
	return ercd;
}

int32_t rw009_get_scan_result(DEV_WNIC *rw009_wnic, uint32_t index, WNIC_SCAN_RESULT *result)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	RW009_AP_INFO_DEF_PTR ap;
	RW009_DEF_PTR rw009;


	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	RW009_CHECK_EXP(result != NULL, E_PAR);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	RW009_CHECK_EXP((rw009_info->scan_status == WNIC_SCAN_FINISHED), E_CLS);
	RW009_CHECK_EXP((index < rw009_info->scan_results), E_PAR);

	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

	ap = &rw009->scanned_ap[index];

	memcpy(result->bssid, ap->bssid, WNIC_BSSID_MAX_LEN);
	memcpy(result->ssid, ap->ssid, WNIC_SSID_MAX_LEN);
	result->ssidlen = strlen(ap->ssid);
	result->ap_config.apConfig = 0;
	result->rssi = ap->rssi;
	result->bsstype = 0;
	result->channel = ap->channel;

error_exit:
	return ercd;
}

int32_t rw009_wnic_connect(DEV_WNIC *rw009_wnic, uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	int32_t ssid_len;
	RW009_CMD_EASY_JOIN_DEF easy_join;


	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	RW009_CHECK_EXP((ssid != NULL), E_PAR);
	RW009_CHECK_EXP(!((key == NULL) && (security != AUTH_SECURITY_OPEN)), E_PAR);

	ssid_len = strlen((const char *)ssid);
	RW009_CHECK_EXP((ssid_len <= WNIC_SSID_MAX_LEN), E_PAR); /* check ssid len */

	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	RW009_CHECK_EXP((rw009_info->busy_status == WNIC_IS_FREE), E_CLS);
	RW009_CHECK_EXP((rw009_info->conn_status == WNIC_NOT_CONNECTED) \
	                || (rw009_info->conn_status == WNIC_CONNECTED), E_CLS);

	if (rw009_info->conn_status == WNIC_CONNECTED) {
		/* if connected to a different ssid, just disconnect it */
		if (strcmp((const char *)ssid, (const char *)(rw009_info->ssid.ssid)) == 0) {
			ercd = E_OK;
			goto error_exit;
		} else {
			if (rw009_wnic_disconnect(rw009_wnic) != E_OK) { /* if disconnect failed just return failed */
				ercd = E_SYS;
			}
		}
	}

	rw009_info->busy_status = WNIC_IS_BUSY;
	rw009_info->conn_status = WNIC_DURING_CONNECTION;

	memcpy(rw009_info->ssid.ssid, ssid, ssid_len);
	strncpy(easy_join.ssid, (const char *)ssid, sizeof(easy_join.ssid) );
	strncpy(easy_join.passwd, (const char *)key->key, sizeof(easy_join.passwd));

	ercd = rw009_cmd(rw009_wnic, RW009_CMD_EASY_JOIN, (void *)&easy_join);

error_exit:
	return ercd;
}

int32_t rw009_poll_conn_status(DEV_WNIC *rw009_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	if (rw009_info->busy_status == WNIC_IS_BUSY) {
		ercd = rw009_info->conn_status;
		goto error_exit;
	}
	if (rw009_info->conn_status == WNIC_CONNECTED) {
		ercd = WNIC_CONNECTED;
		goto error_exit;
	}
	rw009_info->conn_status = ercd;

error_exit:
	return ercd;
}

int32_t rw009_wnic_disconnect(DEV_WNIC *rw009_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	DEV_WNIC_ON_OPS_PTR rw009_on_ops;
	RW009_DEF_PTR rw009;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);
	RW009_CHECK_EXP((rw009_info->conn_status != WNIC_DURING_DISCONNECTION), E_CLS);

	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

	if (rw009_poll_conn_status(rw009_wnic) == WNIC_CONNECTED) {
		rw009_info->conn_status = WNIC_DURING_DISCONNECTION;
		rw009_reset(rw009);
		rw009_info->conn_status = WNIC_NOT_CONNECTED;
		rw009_on_ops = &(rw009_wnic->wnic_on_ops);
		if (rw009_on_ops->on_disconnected != NULL) {
			rw009_on_ops->on_disconnected(rw009_wnic);
		}
	}

error_exit:
	return ercd;
}

int32_t rw009_prepare_tx(DEV_WNIC *rw009_wnic, uint32_t tx_len)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	RW009_DEF_PTR rw009;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

	rw009->snd_cnt = 0;

error_exit:
	return ercd;
}

int32_t rw009_add_tx_data(DEV_WNIC *rw009_wnic, uint8_t *p_buf, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	RW009_DEF_PTR rw009;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	RW009_CHECK_EXP(p_buf != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

	memcpy(rw009->snd_packet.buffer + rw009->snd_cnt, p_buf, len);
	rw009->snd_cnt += len;
	/* copy data into tx buffer */

	ercd = len;

error_exit:
	return ercd;
}

int32_t rw009_commit_tx(DEV_WNIC *rw009_wnic, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	RW009_DEF_PTR rw009;

#ifdef RW009_SPI_CPULOCK_ENABLE
	uint32_t cpu_status;
#endif

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

#ifdef RW009_SPI_CPULOCK_ENABLE
	cpu_status = cpu_lock_save();
#endif

	rw009->snd_packet.len = rw009->snd_cnt;
	rw009->snd_packet.type = RW009_TYPE_ETH_DATA;
	rw009_xfer(rw009_wnic, &rw009->snd_packet, &rw009->rcv_packet);
	rw009->snd_packet.len = 0;
	rw009->snd_cnt = 0;

#ifdef RW009_SPI_CPULOCK_ENABLE
	cpu_unlock_restore(cpu_status);
#endif

error_exit:
	return ercd;
}

int32_t rw009_prepare_rx(DEV_WNIC *rw009_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	RW009_DEF_PTR rw009;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

	rw009->rcv_cnt = 0;
	return rw009->rcv_packet.len;
	/* get received data length in rx buffer */

error_exit:
	return ercd;
}

int32_t rw009_get_rx_data(DEV_WNIC *rw009_wnic, uint8_t *p_buf, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	RW009_DEF_PTR rw009;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	RW009_CHECK_EXP(p_buf != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

	memcpy(p_buf, rw009->rcv_packet.buffer + rw009->rcv_cnt, len);
	rw009->rcv_cnt += len;
	/* copy received data */
	/* just copy data in rx buffer */

error_exit:
	return ercd;
}

int32_t rw009_accept_rx(DEV_WNIC *rw009_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	RW009_DEF_PTR rw009;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

	rw009->rcv_cnt = 0;
	rw009->rcv_packet.len = 0;

	/*  Note: nothing to do in rw009 */

error_exit:
	return ercd;
}

int32_t rw009_config_power_mode(DEV_WNIC *rw009_wnic, int32_t power_mode)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	/* \todo no support in rw009 */

error_exit:
	return ercd;
}

int32_t rw009_poll_power_mode(DEV_WNIC *rw009_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	/* \todo no support in rw009 */
	ercd = rw009_info->power_status;
error_exit:
	return ercd;
}

int32_t rw009_wnic_reset(DEV_WNIC *rw009_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->init_status == WNIC_INIT_SUCCESSFUL), E_CLSED);

	rw009_info->init_status = WNIC_NOT_INITIALIZED;
	rw009_info->power_status = WNIC_POWER_OFF;

	rw009_reset((RW009_DEF_PTR)rw009_info->ctrl);
	ercd = rw009_wnic_init(rw009_wnic, rw009_info->network_type);

error_exit:
	return ercd;
}

void rw009_period_process(DEV_WNIC *rw009_wnic, void *ptr)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO_PTR rw009_info;
	RW009_DEF_PTR rw009;
#ifdef RW009_SPI_CPULOCK_ENABLE
	uint32_t cpu_status;
#endif
	DEV_WNIC_ON_OPS_PTR rw009_on_ops;

	RW009_CHECK_EXP(rw009_wnic != NULL, E_OBJ);
	rw009_info = &(rw009_wnic->wnic_info);
	RW009_CHECK_EXP((rw009_info->power_status == WNIC_POWER_NORMAL), E_CLSED);
	rw009_on_ops = &(rw009_wnic->wnic_on_ops);

	rw009 = (RW009_DEF_PTR)rw009_info->ctrl;

	if (rw009->rw009_to_send) {
#ifdef RW009_SPI_CPULOCK_ENABLE
		cpu_status = cpu_lock_save();
#endif
		rw009_xfer(rw009_wnic, &rw009->snd_packet, &rw009->rcv_packet);
		rw009->rw009_to_send = 0;

#ifdef RW009_SPI_CPULOCK_ENABLE
		cpu_unlock_restore(cpu_status);
#endif
	}

	if (rw009_info->scan_status == WNIC_DURING_SCAN) {
		if (rw009_timer_read_ms() > rw009->scan_timer) {
			//DBG("rw009 scan finished\n");
			rw009_info->scan_results = rw009->scan_cnt;
			rw009_info->scan_status = WNIC_SCAN_FINISHED;
			rw009_info->busy_status = WNIC_IS_FREE;
			rw009->scan_timer = 0;
			if (rw009_on_ops->on_scan_finished != NULL) {
				rw009_on_ops->on_scan_finished(rw009_wnic);
			}
		}
	}

	if (rw009_info->init_status == WNIC_DURING_INITIALIZATION) {
		if (rw009_timer_read_ms() > rw009->init_timer) {
			if (rw009->init_try < RW009_INIT_RETRY) {
				DBG("rw009 re-init\r\n");
				RW009_CMD_INIT_DEF cmd;
				rw009_reset(rw009);
				rw009_deselect(rw009);
				cmd.mode = RW009_MODE_STATION;
				rw009_cmd(rw009_wnic, RW009_CMD_INIT, (void *)&cmd);
				rw009->init_try++;
			} else {
				DBG("rw009 init failed\r\n");
				rw009->init_try = 0;
				rw009_info->init_status = WNIC_INIT_FAILED;
				if (rw009_on_ops->on_init_fail != NULL) {
					rw009_on_ops->on_init_fail(rw009_wnic);
				}
			}
		}
	}

	if (rw009_info->conn_status == WNIC_DURING_CONNECTION) {
		if (rw009_timer_read_ms() > rw009->conn_timer) {
			DBG("rw009 connection time out\r\n");
			rw009_info->conn_status = WNIC_NOT_CONNECTED;
			if (rw009_on_ops->on_disconnected != NULL) {
				rw009_on_ops->on_disconnected(rw009_wnic);
			}
		}
	}

error_exit:
	return;
}

/**
 * @brief interrupt service routine for rw009
 *
 * @param ptr  parameter to isr
 */
void rw009_isr(DEV_WNIC_PTR rw009_wnic)
{
	RW009_DEF_PTR rw009;

	rw009 = (RW009_DEF_PTR)rw009_wnic->wnic_info.ctrl;
	//DBG("interrupt from rw009\r\n");
	/* rw009 has data to send */
	if (rw009->rw009_to_send == 0) {
		rw009->rw009_to_send = 1;
	} else {
		DBG("rw009 interrupt missed\r\n");
	}
	rw009_int_disable(rw009);
}