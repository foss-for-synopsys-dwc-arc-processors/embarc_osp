/************************************************************************/
/*                                                                      */
/*	MRF24GAdaptor.h This implements a Nework adaptor                    */
/*	for the MRF24WG0MA                                                  */
/*                                                                      */
/************************************************************************/
/*	Author: 	Keith Vogel                                             */
/*	Copyright 2013, Digilent Inc.                                       */
/************************************************************************/
/*
*
* Copyright (c) 2013-2014, Digilent <www.digilentinc.com>
* Contact Digilent for the latest version.
*
* This program is free software; distributed under the terms of
* BSD 3-clause license ("Revised BSD License", "New BSD License", or "Modified BSD License")
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1.    Redistributions of source code must retain the above copyright notice, this
*        list of conditions and the following disclaimer.
* 2.    Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
* 3.    Neither the name(s) of the above-listed copyright holder(s) nor the names
*        of its contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/************************************************************************/
/*  Module Description:                                                 */
/*                                                                      */
/*	A WiFi Network adaptor using the Microchip Universal Driver         */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*	5/31/2013(KeithV): Created                                          */
/*                                                                      */
/************************************************************************/
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */

/**
 * \addtogroup	DEVICE_MICROCHIP_MRF24WG	MicroChip MRF24WG Device Driver
 * @{
 *
 * \file
 * \brief	microchip mrf24wg device driver header file based on dev_wnic.h
 *
 */
#ifndef MRF24G_ADAPTOR_H_
#define	MRF24G_ADAPTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dev_wnic.h"

typedef struct MACADDR_T
{
	union
	{
		uint8_t		u8[6];
		uint16_t	u16[3];
	};
} MACADDR;

#define MACBROADCAST	((MACADDR) {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF})	// RFC 894
#define MACNONE			((MACADDR) {0x00, 0x00, 0x00, 0x00, 0x00, 0x00})	// RFC 894

// WPA and WPA2 KEY, used for both
typedef struct
{
	uint8_t rgbKey[32];
} WPA2KEY;

typedef struct
{
	union
	{
		uint8_t		rgbKey[5];
		int8_t		asciiKey[5];
	} key[4];
} WEP40KEY;

typedef struct
{
	union
	{
		uint8_t		rgbKey[13];
		int8_t		asciiKey[13];
	} key[4];
} WEP104KEY;

extern int32_t mrf24g_wnic_get_info(DEV_WNIC *mrf24g_wnic, uint32_t cmd, void *rinfo);
extern int32_t mrf24g_wnic_control(DEV_WNIC *mrf24g_wnic, uint32_t ctrl_cmd, void *param);
extern int32_t mrf24g_wnic_init(DEV_WNIC *mrf24g_wnic, uint32_t network_type);
extern int32_t mrf24g_poll_init_status(DEV_WNIC *mrf24g_wnic);
extern int32_t mrf24g_poll_busy_status(DEV_WNIC *mrf24g_wnic);
extern int32_t mrf24g_set_network_type(DEV_WNIC *mrf24g_wnic, uint32_t type);
extern int32_t mrf24g_set_macaddr(DEV_WNIC *mrf24g_wnic, uint8_t *mac);
extern int32_t mrf24g_get_macaddr(DEV_WNIC *mrf24g_wnic, uint8_t *mac);
extern int32_t mrf24g_start_scan(DEV_WNIC *mrf24g_wnic);
extern int32_t mrf24g_stop_scan(DEV_WNIC *mrf24g_wnic);
extern int32_t mrf24g_poll_scan_status(DEV_WNIC *mrf24g_wnic);
extern int32_t mrf24g_get_scan_result_cnt(DEV_WNIC *mrf24g_wnic);
extern int32_t mrf24g_get_scan_result(DEV_WNIC *mrf24g_wnic, uint32_t index, WNIC_SCAN_RESULT *result);
extern int32_t mrf24g_wnic_connect(DEV_WNIC *mrf24g_wnic, uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key);
extern int32_t mrf24g_poll_conn_status(DEV_WNIC *mrf24g_wnic);
extern int32_t mrf24g_wnic_disconnect(DEV_WNIC *mrf24g_wnic);
extern int32_t mrf24g_prepare_tx(DEV_WNIC *mrf24g_wnic, uint32_t tx_len);
extern int32_t mrf24g_add_tx_data(DEV_WNIC *mrf24g_wnic, uint8_t *p_buf, uint32_t len);
extern int32_t mrf24g_commit_tx(DEV_WNIC *mrf24g_wnic, uint32_t len);
extern int32_t mrf24g_prepare_rx(DEV_WNIC *mrf24g_wnic);
extern int32_t mrf24g_get_rx_data(DEV_WNIC *mrf24g_wnic, uint8_t *p_buf, uint32_t len);
extern int32_t mrf24g_accept_rx(DEV_WNIC *mrf24g_wnic);
extern int32_t mrf24g_config_power_mode(DEV_WNIC *mrf24g_wnic, int32_t power_mode);
extern int32_t mrf24g_poll_power_mode(DEV_WNIC *mrf24g_wnic);
extern int32_t mrf24g_wnic_reset(DEV_WNIC *mrf24g_wnic);
extern void mrf24g_period_process(DEV_WNIC *mrf24g_wnic, void *ptr);

#ifdef __cplusplus
}
#endif

#endif	/* MRF24G_ADAPTOR_H_ */