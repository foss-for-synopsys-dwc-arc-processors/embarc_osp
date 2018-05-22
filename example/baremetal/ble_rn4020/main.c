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

RN4020_DEFINE(rn4020_ble, DFSS_UART_3_ID, DFSS_GPIO_4B0_ID, 2, DFSS_GPIO_4B0_ID, 1, DFSS_GPIO_4B0_ID, 3);


int main(void) {

	uint32_t battery = 100;

	EMBARC_PRINTF("rn4020 test application\r\n");

	rn4020_setup(rn4020_ble);
	rn4020_reset_to_factory(rn4020_ble);
	rn4020_set_dev_name(rn4020_ble, "embARC");
	rn4020_refresh_handle_uuid_table(rn4020_ble);
	rn4020_set_services(rn4020_ble, RN4020_SERVICE_DEVICE_INFORMATION |
					    RN4020_SERVICE_BATTERY);
	rn4020_set_features(rn4020_ble, RN4020_FEATURE_SERVER_ONLY);
	rn4020_reset(rn4020_ble);
	rn4020_advertise(rn4020_ble);

	while (1) {
		rn4020_battery_set_level(rn4020_ble, battery--);

		board_delay_ms(1000, 0);
		if (battery < 30) {
			battery = 100;
		}
	}

}