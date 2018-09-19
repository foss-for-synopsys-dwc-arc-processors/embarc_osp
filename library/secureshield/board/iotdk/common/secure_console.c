/*------------------------------------------
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
#undef LIB_SECURESHIELD_OVERRIDES
#include "embARC.h"

/* APEX UART device registers  */
#define UART_RBR    (0x00)
#define UART_THR    (0x00)
#define UART_DLL    (0x00)
#define UART_IER    (0x04)
#define UART_DLH    (0x04)
#define UART_IIR    (0x08)
#define UART_FCR    (0x08)
#define UART_LCR    (0x0c)
#define UART_MCR    (0x10)
#define UART_LSR    (0x14)  // unused
#define UART_MSR    (0x18)  // unused
#define UART_USR    (0x7c)  // unused
#define UART_CLKEN  (0xc0)

/* use uart 0 as secure console */
#define SECURE_UART_REG_BASE AR_IO_UART0_RBR_THR_DLL

#define REG_READ(x)		_arc_aux_read((SECURE_UART_REG_BASE + x))
#define REG_WRITE(x, y)		_arc_aux_write((SECURE_UART_REG_BASE + x), y)

#define SS_UART_BAUD2DIV(perifreq, baud) ((perifreq) / ((baud)*16) + 1)

#define UART_FORMAT 0x3 /* 8 bits, 1 stop bit, no parity */

static void uart_init(void)
{
	uint32_t baud;

	baud = SS_UART_BAUD2DIV(BOARD_DFSS_IO_CLK, 115200);
	REG_WRITE(UART_CLKEN, 0x1);
	REG_WRITE(UART_IER, 0x0);
	REG_WRITE(UART_FCR, 0x7);
	REG_WRITE(UART_LCR, UART_FORMAT | 0x80);
	REG_WRITE(UART_DLL, (baud & 0x00ff));
	REG_WRITE(UART_DLH, (baud & 0xff00) >> 8);
	REG_WRITE(UART_LCR, UART_FORMAT);
}

static void secure_console_putchar(char chr)
{
	uint32_t old_val;
	/* disbale uart interrupt */
	old_val = REG_READ(UART_IER);
	REG_WRITE(UART_IER, 0x80);

	while ((REG_READ(UART_LSR) & 0x20)); // wait THR empty

	REG_WRITE(UART_THR, chr);

	REG_WRITE(UART_IER, old_val);
}

static int secure_console_getchar(void)
{
	uint32_t old_val;
	int data;
	/* disbale uart interrupt */
	old_val = REG_READ(UART_IER);
	REG_WRITE(UART_IER, 0x80);

	while (!(REG_READ(UART_LSR) & 0x1)); // wait data ready

	data = REG_READ(UART_RBR);

	REG_WRITE(UART_IER, old_val);

	return data;
}

void secure_console_init(void)
{
	uart_init();
	xdev_in(secure_console_getchar);
	xdev_out(secure_console_putchar);

	xprintf("Secureshield runtime starts to run\r\n");
}