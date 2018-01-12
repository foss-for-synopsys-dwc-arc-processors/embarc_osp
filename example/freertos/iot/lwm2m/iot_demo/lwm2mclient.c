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
/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_IOT_LWM2M_IOT_DEMO
 * \brief	Implementation for LwM2M client for IoT Demo
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

#ifndef LWM2M_SERVER_PORT
#define LWM2M_SERVER_PORT 56830
#endif

int g_reboot = 0;
static int g_quit = 0;

#define OBJ_COUNT 4
lwm2m_object_t * objArray[OBJ_COUNT + 10];

typedef struct
{
	lwm2m_object_t * securityObjP;
	lwm2m_object_t * serverObject;
	int sock;
	connection_t * connList;
} client_data_t;

/* function to connect server */
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
	} else if (0==strncmp(uri, "coap://",  strlen("coap://"))) {
		host = uri+strlen("coap://");
	} else {
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
	if (newConnP == NULL) {
		EMBARC_PRINTF("Connection creation failed.\r\n");
	} else {
		dataP->connList = newConnP;
	}

exit:
	lwm2m_free(uri);
	return (void *)newConnP;
}

/* send function when there is data to send */
static uint8_t prv_buffer_send(void * sessionH, uint8_t * buffer, size_t length, void * userdata)
{
	connection_t * connP = (connection_t*) sessionH;

	if (connP == NULL) {
		EMBARC_PRINTF("#> failed sending %lu bytes, missing connection\r\n", length);
		return COAP_500_INTERNAL_SERVER_ERROR ;
	}

	if (-1 == connection_send(connP, buffer, length)) {
		EMBARC_PRINTF("#> failed sending %lu bytes\r\n", length);
		return COAP_500_INTERNAL_SERVER_ERROR ;
	}
	EMBARC_PRINTF("#> sent %lu bytes\r\n", length);
	return COAP_NO_ERROR;
}


int lwm2mclient(lwm2m_client_info *client_info)
{
	client_data_t data;
	int result;
	lwm2m_context_t * lwm2mH = NULL;
	const char * server = client_info->server;
	const char * serverPort = client_info->serverPort;
	const char * name = client_info->ep_name;
	int lifetime = client_info->lifetime;
	time_t reboot_time = 0;


	EMBARC_PRINTF("LwM2M client(%s) try to connect to LwM2M server @%s:%s.\n", name, server, serverPort);

	memset(&data, 0, sizeof(client_data_t));

	/*
	 *This call an internal function that create an IPV6 socket on the port 5683.
	 */
	EMBARC_PRINTF("Trying to open a socket for LwM2M Connection\r\n");
	data.sock = create_socket(0, LWM2M_SERVER_PORT);
	if (data.sock < 0) {
		EMBARC_PRINTF("Failed to open socket: %d\r\n", errno);
		return -1;
	}

	/*
	 * Now the main function fill an array with each object, this list will be later passed to liblwm2m.
	 * Those functions are located in their respective object file.
	 */

	char serverUri[50];
	int serverId = 123;

	sprintf(serverUri, "coap://%s:%s", server, serverPort);
	objArray[0] = get_security_object(serverId, serverUri, false);
	if (NULL == objArray[0]) {
		EMBARC_PRINTF("Failed to create security object\r\n");
		return -1;
	}
	data.securityObjP = objArray[0];

	objArray[1] = get_server_object(serverId, "U", lifetime, false);
	if (NULL == objArray[1]) {
		EMBARC_PRINTF("Failed to create server object\r\n");
		return -1;
	}

	objArray[2] = get_object_device();
	if (NULL == objArray[2]) {
		EMBARC_PRINTF("Failed to create device object\r\n");
		return -1;
	}

	objArray[3] = get_object_firmware();
	if (NULL == objArray[3]) {
		EMBARC_PRINTF("Failed to create firmware object\r\n");
		return -1;
	}
/* get the user object*/
	obj_info *next_obj;
	next_obj = client_info->obj;
	int i = 4;
	while (next_obj) {
		switch (next_obj->oid) {
/* temperature*/
			case 3303:
				objArray[i] = get_object_temperature(next_obj->number);
				break;
/* light*/
			case 3311:
				objArray[i] = get_object_light(next_obj->number);
				break;
/* button*/
			case 3347:
				objArray[i] = get_object_button(next_obj->number);
				break;
/*display*/
			case 3341:
				objArray[i] = get_object_display(next_obj->number);
				break;
		}

		EMBARC_PRINTF("create object: %d\r\n", next_obj->oid);
		if (NULL == objArray[i]) {
			EMBARC_PRINTF("Failed to create object: %d\r\n", next_obj->oid);
			return -1;
		}
		next_obj = next_obj->next;
		i++;
	}
	/*
	 * The liblwm2m library is now initialized with the functions that will be in
	 * charge of communication
	 */
	lwm2mH = lwm2m_init(prv_connect_server, prv_buffer_send, &data);
	if (NULL == lwm2mH) {
		EMBARC_PRINTF("lwm2m_init() failed\r\n");
		return -1;
	}

	/*
	 * We configure the liblwm2m library with the name of the client - which shall be unique for each client -
	 * the number of objects we will be passing through and the objects array
	 */

	result = lwm2m_configure(lwm2mH, (char *)(name), NULL, NULL, i, objArray);

	if (result != 0) {
		lwm2m_free(lwm2mH);
		EMBARC_PRINTF("lwm2m_set_objects() failed: 0x%X\r\n", result);
		return -1;
	}

	/*
	 * This function start your client to the LWM2M servers
	 */
	result = lwm2m_start(lwm2mH);
	if (result != 0) {
		lwm2m_free(lwm2mH);
		EMBARC_PRINTF("lwm2m_register() failed: 0x%X\r\n", result);
		return -1;
	}

	/**
	 * Initialize value changed callback.
	 */
	init_value_change(lwm2mH);

	EMBARC_PRINTF("lwm2m client started.\n");
	/*
	 * We now enter in a while loop that will handle the communications from the server
	 */
	g_quit = 0;
	g_reboot = 0;
	while (0 == g_quit) {
		struct timeval tv;
		fd_set readfds;

		if (g_reboot) {
			time_t tv_sec;

			tv_sec = lwm2m_gettime();

			if (0 == reboot_time) {
				reboot_time = tv_sec + 5;
			}
			if (reboot_time < tv_sec){
				/*
				 * Message should normally be lost with reboot ...
				 */
				EMBARC_PRINTF("reboot time expired, rebooting ...\n");
				g_quit = 1;
			} else {
				tv.tv_sec = reboot_time - tv_sec;
			}
		} else {
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
		if (result != 0) {
			EMBARC_PRINTF("lwm2m_step() failed: 0x%X\r\n", result);
			return -1;
		}

		/*
		 * This part will set up an interruption until an event happen on SDTIN or the socket until "tv" timed out (set
		 * with the precedent function)
		 */
		result = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

		if (result < 0) {
			if (errno != EINTR) {
			  EMBARC_PRINTF("Error in select(): %d\r\n", errno);
			}
		} else if (result > 0) {
			uint8_t buffer[MAX_PACKET_SIZE];
			int numBytes;

			/*
			 * If an event happen on the socket
			 */
			if (FD_ISSET(data.sock, &readfds)) {
				struct sockaddr_storage addr;
				socklen_t addrLen;

				addrLen = sizeof(addr);

				/*
				 * We retrieve the data received
				 */
				numBytes = recvfrom(data.sock, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

				if (0 > numBytes) {
					EMBARC_PRINTF("Error in recvfrom(): %d\r\n", errno);
				} else if (0 < numBytes) {
					char s[INET_ADDRSTRLEN];
					in_port_t port = 0;
					connection_t * connP;

					if (AF_INET == addr.ss_family) {
						struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
						inet_ntop(saddr->sin_family, &(saddr->sin_addr), s, INET_ADDRSTRLEN);
						port = ntohs(saddr->sin_port);
					}

					EMBARC_PRINTF("%d bytes received from [%s]:%d\r\n", numBytes, s, port);

					/*
					 * Display it in the STDERR
					 */
					//output_buffer(buffer, numBytes);

					connP = connection_find(data.connList, &addr, addrLen);
					if (connP != NULL) {
						/*
						 * Let liblwm2m respond to the query depending on the context
						 */
						lwm2m_handle_packet(lwm2mH, buffer, numBytes, connP);
					} else {
						EMBARC_PRINTF("received bytes ignored!\r\n");
					}
				}
			}
		}
	}

	/*
	 * Finally when the loop is left smoothly - asked by user in the command line interface - we unregister our client from it
	 */
	if (g_quit != 0) {
		lwm2m_close(lwm2mH);
	}
	close(data.sock);
	connection_free(data.connList);
	if (g_reboot != 0){
		system_reboot();
	}
	return 0;
}