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
 * \date 2016-08-05
 * \author Xinyi Zhao(xinyi@synopsys.com)
--------------------------------------------- */

/*
 * Implements an object for testing purpose
 *
 *                 Multiple
 * Object |  ID  | Instances | Mandatory |
 *  Test  | 3341 |    Yes    |    No     |
 *
 *  Resources:
 *              Supported    Multiple
 *  Name             |  ID  | Operations | Instances | Mandatory|  Type   | Range | Units | Description |
 *  Text             | 5527 |    R/W     |    No     |    Yes   | String  |       |       |             |
 *  X Coordinate     | 5528 |    R/W     |    No     |    No    | Integer |       |       |             |
 *  Y Coordinate     | 5529 |    R/W     |    No     |    No    | Integer |       |       |             |
 *  Clear Display    | 5530 |     E      |    No     |    No    |         |       |       |             |
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "embARC.h"
#include "embARC_debug.h"
#include "u8g.h"

#define OBJECT_LIGHT_ID                 3341

#define LWM2M_TEXT                      5527
#define LWM2M_XCOORDINATE               5528
#define LWM2M_YCOORDINATE               5529
#define LWM2M_CLEAR                     5530

#define LWM2M_EMSK_INSTANCE_ID  0
static u8g_t u8g;
static void display_prepare(void);
static void display_refresh(lwm2m_object_t * objectP);
/*
 * Multiple instance objects can use userdata to store data that will be shared between the different instances.
 * The lwm2m_object_t object structure - which represents every object of the liblwm2m as seen in the single instance
 * object - contain a chained list called instanceList with the object specific structure prv_instance_t:
 */
typedef struct _prv_instance_
{
	/*
	 * The first two are mandatory and represent the pointer to the next instance and the ID of this one. The rest
	 * is the instance scope user data (uint8_t test in this case)
	 */
	struct _prv_instance_ * next;   // matches lwm2m_list_t::next
	uint16_t shortID;			   // matches lwm2m_list_t::id
	char *text;
	uint32_t x_coordinate;
	uint32_t y_coordinate;
} prv_instance_t;

static uint8_t prv_read(uint16_t instanceId, int * numDataP, lwm2m_tlv_t ** dataArrayP,
		lwm2m_object_t * objectP)
{
	prv_instance_t * instance_ptr;
	int i;

	instance_ptr = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
	if (NULL == instance_ptr) return COAP_404_NOT_FOUND;

	if (*numDataP == 0)
	{
		*dataArrayP = lwm2m_tlv_new(3);
		if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
		*numDataP = 3;
		(*dataArrayP)[0].id = LWM2M_TEXT;
		(*dataArrayP)[1].id = LWM2M_XCOORDINATE;
		(*dataArrayP)[2].id = LWM2M_YCOORDINATE;
	}

	for (i = 0 ; i < *numDataP ; i++)
	{
		switch ((*dataArrayP)[i].id)
		{
			case LWM2M_TEXT:
				(*dataArrayP)[i].value  = instance_ptr->text;
				(*dataArrayP)[i].length = strlen(instance_ptr->text);
				(*dataArrayP)[i].flags  &= ~LWM2M_TLV_FLAG_STATIC_DATA;
				(*dataArrayP)[i].type   = LWM2M_TYPE_RESOURCE;
				(*dataArrayP)[i].dataType = LWM2M_TYPE_STRING;
				break;

			case LWM2M_XCOORDINATE:
				(*dataArrayP)[i].type = LWM2M_TYPE_RESOURCE;
				lwm2m_tlv_encode_int(instance_ptr->x_coordinate, *dataArrayP + i);
				break;

			case LWM2M_YCOORDINATE:
				(*dataArrayP)[i].type = LWM2M_TYPE_RESOURCE;
				lwm2m_tlv_encode_int(instance_ptr->y_coordinate, *dataArrayP + i);
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
	int64_t value;

	instance_ptr = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
	if (NULL == instance_ptr) return COAP_404_NOT_FOUND;

	i = 0;
	do {
		switch (dataArray[i].id) {
			case LWM2M_TEXT:
				memcpy(instance_ptr->text, dataArray[i].value, dataArray[i].length);
				instance_ptr->text[dataArray[i].length] = '\0';
				display_refresh(objectP);
				result = COAP_204_CHANGED;
				break;
			case LWM2M_XCOORDINATE:
				if (lwm2m_tlv_decode_int(dataArray + i, &value)) {
					if(value >= 0 && value <= 128) {
						instance_ptr->x_coordinate = value;
						result = COAP_204_CHANGED;
					} else {
						result = COAP_406_NOT_ACCEPTABLE;
					}
				} else {
					result = COAP_400_BAD_REQUEST;
				}
				break;
			case LWM2M_YCOORDINATE:
				if (lwm2m_tlv_decode_int(dataArray + i, &value)) {
					if(value >= 0 && value <= 64) {
						instance_ptr->y_coordinate = value;
						result = COAP_204_CHANGED;
					} else {
						result = COAP_406_NOT_ACCEPTABLE;
					}
				} else {
					result = COAP_400_BAD_REQUEST;
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

static uint8_t prv_exec(uint16_t instanceId, uint16_t resourceId, uint8_t * buffer, int length,
			lwm2m_object_t * objectP)
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

lwm2m_object_t * get_object_display(uint32_t num)
{
	lwm2m_object_t * obj;
	uint16_t max_x, max_y;
	obj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

	if (NULL != obj) {
		int i;
		prv_instance_t * instance_ptr;
		u8g_InitComFn(&u8g, &u8g_dev_ssd1306_128x64_2x_i2c, U8G_COM_SSD_I2C);
		u8g_Begin(&u8g);
		max_x = u8g_GetWidth(&u8g);
		max_y = u8g_GetHeight(&u8g);
		memset(obj, 0, sizeof(lwm2m_object_t));
		obj->objID = OBJECT_LIGHT_ID;
		for (i = 0 ;i < num ;i++) {
			instance_ptr = (prv_instance_t *)lwm2m_malloc(sizeof(prv_instance_t));
			if (NULL == instance_ptr) {
				lwm2m_free(obj);
				return NULL;
			}
			memset(instance_ptr, 0, sizeof(prv_instance_t));
			instance_ptr->shortID = LWM2M_EMSK_INSTANCE_ID+i;
			instance_ptr->text = (uint8_t *)lwm2m_malloc(21 * sizeof(uint8_t));
			memcpy(instance_ptr->text, "", 1);
			if(i < 3)
				instance_ptr->x_coordinate = 90;
			else
				instance_ptr->x_coordinate = 0;
			instance_ptr->y_coordinate = 14 + 10 * i;
			obj->instanceList = LWM2M_LIST_ADD(obj->instanceList, instance_ptr);
		}
		/*
		 * From a single instance object, two more functions are available.
		 * - The first one (createFunc) create a new instance and filled it with the provided informations. If an ID is
		 *   provided a check is done for verifying his availability, or a new one is generated.
		 * - The other one (deleteFunc) delete an instance by removing it from the instance list (and freeing the memory
		 *   allocated to it)
		 */
		obj->readFunc	= prv_read;
		obj->writeFunc   = prv_write;
		obj->createFunc  = prv_create;
		obj->deleteFunc  = prv_delete;
		obj->executeFunc = prv_exec;
		obj->closeFunc   = prv_close;
		display_refresh(obj);
	}

	return obj;
}

static void display_prepare(void)
{
	u8g_SetFont(&u8g, u8g_font_6x10);
	u8g_SetFontRefHeightExtendedText(&u8g);
	u8g_SetDefaultForegroundColor(&u8g);
	u8g_SetFontPosTop(&u8g);
}

static void display_refresh(lwm2m_object_t * objectP)
{
	prv_instance_t * instance_ptr;
	uint8_t instanceId;
	u8g_FirstPage(&u8g);
	do {
		display_prepare();
		instanceId = 0;
		u8g_DrawStr(&u8g, 13, 0, "embARC SMART HOME");
		u8g_DrawStr(&u8g, 2, 14, "KitchenLight:");
		u8g_DrawStr(&u8g, 2, 24, "RoomLight:");
		u8g_DrawStr(&u8g, 2, 34, "Temperature:        C");
		while((instance_ptr = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId)) != NULL){
			if(!strcmp(instance_ptr->text, "true")){
				u8g_DrawStr(&u8g, instance_ptr->x_coordinate, instance_ptr->y_coordinate, "on");
			} else if(!strcmp(instance_ptr->text, "false")){
				u8g_DrawStr(&u8g, instance_ptr->x_coordinate, instance_ptr->y_coordinate, "off");
			} else {
				u8g_DrawStr(&u8g, instance_ptr->x_coordinate, instance_ptr->y_coordinate, instance_ptr->text);
			}
			instanceId++;
		}

	} while (u8g_NextPage(&u8g));
}
