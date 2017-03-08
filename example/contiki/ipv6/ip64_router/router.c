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
 * \defgroup	EMBARC_APP_CONTIKI_IPV6_IP64_ROUTER	embARC Contiki IPv6-IPv4 Border Router
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_CONTIKI
 * \brief	embARC Example for IPv6-IPv4 Board Router
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     * Digilent PMOD WIFI(MRF24WG0MA)
 *     * Digilent PMOD RF2(MRF24J40)
 *
 * ### Design Concept
 * 	This node works as a border router between 802.15.4 based IPv6 network and Wi-Fi based IPv4 network.
 *  	In IPv6 network, the data packets whose destination is not in IPv6 network are forwarded to IPv4 network.
 *  	Meanwhile, one entry is set up in the NAT table of router, like:
 *  		IPv6 address: IPv6 port : IPv4 address ：IPv4 port : map port (start from 1024).
 *      The data packets from IPv4 netwrok are forwarder to IPv6 netwokr according to the NAT table.
 *
 *
 * ### Usage Manual
 *     * Purpose:
 *     * Hardware:
 *         - Pmod WiFi to J5, Pmod RF to J6 on \ref EMBARC_BOARD_CONNECTION "EMSK"
 *     * NOTE:
 *     * Run Steps:
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_CONTIKI_IPV6_IP64_ROUTER
 */

/**
 * \addtogroup	EMBARC_APP_CONTIKI_IPV6_IP64_ROUTER
 * @{
 */

#include "embARC.h"

#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ipv6/multicast/uip-mcast6.h"
#include "net/ip64/ip64.h"
#include "net/ip64/ip64-dhcpc.h"
#include "net/ip/ip64-addr.h"

#include <string.h>

#include "embARC_debug.h"
#include "net/ip/uip-debug.h"
#include "net/rpl/rpl.h"
#include "init-net.h"

#define ipv4_addr_dump(msg, ipaddr) \
		EMBARC_PRINTF("%s:%d.%d.%d.%d  ",  (msg),			\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[0] : 0),	\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[1] : 0),	\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[2] : 0),	\
			((ipaddr != NULL) ? ((uint8_t *)(ipaddr))[3] : 0))

#if !NETSTACK_CONF_WITH_IPV6 || !UIP_CONF_ROUTER || !UIP_CONF_IPV6_MULTICAST || !UIP_CONF_IPV6_RPL
#error "This example can not work with the current contiki configuration"
#error "Check the values of: NETSTACK_CONF_WITH_IPV6, UIP_CONF_ROUTER, UIP_CONF_IPV6_RPL"
#endif

PROCESS(ip64_router_process, "ip64 border router");
AUTOSTART_PROCESSES(&ip64_router_process);

static void
set_own_addresses(void)
{
	rpl_dag_t *dag;
	uip_ipaddr_t ipaddr;

	uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
	uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
	uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

	/* Become root of a new DODAG with ID our global v6 address */
	dag = rpl_set_root(RPL_DEFAULT_INSTANCE, &ipaddr);
	if(dag != NULL) {
		rpl_set_prefix(dag, &ipaddr, 64);
		EMBARC_PRINTF("Created a new RPL dag with ID: ");
		PRINT6ADDR(&dag->dag_id);
		EMBARC_PRINTF("\r\n");
	}
}

uip_ipaddr_t uip_hostaddr;

PROCESS_THREAD(ip64_router_process, ev, data)
{
	DEV_WNIC_PTR wifi;
	PROCESS_BEGIN();

	EMBARC_PRINTF("------IPv6-IPv4 border router ------\r\n");
	EMBARC_PRINTF("Multicast Engine: '%s'\r\n", UIP_MCAST6.name);

	process_start(&mrf24g_process, NULL); // init mrf24g driver

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

	ip64_init();
	init_net(EMSK_ID);
	set_own_addresses();
	NETSTACK_MAC.off(1);

	EMBARC_PRINTF("Now start get ipv4 address using DHCP, Please wait about 30s!\r\n");

	ip64_dhcpc_init(uip_lladdr.addr, sizeof(uip_lladdr.addr));
	ip64_dhcpc_request();

	while(1) {
		PROCESS_WAIT_EVENT();
		if (ev == PROCESS_EVENT_EXIT) {
			PROCESS_EXIT();
		}
		ip64_dhcpc_appcall(ev, data);
	}

	PROCESS_END();
}


void ip64_dhcpc_configured(const struct ip64_dhcpc_state *s)
{
	uip_ip6addr_t ip6dnsaddr;

	ip64_set_hostaddr((uip_ip4addr_t *)&s->ipaddr);
  	ip64_set_netmask((uip_ip4addr_t *)&s->netmask);
  	ip64_set_draddr((uip_ip4addr_t *)&s->default_router);
  	ip64_addr_4to6((uip_ip4addr_t *)&s->dnsaddr, &ip6dnsaddr);

	ipv4_addr_dump("ip", &s->ipaddr);
	ipv4_addr_dump("netmask", &s->netmask);
	ipv4_addr_dump("gateway", &s->default_router);
	ipv4_addr_dump("dns", &s->dnsaddr);
	EMBARC_PRINTF("\r\n");
	/* continue to init */

}

void ip64_dhcpc_unconfigured(const struct ip64_dhcpc_state *s)
{
	EMBARC_PRINTF("DHCP dhcpc_unconfigured\r\n");
}
/** @} */