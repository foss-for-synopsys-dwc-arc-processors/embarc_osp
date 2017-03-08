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
 * \date 2016-05-03
 * \author Huaqi Fang(Huaqi.Fang@synopsys.com)
----------------------------------------------*/

/**
 * \defgroup	EMBARC_APP_CONTIKI_XIVELY_HTTP	embARC Contiki Xively HTTP Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_CONTIKI
 * \ingroup	EMBARC_APPS_MID_XIVELY
 * \brief	embARC Example for xively http library on Contiki
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     * Digilent PMOD WIFI(MRF24WG0MA)
 *     * Digilent PMOD TMP2
 *
 *
 * ### Design Concept
 *     This example shows how to use the Xively library port for Contiki
 *
 * ### Usage Manual
 *     This is a One-node Xively IoT example.
 *     * Purpose: Test Contiki support for Xively
 *     * Hardware: WiFi Pmod connected to J5, Temp Pmod connected to J2 on \ref EMBARC_BOARD_CONNECTION "EMSK"
 *     * Software: Contiki (uIP) + common library + Xively library
 *     * WiFi Network: *SSID:embARC, PASSWORD:qazwsxedc*
 *     * Run steps:
 *         - Load and run the application
 *         - Application will try to connect to WiFi. After it connected, it will automatically get the IP configuration using DHCP
 *         - Application will get sensor data about every 4 seconds and send it to Xively
 *
 *     ![ScreenShot of Xively feed update under Contiki](pic/images/example/emsk/emsk_contiki_xively.jpg)
 *
 * ### Extra Comments
 *
 */

/**
 * \file example-xively.c
 * \brief one node xively demo based on Contiki
 * \ingroup	EMBARC_APP_CONTIKI_XIVELY_HTTP
 */
/**
 * \addtogroup	EMBARC_APP_CONTIKI_XIVELY_HTTP
 * @{
 */
#include "embARC.h"
#include "embARC_debug.h"

#include "dev_wnic.h"
#include "net/mrf24g/mrf24g_drv.h"
#include "contiki-net.h"
#include "net/ip/dhcpc.h"

#include "xively.h"
#include "xi_err.h"
#include "nob/nob_runner.h"
#include "xively_contiki_data.h"

#define XI_FEED_ID 1812612448 // set Xively Feed ID (numerical, no quoutes)
#define XI_API_KEY "JssbmmyShlH1IKVYKfIleaS7CtpyAXNCS0CBDtm05u0QENiu" // set Xively API key (double-quoted string)


#define ip_addr_dump(msg, ipaddr) \
		EMBARC_PRINTF("%s:%d.%d.%d.%d  ",  (msg),				\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[0] : 0),	\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[1] : 0),	\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[2] : 0),	\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[3] : 0))


PROCESS(main_process, "main_process");
PROCESS(dhcpc_process, "dhcpc_process");
PROCESS(feed_sample_process, "feed sample process");

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&main_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(main_process, ev, data)
{
	PROCESS_BEGIN();
	process_start(&tcpip_process, NULL);
	process_start(&mrf24g_process, NULL);
	process_start(&dhcpc_process, NULL);
	EMBARC_PRINTF("------contiki xively example (one node)------\r\n");
	temp_sensor_init(BOARD_TEMP_IIC_SLVADDR);
	PROCESS_WAIT_UNTIL(ev == PROCESS_EVENT_EXIT);

	PROCESS_END();
}


static xi_feed_t feed;
static uint32_t btn_val, swt_val, led_val;
static int32_t temp_val;
#define UPDATE_PERIOD_SND		(4)

PROCESS_THREAD(feed_sample_process, ev, data)
{
	static struct etimer et;
	static int update_cnt = 0;
	static xi_context_t* xi_context;
	static xi_datapoint_t *current_swt, *current_btn, *current_led, *current_temp;
	static int update_status = 0;
	static int update_s = UPDATE_PERIOD_SND;

	int32_t val;
	int32_t state;

	PROCESS_BEGIN();

	memset( &feed, 0, sizeof( xi_feed_t ) );
	feed.feed_id = XI_FEED_ID;
	feed.datastream_count = 4;

	feed.datastreams[0].datapoint_count = 1;
	xi_datastream_t* swt_datastream = &feed.datastreams[0];
	strcpy( swt_datastream->datastream_id, "switch" );
	current_swt = &swt_datastream->datapoints[0];

	feed.datastreams[1].datapoint_count = 1;
	xi_datastream_t* btn_datastream = &feed.datastreams[1];
	strcpy( btn_datastream->datastream_id, "button" );
	current_btn = &btn_datastream->datapoints[0];

	feed.datastreams[2].datapoint_count = 1;
	xi_datastream_t* led_datastream = &feed.datastreams[2];
	strcpy( led_datastream->datastream_id, "led" );
	current_led = &led_datastream->datapoints[0];

	feed.datastreams[3].datapoint_count = 1;
	xi_datastream_t* temp_datastream = &feed.datastreams[3];
	strcpy( temp_datastream->datastream_id, "temperature" );
	current_temp = &temp_datastream->datapoints[0];

	// create the cosm library context
	xi_context = xi_create_context( XI_HTTP, XI_API_KEY, feed.feed_id );

	// check if everything works
	if ( xi_context == NULL ) {
		EMBARC_PRINTF( "create xi context failed...\r\n" );
		PROCESS_EXIT();
	}

	EMBARC_PRINTF("\r\nYou can Open URL : https://personal.xively.com/feeds/1812612448 to see data changes\r\n");
	EMBARC_PRINTF("Update Button, Switch, Led, Temperature Value to Xively IoT Cloud In About %ds Period\r\n", update_s);

	while(1) {
		etimer_set(&et, CLOCK_SECOND * 2);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		btn_val = button_read(BOARD_BTN_MASK);
		swt_val = switch_read(BOARD_SWT_MASK);
		//led_val = led_read(BOARD_LED_MASK);
		led_val += 5;
		led_val = led_val & 0xff;
		led_write(led_val, BOARD_LED_MASK);

		if (temp_sensor_read(&val) != E_OK) {
			temp_val = 0;
		} else {
			temp_val = val / 10;
		}

		xi_set_value_i32(current_swt, swt_val);
		xi_set_value_i32(current_btn, btn_val);
		xi_set_value_i32(current_led, led_val);
		xi_set_value_i32(current_temp, temp_val);

		xi_nob_feed_update(xi_context, &feed);

		while (1) {
			state = process_xively_nob_step(xi_context);
			if (state == LAYER_STATE_OK)  {
				update_status = 1;
				break;
			} else if (state == LAYER_STATE_WANT_WRITE || state == LAYER_STATE_WANT_READ) {
				PROCESS_WAIT_EVENT();
			} else if (state == LAYER_STATE_ERROR) {
				update_status = 0;
				update_s += 1;
				break;
			}
		}
		if (update_status == 1) {
			EMBARC_PRINTF( "update feed cnt:%d, switch:0x%x, button:0x%x, led:0x%x, temp:%d...\r\n",
				update_cnt++, swt_val, btn_val, led_val, temp_val);

			EMBARC_PRINTF( "done, wait about %ds, you can press button or change dip switch ...\r\n", update_s);
		} else {
			EMBARC_PRINTF("update failed, try to update data again and with 1s slow down...\r\n");
		}
	}

	PROCESS_END();

}


PROCESS_THREAD(dhcpc_process, ev, data)
{
	DEV_WNIC_PTR wifi;

	PROCESS_BEGIN();

	EMBARC_PRINTF("\r\nNow trying to connect to WIFI Hotspot, Please wait about 30s!\r\n");
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
	EMBARC_PRINTF("\r\nNow start get ip address using DHCP, Please wait about 30s!\r\n");
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
	EMBARC_PRINTF("\r\n");

	/* dns process */
	process_start(&resolv_process, NULL);
	/* when ip and dns server is configured, start other processes */
	process_start(&xively_process, NULL);

	process_start(&feed_sample_process, NULL);
}

void dhcpc_unconfigured(const struct dhcpc_state *s)
{
	EMBARC_PRINTF("DHCP dhcpc_unconfigured\r\n");
}
/** @} */