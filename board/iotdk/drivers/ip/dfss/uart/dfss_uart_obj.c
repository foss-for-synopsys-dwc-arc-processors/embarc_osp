/* ------------------------------------------
 * Copyright (c) 2016, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:

 * 1) Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */

/**
 * \defgroup	BOARD_IOTDK_DRV_DFSS_UART_OBJ	IOTDK DFSS UART Object
 * \ingroup	BOARD_IOTDK_DRIVER
 * \brief	IOTDK DFSS UART Objects
 * \details

 *		Realize the IOTDK board uart object using DFSS
 *	uart device driver, only need to realize some DFSS uart
 *	structures combine with IOTDK uart hardware resource. just like
 *	cpp class instantiation.
 */

/**
 * \file
 * \ingroup	BOARD_IOTDK_DRV_DFSS_UART_OBJ
 * \brief	DFSS uart object instantiation on emsk
 */

/**
 * \addtogroup	BOARD_IOTDK_DRV_DFSS_UART_OBJ
 * @{
 */
#include "arc.h"
#include "arc_builtin.h"
#include "arc_exception.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "../../iotdk.h"
#include "dfss_uart_obj.h"
#include "io_config.h"
#include "uart.h"

typedef struct uart_dev_context
{
	uint32_t		intno;
	volatile int32_t	tx_done;
	volatile int32_t	rx_done;
	DEV_UART_INFO		*info;
} UART_DEV_CONTEXT;


UART_DEV_CONTEXT uart_ctx[] =
{
#if (USE_DFSS_UART_0)
	{IO_UART0_INTR},
#else
	{0},
#endif

#if (USE_DFSS_UART_1)
	{IO_UART1_INTR},
#else
	{0},
#endif

#if (USE_DFSS_UART_2)
	{IO_UART2_INTR},
#else
	{0},
#endif

#if (USE_DFSS_UART_3)
	{IO_UART3_INTR},
#else
	{0},
#endif
};

static inline UART_DEV_CONTEXT *get_context(uint32_t dev_id)
{
	return &uart_ctx[dev_id];
}

static void rx_done(uint32_t dev_id)
{
	UART_DEV_CONTEXT *ctx = get_context(dev_id);
	ctx->rx_done = 1;
}

static void tx_done(uint32_t dev_id)
{
	UART_DEV_CONTEXT *ctx = get_context(dev_id);
	ctx->tx_done = 1;
}

static void on_error(uint32_t dev_id)
{
	UART_DEV_CONTEXT *ctx = get_context(dev_id);
	ctx->tx_done = -1;
	ctx->rx_done = -1;
}

static int32_t set_dps(uint32_t dev_id, const UART_DPS_FORMAT *format)
{
	uint32_t param = 0;

	switch (format->databits) {
		case 6:
			param |= 0x1;
			break;
		case 7:
			param |= 0x2;
			break;
		case 8:
			param |= 0x3;
			break;
		default:
			return E_NOSPT;
	}

	if (format->stopbits == UART_STPBITS_ONE) {

	}
	else if (format->stopbits == UART_STPBITS_ONEHALF && format->databits == 5) {
		param |= 0x4;
	}
	else {
		return E_NOSPT;
	}

	if (format->parity != UART_PARITY_NONE) {
		param |= 0x8; // PEN

		if (format->parity == UART_PARITY_EVEN) {
			param |= 0x10; // EPS
		}
		else if (format->parity == UART_PARITY_MARK) {
			// PEN & Stick parity = logic 1
			param |= (0x20);
		} else if (format->parity == UART_PARITY_SPACE) {
			// PEN & EPS & Stick parity = logic 0
			param |= (0x10 | 0x20);
		}
	}

	io_uart_ioctl(dev_id, IO_UART_SET_LCR, &param);

	return E_OK;
}

static int32_t set_hwfc(uint32_t dev_id, uint32_t control)
{
	uint32_t param = 0;

	if (control == UART_FC_NONE) {
		param = 0;
	} else {
		param = 1;
	}

	io_uart_ioctl(dev_id, IO_UART_SET_FLOW_CONTROL, &param);

	return E_OK;
}

static int32_t set_baud(uint32_t dev_id, uint32_t baud)
{
	uint32_t param = 0;

	if (dev_id == 3) {
		uart3_clk_divisor(2);
		param = (BOARD_DFSS_AHB_CLK / 2) / (baud * 16) + 1;
	} else {
		param = BOARD_DFSS_IO_CLK / (baud * 16) + 1;
	}

	io_uart_ioctl(dev_id, IO_UART_SET_DLx, &param);

	return E_OK;
}

static int32_t uart_open(uint32_t dev_id, uint32_t baud)
{
	int32_t ret = E_OK;
	UART_DEV_CONTEXT *ctx = get_context(dev_id);
	DEV_UART_INFO *info = ctx->info;
	io_cb_t callback;


	info->opn_cnt++;

	if (info->opn_cnt > 1) {
		if (baud == info->baudrate) {
			return E_OK;
		} else {
			return E_OPNED;
		}
	}

	if (io_uart_open(dev_id)) {
		return E_SYS;
	}

	callback.cb = tx_done;
	io_uart_ioctl(dev_id, IO_SET_CB_TX, &callback);

	callback.cb = rx_done;
	io_uart_ioctl(dev_id, IO_SET_CB_RX, &callback);

	callback.cb = on_error;
	io_uart_ioctl(dev_id, IO_SET_CB_ERR, &callback);

	info->dps_format = dps_format_default;
	set_dps(dev_id, &info->dps_format);

	info->hwfc = hwfc_default;
	set_hwfc(dev_id, info->hwfc);


	ctx->rx_done = 1;
	ctx->tx_done = 1;

	set_baud(dev_id, baud);
	info->baudrate = baud;

	int_enable(ctx->intno);

	return ret;
}

static int32_t uart_close(uint32_t dev_id)
{
	UART_DEV_CONTEXT *ctx = get_context (dev_id);
	DEV_UART_INFO *info = ctx->info;

	info->opn_cnt--;
	if (info->opn_cnt == 0) {
		info->baudrate = 0;
		io_uart_close(dev_id);
		int_disable(ctx->intno);
	}

	return E_OK;
}

static int32_t uart_control(uint32_t dev_id, uint32_t ctrl_cmd, void *param)
{
	io_cb_t callback;
	DEV_BUFFER *devbuf;
	uint32_t int_val = (uint32_t)param;

	switch (ctrl_cmd) {
		case UART_CMD_SET_BAUD:
			return set_baud(dev_id, int_val);
		case UART_CMD_SET_HWFC:
			io_uart_ioctl(dev_id, IO_UART_SET_FLOW_CONTROL, param);
			return E_OK;
		case UART_CMD_SET_DPS_FORMAT:
			return set_dps(dev_id, (UART_DPS_FORMAT *)param);
		case UART_CMD_GET_RXAVAIL:
			*((uint32_t *)param) = 1;
			return  E_OK;
		case UART_CMD_SET_RXCB:
			callback.cb = param;
			io_uart_ioctl(dev_id, IO_SET_CB_RX, &callback);
			return E_OK;
		case UART_CMD_SET_TXCB:
			callback.cb = param;
			io_uart_ioctl(dev_id, IO_SET_CB_TX, &callback);
			return E_OK;
		case UART_CMD_SET_ERRCB:
			callback.cb = param;
			io_uart_ioctl(dev_id, IO_SET_CB_ERR, &callback);
			return E_OK;
		case UART_CMD_SET_TXINT:
			io_uart_tx_int(dev_id, int_val);
			return E_OK;
		case UART_CMD_SET_RXINT:
			io_uart_rx_int(dev_id, int_val);
			return E_OK;
		case UART_CMD_SET_TXINT_BUF:
			if (param != NULL) {
				devbuf = (DEV_BUFFER *)param;
				io_uart_write(dev_id, (uint8_t *)(devbuf->buf),
						 &(devbuf->len));
			} else {
				io_uart_write(dev_id, NULL, 0);
			}
			break;
		case UART_CMD_SET_RXINT_BUF:
			if (param != NULL) {
				devbuf = (DEV_BUFFER *)param;
				io_uart_read(dev_id, (uint8_t *)(devbuf->buf),
						 &(devbuf->len));
			} else {
				io_uart_read(dev_id, NULL, 0);
			}
			break;
		case UART_CMD_BREAK_SET:
		case UART_CMD_BREAK_CLR:
			return E_NOSPT;
		default:
			return E_NOSPT;
	}
	return E_OK;
}

static int32_t uart_write(uint32_t dev_id, const void *data, uint32_t len)
{
	UART_DEV_CONTEXT *ctx = get_context(dev_id);

	if (arc_locked() || arc_int_active()) {
		io_uart_poll_write(dev_id, (uint8_t *) data, &len);
	} else {
		ctx->tx_done = 0;
		io_uart_write(dev_id, (uint8_t *) data, &len);
		while (!ctx->tx_done);
	}

	return len;
}

static int32_t uart_read(uint32_t dev_id, void *data, uint32_t len)
{
	UART_DEV_CONTEXT *ctx = get_context(dev_id);

	if (arc_locked() || arc_int_active()) {
		io_uart_poll_read(dev_id, (uint8_t *) data, &len);
	} else {
		ctx->rx_done = 0;
		io_uart_read(dev_id, (uint8_t *) data, &len);
		while (!ctx->rx_done);
	}

	return len;
}

/*******************************************************************************/

#if (USE_DFSS_UART_0)

/**
 * \name	IOTDK DFSS UART 0 Object Instantiation
 * @{
 */

static DEV_UART dfss_uart_0; /*!< DFSS uart object */

static int32_t dfss_uart_0_open(uint32_t baud)
{
	return uart_open(0, baud);
}
/** DFSS uart 0 close */
static int32_t dfss_uart_0_close(void)
{
	return uart_close(0);
}
/** DFSS uart 0 control */

static int32_t dfss_uart_0_control(uint32_t ctrl_cmd, void *param)
{
	return uart_control(0, ctrl_cmd, param);
}
/** DFSS uart 0 write */
static int32_t dfss_uart_0_write(const void *data, uint32_t len)
{
	return uart_write(0, data, len);
}
/** DFSS uart 0 close */
static int32_t dfss_uart_0_read(void *data, uint32_t len)
{
	return uart_read(0, data, len);
}
/** install DFSS uart 0 to system */
static void dfss_uart_0_install(void)
{
	DEV_UART *dfss_uart_ptr = &dfss_uart_0;
	DEV_UART_INFO *dfss_uart_info_ptr = &(dfss_uart_0.uart_info);
	UART_DEV_CONTEXT *ctx = get_context(0);

	ctx->info = &(dfss_uart_0.uart_info);
	/** uart info init */
	dfss_uart_info_ptr->uart_ctrl = NULL;
	dfss_uart_info_ptr->opn_cnt = 0;
	dfss_uart_info_ptr->status = 0;
	dfss_uart_info_ptr->baudrate = UART_BAUDRATE_115200;  /* default 115200bps */

	/** uart dev init */
	dfss_uart_ptr->uart_open = dfss_uart_0_open;
	dfss_uart_ptr->uart_close = dfss_uart_0_close;
	dfss_uart_ptr->uart_control = dfss_uart_0_control;
	dfss_uart_ptr->uart_write = dfss_uart_0_write;
	dfss_uart_ptr->uart_read = dfss_uart_0_read;
}
#endif /* USE_DFSS_UART_0 */
/** @} end of name */

/*******************************************************************************/

#if USE_DFSS_UART_1

/**
 * \name	IOTDK DFSS UART 1 Object Instantiation
 * @{
 */

static DEV_UART dfss_uart_1; /*!< DFSS uart object */

static int32_t dfss_uart_1_open(uint32_t baud)
{
	return uart_open(1, baud);
}
/** DFSS uart 1 close */
static int32_t dfss_uart_1_close(void)
{
	return uart_close(1);
}
/** DFSS uart 1 control */

static int32_t dfss_uart_1_control(uint32_t ctrl_cmd, void *param)
{
	return uart_control(1, ctrl_cmd, param);
}
/** DFSS uart 1 write */
static int32_t dfss_uart_1_write(const void *data, uint32_t len)
{
	return uart_write(1, data, len);
}
/** DFSS uart 1 close */
static int32_t dfss_uart_1_read(void *data, uint32_t len)
{
	return uart_read(1, data, len);
}
/** install DFSS uart 1 to system */
static void dfss_uart_1_install(void)
{
	DEV_UART *dfss_uart_ptr = &dfss_uart_1;
	DEV_UART_INFO *dfss_uart_info_ptr = &(dfss_uart_1.uart_info);
	UART_DEV_CONTEXT *ctx = get_context(1);

	ctx->info = &(dfss_uart_1.uart_info);
	/** uart info init */
	dfss_uart_info_ptr->uart_ctrl = NULL;
	dfss_uart_info_ptr->opn_cnt = 0;
	dfss_uart_info_ptr->status = 0;
	dfss_uart_info_ptr->baudrate = UART_BAUDRATE_115200;  /* default 115200bps */

	/** uart dev init */
	dfss_uart_ptr->uart_open = dfss_uart_1_open;
	dfss_uart_ptr->uart_close = dfss_uart_1_close;
	dfss_uart_ptr->uart_control = dfss_uart_1_control;
	dfss_uart_ptr->uart_write = dfss_uart_1_write;
	dfss_uart_ptr->uart_read = dfss_uart_1_read;
}
#endif /* USE_DFSS_UART_1 */
/** @} end of name */

#if USE_DFSS_UART_2

/**
 * \name	IOTDK DFSS UART 2 Object Instantiation
 * @{
 */

static DEV_UART dfss_uart_2; /*!< DFSS uart object */

static int32_t dfss_uart_2_open(uint32_t baud)
{
	return uart_open(2, baud);
}
/** DFSS uart 2 close */
static int32_t dfss_uart_2_close(void)
{
	return uart_close(2);
}
/** DFSS uart 2 control */

static int32_t dfss_uart_2_control(uint32_t ctrl_cmd, void *param)
{
	return uart_control(2, ctrl_cmd, param);
}
/** DFSS uart 2 write */
static int32_t dfss_uart_2_write(const void *data, uint32_t len)
{
	return uart_write(2, data, len);
}
/** DFSS uart 2 close */
static int32_t dfss_uart_2_read(void *data, uint32_t len)
{
	return uart_read(2, data, len);
}
/** install DFSS uart 2 to system */
static void dfss_uart_2_install(void)
{
	DEV_UART *dfss_uart_ptr = &dfss_uart_2;
	DEV_UART_INFO *dfss_uart_info_ptr = &(dfss_uart_2.uart_info);
	UART_DEV_CONTEXT *ctx = get_context(2);

	ctx->info = &(dfss_uart_2.uart_info);
	/** uart info init */
	dfss_uart_info_ptr->uart_ctrl = NULL;
	dfss_uart_info_ptr->opn_cnt = 0;
	dfss_uart_info_ptr->status = 0;
	dfss_uart_info_ptr->baudrate = UART_BAUDRATE_115200;  /* default 115200bps */

	/** uart dev init */
	dfss_uart_ptr->uart_open = dfss_uart_2_open;
	dfss_uart_ptr->uart_close = dfss_uart_2_close;
	dfss_uart_ptr->uart_control = dfss_uart_2_control;
	dfss_uart_ptr->uart_write = dfss_uart_2_write;
	dfss_uart_ptr->uart_read = dfss_uart_2_read;
}
#endif /* USE_DFSS_UART_2 */
/** @} end of name */

#if USE_DFSS_UART_3

/**
 * \name	IOTDK DFSS UART 3 Object Instantiation
 * @{
 */

static DEV_UART dfss_uart_3; /*!< DFSS uart object */

static int32_t dfss_uart_3_open(uint32_t baud)
{
	return uart_open(3, baud);
}
/** DFSS uart 3 close */
static int32_t dfss_uart_3_close(void)
{
	return uart_close(3);
}
/** DFSS uart 3 control */

static int32_t dfss_uart_3_control(uint32_t ctrl_cmd, void *param)
{
	return uart_control(3, ctrl_cmd, param);
}
/** DFSS uart 3 write */
static int32_t dfss_uart_3_write(const void *data, uint32_t len)
{
	return uart_write(3, data, len);
}
/** DFSS uart 3 close */
static int32_t dfss_uart_3_read(void *data, uint32_t len)
{
	return uart_read(3, data, len);
}
/** install DFSS uart 3 to system */
static void dfss_uart_3_install(void)
{
	DEV_UART *dfss_uart_ptr = &dfss_uart_3;
	DEV_UART_INFO *dfss_uart_info_ptr = &(dfss_uart_3.uart_info);
	UART_DEV_CONTEXT *ctx = get_context(3);

	ctx->info = &(dfss_uart_3.uart_info);
	/** uart info init */
	dfss_uart_info_ptr->uart_ctrl = NULL;
	dfss_uart_info_ptr->opn_cnt = 0;
	dfss_uart_info_ptr->status = 0;
	dfss_uart_info_ptr->baudrate = UART_BAUDRATE_115200;  /* default 115200bps */

	/** uart dev init */
	dfss_uart_ptr->uart_open = dfss_uart_3_open;
	dfss_uart_ptr->uart_close = dfss_uart_3_close;
	dfss_uart_ptr->uart_control = dfss_uart_3_control;
	dfss_uart_ptr->uart_write = dfss_uart_3_write;
	dfss_uart_ptr->uart_read = dfss_uart_3_read;
}
#endif /* USE_DFSS_UART_3 */
/** @} end of name */


/** get one DFSS device structure */
DEV_UART_PTR uart_get_dev(int32_t uart_id)
{
	static uint32_t install_flag = 0;

	/* intall device objects */
	if (install_flag == 0) {
		install_flag = 1;
		dfss_uart_all_install();
	}

	switch (uart_id) {
#if USE_DFSS_UART_0
		case DFSS_UART_0_ID:
			return &dfss_uart_0;
			break;
#endif
#if USE_DFSS_UART_1
		case DFSS_UART_1_ID:
			return &dfss_uart_1;
			break;
#endif
#if USE_DFSS_UART_2
		case DFSS_UART_2_ID:
			return &dfss_uart_2;
			break;
#endif
#if USE_DFSS_UART_3
		case DFSS_UART_3_ID:
			return &dfss_uart_3;
			break;
#endif
		default:
			break;
	}
	return NULL;
}

/**
 * \brief	install all uart objects
 * \note	\b MUST be called during system init
 */
void dfss_uart_all_install(void)
{
#if (USE_DFSS_UART_0)
	dfss_uart_0_install();
#endif
#if (USE_DFSS_UART_1)
	dfss_uart_1_install();
#endif
#if (USE_DFSS_UART_2)
	dfss_uart_2_install();
#endif
#if (USE_DFSS_UART_3)
	dfss_uart_3_install();
#endif
}

/** @} end of group BOARD_IOTDK_DRV_DFSS_UART_OBJ */