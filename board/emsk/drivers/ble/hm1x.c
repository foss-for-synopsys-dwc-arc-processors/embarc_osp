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
 * \version 2017.03
 * \date 2015-04-24
 * \author Huaqi Fang(Huaqi.Fang@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	BOARD_EMSK_DRV_BLE_HM1X		EMSK BLE-HM1X Driver
 * \ingroup	BOARD_EMSK_DRV_BLE
 * \brief	EMSK HM1X BLE driver
 * \details
 *		Implement the hm1x ble driver using the DesignWare UART device driver.
 */

/**
 * \file
 * \ingroup	BOARD_EMSK_DRV_BLE_HM1X
 * \brief	HM-1X BLE driver
 * \details	Implement hm-1x ble driver.
 */

/**
 * \addtogroup	BOARD_EMSK_DRV_BLE_HM1X
 * @{
 */

#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "stdio.h"
#include "string.h"

#include "dev_uart.h"
#include "board.h"

DEV_UART *hm1x_ble_uart = NULL;

/** uart id which HM-10 BLE uses */
#define HM_1X_UART_ID		DW_UART_0_ID

#define HM1X_TIMENOW()		OSP_GET_CUR_MS()

#define HM1X_READ_TMOUT		500

#define MAX_CMD_LENGTH		32
#define MAX_RESP_LENGTH		32

#define AT_CMD_PREFIX		"AT+"

#define BAUD_INDEX_MAX		8
#define BAUD_INDEX_MIN		0

#define HM1X_SYS_DELAY_MS	550

#define ENABLE_HM1X_DEBUG

#ifdef ENABLE_HM1X_DEBUG
#include "embARC_debug.h"
#define HM1X_DEBUG(fmt, ...)	EMBARC_PRINTF(fmt, ##__VA_ARGS__)
#else
#define HM1X_DEBUG(fmt, ...)
#endif

/** Internal delay function used by hm1x */
static void hm1x_delay(uint32_t ms)
{
	board_delay_ms(ms, OSP_DELAY_OS_COMPAT_DISABLE);
}

#define MAX_READ_CNT		20
/** Flush the hm1x input data */
void hm1x_flush(void)
{
	uint32_t rd_cnt = 0;
	uint8_t read_chr[MAX_READ_CNT];
	uint32_t rd_avail;

	hm1x_ble_uart->uart_control(UART_CMD_GET_RXAVAIL, (void *)(&rd_avail));
	do {
		rd_cnt = (rd_avail>MAX_READ_CNT)?MAX_READ_CNT:rd_avail;
		if (rd_avail > 0) {
			hm1x_ble_uart->uart_read((void *)read_chr, rd_cnt);
		}
		hm1x_ble_uart->uart_control(UART_CMD_GET_RXAVAIL, (void *)(&rd_avail));
	} while(rd_avail > 0);
}

/**
 * \brief	Init HM1X BLE device
 * \param  baudrate	ble working baudrate
 * \retval 0	success
 * \retval -1	fail
 */
int32_t hm1x_init(uint32_t baudrate)
{
	hm1x_ble_uart = uart_get_dev(HM_1X_UART_ID);

	if (hm1x_ble_uart == NULL) return -1;

	if (hm1x_ble_uart->uart_open(baudrate) == E_OPNED) {
		hm1x_ble_uart->uart_control(UART_CMD_SET_BAUD, (void *)(baudrate));
	}

	return 0;
}

/**
 * \brief	Write data to HM1X
 * \param  buf	Buffer to be written
 * \param  cnt	Number to be written
 * \retval 0 	Failed to write
 * \retval >0	Written count
 */
uint32_t hm1x_write(uint8_t *buf, uint32_t cnt)
{
	if (hm1x_ble_uart == NULL) return 0;

	return hm1x_ble_uart->uart_write(buf, cnt);
}

/**
 * \brief	Read data from HM1X
 * \param  buf	Buffer to be read to
 * \param  cnt	Number to be read
 * \retval 0	Failed to read
 * \retval >0	Read count
 */
uint32_t hm1x_read(uint8_t *buf, uint32_t cnt)
{
	if (hm1x_ble_uart == NULL) return 0;

	uint32_t rd_avail;
	hm1x_ble_uart->uart_control(UART_CMD_GET_RXAVAIL, (void *)(&rd_avail));
	cnt = (cnt > rd_avail)?rd_avail:cnt;
	if (cnt > 0) {
		return hm1x_ble_uart->uart_read(buf, cnt);
	}
	return cnt;
}


/**
 * \brief	Execute command and wait for response with timeout
 * \param  cmd		Command to send to the BLE device using the UART
 * \param  resp		Response buffer, must not be NULL
 * \param  wait_ms	Maximum MS to wait for response
 * \retval -1	Fail
 * \retval >=0	Response string count
 */
int32_t hm1x_exec_command(char *cmd, char *resp, uint32_t wait_ms)
{
	uint32_t cur_ofs = 0;
	uint32_t read_cnt;
	uint32_t cur_time;

	if ((cmd == NULL) || (resp == NULL))  return -1;

	if (hm1x_write((uint8_t *)cmd, strlen(cmd)) > 0) {
		cur_time = HM1X_TIMENOW();
		do {
			read_cnt = hm1x_read((uint8_t *)(&resp[cur_ofs]), 2);
			cur_ofs += read_cnt;
		} while((HM1X_TIMENOW()-cur_time) < wait_ms);
		resp[cur_ofs] = '\0';
		return cur_ofs;
	}
	return -1;
}

/**
 * \brief	Execute command and check whether the response prefix matches
 * \param  cmd		Command to send to the BLE device using the UART
 * \param  resp_prefix	Response prefix string, like OK. Must not be NULL
 * \param  resp_data	Response data buffer, may be NULL
 * \param  max_try	How many times this function can try to run
 * \retval	-1	Fail
 * \retval	>=0	Response data length
 */
int32_t hm1x_exec_cmd_chkresp(char *cmd, char *resp_prefix, char *resp_data, uint32_t max_try)
{
	char resp[MAX_RESP_LENGTH];
	uint32_t try_cnt = 0;
	int32_t rd_cnt;
	int32_t resp_prefix_len = 0;
	int32_t resp_data_len = -1;

	if (cmd == NULL)  return -1;

	if (resp_prefix != NULL) {
		resp_prefix_len = strlen(resp_prefix);
	}

	do {
		rd_cnt = hm1x_exec_command(cmd, resp, HM1X_READ_TMOUT);
		if (rd_cnt >= resp_prefix_len) {
			if (resp_prefix_len > 0) {
				if (strncmp(resp, resp_prefix, resp_prefix_len) != 0) {
					continue;
				}
			}
			if (resp_data == NULL) {
				resp_data_len = 0;
			} else {
				resp_data_len = rd_cnt - resp_prefix_len;
				if (resp_data_len > 0) {
					strcpy(resp_data, &resp[resp_prefix_len]);
				}
			}
			break;
		}
	} while (try_cnt++ < max_try);

	return resp_data_len;
}

#define OK_SET_PREFIX		"OK+Set:"
/** HM1X set param, value and param must end with '\0', 0 for success, -1 for fail */
int32_t hm1x_set_param(char *param, char *val)
{
	char cmd_buf[MAX_CMD_LENGTH];
	uint32_t cmd_length = 0;

	if ((param == NULL) || (val == NULL)) return -1;

	cmd_length = strlen(param) + strlen(val) + strlen(AT_CMD_PREFIX);
	if (cmd_length > MAX_CMD_LENGTH) {
		HM1X_DEBUG("HM1X command is too long to execute!\r\n");
		return -1;
	}
	sprintf(cmd_buf, "%s%s%s", AT_CMD_PREFIX, param, val);

	if (hm1x_exec_cmd_chkresp(cmd_buf, OK_SET_PREFIX, NULL, 2) != -1) {
		return 0;
	} else {
		return -1;
	}
}

#define OK_GET_PREFIX		"OK+Get:"
/** HM1X get param, cmd and val must contain '\0' end, >=0 for success, <0 for fail  */
int32_t hm1x_get_param(char *param, char *r_val)
{
	char cmd_buf[MAX_CMD_LENGTH];
	uint32_t cmd_length = 0;

	if ((param == NULL) || (r_val == NULL)) return -1;

	cmd_length = strlen(param) + 1 + strlen(AT_CMD_PREFIX);
	if (cmd_length > MAX_CMD_LENGTH) {
		HM1X_DEBUG("HM1X command is too long to execute!\r\n");
		return -1;
	}

	sprintf(cmd_buf, "%s%s?", AT_CMD_PREFIX, param);

	return hm1x_exec_cmd_chkresp(cmd_buf, OK_GET_PREFIX, r_val, 2);
}

/** set a 8bits long parameter of hm1x, 0 for success, -1 for fail */
int32_t hm1x_set_param_u8(char *param, uint8_t val)
{
	char buf[2];

	buf[0] = val + '0';
	buf[1] = '\0';

	return hm1x_set_param(param, buf);
}

/** set working role, 0 for success, -1 for fail */
int32_t hm1x_set_role(uint32_t role)
{
	if (hm1x_set_param_u8("ROLE", role) == 0) {
		hm1x_delay(HM1X_SYS_DELAY_MS);
		return 0;
	} else {
		return -1;
	}
}

/** set working mode, 0 for success, -1 for fail */
int32_t hm1x_set_mode(uint32_t mode)
{
	return hm1x_set_param_u8("ROLE", mode);
}

/** set working type, 0 for success, -1 for fail */
int32_t hm1x_set_type(uint32_t type)
{
	return hm1x_set_param_u8("TYPE", type);
}

#define CMD_BAUD_SET_PREFIX	"AT+BAUD"
/** set baudrate of hm1x, >=0 for success, -1 for fail */
int32_t hm1x_set_baud(uint32_t baudrate)
{
	int32_t baud_idx;
	uint32_t prefix_len = strlen(CMD_BAUD_SET_PREFIX);
	char cmd_buf[10] = CMD_BAUD_SET_PREFIX;

	baud_idx = hm1x_baudrate_map(baudrate);

	if (baud_idx == -1) return -1;
	cmd_buf[prefix_len]=baud_idx+'0';
	cmd_buf[prefix_len+1]='\0';

	return hm1x_exec_cmd_chkresp(cmd_buf, "OK", NULL, 2);
}

/** disconnect the already connected ble, >=0 for success, <0 for fail */
int32_t hm1x_disconnect(void)
{
	return hm1x_exec_cmd_chkresp("AT", "OK", NULL, 2);
}

/** reset and restart the module, >=0 for success, <0 for fail */
int32_t hm1x_restart(void)
{
	if (hm1x_exec_cmd_chkresp("AT+RESET", "OK", NULL, 2) == 0) {
		hm1x_delay(HM1X_SYS_DELAY_MS);
		return 0;
	} else {
		return -1;
	}
}

/** renew the module, >=0 for success, <0 for fail */
int32_t hm1x_renew(void)
{
	return hm1x_exec_cmd_chkresp("AT+RENEW", "OK", NULL, 2);
}

/** get the baudrate map of hm1x, return the index of baudrate map in hm1x */
int32_t hm1x_baudrate_map(uint32_t baudrate)
{
	int32_t baud_idx = -1;

	switch (baudrate) {
		case 1200:
			baud_idx  = 7;
			break;
		case 2400:
			baud_idx  = 6;
			break;
		case 4800:
			baud_idx  = 5;
			break;
		case 9600:
			baud_idx  = 0;
			break;
		case 19200:
			baud_idx  = 1;
			break;
		case 38400:
			baud_idx  = 2;
			break;
		case 57600:
			baud_idx  = 3;
			break;
		case 115200:
			baud_idx  = 4;
			break;
		case 230400:
			baud_idx  = 8;
			break;
		default:
			baud_idx  = -1;
			break;
	}
	return baud_idx;
}

/** @} end of group BOARD_EMSK_DRV_BLE_HM1X */
