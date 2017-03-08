/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
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
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *    Wayne Ren - Modified for port to ARC processor  -  wei.ren@synopsys.com
 *******************************************************************************/

#include "MQTT_lwip.h"

#if LWIP_SO_SNDRCVTIMEO_NONSTANDARD
typedef int SOCKET_TIMEOUT;
#define SOCKET_TIMEOUT_SET(tmout, ms_val)	(*(int *)(tmout) = (ms_val))
#else
typedef struct timeval SOCKET_TIMEOUT;
#define SOCKET_TIMEOUT_SET(tmout, ms_val)	{							\
				((struct timeval *)(tmout))->tv_sec = (long)(ms_val) / 1000; 		\
				((struct timeval *)(tmout))->tv_usec = ((long)(ms_val) % 1000) * 1000;	\
			}
#endif

static SOCKET_TIMEOUT mqtt_socket_tmout;

/** check if expired, if true, return 1, else 0 */
char expired(Timer* timer)
{
	long left = (long)(timer->end_time) - (long)sys_now();
	return (left < 0);
}

/** countdown in ms */
void countdown_ms(Timer* timer, unsigned int timeout)
{
	timer->end_time = sys_now() + timeout;
}

/** Count down in unit of second, this is for MQTT Keep Alive timer */
void countdown(Timer* timer, unsigned int timeout)
{
	countdown_ms(timer, timeout*1000);
}


int left_ms(Timer* timer)
{
	long left = (long)(timer->end_time) - (long)sys_now();
	return (left < 0) ? 0 : left;
}


void InitTimer(Timer* timer)
{
	timer->end_time = 0;
}


static int network_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	int bytes = 0;

	SOCKET_TIMEOUT_SET(&mqtt_socket_tmout, timeout_ms);
	setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)(&mqtt_socket_tmout), sizeof(SOCKET_TIMEOUT));

	while (bytes < len) {
		int rc = recv(n->my_socket, &buffer[bytes], (size_t)(len - bytes), 0);
		if (rc < 0) {
			if (errno != ENOTCONN && errno != ECONNRESET) {
				bytes = -1;
				break;
			}
		}
		else {
			bytes += rc;
		}
	}
	return bytes;
}


static int network_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	int rc;

	SOCKET_TIMEOUT_SET(&mqtt_socket_tmout, timeout_ms);
	setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)(&mqtt_socket_tmout), sizeof(SOCKET_TIMEOUT));

	rc = write(n->my_socket, buffer, len);
	return rc;
}


static void network_disconnect(Network* n)
{
	if (n->my_socket != -1) {
		close(n->my_socket);
	}
}


void NewNetwork(Network* n)
{
	n->my_socket = -1;
	n->mqttread = network_read;
	n->mqttwrite = network_write;
	n->disconnect = network_disconnect;
}


int ConnectNetwork(Network* n, char* addr, int port)
{
	int type = SOCK_STREAM;
	struct sockaddr_in address;
	int rc = -1;
	sa_family_t family = AF_INET;
	struct addrinfo *result = NULL;
	struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

	if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0) {
		struct addrinfo* res = result;

		/* prefer ip4 addresses */
		while (res) {
			if (res->ai_family == AF_INET)
			{
				result = res;
				break;
			}
			res = res->ai_next;
		}

		if (result->ai_family == AF_INET) {
			address.sin_port = htons(port);
			address.sin_family = family = AF_INET;
			address.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
		}
		else {
			rc = -1;
		}

		freeaddrinfo(result);
	}

	if (rc == 0) {
		n->my_socket = socket(family, type, 0);
		if (n->my_socket != -1) {
			rc = connect(n->my_socket, (struct sockaddr*)&address, sizeof(struct sockaddr));
		}
	}

	return rc;
}