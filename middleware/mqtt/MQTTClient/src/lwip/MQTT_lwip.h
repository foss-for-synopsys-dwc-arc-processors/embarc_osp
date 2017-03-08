/*******************************************************************************
 * Copyright (c) 2009, 2014 IBM Corp.
 * Copyright (c) 2015 Synopsys, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial implementation and documentation
 *    Ian Craggs - async client updates
 *    Wayne Ren - Modified for port to ARC processor  -  wei.ren@synopsys.com
 *******************************************************************************/

/**
 * \file
 * \brief header file of target dependent part of Paho MQTT library
 */

#ifndef __MQTT_FREERTOS_LWIP_H_
#define __MQTT_FREERTOS_LWIP_H_

#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Timer Timer;

struct Timer {
	unsigned long end_time;
};

typedef struct Network Network;

struct Network
{
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
};

extern char expired(Timer*);
extern void countdown_ms(Timer*, unsigned int);
extern void countdown(Timer*, unsigned int);
extern int left_ms(Timer*);

extern void InitTimer(Timer*);
extern void NewNetwork(Network*);
extern int ConnectNetwork(Network*, char*, int);

#ifdef __cplusplus
}
#endif

#endif