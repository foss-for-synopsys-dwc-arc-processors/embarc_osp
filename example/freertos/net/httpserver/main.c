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
 * \date 2016-04-22
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	EMBARC_APP_FREERTOS_NET_HTTPSERVER	embARC LwIP HTTPServer Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_FREERTOS
 * \ingroup	EMBARC_APPS_MID_LWIP
 * \brief	embARC Example for LwIP HTTPServer on FreeRTOS
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     * Digilent PMOD WIFI(MRF24WG0MA)
 *
 * ### Design Concept
 *     This example is designed to show how to use LwIP netconn API to serve as a httpserver.
 *
 * ### Usage Manual
 *     The Pmod modules should be connected to \ref EMBARC_BOARD_CONNECTION "EMSK".
 *     This is an example using TCP/IP connection to work as a small web httpserver.
 *
 *     ![ScreenShot of httpserver under freertos](pic/images/example/emsk/emsk_lwip_freertos_httpserver.jpg)
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_NET_HTTPSERVER
 * \brief	http server using TCP connection example
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_NET_HTTPSERVER
 * @{
 */
#include "embARC.h"
#include "embARC_debug.h"

#include "httpserver-netconn.h"

int main(void)
{
	EMBARC_PRINTF("Now Open Your Browser Enter The IPAddr Show On Terminal, you will get an webpage servered on EMSK\r\n");
	http_server_netconn_init();

	return 0;
}

/** @} */