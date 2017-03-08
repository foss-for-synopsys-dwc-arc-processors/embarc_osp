/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 * Modified by Qiang Gu(Qiang.Gu@synopsys.com) for embARC
 */
/**
 * \defgroup	EMBARC_APP_CONTIKI_TINYDTLS_DTLS_SERVER_IPV4	embARC Contiki TinyDTLS DTLS Server IPv4 Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_CONTIKI
 * \brief	embARC Contiki TinyDTLS dtls server ipv4 example
 *
 * \details
 * ### Extra Required Tools
 *     - Digilent PMOD WIFI(MRF24WG0MA)
 *
 * ### Extra Required Peripherals
 *
 * ### Design Concept
 *     This example is designed to test the contiki tinydtls dtls server functionality.
 *
 * ### Usage Manual
 *     Run this example you will see the following output.
 *     Refer to \ref EMBARC_APP_CONTIKI_TINYDTLS_DTLS_CLIENT_IPV4 for client node.
 *
 *     ![ScreenShot of tinydtls server example](pic/images/example/emsk/contiki_tinydtls_dtls_server_ipv4.jpg)
 *
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_CONTIKI_TINYDTLS_DTLS_SERVER_IPV4
 * \brief	tinydtls server test example source file
 */

/**
 * \addtogroup	EMBARC_APP_CONTIKI_TINYDTLS_DTLS_SERVER_IPV4
 * @{
 */
#include "embARC.h"

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include "../platform/emsk/net/mrf24g/mrf24g_drv.h" //compatible with RW009

#include <string.h>

#include "tinydtls.h"
#include "dtls.h"
#include "dtls_debug.h"


#ifndef DEBUG
#define DEBUG DEBUG_PRINT
#endif
#include "net/ip/uip-debug.h"

#define UIP_IP_BUF	((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF	((struct uip_udp_hdr *)&uip_buf[UIP_LLIPH_LEN])

#define MAX_PAYLOAD_LEN 120


#define UDP_SERVER_PORT	5683
#define	LOCALHOST	EMBARC_TO_STRING(192.168.43.10) // local address
#define	NETMASK		EMBARC_TO_STRING(255.255.255.0)
#define NET_GATEWAY	EMBARC_TO_STRING(192.168.43.1)
#define DNS_SERVER	NET_GATEWAY


#define ip_addr_dump(msg, ipaddr) \
	EMBARC_PRINTF("%s:%d.%d.%d.%d\r\n",  (msg), 		\
	((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[0] : 0),	\
	((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[1] : 0),	\
	((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[2] : 0),	\
	((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[3] : 0))

static struct uip_udp_conn *server_conn;

static dtls_context_t *dtls_context;

process_event_t dtls_event;


#define DTLS_SERVER_CMD_CLOSE "server:close"
#define DTLS_SERVER_CMD_RENEGOTIATE "server:renegotiate"

static int read_from_peer(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len)
{
	EMBARC_PRINTF("data from client:");
	size_t i;
	for (i = 0; i < len; i++)
		EMBARC_PRINTF("%c", data[i]);
	EMBARC_PRINTF("\r\n");

	if (len >= strlen(DTLS_SERVER_CMD_CLOSE) &&
		!memcmp(data, DTLS_SERVER_CMD_CLOSE, strlen(DTLS_SERVER_CMD_CLOSE))) {
    		EMBARC_PRINTF("server: closing connection\n");
    		dtls_close(ctx, session);
    		return len;
    	} else if (len >= strlen(DTLS_SERVER_CMD_RENEGOTIATE) &&
    		!memcmp(data, DTLS_SERVER_CMD_RENEGOTIATE, strlen(DTLS_SERVER_CMD_RENEGOTIATE))) {
    		EMBARC_PRINTF("server: renegotiate connection\n");
    		dtls_renegotiate(ctx, session);
    		return len;
    	}
	/* echo incoming application data */
	return dtls_write(ctx, session, data, len);
}

static int send_to_peer(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len)
{
	struct uip_udp_conn *conn = (struct uip_udp_conn *)dtls_get_app_data(ctx);

	uip_ipaddr_copy(&conn->ripaddr, &session->addr);
	conn->rport = session->port;

	uip_udp_packet_send(conn, data, len);

	return len;
}

int dtls_event_handler(struct dtls_context_t *ctx, session_t *session,
		dtls_alert_level_t level, unsigned short code)
{
	unsigned int event;

	if (code == DTLS_EVENT_CONNECTED) {
		EMBARC_PRINTF("connected to client\r\n");
		event = DTLS_EVENT_CONNECTED;
	}

	if (level == DTLS_ALERT_LEVEL_WARNING) {
		EMBARC_PRINTF("warning alert from peer, code:%d\r\n", code);
		event =	DTLS_ALERT_LEVEL_WARNING;
	} else if (level == DTLS_ALERT_LEVEL_FATAL) {
		EMBARC_PRINTF("fatal alert from peer, code:%d\r\n", code);
		event = DTLS_ALERT_LEVEL_FATAL;
	}

	process_post(PROCESS_BROADCAST, dtls_event, (process_data_t)event);
	return 0;
}

/* This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identity within this particular
 * session. */
static int get_psk_info(struct dtls_context_t *ctx, const session_t *session,
	dtls_credentials_type_t type,
	const unsigned char *id, size_t id_len,
	unsigned char *result, size_t result_length)
{

	struct keymap_t {
		unsigned char *id;
		size_t id_length;
		unsigned char *key;
		size_t key_length;
	} psk[3] = {
		{ (unsigned char *)"Client_identity", 15, (unsigned char *)"secretPSK", 9 },
		{ (unsigned char *)"default identity", 16, (unsigned char *)"\x11\x22\x33", 3 },
		{ (unsigned char *)"\0", 2, (unsigned char *)"", 1 }
	};

	if (type != DTLS_PSK_KEY) {
    		return 0;
    	}

    	if (id) {
    		int i;
    		for (i = 0; i < sizeof(psk)/sizeof(struct keymap_t); i++) {
    			if (id_len == psk[i].id_length && memcmp(id, psk[i].id, id_len) == 0) {
    				if (result_length < psk[i].key_length) {
    					dtls_warn("buffer too small for PSK");
    					return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    				}
    				memcpy(result, psk[i].key, psk[i].key_length);
    				return psk[i].key_length;
    			}
    		}
    	}
    	return dtls_alert_fatal_create(DTLS_ALERT_DECRYPT_ERROR);
}


PROCESS(udp_server_process, "UDP secure server process");;

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&udp_server_process);
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
static void dtls_handle_read(dtls_context_t *ctx) {
	session_t session;

	if(uip_newdata()) {
		uip_ipaddr_copy(&session.addr, &UIP_IP_BUF->srcipaddr);
		session.port = UIP_UDP_BUF->srcport;
		session.size = sizeof(session.addr) + sizeof(session.port);
		session.ifindex = 1; /* from pmwifi */
		dtls_handle_message(ctx, &session, uip_appdata, uip_datalen());
	}
}

void init_dtls() {
	static dtls_handler_t cb = {
		.write = send_to_peer,
		.read  = read_from_peer,
		.event = dtls_event_handler,
		.get_psk_info = get_psk_info
	};

	EMBARC_PRINTF("DTLS secure server started\r\n");

	server_conn = udp_new(NULL, 0, NULL);
	udp_bind(server_conn, UIP_HTONS(UDP_SERVER_PORT));

	dtls_set_log_level(DTLS_LOG_ALERT);

	dtls_context = dtls_new_context(server_conn);

	if (dtls_context)
		dtls_set_handler(dtls_context, &cb);
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
	DEV_WNIC_PTR wifi;
	uip_ipaddr_t ip;

	PROCESS_BEGIN();


	process_start(&tcpip_process, NULL);
	process_start(&mrf24g_process, NULL);

	EMBARC_PRINTF("---tinydtls server example---\r\n");
	EMBARC_PRINTF("\r\nNow trying to connect to WIFI Hotspot, Please wait about 30s!\r\n");
	/* wait wifi connected */
	while (1) {
		PROCESS_WAIT_EVENT();
		if (ev == wnic_event) {
			wifi = (DEV_WNIC_PTR)data;
			if (wifi->wnic_info.conn_status == WNIC_CONNECTED) {
				break;
			}
		}
	}

	/* set network parameters */
	EMBARC_PRINTF("\r\n-----Set server node with new static ip address-----\r\n");
	uiplib_ipaddrconv(LOCALHOST, &ip);
	uip_sethostaddr(&ip);
	ip_addr_dump("ip:", &ip);
	uiplib_ipaddrconv(NETMASK, &ip);
	uip_setnetmask(&ip);
	ip_addr_dump("netmask:", &ip);
	uiplib_ipaddrconv(NET_GATEWAY, &ip);
	uip_setdraddr(&ip);
	ip_addr_dump("gateway:", &ip);
	EMBARC_PRINTF("\r\n\r\n");

	/* dtls initialization */
	dtls_init();
	init_dtls();

	if (!dtls_context) {
		EMBARC_PRINTF("cannot create context\r\n");
		PROCESS_EXIT();
	}

	dtls_event = process_alloc_event();

	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
		dtls_handle_read(dtls_context);
	}

	PROCESS_END();
}

/** @} */