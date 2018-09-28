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
#define DEBUG
#include "embARC.h"
#include "embARC_debug.h"
#include "dev_gpio.h"
#include "ez_sio.h"

#include "rn4020.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#ifdef RN4020_INTERRUPT_DRIVERN
static RN4020_DEF_PTR rn4020_ptr;
#endif

static void _rn4020_set_state(RN4020_DEF_PTR rn4020, RN4020_STATE new_state)
{
#ifdef DEBUG

	switch (new_state) {
		case RN4020_STATE_INITIALIZING:
			DBG("state: INITIALIZING\n");
			break;

		case RN4020_STATE_READY:
			DBG("state: READY\n");
			break;

		case RN4020_STATE_WAITING_FOR_CMD:
			DBG("state: WAITING_FOR_CMD\n");
			break;

		case RN4020_STATE_WAITING_FOR_AOK:
			DBG("state: WAITING_FOR_AOK\n");
			break;

		case RN4020_STATE_WAITING_FOR_RESET:
			DBG("state: WAITING_FOR_RESET\n");
			break;

		case RN4020_STATE_WAITING_FOR_LS:
			DBG("state: WAITING_FOR_LS\n");
			break;

		default:
			DBG("state: unknown\n");
			break;
	}

#endif
	rn4020->state = new_state;
}

static void _rn4020_get_line(RN4020_DEF_PTR rn4020)
{
	char data = 0;
	uint8_t rd_cnt;

	do {
		rd_cnt = ez_sio_read(rn4020->sio_uart, &data, 1);

		if (rd_cnt != 0 && data != 0) {
			rn4020->rx_buffer[rn4020->rx_cnt++] = data;
		}
	} while (data != '\n' && rn4020->rx_cnt < RN4020_RX_BUFFER_SIZE);

	if (rn4020->rx_buffer[rn4020->rx_cnt - 2] == '\r') {
		/* remove \r\n */
		rn4020->rx_buffer[rn4020->rx_cnt - 2] = 0;
	} else {
		rn4020->rx_buffer[rn4020->rx_cnt - 1] = 0;
	}
}

static void _rn4020_parse_uuid_str(const char *str, uint8_t str_len, uint8_t *uuid, uint8_t *uuid_len)
{
	char temp[3];
	uint8_t str_index, dest_index;

	for (str_index = 0, dest_index = 0; str_index < str_len; str_index += 2) {
		temp[0] = str[str_index];
		temp[1] = str[str_index + 1];
		temp[2] = '\0';
		uuid[dest_index++] = strtol(temp, NULL, 16);
	}

	*uuid_len = dest_index;
}

static uint8_t _rn4020_parse_handle_uuid_line(const char *line, RN4020_HANDLE_UUID_ITEM *item)
{
	const char *uuid_ptr;
	const char *comma_ptr;
	char handle[5];

	/* skip two spaces */
	if (strncmp(line, "  ", 2) != 0) {
		return false;
	}

	uuid_ptr = line + 2;
	comma_ptr = strchr(uuid_ptr, ',');

	_rn4020_parse_uuid_str(uuid_ptr, comma_ptr - uuid_ptr,
	                       item->uuid, &item->uuid_len);

	/* \todo 128bit uuid support */
	strncpy(handle, comma_ptr + 1, 4);
	handle[4] = 0;
	item->handle = strtol(handle, NULL, 16);

	return true;
}

static void _rn4020_set_connected(RN4020_DEF_PTR rn4020, uint8_t connected)
{
	rn4020->connected = connected;
	rn4020_connected_changed(rn4020, connected);
}

static void _rn4020_line_parser(RN4020_DEF_PTR rn4020, const char *line)
{
	char handle_str[5];
	uint16_t handle;

	DBG("rx:%s\n", line);

	/*  connection notification from client */
	if (strcmp(line, "Connected") == 0) {
		_rn4020_set_connected(rn4020, true);
		return;
	}

	if (strcmp(line, "Connection End") == 0) {
		_rn4020_set_connected(rn4020, false);
		rn4020->need_advertise = 1;
		return;
	}

	if (strncmp(line, "RV,", 3) == 0) {
		strncpy(handle_str, line + 3, 4);
		handle_str[4] = '\0';
		handle = strtol(handle_str, NULL, 16);
		rn4020_on_realtime_read(rn4020, handle);
		return;
	}

	if (strncmp(line, "WV,", 3) == 0) {
		strncpy(handle_str, line + 3, 4);
		handle_str[4] = '\0';
		handle = strtol(handle_str, NULL, 16);
		rn4020_on_write(rn4020, handle, (uint8_t *)(line + 8));
		return;
	}

	switch (rn4020->state) {
		case RN4020_STATE_INITIALIZING:
		case RN4020_STATE_WAITING_FOR_CMD:
			if (strcmp(line, "CMD") == 0) {
				_rn4020_set_state(rn4020, RN4020_STATE_READY);
				return;
			}

			if (strcmp(line, "Reboot") == 0) {
				return;
			}

			break;

		case RN4020_STATE_WAITING_FOR_AOK:
			if (strcmp(line, "AOK") == 0) {
				_rn4020_set_state(rn4020, RN4020_STATE_READY);
				return;
			}

			break;

		case RN4020_STATE_WAITING_FOR_RESET:
			if (strcmp(line, "Reboot") == 0) {
				_rn4020_set_state(rn4020, RN4020_STATE_WAITING_FOR_CMD);
				return;
			}

			break;

		case RN4020_STATE_WAITING_FOR_LS:
			if (_rn4020_parse_handle_uuid_line(line,
			                                   &rn4020->handle_uuid_table[rn4020->handle_uuid_cnt])) {
				rn4020->handle_uuid_cnt++;
				return;
			} else if (strcmp(line, "END") == 0) {
				_rn4020_set_state(rn4020, RN4020_STATE_READY);
				return;
			} else if (strlen(line) == 4 || strlen(line) == RN4020_PRIVATE_UUID_HEX_STRING_LENGTH) {
				DBG("service uuid, skip it\r\n");
				return;
			}

			break;

		case RN4020_STATE_READY:
			break;
	}

	DBG("unexpected line: %s\n", line);
}

static int32_t _rn4020_wait_for_ready(RN4020_DEF_PTR rn4020)
{
	if (rn4020->state == RN4020_STATE_READY) {
		return E_OK;
	}

	while (1) {
#ifndef RN4020_INTERRUPT_DRIVERN
		_rn4020_get_line(rn4020);
		_rn4020_line_parser(rn4020, (char *)rn4020->rx_buffer);
		rn4020->rx_cnt = 0;
#endif

		if (rn4020->state == RN4020_STATE_READY) {
			return E_OK;
		}
	}

	return E_SYS;
}

static int32_t _rn4020_cmd_aok(RN4020_DEF_PTR rn4020, const char *cmd)
{
	_rn4020_set_state(rn4020, RN4020_STATE_WAITING_FOR_AOK);
	rn4020_send(rn4020, cmd);
	return _rn4020_wait_for_ready(rn4020);
}

static int32_t _rn4020_server_write_char(RN4020_DEF_PTR rn4020,
        const uint8_t *uuid, uint8_t uuid_len,
        const uint8_t *data, uint32_t data_len)
{
	char line[RN4020_PRIVATE_UUID_HEX_STRING_LENGTH + 3];

	rn4020_uuid_to_string(line, uuid, uuid_len);
	_rn4020_set_state(rn4020, RN4020_STATE_WAITING_FOR_AOK);

	ez_sio_write(rn4020->sio_uart, "SUW,", 4);
	ez_sio_write(rn4020->sio_uart, line, strlen(line));
	ez_sio_write(rn4020->sio_uart, ",", 1);

	for (uint32_t i = 0; i < data_len; i++) {
		sprintf(line, "%02X", data[i]);
		ez_sio_write(rn4020->sio_uart, line, 2);
	}

	ez_sio_write(rn4020->sio_uart, "\n", 1);

	return _rn4020_wait_for_ready(rn4020);
}

#ifdef RN4020_INTERRUPT_DRIVERN
static void _rn4020_rx_cb(uint32_t data)
{
	if (data != '\n' && rn4020_ptr->rx_cnt < RN4020_RX_BUFFER_SIZE) {
		if (data != 0) {
			rn4020_ptr->rx_buffer[rn4020_ptr->rx_cnt++] = (uint8_t)data;
		}
	} else {

		if (rn4020_ptr->rx_buffer[rn4020_ptr->rx_cnt - 2] == '\r') {
			/* remove \r\n */
			rn4020_ptr->rx_buffer[rn4020_ptr->rx_cnt - 2] = 0;
		} else {
			rn4020_ptr->rx_buffer[rn4020_ptr->rx_cnt - 1] = 0;
		}

		_rn4020_line_parser(rn4020_ptr, (char *)rn4020_ptr->rx_buffer);
		rn4020_ptr->rx_cnt = 0;
	}
}
#endif

int32_t rn4020_setup(RN4020_DEF_PTR rn4020)
{
	DEV_GPIO_PTR gpio_wake_sw;
	DEV_GPIO_PTR gpio_wake_hw;
	DEV_GPIO_PTR gpio_cmd;
	int32_t ret;

	rn4020->connected = false;
	rn4020->handle_uuid_cnt = 0;

	gpio_wake_sw = gpio_get_dev(rn4020->gpio_wake_sw);
	gpio_wake_hw = gpio_get_dev(rn4020->gpio_wake_hw);
	gpio_cmd = gpio_get_dev(rn4020->gpio_cmd);

	if (gpio_wake_sw->gpio_open((1 << rn4020->pin_wake_sw)) == E_OPNED) {
		gpio_wake_sw->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT,
		                           (void *)(1 << rn4020->pin_wake_sw));
		gpio_wake_sw->gpio_control(GPIO_CMD_DIS_BIT_INT,
		                           (void *)(1 << rn4020->pin_wake_sw));
	}

	if (gpio_wake_hw->gpio_open((1 << rn4020->pin_wake_hw)) == E_OPNED) {
		gpio_wake_sw->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT,
		                           (void *)(1 << rn4020->pin_wake_hw));
		gpio_wake_sw->gpio_control(GPIO_CMD_DIS_BIT_INT,
		                           (void *)(1 << rn4020->pin_wake_hw));
	}

	if (gpio_cmd->gpio_open((1 << rn4020->pin_cmd)) == E_OPNED) {
		gpio_cmd->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT,
		                       (void *)(1 << rn4020->pin_cmd));
		gpio_cmd->gpio_control(GPIO_CMD_DIS_BIT_INT,
		                       (void *)(1 << rn4020->pin_cmd));
	}

	gpio_wake_hw->gpio_write(0, 1 << rn4020->pin_wake_hw);
	gpio_wake_sw->gpio_write(0, 1 << rn4020->pin_wake_sw);
	board_delay_ms(100, 0);

	rn4020->sio_uart = ez_sio_open(rn4020->uart, 115200, 1024, 1024);
#ifdef RN4020_INTERRUPT_DRIVERN
	rn4020_ptr = rn4020;
	uart->uart_control(UART_CMD_SET_RXCB, (void *)_rn4020_rx_cb);
	uart->uart_control(UART_CMD_SET_RXINT_BUF, NULL);
	uart->uart_control(UART_CMD_SET_RXINT, (void *)1);
#endif

#ifdef RN4020_HW_FLOW_CTRL
	uart->uart_control(UART_CMD_SET_HWFC, (void *)1);
#endif
	_rn4020_set_state(rn4020, RN4020_STATE_INITIALIZING);

	gpio_wake_hw->gpio_write(1 << rn4020->pin_wake_hw,
	                         1 << rn4020->pin_wake_hw);
	gpio_wake_sw->gpio_write(1 << rn4020->pin_wake_sw,
	                         1 << rn4020->pin_wake_sw);
	gpio_cmd->gpio_write(0, 1 << rn4020->pin_cmd);

	board_delay_ms(100, 0);

	ret = _rn4020_wait_for_ready(rn4020);

	return ret;
}

int32_t rn4020_is_connected(RN4020_DEF_PTR rn4020)
{
	return rn4020->connected;
}

int32_t rn4020_reset_to_factory(RN4020_DEF_PTR rn4020)
{
	return _rn4020_cmd_aok(rn4020, "SF,1");
	// DEV_GPIO_PTR gpio_wake_hw = gpio_get_dev(rn4020->gpio_wake_hw);

	// for (int32_t i = 0; i < 4; i++) {
	// 	gpio_wake_hw->gpio_write(1 << rn4020->pin_wake_hw,
	// 			1 << rn4020->pin_wake_hw);
	// 	board_delay_ms(500, 0);
	// 	gpio_wake_hw->gpio_write(0, 1 << rn4020->pin_wake_hw);
	// 	board_delay_ms(500, 0);
	// }

}

int32_t rn4020_set_services(RN4020_DEF_PTR rn4020, uint32_t services)
{
	char line[15];
	snprintf(line, 15, "SS,%08X", services);
	return _rn4020_cmd_aok(rn4020, line);
}

int32_t rn4020_set_features(RN4020_DEF_PTR rn4020, uint32_t features)
{
	char line[15];
	snprintf(line, 15, "SR,%08X", features);
	return _rn4020_cmd_aok(rn4020, line);
}

int32_t rn4020_set_dev_name_mac(RN4020_DEF_PTR rn4020, const char *name)
{
	char line[15];
	snprintf(line, 15, "S-,%s", name);
	return _rn4020_cmd_aok(rn4020, line);
}

int32_t rn4020_set_dev_name(RN4020_DEF_PTR rn4020, const char *name)
{
	char line[15];
	snprintf(line, 15, "SN,%s", name);
	return _rn4020_cmd_aok(rn4020, line);
}

int32_t rn4020_set_manufacturer_name(RN4020_DEF_PTR rn4020, const char *name)
{
	char line[15];
	snprintf(line, 15, "SDN,%s", name);
	return _rn4020_cmd_aok(rn4020, line);
}

int32_t rn4020_reset(RN4020_DEF_PTR rn4020)
{
	_rn4020_set_state(rn4020, RN4020_STATE_WAITING_FOR_RESET);
	rn4020_send(rn4020, "R,1");
	return _rn4020_wait_for_ready(rn4020);
}

int32_t rn4020_advertise(RN4020_DEF_PTR rn4020)
{
	return _rn4020_cmd_aok(rn4020, "A");
}

int32_t rn4020_remove_bond(RN4020_DEF_PTR rn4020)
{
	return _rn4020_cmd_aok(rn4020, "U");
}

/* look up the the handle supported */
int32_t rn4020_refresh_handle_uuid_table(RN4020_DEF_PTR rn4020)
{
	rn4020->handle_uuid_cnt = 0;
	_rn4020_set_state(rn4020, RN4020_STATE_WAITING_FOR_LS);
	rn4020_send(rn4020, "LS");
	return _rn4020_wait_for_ready(rn4020);
}

int32_t rn4020_clear_private(RN4020_DEF_PTR rn4020)
{
	return _rn4020_cmd_aok(rn4020, "PZ");
}

int32_t rn4020_add_prv_service(RN4020_DEF_PTR rn4020, const uint8_t *uuid)
{
	char line[RN4020_PRIVATE_UUID_HEX_STRING_LENGTH+5];
	strcpy(line, "PS,");
	rn4020_uuid_to_string(line + 3, uuid, RN4020_PRIVATE_UUID_LENGTH_BYTES);
	return _rn4020_cmd_aok(rn4020, line);
}

int32_t rn4020_add_prv_char(RN4020_DEF_PTR rn4020, const uint8_t *uuid, uint8_t property,
                            uint8_t size, uint8_t security)
{
	char line[50];
	char *dest;
	strcpy(line, "PC,");
	rn4020_uuid_to_string(line + 3, uuid, RN4020_PRIVATE_UUID_LENGTH_BYTES);
	dest = line + strlen(line);
	snprintf(dest, 50, ",%02X,%02X", property, size);

	if (security != RN4020_PRIVATE_CHAR_SEC_NONE) {
		dest = line + strlen(line);
		sprintf(dest, ",%02X", security);
	}

	return _rn4020_cmd_aok(rn4020, line);
}

void rn4020_uuid_to_string(char *dest, const uint8_t *uuid, uint8_t len)
{
	for (int i = 0; i < len; i++) {
		sprintf(dest, "%02X", uuid[i]);
		dest += 2;
	}
}

RN4020_HANDLE_UUID_ITEM *rn4020_handle_uuid_item(RN4020_DEF_PTR rn4020, uint16_t handle)
{
	RN4020_HANDLE_UUID_ITEM *item;

	for (int i = 0; i < rn4020->handle_uuid_cnt; i++) {
		item = &rn4020->handle_uuid_table[i];

		if (item->handle == handle) {
			return item;
		}
	}

	return NULL;
}

uint8_t rn4020_handle_match_uuid16(RN4020_HANDLE_UUID_ITEM *item, uint16_t uuid)
{
	uint16_t item_uuid;

	if (item->uuid_len != 2) {
		return false;
	}

	item_uuid = ((uint16_t)item->uuid[0] << 8) | item->uuid[1];
	return item_uuid == uuid;
}

uint8_t rn4020_handle_match_uuid128(RN4020_HANDLE_UUID_ITEM *item, const uint8_t *uuid)
{
	if (item->uuid_len != RN4020_PRIVATE_UUID_LENGTH_BYTES) {
		return false;
	}

	for (int i = 0; i < RN4020_PRIVATE_UUID_LENGTH_BYTES; i++) {
		if (uuid[i] != item->uuid[i]) {
			return false;
		}
	}

	return true;
}

EMBARC_WEAK void rn4020_connected_changed(RN4020_DEF_PTR rn4020, uint8_t connected)
{
	EMBARC_PRINTF("%s\n", connected ? "connected" : "disconnected");
}

EMBARC_WEAK void rn4020_on_realtime_read(RN4020_DEF_PTR rn4020, uint16_t handle)
{
	EMBARC_PRINTF("real time read: 0x%04X\n", handle);
}

EMBARC_WEAK void rn4020_on_write(RN4020_DEF_PTR rn4020, uint16_t handle, uint8_t *data)
{
	EMBARC_PRINTF("write: 0x%04X:%s ", handle, data);
}

void rn4020_send(RN4020_DEF_PTR rn4020, const char *line)
{
	ez_sio_write(rn4020->sio_uart, (char *)line, strlen(line));
	ez_sio_write(rn4020->sio_uart, "\n", 1);
	DBG("tx: %s\n", line);
}

int32_t rn4020_server_write_pub_char(RN4020_DEF_PTR rn4020, uint16_t uuid, const uint8_t *data, uint32_t len)
{
	uint8_t uuid_data[2];

	uuid_data[0] = (uuid >> 8) & 0xff;
	uuid_data[1] = (uuid >> 0) & 0xff;

	return _rn4020_server_write_char(rn4020, uuid_data, 2, data, len);
}

int32_t rn4020_server_write_prv_char(RN4020_DEF_PTR rn4020, const uint8_t *uuid, const uint8_t *data, uint32_t len)
{
	return _rn4020_server_write_char(rn4020, uuid, RN4020_PRIVATE_UUID_LENGTH_BYTES, data, len);
}

int32_t rn4020_server_write_pub_char_handle(RN4020_DEF_PTR rn4020,
        uint16_t handle, const uint8_t *data, uint32_t len)
{
	char line[20];

	_rn4020_set_state(rn4020, RN4020_STATE_WAITING_FOR_AOK);

	snprintf(line, 20, "SHW,%04X,", handle);
	ez_sio_write(rn4020->sio_uart, line, strlen(line));

	for (uint32_t i = 0; i < len; i++) {
		sprintf(line, "%02X", data[i]);
		ez_sio_write(rn4020->sio_uart, line, 2);
	}

	ez_sio_write(rn4020->sio_uart, "\n", 1);

	return _rn4020_wait_for_ready(rn4020);
}

int32_t rn4020_battery_set_level(RN4020_DEF_PTR rn4020, uint8_t level)
{
	return rn4020_server_write_pub_char(rn4020, RN4020_BATTERY_LEVEL_UUID, &level, 1);
}