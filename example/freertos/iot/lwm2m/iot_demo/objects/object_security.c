/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *
 *******************************************************************************/

/*
 *  Resources:
 *
 *          Name            | ID | Operations | Instances | Mandatory |  Type   |  Range  | Units |
 *  Server URI              |  0 |            |  Single   |    Yes    | String  |         |       |
 *  Bootstrap Server        |  1 |            |  Single   |    Yes    | Boolean |         |       |
 *  Security Mode           |  2 |            |  Single   |    Yes    | Integer |   0-3   |       |
 *  Public Key or ID        |  3 |            |  Single   |    Yes    | Opaque  |         |       |
 *  Server Public Key or ID |  4 |            |  Single   |    Yes    | Opaque  |         |       |
 *  Secret Key              |  5 |            |  Single   |    Yes    | Opaque  |         |       |
 *  SMS Security Mode       |  6 |            |  Single   |    Yes    | Integer |  0-255  |       |
 *  SMS Binding Key Param.  |  7 |            |  Single   |    Yes    | Opaque  |   6 B   |       |
 *  SMS Binding Secret Keys |  8 |            |  Single   |    Yes    | Opaque  | 32-48 B |       |
 *  Server SMS Number       |  9 |            |  Single   |    Yes    | Integer |         |       |
 *  Short Server ID         | 10 |            |  Single   |    No     | Integer | 1-65535 |       |
 *  Client Hold Off Time    | 11 |            |  Single   |    Yes    | Integer |         |   s   |
 *
 */

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
 * Here we implement a very basic LWM2M Security Object which only knows NoSec security mode.
 */

#include "liblwm2m.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "embARC_debug.h"

#define LWM2M_SECURITY_URI_ID                 0
#define LWM2M_SECURITY_BOOTSTRAP_ID           1
#define LWM2M_SECURITY_SECURITY_ID            2
#define LWM2M_SECURITY_PUBLIC_KEY_ID          3
#define LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID   4
#define LWM2M_SECURITY_SECRET_KEY_ID          5
#define LWM2M_SECURITY_SMS_SECURITY_ID        6
#define LWM2M_SECURITY_SMS_KEY_PARAM_ID       7
#define LWM2M_SECURITY_SMS_SECRET_KEY_ID      8
#define LWM2M_SECURITY_SMS_SERVER_NUMBER_ID   9
#define LWM2M_SECURITY_SHORT_SERVER_ID        10
#define LWM2M_SECURITY_HOLD_OFF_ID            11

typedef struct _security_instance_
{
	struct _security_instance_ * next;        // matches lwm2m_list_t::next
	uint16_t                     instanceId;  // matches lwm2m_list_t::id
	char *                       uri;
	bool                         isBootstrap;
	uint16_t                     shortID;
	uint32_t                     clientHoldOffTime;
} security_instance_t;

static uint8_t prv_get_value(lwm2m_tlv_t * tlvP, security_instance_t * instance_ptr)
{
	// There are no multiple instance ressources
	tlvP->type = LWM2M_TYPE_RESOURCE;

	switch (tlvP->id) {
		case LWM2M_SECURITY_URI_ID:
			tlvP->value = (uint8_t*)instance_ptr->uri;
			tlvP->length = strlen(instance_ptr->uri);
			tlvP->flags = LWM2M_TLV_FLAG_STATIC_DATA;
			tlvP->dataType = LWM2M_TYPE_STRING;
			return COAP_205_CONTENT;

		case LWM2M_SECURITY_BOOTSTRAP_ID:
			lwm2m_tlv_encode_bool(instance_ptr->isBootstrap, tlvP);
			if (0 != tlvP->length) return COAP_205_CONTENT;
			else return COAP_500_INTERNAL_SERVER_ERROR;

		case LWM2M_SECURITY_SECURITY_ID:
			lwm2m_tlv_encode_int(LWM2M_SECURITY_MODE_NONE, tlvP);
			if (0 != tlvP->length) return COAP_205_CONTENT;
			else return COAP_500_INTERNAL_SERVER_ERROR;

		case LWM2M_SECURITY_PUBLIC_KEY_ID:
			// Here we return an opaque of 1 byte containing 0
			tlvP->value = (uint8_t*)"";
			tlvP->length = 1;
			tlvP->flags = LWM2M_TLV_FLAG_STATIC_DATA;
			tlvP->dataType = LWM2M_TYPE_OPAQUE;
			return COAP_205_CONTENT;

		case LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID:
			// Here we return an opaque of 1 byte containing 0
			tlvP->value = (uint8_t*)"";
			tlvP->length = 1;
			tlvP->flags = LWM2M_TLV_FLAG_STATIC_DATA;
			tlvP->dataType = LWM2M_TYPE_OPAQUE;
			return COAP_205_CONTENT;

		case LWM2M_SECURITY_SECRET_KEY_ID:
			// Here we return an opaque of 1 byte containing 0
			tlvP->value = (uint8_t*)"";
			tlvP->length = 1;
			tlvP->flags = LWM2M_TLV_FLAG_STATIC_DATA;
			tlvP->dataType = LWM2M_TYPE_OPAQUE;
			return COAP_205_CONTENT;

		case LWM2M_SECURITY_SMS_SECURITY_ID:
			lwm2m_tlv_encode_int(LWM2M_SECURITY_MODE_NONE, tlvP);
			if (0 != tlvP->length) return COAP_205_CONTENT;
			else return COAP_500_INTERNAL_SERVER_ERROR;

		case LWM2M_SECURITY_SMS_KEY_PARAM_ID:
			// Here we return an opaque of 6 bytes containing a buggy value
			tlvP->value = (uint8_t*)"12345";
			tlvP->length = 6;
			tlvP->flags = LWM2M_TLV_FLAG_STATIC_DATA;
			tlvP->dataType = LWM2M_TYPE_OPAQUE;
			return COAP_205_CONTENT;

		case LWM2M_SECURITY_SMS_SECRET_KEY_ID:
			// Here we return an opaque of 32 bytes containing a buggy value
			tlvP->value = (uint8_t*)"1234567890abcdefghijklmnopqrstu";
			tlvP->length = 32;
			tlvP->flags = LWM2M_TLV_FLAG_STATIC_DATA;
			tlvP->dataType = LWM2M_TYPE_OPAQUE;
			return COAP_205_CONTENT;

		case LWM2M_SECURITY_SMS_SERVER_NUMBER_ID:
			lwm2m_tlv_encode_int(0, tlvP);
			if (0 != tlvP->length) return COAP_205_CONTENT;
			else return COAP_500_INTERNAL_SERVER_ERROR;

		case LWM2M_SECURITY_SHORT_SERVER_ID:
			lwm2m_tlv_encode_int(instance_ptr->shortID, tlvP);
			if (0 != tlvP->length) return COAP_205_CONTENT;
			else return COAP_500_INTERNAL_SERVER_ERROR;

		case LWM2M_SECURITY_HOLD_OFF_ID:
			lwm2m_tlv_encode_int(instance_ptr->clientHoldOffTime, tlvP);
			if (0 != tlvP->length) return COAP_205_CONTENT;
			else return COAP_500_INTERNAL_SERVER_ERROR;

		default:
			return COAP_404_NOT_FOUND;
	}
}

static uint8_t prv_security_read(uint16_t instanceId, int * numDataP, lwm2m_tlv_t ** dataArrayP,
				lwm2m_object_t * objectP)
{
	security_instance_t * instance_ptr;
	uint8_t result;
	int i;

	instance_ptr = (security_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
	if (NULL == instance_ptr) return COAP_404_NOT_FOUND;

	// is the server asking for the full instance ?
	if (*numDataP == 0)
	{
		uint16_t resList[] = {LWM2M_SECURITY_URI_ID,
							  LWM2M_SECURITY_BOOTSTRAP_ID,
							  LWM2M_SECURITY_SECURITY_ID,
							  LWM2M_SECURITY_PUBLIC_KEY_ID,
							  LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID,
							  LWM2M_SECURITY_SECRET_KEY_ID,
							  LWM2M_SECURITY_SMS_SECURITY_ID,
							  LWM2M_SECURITY_SMS_KEY_PARAM_ID,
							  LWM2M_SECURITY_SMS_SECRET_KEY_ID,
							  LWM2M_SECURITY_SMS_SERVER_NUMBER_ID,
							  LWM2M_SECURITY_SHORT_SERVER_ID,
							  LWM2M_SECURITY_HOLD_OFF_ID};
		int nbRes = sizeof(resList)/sizeof(uint16_t);

		*dataArrayP = lwm2m_tlv_new(nbRes);
		if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
		*numDataP = nbRes;
		for (i = 0 ; i < nbRes ; i++) {
			(*dataArrayP)[i].id = resList[i];
		}
	}

	i = 0;
	do {
		result = prv_get_value((*dataArrayP) + i, instance_ptr);
		i++;
	} while (i < *numDataP && result == COAP_205_CONTENT);

	return result;
}

#ifdef LWM2M_BOOTSTRAP

static uint8_t prv_security_write(uint16_t instanceId, int numData, lwm2m_tlv_t * dataArray,
				lwm2m_object_t * objectP)
{
	security_instance_t * instance_ptr;
	int i;
	uint8_t result = COAP_204_CHANGED;

	if ((dataArray->flags & LWM2M_TLV_FLAG_BOOTSTRAPPING) == 0) return COAP_401_UNAUTHORIZED;

	instance_ptr = (security_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
	if (NULL == instance_ptr) {
		return COAP_404_NOT_FOUND;
	}

	i = 0;
	do {
		switch (dataArray[i].id) {
			case LWM2M_SECURITY_URI_ID:
				if (instance_ptr->uri != NULL) lwm2m_free(instance_ptr->uri);
				instance_ptr->uri = (char *)lwm2m_malloc(dataArray[i].length + 1);
				memset(instance_ptr->uri, 0, dataArray[i].length + 1);
				if (instance_ptr->uri != NULL) {
					strncpy(instance_ptr->uri, (char*)dataArray[i].value, dataArray[i].length);
					result = COAP_204_CHANGED;
				} else {
					result = COAP_500_INTERNAL_SERVER_ERROR;
				}
				break;

			case LWM2M_SECURITY_BOOTSTRAP_ID:
				if (lwm2m_tlv_decode_bool(dataArray + i, &(instance_ptr->isBootstrap))) {
					result = COAP_204_CHANGED;
				} else {
					result = COAP_400_BAD_REQUEST;
				}
				break;

			case LWM2M_SECURITY_SECURITY_ID:
				// Let just ignore this
				result = COAP_204_CHANGED;
				break;

			case LWM2M_SECURITY_PUBLIC_KEY_ID:
				// Let just ignore this
				result = COAP_204_CHANGED;
				break;

			case LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID:
				// Let just ignore this
				result = COAP_204_CHANGED;
				break;

			case LWM2M_SECURITY_SECRET_KEY_ID:
				// Let just ignore this
				result = COAP_204_CHANGED;
				break;

			case LWM2M_SECURITY_SMS_SECURITY_ID:
				// Let just ignore this
				result = COAP_204_CHANGED;
				break;

			case LWM2M_SECURITY_SMS_KEY_PARAM_ID:
				// Let just ignore this
				result = COAP_204_CHANGED;
				break;

			case LWM2M_SECURITY_SMS_SECRET_KEY_ID:
				// Let just ignore this
				result = COAP_204_CHANGED;
				break;

			case LWM2M_SECURITY_SMS_SERVER_NUMBER_ID:
				// Let just ignore this
				result = COAP_204_CHANGED;
				break;

			case LWM2M_SECURITY_SHORT_SERVER_ID:
				int64_t value;

				if (lwm2m_tlv_decode_int(dataArray + i, &value)) {
					if (value >= 0 && value <= 0xFFFF) {
						instance_ptr->shortID = value;
						result = COAP_204_CHANGED;
					} else {
						result = COAP_406_NOT_ACCEPTABLE;
					}
				} else {
					result = COAP_400_BAD_REQUEST;
				}
				break;

			case LWM2M_SECURITY_HOLD_OFF_ID:
				int64_t value;

				if (lwm2m_tlv_decode_int(dataArray + i, &value)) {
					if (value >= 0 && value <= 0xFFFF) {
						instance_ptr->clientHoldOffTime = value;
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

static uint8_t prv_security_delete(uint16_t id, lwm2m_object_t * objectP)
{
	security_instance_t * instance_ptr;

	objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&instance_ptr);
	if (NULL == instance_ptr) return COAP_404_NOT_FOUND;
	if (NULL != instance_ptr->uri) {
		lwm2m_free(instance_ptr->uri);
	}

	lwm2m_free(instance_ptr);

	return COAP_202_DELETED;
}

static uint8_t prv_security_create(uint16_t instanceId, int numData, lwm2m_tlv_t * dataArray,
	lwm2m_object_t * objectP)
{
	security_instance_t * instance_ptr;
	uint8_t result;

	instance_ptr = (security_instance_t *)lwm2m_malloc(sizeof(security_instance_t));
	if (NULL == instance_ptr) return COAP_500_INTERNAL_SERVER_ERROR;
	memset(instance_ptr, 0, sizeof(security_instance_t));

	instance_ptr->instanceId = instanceId;
	objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, instance_ptr);

	result = prv_security_write(instanceId, numData, dataArray, objectP);

	if (result != COAP_204_CHANGED) {
		(void)prv_security_delete(instanceId, objectP);
	} else {
		result = COAP_201_CREATED;
	}

	return result;
}
#endif

static void prv_security_close(lwm2m_object_t * objectP)
{
	while (objectP->instanceList != NULL) {
		security_instance_t * securityInstance = (security_instance_t *)objectP->instanceList;
		objectP->instanceList = objectP->instanceList->next;
		if (NULL != securityInstance->uri) {
			lwm2m_free(securityInstance->uri);
		}
		lwm2m_free(securityInstance);
	}
}

void copy_security_object(lwm2m_object_t * objectDest, lwm2m_object_t * objectSrc)
{
	memcpy(objectDest, objectSrc, sizeof(lwm2m_object_t));
	objectDest->instanceList = NULL;
	objectDest->userData = NULL;
	security_instance_t * instanceSrc = (security_instance_t *)objectSrc->instanceList;
	security_instance_t * previousInstanceDest = NULL;
	while (instanceSrc != NULL) {
		security_instance_t * instanceDest = (security_instance_t *)lwm2m_malloc(sizeof(security_instance_t));
		if (NULL == instanceDest) {
			return;
		}
		memcpy(instanceDest, instanceSrc, sizeof(security_instance_t));
		instanceDest->uri = (char*)lwm2m_malloc(strlen(instanceSrc->uri) + 1);
		strcpy(instanceDest->uri, instanceSrc->uri);
		instanceSrc = (security_instance_t *)instanceSrc->next;
		if (previousInstanceDest == NULL) {
			objectDest->instanceList = (lwm2m_list_t *)instanceDest;
		} else {
			previousInstanceDest->next = instanceDest;
		}
		previousInstanceDest = instanceDest;
	}
}

void display_security_object(lwm2m_object_t * object)
{
#ifdef WITH_LOGS
	EMBARC_PRINTF("  /%u: Security object, instances:\r\n", object->objID);
	security_instance_t * instance = (security_instance_t *)object->instanceList;
	while (instance != NULL) {
		EMBARC_PRINTF("    /%u/%u: instanceId: %u, uri: %s, isBootstrap: %s, shortId: %u, clientHoldOffTime: %u\r\n",
				object->objID, instance->instanceId,
				instance->instanceId, instance->uri, instance->isBootstrap ? "true" : "false",
				instance->shortID, instance->clientHoldOffTime);
		instance = (security_instance_t *)instance->next;
	}
#endif
}

lwm2m_object_t * get_security_object(int serverId, const char* serverUri, bool isBootstrap)
{
	lwm2m_object_t * obj;

	obj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

	if (NULL != obj) {
		security_instance_t * instance_ptr;

		memset(obj, 0, sizeof(lwm2m_object_t));

		obj->objID = 0;

		// Manually create an hardcoded instance
		instance_ptr = (security_instance_t *)lwm2m_malloc(sizeof(security_instance_t));
		if (NULL == instance_ptr) {
			lwm2m_free(obj);
			return NULL;
		}

		memset(instance_ptr, 0, sizeof(security_instance_t));
		instance_ptr->instanceId = 0;
		instance_ptr->uri = (char*)lwm2m_malloc(strlen(serverUri)+1);
		strcpy(instance_ptr->uri, serverUri);
		instance_ptr->isBootstrap = isBootstrap;
		instance_ptr->shortID = serverId;
		instance_ptr->clientHoldOffTime = 10;

		obj->instanceList = LWM2M_LIST_ADD(obj->instanceList, instance_ptr);

		obj->readFunc = prv_security_read;
#ifdef LWM2M_BOOTSTRAP
		obj->writeFunc = prv_security_write;
		obj->createFunc = prv_security_create;
		obj->deleteFunc = prv_security_delete;
#endif
		obj->closeFunc = prv_security_close;
	}

	return obj;
}

char * get_server_uri(lwm2m_object_t * objectP, uint16_t secObjInstID)
{
	security_instance_t * instance_ptr = (security_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, secObjInstID);

	if (NULL != instance_ptr) {
		return lwm2m_strdup(instance_ptr->uri);
	}

	return NULL;
}
