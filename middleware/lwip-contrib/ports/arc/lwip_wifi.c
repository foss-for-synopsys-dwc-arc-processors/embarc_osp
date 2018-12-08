/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
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

/**
 * \defgroup	MID_LWIP_CONTRIB_PMWIFI		LWIP PMOD WIFI Interface
 * \ingroup	MID_LWIP_CONTRIB
 * \brief	lwip wifi interface
 * @{
 *
 * \file
 * \brief	lwip wifi interface
 */
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include "lwip/opt.h"

#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/autoip.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/tcpip.h"

#include "lwip/ip_addr.h"

#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"

#include "lwip/sys.h"

#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "dev_wnic.h"
#include "arc_exception.h"

#include "netif/slipif.h"
#include "lwip_wifi.h"

#define DBG_LESS
#define DEBUG
#include "embARC_debug.h"

#include "board.h"

#define LWIP_WIFI_CHECK_EXP(EXPR, ERROR_CODE)        CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

/** MRF24W WIFI and SLIP (esp) WIFI Module Network Interface Add & Enable */
static struct netif wifi_netif_local;
static WIFI_IF     wifi_if;
static int32_t wifi_isup = 0;

static void lwip_wifi_status_callback(struct netif *netif);

#define mac_addr_dump(macaddr)                                              \
                LWIP_DEBUGF(LWIP_DBG_ON, ("MAC ADDRESS:%02x-%02x-%02x-%02x-%02x-%02x\r\n", \
                    ((macaddr != NULL) ? ((uint8_t *)(macaddr))[0] : 0),    \
                    ((macaddr != NULL) ? ((uint8_t *)(macaddr))[1] : 0),    \
                    ((macaddr != NULL) ? ((uint8_t *)(macaddr))[2] : 0),    \
                    ((macaddr != NULL) ? ((uint8_t *)(macaddr))[3] : 0),    \
                    ((macaddr != NULL) ? ((uint8_t *)(macaddr))[4] : 0),    \
                    ((macaddr != NULL) ? ((uint8_t *)(macaddr))[5] : 0)))

/** WNIC ON OPERATIONS */
static int32_t lwip_wifi_on_init_success(DEV_WNIC *wnic_ptr)
{
    int32_t ercd = E_OK;
    WIFI_IF *wifi_if = (WIFI_IF *)wnic_ptr->wnic_info.extra;

    wnic_ptr->set_network_type(wnic_ptr->wnic_info.network_type);
#if WF_ENABLE_MANUAL_SET_MAC
    wnic_ptr->set_macaddr(wifi_if->netif->hwaddr);
#else /* get wifi device mac address */
    wnic_ptr->get_macaddr(wifi_if->netif->hwaddr);
    LWIP_DEBUGF(LWIP_DBG_ON, ("%s ", wnic_ptr->wnic_info.name));
    mac_addr_dump((uint8_t *)(wifi_if->netif->hwaddr));
#endif
    return ercd;
}

static int32_t lwip_wifi_on_init_fail(DEV_WNIC *wnic_ptr)
{
    int32_t ercd = E_OK;

    wnic_ptr->wnic_info.init_status = WNIC_INIT_FAILED;
    wnic_ptr->wnic_info.power_status = WNIC_POWER_OFF;
    wnic_ptr->wnic_info.conn_status = WNIC_NOT_CONNECTED;
    LWIP_DEBUGF(LWIP_DBG_ON, ("\r\n%s Init Failed\r\n", wnic_ptr->wnic_info.name));

    return ercd;
}

static int32_t lwip_wifi_on_connected(DEV_WNIC *wnic_ptr)
{
    static int once_flag = 0;
    WIFI_IF *wifi_if = (WIFI_IF *)wnic_ptr->wnic_info.extra;

    if (!netif_is_link_up(wifi_if->netif)) {
        netif_set_link_up(wifi_if->netif);
        LWIP_DEBUGF(LWIP_DBG_ON, ("Link is UP!\r\n"));
    }
    if (once_flag == 0) {
        once_flag = 1;
#ifdef USE_SLIP
        netif_set_status_callback(wifi_if->netif, lwip_wifi_status_callback);
        lwip_wifi_status_callback(wifi_if->netif);//force call the callback to update status flag now
#else
#if WF_IPADDR_DHCP && !NO_SYS   /* use dhcp to get address */
        netif_set_up(wifi_if->netif);
        netif_set_status_callback(wifi_if->netif, lwip_wifi_status_callback);
        LWIP_DEBUGF(LWIP_DBG_ON, ("\r\nNow start get ip address using DHCP, Please wait about 30s!\r\n"));
        dhcp_start(wifi_if->netif);
#else
        netif_set_status_callback(wifi_if->netif, lwip_wifi_status_callback);
        netif_set_up(wifi_if->netif);
#endif
#endif /* USE_SLIP */

    }

    return E_OK;
}

static int32_t lwip_wifi_on_disconnected(DEV_WNIC *wnic_ptr)
{
    int32_t ercd = E_OK;
    WIFI_IF *wifi_if;

    wifi_if = (WIFI_IF *)wnic_ptr->wnic_info.extra;
    if (netif_is_link_up(wifi_if->netif)) {
        netif_set_link_down(wifi_if->netif);
        LWIP_DEBUGF(LWIP_DBG_ON, ("Link is Down!\r\n"));
    }

    return ercd;
}

static int32_t lwip_wifi_on_mac_updated(DEV_WNIC *wnic_ptr)
{
    int32_t ercd = E_OK;
    struct netif *netif;

    LWIP_DEBUGF(LWIP_DBG_ON, ("\r\nNow update mac address!\r\n"));
    WIFI_IF * wifi_if = (WIFI_IF *)wnic_ptr->wnic_info.extra;
    netif = wifi_if->netif;
    wnic_ptr->get_macaddr(netif->hwaddr);
    LWIP_DEBUGF(LWIP_DBG_ON, ("%s ", wnic_ptr->wnic_info.name));
    mac_addr_dump((uint8_t *)(netif->hwaddr));

    return ercd;
}

static int32_t lwip_wifi_on_scan_finished(DEV_WNIC *wnic_ptr)
{
    int32_t ercd = E_OK;

    LWIP_DEBUGF(LWIP_DBG_ON, ("\r\n%s scan finished, and find %d wireless hotspots\r\n", \
            wnic_ptr->wnic_info.name, wnic_ptr->wnic_info.scan_results));

    return ercd;
}

static int32_t lwip_wifi_on_assert(DEV_WNIC *wnic_ptr)
{
    int32_t ercd = E_OK;

    LWIP_DEBUGF(LWIP_DBG_ON, ("\r\nNow start reset the wifi!\r\n"));

    wnic_ptr->wnic_reset();

    return ercd;
}

static void lwip_wifi_showipinfo(struct netif *netif)
{
#ifndef USE_SLIP
#if LWIP_DNS
    const ip_addr_t *dns_server;
    int i = 0;
#endif
#endif

    LWIP_DEBUGF(LWIP_DBG_ON, ("\r\n-----WIFI IP ADDRESS INFO-----\r\n"));
    LWIP_DEBUGF(LWIP_DBG_ON, (" ipaddr "));
    ip_addr_debug_print(LWIP_DBG_ON, &(netif->ip_addr));
    LWIP_DEBUGF(LWIP_DBG_ON, (" netmask "));
    ip_addr_debug_print(LWIP_DBG_ON, &(netif->netmask));
    LWIP_DEBUGF(LWIP_DBG_ON, (" gw "));
    ip_addr_debug_print(LWIP_DBG_ON, &(netif->gw));


#ifndef USE_SLIP
#if LWIP_DNS
    for (i=0; i<DNS_MAX_SERVERS; i++) {
        dns_server = dns_getserver(i);
        LWIP_DEBUGF(LWIP_DBG_ON, ("\r\n dns server %d :", i));
        ip_addr_debug_print(LWIP_DBG_ON, dns_server);
    }
#endif
#endif

    LWIP_DEBUGF(LWIP_DBG_ON, ("\r\n-----------------------------------\r\n"));
}

static void lwip_wifi_status_callback(struct netif *netif)
{
    static int once_flag = 0;

    if (netif_is_up(netif)) {
        if (once_flag == 0){
            once_flag = 1;
            lwip_wifi_showipinfo(netif);
        }
        wifi_isup = 1;
    }
}

int32_t lwip_wifi_init(void)
{
    int32_t ercd = E_OK;
    struct netif *netif = &wifi_netif_local;
    WIFI_IF *wifi_if_ptr = &wifi_if;
    ip_addr_t addr, netmask, gw;
    DEV_WNIC_PTR wnic_ptr;

    memset(wifi_if_ptr, 0, sizeof(WIFI_IF));
#ifdef USE_SLIP
    char intf_no = BOARD_WIFI_SLIP_UART_ID;
    wnic_ptr = wnic_get_dev(BOARD_SLIPWIFI_0_ID);
#else
    wnic_ptr = wnic_get_dev(BOARD_PMWIFI_0_ID);
#endif /* USE_SLIP */
    wifi_if_ptr->wnic = wnic_ptr;
    if (wnic_ptr == NULL) {
        ercd = E_OBJ;
        goto error_exit;
    }
#ifdef USE_SLIP
    wnic_ptr->wnic_info.extra = (void *)wifi_if_ptr;
#endif /* USE_SLIP */

    LWIP_DEBUGF(LWIP_DBG_ON, ("Start Init LWIP\r\n"));
#if NO_SYS
    lwip_init();
#else
    tcpip_init(NULL, NULL);
#endif

    /** on operations settings */
    WNIC_SET_ON_INIT_SUCCESS(wnic_ptr,   lwip_wifi_on_init_success);
    WNIC_SET_ON_INIT_FAIL(wnic_ptr,      lwip_wifi_on_init_fail);
    WNIC_SET_ON_CONNECTED(wnic_ptr,      lwip_wifi_on_connected);
    WNIC_SET_ON_MAC_UPDATED(wnic_ptr,    lwip_wifi_on_mac_updated);
    WNIC_SET_ON_SCAN_FINISHED(wnic_ptr,  lwip_wifi_on_scan_finished);
    WNIC_SET_ON_DEV_ASSERTED(wnic_ptr,   lwip_wifi_on_assert);
#ifdef USE_SLIP
    WNIC_SET_ON_DISCONNECTED(wnic_ptr,   NULL);
    WNIC_SET_ON_RXDATA_COMES(wnic_ptr,   NULL);
#elif defined WIFI_RS9113
    WNIC_SET_ON_DISCONNECTED(wnic_ptr,   lwip_wifi_on_disconnected);
    WNIC_SET_ON_RXDATA_COMES(wnic_ptr,   NULL);
#else
    WNIC_SET_ON_DISCONNECTED(wnic_ptr,   lwip_wifi_on_disconnected);
    WNIC_SET_ON_RXDATA_COMES(wnic_ptr,   pmwifi_on_input);
#endif
    wifi_if_ptr->netif = netif;

  /* get mannual set MAC hardware address */
#if WF_ENABLE_MANUAL_SET_MAC
    wnic_ptr->get_macaddr(netif->hwaddr);
#endif
    IP4_ADDR(&addr,     WF_IPADDR_1,  WF_IPADDR_2,  WF_IPADDR_3,  WF_IPADDR_4  );
    IP4_ADDR(&netmask,  WF_NETMASK_1, WF_NETMASK_2, WF_NETMASK_3, WF_NETMASK_4 );
    IP4_ADDR(&gw,       WF_GATEWAY_1, WF_GATEWAY_2, WF_GATEWAY_3, WF_GATEWAY_4 );

#ifdef USE_SLIP
    LWIP_DEBUGF(LWIP_DBG_ON, ("Start Init SLIP\r\n"));
    netif_add(netif, &addr, &netmask, &gw, (void *)&intf_no, slipif_init, tcpip_input);

    netif_set_default(netif);
    netif_set_link_up(netif);//since we do not have control interface, just force set the link up
    netif_set_up(netif);//also force set netif up
#else
#if NO_SYS
    netif_add(netif, &addr, &netmask, &gw, (void *)wifi_if_ptr, pmwifi_if_init, ethernet_input);
#else
#ifdef WIFI_RS9113
    netif_add(netif, &addr, &netmask, &gw, (void *)wifi_if_ptr, rswifi_if_init, tcpip_input);
#else
    netif_add(netif, &addr, &netmask, &gw, (void *)wifi_if_ptr, pmwifi_if_init, tcpip_input);
#endif /*WIFI_RS9113*/
#endif /*NO_SYS*/
    netif_set_default(netif);

#endif /* USE_SLIP */

error_exit:
    return ercd;
}


int32_t lwip_wifi_isup(void)
{
    return wifi_isup;
}

/** @} end of group MID_LWIP_CONTRIB_PMWIFI */