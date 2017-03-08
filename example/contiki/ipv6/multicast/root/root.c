/*
 * Copyright (c) 2010, Loughborough University - Computer Science
 * All rights reserved.
 *
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
 */

/**
 * \defgroup	EMBARC_APP_CONTIKI_IPV6_MULTICAST_ROOT	embARC Contiki IPv6 Multicast Example-Root Node
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_CONTIKI
 * \brief	embARC Example for Multicast on Contiki (Root Node)
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     * Digilent PMOD RF2(MRF24J40)
 *
 * ### Design Concept
 *     This node is part of the RPL multicast example. It is an RPL root
 *     and sends a multicast message periodically. For the example to work,
 *     we need one of those nodes. The concept of this example is shown below
 *     ![Concept of contiki ipv6 multicast](pic/images/example/emsk/emsk_contiki_ipv6_multicast.jpg)
 *
 *
 * ### Usage Manual
 *     * Purpose: This example is based on 802.15.4, using Contiki + uIP (IPv6) + RPL + 6lowPAN + multicast. A minimum of two nodes are required.
 *     * Hardware:
 *         - Root node: Pmod RF to J6 on \ref EMBARC_BOARD_CONNECTION "EMSK"
 *         - Sink node: Pmod RF to J6 on \ref EMBARC_BOARD_CONNECTION "EMSK"
 *         - Intermediate node (if exists): Pmod RF to J6 \ref EMBARC_BOARD_CONNECTION "EMSK"
 *     * NOTE:
 *         Users can use the bootloader to load .bin file from the SD card to save JTAG ports,
 *         but multiple terminals are required to see the output of nodes
 *     * Run Steps:
 *         - Load and run the applications in root node, sink node, and intermediate nodes (if they exist)
 *         - The nodes first set up a tree-like network using the RPL protocol.
 *         - After one minute, the root node starts to send multicast messages to a specific multicast group
 *         - The sink node joins the specific multicast group and receives the multicast messages
 *         - The intermediate node relays the multicast message
 *     ![Screeshot of contiki ipv6 multicast example](pic/images/example/emsk/emsk_contiki_ipv6_multicast1.jpg)
 *
 * ### Extra Comments
 *
 */


/**
 * \file
 * \ingroup	EMBARC_APP_CONTIKI_IPV6_MULTICAST_ROOT
 * \author
 *         George Oikonomou - <oikonomou@users.sourceforge.net>
 */

/**
 * \addtogroup	EMBARC_APP_CONTIKI_IPV6_MULTICAST_ROOT
 * @{
 */

#include "embARC.h"
#include "embARC_debug.h"

#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ipv6/multicast/uip-mcast6.h"

#include <string.h>

#include "net/ip/uip-debug.h"
#include "net/rpl/rpl.h"
#include "init-net.h"

#define MAX_PAYLOAD_LEN 120
#define MCAST_SINK_UDP_PORT 3001 /* Host byte order */
#define SEND_INTERVAL CLOCK_SECOND /* clock ticks */
#define ITERATIONS 100 /* messages */

/* Start sending messages START_DELAY secs after we start so that routing can
 * converge */
#define START_DELAY 60

static struct uip_udp_conn * mcast_conn;
static char buf[MAX_PAYLOAD_LEN];
static uint32_t seq_id;

#if !NETSTACK_CONF_WITH_IPV6 || !UIP_CONF_ROUTER || !UIP_CONF_IPV6_MULTICAST || !UIP_CONF_IPV6_RPL
#error "This example can not work with the current contiki configuration"
#error "Check the values of: NETSTACK_CONF_WITH_IPV6, UIP_CONF_ROUTER, UIP_CONF_IPV6_RPL"
#endif
/*---------------------------------------------------------------------------*/
PROCESS(rpl_root_process, "RPL ROOT, Multicast Sender");
AUTOSTART_PROCESSES(&rpl_root_process);
/*---------------------------------------------------------------------------*/
static void multicast_send(void)
{
	uint32_t id;

	id = uip_htonl(seq_id);
	memset(buf, 0, MAX_PAYLOAD_LEN);
	memcpy(buf, &id, sizeof(seq_id));

	EMBARC_PRINTF("Send to: ");
	PRINT6ADDR(&mcast_conn->ripaddr);
	EMBARC_PRINTF(" Remote Port %u,", uip_ntohs(mcast_conn->rport));
	EMBARC_PRINTF(" (msg=0x%08lx)", (unsigned long)uip_ntohl(*((uint32_t *)buf)));
	EMBARC_PRINTF(" %lu bytes\r\n", (unsigned long)sizeof(id));

	seq_id++;
	uip_udp_packet_send(mcast_conn, buf, sizeof(id));
}
/*---------------------------------------------------------------------------*/
static void prepare_mcast(void)
{
	uip_ipaddr_t ipaddr;

	/*
	* IPHC will use stateless multicast compression for this destination
	* (M=1, DAC=0), with 32 inline bits (1E 89 AB CD)
	*/
	uip_ip6addr(&ipaddr, 0xFF1E,0,0,0,0,0,0x89,0xABCD);
	mcast_conn = udp_new(&ipaddr, UIP_HTONS(MCAST_SINK_UDP_PORT), NULL);
}
/*---------------------------------------------------------------------------*/
static void set_own_addresses(void)
{
	int i;
	uint8_t state;
	rpl_dag_t *dag;
	uip_ipaddr_t ipaddr;

	uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
	uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
	uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

	EMBARC_PRINTF("Our IPv6 addresses:\r\n");
	for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
		state = uip_ds6_if.addr_list[i].state;
		if(uip_ds6_if.addr_list[i].isused && (state == ADDR_TENTATIVE || state
			== ADDR_PREFERRED)) {
			EMBARC_PRINTF("  ");
			PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
			EMBARC_PRINTF("\r\n");
			if(state == ADDR_TENTATIVE) {
				uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;
			}
		}
	}

	/* Become root of a new DODAG with ID our global v6 address */
	dag = rpl_set_root(RPL_DEFAULT_INSTANCE, &ipaddr);
	if(dag != NULL) {
		rpl_set_prefix(dag, &ipaddr, 64);
		EMBARC_PRINTF("Created a new RPL dag with ID: ");
		PRINT6ADDR(&dag->dag_id);
		EMBARC_PRINTF("\r\n");
	}
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(rpl_root_process, ev, data)
{
	static struct etimer et;

	PROCESS_BEGIN();

	EMBARC_PRINTF("------IPv6 multcast demo (root node)------\r\n");
	EMBARC_PRINTF("Multicast Engine: '%s'\r\n", UIP_MCAST6.name);

	init_net(EMSK_ID);
	NETSTACK_MAC.off(1);

	set_own_addresses();

	prepare_mcast();

	etimer_set(&et, START_DELAY * CLOCK_SECOND);
	while(1) {
		PROCESS_YIELD();
		if(etimer_expired(&et)) {
			multicast_send();
			etimer_set(&et, SEND_INTERVAL);
		}
	}

	PROCESS_END();
}
/** @} */