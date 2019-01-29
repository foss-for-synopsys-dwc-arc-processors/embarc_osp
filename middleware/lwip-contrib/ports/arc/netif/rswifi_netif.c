/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "wifi_if" to replace it with
 * something that better describes your network interface.
 */
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/debug.h"
#include "lwip/ip_addr.h"
#include "lwip/dhcp.h"

#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"

#include "lwip/sys.h"

#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "dev_wnic.h"
#include "arc_exception.h"

#include "rswifi_netif.h"
WIFI_IF_PTR rswifi_if_ptr_gb = NULL;
/**
 * In this function, the hardware should be initialized.
 * Called from rswifi_if_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this wifi_if
 */
static err_t
low_level_init(struct netif *netif)
{
	WIFI_IF_PTR rswifi_if_ptr;

	rswifi_if_ptr = (WIFI_IF_PTR)(netif->state);

	if (rswifi_if_ptr == NULL) {
		return ERR_MEM;
	}

#if NO_SYS
	rswifi_if_ptr->on_output_cnt = 0;
	rswifi_if_ptr->on_input_cnt = 0;
#else
	sys_sem_new(&(rswifi_if_ptr->output_sem), 1);
	sys_sem_new(&(rswifi_if_ptr->input_sem), 1);
#endif

	/** init hareware */
	rswifi_if_ptr->wnic->wnic_init(WNIC_NETWORK_TYPE_INFRASTRUCTURE);
	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* maximum transfer unit */
	netif->mtu = 1500;

	/* device capabilities */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

	return ERR_OK;
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this wifi_if
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
#define TX_MAX_ERR_CNT		50
static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
	struct pbuf *q;
	uint32_t total_len;
	err_t ercd = ERR_OK;
	WIFI_IF_PTR rswifi_if_ptr;
	DEV_WNIC *rswifi_wnic_ptr;
	static uint32_t tx_err_cnt = 0;

	rswifi_if_ptr = (WIFI_IF_PTR)(netif->state);

	if (rswifi_if_ptr == NULL) {
		return ERR_IF;
	}
	if (!rswifi_if_ptr->ready) { /* not ready */
		return ERR_IF;
	}
	rswifi_wnic_ptr = rswifi_if_ptr->wnic;

	if (rswifi_wnic_ptr->poll_conn_status() != WNIC_CONNECTED) { /** no link, can't send any data */
		LWIP_DEBUGF(NETIF_DEBUG, ("%s is not linked, not ready to send ip packet\r\n" ,\
			rswifi_wnic_ptr->wnic_info.name));
		return ERR_CONN;
	}

#if ETH_PAD_SIZE
	pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

	total_len = p->tot_len; /** get the total length of pbuf need to be sent */
	LWIP_DEBUGF(NETIF_DEBUG, ("%s try send data\r\n", rswifi_wnic_ptr->wnic_info.name));

#if NO_SYS
	rswifi_if_ptr->cpu_status = cpu_lock_save();
	rswifi_if_ptr->on_output_cnt ++;
	cpu_unlock_restore(rswifi_if_ptr->cpu_status);
	/* wait here for other process to output data */
	while (rswifi_if_ptr->on_output_cnt > 1); /*wait*/
	/* enter send */
	if (1) {
#else
	if (sys_arch_sem_wait(&(rswifi_if_ptr->output_sem), 0) != SYS_ARCH_TIMEOUT) {
#endif
		//LWIP_DEBUGF(LWIP_DBG_ON, ("NETIF OUTPUT %d, ", total_len));
		if (rswifi_wnic_ptr->prepare_tx(total_len) == E_OK) {
			LWIP_DEBUGF(NETIF_DEBUG, ("rswifi_if_output, len %d\r\n", total_len));
			for(q = p; q != NULL; q = q->next) {
				/* Send the data from the pbuf to the interface, one pbuf at a
				   time. The size of the data in each pbuf is kept in the ->len
				   variable. */
				rswifi_wnic_ptr->add_tx_data((uint8_t *)q->payload, q->len);
			}
			rswifi_wnic_ptr->commit_tx(total_len);
			LWIP_DEBUGF(NETIF_DEBUG, ("rswifi_if data send successfully\r\n"));
			tx_err_cnt = 0;
		} else {
			if (tx_err_cnt++ > TX_MAX_ERR_CNT) {
				LWIP_DEBUGF(LWIP_DBG_ON, ("Redpine WIFI TX Buffer Full, send failed, if not recovered, you need to restart board.\r\n"));
			}
			ercd = ERR_IF;
			LWIP_DEBUGF(NETIF_DEBUG, ("rswifi_if_output, failed\r\n"));
		}
		//LWIP_DEBUGF(LWIP_DBG_ON, ("NETIF OUTPUT DONE %d\n", ercd));
#if NO_SYS
		rswifi_if_ptr->cpu_status = cpu_lock_save();
		rswifi_if_ptr->on_output_cnt --;
		cpu_unlock_restore(rswifi_if_ptr->cpu_status);
#else
		sys_sem_signal(&(rswifi_if_ptr->output_sem));
#endif
	}

#if ETH_PAD_SIZE
	pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

	LINK_STATS_INC(link.xmit);

	return ercd;
}

err_t rswifi_on_input(uint16_t status, uint8_t *data, const uint32_t length)
{
	LWIP_DEBUGF(NETIF_DEBUG, ("rswifi_on_input\r\n"));
	err_t ercd=ERR_OK;
	struct pbuf *p, *frame;
	struct eth_hdr *ethhdr;
	struct netif *netif;
	WIFI_IF_PTR rswifi_if_ptr;
	rswifi_if_ptr = rswifi_if_ptr_gb;
	if(rswifi_if_ptr == NULL){
		return ERR_ARG;
	}
	netif = rswifi_if_ptr->netif;
	if ( (frame = pbuf_alloc(PBUF_RAW, length, PBUF_POOL)) == NULL) {
		return ERR_MEM; /** memory out of use */
	}

	/** read in rx data packet */
	uint32_t index=0;
	for (p=frame; p!=NULL; p=p->next) {
		//TODO
		pbuf_take(p, data+index, p->len);
		index += p->len;
		//wnic_ptr->get_rx_data((uint8_t *)p->payload, p->len);
	}
	p = frame; /** reset p to frame */
	//wnic_ptr->accept_rx();   /** free rx buffer */

	ethhdr = (struct eth_hdr *)(p->payload);

	switch (htons(ethhdr->type)) {
		/* IP or ARP packet? */
		case ETHTYPE_IP:
		case ETHTYPE_IPV6:
		case ETHTYPE_ARP:
#if PPPOE_SUPPORT
		/* PPPoE packet? */
		case ETHTYPE_PPPOEDISC:
		case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
		/* full packet send to tcpip_thread to process */
		LWIP_DEBUGF(NETIF_DEBUG, ("rswifi_if_input: IP input %d data\r\n", length));
		// LWIP_DEBUGF(LWIP_DBG_ON, ("NETIF INPUT %d, ", len));
		if ((ercd = netif->input(p, netif)) != ERR_OK) {
			LWIP_DEBUGF(NETIF_DEBUG, ("rswifi_if_input: IP input error:%d\r\n", ercd));
			pbuf_free(p);
			p = NULL;
		}
		// LWIP_DEBUGF(LWIP_DBG_ON, ("NETIF DONE %d\r\n", ercd));
		break;

		default:
			pbuf_free(p);
			p = NULL;
			break;
	}
	return ercd;
}
/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this wifi_if
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
rswifi_if_init(struct netif *netif)
{
	WIFI_IF_PTR rswifi_if_ptr;

	LWIP_ASSERT("netif != NULL", (netif != NULL));
	LWIP_ASSERT("netif->state != NULL", (netif->state != NULL));

	rswifi_if_ptr = (WIFI_IF_PTR)(netif->state);
	rswifi_if_ptr_gb = rswifi_if_ptr;

	if ((rswifi_if_ptr == NULL) || (rswifi_if_ptr->wnic == NULL)) {
		LWIP_DEBUGF(NETIF_DEBUG, ("rswifi_if_init: unable to point to rswifi interface\r\n"));
		return ERR_MEM;
	}
	rswifi_if_ptr->ready = false;
	rswifi_if_ptr->wnic->wnic_info.extra = (void *)rswifi_if_ptr;
	rswifi_if_ptr->wnic->wnic_info.ctrl = (void *)rswifi_on_input;

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = (char *)(rswifi_if_ptr->wnic->wnic_info.name);
#endif /* LWIP_NETIF_HOSTNAME */

	/*
	* Initialize the snmp variables and counters inside the struct netif.
	* The last argument should be replaced with your link speed, in units
	* of bits per second.
	*/
	NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

	netif->name[0] = rswifi_if_ptr->wnic->wnic_info.ifname[0];
	netif->name[1] = rswifi_if_ptr->wnic->wnic_info.ifname[1];
	/* We directly use etharp_output() here to save a function call.
	* You can instead declare your own function an call etharp_output()
	* from it if you have to do some checks before sending (e.g. if link
	* is available...) */
	netif->output = etharp_output;
#if LWIP_IPV6
	netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
	netif->linkoutput = low_level_output;

	rswifi_if_ptr->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

	/* initialize the hardware */
	if (low_level_init(netif) != ERR_OK) {
		return ERR_MEM;
	}

	rswifi_if_ptr->netif = netif;
	rswifi_if_ptr->ready = true;

	return ERR_OK;
}