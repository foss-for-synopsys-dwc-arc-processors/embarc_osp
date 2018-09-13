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

#include "embARC.h"
#include "embARC_debug.h"

#include "rn4020.h"

#if defined(BOARD_IOTDK)
#define RN4020_UART_ID DFSS_UART_3_ID
#define RN4020_WAKE_SW_GPIO DFSS_GPIO_4B0_ID
#define RN4020_WAKE_SW_PIN 2
#define RN4020_WAKE_HW_GPIO DFSS_GPIO_4B0_ID
#define RN4020_WAKE_HW_PIN 1
#define RN4020_CMD_GPIO DFSS_GPIO_4B0_ID
#define RN4020_CMD_PIN 3

#else
/*
 * pls configure the correct uart and gpio information for your board
 * and applicaiton
 */
#define RN4020_UART_ID 0
#define RN4020_WAKE_SW_GPIO 0
#define RN4020_WAKE_SW_PIN 1
#define RN4020_WAKE_HW_GPIO 0
#define RN4020_WAKE_HW_PIN 2
#define RN4020_CMD_GPIO 0
#define RN4020_CMD_PIN 3

#endif

RN4020_DEFINE(rn4020_ble, RN4020_UART_ID, RN4020_WAKE_SW_GPIO, RN4020_WAKE_SW_PIN
	, RN4020_WAKE_HW_GPIO, RN4020_WAKE_HW_PIN, RN4020_CMD_GPIO, RN4020_CMD_PIN);

uint8_t test_private_services_uuid[RN4020_PRIVATE_UUID_LENGTH_BYTES] =
				{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

uint8_t test_private_characteristic_0_uuid[RN4020_PRIVATE_UUID_LENGTH_BYTES] =
				{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

uint8_t test_private_characteristic_1_uuid[RN4020_PRIVATE_UUID_LENGTH_BYTES] =
				{0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

int main(void) {

	uint8_t battery = 100;

	EMBARC_PRINTF("rn4020 test application\r\n");

	rn4020_setup(rn4020_ble);
	rn4020_reset_to_factory(rn4020_ble);
	rn4020_set_dev_name(rn4020_ble, "embARC");


	rn4020_set_services(rn4020_ble, RN4020_SERVICE_DEVICE_INFORMATION |
					    RN4020_SERVICE_BATTERY | RN4020_SERVICE_USER_DEFINED);


	rn4020_clear_private(rn4020_ble);
	rn4020_add_prv_service(rn4020_ble, test_private_services_uuid);
	rn4020_add_prv_char(rn4020_ble, test_private_characteristic_0_uuid,
			RN4020_PRIVATE_CHAR_PROP_NOTIFY | RN4020_PRIVATE_CHAR_PROP_READ,
			5, RN4020_PRIVATE_CHAR_SEC_NONE);
	rn4020_add_prv_char(rn4020_ble, test_private_characteristic_1_uuid,
			RN4020_PRIVATE_CHAR_PROP_WRITE | RN4020_PRIVATE_CHAR_PROP_READ,
			5, RN4020_PRIVATE_CHAR_SEC_NONE);

	rn4020_set_features(rn4020_ble, RN4020_FEATURE_SERVER_ONLY);
	//rn4020_remove_bond(rn4020_ble);
	rn4020_reset(rn4020_ble);

	//rn4020_refresh_handle_uuid_table(rn4020_ble);

	rn4020_advertise(rn4020_ble);

	rn4020_refresh_handle_uuid_table(rn4020_ble);

	while (1) {
		rn4020_battery_set_level(rn4020_ble, battery--);
		//rn4020_server_write_prv_char(rn4020_ble, test_private_characteristic_0_uuid, &battery, 1);
		//rn4020_advertise(rn4020_ble);
		board_delay_ms(2000, 0);
		if(rn4020_ble->need_advertise)
		{
			rn4020_ble->need_advertise = 0;
			rn4020_advertise(rn4020_ble);
		}
		if (battery < 30) {
			battery = 100;
		}
	}

}