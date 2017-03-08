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
 *    Simon Bernard - Please refer to git log
 *    Toby Jaffey - Please refer to git log
 *    Julien Vermillard - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
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

/**
 * \file
 * \ingroup	EMBARC_APP_LWIP_FREERTOS_LWM2M_SERVER
 * \brief	Implementation for LwM2M server
 */

#include "liblwm2m.h"
#include "lwm2mserver.h"
#include "connection.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>
#include <errno.h>

#include "embARC_debug.h"

#define MAX_PACKET_SIZE 1024

static int g_quit = 0;
static lwm2m_context_t * lwm2mH = NULL;


static uint8_t prv_buffer_send(void * sessionH,
                               uint8_t * buffer,
                               size_t length,
                               void * userdata)
{
    connection_t * connP = (connection_t*) sessionH;

    if (-1 == connection_send(connP, buffer, length))
    {
        return COAP_500_INTERNAL_SERVER_ERROR;
    }
    return COAP_NO_ERROR;
}

static char * prv_dump_binding(lwm2m_binding_t binding)
{
    switch (binding)
    {
    case BINDING_UNKNOWN:
        return "Not specified";
    case BINDING_U:
        return "UDP";
    case BINDING_UQ:
        return "UDP queue mode";
    case BINDING_S:
        return "SMS";
    case BINDING_SQ:
        return "SMS queue mode";
    case BINDING_US:
        return "UDP plus SMS";
    case BINDING_UQS:
        return "UDP queue mode plus SMS";
    default:
        return "";
    }
}

#define CODE_TO_STRING(X)   case X : return "(" #X ") "

const char* prv_status_to_string(int status)
{
    switch(status)
	{
    CODE_TO_STRING(COAP_NO_ERROR);
    CODE_TO_STRING(COAP_201_CREATED);
    CODE_TO_STRING(COAP_202_DELETED);
    CODE_TO_STRING(COAP_204_CHANGED);
    CODE_TO_STRING(COAP_205_CONTENT);
    CODE_TO_STRING(COAP_400_BAD_REQUEST);
    CODE_TO_STRING(COAP_401_UNAUTHORIZED);
    CODE_TO_STRING(COAP_404_NOT_FOUND);
    CODE_TO_STRING(COAP_405_METHOD_NOT_ALLOWED);
    CODE_TO_STRING(COAP_406_NOT_ACCEPTABLE);
    CODE_TO_STRING(COAP_500_INTERNAL_SERVER_ERROR);
    CODE_TO_STRING(COAP_501_NOT_IMPLEMENTED);
    CODE_TO_STRING(COAP_503_SERVICE_UNAVAILABLE);
    default: return "";
    }
}

static void prv_dump_client(lwm2m_client_t * targetP)
{
    lwm2m_client_object_t * objectP;

    if (targetP == NULL) return;

    EMBARC_PRINTF("Client #%d:\r\n", targetP->internalID);
    EMBARC_PRINTF("\tname: \"%s\"\r\n", targetP->name);
    EMBARC_PRINTF("\tbinding: \"%s\"\r\n", prv_dump_binding(targetP->binding));
    if (targetP->msisdn) EMBARC_PRINTF("\tmsisdn: \"%s\"\r\n", targetP->msisdn);
    EMBARC_PRINTF("\tlifetime: %d sec\r\n", targetP->lifetime);
    EMBARC_PRINTF("\tobjects: ");
    for (objectP = targetP->objectList; objectP != NULL ; objectP = objectP->next)
    {
        if (objectP->instanceList == NULL)
        {
            EMBARC_PRINTF("/%d, ", objectP->id);
        }
        else
        {
            lwm2m_list_t * instanceP;

            for (instanceP = objectP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
            {
                EMBARC_PRINTF("/%d/%d, ", objectP->id, instanceP->id);
            }
        }
    }
    EMBARC_PRINTF("\r\n");
}

void lwm2m_server_output_clients(lwm2m_op_info *op_info)
{
    lwm2m_client_t * targetP;

    targetP = lwm2mH->clientList;

    if (targetP == NULL)
    {
        EMBARC_PRINTF("No client.\r\n");
        return;
    }

    for (targetP = lwm2mH->clientList ; targetP != NULL ; targetP = targetP->next)
    {
        prv_dump_client(targetP);
    }
}

static void print_indent(int num)
{
    int i;

    for ( i = 0 ; i< num ; i++)
        EMBARC_PRINTF(" ");
}

static void output_tlv(uint8_t * buffer,
                       size_t buffer_len,
                       int indent)
{
    lwm2m_tlv_type_t type;
    uint16_t id;
    size_t dataIndex;
    size_t dataLen;
    int length = 0;
    int result;

    while (0 != (result = lwm2m_decodeTLV(buffer + length, buffer_len - length, &type, &id, &dataIndex, &dataLen)))
    {
        print_indent(indent);
        EMBARC_PRINTF("ID: %d", id);
        EMBARC_PRINTF("  type: ");
        switch (type)
        {
        case LWM2M_TYPE_OBJECT_INSTANCE:
            EMBARC_PRINTF("Object Instance");
            break;
        case LWM2M_TYPE_RESOURCE_INSTANCE:
            EMBARC_PRINTF("Resource Instance");
            break;
        case LWM2M_TYPE_MULTIPLE_RESOURCE:
            EMBARC_PRINTF("Multiple Instances");
            break;
        case LWM2M_TYPE_RESOURCE:
            EMBARC_PRINTF("Resource");
            break;
        default:
            printf("Unknown (%d)", (int)type);
            break;
        }
        EMBARC_PRINTF("\n");
        print_indent(indent);
        EMBARC_PRINTF("{\n");
        if (type == LWM2M_TYPE_OBJECT_INSTANCE || type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
            output_tlv(buffer + length + dataIndex, dataLen, indent+2);
        }
        else
        {
            print_indent(indent+2);
            EMBARC_PRINTF("data (%ld bytes):  ", dataLen);
            if (dataLen >= 16) EMBARC_PRINTF("\n");
            output_buffer((uint8_t*)buffer + length + dataIndex, dataLen);
        }
        print_indent(indent);
        EMBARC_PRINTF("}\n");
        length += result;
    }
}

static int prv_read_id(char * buffer,
                       uint16_t * idP)
{
    int nb;
    int value;

    nb = sscanf(buffer, "%d", &value);
    if (nb == 1)
    {
        if (value < 0 || value > LWM2M_MAX_ID)
        {
            nb = 0;
        }
        else
        {
            *idP = value;
        }
    }

    return nb;
}

static void prv_print_result(int status)
{
    EMBARC_PRINTF("  returned status %d.%02d %s\r\n", (status&0xE0)>>5, status&0x1F, prv_status_to_string(status));
}

static void prv_print_error(int status)
{
    EMBARC_PRINTF("Error %d.%02d %s!\r\n", (status&0xE0)>>5, status&0x1F, prv_status_to_string(status));
}

static void prv_result_callback(uint16_t clientID,
                                lwm2m_uri_t * uriP,
                                int status,
                                uint8_t * data,
                                int dataLength,
                                void * userData)
{
    EMBARC_PRINTF("\r\nClient #%d %d", clientID, uriP->objectId);
    if (LWM2M_URI_IS_SET_INSTANCE(uriP))
        EMBARC_PRINTF("/%d", uriP->instanceId);
    else if (LWM2M_URI_IS_SET_RESOURCE(uriP))
        EMBARC_PRINTF("/");
    if (LWM2M_URI_IS_SET_RESOURCE(uriP))
            EMBARC_PRINTF("/%d", uriP->resourceId);
    prv_print_result(status);

    if (data != NULL)
    {
        EMBARC_PRINTF("%d bytes received:\r\n", dataLength);
        if (LWM2M_URI_IS_SET_RESOURCE(uriP))
        {
            output_buffer(data, dataLength);
        }
        else
        {
            output_tlv(data, dataLength, 2);
        }
    }

    EMBARC_PRINTF("\r\n");
}

static void prv_notify_callback(uint16_t clientID,
                                lwm2m_uri_t * uriP,
                                int count,
                                uint8_t * data,
                                int dataLength,
                                void * userData)
{
    EMBARC_PRINTF("\r\nNotify from client #%d %d", clientID, uriP->objectId);
    if (LWM2M_URI_IS_SET_INSTANCE(uriP))
        EMBARC_PRINTF("/%d", uriP->instanceId);
    else if (LWM2M_URI_IS_SET_RESOURCE(uriP))
        EMBARC_PRINTF("/");
    if (LWM2M_URI_IS_SET_RESOURCE(uriP))
            EMBARC_PRINTF("/%d", uriP->resourceId);
    EMBARC_PRINTF(" number %d\r\n", count);

    if (data != NULL)
    {
        EMBARC_PRINTF("%d bytes received:\r\n", dataLength);
        output_buffer(data, dataLength);
    }

    EMBARC_PRINTF("\r\n");
}

void lwm2m_server_read_client(lwm2m_op_info *op_info)
{
    lwm2m_uri_t uri;
    int result;

    result = lwm2m_stringToUri(op_info->r_uri, strlen(op_info->r_uri), &uri);
    if (result == 0) goto syntax_error;

    result = lwm2m_dm_read(lwm2mH, op_info->clientId, &uri, prv_result_callback, NULL);

    if (result == 0)
    {
        EMBARC_PRINTF("OK\r\n");
    }
    else
    {
        prv_print_error(result);
    }
    return;

syntax_error:
    EMBARC_PRINTF("Syntax error !\r\n");
}

void lwm2m_server_write_client(lwm2m_op_info *op_info)
{
    lwm2m_uri_t uri;
    int result;

    result = lwm2m_stringToUri(op_info->r_uri, strlen(op_info->r_uri), &uri);
    if (result == 0) goto syntax_error;

    if (op_info->r_data != NULL) {
        result = lwm2m_dm_write(lwm2mH, op_info->clientId, &uri, (uint8_t *)(op_info->r_data), strlen(op_info->r_data), prv_result_callback, NULL);
    } else {
        goto syntax_error;
    }

    if (result == 0)
    {
        EMBARC_PRINTF("OK\r\n");
    }
    else
    {
        prv_print_error(result);
    }
    return;

syntax_error:
    EMBARC_PRINTF("Syntax error !\r\n");
}


void lwm2m_server_exec_client(lwm2m_op_info *op_info)
{
    lwm2m_uri_t uri;
    int result;

    result = lwm2m_stringToUri(op_info->r_uri, strlen(op_info->r_uri), &uri);
    if (result == 0) goto syntax_error;

    if (op_info->r_data == NULL) {
        result = lwm2m_dm_execute(lwm2mH, op_info->clientId, &uri, NULL, 0, prv_result_callback, NULL);
    } else {
        result = lwm2m_dm_execute(lwm2mH, op_info->clientId, &uri, (uint8_t *)(op_info->r_data), strlen(op_info->r_data), prv_result_callback, NULL);
    }

    if (result == 0)
    {
        EMBARC_PRINTF("OK\r\n");
    }
    else
    {
        prv_print_error(result);
    }
    return;

syntax_error:
    EMBARC_PRINTF("Syntax error !\r\n");
}

void lwm2m_server_create_client(lwm2m_op_info *op_info)
{
    lwm2m_uri_t uri;
    int result;
    int64_t value;
    char temp_buffer[MAX_PACKET_SIZE];
    int temp_length = 0;

    result = lwm2m_stringToUri(op_info->r_uri, strlen(op_info->r_uri), &uri);
    if (result == 0) goto syntax_error;

    //Get Data to Post
    if (op_info->r_data == NULL) goto syntax_error;

    // TLV

    /* Client dependent part   */

    if (uri.objectId == 1024)
    {
        result = lwm2m_PlainTextToInt64((uint8_t *)(op_info->r_data), strlen(op_info->r_data), &value);
        temp_length = lwm2m_intToTLV(LWM2M_TYPE_RESOURCE, value, (uint16_t) 1, (uint8_t *)temp_buffer, MAX_PACKET_SIZE);
    } else {
        EMBARC_PRINTF("Currently only support object 1024.\n");
        goto syntax_error;
    }
    /* End Client dependent part*/

    //Create
    result = lwm2m_dm_create(lwm2mH, op_info->clientId, &uri, (uint8_t *)temp_buffer, temp_length, prv_result_callback, NULL);

    if (result == 0)
    {
        EMBARC_PRINTF("OK\r\n");
    }
    else
    {
        prv_print_error(result);
    }
    return;

syntax_error:
    EMBARC_PRINTF("Syntax error !\r\n");
}

void lwm2m_server_delete_client(lwm2m_op_info *op_info)
{
    lwm2m_uri_t uri;
    int result;

    result = lwm2m_stringToUri(op_info->r_uri, strlen(op_info->r_uri), &uri);
    if (result == 0) goto syntax_error;

    result = lwm2m_dm_delete(lwm2mH, op_info->clientId, &uri, prv_result_callback, NULL);

    if (result == 0)
    {
        EMBARC_PRINTF("OK\r\n");
    }
    else
    {
        prv_print_error(result);
    }
    return;

syntax_error:
    EMBARC_PRINTF("Syntax error !\r\n");
}

void lwm2m_server_observe_client(lwm2m_op_info *op_info)
{
    lwm2m_uri_t uri;
    int result;

    result = lwm2m_stringToUri(op_info->r_uri, strlen(op_info->r_uri), &uri);
    if (result == 0) goto syntax_error;

    result = lwm2m_observe(lwm2mH, op_info->clientId, &uri, prv_notify_callback, NULL);

    if (result == 0)
    {
        EMBARC_PRINTF("OK\r\n");
    }
    else
    {
        prv_print_error(result);
    }
    return;

syntax_error:
    EMBARC_PRINTF("Syntax error !\r\n");
}

void lwm2m_server_cancel_client(lwm2m_op_info *op_info)
{
    lwm2m_uri_t uri;
    int result;

    result = lwm2m_stringToUri(op_info->r_uri, strlen(op_info->r_uri), &uri);
    if (result == 0) goto syntax_error;

    result = lwm2m_observe_cancel(lwm2mH, op_info->clientId, &uri, prv_result_callback, NULL);

    if (result == 0)
    {
        EMBARC_PRINTF("OK\r\n");
    }
    else
    {
        prv_print_error(result);
    }
    return;

syntax_error:
    EMBARC_PRINTF("Syntax error !\r\n");
}

static void prv_monitor_callback(uint16_t clientID,
                                 lwm2m_uri_t * uriP,
                                 int status,
                                 uint8_t * data,
                                 int dataLength,
                                 void * userData)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) userData;
    lwm2m_client_t * targetP;

    switch (status)
    {
    case COAP_201_CREATED:
        EMBARC_PRINTF("\r\nNew client #%d registered.\r\n", clientID);

        targetP = (lwm2m_client_t *)lwm2m_list_find((lwm2m_list_t *)lwm2mH->clientList, clientID);

        if (targetP) prv_dump_client(targetP);
        break;

    case COAP_202_DELETED:
        EMBARC_PRINTF("\r\nClient #%d unregistered.\r\n", clientID);
        break;

    case COAP_204_CHANGED:
        EMBARC_PRINTF("\r\nClient #%d updated.\r\n", clientID);

        targetP = (lwm2m_client_t *)lwm2m_list_find((lwm2m_list_t *)lwm2mH->clientList, clientID);

        if (targetP) prv_dump_client(targetP);
        break;

    default:
        EMBARC_PRINTF("\r\nMonitor callback called with an unknown status: %d.\r\n", status);
        break;
    }

    EMBARC_PRINTF("\r\n");
}


static void prv_quit(char * buffer,
                     void * user_data)
{
    g_quit = 1;
}

void handle_sigint(int signum)
{
    prv_quit(NULL, NULL);
}


int lwm2mserver(lwm2m_server_info *server_info)
{
    int sock;
    fd_set readfds;
    struct timeval tv;
    int result;
    connection_t * connList = NULL;
    uint16_t serverPort = LWM2M_STANDARD_PORT;

    if (server_info->serverPort > 0) {
        serverPort = server_info->serverPort;
        server_info->serverPort = serverPort;
    }
    EMBARC_PRINTF("Try to start LwM2M server using port %d.\n", serverPort);
    sock = create_socket(1, serverPort);
    if (sock < 0)
    {
        EMBARC_PRINTF("Error opening socket: %d\r\n", errno);
        return -1;
    }

    lwm2mH = lwm2m_init(NULL, prv_buffer_send, NULL);
    if (NULL == lwm2mH)
    {
        EMBARC_PRINTF("lwm2m_init() failed\r\n");
        return -1;
    }

    lwm2m_set_monitoring_callback(lwm2mH, prv_monitor_callback, lwm2mH);
    g_quit = 0;
    while (0 == g_quit)
    {
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        tv.tv_sec = 60;
        tv.tv_usec = 0;

        result = lwm2m_step(lwm2mH, (time_t *)(&(tv.tv_sec)));
        if (result != 0)
        {
            EMBARC_PRINTF("lwm2m_step() failed: 0x%X\r\n", result);
            return -1;
        }

        result = select(FD_SETSIZE, &readfds, 0, 0, &tv);

        if ( result < 0 )
        {
            if (errno != EINTR)
            {
              EMBARC_PRINTF("Error in select(): %d\r\n", errno);
            }
        }
        else if (result > 0)
        {
            uint8_t buffer[MAX_PACKET_SIZE];
            int numBytes;

            if (FD_ISSET(sock, &readfds))
            {
                struct sockaddr_storage addr;
                socklen_t addrLen;

                addrLen = sizeof(addr);
                numBytes = recvfrom(sock, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

                if (numBytes == -1)
                {
                    EMBARC_PRINTF("Error in recvfrom(): %d\r\n", errno);
                }
                else
                {
                    char s[INET_ADDRSTRLEN];
                    in_port_t port;
                    connection_t * connP;

                    s[0] = 0;
                    if (AF_INET == addr.ss_family)
                    {
                        struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
                        inet_ntop(saddr->sin_family, &(saddr->sin_addr), s, INET_ADDRSTRLEN);
                        port = ntohs(saddr->sin_port);
                    } else {
                        EMBARC_PRINTF("IPV6 network package received!\n");
                        continue;
                    }

                    EMBARC_PRINTF("%d bytes received from [%s]:%d\r\n", numBytes, s, port);

                    output_buffer(buffer, numBytes);

                    connP = connection_find(connList, &addr, addrLen);
                    if (connP == NULL)
                    {
                        connP = connection_new_incoming(connList, sock, (struct sockaddr *)&addr, addrLen);
                        if (connP != NULL)
                        {
                            connList = connP;
                        }
                    }
                    if (connP != NULL)
                    {
                        lwm2m_handle_packet(lwm2mH, buffer, numBytes, connP);
                    }
                }
            }
        }
    }

    lwm2m_close(lwm2mH);
    close(sock);
    connection_free(connList);
    lwm2mH = NULL;

    return 0;
}
