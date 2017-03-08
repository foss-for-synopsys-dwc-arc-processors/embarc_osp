/*
 * Copyright (c) 2014, Swedish Institute of Computer Science.
 *
 * Copyright (c) 2017, Synopsys, Inc.
 * Modified for port to ARC processor by Wayne Ren wei.ren@synopsys.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in the
 *	documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *	may be used to endorse or promote products derived from this software
 *	without specific prior written permission.
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
 */

/**
 * \file mrf24g_drv.c
 * \brief mrf24g wifi driver for contiki(ipv4 and ip64)
 */

#include "contiki-net.h"
#include "net/ip/uip.h"
#include "net/ip/uipopt.h"

#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "mrf24g_drv.h"
#include "dev_wnic.h"


#include "board.h"
#define DEBUG DEBUG_PRINT
#include "embARC_debug.h"
#include "net/ip/uip-debug.h"


#define BUF	((struct uip_eth_hdr *)&uip_buf[0])
#define IPBUF	((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_CHECK_EXP(EXPR, ERROR_CODE)		CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)


PROCESS(mrf24g_process, "mrf24g driver");


process_event_t wnic_event;

static int32_t network_read(DEV_WNIC *wnic_ptr);
static int32_t network_init(uint32_t network_type);
static int32_t network_send(uint8_t * buf, uint32_t len);


static DEV_WNIC *pmwifi_wnic_ptr;
static int network_up_flag = 0;

/** On Operations */
static int32_t uip_pmwifi_on_init_success(DEV_WNIC *wnic_ptr)
{
	int32_t ercd = E_OK;
	uip_lladdr_t *mac = &uip_lladdr;

#if WF_ENABLE_MANUAL_SET_MAC
	/* mac address set */
	wnic_ptr->set_macaddr((uint8_t *)mac);
#else /* get pmod wifi device mac address */
	wnic_ptr->get_macaddr((uint8_t *)mac);
#endif
	DBG("mac address: ");
	PRINTLLADDR(mac);
	DBG("\r\n");

	return ercd;
}

static int32_t uip_pmwifi_on_init_fail(DEV_WNIC *wnic_ptr)
{
	int32_t ercd = E_OK;

	wnic_ptr->wnic_info.init_status = WNIC_INIT_FAILED;
	wnic_ptr->wnic_info.power_status = WNIC_POWER_OFF;
	wnic_ptr->wnic_info.conn_status = WNIC_NOT_CONNECTED;
	network_up_flag = 0;
	DBG("\r\n%s Init Failed\r\n", wnic_ptr->wnic_info.name);

	return ercd;
}

static int32_t uip_pmwifi_on_mac_updated(DEV_WNIC *wnic_ptr)
{
	int32_t ercd = E_OK;
	uip_lladdr_t *mac = &uip_lladdr;

	DBG("\r\nNow update mac address!\r\n");

	wnic_ptr->get_macaddr((uint8_t *)mac);

	DBG("mac address: ");
	PRINTLLADDR(mac);
	DBG("\r\n");

	return ercd;
}

static int32_t uip_pmwifi_on_scan_finished(DEV_WNIC *wnic_ptr)
{
	int32_t ercd = E_OK;

	DBG("\r\n%s scan finished, and find %d wireless hotspots\r\n", \
		wnic_ptr->wnic_info.name, wnic_ptr->wnic_info.scan_results);
	process_post(PROCESS_BROADCAST, wnic_event, wnic_ptr);

	return ercd;
}

static int32_t uip_pmwifi_on_assert(DEV_WNIC *wnic_ptr)
{
	int32_t ercd = E_OK;

	DBG("\r\nNow start reset the pmod wifi!\r\n");

	network_up_flag = 0;
	wnic_ptr->wnic_info.init_status = WNIC_NOT_INITIALIZED;
	wnic_ptr->wnic_reset();

	return ercd;
}

static int32_t uip_pmwifi_on_connected(DEV_WNIC *wnic_ptr)
{
	int32_t ercd = E_OK;
	static int once = 0;
#if !NETSTACK_CONF_WITH_IPV6
	uip_ipaddr_t ipaddr;
#endif

	network_up_flag = 1;
	if (once == 0) {
		once = 1;
#if !NETSTACK_CONF_WITH_IPV6
		DBG("Manually Set IP Info As Follows:\r\n");
		DBG("Notice: If dhcp enabled, static ip address will be replaced by dhcp.\r\n");
		uip_ipaddr(&ipaddr, WF_IPADDR_1,  WF_IPADDR_2,  WF_IPADDR_3,  WF_IPADDR_4);
		uip_sethostaddr(&ipaddr);
		DBG("host: ");
		PRINT6ADDR(&ipaddr);
		uip_ipaddr(&ipaddr, WF_GATEWAY_1, WF_GATEWAY_2, WF_GATEWAY_3, WF_GATEWAY_4);
		uip_setdraddr(&ipaddr);
		DBG("\r\ngateway: ");
		PRINT6ADDR(&ipaddr);
		uip_ipaddr(&ipaddr, WF_NETMASK_1, WF_NETMASK_2, WF_NETMASK_3, WF_NETMASK_4);
		uip_setnetmask(&ipaddr);
		DBG("\r\nnetmask: ");
		PRINT6ADDR(&ipaddr);
		DBG("\r\n");
#endif
		process_post(PROCESS_BROADCAST, wnic_event, wnic_ptr);
	}


	return ercd;
}

static int32_t uip_pmwifi_on_disconnected(DEV_WNIC *wnic_ptr)
{
	int32_t ercd = E_OK;

	network_up_flag = 0;
	process_post(PROCESS_BROADCAST, wnic_event, wnic_ptr);
	return ercd;
}

static int32_t network_send(uint8_t * buf, uint32_t len)
{
	static int snd_pending = 0;
	int32_t ercd = E_OK;

	snd_pending ++;
	if ( (snd_pending > 1) || (get_network_up_status() == 0) ) {
		snd_pending --;
		ercd = E_SYS;
		goto error_exit;
	}
	if (pmwifi_wnic_ptr->prepare_tx(len) == E_OK) {
		pmwifi_wnic_ptr->add_tx_data(buf, len);
		pmwifi_wnic_ptr->commit_tx(len);
	} else {
		ercd = E_OBJ;
	}
	snd_pending --;

error_exit:
	return ercd;
}

static int32_t network_init(uint32_t network_type)
{
	int32_t ercd = E_OK;
#if WF_ENABLE_MANUAL_SET
	uip_lladdr_t mac;
#endif

	  /** on operations settings */
	WNIC_SET_ON_INIT_SUCCESS(pmwifi_wnic_ptr,	uip_pmwifi_on_init_success);
	WNIC_SET_ON_INIT_FAIL(pmwifi_wnic_ptr,		uip_pmwifi_on_init_fail);
	WNIC_SET_ON_CONNECTED(pmwifi_wnic_ptr,		uip_pmwifi_on_connected);
	WNIC_SET_ON_DISCONNECTED(pmwifi_wnic_ptr,	uip_pmwifi_on_disconnected);
	WNIC_SET_ON_MAC_UPDATED(pmwifi_wnic_ptr,	uip_pmwifi_on_mac_updated);
	WNIC_SET_ON_SCAN_FINISHED(pmwifi_wnic_ptr,	uip_pmwifi_on_scan_finished);
	WNIC_SET_ON_RXDATA_COMES(pmwifi_wnic_ptr,	network_read);
	WNIC_SET_ON_DEV_ASSERTED(pmwifi_wnic_ptr,	uip_pmwifi_on_assert);

	ercd = pmwifi_wnic_ptr->wnic_init(network_type);

#if WF_ENABLE_MANUAL_SET_MAC
	/* manual set mac address */
	pmwifi_wnic_ptr->get_macaddr((uint8_t *)&mac);
	uip_setethaddr(mac);
#endif

	return ercd;
}

#if !UIP_CONF_IPV6_IP64
static int32_t network_read(DEV_WNIC *wnic_ptr)
{
	int32_t ercd = E_OK;
	uint32_t rx_avail = 0;

	rx_avail = wnic_ptr->prepare_rx();
	if (rx_avail <= 0) {
		wnic_ptr->accept_rx();
		ercd = E_OBJ;
		goto error_exit;
	}

	if (rx_avail > UIP_BUFSIZE) {
		rx_avail = UIP_BUFSIZE;
		DBG("Income More Data Than Buffer Size!\r\n");
	}
	wnic_ptr->get_rx_data((uint8_t *)uip_buf, rx_avail);
	wnic_ptr->accept_rx();   /** free rx buffer */

	uip_len = rx_avail;

	if (BUF->type == uip_htons(UIP_ETHTYPE_IP)) {
		uip_len -= sizeof(struct uip_eth_hdr);
		tcpip_input();
	} else if(BUF->type == uip_htons(UIP_ETHTYPE_ARP)) {
		uip_arp_arpin();
		/* If the above function invocation resulted in data that
		   should be sent out on the network, the global variable
		   uip_len is set to a value > 0. */
		if (uip_len > 0) {
			network_send((uint8_t *)uip_buf, uip_len);
		}
	}

error_exit:
	return ercd;
}
#else /* IP64 is enabled when IPv6 is enabled */
#include "ip64.h"
static int32_t network_read(DEV_WNIC *wnic_ptr)
{
	int32_t ercd = E_OK;
	uint32_t rx_avail = 0;

	rx_avail = wnic_ptr->prepare_rx();
	if (rx_avail <= 0) {
		wnic_ptr->accept_rx();
		ercd = E_OBJ;
		goto error_exit;
	}

	if (rx_avail > UIP_BUFSIZE) {
		rx_avail = UIP_BUFSIZE;
		DBG("Income More Data Than Buffer Size!\r\n");
	}
	wnic_ptr->get_rx_data((uint8_t *)ip64_packet_buffer, rx_avail);
	wnic_ptr->accept_rx();   /** free rx buffer */

	IP64_INPUT(ip64_packet_buffer, rx_avail);
error_exit:
	return ercd;
}
#endif

#if !UIP_CONF_IPV6_IP64
uint8_t mrf24g_output(void)
{

	uip_arp_out();
	network_send((uint8_t *)uip_buf, uip_len);

	return 0;
}
#endif

int get_network_up_status(void)
{
	return network_up_flag;
}

PROCESS_THREAD(mrf24g_process, ev, data)
{
	static struct etimer et;
	static WNIC_AUTH_KEY auth_key;
	PROCESS_BEGIN();

#if WF_HOTSPOT_IS_OPEN
	auth_key.key = NULL;
	auth_key.key_len = 0;
	auth_key.key_idx = 0;
#else
	auth_key.key = (const uint8_t *)WF_HOTSPOT_PASSWD;
	auth_key.key_len = sizeof(WF_HOTSPOT_PASSWD);
	auth_key.key_idx = 0;
#endif

	pmwifi_wnic_ptr = wnic_get_dev(BOARD_PMWIFI_0_ID);

	if (network_init(WNIC_NETWORK_TYPE_INFRASTRUCTURE) != E_OK) {
		DBG("network init error\r\n");
		PROCESS_EXIT();
	}
	wnic_event = process_alloc_event();
#if !NETSTACK_CONF_WITH_IPV6
	tcpip_set_outputfunc(mrf24g_output);
#endif
	DBG("uIP Start Init\r\n");

	while (1) {
		etimer_set(&et, 10);	/* period: 100 ms */
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		pmwifi_wnic_ptr->period_process(NULL);

#if WF_HOTSPOT_IS_OPEN
		pmwifi_wnic_ptr->wnic_connect(AUTH_SECURITY_OPEN, (const uint8_t *)WF_HOTSPOT_NAME, &auth_key);
#else
		pmwifi_wnic_ptr->wnic_connect(AUTH_SECURITY_WPA_AUTO_WITH_PASS_PHRASE,
			(const uint8_t *)WF_HOTSPOT_NAME, &auth_key);
#endif
	}

	PROCESS_END();
}

#if UIP_CONF_IPV6_IP64
static void mrf24g_ip64_init(void)
{

}

static int mrf24g_ip64_output(uint8_t *packet, uint16_t len)
{
	if (network_send(packet, len) != E_OK) {
		return 0;
	}

	return len;
}

const struct ip64_driver mrf24g_ip64_driver = {
	mrf24g_ip64_init,
	mrf24g_ip64_output
};
#endif