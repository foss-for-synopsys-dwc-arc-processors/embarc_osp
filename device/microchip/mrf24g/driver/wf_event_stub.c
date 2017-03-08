/* MRF24WG0M Universal Driver
*
* Copyright (c) 2012-2013, Microchip <www.microchip.com>
* Contact Microchip for the latest version.
*
* This program is free software; distributed under the terms of BSD
* license:
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
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */

#include "./inc/shared/wf_universal_driver.h"
#include "./inc/internal/wf_global_includes.h"

#include "dev_wnic.h"
#include "string.h"

#define DBG_LESS
#define DEBUG
#include "embARC_debug.h"

static void OutputMrf24wgAssertInfo(uint32_t eventData);

/*****************************************************************************
  Function:
    void WF_ProcessEvent(uint8_t eventType, uint32_t eventData);

  Summary:
    Called by Universal Driver to inform application of MRF24WG events.

  Description:
    Various events are reported to the application via this function callback.
    The application should take appropriate action based on the event.

    Example 1) After WF_Init() is called, no other WiFI functions can be called,
               other than calling WF_Task(), until the WF_EVENT_INITIALIZATION
               occurs with eventData = WF_INIT_SUCCESSFUL.

    Example 2) If the WiFi connection is temporarily lost the application should
               not be sending messages until the WiFi connection is restored.
               If DHCP is being used, then, after the connection is restored, a
               new DHCP request should be initiated.

 Parameters:
    eventType -- type of event
    eventData -- data associated with event; not always used.

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_ProcessEvent(DEV_WNIC *mrf24g_wnic, uint8_t eventType, uint32_t eventData)
{
    DEV_WNIC_ON_OPS *mrf24g_on_ops;
    DEV_WNIC_INFO *mrf24g_info;

    mrf24g_on_ops = &(mrf24g_wnic->wnic_on_ops);
    mrf24g_info = &(mrf24g_wnic->wnic_info);
    mrf24g_info->busy_status = WNIC_IS_FREE;

    switch (eventType)
    {
        case WF_EVENT_INITIALIZATION:
            if (eventData == WF_INIT_SUCCESSFUL) {
                dbg_printf(DBG_LESS_INFO, "WF INIT SUCCESSFULL!\n");
                mrf24g_info->init_status = WNIC_INIT_SUCCESSFUL;
                if (mrf24g_on_ops->on_init_success != NULL) {
                    mrf24g_on_ops->on_init_success(mrf24g_wnic);
                }
            } else {
                dbg_printf(DBG_LESS_INFO, "WF INIT FAILED!\n");
                mrf24g_info->init_status = WNIC_INIT_FAILED;
                if (mrf24g_on_ops->on_init_fail != NULL) {
                    mrf24g_on_ops->on_init_fail(mrf24g_wnic);
                }
            }
            break;

        case WF_EVENT_CONNECTION_SUCCESSFUL:
            dbg_printf(DBG_LESS_INFO, "WF_EVENT_CONNECTION_SUCCESSFUL\n");
            mrf24g_info->conn_status = WNIC_CONNECTED;
            if (mrf24g_on_ops->on_connected != NULL) {
                mrf24g_on_ops->on_connected(mrf24g_wnic);
            }
            break;

        case WF_EVENT_CONNECTION_TEMPORARILY_LOST:
            dbg_printf(DBG_LESS_INFO, "WF_EVENT_CONNECTION_TEMPORARILY_LOST\n");
            // don't do anything during the reconnect!
            mrf24g_info->conn_status = WNIC_CONN_TEMP_LOST;
            mrf24g_info->busy_status = WNIC_IS_BUSY;
             // don't do anything during the reconnect!
            break;

        case WF_EVENT_CONNECTION_REESTABLISHED:
            dbg_printf(DBG_LESS_INFO, "WF_EVENT_CONNECTION_REESTABLISHED\n");
            mrf24g_info->conn_status = WNIC_CONNECTED;
            break;

        case WF_EVENT_CONNECTION_PERMANENTLY_LOST:
            dbg_printf(DBG_LESS_INFO, "WF_EVENT_CONNECTION_PERMANENTLY_LOST\n");
            mrf24g_info->conn_status = WNIC_NOT_CONNECTED;
            if (mrf24g_on_ops->on_disconnected != NULL) {
                mrf24g_on_ops->on_disconnected(mrf24g_wnic);
            }
            break;

        case WF_EVENT_CONNECTION_FAILED:
            dbg_printf(DBG_LESS_INFO, "WF_EVENT_CONNECTION_FAILED\n");
            mrf24g_info->conn_status = WNIC_NOT_CONNECTED;
            if (mrf24g_on_ops->on_disconnected != NULL) {
                mrf24g_on_ops->on_disconnected(mrf24g_wnic);
            }
            break;

        case WF_EVENT_SCAN_RESULTS_READY:
            if (mrf24g_info->scan_status == WNIC_DURING_SCAN) {
                dbg_printf(DBG_LESS_INFO, "WF_EVENT_SCAN_RESULTS_READY\n");
                mrf24g_info->scan_status = WNIC_SCAN_FINISHED;
                mrf24g_info->scan_results = eventData;
                if (mrf24g_on_ops->on_scan_finished != NULL) {
                    mrf24g_on_ops->on_scan_finished(mrf24g_wnic);
                }
            } else {
                dbg_printf(DBG_LESS_INFO, "WF SCAN EVENT IGNORED\n");
            }
            break;

#if defined(WF_USE_HOST_WPA_KEY_CALCULATION)
        case WF_WPS_EVENT_KEY_CALCULATION_REQUEST:
            dbg_printf(DBG_LESS_INFO, "WF_WPS_EVENT_KEY_CALCULATION_REQUEST\n");
            WF_WpsKeyGenerate(); // can be called here or later, but must be called
                                 // to complete WPS connection
            // wait for connection status or error.
            mrf24g_info->busy_status = WNIC_IS_BUSY;
            break;
#endif /* WF_USE_HOST_WPA_KEY_CALCULATION */

        case WF_EVENT_MRF24WG_MODULE_ASSERT:
            dbg_printf(DBG_LESS_INFO, "WF_EVENT_MRF24WG_MODULE_ASSERT\n");
            mrf24g_info->init_status = WNIC_INIT_FAILED;
            if (mrf24g_on_ops->on_dev_asserted != NULL) {
                mrf24g_on_ops->on_dev_asserted(mrf24g_wnic);
            }
            break;

        // if we get an event error, it will be in
        // the last eventData
        case WF_EVENT_ERROR:
        default:
            dbg_printf(DBG_MORE_INFO, "WF_EVENT_ERROR:%d\n", eventData);
            break;
    }
}

// called from WiFi_Task
void  RxPacketCheck(DEV_WNIC *mrf24g_wnic)
{
    if (isPacketRx()) {
        ClearPacketRx();
        WF_ProcessRxPacket(mrf24g_wnic);
    }
}

/*****************************************************************************
  Function:
    void WF_ProcessRxPacket(void);

  Summary:
    Called by Universal Driver to notify application of incoming packet.

  Description:
    None
 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_ProcessRxPacket(DEV_WNIC *mrf24g_wnic)
{
    DEV_WNIC_ON_OPS *mrf24g_on_ops;
    DEV_WNIC_INFO *mrf24g_info;

    mrf24g_on_ops = &(mrf24g_wnic->wnic_on_ops);
    mrf24g_info = &(mrf24g_wnic->wnic_info);

    if (mrf24g_info->init_status == WNIC_INIT_SUCCESSFUL) {
        if (mrf24g_on_ops->on_rxdata_comes != NULL) {
            mrf24g_on_ops->on_rxdata_comes(mrf24g_wnic);
        }
    }
}

static void OutputMrf24wgAssertInfo(uint32_t assertInfo)
{
    while(true);
}