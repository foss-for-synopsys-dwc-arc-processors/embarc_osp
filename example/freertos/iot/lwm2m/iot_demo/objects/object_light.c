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
 *  Test  | 3311 |    Yes    |    No     |
 *
 *  Ressources:
 *              Supported    Multiple
 *  Name   |  ID  | Operations | Instances | Mandatory |  Type   | Range | Units | Description |
 *  On/Off | 5850 |    R/W     |    No     |    Yes    | Boolean |       |       |             |
 *  Dimer  | 5851 |    R/W     |    No     |    No     |         | 0-100 |   %   |             |
 *
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "embARC.h"
#include "embARC_debug.h"

#define OBJECT_LIGHT_ID		3311

#define LWM2M_ONOFF		5850
#define LWM2M_DIMER		5851


#define LWM2M_EMSK_INSTANCE_ID  0

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
	bool onoff;
	uint8_t dimer;
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
		(*dataArrayP)[0].id = LWM2M_ONOFF;
		(*dataArrayP)[1].id = LWM2M_DIMER;
	}

	for (i = 0 ; i < *numDataP ; i++) {
		switch ((*dataArrayP)[i].id) {
			case LWM2M_ONOFF:
				(*dataArrayP)[i].type = LWM2M_TYPE_RESOURCE;
				instance_ptr->onoff = 1 & ((led_read(1 << instanceId)) >> instanceId);
				lwm2m_tlv_encode_bool(instance_ptr->onoff, *dataArrayP + i);
				break;

			case LWM2M_DIMER:
				(*dataArrayP)[i].type = LWM2M_TYPE_RESOURCE;
				lwm2m_tlv_encode_int(instance_ptr->dimer, *dataArrayP + i);
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
	prv_instance_t * instance_ptr;
	int i;
	uint8_t result;

	instance_ptr = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
	if (NULL == instance_ptr) return COAP_404_NOT_FOUND;

	i = 0;
	do {
		switch (dataArray[i].id) {
			case LWM2M_ONOFF:
				{
				bool value;

				if (lwm2m_tlv_decode_bool(dataArray + i, &value)) {
					instance_ptr->onoff = value;
					led_write((instance_ptr->onoff) << instanceId, 1 << instanceId);
					result = COAP_204_CHANGED;
				}
				else {
					result = COAP_400_BAD_REQUEST;
				}
				}
				break;

			case LWM2M_DIMER:
				{
				int64_t value;

				if (lwm2m_tlv_decode_int(dataArray + i, &value)) {
					if(value >= 0 && value <= 100) {
						instance_ptr->dimer = value;
						result = COAP_204_CHANGED;
					} else {
						result = COAP_406_NOT_ACCEPTABLE;
					}
				} else {
					result = COAP_400_BAD_REQUEST;
				}
				}
				break;
			default:
				return COAP_404_NOT_FOUND;
		}
		i++;
	} while (i < numData && result == COAP_204_CHANGED);

	return result;
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

lwm2m_object_t * get_object_light(uint32_t num)
{
	lwm2m_object_t * obj;

	obj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

	if (NULL != obj) {
		int i;
		prv_instance_t * instance_ptr;

		memset(obj, 0, sizeof(lwm2m_object_t));

		obj->objID = OBJECT_LIGHT_ID;
		for (i=0 ; i < num ; i++) {
			instance_ptr = (prv_instance_t *)lwm2m_malloc(sizeof(prv_instance_t));
			if (NULL == instance_ptr) {
				lwm2m_free(obj);
				return NULL;
			}
			memset(instance_ptr, 0, sizeof(prv_instance_t));
			instance_ptr->shortID = LWM2M_EMSK_INSTANCE_ID+i;
			instance_ptr->onoff = 0;
			instance_ptr->dimer = 100;
			obj->instanceList = LWM2M_LIST_ADD(obj->instanceList, instance_ptr);
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

	return obj;
}
