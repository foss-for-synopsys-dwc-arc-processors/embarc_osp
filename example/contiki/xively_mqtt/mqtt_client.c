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
--------------------------------------------- */

/**
 * \defgroup	EMBARC_APP_CONTIKI_XIVELY_MQTT	embARC Contiki Xively MQTT Client Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_CONTIKI
 * \ingroup	EMBARC_APPS_MID_MQTT
 * \brief	embARC Example for MQTT Client to Xively IoT Cloud on Contiki
 *
 * \details
 * ### Extra Required Tools
 * ### Extra Required Peripherals
 *     * Digilent PMOD WIFI(MRF24WG0MA)
 *     * Digilent PMOD TMP2
 *
 * ### Design Concept
 *     This is an example running mqtt client communicate with xively using mqtt protocal, sometimes it may fail, since connection to xively cloud is not always good enough.
 *     - This example update button/switch/led/temperature data to the [xively cloud](https://personal.xively.com/dev/docs/api/communicating/mqtts/) by mqtt connection
 *
 * ### Usage Manual
 *     * Hardware:
 *         - EMSK node: PMOD WiFi to J5, PMOD temperature to J2 on \ref EMBARC_BOARD_CONNECTION "EMSK"
 *     * WiFi Network: *SSID:embARC, PASSWORD:qazwsxedc*, the network should have access to internet.
 *     * Run steps
 *     	   - build and run.
 *     	   - EMSK node will connect to the mqtt server of xively.
 *     	   - data will be exchanged between EMSK node and xively.
 *
 *     ![ScreenShot of contiki mqtt client](pic/images/example/emsk/emsk_contiki_mqtt_client.jpg)
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup EMBARC_APP_CONTIKI_XIVELY_MQTT
 * \brief MQTT client example
 */

/**
 * \addtogroup	EMBARC_APP_CONTIKI_XIVELY_MQTT
 * @{
 */

#include <stdio.h>
#include <string.h>

#include "embARC.h"
#include "embARC_debug.h"

#include "contiki-net.h"
#include "dev/sensors/temperature-sensor.h"
#include "mqtt-service.h"
#include "net/mrf24g/mrf24g_drv.h"
#include "net/ip/dhcpc.h"
#include "dev/leds.h"

#define XI_FEED_ID	"/v2/feeds/1812612448.csv" // set Xively Feed ID (numerical, no quoutes)
#define XI_API_KEY	"JssbmmyShlH1IKVYKfIleaS7CtpyAXNCS0CBDtm05u0QENiu" // set Xively API key (double-quoted string)
#define XI_HOST		"api.xively.com"
#define XI_LED		"/v2/feeds/1812612448/datastreams/led.csv"

#define ip_addr_dump(msg, ipaddr) \
		EMBARC_PRINTF("%s:%d.%d.%d.%d  ",  (msg),				\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[0] : 0),	\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[1] : 0),	\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[2] : 0),	\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[3] : 0))


SUBSCRIBE_TOPIC(leds, XI_LED, 0);

void leds_handler(mqtt_event_data_t *event_data)
{
	long val;
	char *s;
	int len;
	s = strchr(event_data->data, ',');
	s++;
	len = event_data->data_length - (s - event_data->data);
	s[len]='\0';
	EMBARC_PRINTF(">>XILVEY2BOARD : led %s\r\n", s);
	xatoi(&s, &val);
	leds_arch_set((unsigned char)val);
}

#define UPDATE_PERIOD_SND		(5)

PROCESS(dhcpc_process, "dhcpc_process");
PROCESS(mqtt_client_process, "mqtt clinet process");

AUTOSTART_PROCESSES(&dhcpc_process);

PROCESS_THREAD(mqtt_client_process, ev, data)
{
	static struct etimer et;
	static int err_count = 0;
	static float temp_val;
	static uint32_t btn_val, swt_val, led_val = 0;

	// Allocate buffer space for the MQTT client
	static uint8_t in_buffer[256];
	static uint8_t out_buffer[256];
	static char msg_string[256];

	// Setup an mqtt_connect_info_t structure to describe
	// how the connection should behave
	static mqtt_connect_info_t connect_info =
	{
		.client_id = "EMSK",
		.username = XI_API_KEY,
		.password = NULL,
		.will_topic = NULL,
		.will_message = NULL,
		.keepalive = 60,
		.will_qos = 0,
		.will_retain = 0,
		.clean_session = 1
	};

	uip_ipaddr_t *server_ip;
	resolv_status_t dns_status;
	PROCESS_BEGIN();

	while (1) {
		dns_status = resolv_lookup(XI_HOST, &server_ip);
		if (dns_status != RESOLV_STATUS_CACHED) {
			if (dns_status ==  RESOLV_STATUS_NOT_FOUND || dns_status == RESOLV_STATUS_ERROR) {
				EMBARC_PRINTF( "Getting Host by name [failed]\r\n" );
				PROCESS_EXIT(); /* fatal error */
			}
			if (dns_status != RESOLV_STATUS_RESOLVING) {
				resolv_query(XI_HOST);
			}
		} else {
			err_count = 0;
			break;
		}
		etimer_set(&et, CLOCK_SECOND * 5);
		PROCESS_WAIT_EVENT();
		if (ev == resolv_event_found && !strcmp((char *)data, XI_HOST)) {
			etimer_stop(&et);
			EMBARC_PRINTF("Got MQTT server IP by DNS\r\n");
		} else if (etimer_expired(&et)) {
			err_count++;
			if (err_count > 5)
			{
				EMBARC_PRINTF("DNS timeout\r\n");
				err_count = 0;
				PROCESS_EXIT();
			}
		}
	}


	// Initialise the MQTT client
	mqtt_init(in_buffer, sizeof(in_buffer), out_buffer, sizeof(out_buffer));

	// Ask the client to connect to the server
	// and wait for it to complete.
	mqtt_connect(server_ip, UIP_HTONS(1883), 1, &connect_info);
	PROCESS_WAIT_EVENT_UNTIL(ev == mqtt_event);

	if (mqtt_connected()) {
		EMBARC_PRINTF("---mqtt server connected---\r\n");

		mqtt_subscribe(&subscribe_topic_leds);
		PROCESS_WAIT_EVENT_UNTIL(ev == mqtt_event);
		if (((mqtt_event_data_t *)data)->type != MQTT_EVENT_TYPE_SUBSCRIBED) {
			EMBARC_PRINTF("---subscribe topic failed---\r\n");
		}

		while (1) {
			temp_val = temperature_sensor.value(0);
			temp_val *= 0.0625;
			btn_val = button_read(BOARD_BTN_MASK);
			swt_val = switch_read(BOARD_SWT_MASK);
			led_val += 5;
			led_val = led_val & 0xff;
			led_write(led_val, BOARD_LED_MASK);
			sprintf(msg_string, "temperature,%.3f\nbutton,%d\nled,%d\nswitch,%d", \
				temp_val, btn_val, led_val, swt_val);
			mqtt_publish(XI_FEED_ID, msg_string, 0, 1);
			PROCESS_WAIT_EVENT_UNTIL(ev == mqtt_event);
			if (((mqtt_event_data_t *)data)->type != MQTT_EVENT_TYPE_PUBLISHED) {
				EMBARC_PRINTF("---data publish failed, try again---\r\n");
			} else {
				EMBARC_PRINTF("<<BOARD2XIVELY : temperature,%d  button,%d  led,%d  switch,%d\r\n", \
					(int)(temp_val), btn_val, led_val, swt_val);
			}
			etimer_set(&et, CLOCK_SECOND * UPDATE_PERIOD_SND);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		}
	} else {
		EMBARC_PRINTF("mqtt client connect failed\r\n");
	}

	PROCESS_END();

}

PROCESS_THREAD(dhcpc_process, ev, data)
{
	DEV_WNIC_PTR wifi;

	PROCESS_BEGIN();

	SENSORS_ACTIVATE(temperature_sensor);
	process_start(&tcpip_process, NULL);
	process_start(&mrf24g_process, NULL);

	EMBARC_PRINTF("---Mqtt client demo based on Contiki---\r\n");
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
	EMBARC_PRINTF("\r\nNow start get ip address using DHCP, Please wait about 30s!\n\n");
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
	process_start(&mqtt_client_process, NULL);
	/* when ip and dns server is configured, start other processes */
}

void dhcpc_unconfigured(const struct dhcpc_state *s)
{
	EMBARC_PRINTF("DHCP dhcpc_unconfigured\r\n");
}

/** @} */
