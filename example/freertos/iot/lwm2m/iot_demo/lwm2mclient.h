/*******************************************************************************
 *
 * Copyright (c) 2014 Bosch Software Innovations GmbH, Germany.
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
 *    Bosch Software Innovations GmbH - Please refer to git log
 *
 *******************************************************************************/
/*
 * lwm2mclient.h
 *
 *  General functions of lwm2m test client.
 *
 *  Created on: 22.01.2015
 *  Author: Achim Kraus
 *  Copyright (c) 2015 Bosch Software Innovations GmbH, Germany. All rights reserved.
 */

#ifndef LWM2MCLIENT_H_
#define LWM2MCLIENT_H_

#include "liblwm2m.h"

extern int g_reboot;
typedef void (*CHANGE_HANDLER) (void* context, const char* uriPath, const char * value, size_t valueLength);

typedef struct _obj_info_ {
	uint32_t oid;
	uint32_t number;
	struct _obj_info_ *next;
} obj_info;

typedef struct _lwm2m_client_info_ {
	const char * server;
	const char * serverPort;
	const char * ep_name;
	const char * model;
	obj_info *obj;
	int lifetime;
} lwm2m_client_info;


/*
 * main.c
 */

extern void system_setValueChangedHandler(lwm2m_context_t *lwm2m, CHANGE_HANDLER prv_value_change);

/*
 * lwm2mclient.c
 */
int lwm2mclient(lwm2m_client_info *client_info);

/*
 * object_device.c
 */
extern lwm2m_object_t * get_object_device();
uint8_t device_change(lwm2m_tlv_t * dataArray, lwm2m_object_t * objectP);
/*
 * object_firmware.c
 */
extern lwm2m_object_t * get_object_firmware();
/*
 * object_light.c
 */
extern lwm2m_object_t * get_object_light(uint32_t num);
/*
 * object_button.c
 */
extern lwm2m_object_t * get_object_button(uint32_t num);
/*
 * object_tempterature.c
 */
extern lwm2m_object_t * get_object_temperature(uint32_t num);
/*
 * object_display.c
 */
extern lwm2m_object_t * get_object_display(uint32_t num);
/*
 * object_server.c
 */
extern lwm2m_object_t * get_server_object(int serverId, const char* binding, int lifetime, bool storing);

/*
 * system_api.c
 */
extern void init_value_change(lwm2m_context_t * lwm2m);
extern void system_reboot();

/*
 * object_security.c
 */
extern lwm2m_object_t * get_security_object();
extern char * get_server_uri(lwm2m_object_t * objectP, uint16_t serverID);

#endif /* LWM2MCLIENT_H_ */
