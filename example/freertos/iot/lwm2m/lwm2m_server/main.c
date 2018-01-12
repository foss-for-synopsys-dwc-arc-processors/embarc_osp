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
/**
 * \defgroup	EMBARC_APP_FREERTOS_IOT_LWM2M_LWM2M_SERVER	embARC LwIP LwM2M Server Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_FREERTOS
 * \ingroup	EMBARC_APPS_MID_LWIP
 * \ingroup	EMBARC_APPS_MID_LWM2M
 * \ingroup	EMBARC_APPS_MID_NTSHELL
 * \ingroup	EMBARC_APPS_MID_FATFS
 * \brief	embARC Example for LwM2M Server on LwIP and FreeRTOS
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     * Digilent PMOD WIFI(MRF24WG0MA)
 *
 * ### Design Concept
 *     This example is designed to show how to use LwM2M server in FreeRTOS.
 *
 * ### Usage Manual
 *     The Pmod modules should be connected to \ref EMBARC_BOARD_CONNECTION "EMSK".
 *     This is an example running LwM2M server. It is not recommended to run lwm2m server on a resource-constrained board. It provides two ntshell commands to show this demo, one is *lwm2m_server* to set up a LwM2M server, and another one is *lwm2m_op* to operate at connected clients.
 *     - First, download the program to the board, when program is ready, you can run *lwm2m_server* to start up a LwM2M server at default port 5683.
 *     - Then, you need to install firefox and a firefox plugin - [OMA LwM2M Kit](https://addons.mozilla.org/en-US/firefox/addon/oma-lwm2m-devkit/), this plugin can serve as a lwm2m client.
 *     - If the board IP is 192.168.43.3, then you can startup a LwM2M client in firefox and connect to the LwM2M server using this link: coap+lwm2m://192.168.43.3:5683/, click on Load LWM2M Client to load an example client, and click on Client Registration to register the client.
 *     - When the client is registered, you can run *lwm2m_op -o list* to see the registered clients, and you can also run *lwm2m_op -o read -c 0 -r /3/0/0* to read a device object's manufacturer resource or other resource by specify different resource uri.
 *
 *     ![ScreenShot of lwm2m server under freertos](pic/images/example/emsk/emsk_lwip_freertos_lwm2m_server.jpg)
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_IOT_LWM2M_LWM2M_SERVER
 * \brief	main source of LwM2M Server example
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_IOT_LWM2M_LWM2M_SERVER
 * @{
 */

#include "embARC.h"
#include "embARC_debug.h"

extern CMD_TABLE_T * register_ntshell_cmd_lwm2m_server(CMD_TABLE_T *prev);
static volatile int main_run_flag = 0;

int main(void)
{

	NTSHELL_IO *nt_io;
	nt_io = get_ntshell_io(BOARD_ONBOARD_NTSHELL_ID);

	EMBARC_PRINTF("Start FreeRTOS LwM2M Client Demo by run lwm2m_server and lwm2m_op command\r\n");
	if (main_run_flag == 0) {
		main_run_flag = 1;
		register_ntshell_cmd_lwm2m_server(nt_io->cmd_tbl_head);
	}

	return 0;
}

/** @} */