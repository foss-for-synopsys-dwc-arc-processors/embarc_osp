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

#include "embARC.h"
#include "dw_uart_hal.h"
#include "dw_uart.h"

#define DW_UART_BAUD2DIV(perifreq, baud)	((perifreq) / ((baud)*16))

static DW_UART_REG_PTR  uart_reg_ptr = (DW_UART_REG_PTR) (0xF0009000U); // USB-UART as output

static void uart_init(void)
{
	uint32_t baud_divisor;

	baud_divisor = DW_UART_BAUD2DIV(CLK_BUS_APB, BOARD_CONSOLE_UART_BAUD);

	uart_reg_ptr->SRR = DW_UART_SRR_UR|DW_UART_SRR_RFR|DW_UART_SRR_XFR;
	while(uart_reg_ptr->USR & DW_UART_USR_BUSY); /* wait until software reset completed */

	uart_reg_ptr->MCR = 0;

	/* clear dps bits */
	uart_reg_ptr->LCR &= (~DW_UART_LCR_DPS_MASK);
	/* set dps bits */
	uart_reg_ptr->LCR |= (DW_UART_LCR_PARITY_NONE |  DW_UART_LCR_1_STOP_BIT | DW_UART_LCR_WORD_LEN8);


	/* enable uart baudrate update */
	uart_reg_ptr->LCR |= DW_UART_LCR_DLAB;
	/**
	 * setting uart baudrate registers
	 */
	uart_reg_ptr->DATA = baud_divisor & 0xff;	/*!< DLL */
	uart_reg_ptr->IER = (baud_divisor>>8) & 0xff;	/*!< DLH */
	/** disable DLAB */
	uart_reg_ptr->LCR &= ~(DW_UART_LCR_DLAB);

	uart_reg_ptr->IIR = 0x1;	/** enable uart fifo (FCR IIR is the same) */
	uart_reg_ptr->IER = 0x0;	/** disable all uart interrupt */
}


static void secure_console_putchar(char chr)
{
	while (!(uart_reg_ptr->USR & DW_UART_USR_TFNF));

	uart_reg_ptr->DATA = chr;
}

static int secure_console_getchar(void)
{
	while (!(uart_reg_ptr->USR & DW_UART_USR_RFNE));

	return (int) uart_reg_ptr->DATA;
}


void secure_console_init(void)
{
	uart_init();
	xdev_in(secure_console_getchar);
	xdev_out(secure_console_putchar);

	xprintf("Secureshield runtime starts to run\r\n");
}