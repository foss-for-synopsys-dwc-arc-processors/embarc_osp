/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "iotdk_hardware.h"

#include "dfss_uart_obj.h"
#include "ip/subsystem/uart/ss_uart.h"

/*******************************************************************************/

#if (USE_DFSS_UART_0)

static void uart_tx_cb0(void *param);
static void uart_rx_cb0(void *param);
static void uart_err_cb0(void *param);

static DEV_UART dfss_uart_0;

static SS_UART_DEV_CONTEXT uart_context0 = {
	AR_IO_UART0_RBR_THR_DLL,
	DFSS_UART_0_ID,
	IO_UART0_INTR,
	0,
	BOARD_DFSS_IO_CLK,
	uart_tx_cb0,
	uart_rx_cb0,
	uart_err_cb0,
};

static void uart_tx_cb0(void *param)
{
	ss_uart_tx_cb(&uart_context0, param);
}

static void uart_rx_cb0(void *param)
{
	ss_uart_rx_cb(&uart_context0, param);
}

static void uart_err_cb0(void *param)
{
	ss_uart_err_cb(&uart_context0, param);
}

static int32_t dfss_uart_0_open(uint32_t baud)
{
	return ss_uart_open(&uart_context0, baud);
}

static int32_t dfss_uart_0_close(void)
{
	return ss_uart_close(&uart_context0);
}

static int32_t dfss_uart_0_control(uint32_t ctrl_cmd, void *param)
{
	return ss_uart_control(&uart_context0, ctrl_cmd, param);
}

static int32_t dfss_uart_0_write(const void *data, uint32_t len)
{
	return ss_uart_write(&uart_context0, data, len);
}

static int32_t dfss_uart_0_read(void *data, uint32_t len)
{
	return ss_uart_read(&uart_context0, data, len);
}

static void dfss_uart_0_install(void)
{
	DEV_UART *dfss_uart_ptr = &dfss_uart_0;
	DEV_UART_INFO *dfss_uart_info_ptr = &(dfss_uart_0.uart_info);


	uart_context0.info = &(dfss_uart_0.uart_info);
	/** uart info init */
	dfss_uart_info_ptr->uart_ctrl = (void *)&uart_context0;
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

/*******************************************************************************/

#if USE_DFSS_UART_1

static void uart_tx_cb1(void *param);
static void uart_rx_cb1(void *param);
static void uart_err_cb1(void *param);

static DEV_UART dfss_uart_1;

static SS_UART_DEV_CONTEXT uart_context1 = {
	AR_IO_UART1_RBR_THR_DLL,
	DFSS_UART_1_ID,
	IO_UART1_INTR,
	0,
	BOARD_DFSS_IO_CLK,
	uart_tx_cb1,
	uart_rx_cb1,
	uart_err_cb1,
};

static void uart_tx_cb1(void *param)
{
	ss_uart_tx_cb(&uart_context1, param);
}

static void uart_rx_cb1(void *param)
{
	ss_uart_rx_cb(&uart_context1, param);
}

static void uart_err_cb1(void *param)
{
	ss_uart_err_cb(&uart_context1, param);
}

static int32_t dfss_uart_1_open(uint32_t baud)
{
	return ss_uart_open(&uart_context1, baud);
}

static int32_t dfss_uart_1_close(void)
{
	return ss_uart_close(&uart_context1);
}

static int32_t dfss_uart_1_control(uint32_t ctrl_cmd, void *param)
{
	return ss_uart_control(&uart_context1, ctrl_cmd, param);
}

static int32_t dfss_uart_1_write(const void *data, uint32_t len)
{
	return ss_uart_write(&uart_context1, data, len);
}

static int32_t dfss_uart_1_read(void *data, uint32_t len)
{
	return ss_uart_read(&uart_context1, data, len);
}

static void dfss_uart_1_install(void)
{
	DEV_UART *dfss_uart_ptr = &dfss_uart_1;
	DEV_UART_INFO *dfss_uart_info_ptr = &(dfss_uart_1.uart_info);


	uart_context1.info = &(dfss_uart_1.uart_info);
	/** uart info init */
	dfss_uart_info_ptr->uart_ctrl = (void *)&uart_context1;;
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

#if USE_DFSS_UART_2

static void uart_tx_cb2(void *param);
static void uart_rx_cb2(void *param);
static void uart_err_cb2(void *param);

static DEV_UART dfss_uart_2;

static SS_UART_DEV_CONTEXT uart_context2 = {
	AR_IO_UART2_RBR_THR_DLL,
	DFSS_UART_2_ID,
	IO_UART2_INTR,
	0,
	BOARD_DFSS_IO_CLK,
	uart_tx_cb2,
	uart_rx_cb2,
	uart_err_cb2,
};

static void uart_tx_cb2(void *param)
{
	ss_uart_tx_cb(&uart_context2, param);
}

static void uart_rx_cb2(void *param)
{
	ss_uart_rx_cb(&uart_context2, param);
}

static void uart_err_cb2(void *param)
{
	ss_uart_err_cb(&uart_context2, param);
}

static int32_t dfss_uart_2_open(uint32_t baud)
{
	return ss_uart_open(&uart_context2, baud);
}

static int32_t dfss_uart_2_close(void)
{
	return ss_uart_close(&uart_context2);
}

static int32_t dfss_uart_2_control(uint32_t ctrl_cmd, void *param)
{
	return ss_uart_control(&uart_context2, ctrl_cmd, param);
}

static int32_t dfss_uart_2_write(const void *data, uint32_t len)
{
	return ss_uart_write(&uart_context2, data, len);
}

static int32_t dfss_uart_2_read(void *data, uint32_t len)
{
	return ss_uart_read(&uart_context2, data, len);
}

static void dfss_uart_2_install(void)
{
	DEV_UART *dfss_uart_ptr = &dfss_uart_2;
	DEV_UART_INFO *dfss_uart_info_ptr = &(dfss_uart_2.uart_info);

	uart_context2.info = &(dfss_uart_2.uart_info);
	/** uart info init */
	dfss_uart_info_ptr->uart_ctrl = (void *)&uart_context2;
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


#if USE_DFSS_UART_3

static void uart_tx_cb3(void *param);
static void uart_rx_cb3(void *param);
static void uart_err_cb3(void *param);

static DEV_UART dfss_uart_3;

static SS_UART_DEV_CONTEXT uart_context3 = {
	AR_IO_UART3_RBR_THR_DLL,
	DFSS_UART_3_ID,
	IO_UART3_INTR,
	0,
	BOARD_DFSS_AHB_CLK,
	uart_tx_cb3,
	uart_rx_cb3,
	uart_err_cb3,
};

static void uart_tx_cb3(void *param)
{
	ss_uart_tx_cb(&uart_context3, param);
}

static void uart_rx_cb3(void *param)
{
	ss_uart_rx_cb(&uart_context3, param);
}

static void uart_err_cb3(void *param)
{
	ss_uart_err_cb(&uart_context3, param);
}

static int32_t dfss_uart_3_open(uint32_t baud)
{
	return ss_uart_open(&uart_context3, baud);
}

static int32_t dfss_uart_3_close(void)
{
	return ss_uart_close(&uart_context3);
}

static int32_t dfss_uart_3_control(uint32_t ctrl_cmd, void *param)
{
	return ss_uart_control(&uart_context3, ctrl_cmd, param);
}

static int32_t dfss_uart_3_write(const void *data, uint32_t len)
{
	return ss_uart_write(&uart_context3, data, len);
}

static int32_t dfss_uart_3_read(void *data, uint32_t len)
{
	return ss_uart_read(&uart_context3, data, len);
}

static void dfss_uart_3_install(void)
{
	DEV_UART *dfss_uart_ptr = &dfss_uart_3;
	DEV_UART_INFO *dfss_uart_info_ptr = &(dfss_uart_3.uart_info);

	uart_context3.info = &(dfss_uart_3.uart_info);
	/** uart info init */
	dfss_uart_info_ptr->uart_ctrl = (void *)&uart_context3;
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
