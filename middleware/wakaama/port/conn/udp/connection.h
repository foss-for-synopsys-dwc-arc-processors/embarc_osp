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
 *
 *******************************************************************************/

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LWM2M_STANDARD_PORT_STR "5683"
#define LWM2M_STANDARD_PORT      5683

typedef struct _connection_t
{
    struct _connection_t *  next;
    int                     sock;
    struct sockaddr_in      addr;
    size_t                  addrLen;
} connection_t;

extern int create_socket(uint16_t bind_flag, uint16_t local_port);

extern connection_t * connection_find(connection_t * connList, struct sockaddr_storage * addr, size_t addrLen);
extern connection_t * connection_new_incoming(connection_t * connList, int sock, struct sockaddr * addr, size_t addrLen);
extern connection_t * connection_create(connection_t * connList, int sock, char * host, uint16_t port);

extern void connection_free(connection_t * connList);

extern int connection_send(connection_t *connP, uint8_t * buffer, size_t length);

extern void output_buffer(uint8_t * buffer, int length);

#ifdef __cplusplus
}
#endif

#endif
