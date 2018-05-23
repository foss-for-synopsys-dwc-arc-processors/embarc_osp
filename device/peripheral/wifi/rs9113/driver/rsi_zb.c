/**
 * @file     rsi_zigb.c
 * @version  0.1
 * @date     2015-Aug-31
 *
 * Copyright(C) Redpine Signals 2012
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief
 * Definitions of all ZigBee Frame Descriptors
 *
 * @section Description
 * This file contains definition of all frame descriptors used in ZigBee.
 * These definition are used to construct frames.
 *
 *
 */

/**
 * Includes
 */
#ifdef RSI_ZB_ENABLE
#include "rsi_driver.h"

rsi_zigb_app_info_t rsi_zigb_app_info;
uint16_t rsi_zigb_get_nvm_payload_len(uint16_t offset);
/*==============================================*/
/**
 * @fn          int32_t rsi_driver_send_cmd(rsi_zigb_cmd_request_t cmd, rsi_pkt_t *pkt)
 * @brief       This functions fills commands and places into zigb TX queue
 * @param[in]   cmd, type of the command to send
 * @param[in]   pkt, pointer of packet to send
 * @param[out]
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This functions fills commands and places into zigb TX queue
 *
 *
 */
int32_t rsi_zigb_send_cmd(uint8_t *cmd, rsi_pkt_t *pkt)
{
	uint16_t payload_size = 0;
	int32_t  status       = RSI_SUCCESS;

	//! Get host descriptor pointer
	uint8_t *host_desc = (pkt->desc);

	//! Memset host descriptor
	memset(host_desc, 0, RSI_HOST_DESC_LENGTH);

  //! Save expected response type
  rsi_driver_cb->zigb_cb->expected_response = cmd[1];

  // Based on the command id,update the payload length
  switch(cmd[1])
	{
		case ZIGBEESTACKINIT:
		case ZIGBEESTACKRESET:
		case ZIGBEELEAVENETWORK:
		case ZIGBEESTOPSCAN:
		case ZIGBEENETWORKSTATE:
		case ZIGBEESTACKISUP:
		case ZIGBEEGETSELFIEEEADDRESS:
    case ZIGBEEGETDEVICETYPE:
    case ZIGBEEENDDEVICEPOLLFORDATA:
    case ZIGBEEGETSELFSHORTADDRESS:
    case ZIGBEEGETOPERATINGCHANNEL:
    case ZIGBEEGETSHORTPANID:
    case ZIGBEEGETEXTENDEDPANID:
    case ZIGBEESTACKPROFILE:
    case ZIGBEETREEDEPTH:
    case ZIGBEEGETNEIGHBORTABLEENTRYCOUNT:
    case ZIGBEEREADCOUNTOFCHILDDEVICES:
    case ZIGBEEREADCOUNTOFROUTERCHILDDEVICES:
    case ZIGBEEGETPARENTSHORTADDRESS:
    case ZIGBEEGETPARENTIEEEADDRESS:
    case ZDPSENDDEVICEANNOUNCEMENT:
    case ZIGBEEGETMAXAPSPAYLOADLENGTH:
    case ZIGBEEGETBINDINGINDICES:
    case ZIGBEECLEARBINDINGTABLE:
    case ZIGBEEGETNWKMAXDEPTH:
    case ZIGBEEGETSECURITYLVL:
    case ZIGBEEGETTCSHORTADDR:
    case ZIGBEEGETMAXAPSLEN:
    case ZIGBEEGETNWKKEY:
    case ZIGBEEREADSAS:
    case ZIGBEESETSTACKEVENT:
    case ZIGBEECLRSTACKEVENT:
#ifdef ZB_MAC_API
	case ZIGBEEGETCCASTATUS:
#endif
      {
        payload_size = 0;
	  }
	  break;

    case ZIGBEEFORMNETWORK:
      {
        payload_size = sizeof(formNetworkFrameSnd);
        rsi_driver_cb->zigb_cb->expected_response = APPZIGBEESTACKSTATUSHANDLER;
      }
      break;

    case ZIGBEEJOINNETWORK:
      {
        payload_size = sizeof(joinNetworkFrameSnd);
        rsi_driver_cb->zigb_cb->expected_response = APPZIGBEESTACKSTATUSHANDLER;
      }
      break;

    case ZIGBEEPROFILETESTENABLE:
      {
	payload_size = sizeof(setprofiletestFrameSnd);
      }
      break;

    case ZIGBEEPERMITJOIN:
      {
        payload_size = sizeof(permitJoinFrameSnd);
      }
      break;

    case ZIGBEEINITIATESCAN:
      {
        payload_size = sizeof(initScanFrameSnd);
        rsi_driver_cb->zigb_cb->expected_response = APPSCANCOMPLETEHANDLER;
      }
      break;

    case ZIGBEEINITPS :
      {
        payload_size = sizeof(pwrModeFrameSnd);
      }
      break;

    case ZIGBEEINITIATEENERGYSCANREQUEST:
      {
        payload_size = sizeof(initEnergyScanFrameSnd);
        rsi_driver_cb->zigb_cb->expected_response = APPENERGYSCANRESULTHANDLER;
      }
      break;


    case ZDPSENDNWKADDRREQUEST:
      {
        payload_size = sizeof(getZDPNWKShortAddrFrameSnd);
      }
      break;

    case ZIGBEEBROADCASTNWKMANAGERREQUEST:
      {
        payload_size = sizeof(bcastNWKManagerFrameSnd);
      }
      break;

    case ZDPSENDIEEEADDRREQUEST:
      {
        payload_size = sizeof(getZDPIEEEAddrFrameSnd);
      }
      break;

    case ZIGBEEUPDATESAS:
      {
        payload_size = sizeof(Startup_Attribute_Set_t);
      }
      break;
    case ZIGBEEUPDATEALLZDO:
      {
        payload_size = sizeof(ZDO_Configuration_Table_t);
      }
      break;

	 case ZIGBEEZDOUPDATESAS:
      {
        payload_size = 102;

      }
      break;
    case ZIGBEEUPDATEZDO:
      {
        payload_size = sizeof(ZDO_Configuration_Table_t);
      }
      break;

    case ZIGBEEISITSELFIEEEADDRESS:
      {
        payload_size = sizeof(isitSelfIEEEFrameSnd);
      }
      break;

    case ZIGBEESETMANUFACTURERCODEFORNODEDESC:
      {
        payload_size = sizeof(setManufFrameSnd);
      }
      break;

    case ZIGBEESETPOWERDESCRIPTOR:
      {
        payload_size = sizeof(setPowerDescFrameSnd);
      }
      break;

    case ZIGBEESETMAXMINCOMINGTXFRSIZE:
      {
        payload_size = sizeof(setMaxIncomingTxfrFrameSnd);
      }
      break;

    case ZIGBEESETSIMPLEDESCRIPTOR:
      {
        payload_size = sizeof(setSimpleDescFrameSnd);
      }
      break;

    case ZIGBEESETOPERATINGCHANNEL:
      {
        payload_size = sizeof(setOperChanFrameSnd);
      }
      break;

    case ZDPSENDMATCHDESCRIPTORSREQUEST:
      {
        payload_size = sizeof(sendMatchDescFrameSnd);
      }
      break;

    case ZIGBEESENDUNICASTDATA:
      {
        payload_size = sizeof(unicastDataFrameSnd);
      }
      break;

    case ZIGBEEGETENDPOINTID:
      {
        payload_size = sizeof(getEndPointIdFrameSnd);
      }
      break;


    case ZIGBEEGETSIMPLEDESCRIPTOR:
      {
        payload_size = sizeof(getSimpleDescFrameSnd);
      }
      break;

    case ZIGBEEGETENDPOINTCLUSTER:
      {
        payload_size = sizeof(getEPClusterFrameSnd);
      }
      break;

    case ZIGBEEGETSHORTADDRFORSPECIFIEDIEEEADDR:
      {
        payload_size = sizeof(getShortAddrForIeeeAddrFrameSnd);
      }
      break;


    case ZIGBEEGETIEEEADDRFORSPECIFIEDSHORTADDR:
      {
        payload_size = sizeof(getIeeeAddrForShortAddrFrameSnd);
      }
      break;

    case ZIGBEEREADNEIGHBORTABLEENTRY:
      {
        payload_size = sizeof(readNeighborTableFrameSnd);
      }
      break;

    case ZIGBEEGETROUTETABLEENTRY:
      {
        payload_size = sizeof(getRouteTableFrameSnd);
      }
      break;

    case ZIGBEEGETCHILDSHORTADDRESSFORTHEINDEX:
      {
        payload_size = sizeof(getChildShortAddrFrameSnd);
      }
      break;


    case ZIGBEEGETCHILDINDEXFORSPECIFIEDSHORTADDR:
      {
        payload_size = sizeof(getChildIndexForShortAddrFrameSnd);
      }
      break;


    case ZIGBEEGETCHILDDETAILS:
      {
        payload_size = sizeof(getChildDetailsFrameSnd);
      }
      break;

    case ZIGBEEACTIVEENDPOINTSREQUEST:
      {
        payload_size = sizeof(activeEPOfShortAddrFrameSnd);
      }
      break;

    case ZDPSENDPOWERDESCRIPTORREQUEST:
      {
        payload_size = sizeof(powerDescFrameSnd);
      }
      break;

    case ZDPSENDNODEDESCRIPTORREQUEST:
      {
        payload_size = sizeof(nodeDescFrameSnd);
      }
      break;

    case ZIGBEESENDGROUPDATA:
      {
        payload_size = sizeof(groupDataFrameSnd);
      }
      break;

    case ZIGBEESENDBROADCASTDATA:
      {
        payload_size = sizeof(bcastDataFrameSnd);
      }
      break;


    case ZIGBEESETBINDINGENTRY:
      {
        payload_size = sizeof(setBindEntryFrameSnd);
      }
      break;

    case ZIGBEEDELETEBINDING:
      {
        payload_size = sizeof(delBindEntryFrameSnd);
      }
      break;

    case ZIGBEEISBINDINGENTRYACTIVE:
      {
        payload_size = sizeof(isBindEntryActiveFrameSnd);
      }
      break;


    case ZIGBEEBINDREQUEST:
      {
        payload_size = sizeof(bindReqFrameSnd);
      }
      break;

    case ZIGBEEENDDEVICEBINDREQUEST:
      {
        payload_size = sizeof(endDevBindFrameSnd);
      }
      break;

    case ZIGBEEUNBINDREQUEST:
      {
        payload_size = sizeof(unbindReqFrameSnd);
      }
      break;

    case ZIGBEESIMPLEDESCRIPTORREQUEST:
    case ZIGBEEVERFYGRPEP:
      {
        payload_size = sizeof(getSimpleDescOfShortAddrFrameSnd);
      }
      break;

    case ZIGBEEGETADDRESSMAPTABLEENTRY:
      {
        payload_size = sizeof(getAddrMapTableFrameSnd);
      }
      break;

    case ZIGBEEGETKEY:
    case ZIGBEEGETTCLINKKEY:
      {
        payload_size = sizeof(getKeyFrameSnd);
      }
      break;

    case ZIGBEEREQUESTLINKKEY:
      {
        payload_size = sizeof(reqLinkKeyFrameSnd);
      }
      break;

    case ZIGBEEGETKEYTABLEENTRY:
      {
        payload_size = sizeof(getKeyTableFrameSnd);
      }
      break;


    case ZIGBEESETKEYTABLEENTRY:
      {
        payload_size = sizeof(setKeyTableFrameSnd);
      }
      break;

    case ZIGBEEADDORUPDATEKEYTABLEENTRY:
      {
        payload_size = sizeof(addKeyTableFrameSnd);
      }
      break;


    case ZIGBEEFINDKEYTABLEENTRY:
      {
        payload_size = sizeof(findKeyTableFrameSnd);
      }
      break;

    case ZIGBEEAPSASDUHANDLER:
      {
	payload_size = sizeof(sethandlerFrameSnd);
      }
      break;

    case ZIGBEEERASEKEYTABLEENTRY:
      {
        payload_size = sizeof(eraseKeyTableFrameSnd);
      }
      break;

    case ZIGBEESETPROFILE:
    case ZIGBEEREMALLGRP:
    case ZIGBEEGETGRPID:
      {
        payload_size = sizeof(setProfileFrameSnd);
      }
      break;

    case ZIGBEESEARCHADDR:
      {
        payload_size = sizeof(setSearchApsmeAddr);
      }
      break;

    case ZIGBEEAPSREMGRP:
    case ZIGBEEAPSADDGRP:
      {
        payload_size = sizeof(Remove_Group_Request_t);
      }
      break;

    case ZIGBEEGETROUTERECORDFORDEV:
      {
        payload_size = sizeof(getRouteRecordForDeviceFrameSnd);
      }

    case ZIGBEEFINDNETWORKANDPERFORMREJOIN:
      {
        payload_size = sizeof(findNWKRejoinFrameSnd);
      }
      break;

    case ZIGBEEREJOINNETWORK:
      {
        payload_size = sizeof(rejoinNWKFrameSnd);
      }
      break;

    case ZIGBEESETTCLINKKEY:
      {
        payload_size = sizeof(setTCLinkKeySnd);
      }
      break;

    case ZIGBEEZDOAPPZDPREQ:
      {
	      if(pkt->data[0] == ZIGBEESTARTDEVICEREQ)
        {
          rsi_driver_cb->zigb_cb->expected_response = APPZIGBEESTACKSTATUSHANDLER;
        }
	      payload_size = RSI_ZIGB_LARGE_BUFFER_SIZE;
      }
      break;
    case ZIGBEENWKSENDREQ:
    case ZIGBEEAPSDEREQ:
      {
        payload_size = RSI_ZIGB_LARGE_BUFFER_SIZE;
      }
      break;

    case ZIGBEEZDOSILENTSTARTREQ:
      {
        payload_size = RSI_ZIGB_ZDO_SILENT_START;
      }
      break;

    case ZIGBEEGETSETUPDATEID:
      {
        payload_size = 2;
      }
      break;

    case ZIGBEESETMACPIB:
      {
        payload_size = 4;
      }
      break;

    case ZIGBEEAESENCRYPT:
      {
        payload_size = pkt->data[0] + 2;
      }
      break;

    case ZIGBEESETZLLPARAMS:
      {
      payload_size = rsi_zigb_get_payload_len(pkt->data[0]) + 1;
      }
      break;

    case ZIGBEENVMUPDATE:
      {
        uint16_t offset = (uint16_t)((pkt->data[1]<<8) | (pkt->data[0]));
        payload_size = rsi_zigb_get_nvm_payload_len(offset);
        payload_size +=2;

      }
      break;

    case ZIGBEEENQUEUEMACMLME:
    case ZIGBEEENQUEUEMACMCPS:
      {
        payload_size = RSI_ZIGB_LARGE_BUFFER_SIZE;
      }
      break;

    case ZIGBEEMACPIBSET:
      {
        payload_size = sizeof(setMacPibFrameSnd);
      }
      break;

    case ZIGBEEMACPIBGET:
      {
        payload_size = sizeof(getMacPibFrameSnd);//TODO
      }
      break;

    default:
			{
        payload_size = 0;
      }
      break;
  }

	//! Fill payload length
	rsi_uint16_to_2bytes(host_desc, (payload_size & 0xFFF));

	//! Fill frame type
	host_desc[1] = RSI_ZIGB_QUEUE;
  host_desc[13] =MANAGEMENT_INTERFACE;

	//! Fill frame type
	host_desc[14] = cmd[0];
  host_desc[15] = cmd[1];
	//! If it is a command from common block
	if(cmd[1] == RSI_COMMON_REQ_OPERMODE)
	{
		//! Block WLAN TX queue
		rsi_block_queue(&rsi_driver_cb->wlan_tx_q);
#if (defined RSI_BT_ENABLE ||defined RSI_BLE_ENABLE || defined RSI_ZB_ENABLE)
		//! Block BT or ZB TX queue
		rsi_block_queue(&rsi_driver_cb->zigb_tx_q);
#endif

		rsi_driver_cb->common_cb->expected_response = cmd[1];

		//! Enqueue packet to common TX queue
		rsi_enqueue_pkt(&rsi_driver_cb->common_tx_q, pkt);

		//! Set TX packet pending event
		rsi_set_event(RSI_TX_EVENT);

		//! Wait on common semaphore
		rsi_semaphore_wait(&rsi_driver_cb->common_cb->common_sem, RSI_WAIT_FOREVER);

		//! get common cmd response status
		status =  rsi_common_get_status();
	}
	else
	{

		//! Enqueue packet to zigb TX queue
		rsi_enqueue_pkt(&rsi_driver_cb->zigb_tx_q, pkt);

		//! Set TX packet pending event
		rsi_set_event(RSI_TX_EVENT);

		//! Wait on zigb semaphore
		rsi_semaphore_wait(&rsi_driver_cb->zigb_cb->zigb_sem, RSI_WAIT_FOREVER);

	}

	//! Return status
	return status;
}


/*==============================================*/
/**
 * @fn         int32_t rsi_driver_process_zigb_recv_cmd(rsi_pkt_t *pkt)
 * @brief       This function processes the received pkt from firmware.
 * @param[in]   pkt, pointer of packet to send
 * @param[out]
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This functions processes the received packet.
 *
 *
 */
int32_t rsi_driver_process_zigb_recv_cmd(rsi_pkt_t *pkt)
{
  uint16_t payload_length=0;
  uint8_t  *buf_ptr=NULL, cmd_id = 0, intf_id = 0;


  //! Get zigb cb struct pointer
  rsi_zigb_cb_t *rsi_zigb_cb = rsi_driver_cb->zigb_cb;
  buf_ptr = (uint8_t *)pkt->desc;

  //! Step-1:- Get the payload length
	payload_length = ((uint16_t )buf_ptr[0] & 0xfff);

  //! update interface id and command id
  intf_id = *(buf_ptr + RSI_ZIGB_INTF_ID_OFFSET);
  cmd_id = *(buf_ptr + RSI_ZIGB_INTF_ID_OFFSET + 1);

#if 0
  RSI_DPRINT(RSI_PL1,"cmd_id : %x\n", cmd_id);
  RSI_DPRINT(RSI_PL1,"intf_id : %x\n",intf_id);
  RSI_DPRINT(RSI_PL1,"payload len: %d\n",payload_length);
#endif
  if(intf_id == INTERFACE_CALLBACK && (cmd_id != 0xFF))
  {
    //! Entry point to event handlers
    rsi_zigb_app_cb_handler(cmd_id, buf_ptr+RSI_ZIGB_FRAME_DESC_LEN);
  }
  else
  {
    //! check zigb_cb for any task is waiting for response
    if(rsi_zigb_cb->expected_response == cmd_id)
    {
      //copying the response to the received buffer
      memcpy(&rsi_zigb_cb->resp,(buf_ptr + RSI_ZIGB_FRAME_DESC_LEN),payload_length);
      //! Clear expected response
      rsi_zigb_cb->expected_response = 0;

      //! signal the zigb semaphore
      rsi_semaphore_post(&rsi_zigb_cb->zigb_sem);
    }
    else
    {
      //Received Unexpected Response
      //Should not come here
    }
  }

  return RSI_SUCCESS;
}

uint16_t rsi_zigb_global_cb_init(uint8_t *buffer)
{
	rsi_zigb_global_cb_t *zigb_global_cb = rsi_driver_cb->zigb_cb->zigb_global_cb;

	zigb_global_cb->zigb_specific_cb = (rsi_zigb_callback_t*)buffer;

	rsi_driver_cb->zigb_cb->zigb_global_cb = zigb_global_cb;

	return sizeof(rsi_zigb_global_cb_t);
}

#ifdef ZB_MAC_API
uint16_t rsi_zigb_global_mac_cb_init(uint8_t *buffer)
{
	rsi_zigb_global_mac_cb_t *zigb_global_mac_cb = rsi_driver_cb->zigb_cb->zigb_global_mac_cb;

	zigb_global_mac_cb->zigb_mac_cb = (rsi_zigb_mac_callback_t*)buffer;

	rsi_driver_cb->zigb_cb->zigb_global_mac_cb = zigb_global_mac_cb;
	return sizeof(rsi_zigb_global_mac_cb_t);
}
#endif
/**
 * @fn          rsi_zigb_register_gap_callbacks
 * @brief       This function registers the zigbee callbacks
 * @param[in]   rsi_ble_on_adv_report_event_t   ble_on_adv_report_event : Advertise report callback
 * @param[in]   rsi_ble_on_connect_t  ble_on_conn_status_event: Connection status callback
 * @param[in]	rsi_ble_on_disconnect_t   ble_on_disconnect_event : Disconnection status callback
 * @param[out]
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function registers the GAP callbacks
 *
 */
void rsi_zigb_register_callbacks (
      rsi_zigb_app_scan_complete_handler_t                  zigb_app_scan_complete_handler,
      rsi_zigb_app_energy_scan_result_handler_t             zigb_app_energy_scan_result_handler,
      rsi_zigb_app_network_found_handler_t                  zigb_app_network_found_handler,
      rsi_zigb_app_stack_status_handler_t                   zigb_app_stack_status_handler,
      rsi_zigb_app_incoming_many_to_one_route_req_handler_t zigb_app_incoming_many_to_one_route_req_handler,
      rsi_zigb_app_handle_data_indication_t                 zigb_app_handle_data_indication,
      rsi_zigb_app_handle_data_confirmation_t               zigb_app_handle_data_confirmation,
      rsi_zigb_app_child_join_handler_t                     zigb_app_child_join_handler,
      rsi_zigb_nvm_backup_handler_t                         zigb_nvm_backup_handler,
      rsi_zigb_zdp_app_response_handler_t                   zigb_zdp_app_response_handler
#ifdef ZB_PROFILE
      ,rsi_zigb_app_interpan_data_indication_t              zigb_app_interpan_data_indication,
       rsi_zigb_app_interpan_data_confirmation_t            zigb_app_interpan_data_confirmation
#endif
)
{
	   rsi_zigb_callback_t *zigb_specific_cb = rsi_driver_cb->zigb_cb->zigb_global_cb->zigb_specific_cb;

     zigb_specific_cb->zigb_app_scan_complete_handler                  = zigb_app_scan_complete_handler;
     zigb_specific_cb->zigb_app_energy_scan_result_handler             = zigb_app_energy_scan_result_handler;
     zigb_specific_cb->zigb_app_network_found_handler                  = zigb_app_network_found_handler;
     zigb_specific_cb->zigb_app_stack_status_handler                   = zigb_app_stack_status_handler;
     zigb_specific_cb->zigb_app_incoming_many_to_one_route_req_handler = zigb_app_incoming_many_to_one_route_req_handler;
     zigb_specific_cb->zigb_app_handle_data_indication                 = zigb_app_handle_data_indication;
     zigb_specific_cb->zigb_app_handle_data_confirmation               = zigb_app_handle_data_confirmation;
     zigb_specific_cb->zigb_app_child_join_handler                     = zigb_app_child_join_handler;
     zigb_specific_cb->zigb_nvm_backup_handler                         = zigb_nvm_backup_handler;
     zigb_specific_cb->zigb_zdp_app_response_handler                   = zigb_zdp_app_response_handler;
#ifdef ZB_PROFILE
     zigb_specific_cb->zigb_app_interpan_data_indication	       = zigb_app_interpan_data_indication;
     zigb_specific_cb->zigb_app_interpan_data_confirmation             = zigb_app_interpan_data_confirmation;
#endif
}

#ifdef ZB_MAC_API
/**
 * @fn          rsi_zigb_register_mac_callbacks
 * @brief       This function registers the zigbee callbacks
 * @param[in]   rsi_zigb_mac_data_confirm_t                zigb_mac_data_confirm
 * @param[in]   rsi_zigb_mac_data_indication_t             zigb_mac_data_indication
 * @param[in]   rsi_zigb_mac_assoc_confirm_t               zigb_mac_assoc_confirm
 * @param[in]   rsi_zigb_mac_assoc_indication_t            zigb_mac_assoc_indication
 * @param[in]   rsi_zigb_mac_disassoc_confirm_t            zigb_mac_disassoc_confirm
 * @param[in]   rsi_zigb_mac_disassoc_indication_t         zigb_mac_disassoc_indication
 * @param[in]   rsi_zigb_mac_beacon_notify_indication_t    zigb_mac_beacon_notify_indication
 * @param[in]   rsi_zigb_mac_orphan_indication_t           zigb_mac_orphan_indication
 * @param[in]   rsi_zigb_mac_rx_enable_confirm_t           zigb_mac_rx_enable_confirm
 * @param[in]   rsi_zigb_mac_scan_confirm_t                zigb_mac_scan_confirm
 * @param[in]   rsi_zigb_mac_comm_status_confirm_t         zigb_mac_comm_status_confirm
 * @param[in]   rsi_zigb_mac_start_confirm_t               zigb_mac_start_confirm
 * @param[in]   rsi_zigb_mac_poll_confirm_t                zigb_mac_poll_confirm
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function registers the zigb MAC callbacks.
 *
 */

void rsi_zigb_register_mac_callbacks (
    rsi_zigb_mac_data_confirm_t                zigb_mac_data_confirm,
    rsi_zigb_mac_data_indication_t             zigb_mac_data_indication,
    rsi_zigb_mac_assoc_confirm_t               zigb_mac_assoc_confirm,
    rsi_zigb_mac_assoc_indication_t            zigb_mac_assoc_indication,
    rsi_zigb_mac_disassoc_confirm_t            zigb_mac_disassoc_confirm,
    rsi_zigb_mac_disassoc_indication_t         zigb_mac_disassoc_indication,
    rsi_zigb_mac_beacon_notify_indication_t    zigb_mac_beacon_notify_indication,
    rsi_zigb_mac_orphan_indication_t           zigb_mac_orphan_indication,
    rsi_zigb_mac_rx_enable_confirm_t           zigb_mac_rx_enable_confirm,
    rsi_zigb_mac_scan_confirm_t                zigb_mac_scan_confirm,
    rsi_zigb_mac_comm_status_confirm_t         zigb_mac_comm_status_confirm,
    rsi_zigb_mac_start_confirm_t               zigb_mac_start_confirm,
    rsi_zigb_mac_poll_confirm_t                zigb_mac_poll_confirm
    )
{
  rsi_zigb_mac_callback_t *zigb_mac_cb = rsi_driver_cb->zigb_cb->zigb_global_mac_cb->zigb_mac_cb;

  zigb_mac_cb->zigb_mac_data_confirm                = zigb_mac_data_confirm;
  zigb_mac_cb->zigb_mac_data_indication             = zigb_mac_data_indication;
  zigb_mac_cb->zigb_mac_assoc_confirm               = zigb_mac_assoc_confirm;
  zigb_mac_cb->zigb_mac_assoc_indication            = zigb_mac_assoc_indication;
  zigb_mac_cb->zigb_mac_disassoc_confirm            = zigb_mac_disassoc_confirm;
  zigb_mac_cb->zigb_mac_disassoc_indication         = zigb_mac_disassoc_indication;
  zigb_mac_cb->zigb_mac_beacon_notify_indication    = zigb_mac_beacon_notify_indication;
  zigb_mac_cb->zigb_mac_orphan_indication           = zigb_mac_orphan_indication;
  zigb_mac_cb->zigb_mac_rx_enable_confirm           = zigb_mac_rx_enable_confirm;
  zigb_mac_cb->zigb_mac_scan_confirm                = zigb_mac_scan_confirm;
  zigb_mac_cb->zigb_mac_comm_status_confirm         = zigb_mac_comm_status_confirm;
  zigb_mac_cb->zigb_mac_start_confirm               = zigb_mac_start_confirm;
  zigb_mac_cb->zigb_mac_poll_confirm                = zigb_mac_poll_confirm;
}
#endif
/*===========================================================================
 *
 * @fn          void rsi_zigb_app_cb_handler(uint8_t cmd_id, uint8_t *buffer)
 * @brief       Handler for asyncronous data and interface pkts
 * @param[in]   cmd type
 * @param[in]   Buffer
 * @param[out]  none
 * @return      none
 * @section description
 * This API is used to handle different interface pkts
 * For eg: In this handler Scancomplete , network info/status, data Indication
 * and Confirmation Pkts will be handled
 *
 * ===========================================================================*/
void rsi_zigb_app_cb_handler(uint8_t cmd_id, uint8_t *buffer)
{
  uint8_t i = 0;
  uint16_t nvm_offset=0;
  rsi_zigb_cb_t *rsi_zigb_cb = rsi_driver_cb->zigb_cb;
	rsi_zigb_callback_t *zigb_specific_cb = NULL;
#ifdef ZB_MAC_API
  rsi_zigb_mac_callback_t *zigb_mac_cb = rsi_driver_cb->zigb_cb->zigb_global_mac_cb->zigb_mac_cb;
#else
	zigb_specific_cb = rsi_driver_cb->zigb_cb->zigb_global_cb->zigb_specific_cb;
#endif
  switch(cmd_id)
  {
    case APPSCANCOMPLETEHANDLER:
    {
      uint32_t channel = *(uint32_t *)buffer;
      uint8_t status = *(buffer + 4);
      zigb_specific_cb->zigb_app_scan_complete_handler (channel, status);
    }
    break;

    case APPENERGYSCANRESULTHANDLER:
    {
      uint32_t channel = *(uint32_t *)buffer;
      uint8_t pEnergyValue[16];

      rsi_zigb_mcpy((buffer + 4), pEnergyValue, 16);

      zigb_specific_cb->zigb_app_energy_scan_result_handler(channel, pEnergyValue);
    }
    break;

    case APPNETWORKFOUNDHANDLER:
    {
      ZigBeeNetworkDetails networkInformation;
      networkInformation.shortPanId = rsi_zigb_bytes2R_to_uint16(buffer);
      buffer += SHORT_PANID_SIZE;
      networkInformation.channel = *buffer++;
      rsi_zigb_mcpy(buffer, networkInformation.extendedPanId, EXTENDED_PANID_SIZE);
      buffer += EXTENDED_PANID_SIZE;

      networkInformation.stackProfile = *buffer++;
      networkInformation.nwkUpdateId = *buffer++;
      networkInformation.allowingJoining = (BOOL)*buffer++;

      zigb_specific_cb->zigb_app_network_found_handler(networkInformation);
    }
    break;

    case APPZIGBEESTACKSTATUSHANDLER:
    {
      ZigBeeNWKStatusInfo statusInfo;
      statusInfo = (ZigBeeNWKStatusInfo)*buffer;
      zigb_specific_cb->zigb_app_stack_status_handler(&statusInfo);
    }
    break;

    case APPINCOMINGMANYTOONEROUTEREQUESTHANDLER:
    {
      uint8_t pSrcIEEEAddr[8], PathCost;
      uint16_t SourceAddr;

      SourceAddr = rsi_zigb_bytes2R_to_uint16(buffer);
      buffer += 2;

      rsi_zigb_mcpy(buffer, pSrcIEEEAddr, EXTENDED_ADDR_SIZE);
      buffer += EXTENDED_ADDR_SIZE;

      PathCost = *buffer;

      zigb_specific_cb->zigb_app_incoming_many_to_one_route_req_handler(SourceAddr, pSrcIEEEAddr, PathCost);
    }
    break;

    case APPHANDLEDATAINDICATION:
    {
      APSDE_Data_Indication_t pDataIndication;
      pDataIndication.dest_addr_mode = *buffer++;

      if(pDataIndication.dest_addr_mode == g_SHORT_ADDR_MODE_c) {
        pDataIndication.dest_address.short_address = rsi_zigb_bytes2R_to_uint16(buffer);
        buffer += 2;
      } else if(pDataIndication.dest_addr_mode == g_EXTENDED_ADDR_MODE_c) {
        for(i =0; i < 8; i++) {
          pDataIndication.dest_address.IEEE_address[i] = *buffer++;
        }
      }

      pDataIndication.dest_endpoint = *buffer++;
      pDataIndication.src_addr_mode = *buffer++;

      if(pDataIndication.src_addr_mode == g_SHORT_ADDR_MODE_c) {
        pDataIndication.src_address.short_address = rsi_zigb_bytes2R_to_uint16(buffer);
        buffer += 2;
      } else if(pDataIndication.src_addr_mode == g_EXTENDED_ADDR_MODE_c) {
        for(i =0; i < 8; i++) {
          pDataIndication.src_address.IEEE_address[i] = *buffer++;
        }
      }
      pDataIndication.src_endpoint = *buffer++;
      pDataIndication.profile_id = rsi_zigb_bytes2R_to_uint16(buffer);
      buffer += 2;

      pDataIndication.cluster_id = rsi_zigb_bytes2R_to_uint16(buffer);
      buffer += 2;

      pDataIndication.asdulength = *buffer++;
      pDataIndication.was_broadcast = *buffer++;
      pDataIndication.security_status = *buffer++;
      pDataIndication.link_quality = *buffer++;

      for(i = 0; i < pDataIndication.asdulength; i++) {
        pDataIndication.a_asdu[i] = buffer[i];
      }
      zigb_specific_cb->zigb_app_handle_data_indication(&pDataIndication );
    }
    break;

    case APPHANDLEDATACONFIRMATION:
    {
      APSDE_Data_Confirmation_t DataConfirmation;
      DataConfirmation.dest_addr_mode = *buffer++;

      if(DataConfirmation.dest_addr_mode == g_SHORT_ADDR_MODE_c)
      {
        DataConfirmation.dest_address.short_address = rsi_zigb_bytes2R_to_uint16(buffer);
        buffer += 2;
      } else if(DataConfirmation.dest_addr_mode == g_EXTENDED_ADDR_MODE_c) {
        for(i =0; i < 8; i++) {
          DataConfirmation.dest_address.IEEE_address[i] = *buffer++;
        }
      }

      DataConfirmation.dest_endpoint = *buffer++;
      DataConfirmation.src_endpoint = *buffer++;
      DataConfirmation.status = *buffer++;

      zigb_specific_cb->zigb_app_handle_data_confirmation(&DataConfirmation);
    }
    break;

    case APPCHILDJOINHANDLER:
    {
	    BOOL Joining;
      uint16_t Short_address = rsi_zigb_bytes2R_to_uint16(buffer);
      buffer += 2;
      Joining = *buffer;
      zigb_specific_cb->zigb_app_child_join_handler(Short_address, Joining);
    }
    break;

#ifdef ZB_PROFILE
    case APPHANDLEINTERPANDATAINDDICATION:
    {
      uint8_t *ptxbuffer=NULL;
      uint8_t buffindex = 0;
      APSDE_InterPan_Data_Indication_t ApsdeDataInd;

      buffindex = buffMgmt_allocateBuffer ( RSI_ZIGB_LARGE_BUFFER_SIZE );
      if ( buffindex != 0xFF ) {
        ptxbuffer = buffMgmt_getBufferPointer( buffindex );
        memUtils_memCopy(ptxbuffer,buffer, RSI_ZIGB_LARGE_BUFFER_SIZE);
        buffer += RSI_ZIGB_LARGE_BUFFER_SIZE;
      }
      else{
        return 0xFF ;
      }

      ApsdeDataInd.srcaddrmode = *buffer++;
      ApsdeDataInd.srcpanid = rsi_zigb_bytes2R_to_uint16(buffer);
      buffer += 2;

      if(ApsdeDataInd.srcaddrmode == g_SHORT_ADDR_MODE_c) {
        ApsdeDataInd.srcaddress.short_address = rsi_zigb_bytes2R_to_uint16(buffer);
        buffer += 2;
      } else if (ApsdeDataInd.srcaddrmode == g_EXTENDED_ADDR_MODE_c) {
        for(i =0; i < 8; i++) {
          ApsdeDataInd.srcaddress.IEEE_address[i] = *buffer++;
        }
      }

      ApsdeDataInd.dstaddrmode = *buffer++;
      ApsdeDataInd.dstpanid = rsi_zigb_bytes2R_to_uint16(buffer);
      buffer += 2;

      if(ApsdeDataInd.dstaddrmode == g_SHORT_ADDR_MODE_c) {
        ApsdeDataInd.dstAddress.short_address = rsi_zigb_bytes2R_to_uint16(buffer);
        buffer += 2;
      } else if (ApsdeDataInd.dstaddrmode == g_EXTENDED_ADDR_MODE_c) {
        for(i =0; i < 8; i++) {
          ApsdeDataInd.dstAddress.IEEE_address[i] = *buffer++;
        }
      }
      ApsdeDataInd.profileId = rsi_zigb_bytes2R_to_uint16(buffer);
      buffer += 2;

      ApsdeDataInd.clusterId = rsi_zigb_bytes2R_to_uint16(buffer);
      buffer += 2;

      ApsdeDataInd.asdulength = *buffer++;
      ApsdeDataInd.link_quality = *buffer++;


      for(i = 0; i < ApsdeDataInd.asdulength; i++) {
        ApsdeDataInd.a_asdu[i] = buffer[i];
      }

      zigb_specific_cb->zigb_app_interpan_data_indication(buffindex ,&ApsdeDataInd);
    }
    break;

    case APPHANDLEINTERPANDATACONFIRMATION:
    {
      uint8_t *ptxbuffer=NULL;
      uint8_t buffindex = 0;
      APSDE_InterPan_Data_Confirmation_t InterPanDataConf;

      buffindex = buffMgmt_allocateBuffer ( RSI_ZIGB_LARGE_BUFFER_SIZE );
      if ( buffindex != 0xFF ) {
        ptxbuffer = buffMgmt_getBufferPointer( buffindex );
        memUtils_memCopy(ptxbuffer,buffer, RSI_ZIGB_LARGE_BUFFER_SIZE);
        buffer += RSI_ZIGB_LARGE_BUFFER_SIZE;
      }
      else{
        return 0xFF ;
      }
      InterPanDataConf.asduhandle = *buffer++;
      InterPanDataConf.status = *buffer++;
      zigb_specific_cb->zigb_app_interpan_data_confirmation(buffindex ,&InterPanDataConf);
    }
    break;
#endif

    case APPHANDLENVMDATA:
    {
      nvm_offset = rsi_zigb_bytes2R_to_uint16(buffer);;
      buffer += 2;

      zigb_specific_cb->zigb_nvm_backup_handler(buffer,nvm_offset);
    }
    break;

#ifdef ZB_MAC_API
    case ZIGBMACMCPSCB:
    case ZIGBMACMLMECB:
    {
      switch(*buffer)
      {
        case g_MCPS_DATA_CONFIRM_c:
          {
            zigb_mac_cb->zigb_mac_data_confirm((MCPS_Data_Confirm_t*)buffer);
          }
          break;
        case g_MCPS_DATA_INDICATION_c:
          {
            zigb_mac_cb->zigb_mac_data_indication((MCPS_Data_Indication_t*)buffer);
          }
          break;
        case g_MLME_ASSOCIATE_CONFIRM_c:
          {
            zigb_mac_cb->zigb_mac_assoc_confirm((MLME_Associate_Confirm_t*)buffer);
          }
          break;
        case g_MLME_ASSOCIATE_INDICATION_c:
          {
            zigb_mac_cb->zigb_mac_assoc_indication((MLME_Associate_Indication_t*)buffer);
          }
          break;
        case g_MLME_DISASSOCIATE_CONFIRM_c:
          {
            zigb_mac_cb->zigb_mac_disassoc_confirm((MLME_Disassociate_Confirm_t*)buffer);
          }
          break;
        case g_MLME_DISASSOCIATE_INDICATION_c:
          {
            zigb_mac_cb->zigb_mac_disassoc_indication((MLME_Disassociate_Indication_t*)buffer);
          }
          break;
        case g_MLME_BEACON_NOTIFY_INDICATION_c:
          {
            zigb_mac_cb->zigb_mac_beacon_notify_indication((MLME_Beacon_Notify_Indication_t*)buffer);
          }
          break;
        case g_MLME_ORPHAN_INDICATION_c:
          {
            zigb_mac_cb->zigb_mac_orphan_indication((MLME_Orphan_Indication_t*)buffer);
          }
          break;
        case g_MLME_RX_ENABLE_CONFIRM_c:
          {
            zigb_mac_cb->zigb_mac_rx_enable_confirm((MLME_RX_Enable_Confirm_t*)buffer);
          }
          break;
        case g_MLME_SCAN_CONFIRM_c:
          {
            zigb_mac_cb->zigb_mac_scan_confirm((MLME_Scan_Confirm_t*)buffer);
          }
          break;
        case g_MLME_COMM_STATUS_INDICATION_c:
          {
            zigb_mac_cb->zigb_mac_comm_status_confirm((MLME_Comm_Status_Indication_t*)buffer);
          }
          break;

        case g_MLME_START_CONFIRM_c:
          {
            zigb_mac_cb->zigb_mac_start_confirm((MLME_Start_Confirm_t*)buffer);
          }
          break;
        case g_MLME_POLL_CONFIRM_c:
          {
            zigb_mac_cb->zigb_mac_poll_confirm((MLME_Poll_Confirm_t*)buffer);
          }
          break;

        default:
          {
          }
          break;

      }

    }
    break;
#endif
case ZIGBEEZDPAPPRESPONSE:
    {
		    zigb_specific_cb->zigb_zdp_app_response_handler(buffer);
    }
    break;
    default:
    break;
  }
  if(rsi_zigb_cb->expected_response == cmd_id)
  {
    rsi_zigb_cb->expected_response = 0;
    rsi_semaphore_post(&rsi_zigb_cb->zigb_sem);
  }
}
/*==============================================*/
/**
 * @fn         int8_t rsi_zigb_cb_init(rsi_zigb_cb_t *zigb_cb)
 * @brief      Initializes zigb control block structure
 * @param[in]  zigb_cb, pointer to zigb cb structure
 * @param[out] None
 * @return     None
 *
 *
 * @section description
 * This function initializes zigb control block structure
 *
 *
 */

int8_t rsi_zigb_cb_init(rsi_zigb_cb_t *zigb_cb)
{
  int8_t retval = RSI_ERR_NONE;

  //! validate input parameter
  if(zigb_cb == NULL)
  {
    return RSI_ERROR_INVALID_PARAM;
  }


  //! Create zigb mutex
  rsi_mutex_create(&zigb_cb->zigb_mutex);
  zigb_cb->expected_response = 0;

  //! Create zigb semaphore
  rsi_semaphore_create(&zigb_cb->zigb_sem, 0);

  return retval;
}

/**
 * @fn          void rsi_zb_tx_done(rsi_pkt_t *pkt)
 * @brief       Handles protocol specific zb data transfer completion
 * @param[in]   zb_cb ZB control block
 * @param[out]  None
 * @return      void
 *
 * @section description
 * This function handles the protocol specific zb data transfer completion
 *
 */
void rsi_zb_tx_done(rsi_pkt_t *pkt)
{
  	rsi_zigb_cb_t *rsi_zigb_cb = rsi_driver_cb->zigb_cb;

	if((rsi_zigb_cb->expected_response == ZIGBEEPERMODE)
			&& ((pkt->data[0] == ZIGB_PER_TRANSMIT) || ((pkt->data[0] == ZIGB_ANT_SEL))))
	{
    	rsi_zigb_cb->expected_response = 0;
    	rsi_semaphore_post(&rsi_zigb_cb->zigb_sem);
	}
}

uint8_t rsi_zigb_get_payload_len(uint8_t id)
{
  switch(id)
  {

    case ZIGBEESETAESKEY:
    case ZIGBEESETNWKKEY:
      return RSI_ZIGB_FRAME_DESC_LEN;
      break;

    case ZIGBEESETEXTPANID:
    case ZIGBEESETTCADDR  :
    case ZIGBEESETEXTADDR :
      return RSI_ZIGB_EXTENDDED_ADDR_LEN;
      break;

    case ZIGBEESETSHORTPANID   :
    case ZIGBEESETSELFSHORTADDR:
      return RSI_ZIGB_BYTES_2;
      break;

    case ZIGBEESETRXONWHENIDLE:
    case ZIGBEEZLLGETOPERCHANNEL :
      return 0;
      break;

    case ZIGBEESETPHYTRXSTATE :
    case ZIGBEESETSASINDEX :
    case ZIGBEEZLLSETOPERCHANNEL :
      return 1;
      break;
    default:
      return RSI_ZIGB_EXTENDDED_ADDR_LEN;
      break;
  }
  return 0;
}
#endif
