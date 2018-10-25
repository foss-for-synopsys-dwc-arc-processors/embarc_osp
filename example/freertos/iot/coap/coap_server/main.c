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
 * \defgroup	EMBARC_APP_FREERTOS_IOT_COAP_COAP_SERVER	embARC LwIP CoAP Server Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_FREERTOS
 * \ingroup	EMBARC_APPS_MID_LWIP
 * \ingroup	EMBARC_APPS_MID_COAP
 * \brief	embARC Example for CoAP Protocol on FreeRTOS
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
 *     This example is designed to show how CoAP server running on FreeRTOS.
 *
 * ### Usage Manual
 *     This is an example running coap server on LwIP with FreeRTOS.
 *     The Pmod modules should be connected to \ref EMBARC_BOARD_CONNECTION "EMSK".
 *     - This is a example similiar but simpler than the contiki coap_server one,
 *     so you can almost following the instruction list in contiki coap_server example,
 *     you need to check if you can ping emsk board, sometimes you need to clear arp table by run *arp -d*.
 *     - Open Firefox and enter *coap://<your board ip address>*, and then you can test the coap server example.
 *     - Know issues: temperature obeserve will hang up.
 *
 *     ![ScreenShot of coap_server under freertos](pic/images/example/emsk/emsk_lwip_freertos_coap_server.jpg)
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_IOT_COAP_COAP_SERVER
 * \brief	lwip coap server example
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_IOT_COAP_COAP_SERVER
 * @{
 */
#include "embARC.h"
#include "embARC_debug.h"
#include "lwip/netif.h"
#include "coap.h"

#if defined(BOARD_EMSK)
#include "adt7420.h"
#endif

#define min(x,y) ((x) < (y) ? (x) : (y))

#define TSKPRI_COAP_SERVER	(configMAX_PRIORITIES-2)	  /**< coap server task priority */

static void task_coap_server(void *par);

static TaskHandle_t task_coap_server_handle = NULL;
static coap_resource_t *temp_resource = NULL;

#if defined(BOARD_EMSK)
#define TSKPRI_TEMPTX		(configMAX_PRIORITIES-3)
static TaskHandle_t task_temptx_handle = NULL;
static void task_temp_tx(void *par);
static ADT7420_DEFINE(temp, BOARD_TEMP_SENSOR_IIC_ID, TEMP_I2C_SLAVE_ADDRESS);
#endif

int main(void)
{
	if (task_coap_server_handle != NULL) {
		return 0;
	}
	if (xTaskCreate(task_coap_server, "coap server", 512, (void *)1,
		TSKPRI_COAP_SERVER, &task_coap_server_handle) != pdPASS) {
		EMBARC_PRINTF("create coap server failed\r\n");
		return -1;
	}

	return 0;
}

#define HELLO "hello world"

static void hnd_get_hello(coap_context_t  *ctx, struct coap_resource_t *resource,
			coap_address_t *peer, coap_pdu_t *request, str *token,
			coap_pdu_t *response) {
	unsigned char buf[3];

	response->hdr->code = COAP_RESPONSE_CODE(205);

	coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
		coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);

	coap_add_option(response, COAP_OPTION_MAXAGE,
		coap_encode_var_bytes(buf, 0x2ffff), buf);

	coap_add_data(response, strlen(HELLO), (unsigned char *)HELLO);
}


static void hnd_get_temp(coap_context_t  *ctx, struct coap_resource_t *resource,
			coap_address_t *peer, coap_pdu_t *request, str *token,
			coap_pdu_t *response) {

	coap_opt_iterator_t opt_iter;
	unsigned char buf[40];
	size_t len;
	float temp_val = 10.0;
	coap_subscription_t *subscription;

	response->hdr->code = COAP_RESPONSE_CODE(205);

	/* handle observe request */
	if (request != NULL &&
		coap_check_option(request, COAP_OPTION_OBSERVE, &opt_iter)) {
		subscription = coap_add_observer(resource, peer, token);
		if (subscription) {
			subscription->non = request->hdr->type == COAP_MESSAGE_NON;
			coap_add_option(response, COAP_OPTION_OBSERVE, 0, NULL);
		}
	}

	if (resource->dirty == 1) {
		coap_add_option(response, COAP_OPTION_OBSERVE,
			coap_encode_var_bytes(buf, ctx->observe), buf);
	}

	coap_add_option(response, COAP_OPTION_CONTENT_FORMAT,
			coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);

	/* temp data is fresh in 60 seconds */
	coap_add_option(response, COAP_OPTION_MAXAGE,
		coap_encode_var_bytes(buf, 60), buf);

#if defined(BOARD_EMSK)
	if (adt7420_sensor_read(temp, &temp_val) != E_OK) {
		temp_val = 0.0f;
	}
#endif

	len = snprintf((char *)buf,
			min(sizeof(buf), response->max_size - response->length),
			"%f", temp_val);
	coap_add_data(response, len, buf);
}

static void hnd_post_leds(coap_context_t  *ctx, struct coap_resource_t *resource,
			coap_address_t *peer, coap_pdu_t *request, str *token,
			coap_pdu_t *response) {

	long val;
	size_t size;
	unsigned char *data;

	response->hdr->code = COAP_RESPONSE_CODE(205);

	coap_get_data(request, &size, &data);

	if (size) {
		data[size] = '\0';	/* borrow one byte in pbuf */
		xatoi((char **)&data, &val);
		led_write(val, 0xffffffff);
	}
}


void init_resources(coap_context_t *ctx) {
	coap_resource_t *r;

	/* register hello world resource */
	r = coap_resource_init((unsigned char *)"hello", 5, 0);
	coap_register_handler(r, COAP_REQUEST_GET, hnd_get_hello);

	coap_add_attr(r, (unsigned char *)"ct", 2, (unsigned char *)"0", 1, 0);
	coap_add_attr(r, (unsigned char *)"title", 5, (unsigned char *)"\"Hello World\"", 13, 0);
	coap_add_resource(ctx, r);
	/* register temperature resource */
	r = coap_resource_init((unsigned char *)"temperature", 11, 0);
	coap_register_handler(r, COAP_REQUEST_GET, hnd_get_temp);

	coap_add_attr(r, (unsigned char *)"ct", 2, (unsigned char *)"0", 1, 0);
	coap_add_attr(r, (unsigned char *)"title", 5, (unsigned char *)"\"Temperature\"", 13, 0);
	coap_add_resource(ctx, r);
	r->observable = 1;
	temp_resource = r;
	/* register led  resources */
	r = coap_resource_init((unsigned char *)"leds", 4, 0);
	coap_register_handler(r, COAP_REQUEST_POST, hnd_post_leds);

	coap_add_attr(r, (unsigned char *)"ct", 2, (unsigned char *)"0", 1, 0);
	coap_add_attr(r, (unsigned char *)"title", 5, (unsigned char *)"\"LEDs\"", 6, 0);

	coap_add_resource(ctx, r);

#ifndef WITHOUT_ASYNC
#endif /* WITHOUT_ASYNC */
}

#ifndef WITHOUT_ASYNC
#endif /* WITHOUT_ASYNC */


static void task_coap_server(void *par)
{
	coap_context_t  *ctx;
	coap_address_t addr;

	addr.port = 5683;
	addr.addr = netif_default->ip_addr;

	ctx = coap_new_context(&addr);

	if (!ctx) {
		EMBARC_PRINTF("create coap context error, run failed!\r\n");
		vTaskDelete(NULL);
		return;
	}

	init_resources(ctx);

#if defined(BOARD_EMSK)
	if (xTaskCreate(task_temp_tx, "temperature update task", 128, (void *)1,
		TSKPRI_TEMPTX, &task_temptx_handle) != pdPASS) {
		EMBARC_PRINTF("create temperature update task failed\r\n");
		vTaskDelete(NULL);
		return;
	}
#endif

	/**
	 * every 100 ticks, the server will the incoming data.
	 * FIXME: add semaphore post in received_package(coap/net.c) to
	 *  notify  this task to dispatch
	 */
	while (1) {
		coap_dispatch(ctx);
		coap_check_notify(ctx);
		vTaskDelay(100);
	}
	coap_free_context(ctx);
}

#if defined(BOARD_EMSK)
static void task_temp_tx(void *par)
{
	adt7420_sensor_init(temp);
	EMBARC_PRINTF("temperature update running\r\n");
	while (1) {
		temp_resource->dirty = 1;
		vTaskDelay(2000);
	}
}
#endif

/** @} */