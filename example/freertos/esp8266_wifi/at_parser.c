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

#include "embARC_error.h"
#include "string.h"
#include "board.h"
#include "at_parser.h"
//#include "embARC_error.h"

#define DBG_MORE
#include "embARC_debug.h"

/* if \r\n is needed to be attached at the end of AT comand, define AT_ADD_POSTFIX
*  otherwise comment out this line
*/
#define AT_ADD_POSTFIX

#define AT_PREFIX	"AT"
#define AT_POSTFIX	"\r\n"

#define AT_TIMENOW()	OSP_GET_CUR_MS()

#define AT_MAX_LEN 128
#define AT_MAX_PARAMETER 8


int32_t at_parser_init(AT_PARSER_DEF_PTR obj, uint32_t baudrate)
{
	obj->psio = ez_sio_open(obj->uart_id, baudrate, AT_TX_BUFSIZE, AT_RX_BUFSIZE);
	dbg_printf(DBG_MORE_INFO, "[%s]%d: obj->psio 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, obj->psio, *obj->psio);
	return (obj->psio != NULL) ? AT_OK : AT_ERROR;
}

void at_parser_deinit(AT_PARSER_DEF_PTR obj)
{
	ez_sio_close(obj->psio);
	return;
}

int32_t at_read(AT_PARSER_DEF_PTR obj, char *buf, uint32_t cnt)
{
	return ez_sio_read(obj->psio, buf, cnt);
}

int32_t at_write(AT_PARSER_DEF_PTR obj, char *buf, uint32_t cnt)
{
	return ez_sio_write(obj->psio, buf, cnt);
}

/*
* please use NULL as last parameter
*/
int32_t at_send_cmd(AT_PARSER_DEF_PTR obj, AT_MODE mode, AT_STRING command, ...)
{
	va_list vl;
	char at_out[AT_MAX_LEN] = AT_PREFIX;
	char *str = command;

	if (str == NULL) {
		dbg_printf(DBG_MORE_INFO, "[%s]%d: command is NULL, send AT test command\r\n", __FUNCTION__, __LINE__);
	} else {
		strcat(at_out,"+");
		strcat(at_out, command);

		switch (mode) {
		case AT_LIST:
			strcat(at_out, "=?");
			break;

		case AT_READ:
			strcat(at_out, "?");
			break;

		case AT_WRITE:
			strcat(at_out, "=");
			va_start(vl, command);

			for (int i = 0; i < AT_MAX_PARAMETER; i++) {
				str = va_arg(vl, AT_STRING);

				if (str == NULL) {
					break;
				}

				if (i != 0) {
					strcat(at_out, ",");
				}

				strcat(at_out, str);
			}

			va_end(vl);
			break;

		case AT_EXECUTE:
		default:
			break;
		}
	}

#ifdef AT_ADD_POSTFIX
	strcat(at_out, AT_POSTFIX);
#endif /*AT_ADD_POSTFIX*/
	dbg_printf(DBG_LESS_INFO, "[%s]%d: at_out: \"%s\" (%d)\r\n", __FUNCTION__, __LINE__, at_out, strlen(at_out));
	return at_write(obj, at_out, strlen(at_out));
}

/* make sure the buf is large enough*/
int32_t at_get_reply(AT_PARSER_DEF_PTR obj, char *buf, uint32_t timeout)
{
	uint32_t cur_ofs = 0;
	uint32_t read_cnt;
	uint32_t cur_time;
	cur_time = AT_TIMENOW();

	do {
		read_cnt = at_read(obj, &buf[cur_ofs], 1);
		cur_ofs += read_cnt;
		buf[cur_ofs] = '\0';

		if ((strstr(buf, AT_OK_STR)!= NULL) || (strstr(buf, AT_ERROR_STR)!= NULL)) {
			break;
		}
	} while ((AT_TIMENOW()-cur_time) < timeout);

	buf[cur_ofs] = '\0';
	dbg_printf(DBG_LESS_INFO, "[%s]%d: \"%s\" (%d)\r\n", __FUNCTION__, __LINE__, buf, strlen(buf));

	if (strstr(buf, AT_OK_STR)!= NULL) {
		return AT_OK;
	}

	return AT_ERROR;
}

int32_t at_test(AT_PARSER_DEF_PTR obj)
{
	char rcv_buf[64];
	at_send_cmd(obj, AT_LIST, NULL);
	return at_get_reply(obj, rcv_buf, AT_NORMAL_TIMEOUT);
}

