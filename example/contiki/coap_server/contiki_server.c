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
 * \date 2016-04-28
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */

/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \defgroup	EMBARC_APP_CONTIKI_COAP_SERVER	embARC Contiki CoAP Server Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_CONTIKI
 * \ingroup	EMBARC_APPS_MID_COAP
 * \brief	embARC Example for CoAP Server on Contiki
 *
 * \details
 * ### Extra Required Tools
 *     * [Firefox with CoAP Plugin](https://addons.mozilla.org/en-US/firefox/addon/copper-270430/)
 *
 * ### Extra Required Peripherals
 *     * Digilent PMOD WIFI(MRF24WG0MA)
 *     * Digilent PMOD TMP2
 *
 * ### Design Concept
 *     This example is designed to show the CoAP server example in Contiki
 *
 * ### Usage Manual
 *     * Purpose: This example is WiFi-based. It needs one EMSK node and one PC. The EMSK node is the CoAP server, and the PC is the CoAP client
 *     * Hardware:
 *         - EMSK node: PMOD WiFi to J5, PMOD temperature to J2 on \ref EMBARC_BOARD_CONNECTION "EMSK"
 *         - PC node: Firefox and the Copper add-on: https://addons.mozilla.org/en-US/firefox/addon/copper-270430/
 *     * WiFi Network: *SSID:embARC, PASSWORD:qazwsxedc*
 *     * Run steps:
 *         - Load and run the application first on the sensor node
 *         - The sensor node will try to connect to WiFi. If it connects, EMSK will try to get an IP address from the DHCP server, which is the WiFi hotspot
 *         - The IP address may look like 192.168.43.3, for example
 *         - Open Firefox  and ensure the PC is in the same network as the sensor node
 *         - In Firefox, type coap://192.168.43.3:5683 and click Discover. Then the resources can be discovered
 *         - If you cannot discover resources,  check if the IP address shown on the UART terminal is the same as the one assigned by the DHCP server. You can also try to clean the ARP table using the command "arp -d", and then try to ping the IP address of the board, if ping is not successful, you can try re-downloading program or restarting your computer
 *         - Click the temperature resource, then click GET, so the temperature data can be retrieved
 *         - Click the actuator/toggle resource, then click POST, and the LEDs in the sensor node toggle
 *         - Click sensors/button or test/push resource, then click Observe. The CoAP client in Firefox will observe the resource, and the CoAP server on the board will publish data periodically to the clients
 *         - Click the actuators/LEDs , then enter address in Firefox like this: coap://192.168.43.3:5683/actuators/leds?bits=xx. The xx must be a decimal from 0 to 255. Then set the outgoing mode=on to turn on the LEDs with the bitmask, or set mode=off to turn off the LEDs with bitmask
 *         - click the test/chunks and then click GET to get bulk transfer data which may be in chunks
 *         - click the test/separate and then click GET to get data. You need to press the **BUTTON L** to let the server send a notification to client that data is ready for download.
 *
 *     ![ScreenShot of contiki coap server](pic/images/example/emsk/emsk_contiki_coap_server.jpg)
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_CONTIKI_COAP_SERVER
 * \brief	Erbium (Er) REST Engine example.
 * based on the er-rest-example in contiki
 */

/**
 * \addtogroup	EMBARC_APP_CONTIKI_COAP_SERVER
 * @{
 */

#include <stdlib.h>
#include <string.h>

#include "embARC.h"

#include "contiki-net.h"
#include "rest-engine.h"
#include "net/ip/dhcpc.h"
#include "net/mrf24g/mrf24g_drv.h"

#include "embARC_debug.h"

#if PLATFORM_HAS_BUTTON
#include "dev/sensors/button-sensor.h"
#endif
#if PLATFORM_HAS_LEDS
#include "dev/leds.h"
#endif
#if PLATFORM_HAS_TEMP
#include "dev/sensors/temperature-sensor.h"
#endif

#define DEBUG 1
#if DEBUG
#define PRINTF(...) EMBARC_PRINTF(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

#define ip_addr_dump(msg, ipaddr) \
		EMBARC_PRINTF("%s:%d.%d.%d.%d  ",  (msg),			\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[0] : 0),	\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[1] : 0),	\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[2] : 0),	\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[3] : 0))

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern resource_t
  res_hello,
  res_mirror,
  res_chunks,
  res_separate,
  res_push,
  res_event,
  res_sub,
  res_b1_sep_b2;
#if PLATFORM_HAS_LEDS
extern resource_t res_leds, res_toggle;
#endif
#if PLATFORM_HAS_TEMP
#include "dev/sensors/temperature-sensor.h"
extern resource_t res_temperature;
#endif


PROCESS(er_example_server, "ARC EMSK Erbium Example Server");
PROCESS(main_process, "main_process");
PROCESS(dhcpc_process, "dhcpc_process");

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&main_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(main_process, ev, data)
{
	PROCESS_BEGIN();
	process_start(&tcpip_process, NULL);
	process_start(&mrf24g_process, NULL);
	process_start(&dhcpc_process, NULL);
	PRINTF("-----Contiki coap server example, default port:5683--------\r\n");
	PRINTF("-----Waiting for WiFi connecting and dhcping-----\r\n");
	PROCESS_WAIT_UNTIL(ev == PROCESS_EVENT_EXIT);

	PROCESS_END();
}

SENSORS(&button_sensor1, &temperature_sensor);

PROCESS_THREAD(er_example_server, ev, data)
{
	PROCESS_BEGIN();

	PRINTF("Starting Erbium Example Server\n");
#if 0
	PRINTF("uIP buffer: %u\r\n", UIP_BUFSIZE);
	PRINTF("LL header: %u\r\n", UIP_LLH_LEN);
	PRINTF("IP+UDP header: %u\r\n", UIP_IPUDPH_LEN);
	PRINTF("REST max chunk: %u\r\n", REST_MAX_CHUNK_SIZE);
#endif
  	/* Initialize the REST engine. */
	rest_init_engine();

	/*
	 * Bind the resources to their Uri-Path.
	 * WARNING: Activating twice only means alternate path, not two instances!
	 * All static variables are the same for each URI path.
	 */
	rest_activate_resource(&res_hello, "test/hello");
	rest_activate_resource(&res_mirror, "debug/mirror");
	rest_activate_resource(&res_chunks, "test/chunks");
	rest_activate_resource(&res_separate, "test/separate");
	rest_activate_resource(&res_push, "test/push");
	rest_activate_resource(&res_event, "sensors/button");
	rest_activate_resource(&res_temperature, "sensors/temperature");
	rest_activate_resource(&res_sub, "test/sub");
	rest_activate_resource(&res_b1_sep_b2, "test/b1sepb2");
#if PLATFORM_HAS_LEDS
	/*  rest_activate_resource(&res_leds, "actuators/leds"); */
	rest_activate_resource(&res_toggle, "actuators/toggle");
	rest_activate_resource(&res_leds, "actuators/leds");
#endif

	SENSORS_ACTIVATE(button_sensor1);
	SENSORS_ACTIVATE(temperature_sensor);
	/* Define application-specific events here. */
	while(1) {
		PROCESS_WAIT_EVENT();
#if PLATFORM_HAS_BUTTON
		if(ev == sensors_event && data == &button_sensor1) {
			PRINTF("*******BUTTON*******\r\n");

			/* Call the event_handler for this application-specific event. */
			res_event.trigger();

			/* Also call the separate response example handler. */
			res_separate.resume();
		}
#endif /* PLATFORM_HAS_BUTTON */
	}
	/* while (1) */

	PROCESS_END();
}

PROCESS_THREAD(dhcpc_process, ev, data)
{
	DEV_WNIC_PTR wifi;
	PROCESS_BEGIN();

	PRINTF("\r\nNow trying to connect to WIFI hotspot, Please wait about 30s!\r\n");
	/* wait wifi connected */
	while (1) {
		PROCESS_WAIT_EVENT();
		if (ev == wnic_event) {
			wifi = (DEV_WNIC_PTR)data;
			if (wifi->wnic_info.conn_status == WNIC_CONNECTED) {
				break;
			}
		}
	}
	PRINTF("Now start get ip address using DHCP, Please wait about 30s!\r\n");

	dhcpc_init(uip_lladdr.addr, sizeof(uip_lladdr.addr));
	dhcpc_request();

	while(1) {
		PROCESS_WAIT_EVENT();
		if (ev == PROCESS_EVENT_EXIT) {
			process_exit(&dhcpc_process);
		}
		dhcpc_appcall(ev, data);
	}

	PROCESS_END();
}

void dhcpc_configured(const struct dhcpc_state *s)
{
	uip_sethostaddr(&s->ipaddr);
	uip_setnetmask(&s->netmask);
	uip_setdraddr(&s->default_router);
	uip_nameserver_update((uip_ipaddr_t *)&s->dnsaddr, UIP_NAMESERVER_INFINITE_LIFETIME);
	ip_addr_dump("ip", &s->ipaddr);
	ip_addr_dump("netmask", &s->netmask);
	ip_addr_dump("gateway", &s->default_router);
	ip_addr_dump("dns", &s->dnsaddr);
	PRINTF("\r\n");
	process_start(&er_example_server, NULL);
	process_start(&sensors_process, NULL);
}

void dhcpc_unconfigured(const struct dhcpc_state *s)
{
	PRINTF("DHCP dhcpc_unconfigured\r\n");
}

/** @} */