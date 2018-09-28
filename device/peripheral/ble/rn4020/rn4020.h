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
#ifndef _RN4020_H_
#define _RN4020_H_

#include "ez_sio.h"

#ifndef RN4020_HANDLE_UUID_TABLE_SIZE
#define RN4020_HANDLE_UUID_TABLE_SIZE 20
#endif

#define RN4020_RX_BUFFER_SIZE 256

#define RN4020_PRIVATE_UUID_LENGTH_BITS       128
#define RN4020_PRIVATE_UUID_LENGTH_BYTES      (128 / 8)
#define RN4020_PRIVATE_UUID_HEX_STRING_LENGTH (RN4020_PRIVATE_UUID_LENGTH_BYTES * 2)
#define RN4020_MAX_UUID_LEN_BYTES             (128 / 8)

#define RN4020_SERVICE_DEVICE_INFORMATION    0x80000000
#define RN4020_SERVICE_BATTERY               0x40000000
#define RN4020_SERVICE_HEART_RATE            0x20000000
#define RN4020_SERVICE_HEALTH_THERMOMETER    0x10000000
#define RN4020_SERVICE_GLUCOSE               0x08000000
#define RN4020_SERVICE_BLOOD_PRESSURE        0x04000000
#define RN4020_SERVICE_RUNNING_SPEED_CADENCE 0x02000000
#define RN4020_SERVICE_CYCLING_SPEED_CADENCE 0x01000000
#define RN4020_SERVICE_CURRENT_TIME          0x00800000
#define RN4020_SERVICE_NEXT_DST_CHANGE       0x00400000
#define RN4020_SERVICE_REFERENCE_TIME_UPDATE 0x00200000
#define RN4020_SERVICE_LINK_LOSS             0x00100000
#define RN4020_SERVICE_IMMEDIATE_ALERT       0x00080000
#define RN4020_SERVICE_TX_POWER              0x00040000
#define RN4020_SERVICE_ALERT_NOTIFICATION    0x00020000
#define RN4020_SERVICE_PHONE_ALERT_STATUS    0x00010000
#define RN4020_SERVICE_SCAN_PARAMETERS       0x00004000
#define RN4020_SERVICE_USER_DEFINED          0x00000001

#define RN4020_FEATURE_CENTRAL             0x80000000
#define RN4020_FEATURE_REAL_TIME_READ      0x40000000
#define RN4020_FEATURE_AUTO_ADVERTISE      0x20000000
#define RN4020_FEATURE_ENABLE_MLDP         0x10000000
#define RN4020_FEATURE_AUTO_MLDP_DISABLE   0x08000000
#define RN4020_FEATURE_NO_DIRECT_ADVERTISE 0x04000000
#define RN4020_FEATURE_UART_FLOW_CONTROL   0x02000000
#define RN4020_FEATURE_RUN_SCRIPT_PWR_ON   0x01000000
#define RN4020_FEATURE_ENABLE_AUTH         0x00400000
#define RN4020_FEATURE_ENABLE_REMOTE_CMD   0x00200000
#define RN4020_FEATURE_DO_NOT_SAVE_BONDING 0x00100000
#define RN4020_FEATURE_IO_CAP              0x00E00000
#define RN4020_FEATURE_BLOCK_SET_CMD       0x00010000
#define RN4020_FEATURE_ENABLE_OTA          0x00008000
#define RN4020_FEATURE_IOS_MODE            0x00004000
#define RN4020_FEATURE_SERVER_ONLY         0x00002000
#define RN4020_FEATURE_ENABLE_UART_SCRIPT  0x00001000
#define RN4020_FEATURE_AUTO_MLDP           0x00000800
#define RN4020_FEATURE_MLDP_WITHOUT_STATUS 0x00000400

#define RN4020_PRIVATE_CHAR_PROP_EXTENDED        0b10000000
#define RN4020_PRIVATE_CHAR_PROP_AUTH_WRITE      0b01000000
#define RN4020_PRIVATE_CHAR_PROP_INDICATE        0b00100000
#define RN4020_PRIVATE_CHAR_PROP_NOTIFY          0b00010000
#define RN4020_PRIVATE_CHAR_PROP_WRITE           0b00001000
#define RN4020_PRIVATE_CHAR_PROP_WRITE_NO_RESP   0b00000100
#define RN4020_PRIVATE_CHAR_PROP_READ            0b00000010
#define RN4020_PRIVATE_CHAR_PROP_BROADCAST       0b00000001

#define RN4020_PRIVATE_CHAR_SEC_NONE     0b00000000
#define RN4020_PRIVATE_CHAR_SEC_ENCR_R   0b00000001
#define RN4020_PRIVATE_CHAR_SEC_AUTH_R   0b00000010
#define RN4020_PRIVATE_CHAR_SEC_ENCR_W   0b00010000
#define RN4020_PRIVATE_CHAR_SEC_AUTH_W   0b00100000

#define RN4020_DEV_INFO_MANUFACTURER_NAME  0x2a29
#define RN4020_DEV_INFO_MODEL_NUMBER       0x2a24
#define RN4020_DEV_INFO_SERIAL_NUMBER      0x2a25
#define RN4020_DEV_INFO_HARDWARE_REVISION  0x2a27
#define RN4020_DEV_INFO_FIRMWARE_REVISION  0x2a26
#define RN4020_DEV_INFO_SOFTWARE_REVISION  0x2a28
#define RN4020_DEV_INFO_SYSTEM_ID          0x2a23
#define RN4020_DEV_INFO_REG_CERT_DATA      0x2a2a

#define RN4020_BATTERY_LEVEL_UUID  0x2a19
#define RN4020_BATTERY_MAX_LEVEL   0x64


typedef enum rn4020_state {
	RN4020_STATE_INITIALIZING,
	RN4020_STATE_READY,
	RN4020_STATE_WAITING_FOR_CMD,
	RN4020_STATE_WAITING_FOR_AOK,
	RN4020_STATE_WAITING_FOR_RESET,
	RN4020_STATE_WAITING_FOR_LS
} RN4020_STATE;


typedef struct {
	uint16_t handle;
	uint8_t uuid[RN4020_MAX_UUID_LEN_BYTES ];
	uint8_t uuid_len;
} RN4020_HANDLE_UUID_ITEM;


typedef struct rn4020_def {
	uint8_t uart;
	uint8_t gpio_wake_sw;
	uint8_t pin_wake_sw;
	uint8_t gpio_wake_hw;
	uint8_t pin_wake_hw;
	uint8_t gpio_cmd;
	uint8_t pin_cmd;
	EZ_SIO *sio_uart;
	volatile uint8_t connected;
	volatile uint8_t need_advertise;
	volatile RN4020_STATE state;

	uint8_t rx_buffer[RN4020_RX_BUFFER_SIZE];
	uint16_t rx_cnt;

	RN4020_HANDLE_UUID_ITEM handle_uuid_table[RN4020_HANDLE_UUID_TABLE_SIZE];
	uint32_t handle_uuid_cnt;
} RN4020_DEF, *RN4020_DEF_PTR;

#define RN4020_DEFINE(name, uart_id, sw_id, sw_pin, hw_id, hw_pin, cmd_id, cmd_pin) \
	RN4020_DEF __ ## name = { \
		.uart = uart_id, \
		.gpio_wake_sw = sw_id, \
		.pin_wake_sw = sw_pin, \
		.gpio_wake_hw = hw_id, \
		.pin_wake_hw = hw_pin, \
		.gpio_cmd = cmd_id, \
		.pin_cmd = cmd_pin, \
	}; \
	RN4020_DEF_PTR name = &__ ## name


EMBARC_WEAK extern void rn4020_on_realtime_read(RN4020_DEF_PTR rn4020, uint16_t handle);
EMBARC_WEAK extern void rn4020_on_write(RN4020_DEF_PTR rn4020, uint16_t handle, uint8_t *data);
EMBARC_WEAK extern void rn4020_connected_changed(RN4020_DEF_PTR rn4020, uint8_t connected);

extern int32_t rn4020_setup(RN4020_DEF_PTR rn4020);
extern int32_t rn4020_reset_to_factory(RN4020_DEF_PTR rn4020);
extern int32_t rn4020_set_services(RN4020_DEF_PTR rn4020, uint32_t services);
extern int32_t rn4020_set_features(RN4020_DEF_PTR rn4020, uint32_t features);
extern int32_t rn4020_reset(RN4020_DEF_PTR rn4020);
extern int32_t rn4020_set_dev_name_mac(RN4020_DEF_PTR rn4020, const char *name);
extern int32_t rn4020_set_dev_name(RN4020_DEF_PTR rn4020, const char *name);
extern int32_t rn4020_set_manufacturer_name(RN4020_DEF_PTR rn4020, const char *name);
extern int32_t rn4020_advertise(RN4020_DEF_PTR rn4020);
extern int32_t rn4020_remove_bond(RN4020_DEF_PTR rn4020);
extern int32_t rn4020_refresh_handle_uuid_table(RN4020_DEF_PTR rn4020);
extern int32_t rn4020_clear_private(RN4020_DEF_PTR rn4020);
extern int32_t rn4020_is_connected(RN4020_DEF_PTR rn4020);
extern int32_t rn4020_add_prv_service(RN4020_DEF_PTR rn4020, const uint8_t *uuid);
extern int32_t rn4020_add_prv_char(RN4020_DEF_PTR rn4020, const uint8_t *uuid, uint8_t property,
                                   uint8_t size, uint8_t security);
extern void rn4020_uuid_to_string(char *dest, const uint8_t *uuid, uint8_t len);
extern RN4020_HANDLE_UUID_ITEM *rn4020_lookup_handle(RN4020_DEF_PTR rn4020, uint16_t handle);
extern uint8_t rn4020_handle_match_uuid16(RN4020_HANDLE_UUID_ITEM *item, uint16_t uuid);
extern uint8_t rn4020_handle_match_uuid128(RN4020_HANDLE_UUID_ITEM *item, const uint8_t *uuid);
extern void rn4020_tick(RN4020_DEF_PTR rn4020);
extern void rn4020_send(RN4020_DEF_PTR rn4020, const char *line);

extern int32_t rn4020_server_write_pub_char(RN4020_DEF_PTR rn4020, uint16_t uuid, const uint8_t *data, uint32_t len);
extern int32_t rn4020_server_write_prv_char(RN4020_DEF_PTR rn4020, const uint8_t *uuid, const uint8_t *data, uint32_t len);
extern int32_t rn4020_server_write_pub_char_handle(RN4020_DEF_PTR rn4020, uint16_t handle, const uint8_t *data, uint32_t len);

/**
 * level 0x00 (0%) - 0x64 (100%)
 */
extern int32_t rn4020_battery_set_level(RN4020_DEF_PTR rn4020, uint8_t level);


#endif /* _RN4020_H_ */