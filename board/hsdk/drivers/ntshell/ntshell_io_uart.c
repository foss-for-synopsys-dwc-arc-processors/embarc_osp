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
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include <stdarg.h>

#ifdef MID_NTSHELL /* only available when enable ntshell middleware */
#include "dev_uart.h"
#include "xprintf.h"

#include "ntshell_io_uart.h"
#include "ntshell_task.h"

#include "hsdk/hsdk.h"

#ifdef ENABLE_OS
#include "os_hal_inc.h"
#endif

#define HSDK_NTSHELL_UART_CHECK_EXP(EXPR, ERROR_CODE)	CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

typedef struct ntshell_io_uart NTSHELL_IO_UART, *NTSHELL_IO_UART_PTR;

struct ntshell_io_uart {
	uint32_t uart_id;
	uint32_t uart_freq;
	uint32_t uart_mthd;
	void (*write_byte)(unsigned char byte);
};

static int32_t ntshell_uart_nt_ioinit(NTSHELL_IO *nt_io)
{
	int32_t ercd = E_OK;
	DEV_UART *uart_dev;
	NTSHELL_IO_UART *nt_uart;

	HSDK_NTSHELL_UART_CHECK_EXP(nt_io!=NULL, E_OBJ);
	HSDK_NTSHELL_UART_CHECK_EXP(nt_io->extra_info!=NULL, E_OBJ);

	nt_uart = (NTSHELL_IO_UART *)nt_io->extra_info;

	uart_dev = uart_get_dev(nt_uart->uart_id);

	HSDK_NTSHELL_UART_CHECK_EXP(uart_dev!=NULL, E_PAR);
	uart_dev->uart_open(nt_uart->uart_freq);

	ercd = E_OK;

error_exit:
	return ercd;
}

static int32_t ntshell_uart_nt_read(NTSHELL_IO *nt_io, void *buf, uint32_t cnt)
{
	int32_t ercd = E_OK;
	DEV_UART *uart_dev;
	NTSHELL_IO_UART *nt_uart;
	int32_t rd_avail = 0;

	HSDK_NTSHELL_UART_CHECK_EXP(nt_io!=NULL, E_OBJ);
	HSDK_NTSHELL_UART_CHECK_EXP(nt_io->extra_info!=NULL, E_OBJ);

	nt_uart = (NTSHELL_IO_UART *)nt_io->extra_info;
	uart_dev = uart_get_dev(nt_uart->uart_id);

	HSDK_NTSHELL_UART_CHECK_EXP(uart_dev!=NULL, E_PAR);

	ercd = cnt;
	do {
		uart_dev->uart_control(UART_CMD_GET_RXAVAIL, (void *)(&rd_avail));
		if (rd_avail > 0) {
			rd_avail = (rd_avail > cnt) ? cnt : rd_avail;
			uart_dev->uart_read(buf, rd_avail);
			buf = (void *)(((uint32_t)buf) + rd_avail);
			cnt -= rd_avail;
		} else {
#ifdef OS_FREERTOS
			vTaskDelay(5);
#endif
		}
	} while (cnt > 0);


error_exit:
	return ercd;
}

static int32_t ntshell_uart_nt_write(NTSHELL_IO *nt_io, const void *buf, uint32_t cnt)
{
	int32_t ercd = E_OK;
	DEV_UART *uart_dev;
	NTSHELL_IO_UART *nt_uart;

	HSDK_NTSHELL_UART_CHECK_EXP(nt_io!=NULL, E_OBJ);
	HSDK_NTSHELL_UART_CHECK_EXP(nt_io->extra_info!=NULL, E_OBJ);

	nt_uart = (NTSHELL_IO_UART *)nt_io->extra_info;
	uart_dev = uart_get_dev(nt_uart->uart_id);

	HSDK_NTSHELL_UART_CHECK_EXP(uart_dev!=NULL, E_PAR);

	ercd = (int32_t)uart_dev->uart_write(buf, cnt);

error_exit:
	return ercd;
}

static void ntshell_uart_nt_printf(NTSHELL_IO *nt_io, const char *fmt, va_list arp)
{
	int32_t ercd = E_OK;
	NTSHELL_IO_UART *nt_uart;
	void (*pf)(unsigned char);

	HSDK_NTSHELL_UART_CHECK_EXP(nt_io!=NULL, E_OBJ);
	HSDK_NTSHELL_UART_CHECK_EXP(nt_io->extra_info!=NULL, E_OBJ);

	nt_uart = (NTSHELL_IO_UART *)nt_io->extra_info;

	if (nt_uart->write_byte) {
		pf = xfunc_out;	 /* Save current output device */
		xfunc_out = nt_uart->write_byte;   /* Switch output to specified device */
		xvprintf(fmt, arp);
		xfunc_out = pf;	 /* Restore output device */
	}

error_exit:
	return ;
}

static void ntshell_uart_write_byte(NTSHELL_IO_UART *nt_uart, unsigned char byte)
{
	int32_t ercd = E_OK;
	DEV_UART *uart_dev;

	HSDK_NTSHELL_UART_CHECK_EXP(nt_uart!=NULL, E_OBJ);

	uart_dev = uart_get_dev(nt_uart->uart_id);
	HSDK_NTSHELL_UART_CHECK_EXP(uart_dev!=NULL, E_PAR);

	uart_dev->uart_write((const void *)&byte, 1);

error_exit:
	return ;
}


#if USE_HSDK_NTSHELL_UART_1
static int32_t ntshell_uart_1_nt_ioinit(void)
{
	return ntshell_uart_nt_ioinit(&ntshell_uart_1);
}

static int32_t ntshell_uart_1_nt_read(void *buf, uint32_t cnt)
{
	return ntshell_uart_nt_read(&ntshell_uart_1, buf, cnt);
}

static int32_t ntshell_uart_1_nt_write(const void *buf, uint32_t cnt)
{
	return ntshell_uart_nt_write(&ntshell_uart_1, buf, cnt);
}

static void ntshell_uart_1_nt_printf(const char *fmt, ...)
{
	va_list arp;

	va_start(arp, fmt);
	ntshell_uart_nt_printf(&ntshell_uart_1, fmt, arp);
	va_end(arp);
}

static void ntshell_uart_1_write_byte(unsigned char byte);

static ntshell_t  uart_ntshell_1;
static NTSHELL_IO_UART ntshell_uart_1_info = {
	BOARD_CONSOLE_UART_ID, UART_BAUDRATE_115200, DEV_POLL_METHOD, \
	ntshell_uart_1_write_byte
};

static void ntshell_uart_1_write_byte(unsigned char byte)
{
	ntshell_uart_write_byte(&ntshell_uart_1_info, byte);
}

NTSHELL_IO ntshell_uart_1 = {
	&uart_ntshell_1, \
	&help_cmd, \
	NULL, \
	"COM1>", \
	(void *)(&ntshell_uart_1_info), \
	ntshell_uart_1_nt_ioinit, \
	ntshell_uart_1_nt_read, \
	ntshell_uart_1_nt_write, \
	ntshell_uart_1_nt_printf
};

#endif /** USE_HSDK_NTSHELL_UART_1 */

#endif /** MID_NTSHELL */
