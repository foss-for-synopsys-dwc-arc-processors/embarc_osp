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
 * \date 2016-04-20
 * \author Qiang Gu(Qiang.Gu@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	EMBARC_APP_CONTIKI_SENSOR	embARC Contiki Sensor Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_CONTIKI
 * \brief	embARC example for sensor framework on Contiki
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     * Digilent PMOD TMP2
 *
 * ### Design Concept
 *     This example is designed to show how to write process, the sensor framework and how event driving works in Contiki
 *
 * ### Usage Manual
 *     - How to connect
 *
 *       PMOD temperature to J2 on \ref EMBARC_BOARD_CONNECTION "EMSK"
 *
 *     - How to use this example
 *
 *       The 3 buttons, except reset and reconfig buttons, are registerd as button sensor. Pressing one button will trigger the
 *       sensor_event, then the coressonding process will be called.
 *
 *     ![ScreenShot of contiki sensor example](pic/images/example/emsk/emsk_contiki_sensor.jpg)
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_CONTIKI_SENSOR
 * \brief	sensor example based on Contiki
 */

/**
 * \addtogroup	EMBARC_APP_CONTIKI_SENSOR
 * @{
 */

#include "embARC.h"
#include "embARC_debug.h"
#include "dev/sensors/button-sensor.h"
#include "dev/sensors/temperature-sensor.h"
#include "dev/leds.h"

SENSORS(&button_sensor1, &button_sensor2, &button_sensor3, &temperature_sensor);
/*---------------------------------------------------------------------------*/
PROCESS(button1_process, "button 1 process");
PROCESS(button2_process, "button 2 process");
PROCESS(button3_process, "button 3 process");

AUTOSTART_PROCESSES(&button1_process, &button2_process, &button3_process);

PROCESS_THREAD(button1_process, ev, data)
{
	PROCESS_BEGIN();
	process_start(&sensors_process, NULL);
	SENSORS_ACTIVATE(button_sensor1);
	EMBARC_PRINTF("----------Contiki Sensor Example-----------\r\n");
	EMBARC_PRINTF("Pmod temp module is required\r\n");
	EMBARC_PRINTF("Press the button 1 to toggle the leds.\r\n");

	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event &&
			data == &button_sensor1);
		leds_toggle(LEDS_ALL);
	}

	PROCESS_END();
	}

PROCESS_THREAD(button2_process, ev, data)
{
	static long temp_data;

	PROCESS_BEGIN();
	SENSORS_ACTIVATE(button_sensor2);
	SENSORS_ACTIVATE(temperature_sensor);
	EMBARC_PRINTF("Press the button 2 to get temperature.\r\n");

	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event &&
		data == &button_sensor2);
		temp_data = temperature_sensor.value(0);
		temp_data *= 625;
		EMBARC_PRINTF("Temp: %2d.%2d C\r\n", (int)(temp_data/10000), (int)(((temp_data-(temp_data/10000)*10000))/100));
	}

	PROCESS_END();
}

PROCESS_THREAD(button3_process, ev, data)
{
	PROCESS_BEGIN();

	SENSORS_ACTIVATE(button_sensor3);
	EMBARC_PRINTF("Press the button 3 to show message.\r\n");

	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event &&
				 data == &button_sensor3);
		EMBARC_PRINTF("Button 3 is pressed\r\n");
	}

	PROCESS_END();
}
/** @} */