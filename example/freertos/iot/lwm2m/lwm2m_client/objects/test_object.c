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
 *    domedambrosio - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Axel Lorente - Please refer to git log
 *    Achim Kraus, Bosch Software Innovations GmbH - Please refer to git log
 *
 *******************************************************************************/

/*
 Copyright (c) 2013, 2014 Intel Corporation

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
     * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>

*/

/*
 * Implements an object for testing purpose
 *
 *                 Multiple
 * Object |  ID  | Instances | Mandatoty |
 *  Test  | 1024 |    Yes    |    No     |
 *
 *  Ressources:
 *              Supported    Multiple
 *  Name | ID | Operations | Instances | Mandatory |  Type   | Range | Units | Description |
 *  test |  1 |    R/W     |    No     |    Yes    | Integer | 0-255 |       |             |
 *  exec |  2 |     E      |    No     |    Yes    |         |       |       |             |
 *
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "embARC.h"
#include "embARC_debug.h"

#define PRV_RESOURCE_3_SIZE 190
#define PRV_TLV_BUFFER_SIZE 64

#define LWM2M_BTN_ID	    0
#define LWM2M_SWT_ID	    1
#define LWM2M_LED_ID	    2
#define LWM2M_FOLAT_ID		3

#define LWM2M_INSTANCE_ID  0

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
    uint16_t btn;
    uint16_t swt;
    uint16_t led;
    double   dec;
} prv_instance_t;

static void prv_output_buffer(uint8_t * buffer,
                              int length)
{
    int i;
    uint8_t array[16];

    i = 0;
    while (i < length)
    {
        int j;
        EMBARC_PRINTF("  ");

        memcpy(array, buffer+i, 16);

        for (j = 0 ; j < 16 && i+j < length; j++)
        {
            EMBARC_PRINTF("%02X ", array[j]);
        }
        while (j < 16)
        {
            EMBARC_PRINTF("   ");
            j++;
        }
        EMBARC_PRINTF("  ");
        for (j = 0 ; j < 16 && i+j < length; j++)
        {
            if (isprint(array[j]))
                EMBARC_PRINTF("%c ", array[j]);
            else
                EMBARC_PRINTF(". ");
        }
        EMBARC_PRINTF("\n");

        i += 16;
    }
}

static uint8_t prv_get_value(lwm2m_tlv_t * tlvP,
                             prv_instance_t * targetP)
{
    // There are no multiple instance resources
    tlvP->type = LWM2M_TYPE_RESOURCE;

    switch (tlvP->id)
    {
    case LWM2M_BTN_ID:
        targetP->btn = button_read(BOARD_BTN_MASK);
        lwm2m_tlv_encode_int(targetP->btn, tlvP);
        if (0 != tlvP->length) return COAP_205_CONTENT;
        else return COAP_500_INTERNAL_SERVER_ERROR;

    case LWM2M_SWT_ID:
        targetP->swt = switch_read(BOARD_SWT_MASK);
        lwm2m_tlv_encode_int(targetP->swt, tlvP);
        if (0 != tlvP->length) return COAP_205_CONTENT;
        else return COAP_500_INTERNAL_SERVER_ERROR;

    case LWM2M_LED_ID:
        targetP->led = led_read(BOARD_LED_MASK);
        lwm2m_tlv_encode_int(targetP->led, tlvP);
        if (0 != tlvP->length) return COAP_205_CONTENT;
        else return COAP_500_INTERNAL_SERVER_ERROR;

    case LWM2M_FOLAT_ID:
        lwm2m_tlv_encode_float(targetP->dec, tlvP);
        if (0 != tlvP->length) return COAP_205_CONTENT;
        else return COAP_500_INTERNAL_SERVER_ERROR;
        break;

    default:
        return COAP_404_NOT_FOUND;
    }
}

static uint8_t prv_read(uint16_t instanceId,
                        int * numDataP,
                        lwm2m_tlv_t ** dataArrayP,
                        lwm2m_object_t * objectP)
{
    prv_instance_t * targetP;
    uint8_t result;
    int i;

    targetP = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // if (instanceId == 0) {
        if (*numDataP == 0)
        {
            uint16_t resList[] = {
                    LWM2M_BTN_ID,
                    LWM2M_SWT_ID,
                    LWM2M_LED_ID,
                    LWM2M_FOLAT_ID
            };
            int nbRes = sizeof(resList)/sizeof(uint16_t);

            *dataArrayP = lwm2m_tlv_new(nbRes);
            if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
            *numDataP = nbRes;
            for (i = 0 ; i < nbRes ; i++)
            {
                (*dataArrayP)[i].id = resList[i];
            }

        }
        i = 0;
        do
        {
            result = prv_get_value((*dataArrayP) + i, targetP);
            i++;
        } while (i < *numDataP && result == COAP_205_CONTENT);

        return result;
    // } else {
    //     return COAP_404_NOT_FOUND;
    // }
}

static uint8_t prv_write(uint16_t instanceId,
                         int numData,
                         lwm2m_tlv_t * dataArray,
                         lwm2m_object_t * objectP)
{
    prv_instance_t * targetP;
    int i;
    uint8_t result;

    targetP = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    i = 0;
    do
    {
        switch (dataArray[i].id)
        {
        case LWM2M_BTN_ID:
        case LWM2M_SWT_ID:
            result = COAP_405_METHOD_NOT_ALLOWED;
            break;

        case LWM2M_LED_ID:
        {
            int64_t value;

            if (1 == lwm2m_tlv_decode_int(dataArray + i, &value))
            {
                if (value >= 0 && value <= BOARD_LED_MASK)
                {
                    targetP->led = value;
                    led_write(targetP->led, BOARD_LED_MASK);
                    result = COAP_204_CHANGED;
                }
                else
                {
                    result = COAP_406_NOT_ACCEPTABLE;
                }
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
        }
        break;
        case LWM2M_FOLAT_ID:
            if (1 != lwm2m_tlv_decode_float(dataArray + i, &(targetP->dec))) {
                result = COAP_400_BAD_REQUEST;
            } else {
                result = COAP_204_CHANGED;
            }
            break;

        default:
            return COAP_404_NOT_FOUND;
        }
        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}

static uint8_t prv_delete(uint16_t id,
                          lwm2m_object_t * objectP)
{
    prv_instance_t * targetP;

    objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&targetP);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    lwm2m_free(targetP);

    return COAP_202_DELETED;
}

static uint8_t prv_create(uint16_t instanceId,
                          int numData,
                          lwm2m_tlv_t * dataArray,
                          lwm2m_object_t * objectP)
{
    prv_instance_t * targetP;
    uint8_t result;


    targetP = (prv_instance_t *)lwm2m_malloc(sizeof(prv_instance_t));
    if (NULL == targetP) return COAP_500_INTERNAL_SERVER_ERROR;
    memset(targetP, 0, sizeof(prv_instance_t));

    targetP->shortID = instanceId;
    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, targetP);

    result = prv_write(instanceId, numData, dataArray, objectP);

    if (result == COAP_404_NOT_FOUND)
    {
        (void)prv_delete(instanceId, objectP);
    }
    else
    {
        result = COAP_201_CREATED;
    }

    return result;
}

static uint8_t prv_exec(uint16_t instanceId,
                        uint16_t resourceId,
                        uint8_t * buffer,
                        int length,
                        lwm2m_object_t * objectP)
{

    if (NULL == lwm2m_list_find(objectP->instanceList, instanceId)) return COAP_404_NOT_FOUND;

    switch (resourceId)
    {
    case LWM2M_SWT_ID:
    case LWM2M_BTN_ID:
        return COAP_405_METHOD_NOT_ALLOWED;
    case LWM2M_LED_ID:
        EMBARC_PRINTF("\r\n-----------------\r\n"
                        "Execute on %hu/%d/%d\r\n"
                        " Parameter (%d bytes):\r\n",
                        objectP->objID, instanceId, resourceId, length);
        prv_output_buffer((uint8_t*)buffer, length);
        EMBARC_PRINTF("-----------------\r\n\r\n");
        return COAP_204_CHANGED;
    case LWM2M_FOLAT_ID:
        return COAP_405_METHOD_NOT_ALLOWED;
    default:
        return COAP_404_NOT_FOUND;
    }
}

static void prv_close(lwm2m_object_t * objectP)
{
    LWM2M_LIST_FREE(objectP->instanceList);
    if (objectP->userData != NULL)
    {
        lwm2m_free(objectP->userData);
        objectP->userData = NULL;
    }
}

void display_test_object(lwm2m_object_t * object)
{
#ifdef WITH_LOGS
    EMBARC_PRINTF("  /%u: Test object, instances:\r\n", object->objID);
    prv_instance_t * instance = (prv_instance_t *)object->instanceList;
    while (instance != NULL)
    {
        EMBARC_PRINTF("    /%u/%u: shortId: %u, btn: %u\r\n",
                object->objID, instance->shortID,
                instance->shortID, instance->btn);
        instance = (prv_instance_t *)instance->next;
    }
#endif
}

lwm2m_object_t * get_test_object(void)
{
    lwm2m_object_t * testObj;

    testObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != testObj)
    {
        int i;
        prv_instance_t * targetP;

        memset(testObj, 0, sizeof(lwm2m_object_t));

        testObj->objID = 1024;
        for (i=0 ; i < 1 ; i++)
        {
            targetP = (prv_instance_t *)lwm2m_malloc(sizeof(prv_instance_t));
            if (NULL == targetP) {
                lwm2m_free(testObj);
                return NULL;
            }
            memset(targetP, 0, sizeof(prv_instance_t));
            targetP->shortID = LWM2M_INSTANCE_ID+i;
            targetP->swt = switch_read(BOARD_SWT_MASK);
            targetP->led = led_read(BOARD_LED_MASK);
            targetP->btn = button_read(BOARD_LED_MASK);
            targetP->dec = 1.32;
            testObj->instanceList = LWM2M_LIST_ADD(testObj->instanceList, targetP);
        }
        /*
         * From a single instance object, two more functions are available.
         * - The first one (createFunc) create a new instance and filled it with the provided informations. If an ID is
         *   provided a check is done for verifying his disponibility, or a new one is generated.
         * - The other one (deleteFunc) delete an instance by removing it from the instance list (and freeing the memory
         *   allocated to it)
         */
        testObj->readFunc    = prv_read;
        testObj->writeFunc   = prv_write;
        testObj->createFunc  = prv_create;
        testObj->deleteFunc  = prv_delete;
        testObj->executeFunc = prv_exec;
        testObj->closeFunc   = prv_close;
    }

    return testObj;
}
