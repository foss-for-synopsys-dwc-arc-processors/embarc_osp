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

#include "dw_uart.h"
#include "nsim_uart_obj.h"

#include "nsim/nsim.h"

/**
 * NSIM UART 0 Object Instantiation
 */
#if (USE_NSIM_UART_0)
static void dw_uart_0_isr(void *ptr);
#define DW_UART_0_BASE   (0xf0000000)   /*!< designware uart 0 relative baseaddr */
#define DW_UART_0_INTNO  (24)           /*!< designware uart 0 interrupt number  */

static DEV_UART dw_uart_0;              /*!< designware uart object */
static DW_UART_CTRL dw_uart_0_ctrl = {  /*!< designware uart 0 ctrl */
	0, 1000000, DW_UART_0_INTNO, (INT_HANDLER_T)dw_uart_0_isr,
	1, 1, 0
};

/** designware uart 0 open */
static int32_t dw_uart_0_open(uint32_t baud)
{
	return dw_uart_open(&dw_uart_0, baud);
}
/** designware uart 0 close */
static int32_t dw_uart_0_close(void)
{
	return dw_uart_close(&dw_uart_0);
}
/** designware uart 0 control */
static int32_t dw_uart_0_control(uint32_t ctrl_cmd, void *param)
{
	return dw_uart_control(&dw_uart_0, ctrl_cmd, param);
}
/** designware uart 0 write */
static int32_t dw_uart_0_write(const void *data, uint32_t len)
{
	return dw_uart_write(&dw_uart_0, data, len);
}
/** designware uart 0 close */
static int32_t dw_uart_0_read(void *data, uint32_t len)
{
	return dw_uart_read(&dw_uart_0, data, len);
}
/** designware uart 0 interrupt rountine */
static void dw_uart_0_isr(void *ptr)
{
	dw_uart_isr(&dw_uart_0, ptr);
}
/** install designware uart 0 to system */
static void dw_uart_0_install(void)
{
	uint32_t uart_abs_base = 0;
	DEV_UART *dw_uart_ptr = &dw_uart_0;
	DEV_UART_INFO *dw_uart_info_ptr = &(dw_uart_0.uart_info);
	DW_UART_CTRL *dw_uart_ctrl_ptr = &dw_uart_0_ctrl;

	/**
	 * get absolute designware base address
	 */
	uart_abs_base = (uint32_t)DW_UART_0_BASE;
	dw_uart_ctrl_ptr->dw_uart_regbase = uart_abs_base;

	/** uart info init */
	dw_uart_info_ptr->uart_ctrl = (void *)dw_uart_ctrl_ptr;
	dw_uart_info_ptr->opn_cnt = 0;
	dw_uart_info_ptr->status = 0;
	dw_uart_info_ptr->baudrate = UART_BAUDRATE_115200;  /* default 115200bps */

	/** uart dev init */
	dw_uart_ptr->uart_open = dw_uart_0_open;
	dw_uart_ptr->uart_close = dw_uart_0_close;
	dw_uart_ptr->uart_control = dw_uart_0_control;
	dw_uart_ptr->uart_write = dw_uart_0_write;
	dw_uart_ptr->uart_read = dw_uart_0_read;

}
#endif /* USE_DW_UART_0 */

/**
 * @brief	install all uart objects
 * @note	@b MUST be called during system init
 */
static void nsim_uart_all_install(void)
{
#if (USE_NSIM_UART_0)
	dw_uart_0_install();
#endif
}

/** get one uart device structure */
DEV_UART_PTR uart_get_dev(int32_t uart_id)
{
	static uint32_t install_flag = 0;

	/* intall device objects */
	if (install_flag == 0) {
		install_flag = 1;
		nsim_uart_all_install();
	}

	switch (uart_id) {
#if (USE_NSIM_UART_0)
	case NSIM_UART_0_ID:
		return &dw_uart_0;
		break;
#endif
	default:
		break;
	}
	return NULL;
}
