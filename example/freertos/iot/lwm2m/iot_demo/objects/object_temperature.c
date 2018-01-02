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

/*
 * Implements an object for testing purpose
 *
 *                 Multiple
 * Object |  ID  | Instances | Mandatoty |
 *  Test  | 3303 |    Yes    |    No     |
 *
 *  Ressources:
 *              Supported    Multiple
 *  Name           |  ID  | Operations | Instances | Mandatory |  Type   | Range | Units | Description |
 *  value          | 5700 |     R      |    No     |    Yes    |  Float  |       |       |             |
 *  Units          | 5701 |     R      |    No     |    No     |  String |       |       |             |
 *  Min Measured   | 5601 |     R      |    No     |    No     |  Float  |       |       |             |
 *  Max Measured   | 5602 |     R      |    No     |    No     |  Float  |       |       |             |
 *  Reset Measured | 5605 |     E      |    No     |    No     |         |       |       |             |
 *\
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "embARC.h"
#include "embARC_debug.h"
#include "adt7420.h"

#define OBJECT_TEMP_ID		3303

#define LWM2M_VALUE		5700
#define LWM2M_MIN_MEASURED	5601
#define LWM2M_MAX_MEASURED	5602
#define LWM2M_RESET_MEASURED	5605


#define LWM2M_EMSK_INSTANCE_ID  0

#define BUTTON_SENSOR_ID    EMSK_GPIO_PORT_A

static TaskHandle_t task_lwm2m_temperature_handle = NULL;
static void task_lwm2m_temperature(void* par);

static lwm2m_object_t * tempure_obj;
static ADT7420_DEFINE(temp, BOARD_TEMP_SENSOR_IIC_ID, BOARD_TEMP_IIC_SLVADDR);
extern xQueueHandle queue_observe;

/*
 * Multiple instance objects can use userdata to store data that will be shared between the different instances.
 * The lwm2m_object_t object structure - which represent every object of the liblwm2m as seen in the single instance
 * object - contain a chained list called instanceList with the object specific structure prv_instance_t:
 */
typedef struct _prv_instance_
{
	/*
	 * The first two are mandatories and represent the pointer to the next instance and the ID of this one. The rest
	 * is the instance scope user data (uint8_t test in this case)
	 */
	struct _prv_instance_ * next;   // matches lwm2m_list_t::next
	uint16_t shortID;               // matches lwm2m_list_t::id
	float value;
	float min_measured;
	float max_measured;
} prv_instance_t;

static uint8_t prv_read(uint16_t instanceId,
						int * numDataP,
						lwm2m_tlv_t ** dataArrayP,
						lwm2m_object_t * objectP)
{
	prv_instance_t * instance_ptr;
	int i;

	instance_ptr = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
	if (NULL == instance_ptr) return COAP_404_NOT_FOUND;

	if (*numDataP == 0) {
		*dataArrayP = lwm2m_tlv_new(3);
		if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
		*numDataP = 3;
		(*dataArrayP)[0].id = LWM2M_VALUE;
		(*dataArrayP)[1].id = LWM2M_MIN_MEASURED;
		(*dataArrayP)[2].id = LWM2M_MAX_MEASURED;
	}

	for (i = 0 ; i < *numDataP ; i++) {
		switch ((*dataArrayP)[i].id) {
			case LWM2M_VALUE:
				(*dataArrayP)[i].type = LWM2M_TYPE_RESOURCE;
				lwm2m_tlv_encode_float(instance_ptr->value, *dataArrayP + i);
				break;

			case LWM2M_MIN_MEASURED:
				(*dataArrayP)[i].type = LWM2M_TYPE_RESOURCE;
				lwm2m_tlv_encode_float(instance_ptr->min_measured, *dataArrayP + i);
				break;

			case LWM2M_MAX_MEASURED:
				(*dataArrayP)[i].type = LWM2M_TYPE_RESOURCE;
				lwm2m_tlv_encode_float(instance_ptr->max_measured, *dataArrayP + i);
				break;

			case LWM2M_RESET_MEASURED:
				return COAP_405_METHOD_NOT_ALLOWED;

			default:
				return COAP_404_NOT_FOUND;
		}

		if ((*dataArrayP)[i].length == 0) return COAP_500_INTERNAL_SERVER_ERROR;
	}

	return COAP_205_CONTENT;
}

static uint8_t prv_write(uint16_t instanceId, int numData, lwm2m_tlv_t * dataArray,
			lwm2m_object_t * objectP)
{
	return COAP_405_METHOD_NOT_ALLOWED;
}

static uint8_t prv_delete(uint16_t id, lwm2m_object_t * objectP)
{
	return COAP_405_METHOD_NOT_ALLOWED;
}

static uint8_t prv_create(uint16_t instanceId, int numData, lwm2m_tlv_t * dataArray,
			lwm2m_object_t * objectP)
{
	return COAP_405_METHOD_NOT_ALLOWED;
}

static uint8_t prv_exec(uint16_t instanceId, uint16_t resourceId, uint8_t * buffer,
			int length, lwm2m_object_t * objectP)
{
	return COAP_405_METHOD_NOT_ALLOWED;
}

static void prv_close(lwm2m_object_t * objectP)
{
	LWM2M_LIST_FREE(objectP->instanceList);
	if (objectP->userData != NULL) {
		lwm2m_free(objectP->userData);
		objectP->userData = NULL;
	}
}

static void prv_temperature_set(uint8_t temperature_id, uint8_t enable)
{
	if(enable) {
		adt7420_sensor_init(temp);
		if (xTaskCreate(task_lwm2m_temperature, "lwm2m temperature", 2048, (void *)1,
			0, &task_lwm2m_temperature_handle) != pdPASS) {
			EMBARC_PRINTF("create lwm2m client failed\r\n");
		}
	} else {
		vTaskSuspend(task_lwm2m_temperature_handle);
	}

}

lwm2m_object_t * get_object_temperature(uint32_t num)
{
	lwm2m_object_t * obj;
	obj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

	if (NULL != obj) {
		int i;
		prv_instance_t * instance_ptr;

		memset(obj, 0, sizeof(lwm2m_object_t));

		obj->objID = OBJECT_TEMP_ID;
		for (i = 0 ; i < num ; i++)
		{
			instance_ptr = (prv_instance_t *)lwm2m_malloc(sizeof(prv_instance_t));
			if (NULL == instance_ptr) {
				lwm2m_free(obj);
				return NULL;
			}
			memset(instance_ptr, 0, sizeof(prv_instance_t));
			instance_ptr->shortID = LWM2M_EMSK_INSTANCE_ID + i;
			instance_ptr->value = 0.0;
			instance_ptr->min_measured = 100.0;
			instance_ptr->max_measured = 0.0;
			obj->instanceList = LWM2M_LIST_ADD(obj->instanceList, instance_ptr);
			prv_temperature_set(i, 1);
		}
		/*
		 * From a single instance object, two more functions are available.
		 * - The first one (createFunc) create a new instance and filled it with the provided informations. If an ID is
		 *   provided a check is done for verifying his disponibility, or a new one is generated.
		 * - The other one (deleteFunc) delete an instance by removing it from the instance list (and freeing the memory
		 *   allocated to it)
		 */
		obj->readFunc    = prv_read;
		obj->writeFunc   = prv_write;
		obj->createFunc  = prv_create;
		obj->deleteFunc  = prv_delete;
		obj->executeFunc = prv_exec;
		obj->closeFunc   = prv_close;
	}
	tempure_obj = obj;
	return obj;
}

static void task_lwm2m_temperature(void* par)
{
	prv_instance_t * instance_ptr;
	char *buff = "/3303/0/5700";
	instance_ptr = (prv_instance_t *)lwm2m_list_find(tempure_obj->instanceList, 0);
	float value;
	TickType_t xLastExecutionTime;
	xLastExecutionTime = xTaskGetTickCount();
	while(1){
		adt7420_sensor_read(temp, &value);
		instance_ptr -> value = value;
		xQueueSendToFrontFromISR(queue_observe, (void *)(&buff), 0);
		vTaskDelayUntil( &xLastExecutionTime, 5000);
	}
}