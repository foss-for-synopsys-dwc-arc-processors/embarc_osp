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
 * lwm2mserver.h
 *
 *  General functions of lwm2m test server.
 *
 */

#ifndef LWM2MSERVER_H_
#define LWM2MSERVER_H_

#include "liblwm2m.h"

extern int g_reboot;

typedef struct _lwm2m_server_info_ {
	int serverPort;
} lwm2m_server_info;

typedef struct _lwm2m_op_info_ {
	int clientId;
	char *r_uri;
	char *r_data;
} lwm2m_op_info;

/*
 * lwm2mserver.c
 */
extern int lwm2mserver(lwm2m_server_info *server_info);
extern void handle_sigint(int signum);

typedef void (* lwm2m_handler)(lwm2m_op_info *op_info);
extern void lwm2m_server_output_clients(lwm2m_op_info *op_info);
extern void lwm2m_server_read_client(lwm2m_op_info *op_info);
extern void lwm2m_server_write_client(lwm2m_op_info *op_info);
extern void lwm2m_server_exec_client(lwm2m_op_info *op_info);
extern void lwm2m_server_create_client(lwm2m_op_info *op_info);
extern void lwm2m_server_delete_client(lwm2m_op_info *op_info);
extern void lwm2m_server_observe_client(lwm2m_op_info *op_info);
extern void lwm2m_server_cancel_client(lwm2m_op_info *op_info);

/** operation methods */
#define LWM2M_OP_METHOD_LIST		"list"
#define LWM2M_OP_METHOD_LIST_ID		0
#define LWM2M_OP_METHOD_READ		"read"
#define LWM2M_OP_METHOD_READ_ID		1
#define LWM2M_OP_METHOD_WRITE		"write"
#define LWM2M_OP_METHOD_WRITE_ID	2
#define LWM2M_OP_METHOD_EXEC		"exec"
#define LWM2M_OP_METHOD_EXEC_ID		3
#define LWM2M_OP_METHOD_CREATE		"create"
#define LWM2M_OP_METHOD_CREATE_ID	4
#define LWM2M_OP_METHOD_DELETE		"delete"
#define LWM2M_OP_METHOD_DELETE_ID	5
#define LWM2M_OP_METHOD_OBSERVE		"observe"
#define LWM2M_OP_METHOD_OBSERVE_ID	6
#define LWM2M_OP_METHOD_CANCEL		"cancel"
#define LWM2M_OP_METHOD_CANCEL_ID	7

#endif /* LWM2MSERVER_H_ */
