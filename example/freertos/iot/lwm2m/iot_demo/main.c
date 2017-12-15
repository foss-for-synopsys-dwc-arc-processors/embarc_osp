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
 * \date 2016-07-25
 * \author Xinyi Zhao(xinyi@synopsys.com)
--------------------------------------------- */
/**
 * \defgroup	EMBARC_APP_FREERTOS_IOT_LWM2M_IOT_DEMO	embARC IoT Demo with LwM2M Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_FREERTOS
 * \ingroup	EMBARC_APPS_MID_LWM2M
 * \ingroup	EMBARC_APPS_MID_LWIP
 * \ingroup	EMBARC_APPS_MID_PARSON
 * \ingroup	EMBARC_APPS_MID_U8GLIB
 * \ingroup	EMBARC_APPS_MID_FATFS
 * \brief	embARC Example for LwM2M used in IoT Demo
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     * Digilent PMOD WIFI(MRF24WG0MA)
 *     * Digilent PMOD TMP2
 *
 * ### Design Concept
 *     This example is designed to show how to use LwM2M client in a full IoT demo running on FreeRTOS.
 *
 * ### Usage Manual
 *
 *
 * ### Extra Comments
 *    A detailed application note will be provided to explain how to run this demo.
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_IOT_LWM2M_IOT_DEMO
 * \brief	main source of iot demo using lwm2m
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_IOT_LWM2M_IOT_DEMO
 * @{
 */
#include "embARC.h"
#include "embARC_debug.h"
#include "lwm2mclient.h"
#include "parson.h"
#include <stdlib.h>

#define SERVER_HOST		"192.168.2.1"
#define CLIENT_NAME		"embARC"
#define CLIENT_MODEL		"default"
#define SERVER_PORT		"5683"
#define CLIENT_LIFETIME		300
#define CFG_SIZE		500
#define TSKPRI_LWM2M_CLIENT	(configMAX_PRIORITIES-2)	/**< lwm2m task priority */

static TaskHandle_t task_lwm2m_observe_handle = NULL;

void task_lwm2m_observe(void *par);

xQueueHandle queue_observe;
lwm2m_client_info c_info;

static int32_t add_obj(uint32_t oid, uint32_t number)
{
	static uint32_t first_flag = 1;
	static obj_info **next_obj;
	obj_info *new_obj;

	if(first_flag == 1) {
		next_obj = &(c_info.obj);
		first_flag = 0;
	}

	new_obj = (obj_info *)malloc(sizeof(obj_info));
	if (new_obj == NULL) {
		return -1;
	}
	*next_obj = new_obj;

	new_obj->oid = oid;
	new_obj->number = number;

	new_obj->next = NULL;
	next_obj = &(new_obj->next);
	return 0;
}

int main(void)
{
	/*parse the configuration first*/
	FIL file;
	JSON_Value *cfg_json;
	char *app_cfg = (char *)0x17f00000;
	uint32_t i;
	char *cfg_str = NULL;

	c_info.server = SERVER_HOST;
	c_info.ep_name = CLIENT_NAME;
	c_info.serverPort = SERVER_PORT;
	c_info.lifetime = CLIENT_LIFETIME;
	c_info.model = CLIENT_MODEL;

	if (f_open(&file, "lwm2m.json", FA_READ)) {
		 EMBARC_PRINTF("open cfg file failed\r\n");
		 return -1;
	} else {
		if (file.fsize > CFG_SIZE) {
			EMBARC_PRINTF("cfg file is too big, max size is %d bytes\r\n", CFG_SIZE);
			return -1;
		} else {
			cfg_str = (char *)malloc(file.fsize);
			if (cfg_str == NULL) {
				EMBARC_PRINTF("malloc buffer for cfg file failed\r\n");
				return -1;
			}
			f_read(&file, cfg_str, file.fsize, &i);
			f_close(&file);
		}
	}

	cfg_json = json_parse_string(cfg_str);
	free(cfg_str);

	if (cfg_json != NULL) {
		/*parse the boot.json*/
		const char *str;
		str = json_object_get_string(json_object(cfg_json), "host");
		if (str != NULL) {
			c_info.server = str;
		}
		str = json_object_get_string(json_object(cfg_json), "port");
		if (str != NULL) {
			c_info.serverPort = str;
		}
		str = json_object_get_string(json_object(cfg_json), "name");
		if (str != NULL) {
			c_info.ep_name = str;
		}
		str = json_object_get_string(json_object(cfg_json), "model");
		if (str != NULL) {
			c_info.model = str;
		}
		int32_t num;
		num = json_object_get_number(json_object(cfg_json), "lifetime");
		if (num != -1) {
			c_info.lifetime = num;
		}
		if (!strcmp(c_info.model, "all")) {
			add_obj(3303, 1);
			add_obj(3311, 8);
			add_obj(3347, 3);
			add_obj(3341, 5);
		} else if (!strcmp(c_info.model, "button")) {
			add_obj(3347, 3);
			add_obj(3303, 1);
		} else if (!strcmp(c_info.model, "light")) {
			add_obj(3311, 1);
			add_obj(3347, 1);
		} else if (!strcmp(c_info.model, "temperature")) {
			add_obj(3303, 1);
		} else if (!strcmp(c_info.model, "display")) {
			add_obj(3341, 5);
		} else if (!strcmp(c_info.model, "default")) {
			JSON_Array *array;
			JSON_Object *obj;
			uint32_t oid;
			array = json_object_get_array(json_object(cfg_json), "obj");
			if (array != 0) {
				i = 0;
				while ((obj = json_array_get_object(array, i))) {
					num = json_object_get_number(obj, "oid");
					if (num != -1) {
						oid = num;
						num = json_object_get_number(obj, "number");
						if (num != -1) {
							add_obj(oid, num);
						}
					}
					i++;
				}
			}
		} else {
			EMBARC_PRINTF("unknow client model: %s\n", c_info.model);
		}

	} else {
		EMBARC_PRINTF("Cannot parse %s, please check it. Now use the default setting\r\n", app_cfg);
	}

	EMBARC_PRINTF("cfg:\n%s\n%s\n%s\n%d\n%s\n", c_info.server, c_info.serverPort, c_info.ep_name, c_info.lifetime, c_info.model);

	while (1) {

		if (lwm2mclient(&c_info) == 0) {
			EMBARC_PRINTF("LwM2M client end successfully\r\n");
		} else {
			EMBARC_PRINTF("LwM2M client end failed\r\n");
		}


		if (c_info.server != NULL) free((void *)(c_info.server));
		if (c_info.ep_name != NULL) free((void *)(c_info.ep_name));
		if (c_info.serverPort != NULL) free((void *)(c_info.serverPort));

		vTaskSuspend(NULL);
	}

}

void system_setValueChangedHandler(lwm2m_context_t *lwm2mH, CHANGE_HANDLER prv_value_change)
{
	static uint32_t par[2];

	par[0] = (uint32_t)lwm2mH;
	par[1] = (uint32_t)prv_value_change;

	if (xTaskCreate(task_lwm2m_observe, "lwm2m observe", 2048, (void *)par,
		TSKPRI_LWM2M_CLIENT, &task_lwm2m_observe_handle) != pdPASS) {
		EMBARC_PRINTF("create lwm2m observe failed\r\n");
	}
}

void task_lwm2m_observe(void *par)
{
	queue_observe = xQueueCreate(5, 4);
	portBASE_TYPE err;
	uint32_t *pt;
	char *uri;
	char **buffer;
	pt = (uint32_t *)par;
	lwm2m_context_t *lwm2mH = (lwm2m_context_t *)(pt[0]);
	CHANGE_HANDLER prv_value_change = (CHANGE_HANDLER)(pt[1]);
	while (1) {
		err = xQueueReceive(queue_observe, buffer, portMAX_DELAY);
		if (err != pdPASS) continue;
		if (*buffer != NULL) {
			uri = (char *)(*buffer);
			prv_value_change(lwm2mH, uri, NULL, 0);
		}
	}
}