/************************************************************************/
/*                                                                      */
/*	MRF24GAdaptor.c This implements a Nework adaptor                    */
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
/*	A WiFi Network adaptor using the Microchip Universal Driver     */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*	5/31/2013(KeithV): Created                                      */
/*                                                                      */
/************************************************************************/
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */

/**
 * \defgroup	DEVICE_MICROCHIP_MRF24WG	MicroChip MRF24WG Device Driver
 * \ingroup	DEVICE_MICROCHIP
 * \brief	microchip mrf24wg device driver implementation based on dev_wnic.h
 * @{
 *
 * \file	mrf24g_adapter.c
 * \brief	microchip mrf24wg device driver implementation based on dev_wnic.h
 *
 */
#include "string.h"
#include "dev_wnic.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "mrf24g_adapter.h"
#include "./driver/inc/shared/wf_universal_driver.h"

#define MRF24G_CHECK_EXP(EXPR, ERROR_CODE)	CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

int32_t mrf24g_wnic_get_info(DEV_WNIC *mrf24g_wnic, uint32_t cmd, void *rinfo)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

error_exit:
	return ercd;
}

int32_t mrf24g_wnic_control(DEV_WNIC *mrf24g_wnic, uint32_t ctrl_cmd, void *param)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

error_exit:
	return ercd;
}

int32_t mrf24g_wnic_init(DEV_WNIC *mrf24g_wnic, uint32_t network_type)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_NOT_INITIALIZED) \
				|| (mrf24g_info->init_status==WNIC_INIT_FAILED), E_OPNED);

	MRF24G_CHECK_EXP((network_type==WNIC_NETWORK_TYPE_INFRASTRUCTURE) \
				|| (network_type==WNIC_NETWORK_TYPE_ADHOC), E_PAR);

	mrf24g_info->init_status = WNIC_DURING_INITIALIZATION;  /* start initialization process */
	mrf24g_info->network_type = network_type; /* set network type used next */

	WF_Init();
	mrf24g_info->power_status = WNIC_POWER_NORMAL;

error_exit:
	return ercd;
}

int32_t mrf24g_poll_init_status(DEV_WNIC *mrf24g_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);

	ercd = mrf24g_info->init_status;

error_exit:
	return ercd;
}

int32_t mrf24g_poll_busy_status(DEV_WNIC *mrf24g_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	ercd = mrf24g_info->busy_status;

error_exit:
	return ercd;
}

int32_t mrf24g_set_network_type(DEV_WNIC *mrf24g_wnic, uint32_t type)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	MRF24G_CHECK_EXP((type==WNIC_NETWORK_TYPE_INFRASTRUCTURE) \
				|| (type==WNIC_NETWORK_TYPE_ADHOC), E_PAR);

	WF_NetworkTypeSet(type);
	mrf24g_info->network_type = type;

error_exit:
	return ercd;
}

int32_t mrf24g_set_macaddr(DEV_WNIC *mrf24g_wnic, uint8_t *mac)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;
	DEV_WNIC_ON_OPS *mrf24g_on_ops;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	MRF24G_CHECK_EXP((mac!=NULL), E_PAR);

	WF_MacAddressSet((uint8_t *) mac);
	memcpy(mrf24g_info->mac_addr, mac, WNIC_HDR_LEN);
	mrf24g_info->mac_status = WNIC_MAC_UPDATED;

	mrf24g_on_ops = &(mrf24g_wnic->wnic_on_ops);
	if (mrf24g_on_ops->on_mac_updated != NULL) {
		mrf24g_on_ops->on_mac_updated(mrf24g_wnic);
	}

error_exit:
	return ercd;
}

int32_t mrf24g_get_macaddr(DEV_WNIC *mrf24g_wnic, uint8_t *mac)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	MRF24G_CHECK_EXP((mac!=NULL), E_PAR);

	if (mrf24g_info->mac_status == WNIC_MAC_NOT_UPDATED) {
		WF_MacAddressGet(mrf24g_info->mac_addr);
		mrf24g_info->mac_status = WNIC_MAC_UPDATED;
	}

	memcpy(mac, mrf24g_info->mac_addr, WNIC_HDR_LEN);
error_exit:
	return ercd;
}

int32_t mrf24g_start_scan(DEV_WNIC *mrf24g_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	MRF24G_CHECK_EXP((mrf24g_info->scan_status!=WNIC_DURING_SCAN), E_CLS);
	MRF24G_CHECK_EXP((mrf24g_info->conn_status==WNIC_NOT_CONNECTED) \
				|| (mrf24g_info->conn_status==WNIC_CONNECTED), E_CLS);
	MRF24G_CHECK_EXP((mrf24g_info->busy_status==WNIC_IS_FREE), E_CLS);

	mrf24g_info->scan_status = WNIC_DURING_SCAN;
	mrf24g_info->busy_status = WNIC_IS_BUSY;
	WF_Scan(WF_SCAN_ALL);

error_exit:
	return ercd;
}

int32_t mrf24g_stop_scan(DEV_WNIC *mrf24g_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	mrf24g_info->scan_status = WNIC_NOT_SCANED;
	mrf24g_info->busy_status = WNIC_IS_FREE;

error_exit:
	return ercd;
}

int32_t mrf24g_poll_scan_status(DEV_WNIC *mrf24g_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	ercd = mrf24g_info->scan_status;

error_exit:
	return ercd;
}

int32_t mrf24g_get_scan_result_cnt(DEV_WNIC *mrf24g_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	if (mrf24g_info->scan_status == WNIC_SCAN_FINISHED) {
		ercd = mrf24g_info->scan_results;
	} else {
		ercd = 0;
	}

error_exit:
	return ercd;
}

static t_scanResult scan_result_temp;
static t_scanResult *p_scan_result_temp = &scan_result_temp;

int32_t mrf24g_get_scan_result(DEV_WNIC *mrf24g_wnic, uint32_t index, WNIC_SCAN_RESULT *result)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	MRF24G_CHECK_EXP(result!=NULL, E_PAR);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	MRF24G_CHECK_EXP((mrf24g_info->scan_status==WNIC_SCAN_FINISHED), E_CLS);
	MRF24G_CHECK_EXP((index < mrf24g_info->scan_results), E_PAR);

	WF_ScanResultGet(index, p_scan_result_temp);

	memcpy(result->bssid, p_scan_result_temp->bssid, WNIC_BSSID_MAX_LEN);
	memcpy(result->ssid, p_scan_result_temp->ssid, WNIC_SSID_MAX_LEN);
	result->ssidlen = p_scan_result_temp->ssidLen;
	result->ssid[result->ssidlen] = '\0'; /* make end of ssid */
	result->ap_config.apConfig = p_scan_result_temp->apConfig;
	result->rssi = p_scan_result_temp->rssi;
	result->bsstype = p_scan_result_temp->bssType;
	result->channel = p_scan_result_temp->channel;

error_exit:
	return ercd;
}

static t_wpaKeyInfo wpaKeyInfoG;
int32_t mrf24g_wnic_connect(DEV_WNIC *mrf24g_wnic, uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;
	int32_t ssid_len;
	int32_t fPICKeyCalc = 0;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	MRF24G_CHECK_EXP((ssid != NULL), E_PAR);
	MRF24G_CHECK_EXP(!((key == NULL) && (security != AUTH_SECURITY_OPEN)), E_PAR);

	ssid_len = strlen((const char *)ssid);
	MRF24G_CHECK_EXP((ssid_len <= WNIC_SSID_MAX_LEN), E_PAR); /* check ssid len */

	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	MRF24G_CHECK_EXP((mrf24g_info->busy_status==WNIC_IS_FREE), E_CLS);
	MRF24G_CHECK_EXP((mrf24g_info->conn_status==WNIC_NOT_CONNECTED) \
				|| (mrf24g_info->conn_status==WNIC_CONNECTED), E_CLS);

	if (mrf24g_info->conn_status == WNIC_CONNECTED) {
		/* if connected to a different ssid, just disconnect it */
		if (strcmp((const char *)ssid, (const char *)(mrf24g_info->ssid.ssid)) == 0) {
			ercd = E_OK;
			goto error_exit;
		} else {
			if (mrf24g_wnic_disconnect(mrf24g_wnic) != E_OK) { /* if disconnect failed just return failed */
				ercd = E_SYS;
			}
		}
	} else {
		mrf24g_wnic_disconnect(mrf24g_wnic);
	}
	mrf24g_info->busy_status = WNIC_IS_BUSY;

	mrf24g_info->conn_status = WNIC_DURING_CONNECTION;

	WF_SsidSet((uint8_t *)ssid, ssid_len);
	memcpy(mrf24g_info->ssid.ssid, ssid, ssid_len);
	mrf24g_info->ssid.ssid_len = ssid_len;

	t_wepContext wep;
	t_wpaContext wpa;
	t_wpsContext wps;

	switch (security) {
		case AUTH_SECURITY_OPEN:
			WF_SecurityOpenSet();
			break;

		case AUTH_SECURITY_WEP_40:
			wep.wepSecurityType = security;	// WF_SECURITY_WEP_40 or WF_SECURITY_WEP_104
			wep.wepKeyIndex = key->key_idx;	// 0 thru 3
			wep.wepKeyLength = sizeof(WEP40KEY);	// number of bytes pointed to by p_wepKey
			memcpy(wep.wepKey, key->key, sizeof(WEP40KEY));	 // array containing WEP binary security key (4 5-byte keys for WEP-40, 4 13-byte keys for WEP-104)
			wep.wepKeyType = WF_SECURITY_WEP_OPENKEY;  // WF_SECURITY_WEP_OPENKEY (default) or WF_SECURITY_WEP_SHAREDKEY
			WF_SecurityWepSet(&wep);
			break;

		case AUTH_SECURITY_WEP_104:
			wep.wepSecurityType = security;		// WF_SECURITY_WEP_40 or WF_SECURITY_WEP_104
			wep.wepKeyIndex = key->key_idx;		// 0 thru 3
			wep.wepKeyLength = sizeof(WEP104KEY);	// number of bytes pointed to by p_wepKey
			memcpy(wep.wepKey, key->key, sizeof(WEP104KEY));	// array containing WEP binary security key (4 5-byte keys for WEP-40, 4 13-byte keys for WEP-104)
			wep.wepKeyType = WF_SECURITY_WEP_OPENKEY;  // WF_SECURITY_WEP_OPENKEY (default) or WF_SECURITY_WEP_SHAREDKEY
			WF_SecurityWepSet(&wep);
			break;

		case AUTH_SECURITY_WPA_WITH_PASS_PHRASE:
		case AUTH_SECURITY_WPA2_WITH_PASS_PHRASE:
		case AUTH_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
			wpa.wpaSecurityType = security;	// WF_SECURITY_WPA_WITH_KEY, WF_SECURITY_WPA_WITH_PASS_PHRASE, WF_SECURITY_WPA2_WITH_KEY, WF_SECURITY_WPA2_WITH_PASS_PHRASE,WF_SECURITY_WPA_AUTO_WITH_KEY, WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE
			wpa.keyInfo.keyLength = strlen((char *)key->key);		// number of bytes in binary key (always 32) or passphrase
			memcpy(wpa.keyInfo.key, key->key, wpa.keyInfo.keyLength);	// binary key or passphrase
			wpa.keyInfo.ssidLen = ssid_len;					// number of bytes in SSID
			memcpy(wpa.keyInfo.ssid, ssid, wpa.keyInfo.ssidLen);		// ssid

#if defined(WF_USE_HOST_WPA_KEY_CALCULATION)
			// if (fPICKeyCalc) {
			//	 // this is very ugly as it holds the PIC
			//	 // for 4 seconds
			//	 WF_WpaConvPassphraseToKey(&wpa.keyInfo);	// not sure how to check for errors
			//	 wpa.wpaSecurityType--;					  // go to the KEY form of the type
			// }
#endif /* WF_USE_HOST_WPA_KEY_CALCULATION */

			WF_SecurityWpaSet(&wpa);
			break;

		case AUTH_SECURITY_WPA_WITH_KEY:
		case AUTH_SECURITY_WPA2_WITH_KEY:
		case AUTH_SECURITY_WPA_AUTO_WITH_KEY:
			wpa.wpaSecurityType = security;				// WF_SECURITY_WPA_WITH_KEY, WF_SECURITY_WPA_WITH_PASS_PHRASE, WF_SECURITY_WPA2_WITH_KEY, WF_SECURITY_WPA2_WITH_PASS_PHRASE,WF_SECURITY_WPA_AUTO_WITH_KEY, WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE
			wpa.keyInfo.keyLength = sizeof(WPA2KEY);		// number of bytes in binary key (always 32) or passphrase
			memcpy(wpa.keyInfo.key, key->key, sizeof(WPA2KEY));	// binary key or passphrase
			wpa.keyInfo.ssidLen = ssid_len;				// number of bytes in SSID
			memcpy(wpa.keyInfo.ssid, ssid, wpa.keyInfo.ssidLen);	// ssid
			WF_SecurityWpaSet(&wpa);
			break;

		case AUTH_SECURITY_WPS_PUSH_BUTTON:
			wps.wpsPinLength = WF_WPS_PIN_LENGTH;			// should always be 8
			memcpy(wps.wpsPin, key->key, WF_WPS_PIN_LENGTH);	// if using WF_SECURITY_WPS_PIN then pointer to 8-digit pin
			// fall thru

		case AUTH_SECURITY_WPS_PIN:
			wps.wpsSecurityType = security;				// WF_SECURITY_WPS_PUSH_BUTTON or WF_SECURITY_WPS_PIN
#if defined(WF_USE_HOST_WPA_KEY_CALCULATION)
			wps.getPassPhrase = fPICKeyCalc;			// calculate key in PIC32(true) or on MRF (false)
			memset(&wpaKeyInfoG, 0, sizeof(t_wpaKeyInfo));
			wps.p_keyInfo = &wpaKeyInfoG;				// pointer to where the Universal driver will store passphrase info (must be global memory)
#endif /* WF_USE_HOST_WPA_KEY_CALCULATION */
			WF_SecurityWpsSet(&wps);
			break;

		default:
			mrf24g_info->conn_status = WNIC_NOT_CONNECTED;
			mrf24g_info->busy_status = WNIC_IS_FREE;
			ercd = E_PAR;
			goto error_exit;
			break;
	}
	WF_Connect();

error_exit:
	return ercd;
}

int32_t mrf24g_poll_conn_status(DEV_WNIC *mrf24g_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;
	uint8_t connection_status;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	if (mrf24g_info->busy_status == WNIC_IS_BUSY) {
		ercd = mrf24g_info->conn_status;
		goto error_exit;
	}
	if (mrf24g_info->conn_status == WNIC_CONNECTED) {
		ercd = WNIC_CONNECTED;
		goto error_exit;
	}
	WF_ConnectionStateGet(&connection_status);
	switch (connection_status) {
		// normal case at dropping the AP
		case WF_CSTATE_CONNECTED_INFRASTRUCTURE:
			mrf24g_info->conn_type = WNIC_CONN_INFRASTRUCTURE;
			ercd = WNIC_CONNECTED;
			break;
		case WF_CSTATE_CONNECTED_ADHOC:
			mrf24g_info->conn_type = WNIC_CONN_ADHOC;
			ercd = WNIC_CONNECTED;
			break;

		// this is an ugly case, we could be in the middle of trying
		// to reconnect and we MUST NOT disconnect or we could
		// put the firmware into a locked up state
		// so instead just say we are busy and when the connect attempts
		// the connect will wait until it is done being busy
		// however this should never happen as we set the busy state when
		// we entered either of these states in the event stub
		case WF_CSTATE_CONNECTION_IN_PROGRESS:
			ercd = WNIC_CONN_TEMP_LOST;
			break;
		case WF_CSTATE_RECONNECTION_IN_PROGRESS:
			ercd = WNIC_DURING_CONNECTION;
			break;

		case WF_CSTATE_NOT_CONNECTED:
		case WF_CSTATE_CONNECTION_PERMANENTLY_LOST:
			ercd = WNIC_NOT_CONNECTED;
			break;

		default:
			ercd = mrf24g_info->conn_status;
			break;
	}
	mrf24g_info->conn_status = ercd;

error_exit:
	return ercd;
}

int32_t mrf24g_wnic_disconnect(DEV_WNIC *mrf24g_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;
	DEV_WNIC_ON_OPS *mrf24g_on_ops;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);
	MRF24G_CHECK_EXP((mrf24g_info->conn_status!=WNIC_DURING_DISCONNECTION), E_CLS);

	if (mrf24g_poll_conn_status(mrf24g_wnic) == WNIC_CONNECTED) {
		mrf24g_info->conn_status = WNIC_DURING_DISCONNECTION;
		WF_Disconnect();
		mrf24g_info->conn_status = WNIC_NOT_CONNECTED;
		mrf24g_on_ops = &(mrf24g_wnic->wnic_on_ops);
		if (mrf24g_on_ops->on_disconnected != NULL) {
			mrf24g_on_ops->on_disconnected(mrf24g_wnic);
		}
	}

error_exit:
	return ercd;
}

int32_t mrf24g_prepare_tx(DEV_WNIC *mrf24g_wnic, uint32_t tx_len)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	if (WF_TxPacketAllocate(tx_len) == false) {
		ercd = E_OBJ;
	}

error_exit:
	return ercd;
}

int32_t mrf24g_add_tx_data(DEV_WNIC *mrf24g_wnic, uint8_t *p_buf, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	MRF24G_CHECK_EXP(p_buf!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	WF_TxPacketCopy(p_buf, len);
	ercd = len;

error_exit:
	return ercd;
}

int32_t mrf24g_commit_tx(DEV_WNIC *mrf24g_wnic, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	WF_TxPacketTransmit(len);

error_exit:
	return ercd;
}

int32_t mrf24g_prepare_rx(DEV_WNIC *mrf24g_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	ercd = (int32_t)WF_RxPacketLengthGet();

error_exit:
	return ercd;
}

int32_t mrf24g_get_rx_data(DEV_WNIC *mrf24g_wnic, uint8_t *p_buf, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	MRF24G_CHECK_EXP(p_buf!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	WF_RxPacketCopy(p_buf, len);

error_exit:
	return ercd;
}

int32_t mrf24g_accept_rx(DEV_WNIC *mrf24g_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	WF_RxPacketDeallocate();

error_exit:
	return ercd;
}

int32_t mrf24g_config_power_mode(DEV_WNIC *mrf24g_wnic, int32_t power_mode)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

error_exit:
	return ercd;
}

int32_t mrf24g_poll_power_mode(DEV_WNIC *mrf24g_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	ercd = mrf24g_info->power_status;
error_exit:
	return ercd;
}

int32_t mrf24g_wnic_reset(DEV_WNIC *mrf24g_wnic)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->init_status==WNIC_INIT_SUCCESSFUL), E_CLSED);

	mrf24g_info->init_status = WNIC_NOT_INITIALIZED;
	mrf24g_info->power_status = WNIC_POWER_OFF;
	ercd = mrf24g_wnic_init(mrf24g_wnic, mrf24g_info->network_type);

error_exit:
	return ercd;
}

void mrf24g_period_process(DEV_WNIC *mrf24g_wnic, void *ptr)
{
	int32_t ercd = E_OK;
	DEV_WNIC_INFO *mrf24g_info;

	MRF24G_CHECK_EXP(mrf24g_wnic!=NULL, E_OBJ);
	mrf24g_info = &(mrf24g_wnic->wnic_info);
	MRF24G_CHECK_EXP((mrf24g_info->power_status==WNIC_POWER_NORMAL), E_CLSED);

	WF_Task(mrf24g_wnic, ptr);
	//WF_EintEnable();

error_exit:
	return;
}
/** @} end of group DEVICE_MICROCHIP_MRF24WG */