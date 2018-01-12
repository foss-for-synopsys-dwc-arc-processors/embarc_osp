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
/*
 * Implements an object for testing purpose
 *
 *                 Multiple
 * Object |  ID  | Instances | Mandatoty |
 *  Test  | 3347 |    Yes    |    No     |
 *
 *  Ressources:
 *              Supported    Multiple
 *  Name   |  ID  | Operations | Instances | Mandatory |  Type   | Range | Units | Description |
 *  State  | 5500 |     R      |    No     |    Yes    | Boolean |       |       |             |
 *  Counter| 5501 |     R      |    No     |    No     |   int   |       |       |             |
 *
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "embARC.h"
#include "embARC_debug.h"

#define OBJECT_BUTTON_ID        3347

#define LWM2M_STATE         	5500
#define LWM2M_COUNTER           5501


#define LWM2M_EMSK_INSTANCE_ID  0

#define BUTTON_SENSOR_ID    DEV_GPIO_PORT_A

static void button0_isr(void *ptr);
static void button1_isr(void *ptr);
static void button2_isr(void *ptr);
static lwm2m_object_t * button_obj;
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
	bool state;
	uint32_t counter;
} prv_instance_t;

static uint8_t prv_read(uint16_t instanceId, int * numDataP, lwm2m_tlv_t ** dataArrayP,
			lwm2m_object_t * objectP)
{
	prv_instance_t * instance_ptr;
	int i;

	instance_ptr = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
	if (NULL == instance_ptr) return COAP_404_NOT_FOUND;

	if (*numDataP == 0) {
		*dataArrayP = lwm2m_tlv_new(2);
		if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
		*numDataP = 2;
		(*dataArrayP)[0].id = LWM2M_STATE;
		(*dataArrayP)[1].id = LWM2M_COUNTER;
	}

	for (i = 0 ; i < *numDataP ; i++) {
		switch ((*dataArrayP)[i].id) {
			case LWM2M_STATE:
				(*dataArrayP)[i].type = LWM2M_TYPE_RESOURCE;
				if(instance_ptr->state == 0){
					instance_ptr->state = (bool)button_read(1 << instanceId);
				}
				lwm2m_tlv_encode_bool(instance_ptr->state, *dataArrayP + i);
				instance_ptr->state = 0;
				break;

			case LWM2M_COUNTER:
				(*dataArrayP)[i].type = LWM2M_TYPE_RESOURCE;
				lwm2m_tlv_encode_int(instance_ptr->counter, *dataArrayP + i);
				break;
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

static uint8_t prv_delete(uint16_t id,
			lwm2m_object_t * objectP)
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

static void prv_button_int(uint8_t button_id, uint8_t enable)
{
/*install the button isr*/
	DEV_GPIO_PTR port;
	uint32_t mask = (1 << button_id);
	DEV_GPIO_BIT_ISR bit_isr;
	DEV_GPIO_INT_CFG int_cfg;

	port = gpio_get_dev(BUTTON_SENSOR_ID);
	port->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)(mask));
	int_cfg.int_bit_mask = mask;
	int_cfg.int_bit_type = GPIO_INT_BITS_EDGE_TRIG(mask);
	int_cfg.int_bit_polarity = GPIO_INT_BITS_POL_FALL_EDGE(mask);
	int_cfg.int_bit_debounce = GPIO_INT_BITS_ENA_DEBOUNCE(mask);
	port->gpio_control(GPIO_CMD_SET_BIT_INT_CFG, (void *)(&int_cfg));
	bit_isr.int_bit_ofs = button_id;

	switch (button_id) {
		case 0:
			bit_isr.int_bit_handler = button0_isr;
			break;
		case 1:
			bit_isr.int_bit_handler = button1_isr;
			break;
		case 2:
			bit_isr.int_bit_handler = button2_isr;
			break;
		default:
			break;
	}

	port->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&bit_isr));

	if (enable) {
		port->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)mask);
	}
	else {
		port->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)mask);
	}

}

lwm2m_object_t * get_object_button(uint32_t num)
{
	lwm2m_object_t * obj;
	obj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

	if (NULL != obj) {
		int i;
		prv_instance_t * instance_ptr;

		memset(obj, 0, sizeof(lwm2m_object_t));

		obj->objID = OBJECT_BUTTON_ID;

		for (i=0 ; i < num ; i++) {
			instance_ptr = (prv_instance_t *)lwm2m_malloc(sizeof(prv_instance_t));
			if (NULL == instance_ptr) {
				lwm2m_free(obj);
				return NULL;
			}

			memset(instance_ptr, 0, sizeof(prv_instance_t));
			instance_ptr->shortID = LWM2M_EMSK_INSTANCE_ID+i;
			instance_ptr->state = 0;
			instance_ptr->counter = 0;
			obj->instanceList = LWM2M_LIST_ADD(obj->instanceList, instance_ptr);
			prv_button_int(i, 1);
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
	button_obj = obj;
	return obj;
}

static void button0_isr(void *ptr)
{
	prv_instance_t * instance_ptr;
	char *buff = "/3347/0/5500";
	instance_ptr = (prv_instance_t *)lwm2m_list_find(button_obj->instanceList, 0);
	instance_ptr -> state = 1;
	(instance_ptr -> counter)++;
	xQueueSendToFrontFromISR(queue_observe, (void *)(&buff), 0);
}

static void button1_isr(void *ptr)
{
	prv_instance_t * instance_ptr;
	char *buff = "/3347/1/5500";
	instance_ptr = (prv_instance_t *)lwm2m_list_find(button_obj->instanceList, 1);
	instance_ptr -> state = 1;
	(instance_ptr -> counter)++;
	xQueueSendToFrontFromISR(queue_observe, (void *)(&buff), 0);
}

static void button2_isr(void *ptr)
{
	prv_instance_t * instance_ptr;
	char *buff = "/3347/2/5500";
	instance_ptr = (prv_instance_t *)lwm2m_list_find(button_obj->instanceList, 2);
	instance_ptr -> state = 1;
	(instance_ptr -> counter)++;
	xQueueSendToFrontFromISR(queue_observe, (void *)(&buff), 0);
}
