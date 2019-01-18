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
 *    Benjamin Cabé - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Simon Bernard - Please refer to git log
 *    Julien Vermillard - Please refer to git log
 *    Axel Lorente - Please refer to git log
 *    Toby Jaffey - Please refer to git log
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
 Bosch Software Innovations GmbH - Please refer to git log

*/
/**
 * \file
 * \ingroup	EMBARC_APP_LWIP_FREERTOS_LWM2M_CLIENT
 * \brief	Implementation for LwM2M client
 */

#include "lwm2mclient.h"
#include "liblwm2m.h"
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

int g_reboot = 0;
static int g_quit = 0;

#if defined(BOARD_EMSK)
#define OBJ_COUNT 9
#else
#define OBJ_COUNT 8
#endif
lwm2m_object_t * objArray[OBJ_COUNT];

// only backup security and server objects
# define BACKUP_OBJECT_COUNT 2
lwm2m_object_t * backupObjectArray[BACKUP_OBJECT_COUNT];

typedef struct
{
    lwm2m_object_t * securityObjP;
    lwm2m_object_t * serverObject;
    int sock;
    connection_t * connList;
} client_data_t;

static void prv_quit(char * buffer,
                     void * user_data)
{
    g_quit = 1;
}

void handle_sigint(int signum)
{
    g_quit = 2;
}

// void print_usage(void)
// {
//     EMBARC_PRINTF("Usage: lwm2mclient [[[localPort] server] serverPort]\r\n");
//     EMBARC_PRINTF("Launch a LWM2M client.\r\n\n");
// }

static lwm2m_object_t * prv_find_object(lwm2m_context_t * contextP,
                                        uint16_t Id)
{
    int i;

    for (i = 0 ; i < contextP->numObject ; i++)
    {
        if (contextP->objectList[i]->objID == Id)
        {
            return contextP->objectList[i];
        }
    }

    return NULL;
}

void handle_value_changed(lwm2m_context_t * lwm2mH,
                          lwm2m_uri_t * uri,
                          const char * value,
                          size_t valueLength)
{
    lwm2m_object_t * object = prv_find_object(lwm2mH, uri->objectId);

    if (NULL != object)
    {
        if (object->writeFunc != NULL)
        {
            lwm2m_tlv_t * tlvP;
            int result;

            tlvP = lwm2m_tlv_new(1);
            if (tlvP == NULL)
            {
                EMBARC_PRINTF("Internal allocation failure !\n");
                return;
            }
            tlvP->flags = LWM2M_TLV_FLAG_STATIC_DATA | LWM2M_TLV_FLAG_TEXT_FORMAT;
#ifdef LWM2M_BOOTSTRAP
            if (lwm2mH->bsState == BOOTSTRAP_PENDING) {
                tlvP->flags |= LWM2M_TLV_FLAG_BOOTSTRAPPING;
            }
#endif
            tlvP->id = uri->resourceId;
            tlvP->length = valueLength;
            tlvP->value = (uint8_t*) value;

            result = object->writeFunc(uri->instanceId, 1, tlvP, object);
            if (COAP_405_METHOD_NOT_ALLOWED == result)
            {
                switch (uri->objectId)
                {
                case LWM2M_DEVICE_OBJECT_ID:
                    result = device_change(tlvP, object);
                    break;
                default:
                    break;
                }
            }

            if (COAP_204_CHANGED != result)
            {
                EMBARC_PRINTF("Failed to change value!\n");
            }
            else
            {
                EMBARC_PRINTF("value changed!\n");
                lwm2m_resource_value_changed(lwm2mH, uri);
            }
            lwm2m_tlv_free(1, tlvP);
            return;
        }
        else
        {
            EMBARC_PRINTF("write not supported for specified resource!\n");
        }
        return;
    }
    else
    {
        EMBARC_PRINTF("Object not found !\n");
    }
}

static void * prv_connect_server(uint16_t secObjInstID, void * userData)
{
    client_data_t * dataP;
    char * uri;
    char * host;
    char * portStr;
    int port;
    char * ptr;
    connection_t * newConnP = NULL;

    dataP = (client_data_t *)userData;

    uri = get_server_uri(dataP->securityObjP, secObjInstID);
    if (uri == NULL) return NULL;

    // parse uri in the form "coaps://[host]:[port]"
    if (0==strncmp(uri, "coaps://", strlen("coaps://"))) {
        host = uri+strlen("coaps://");
    }
    else if (0==strncmp(uri, "coap://",  strlen("coap://"))) {
        host = uri+strlen("coap://");
    }
    else {
        goto exit;
    }

    portStr = strchr(host, ':');
    if (portStr == NULL) goto exit;
    // split strings
    *portStr = 0;
    portStr++;
    port = strtol(portStr, &ptr, 10);
    if (*ptr != 0) goto exit;

    EMBARC_PRINTF("Trying to connect to LWM2M Server at %s:%d\r\n", host, port);
    newConnP = connection_create(dataP->connList, dataP->sock, host, port);
    if (newConnP == NULL)
    {
        EMBARC_PRINTF("Connection creation failed.\r\n");
    }
    else
    {
        dataP->connList = newConnP;
    }

exit:
    lwm2m_free(uri);
    return (void *)newConnP;
}

static uint8_t prv_buffer_send(void * sessionH,
                               uint8_t * buffer,
                               size_t length,
                               void * userdata)
{
    connection_t * connP = (connection_t*) sessionH;

    if (connP == NULL)
    {
        EMBARC_PRINTF("#> failed sending %lu bytes, missing connection\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }

    if (-1 == connection_send(connP, buffer, length))
    {
        EMBARC_PRINTF("#> failed sending %lu bytes\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }
    conn_s_updateTxStatistic(objArray[7], (uint16_t)length, false);
    EMBARC_PRINTF("#> sent %lu bytes\r\n", length);
    return COAP_NO_ERROR;
}

static void prv_output_servers(char * buffer,
                               void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_server_t * targetP;

    targetP = lwm2mH->serverList;

    if (targetP == NULL)
    {
        EMBARC_PRINTF("No server.\r\n");
        return;
    }

    for (targetP = lwm2mH->serverList ; targetP != NULL ; targetP = targetP->next)
    {
        EMBARC_PRINTF("Server ID %d:\r\n", targetP->shortID);
        EMBARC_PRINTF("\tstatus: ");
        switch(targetP->status)
        {
        case STATE_DEREGISTERED:
            EMBARC_PRINTF("DEREGISTERED\r\n");
            break;
        case STATE_REG_PENDING:
            EMBARC_PRINTF("REGISTRATION PENDING\r\n");
            break;
        case STATE_REGISTERED:
            EMBARC_PRINTF("REGISTERED location: \"%s\"\r\n", targetP->location);
            EMBARC_PRINTF("\tLifetime: %lu s\r\n", (unsigned long)targetP->lifetime);
            break;
        case STATE_REG_UPDATE_PENDING:
            EMBARC_PRINTF("REGISTRATION UPDATE PENDING\r\n");
            break;
        case STATE_DEREG_PENDING:
            EMBARC_PRINTF("DEREGISTRATION PENDING\r\n");
            break;
        case STATE_REG_FAILED:
            EMBARC_PRINTF("REGISTRATION FAILED\r\n");
            break;
        }
        EMBARC_PRINTF("\r\n");
    }
}

// static void prv_change(char * buffer,
//                        void * user_data)
// {
//     lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
//     lwm2m_uri_t uri;
//     char * end = NULL;
//     int result;

//     end = get_end_of_arg(buffer);
//     if (end[0] == 0) goto syntax_error;

//     result = lwm2m_stringToUri(buffer, end - buffer, &uri);
//     if (result == 0) goto syntax_error;

//     buffer = get_next_arg(end, &end);

//     if (buffer[0] == 0)
//     {
//         EMBARC_PRINTF("report change!\n");
//         lwm2m_resource_value_changed(lwm2mH, &uri);
//     }
//     else
//     {
//         handle_value_changed(lwm2mH, &uri, buffer, end - buffer);
//     }
//     return;

// syntax_error:
//     EMBARC_PRINTF("Syntax error !\n");
// }

static void prv_update(void * user_data)
{
    static time_t next_update_time = 0;
    time_t tv_sec;
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_server_t * targetP;

    tv_sec = lwm2m_gettime();
    if (tv_sec < 0) return;

    targetP = lwm2mH->serverList;

    if (targetP == NULL)
    {
        EMBARC_PRINTF("No server.\r\n");
        return;
    }

    for (targetP = lwm2mH->serverList ; targetP != NULL ; targetP = targetP->next)
    {
        if (next_update_time < tv_sec) {
            next_update_time = tv_sec + 30;
            switch(targetP->status)
            {
            case STATE_DEREGISTERED:
            case STATE_REG_PENDING:
            case STATE_REG_UPDATE_PENDING:
            case STATE_DEREG_PENDING:
            case STATE_REG_FAILED:
                break;
            case STATE_REGISTERED: /* keep alive */
                if (lwm2m_update_registration(lwm2mH, targetP->shortID) != 0) {
                    EMBARC_PRINTF("Registration update error\n");
                }
                break;
            }
        }
    }

    return;
}

static void update_battery_level(lwm2m_context_t * context)
{
    static time_t next_change_time = 0;
    time_t tv_sec;

    tv_sec = lwm2m_gettime();
    if (tv_sec < 0) return;

    if (next_change_time < tv_sec)
    {
        char value[15];
        int valueLength;
        lwm2m_uri_t uri;
        int level = rand() % 100;

        if (0 > level) level = -level;
        if (lwm2m_stringToUri("/3/0/9", 6, &uri))
        {
            valueLength = sprintf(value, "%d", level);
            EMBARC_PRINTF("New Battery Level: %d\n", level);
            handle_value_changed(context, &uri, value, valueLength);
        }
        level = rand() % 20;
        if (0 > level) level = -level;
        next_change_time = tv_sec + level + 10;
    }
}

#ifdef LWM2M_BOOTSTRAP

static void prv_initiate_bootstrap(char * buffer,
                                   void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    if ((lwm2mH->bsState != BOOTSTRAP_CLIENT_HOLD_OFF)
            && (lwm2mH->bsState != BOOTSTRAP_PENDING)) {
        lwm2mH->bsState = BOOTSTRAP_REQUESTED;
    }
}

static void prv_display_objects(char * buffer,
                                void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    int i;
    if (NULL != lwm2mH->objectList) {
        for (i = 0; i < lwm2mH->numObject; i++) {
            lwm2m_object_t * object = lwm2mH->objectList[i];
            if (NULL != object) {
                switch (object->objID)
                {
                case LWM2M_SECURITY_OBJECT_ID:
                    display_security_object(object);
                    break;
                case LWM2M_SERVER_OBJECT_ID:
                    display_server_object(object);
                    break;
                case LWM2M_ACL_OBJECT_ID:
                    break;
                case LWM2M_DEVICE_OBJECT_ID:
                    display_device_object(object);
                    break;
                case LWM2M_CONN_MONITOR_OBJECT_ID:
                    break;
#if defined(BOARD_EMSK)
                case LWM2M_FIRMWARE_UPDATE_OBJECT_ID:
                    display_firmware_object(object);
                    break;
#endif
                case LWM2M_LOCATION_OBJECT_ID:
                    display_location_object(object);
                    break;
                case LWM2M_CONN_STATS_OBJECT_ID:
                    break;
                case TEST_OBJECT_ID:
                    display_test_object(object);
                    break;
                }
            }
        }
    }
}

static void prv_display_backup(char * buffer,
        void * user_data)
{
    if (NULL != backupObjectArray) {
        int i;
        for (i = 0 ; i < BACKUP_OBJECT_COUNT ; i++) {
            lwm2m_object_t * object = backupObjectArray[i];
            if (NULL != object) {
                switch (object->objID)
                {
                case LWM2M_SECURITY_OBJECT_ID:
                    display_security_object(object);
                    break;
                case LWM2M_SERVER_OBJECT_ID:
                    display_server_object(object);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

static void prv_display_bootstrap_state(lwm2m_bootstrap_state_t bootstrapState)
{
    switch (bootstrapState) {
    case NOT_BOOTSTRAPPED:
        EMBARC_PRINTF("NOT BOOTSTRAPPED\r\n");
        break;
    case BOOTSTRAP_REQUESTED:
        EMBARC_PRINTF("DI BOOTSTRAP REQUESTED\r\n");
        break;
    case BOOTSTRAP_CLIENT_HOLD_OFF:
        EMBARC_PRINTF("DI BOOTSTRAP CLIENT HOLD OFF\r\n");
        break;
    case BOOTSTRAP_INITIATED:
        EMBARC_PRINTF("DI BOOTSTRAP INITIATED\r\n");
        break;
    case BOOTSTRAP_PENDING:
        EMBARC_PRINTF("DI BOOTSTRAP PENDING\r\n");
        break;
    case BOOTSTRAP_FINISHED:
        EMBARC_PRINTF("DI BOOTSTRAP FINISHED\r\n");
        break;
    case BOOTSTRAP_FAILED:
        EMBARC_PRINTF("DI BOOTSTRAP FAILED\r\n");
        break;
    case BOOTSTRAPPED:
        EMBARC_PRINTF("BOOTSTRAPPED\r\n");
        break;
    default:
        break;
    }
}

static void prv_backup_objects(lwm2m_context_t * context)
{
    uint16_t i;

    for (i = 0; i < BACKUP_OBJECT_COUNT; i++) {
        if (NULL != backupObjectArray[i]) {
            backupObjectArray[i]->closeFunc(backupObjectArray[i]);
            lwm2m_free(backupObjectArray[i]);
        }
        backupObjectArray[i] = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
        memset(backupObjectArray[i], 0, sizeof(lwm2m_object_t));
    }

    /*
     * Backup content of objects 0 (security) and 1 (server)
     */
    for (i = 0; i < context->numObject; i++) {
        lwm2m_object_t * object = context->objectList[i];
        if (NULL != object) {
            switch (object->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
                copy_security_object(backupObjectArray[0], object);
                break;
            case LWM2M_SERVER_OBJECT_ID:
                copy_server_object(backupObjectArray[1], object);
                break;
            default:
                break;
            }
        }
    }
}

static void prv_restore_objects(lwm2m_context_t * context)
{
    uint16_t i;

    /*
     * Restore content  of objects 0 (security) and 1 (server)
     */
    for (i = 0; i < context->numObject; i++) {
        lwm2m_object_t * object = context->objectList[i];
        if (NULL != object) {
            switch (object->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
                // first delete internal content
                object->closeFunc(object);
                // then restore previous object
                copy_security_object(object, backupObjectArray[0]);
                break;
            case LWM2M_SERVER_OBJECT_ID:
                // first delete internal content
                object->closeFunc(object);
                // then restore previous object
                copy_server_object(object, backupObjectArray[1]);
                break;
            default:
                break;
            }
        }
    }

    // restart the old servers
    lwm2m_start(context);
    EMBARC_PRINTF("[BOOTSTRAP] ObjectList restored\r\n");
}

static void prv_connections_free(lwm2m_context_t * context)
{
    client_data_t * app_data;

    app_data = context->userData;
    if (NULL != app_data)
    {
        connection_free(app_data->connList);
        app_data->connList = NULL;
    }
}

static void update_bootstrap_info(lwm2m_bootstrap_state_t * previousBootstrapState,
        lwm2m_context_t * context)
{
    if (*previousBootstrapState != context->bsState)
    {
        *previousBootstrapState = context->bsState;
        switch(context->bsState)
        {
            case BOOTSTRAP_CLIENT_HOLD_OFF:
#ifdef WITH_LOGS
                EMBARC_PRINTF("[BOOTSTRAP] backup security and server objects\r\n");
#endif
                prv_backup_objects(context);
                break;
            case BOOTSTRAP_FINISHED:
#ifdef WITH_LOGS
                EMBARC_PRINTF("[BOOTSTRAP] free connections\r\n");
#endif
                prv_connections_free(context);
                break;
            case BOOTSTRAP_FAILED:
#ifdef WITH_LOGS
                EMBARC_PRINTF("[BOOTSTRAP] restore security and server objects\r\n");
#endif
                prv_connections_free(context);
                prv_restore_objects(context);
                break;
            default:
                break;
        }
    }

#ifdef WITH_LOGS
    prv_display_bootstrap_state(context->bsState);
#endif
}

static void close_backup_object()
{
    int i;
    for (i = 0; i < BACKUP_OBJECT_COUNT; i++) {
        if (NULL != backupObjectArray[i]) {
            backupObjectArray[i]->closeFunc(backupObjectArray[i]);
            lwm2m_free(backupObjectArray[i]);
        }
    }
}
#endif

int lwm2mclient(lwm2m_client_info *client_info)
{
    client_data_t data;
    int result;
    lwm2m_context_t * lwm2mH = NULL;
    const char * server = "leshan.eclipse.org";
    const char * serverPort = LWM2M_STANDARD_PORT_STR;
    const char * name = "embARCLwM2M";
    int lifetime = 300;
    int batterylevelchanging = 1;
    time_t reboot_time = 0;

#ifdef LWM2M_BOOTSTRAP
    char* bootstrapRequested = "no";
    lwm2m_bootstrap_state_t previousBootstrapState = NOT_BOOTSTRAPPED;
#endif

    if (client_info->server == NULL) {
        EMBARC_PRINTF("LwM2M Server is invalid, please check!\n");
        return -1;
    } else {
        server = client_info->server;
    }
    if (client_info->ep_name != NULL) {
        name = client_info->ep_name;
    }
    if (client_info->serverPort != NULL) {
        serverPort = client_info->serverPort;
    }

    EMBARC_PRINTF("LwM2M client(%s) try to connect to LwM2M server @%s:%s.\n", name, server, serverPort);

    memset(&data, 0, sizeof(client_data_t));

    /*
     *This call an internal function that create an IPV6 socket on the port 5683.
     */
    EMBARC_PRINTF("Trying to open a socket for LwM2M Connection\r\n");
    data.sock = create_socket(0, 56830);
    if (data.sock < 0)
    {
        EMBARC_PRINTF("Failed to open socket: %d\r\n", errno);
        return -1;
    }

    /*
     * Now the main function fill an array with each object, this list will be later passed to liblwm2m.
     * Those functions are located in their respective object file.
     */
    objArray[0] = get_object_device();
    if (NULL == objArray[0])
    {
        EMBARC_PRINTF("Failed to create Device object\r\n");
        return -1;
    }

    objArray[1] = get_test_object();
    if (NULL == objArray[1])
    {
        EMBARC_PRINTF("Failed to create test object\r\n");
        return -1;
    }

    int serverId = 123;
    objArray[2] = get_server_object(serverId, "U", lifetime, false);
    if (NULL == objArray[2])
    {
        EMBARC_PRINTF("Failed to create server object\r\n");
        return -1;
    }

    char serverUri[50];
    sprintf(serverUri, "coap://%s:%s", server, serverPort);
    objArray[3] = get_security_object(serverId, serverUri, false);
    if (NULL == objArray[3])
    {
        EMBARC_PRINTF("Failed to create security object\r\n");
        return -1;
    }
    data.securityObjP = objArray[3];

    objArray[4] = get_object_location();
    if (NULL == objArray[4])
    {
        EMBARC_PRINTF("Failed to create location object\r\n");
        return -1;
    }

    objArray[5] = get_object_conn_m();
    if (NULL == objArray[5])
    {
        EMBARC_PRINTF("Failed to create connectivity monitoring object\r\n");
        return -1;
    }

    objArray[6] = get_object_conn_s();
    if (NULL == objArray[6])
    {
        EMBARC_PRINTF("Failed to create connectivity statistics object\r\n");
        return -1;
    }

    int instId = 0;
    objArray[7] = acc_ctrl_create_object();
    if (NULL == objArray[7])
    {
        EMBARC_PRINTF("Failed to create Access Control object\r\n");
        return -1;
    }
    else if (acc_ctrl_obj_add_inst(objArray[7], instId, 3, 0, serverId)==false)
    {
        EMBARC_PRINTF("Failed to create Access Control object instance\r\n");
        return -1;
    }
    else if (acc_ctrl_oi_add_ac_val(objArray[7], instId, 0, 0b000000000001111)==false)
    {
        EMBARC_PRINTF("Failed to create Access Control ACL default resource\r\n");
        return -1;
    }
    else if (acc_ctrl_oi_add_ac_val(objArray[7], instId, 999, 0b000000000000001)==false)
    {
        EMBARC_PRINTF("Failed to create Access Control ACL resource for serverId: 999\r\n");
        return -1;
    }

#if defined(BOARD_EMSK)
    objArray[8] = get_object_firmware();
    if (NULL == objArray[8])
    {
        EMBARC_PRINTF("Failed to create Firmware object\r\n");
        return -1;
    }
#endif
    /*
     * The liblwm2m library is now initialized with the functions that will be in
     * charge of communication
     */
    lwm2mH = lwm2m_init(prv_connect_server, prv_buffer_send, &data);
    if (NULL == lwm2mH)
    {
        EMBARC_PRINTF("lwm2m_init() failed\r\n");
        return -1;
    }

    /*
     * We configure the liblwm2m library with the name of the client - which shall be unique for each client -
     * the number of objects we will be passing through and the objects array
     */
    result = lwm2m_configure(lwm2mH, (char *)(name), NULL, NULL, OBJ_COUNT, objArray);
    if (result != 0)
    {
        lwm2m_free(lwm2mH);
        EMBARC_PRINTF("lwm2m_set_objects() failed: 0x%X\r\n", result);
        return -1;
    }

    //signal(SIGINT, handle_sigint);

    /*
     * This function start your client to the LWM2M servers
     */
    result = lwm2m_start(lwm2mH);
    if (result != 0)
    {
        lwm2m_free(lwm2mH);
        EMBARC_PRINTF("lwm2m_register() failed: 0x%X\r\n", result);
        return -1;
    }

    /**
     * Initialize value changed callback.
     */
    init_value_change(lwm2mH);

    EMBARC_PRINTF("> ");
    EMBARC_PRINTF("lwm2m client started.\n");
    /*
     * We now enter in a while loop that will handle the communications from the server
     */
    g_quit = 0;
    g_reboot = 0;
    while (0 == g_quit)
    {
        struct timeval tv;
        fd_set readfds;

        if (g_reboot)
        {
            time_t tv_sec;

            tv_sec = lwm2m_gettime();

            if (0 == reboot_time)
            {
                reboot_time = tv_sec + 5;
            }
            if (reboot_time < tv_sec)
            {
                /*
                 * Message should normally be lost with reboot ...
                 */
                EMBARC_PRINTF("reboot time expired, rebooting ...\n");
                system_reboot();
                g_quit = 1;
            }
            else
            {
                tv.tv_sec = reboot_time - tv_sec;
            }
        }
        else if (batterylevelchanging)
        {
            update_battery_level(lwm2mH);
            tv.tv_sec = 5;
            prv_update(lwm2mH);
        }
        else
        {
            tv.tv_sec = 60;
        }
        tv.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_SET(data.sock, &readfds);

        /*
         * This function does two things:
         *  - first it does the work needed by liblwm2m (eg. (re)sending some packets).
         *  - Secondly it adjust the timeout value (default 60s) depending on the state of the transaction
         *    (eg. retransmission) and the time between the next operation
         */
        result = lwm2m_step(lwm2mH, (time_t *)(&(tv.tv_sec)));
        if (result != 0)
        {
            EMBARC_PRINTF("lwm2m_step() failed: 0x%X\r\n", result);
            return -1;
        }

        /*
         * This part will set up an interruption until an event happen on SDTIN or the socket until "tv" timed out (set
         * with the precedent function)
         */
        result = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

        if (result < 0)
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

            /*
             * If an event happen on the socket
             */
            if (FD_ISSET(data.sock, &readfds))
            {
                struct sockaddr_storage addr;
                socklen_t addrLen;

                addrLen = sizeof(addr);

                /*
                 * We retrieve the data received
                 */
                numBytes = recvfrom(data.sock, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

                if (0 > numBytes)
                {
                    EMBARC_PRINTF("Error in recvfrom(): %d\r\n", errno);
                }
                else if (0 < numBytes)
                {
                    char s[INET_ADDRSTRLEN];
                    in_port_t port = 0;
                    connection_t * connP;

                    if (AF_INET == addr.ss_family)
                    {
                        struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
                        inet_ntop(saddr->sin_family, &(saddr->sin_addr), s, INET_ADDRSTRLEN);
                        port = ntohs(saddr->sin_port);
                    } else {
                        EMBARC_PRINTF("IPV6 network package received!\n");
                        continue;
                    }
                    // else if (AF_INET6 == addr.ss_family)
                    // {
                    //     struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&addr;
                    //     inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
                    //     port = saddr->sin6_port;
                    // }
                    EMBARC_PRINTF("%d bytes received from [%s]:%d\r\n", numBytes, s, port);

                    /*
                     * Display it in the STDERR
                     */
                    output_buffer(buffer, numBytes);

                    connP = connection_find(data.connList, &addr, addrLen);
                    if (connP != NULL)
                    {
                        /*
                         * Let liblwm2m respond to the query depending on the context
                         */
                        lwm2m_handle_packet(lwm2mH, buffer, numBytes, connP);
                        conn_s_updateRxStatistic(objArray[7], numBytes, false);
                    }
                    else
                    {
                        EMBARC_PRINTF("received bytes ignored!\r\n");
                    }
                }
            }
        }
    }

    /*
     * Finally when the loop is left smoothly - asked by user in the command line interface - we unregister our client from it
     */
    if (g_quit != 0)
    {
        lwm2m_close(lwm2mH);
    }
    close(data.sock);
    connection_free(data.connList);
    if (g_quit == 2) {
        EMBARC_PRINTF("LwM2M client quit successfully!\n");
    }

    return 0;
}
