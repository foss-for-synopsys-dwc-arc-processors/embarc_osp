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

#include "cmds_fs_cfg.h"
#if NTSHELL_USE_CMDS_FS_YMODEM
#include "cmd_fs_common.h"

#define YMODEM_USART_ID			BOARD_CONSOLE_UART_ID
#define YM_GET_CUR_MS			OSP_GET_CUR_MS

/** sleep ms */
void ym_sleep_ms(uint32_t ms)
{
	board_delay_ms(ms, OSP_DELAY_OS_COMPAT_ENABLE);
}

/** the interface of the ymodem: receive one byte */
int32_t ym_get_char(int32_t delay_s)
{
	int32_t num = 0, ch=0, delay_ms = delay_s * 1000;
	uint32_t cur_ms;
	DEV_UART *uart_obj = uart_get_dev(YMODEM_USART_ID);

	cur_ms = YM_GET_CUR_MS();
	do {
		/* check whether to receive any data */
		uart_obj->uart_control(UART_CMD_GET_RXAVAIL, (void *)&num);
		if(num > 0) {
			uart_obj->uart_read((void *)&ch, 1);
			return ch;
		}
	} while ((YM_GET_CUR_MS()-cur_ms) <= delay_ms);

	return -1;
}

/** the interface of the ymodem: send one byte */
int32_t ym_put_char(int32_t ch)
{
	int32_t num = 0, delay_times = 100;
	DEV_UART *uart_obj = uart_get_dev(YMODEM_USART_ID);
	uint32_t cur_ms;

	cur_ms = YM_GET_CUR_MS();
	do {
		/* check whether the usart is empty */
		uart_obj->uart_control(UART_CMD_GET_TXAVAIL, (void *)&num);
		if(num > 0) {
			uart_obj->uart_write((void *)&ch, 1);
			return ch;
		}
	} while ((YM_GET_CUR_MS()-cur_ms) <= delay_times);

	return -1;
}

/** open the destination of the the received file*/
int32_t ym_open_dest(const char *filename)
{
	uint8_t res = 0;
	/* Open the 'file_name' file with write operation */
	res = f_open(&cmd_files[0], filename, FA_WRITE | FA_OPEN_ALWAYS);
	return res;
}
/** close the destination of the the received file*/
int32_t ym_close_dest(void)
{
	uint8_t res = 0;
	/* Close the file */
	res = f_close(&cmd_files[0]);
	return res;
}

/** the output destination of the received file */
int32_t ym_output_dest(uint8_t *buf, int32_t length, const char *filename)
{
	uint32_t cnt = 0;
	/* Write data to the file */
	f_write(&cmd_files[0], buf, length, &cnt);
	return 0;
}

/** get the number bytes needed to send */
int32_t ym_get_size(const char *filename)
{
	uint8_t res = 0;
	int32_t file_size;

	/* Open the 'file_name' file with write operation */
	res = f_open(&cmd_files[0], filename, FA_READ);
	if (res != RES_OK) {
		file_size = -1;
	} else {
		/* Get the size of the file */
		file_size = f_size(&cmd_files[0]);
		/* Close the file */
		f_close(&cmd_files[0]);
	}

	return file_size;
}

/** get the data needed to send */
int32_t ym_get_data(char *buf, int32_t pt, int32_t length, const char *filename)
{
	uint8_t res = 0;
	uint32_t cnt = 0;

	/* Open the 'file_name' file with write operation */
	res = f_open(&cmd_files[0], filename, FA_READ);
	/* Move the pointer to the specified point */
	f_lseek(&cmd_files[0], pt);
	/* read data from file*/
	f_read(&cmd_files[0], buf, length, &cnt);
	/* Close the file */
	f_close(&cmd_files[0]);

	return cnt;
}

#endif /* NTSHELL_USE_CMDS_FS_YMODEM */
