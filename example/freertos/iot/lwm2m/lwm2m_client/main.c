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
 * \defgroup	EMBARC_APP_FREERTOS_IOT_LWM2M_LWM2M_CLIENT	embARC LwIP LwM2M Client Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_FREERTOS
 * \ingroup	EMBARC_APPS_MID_LWIP
 * \ingroup	EMBARC_APPS_MID_LWM2M
 * \ingroup	EMBARC_APPS_MID_NTSHELL
 * \ingroup	EMBARC_APPS_MID_FATFS
 * \brief	embARC Example for LwM2M Client on LwIP and FreeRTOS
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     * Digilent PMOD WIFI(MRF24WG0MA)
 *
 * ### Design Concept
 *     This example is designed to show how to use LwM2M client in FreeRTOS.
 *
 * ### Usage Manual
 *     The Pmod modules should be connected to \ref EMBARC_BOARD_CONNECTION "EMSK".
 *     This is an example running OMA LightweightM2M(LwM2M) client, provides core device management features(device, connectivity, etc). It provides a ntshell command called *lwm2m_client* to show this demo.
 *     - About LwM2M provided in embARC
 *         + LwM2M features supported by [eclipse wakaama project](https://projects.eclipse.org/projects/technology.wakaama)
 *         + We only support FreeRTOS+LwIP
 *         + LwM2M use UDP connections to exchange data
 *     - First, you need to provide a LwM2M Server, here we take [Eclipse Leshan server](https://github.com/eclipse/leshan) for example.
 *         + Eclipse provide a sandbox leshan server:http://leshan.eclipse.org/
 *         + You can download a standalone leshan server binary from leshan website, and java environment(version >= 1.7.0_75) is required to run this binary
 *     - When leshan server is running on your local PC(connected to the same wifi network which your board will connect), then download program to the board, and run *lwm2m_client* ntshell command.
 *         + If you local PC IP is 192.168.43.4, then you need to run command *lwm2m_client -s 192.168.43.4* to connect to local leshan server.
 *         + If connected to local leshan server, you will see the connected LwM2M client in http://127.0.0.1:8080/#/clients.
 *         + Click on the client, then you will see many objects which the connected client provided.
 *         + On this website, you can do many operations on this objects.
 *         + Here you can read onboard switch and button value, and control onboard led using Object 1024.
 *             * First read the Instance 0, then 4 resources will show. Resource 0 is button value, Resource 1 is switch value, Resource 2 is led control, Resource 3 is a float variable.
 *             * Read Instance 0/1 will return the value of this resources in DEC format, you can press buttons or pull on/up the switches to see value changes.
 *             * Write on Resource 2 in DEC format such as 34 will change the onboard led with value 34.
 *             * Write on Resource 3 with a float value such as 1.45, then the float value is changed, you can read back to check it.
 *
 *     ![ScreenShot of lwm2m client under freertos](pic/images/example/emsk/emsk_lwip_freertos_lwm2m_client.jpg)
 *
 * ### Extra Comments
 *     - Sometimes the server may not respond to client, this may caused by the wifi connection is bad.
 *     - In the upper case, you need to disconnect all the power of your board, and reconnect PMOD WIFI module and wait for a while, then restart this application.
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_IOT_LWM2M_LWM2M_CLIENT
 * \brief	main source of LwM2M client example
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_IOT_LWM2M_LWM2M_CLIENT
 * @{
 */

#include "embARC.h"
#include "embARC_debug.h"

extern CMD_TABLE_T * register_ntshell_cmd_lwm2m_client(CMD_TABLE_T *prev);

static volatile int main_run_flag = 0;

int main(void)
{
	NTSHELL_IO *nt_io;
	nt_io = get_ntshell_io(BOARD_ONBOARD_NTSHELL_ID);

	EMBARC_PRINTF("Start FreeRTOS LwM2M Client Demo by run lwm2m_client command\r\n");
	if (main_run_flag == 0) {
		main_run_flag = 1;
		register_ntshell_cmd_lwm2m_client(nt_io->cmd_tbl_head);
	}

	return 0;
}
/** @} */
