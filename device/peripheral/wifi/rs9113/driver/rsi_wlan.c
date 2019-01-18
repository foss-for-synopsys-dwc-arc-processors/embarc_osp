/**
 * @file     rsi_wlan.c
 * @version  0.1
 * @date     15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains functions related to WLAN control block
 *
 *  @section Description  This file contains funtions related to WLAN control block
 *
 *
 */

/**
 * Include files
 * */

#include "rsi_driver.h"


/*
 * Global Variables
 * */
extern rsi_driver_cb_t *rsi_driver_cb;
//extern uint8_t* rsi_json_extract_filename(uint8_t* json, uint8_t* buffer);

/*==============================================*/
/**
 * @fn         int8_t rsi_wlan_cb_init(rsi_wlan_cb_t *wlan_cb)
 * @brief      Initializes wlan control block structure
 * @param[in]  wlan_cb, pointer to wlan cb structure
 * @param[out] None
 * @return     None
 *
 *
 * @section description
 * This function initializes wlan control block structure
 *
 *
 */

int8_t rsi_wlan_cb_init(rsi_wlan_cb_t *wlan_cb)
{
  int8_t retval = RSI_ERR_NONE;

  //! validate input parameter
  if(wlan_cb == NULL)
  {
    return RSI_ERROR_INVALID_PARAM;
  }

  //! initialize wlan control block with default values
  wlan_cb->state = RSI_WLAN_STATE_NONE;
  wlan_cb->status = 0;

  //! Create wlan mutex
  rsi_mutex_create(&wlan_cb->wlan_mutex);
  wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;

  //! Create wlan semaphore
  rsi_semaphore_create(&wlan_cb->wlan_sem, 0);
  wlan_cb->app_buffer = 0;

  return retval;
}


/*==============================================*/
/**
 * @fn          int32_t rsi_driver_wlan_send_cmd(rsi_wlan_cmd_request_t cmd, rsi_pkt_t *pkt)
 * @brief       This functions fills commands and places into wlan TX queue
 * @param[in]   cmd, type of the command to send
 * @param[in]   pkt, pointer of packet to send
 * @param[out]
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This functions fills commands and places into wlan TX queue
 *
 *
 */
int32_t rsi_driver_wlan_send_cmd(rsi_wlan_cmd_request_t cmd, rsi_pkt_t *pkt)
{
  uint16_t payload_size = 0;
  int32_t  status       = RSI_SUCCESS;
  uint8_t cmd_type = 0;
  uint16_t multicast_bitmap = 0;

  //! Get host descriptor pointer
  uint8_t *host_desc = (pkt->desc);

  switch(cmd)
  {
    case RSI_WLAN_REQ_BAND:
      {
        rsi_req_band_t *rsi_band = (rsi_req_band_t *)pkt->data;

        //! fill band value
        rsi_band->band_value =  RSI_BAND;

        //! fill payload size
        payload_size = sizeof(rsi_req_band_t);

      }
      break;
#if RSI_WLAN_CONFIG_ENABLE
    case RSI_WLAN_REQ_CONFIG:
      {
        rsi_req_config_t *rsi_config = (rsi_req_config_t *)pkt->data;

        //! fill configuration type
        rsi_config->config_type =  CONFIG_RTSTHRESHOLD;

        //! fill rtsthreshold value
        rsi_config->value =  RSI_RTS_THRESHOLD;

        //! fill payload size
        payload_size = sizeof(rsi_req_config_t);

      }
      break;
#endif
    case RSI_WLAN_REQ_SET_MAC_ADDRESS:
      {
        rsi_req_mac_address_t *rsi_set_mac = (rsi_req_mac_address_t *)pkt->data;

        //! fill mac address
        memcpy(rsi_set_mac->mac_address, rsi_driver_cb->wlan_cb->mac_address, 6);

        //! fill payload size
        payload_size = sizeof(rsi_req_mac_address_t);

      }
      break;
    case RSI_WLAN_REQ_SET_MULTICAST_FILTER:
      {

        rsi_req_multicast_filter_info_t *multicast_filter;

        multicast_filter = (rsi_req_multicast_filter_info_t *)pkt->data;

        cmd_type = multicast_filter->cmd_type;

        multicast_bitmap = (uint16_t )cmd_type;

        if((cmd_type == RSI_MULTICAST_MAC_ADD_BIT) || (cmd_type == RSI_MULTICAST_MAC_CLEAR_BIT))
        {
          multicast_bitmap |= (multicast_mac_hash((uint8_t *)multicast_filter->mac_address) << 8);
        }

        //! memset the pkt
        memset(&pkt->data,0,2);

        //! copy processed payload in to buffer
        rsi_uint16_to_2bytes((uint8_t *)&pkt->data, multicast_bitmap);

        //! fill payload size
        payload_size = 2;

      }
      break;
    case RSI_WLAN_REQ_SET_REGION:
      {
        rsi_req_set_region_t *rsi_set_region = (rsi_req_set_region_t *)pkt->data;

        //! select set region code from user or from beacon
        rsi_set_region->set_region_code_from_user_cmd =  RSI_SET_REGION_FROM_USER_OR_BEACON;

        //! fill region code
        rsi_set_region->region_code =  RSI_REGION_CODE;

        //! fill module type
        rsi_uint16_to_2bytes(rsi_set_region->module_type, RSI_MODULE_TYPE);

        //! fill payload size
        payload_size = sizeof(rsi_req_set_region_t);

      }
      break;
    case RSI_WLAN_REQ_SET_REGION_AP:
      {
        rsi_req_set_region_ap_t *rsi_set_region_ap = (rsi_req_set_region_ap_t *)pkt->data;

        //! select set region code from user or flash
        rsi_set_region_ap->set_region_code_from_user_cmd =  RSI_SET_REGION_AP_FROM_USER;

        //! fill region code
        rsi_strcpy( rsi_set_region_ap->country_code, RSI_COUNTRY_CODE);

#if RSI_SET_REGION_AP_FROM_USER
        //! extract set region ap information
        extract_setregionap_country_info(rsi_set_region_ap);
#endif

        //! fill payload size
        payload_size = sizeof(rsi_req_set_region_ap_t);

      }
      break;
    case RSI_WLAN_REQ_REJOIN_PARAMS:
      {
        rsi_req_rejoin_params_t *rsi_rejoin = (rsi_req_rejoin_params_t *)pkt->data;

        //! fill rejoin retry count
        rsi_uint32_to_4bytes(rsi_rejoin->max_retry, RSI_REJOIN_MAX_RETRY);

        //! fill rejoin retry scan interval
        rsi_uint32_to_4bytes(rsi_rejoin->scan_interval, RSI_REJOIN_SCAN_INTERVAL);

        //! fill beacon missed count
        rsi_uint32_to_4bytes(rsi_rejoin->beacon_missed_count, RSI_REJOIN_BEACON_MISSED_COUNT);

        //! fill rejoin first time retry enable or disable
        rsi_uint32_to_4bytes(rsi_rejoin->first_time_retry_enable, RSI_REJOIN_FIRST_TIME_RETRY);

        //! fill payload size
        payload_size = sizeof(rsi_req_rejoin_params_t);

      }
      break;
    case RSI_WLAN_REQ_EAP_CONFIG:
      {
        rsi_req_eap_config_t *rsi_eap_req =  (rsi_req_eap_config_t *)pkt->data;

        //! copy enterprise configuratiomn data
        rsi_strcpy((int8_t *)rsi_eap_req->eap_method, RSI_EAP_METHOD);
        rsi_strcpy((int8_t *)rsi_eap_req->inner_method, RSI_EAP_INNER_METHOD);
        rsi_uint32_to_4bytes((uint8_t *)rsi_eap_req->okc_enable, 0 );
        rsi_strcpy((int8_t *)rsi_eap_req->private_key_password, RSI_PRIVATE_KEY_PASSWORD);

        //! fill payload size
        payload_size = sizeof(rsi_req_eap_config_t);

      }
      break;
    case RSI_WLAN_REQ_SCAN:
      {
        //! Scan Parameters
        rsi_req_scan_t *rsi_scan = (rsi_req_scan_t *)pkt->data;

        //! Sets channel bitmap for 2.4GHz
        if(RSI_BAND == RSI_DUAL_BAND)
        {
          rsi_uint16_to_2bytes(rsi_scan->channel_bit_map_2_4, RSI_SCAN_CHANNEL_BIT_MAP_2_4);
          rsi_uint32_to_4bytes(rsi_scan->channel_bit_map_5, RSI_SCAN_CHANNEL_BIT_MAP_5);
        }
        else if (RSI_BAND == RSI_BAND_5GHZ)
        {
          //! Set channel bitmap for 5GHz
          rsi_uint32_to_4bytes(rsi_scan->channel_bit_map_5, RSI_SCAN_CHANNEL_BIT_MAP_5);
        }
        else
        {
          rsi_uint16_to_2bytes(rsi_scan->channel_bit_map_2_4, RSI_SCAN_CHANNEL_BIT_MAP_2_4);
        }

        if((!(rsi_bytes4R_to_uint32(rsi_scan->channel))) && !(rsi_strlen(rsi_scan->ssid)))
        {
          rsi_scan->scan_feature_bitmap = RSI_SCAN_FEAT_BITMAP;
        }
        else
        {
          rsi_scan->scan_feature_bitmap = 0;
        }

        payload_size = sizeof(rsi_req_scan_t);

      }
      break;
    case RSI_WLAN_REQ_JOIN:
      {
        //! Join Parameters
        rsi_req_join_t *rsi_join = (rsi_req_join_t *)pkt->data;

        rsi_join->data_rate     = RSI_DATA_RATE;
        rsi_join->power_level   = RSI_POWER_LEVEL;
        rsi_join->join_feature_bitmap = RSI_JOIN_FEAT_BIT_MAP;
        rsi_uint32_to_4bytes(rsi_join->listen_interval, RSI_LISTEN_INTERVAL);

        payload_size = sizeof(rsi_req_join_t);

      }
      break;
    case RSI_WLAN_REQ_CONFIGURE_P2P:
      {
        //! payload of p2p parameters
        payload_size = sizeof(rsi_req_configure_p2p_t);
      }
      break;
    case RSI_WLAN_REQ_HOST_PSK:
      {
        //! PSK and PMK parameters
        payload_size = sizeof(rsi_req_psk_t);
      }
      break;
    case RSI_WLAN_REQ_BG_SCAN:
      {
        rsi_req_bg_scan_t *rsi_bg_scan = (rsi_req_bg_scan_t *)pkt->data;

        if(pkt->data[0] == 0xAB)
        {
          if(pkt->data[1] == 1)
          {
            //! Enable or Disable bg scan
            rsi_uint16_to_2bytes(rsi_bg_scan->bgscan_enable, 1);

            //! Enable or Disable instant bg scan
            rsi_uint16_to_2bytes(rsi_bg_scan->enable_instant_bgscan, 1);

          }

        }
        else
        {
          //! Enable or Disable bg scan
          rsi_uint16_to_2bytes(rsi_bg_scan->bgscan_enable, RSI_BG_SCAN_ENABLE);

          //! Enable or Disable instant bg scan
          rsi_uint16_to_2bytes(rsi_bg_scan->enable_instant_bgscan, RSI_INSTANT_BG);
        }
        //! bg scan threshold value
        rsi_uint16_to_2bytes(rsi_bg_scan->bgscan_threshold, RSI_BG_SCAN_THRESHOLD);

        //! tolerance threshold
        rsi_uint16_to_2bytes(rsi_bg_scan->rssi_tolerance_threshold, RSI_RSSI_TOLERANCE_THRESHOLD);

        //! periodicity
        rsi_uint16_to_2bytes(rsi_bg_scan->bgscan_periodicity, RSI_BG_SCAN_PERIODICITY);

        //! active scan duration
        rsi_uint16_to_2bytes(rsi_bg_scan->active_scan_duration, RSI_ACTIVE_SCAN_DURATION);

        //! passive scan duration
        rsi_uint16_to_2bytes(rsi_bg_scan->passive_scan_duration, RSI_PASSIVE_SCAN_DURATION);

        //! Enable multi probe
        rsi_bg_scan->multi_probe = RSI_MULTIPROBE;

        //! fill payload size
        payload_size = sizeof(rsi_req_bg_scan_t);

      }
      break;

    case RSI_WLAN_REQ_TIMEOUT:
      {

    	rsi_req_timeout_t * rsi_timeout = (rsi_req_timeout_t *)pkt->data;

    	//! Timeout Bitmap
    	rsi_uint32_to_4bytes(rsi_timeout->timeout_bitmap, RSI_TIMEOUT_BIT_MAP);

    	//! Timeout value
    	rsi_uint16_to_2bytes(rsi_timeout->timeout_value, RSI_TIMEOUT_VALUE);

    	//! fill payload size
    	payload_size = sizeof(rsi_req_timeout_t);
      }
      break;

    case RSI_WLAN_REQ_ROAM_PARAMS:
      {
        rsi_req_roam_params_t *rsi_roam_params = (rsi_req_roam_params_t *)pkt->data;

        //! Enable or Disable bg scan
        rsi_uint32_to_4bytes(rsi_roam_params->roam_enable, RSI_ROAMING_SUPPORT);

        //! Enable or Disable instant bg scan
        rsi_uint32_to_4bytes(rsi_roam_params->roam_threshold, RSI_ROAMING_THRESHOLD);

        //! bg scan threshold value
        rsi_uint32_to_4bytes(rsi_roam_params->roam_hysteresis, RSI_ROAMING_HYSTERISIS);

        //! fill payload size
        payload_size = sizeof(rsi_req_roam_params_t);

      }
      break;
    case RSI_WLAN_REQ_WPS_METHOD:
      {
        //! fill payload size for wps method
        payload_size = sizeof(rsi_req_wps_method_t);

      }
      break;
    case RSI_WLAN_REQ_DISCONNECT:
      {
        //! fill payload size for disconnect parameters
        payload_size = sizeof(rsi_req_disassoc_t);

      }
      break;
    case RSI_WLAN_REQ_TX_TEST_MODE:
      {
        rsi_req_tx_test_info_t *rsi_tx_test_info = (rsi_req_tx_test_info_t *)pkt->data;

        //! Enable or Disable transmit test mode
        rsi_uint16_to_2bytes(rsi_tx_test_info->rate_flags,RSI_TX_TEST_RATE_FLAGS );

        //! Enable or Disable transmit test mode
        rsi_uint16_to_2bytes(rsi_tx_test_info->channel_bw, RSI_TX_TEST_PER_CH_BW);

        //! Enable or Disable transmit test mode
        rsi_uint16_to_2bytes(rsi_tx_test_info->aggr_enable,RSI_TX_TEST_AGGR_ENABLE );

        //! Enable or Disable transmit test mode
        rsi_uint16_to_2bytes(rsi_tx_test_info->reserved,0 );

        //! Enable or Disable transmit test mode
        rsi_uint16_to_2bytes(rsi_tx_test_info->no_of_pkts, 0);

        //! Enable or Disable transmit test mode
        rsi_uint16_to_2bytes(rsi_tx_test_info->delay,RSI_TX_TEST_DELAY );


        //!  fill payload size tx test mode parameters
        payload_size = sizeof(rsi_req_tx_test_info_t);

      }
      break;
    case RSI_WLAN_REQ_RX_STATS:
      {
        //!  fill payload size rx stats parameters
        payload_size = sizeof(rsi_req_rx_stats_t);

      }
      break;
    case RSI_WLAN_REQ_PING_PACKET:
      {
        //! PING parameters
        payload_size = sizeof(rsi_req_ping_t);
      }
      break;
    case RSI_WLAN_REQ_SOCKET_CREATE:
      {
        //! Socket Parameters
        rsi_req_socket_t *socket_create = (rsi_req_socket_t *)pkt->data;

        rsi_uint32_to_4bytes(socket_create->tos, RSI_TOS);

        socket_create->ssl_ciphers  = RSI_SSL_CIPHERS;

        payload_size = sizeof(rsi_req_socket_t);

      }
      break;
    case RSI_WLAN_REQ_SOCKET_READ_DATA:
      {
        payload_size = sizeof(rsi_req_socket_read_t);
      }
      break;
    case RSI_WLAN_REQ_SET_CERTIFICATE:
      {
        rsi_req_set_certificate_t *set_certificate = (rsi_req_set_certificate_t *)pkt->data;

        if(set_certificate->cert_info.more_chunks)
        {
          payload_size = RSI_MAX_CERT_SEND_SIZE;
        }
        else
        {
          payload_size = rsi_bytes2R_to_uint16(set_certificate->cert_info.certificate_length) + sizeof(struct rsi_cert_info_s) ;
        }
      }
      break;
    case RSI_WLAN_REQ_SET_WEP_KEYS:
      {
        payload_size = sizeof(rsi_wep_keys_t);
      }
      break;
    case RSI_WLAN_REQ_IPCONFV4:
      {
        payload_size = sizeof(rsi_req_ipv4_parmas_t);
      }
      break;
    case RSI_WLAN_REQ_IPCONFV6:
      {
        payload_size = sizeof(rsi_req_ipv6_parmas_t);
      }
      break;
    case RSI_WLAN_REQ_MDNSD:
    case RSI_WLAN_REQ_HTTP_CLIENT_POST:
    case RSI_WLAN_REQ_HTTP_CLIENT_GET:
    case RSI_WLAN_REQ_HTTP_CLIENT_POST_DATA:
      {
        payload_size = rsi_bytes2R_to_uint16(host_desc);
      }
      break;
    case RSI_WLAN_REQ_SMTP_CLIENT:
      {
        payload_size = rsi_bytes2R_to_uint16(host_desc);
      }
      break;
    case RSI_WLAN_REQ_SOCKET_ACCEPT:
      {
        payload_size = sizeof(rsi_req_socket_accept_t);
      }
      break;
    case RSI_WLAN_REQ_DNS_QUERY:
      {
        payload_size = sizeof(rsi_req_dns_query_t);
      }
      break;
    case RSI_WLAN_REQ_DNS_SERVER_ADD:
      {
        payload_size = sizeof(rsi_req_dns_server_add_t);
      }
      break;
    case RSI_WLAN_REQ_DHCP_USER_CLASS:
      {
    	payload_size = sizeof(rsi_dhcp_user_class_t);
      }
      break;
    case RSI_WLAN_REQ_SOCKET_CLOSE:
      {
        payload_size = sizeof(rsi_req_socket_close_t);
      }
      break;

    case RSI_WLAN_REQ_AP_CONFIGURATION:
      {
        rsi_req_ap_config_t *ap_config = (rsi_req_ap_config_t *)pkt->data;
#if RSI_AP_KEEP_ALIVE_ENABLE
        //! copy AP keep alive type
        ap_config->ap_keepalive_type = RSI_AP_KEEP_ALIVE_TYPE;

        //! copy AP keep alive period
        ap_config->ap_keepalive_period = RSI_AP_KEEP_ALIVE_PERIOD;
#endif
        //! copy max station supported  by uint16 to 2 bytes conversion
        rsi_uint16_to_2bytes(ap_config->max_sta_support, RSI_MAX_STATIONS_SUPPORT);

        payload_size = sizeof(rsi_req_ap_config_t);

      }
      break;
    case RSI_WLAN_REQ_HT_CAPABILITIES:
      {
        rsi_req_ap_ht_caps_t *ap_ht_caps = (rsi_req_ap_ht_caps_t *)pkt->data;

        rsi_uint16_to_2bytes(ap_ht_caps->mode_11n_enable, RSI_MODE_11N_ENABLE);

        rsi_uint16_to_2bytes(ap_ht_caps->ht_caps_bitmap, RSI_HT_CAPS_BIT_MAP);

        payload_size = sizeof(rsi_req_ap_ht_caps_t);

      }
      break;
    case RSI_WLAN_REQ_FTP:
      {
        rsi_ftp_file_ops_t *file_ops = (rsi_ftp_file_ops_t *)pkt->data;


        switch(file_ops->command_type)
        {
          case RSI_FTP_CREATE:
          case RSI_FTP_DISCONNECT:
          case RSI_FTP_DESTROY:
          case RSI_FTP_FILE_WRITE:
          case RSI_FTP_FILE_READ:
          case RSI_FTP_FILE_DELETE:
          case RSI_FTP_DIRECTORY_SET:
          case RSI_FTP_DIRECTORY_DELETE:
          case RSI_FTP_DIRECTORY_LIST:
          case RSI_FTP_DIRECTORY_CREATE:
          case RSI_FTP_PASSIVE:
          case RSI_FTP_ACTIVE:
            {
              payload_size = sizeof(rsi_ftp_file_ops_t);
            }
            break;
          case RSI_FTP_CONNECT:
            {
              payload_size = sizeof(rsi_ftp_connect_t);
            }
            break;
          case RSI_FTP_FILE_RENAME:
            {
              payload_size = sizeof(rsi_ftp_file_rename_t);
            }
            break;
          case RSI_FTP_FILE_WRITE_CONTENT:
            {
              payload_size = rsi_bytes2R_to_uint16(host_desc);
            }
            break;
        }

      }
      break;
    case RSI_WLAN_REQ_WMM_PS:
      {
        //! wmmps parameters
        rsi_wmm_ps_parms_t *wmm_ps = (rsi_wmm_ps_parms_t *)pkt->data;

        //! set wmm enable or disable
        rsi_uint16_to_2bytes(wmm_ps->wmm_ps_enable, RSI_WMM_PS_ENABLE);

        //! set wmm enable or disable
        //! 0- TX BASED 1 - PERIODIC
        rsi_uint16_to_2bytes(wmm_ps->wmm_ps_type, RSI_WMM_PS_TYPE);

        //! set wmm wake up interval
        rsi_uint32_to_4bytes(wmm_ps->wmm_ps_wakeup_interval, RSI_WMM_PS_WAKE_INTERVAL);

        //! set wmm UAPSD bitmap
        wmm_ps->wmm_ps_uapsd_bitmap = RSI_WMM_PS_UAPSD_BITMAP;

        payload_size = sizeof(rsi_wmm_ps_parms_t);
      }
      break;
	  case RSI_WLAN_REQ_MULTICAST:
      {
        payload_size = sizeof(rsi_req_multicast_t);
      }
      break;
	  case RSI_WLAN_REQ_FWUP:
      {
        rsi_req_fwup_t *fwup = (rsi_req_fwup_t *)pkt->data;

        //! Get packet length
        uint16_t length = rsi_bytes2R_to_uint16(fwup->length);

        //! Content length + 2bytes type + 2bytes length
        payload_size = (length + 4);
      }
      break;
    case RSI_WLAN_REQ_WEBPAGE_LOAD:
      {
         payload_size = rsi_bytes2R_to_uint16(host_desc);
      }
      break;
    case RSI_WLAN_REQ_JSON_LOAD:
      {
        payload_size = rsi_bytes2R_to_uint16(host_desc);
      }
      break;
    case  RSI_WLAN_REQ_WEBPAGE_ERASE:
      {
        payload_size = rsi_bytes2R_to_uint16(host_desc);
      }
      break;
    case  RSI_WLAN_REQ_JSON_OBJECT_ERASE:
      {
        payload_size = rsi_bytes2R_to_uint16(host_desc);
      }
	  break;
	   case  RSI_WLAN_REQ_WEBPAGE_CLEAR_ALL:
      {

        payload_size = rsi_bytes2R_to_uint16(host_desc);
      }
      break;
    case  RSI_WLAN_REQ_HOST_WEBPAGE_SEND:
      {
        payload_size = rsi_bytes2R_to_uint16(host_desc);
      }
      break;
   case RSI_WLAN_REQ_POP3_CLIENT:
      {
        payload_size = sizeof(rsi_req_pop3_client_t);
      }
	  break;
   case RSI_WLAN_REQ_HTTP_CLIENT_PUT:
    {

      payload_size = rsi_bytes2R_to_uint16(host_desc);
    }
    break;
   case RSI_WLAN_REQ_OTA_FWUP:
    {
      payload_size = sizeof(rsi_req_ota_fwup_t);
    }
    break;
   case RSI_WLAN_REQ_SET_PROFILE:
    {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    }
    break;
    case RSI_WLAN_REQ_GET_PROFILE:
    case RSI_WLAN_REQ_DELETE_PROFILE:
      {
        payload_size = sizeof(rsi_profile_req_t);
      }
      break;
    case RSI_WLAN_REQ_AUTO_CONFIG_ENABLE:
      {
        payload_size = sizeof(rsi_auto_config_enable_t);
      }
      break;
    case RSI_WLAN_REQ_DEBUG_PRINTS:
      {
        rsi_req_debug_print_t *rsi_debug_print = (rsi_req_debug_print_t *)pkt->data;

        //! Enable or Disable debug print
        rsi_uint32_to_4bytes(rsi_debug_print->assertion_type, RSI_ASSERSION_TYPE);

        //! value of debug print
        rsi_uint32_to_4bytes(rsi_debug_print->assertion_value, RSI_ASSERSION_VALUE);

        //! fill payload size
        payload_size = sizeof(rsi_req_debug_print_t);

      }
      break;
    default:
      {
        break;
      }
  }
  //! Memset host descriptor
  memset(host_desc, 0, RSI_HOST_DESC_LENGTH);

  //! Fill payload length
  rsi_uint16_to_2bytes(host_desc, (payload_size & 0xFFF));

  //! Fill frame type
  host_desc[1] |= (RSI_WLAN_MGMT_Q << 4);

  //! Fill frame type
  host_desc[2] = cmd;



  //! Enqueue packet to WLAN TX queue
  rsi_enqueue_pkt(&rsi_driver_cb->wlan_tx_q, pkt);

  //! Set TX packet pending event
  rsi_set_event(RSI_TX_EVENT);

  if(rsi_driver_cb->wlan_cb->expected_response != RSI_WLAN_RSP_ASYNCHRONOUS)
  {
    if(cmd == RSI_WLAN_REQ_SOCKET_ACCEPT)
    {
      rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_CONN_ESTABLISH;
    }
    else if(cmd == RSI_WLAN_REQ_IPCONFV6)
    {
      rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_IPCONFV6;
    }
    else
    {
      //! Save expected response type
      rsi_driver_cb->wlan_cb->expected_response = (rsi_wlan_cmd_response_t )cmd;
    }
  }

  //! Wait on wlan semaphore
  rsi_semaphore_wait(&rsi_driver_cb->wlan_cb->wlan_sem, RSI_WAIT_FOREVER);

  if(rsi_driver_cb->wlan_cb->expected_response == RSI_WLAN_RSP_ASYNCHRONOUS)
  {
    if(cmd == RSI_WLAN_REQ_SOCKET_ACCEPT)
    {
      rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_CONN_ESTABLISH;
    }
    else if(cmd == RSI_WLAN_REQ_IPCONFV6)
    {
      rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_IPCONFV6;
    }
    else
    {
      //! Save expected response type
      rsi_driver_cb->wlan_cb->expected_response = (rsi_wlan_cmd_response_t )cmd;
    }
  }
  //! get wlan/network command response status
  status =  rsi_wlan_get_status();


  //! Return status
  return status;
}


/*==============================================*/
/**
 * @fn          int32_t rsi_driver_process_wlan_recv_cmd(rsi_pkt_t *pkt)
 * @brief       Processes wlan receive commands
 * @param[in]   pkt, pointet to received RX packet
 * @param[out]  None
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function processes wlan RX packets
 *
 */

int32_t rsi_driver_process_wlan_recv_cmd(rsi_pkt_t *pkt)
{
  uint8_t  cmd_type;
  //uint8_t  ip_version,sockID,sock_id;
  int32_t  status = RSI_SUCCESS;
  uint16_t copy_length;
  uint8_t  *host_desc;
  uint8_t  *payload;
  uint16_t payload_length;
  //uint16_t length;
  //uint8_t  buffers_freed;
  int8_t wlan_pkt_pending = 0;


  //! Get wlan cb struct pointer
  rsi_wlan_cb_t *rsi_wlan_cb = rsi_driver_cb->wlan_cb;

  //! Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  //! Get host descriptor
  host_desc = pkt->desc;

  //! Get command type
  cmd_type = pkt->desc[2];

  //! Get payload pointer
  payload = pkt->data;

  //! Get payoad length
  payload_length = (rsi_bytes2R_to_uint16(host_desc) & 0xFFF);

  //! check status
  status  = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

#if RSI_WITH_OS
  //! added to get over race condition due to async messsages updating status of global structure
  if(rsi_wlan_cb->expected_response == cmd_type)
	{
      rsi_wlan_set_status(status);
	}
#else
  //! update the status in wlan_cb
  rsi_wlan_set_status(status);
#endif

  //! check for packet pending in wlan queue
  wlan_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->wlan_tx_q);

  switch(cmd_type)
  {
    case RSI_WLAN_RSP_BAND:
      {
        //! if success, update state  wlan_cb state to band done
        if(status == RSI_SUCCESS)
        {
          rsi_wlan_cb->state = RSI_WLAN_STATE_BAND_DONE;
        }
      }
      break;
    case RSI_WLAN_RSP_INIT:
      {

        //! update state  wlan_cb state to init done
        if(status == RSI_SUCCESS)
        {
          rsi_wlan_cb->state = RSI_WLAN_STATE_INIT_DONE;
          common_cb->ps_coex_mode = (common_cb->ps_coex_mode | BIT(0));
        }

      }
      break;
    case RSI_WLAN_RSP_WPS_METHOD:
      {

        if(status == RSI_SUCCESS)
        {
          //! check the length of application buffer and copy generated wps pin
          if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
          {
            copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length) : (rsi_wlan_cb->app_buffer_length);
            memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
            rsi_wlan_cb->app_buffer = NULL;
          }
        }

      }
      break;
    case RSI_WLAN_RSP_CONNECTION_STATUS:
      {

        if(status == RSI_SUCCESS)
        {
          //! check the length of application buffer and copy connection status
          if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
          {
            copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length) : (rsi_wlan_cb->app_buffer_length);
            memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
            rsi_wlan_cb->app_buffer = NULL;
          }
        }

      }
      break;
    case RSI_WLAN_RSP_RSSI:
      {

        if(status == RSI_SUCCESS)
        {
          //! check the length of application buffer and copy rssi value
          if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
          {
            copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length) : (rsi_wlan_cb->app_buffer_length);
            memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
            rsi_wlan_cb->app_buffer = NULL;
          }
        }

      }
      break;
    case RSI_WLAN_RSP_FW_VERSION:
      {

        if(status == RSI_SUCCESS)
        {
          //! check the length of application buffer and copy firmware version
          if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
          {
            copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length) : (rsi_wlan_cb->app_buffer_length);
            memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
            rsi_wlan_cb->app_buffer = NULL;
          }
        }

      }
      break;
    case RSI_WLAN_RSP_MAC_ADDRESS:
      {

        if(status == RSI_SUCCESS)
        {
          //! check the length of application buffer and copy mac address
          if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
          {
            copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length) : (rsi_wlan_cb->app_buffer_length);
            memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
            rsi_wlan_cb->app_buffer = NULL;
          }
        }

      }
      break;
    case RSI_WLAN_RSP_QUERY_NETWORK_PARAMS:
      {

        if(status == RSI_SUCCESS)
        {
          //! check the length of application buffer and copy scan results
          if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
          {

            if(rsi_wlan_cb->query_cmd == RSI_WLAN_INFO )
            {
              //! copy wlan related information in to output buffer
              if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
              {
                copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? sizeof(rsi_rsp_wireless_info_t) : (rsi_wlan_cb->app_buffer_length);

                ((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->wlan_state = (uint16_t)((rsi_rsp_nw_params_t *)payload)->wlan_state;
                ((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->channel_number = (uint16_t)((rsi_rsp_nw_params_t *)payload)->channel_number;
                memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->ssid, ((rsi_rsp_nw_params_t *)payload)->ssid, RSI_SSID_LEN);
                memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->mac_address, ((rsi_rsp_nw_params_t *)payload)->mac_address, 6);
                ((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->sec_type = ((rsi_rsp_nw_params_t *)payload)->sec_type;
                memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->psk, ((rsi_rsp_nw_params_t *)payload)->psk, 64);
                memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->ipv4_address, ((rsi_rsp_nw_params_t *)payload)->ipv4_address, 4);
                memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->ipv6_address, ((rsi_rsp_nw_params_t *)payload)->ipv6_address, 16);
              }

            }
            else if(rsi_wlan_cb->query_cmd == RSI_SOCKETS_INFO )
            {
              if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
              {
                copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? sizeof(rsi_rsp_sockets_info_t) : (rsi_wlan_cb->app_buffer_length);

                memcpy(((rsi_rsp_sockets_info_t *)rsi_wlan_cb->app_buffer)->num_open_socks, ((rsi_rsp_nw_params_t *)payload)->num_open_socks, 2);
                memcpy(((rsi_rsp_sockets_info_t *)rsi_wlan_cb->app_buffer)->socket_info, ((rsi_rsp_nw_params_t *)payload)->socket_info, sizeof(rsi_sock_info_query_t)*RSI_MN_NUM_SOCKETS);
              }

            }
            rsi_wlan_cb->app_buffer = NULL;

          }

        }
      }
      break;
    case RSI_WLAN_RSP_QUERY_GO_PARAMS:
      {

        if(status == RSI_SUCCESS)
        {
          //! check the length of application buffer and copy scan results
          if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
          {

            if(rsi_wlan_cb->query_cmd == RSI_WLAN_INFO )
            {

              if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
              {
                copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? sizeof(rsi_rsp_wireless_info_t) : (rsi_wlan_cb->app_buffer_length);

                ((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->wlan_state = rsi_bytes2R_to_uint16(((rsi_rsp_go_params_t *)payload)->sta_count);
                ((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->channel_number = rsi_bytes2R_to_uint16(((rsi_rsp_go_params_t *)payload)->channel_number);
                memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->ssid, ((rsi_rsp_go_params_t *)payload)->ssid, RSI_SSID_LEN);
                memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->mac_address, ((rsi_rsp_go_params_t *)payload)->mac_address, 6);
                memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->psk, ((rsi_rsp_go_params_t *)payload)->psk, 64);
                memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->ipv4_address, ((rsi_rsp_go_params_t *)payload)->ipv4_address, 4);
                memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->ipv6_address, ((rsi_rsp_go_params_t *)payload)->ipv6_address, 16);
              }
            }
            else if(rsi_wlan_cb->query_cmd == RSI_STATIONS_INFO )
            {

              if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
              {
                copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? sizeof(rsi_rsp_wireless_info_t) : (rsi_wlan_cb->app_buffer_length);

                memcpy(((rsi_rsp_stations_info_t *)rsi_wlan_cb->app_buffer)->sta_count, ((rsi_rsp_go_params_t *)payload)->sta_count, 2);
                memcpy(((rsi_rsp_stations_info_t *)rsi_wlan_cb->app_buffer)->sta_info, ((rsi_rsp_go_params_t *)payload)->sta_info, sizeof(rsi_go_sta_info_t)*RSI_MAX_STATIONS);
              }
            }
            rsi_wlan_cb->app_buffer = NULL;

          }

        }
      }
      break;
    case RSI_WLAN_RSP_SMTP_CLIENT:
      {
        if((host_desc[5] == RSI_SMTP_CLIENT_MAIL_SEND) &&(rsi_driver_cb->nwk_cb->nwk_callbacks.smtp_client_mail_response_handler != NULL))
        {
          //! Call asynchronous response handler to indicate to host
          rsi_driver_cb->nwk_cb->nwk_callbacks.smtp_client_mail_response_handler(status, host_desc[5]);

          return RSI_SUCCESS;
        }
        else if((host_desc[5] == RSI_SMTP_CLIENT_DEINIT) &&(rsi_driver_cb->nwk_cb->nwk_callbacks.smtp_client_delete_response_handler != NULL))
        {
          //! Call asynchronous response handler to indicate to host
          rsi_driver_cb->nwk_cb->nwk_callbacks.smtp_client_delete_response_handler(status, host_desc[5]);

          return RSI_SUCCESS;
        }
      }
      break;
    case RSI_WLAN_RSP_P2P_CONNECTION_REQUEST:
      {
        if(rsi_wlan_cb->callback_list.wlan_wfd_connection_request_notify_handler != NULL)
        {
          //! Call asynchronous response handler to indicate to host
          rsi_wlan_cb->callback_list.wlan_wfd_connection_request_notify_handler(status, payload, payload_length);
        }
      }
      break;
    case RSI_WLAN_RSP_WFD_DEVICE:
      {
        if(rsi_wlan_cb->callback_list.wlan_wfd_discovery_notify_handler != NULL)
        {
          //! Call asynchronous response handler to indicate to host
          rsi_wlan_cb->callback_list.wlan_wfd_discovery_notify_handler(status, payload, payload_length);
        }
      }
      break;
    case RSI_WLAN_RSP_CLIENT_CONNECTED:
      {
        if(rsi_wlan_cb->callback_list.stations_connect_notify_handler != NULL)
        {
          //! Call asynchronous response handler to indicate to host
          rsi_wlan_cb->callback_list.stations_connect_notify_handler(status, payload, payload_length);
        }
      }
      break;
    case RSI_WLAN_RSP_CLIENT_DISCONNECTED:
      {
        if(rsi_wlan_cb->callback_list.stations_disconnect_notify_handler != NULL)
        {
          //! Call asynchronous response handler to indicate to host
          rsi_wlan_cb->callback_list.stations_disconnect_notify_handler(status, payload, payload_length);
        }
      }
      break;
    case RSI_WLAN_RSP_SCAN:
      {

        //! update state  wlan_cb state to scan done
        if(status == RSI_SUCCESS)
        {
          rsi_wlan_cb->state = RSI_WLAN_STATE_SCAN_DONE;

          if(rsi_driver_cb->wlan_cb->callback_list.wlan_scan_response_handler != NULL)
          {

            //! Call asynchronous response handler to indicate to host
            rsi_wlan_cb->callback_list.wlan_scan_response_handler(status, payload, payload_length);

            return RSI_SUCCESS;
          }
          else
          {
            //! check the length of application buffer and copy scan results
            if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
            {
              copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length) : (rsi_wlan_cb->app_buffer_length);
              memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
              rsi_wlan_cb->app_buffer = NULL;
            }
          }
        }
      }
      break;
    case RSI_WLAN_RSP_BG_SCAN:
      {
        if(status == RSI_SUCCESS)
        {
          //! check the length of application buffer and copy bg scan results
          if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
          {
            copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length) : (rsi_wlan_cb->app_buffer_length);
            memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
            rsi_wlan_cb->app_buffer = NULL;
          }
        }
      }
      break;
    case RSI_WLAN_RSP_JOIN:
      {


        //! update state  wlan_cb state to connected
        if(status == RSI_SUCCESS)
        {
          rsi_wlan_cb->state = RSI_WLAN_STATE_CONNECTED;

          //! Check for Auto config done
          if(host_desc[15] & RSI_AUTO_CONFIG_DONE)
          {
            //! Set auto config state to done
            rsi_driver_cb->wlan_cb->auto_config_state = RSI_WLAN_STATE_AUTO_CONFIG_DONE;

            if(rsi_driver_cb->wlan_cb->callback_list.auto_config_rsp_handler)
            {
              rsi_driver_cb->wlan_cb->callback_list.auto_config_rsp_handler(status, RSI_AUTO_CONFIG_DONE);
            }
          }

        }
        else
        {

          //!Reset powe save mode as join fails
          common_cb->power_save.current_ps_mode = 0;

          //! Reset module state as join fails
          common_cb->power_save.module_state = 0;

          if(status != RSI_WPS_NOT_SUPPORTED)
          {
            rsi_wlan_cb->state = RSI_WLAN_STATE_BAND_DONE;

            if(rsi_wlan_cb->expected_response != cmd_type)
            {
              if(rsi_wlan_cb->callback_list.join_fail_handler != NULL)
              {
                //! Call asynchronous response handler to indicate to host
                rsi_wlan_cb->callback_list.join_fail_handler(status, payload, payload_length);
              }

              //! Clear all sockets
              //rsi_clear_sockets(RSI_CLEAR_ALL_SOCKETS);

              if((rsi_wlan_check_waiting_socket_cmd() || rsi_wlan_check_waiting_wlan_cmd()) && (wlan_pkt_pending == 0))
              {
                //! Clear expected response
                rsi_wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;

                //! Post the semaphore which is waiting on socket send
                rsi_semaphore_post(&rsi_driver_cb->wlan_cb->wlan_sem);

                return RSI_SUCCESS;
              }

            }
          }
        }
		if(rsi_driver_cb->wlan_cb->callback_list.wlan_join_response_handler != NULL)
        {
          //! Call asynchronous response handler to indicate to host
          rsi_wlan_cb->callback_list.wlan_join_response_handler(status, payload, payload_length);

          //! Clear expected response
          rsi_wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;

          return RSI_SUCCESS;
        }
      }
      break;
    case RSI_WLAN_RSP_IPCONFV6:
    case RSI_WLAN_RSP_IPCONFV4:
      {
        //! if success, update state  wlan_cb state to scan done
        if(status == RSI_SUCCESS)
        {
          if(cmd_type == RSI_WLAN_RSP_IPCONFV4)
          {
            rsi_wlan_cb->state = RSI_WLAN_STATE_IP_CONFIG_DONE;
          }
          else
          {
            rsi_wlan_cb->state = RSI_WLAN_STATE_IPV6_CONFIG_DONE;
          }

          //! Check for Auto config done
          if(host_desc[15] & RSI_AUTO_CONFIG_DONE)
          {
            //! Set auto config state to done
            rsi_wlan_cb->auto_config_state = RSI_WLAN_STATE_AUTO_CONFIG_DONE;

            //! Call auto config response handler
            if(rsi_wlan_cb->callback_list.auto_config_rsp_handler)
            {
              rsi_wlan_cb->callback_list.auto_config_rsp_handler(status, RSI_AUTO_CONFIG_DONE);
            }
          }

          //! check the length of application buffer and copy scan results
          if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
          {
            copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length) : (rsi_wlan_cb->app_buffer_length);
            memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
            rsi_wlan_cb->app_buffer = NULL;
          }
        }
        else
        {
          if(rsi_wlan_cb->expected_response != cmd_type)
          {
            if(rsi_wlan_cb->callback_list.ip_renewal_fail_handler != NULL)
            {
              //! Call asynchronous response handler to indicate to host
              rsi_wlan_cb->callback_list.ip_renewal_fail_handler(status, payload, payload_length);
            }

            //! Clear all sockets info
            //rsi_clear_sockets(RSI_CLEAR_ALL_SOCKETS);

            if(rsi_wlan_check_waiting_socket_cmd()  && (wlan_pkt_pending == 0))
            {
              //! Clear expected response
              rsi_wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;

              //! Post the semaphore which is waiting on socket send
              rsi_semaphore_post(&rsi_driver_cb->wlan_cb->wlan_sem);

              return RSI_SUCCESS;
            }
          }
        }
      }
      break;
    case RSI_WLAN_RSP_GET_RANDOM:
      {
        //! if success, update state  wlan_cb state to scan done
        if(status == RSI_SUCCESS)
        {
          //! check the length of application buffer and copy scan results
          if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
          {
            copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length) : (rsi_wlan_cb->app_buffer_length);
            memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
          }
        }
        rsi_wlan_cb->app_buffer = NULL;
      }
      break;
    case RSI_WLAN_RSP_PING_PACKET:
      {
        if(rsi_wlan_cb->callback_list.wlan_ping_response_handler != NULL)
        {
          //! Call asynchronous response handler to indicate to host
          rsi_wlan_cb->callback_list.wlan_ping_response_handler(status, payload, payload_length);

          return RSI_SUCCESS;
        }
      }
      break;
    case RSI_WLAN_RSP_RX_STATS:
      {
        if(rsi_wlan_cb->callback_list.wlan_receive_stats_response_handler != NULL)
        {
          //! Call asynchronous response handler to indicate to host
          rsi_wlan_cb->callback_list.wlan_receive_stats_response_handler(status, payload, payload_length);

          return RSI_SUCCESS;
        }
      }
      break;
    case RSI_WLAN_RSP_DISCONNECT:
      {
        //! if success, update state  wlan_cb state to band done
        if(status == RSI_SUCCESS)
        {
          rsi_wlan_cb->state = RSI_WLAN_STATE_BAND_DONE;

          //!Reset powe save mode as Disconnect received
          common_cb->power_save.current_ps_mode = 0;

          //! Reset module state as Disconnect received
          common_cb->power_save.module_state = 0;

          //! Clear all sockets
          //rsi_clear_sockets(RSI_CLEAR_ALL_SOCKETS);
        }
      }
      break;

    case RSI_WLAN_RSP_SOCKET_CREATE:
      {
        // if(status == RSI_SUCCESS)
        // {
        //   rsi_rsp_socket_create_t *socket_rsp = (rsi_rsp_socket_create_t *)payload;

        //   //! Get socket descriptor
        //   sock_id = rsi_bytes2R_to_uint16(socket_rsp->socket_id);

        //   //! Get waiting socket ID
        //   sockID = rsi_wlan_cb->waiting_socket_id;

        //   //! Save socket id
        //   rsi_socket_pool[sockID].sock_id = sock_id;
        // }
      }
      break;
    case RSI_WLAN_RSP_CONN_ESTABLISH:
      {
        // if(status == RSI_SUCCESS)
        // {
        //   rsi_rsp_ltcp_est_t *ltcp = (rsi_rsp_ltcp_est_t *)payload;

        //   //! Get IP version
        //   ip_version = rsi_bytes2R_to_uint16(ltcp->ip_version);

        //   //! Get socket descriptor from response
        //   sock_id = rsi_bytes2R_to_uint16(ltcp->socket_id);

        //   //! Get waiting socket ID
    		  // sockID = rsi_get_application_socket_descriptor(sock_id);

        //   //! Save socket id
        //   rsi_socket_pool[sockID].sock_id = sock_id;

        //   //! Save destination port number
        //   rsi_socket_pool[sockID].destination_port = rsi_bytes2R_to_uint16(ltcp->dest_port);

        //   //! Save destination IP addess
        //   if(ip_version == 4)
        //   {
        //     memcpy(rsi_socket_pool[sockID].destination_ip_addr.ipv4, ltcp->dest_ip_addr.ipv4_address, RSI_IPV4_ADDRESS_LENGTH);
        //   }
        //   else
        //   {
        //     memcpy(rsi_socket_pool[sockID].destination_ip_addr.ipv6, ltcp->dest_ip_addr.ipv6_address, RSI_IPV6_ADDRESS_LENGTH);
        //   }
        // }
      }
      break;
    case RSI_WLAN_RSP_REMOTE_TERMINATE:
      {
        // rsi_rsp_socket_close_t *close = (rsi_rsp_socket_close_t *)payload;

        // if(rsi_wlan_cb->callback_list.remote_socket_terminate_handler != NULL)
        // {
        //   //! Call asynchronous response handler to indicate to host
        //   rsi_wlan_cb->callback_list.remote_socket_terminate_handler(status, payload, payload_length);
        // }

        // //! Get application socket ID
        // sockID = rsi_get_application_socket_descriptor(rsi_bytes2R_to_uint16(close->socket_id));

        // if(sockID < 0 || (sockID >= RSI_NUMBER_OF_SOCKETS))
        // {
        //   break;
        // }
        // //! Clear socket info
        // // rsi_clear_sockets(sockID);

        // //! Need to set reason for remote terminate
        // rsi_wlan_set_status(RSI_ERROR_REMOTE_TERNIMATE);

        // if(rsi_wlan_check_waiting_socket_cmd())
        // {
        //   sock_id = rsi_driver_cb->wlan_cb->waiting_socket_id;
        //   if(sock_id == sockID && (wlan_pkt_pending == 0))
        //   {
        //     //! Clear expected response
        //     rsi_wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;

        //     //! Post the semaphore which is waiting on socket send
        //     rsi_semaphore_post(&rsi_driver_cb->wlan_cb->wlan_sem);

        //     return RSI_FAILURE;
        //   }
        // }

      }
      break;
    case RSI_WLAN_RSP_SOCKET_CLOSE:
      {
        // if(status == RSI_SUCCESS)
        // {
        //   rsi_rsp_socket_close_t *close = (rsi_rsp_socket_close_t *)payload;

        //   if(rsi_bytes2R_to_uint16(close->socket_id))
        //   {
        //     //! Normal socket close
        //     //! Get socket descriptor
        //     sockID = rsi_get_application_socket_descriptor(rsi_bytes2R_to_uint16(close->socket_id));
        //   }
        //   else
        //   {
        //     //! Port based LTCP socket close
        //     sockID = RSI_LTCP_PORT_BASED_CLOSE;
        //   }

        //   //! Clear socket info
        //   rsi_clear_sockets(sockID);
        // }
      }
      break;

    case RSI_WLAN_RSP_IPV4_CHANGE:
      {
        if(rsi_wlan_cb->callback_list.ip_change_notify_handler != NULL)
        {
          //! Call asynchronous response handler to indicate to host
          rsi_wlan_cb->callback_list.ip_change_notify_handler(status, payload, payload_length);
        }

        //! Clear all sockets info
        //rsi_clear_sockets(RSI_CLEAR_ALL_SOCKETS);

        if(rsi_wlan_check_waiting_socket_cmd() && (wlan_pkt_pending == 0))
        {
          //! Clear expected response
          rsi_wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;

          //! Post the semaphore which is waiting on socket send
          rsi_semaphore_post(&rsi_driver_cb->wlan_cb->wlan_sem);

          return RSI_SUCCESS;
        }

      }
      break;
    case RSI_WLAN_RSP_HTTP_CLIENT_POST:
    case RSI_WLAN_RSP_HTTP_CLIENT_GET:
      {
        if(rsi_driver_cb->nwk_cb->nwk_callbacks.http_client_response_handler != NULL)
        {
          if(status == RSI_SUCCESS)
          {
            //! more data
            uint32_t moredata = rsi_bytes4R_to_uint32(payload);

            //! Adjust payload length
            payload_length -= RSI_HTTP_OFFSET;

            //! Call asynchronous response handler to indicate to host
             rsi_driver_cb->nwk_cb->nwk_callbacks.http_client_response_handler(status, (payload + RSI_HTTP_OFFSET), payload_length, moredata);
          }
          else
          {
            //! Call asynchronous response handler to indicate to host
            rsi_driver_cb->nwk_cb->nwk_callbacks.http_client_response_handler(status, payload, payload_length, 0);

          }
        }
        return RSI_SUCCESS;
      }

    case RSI_WLAN_RSP_HTTP_CLIENT_POST_DATA:
      {
        if(rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_http_client_post_data_response_handler != NULL)
        {
          if(status == RSI_SUCCESS)
          {
            //! more data
            uint32_t moredata = rsi_bytes4R_to_uint32(payload);

            //! Adjust payload length
            payload_length -= RSI_HTTP_OFFSET;

            //! Call asynchronous response handler to indicate to host
             rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_http_client_post_data_response_handler(status, (payload + RSI_HTTP_OFFSET), payload_length, moredata);
          }
          else
          {
            //! Call asynchronous response handler to indicate to host
            rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_http_client_post_data_response_handler(status, payload, payload_length, 0);

          }
        }
        return RSI_SUCCESS;
      }
     break;
    case RSI_WLAN_RSP_DNS_QUERY:
      {
        if(status == RSI_SUCCESS)
        {
          if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
          {
            copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length) : (rsi_wlan_cb->app_buffer_length);
            memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
            rsi_wlan_cb->app_buffer = NULL;
          }
        }
      }
      break;
    case RSI_WLAN_RSP_FTP:
      {
        rsi_ftp_file_rsp_t *ftp_file_rsp = (rsi_ftp_file_rsp_t *)payload;


        if(ftp_file_rsp->command_type == RSI_FTP_FILE_READ)
        {
          if(status == RSI_SUCCESS)
          {
            //! Call asynchronous response handler to indicate to host
            rsi_driver_cb->nwk_cb->nwk_callbacks.ftp_file_read_call_back_handler(status, ftp_file_rsp->data_content, ftp_file_rsp->data_length, !(ftp_file_rsp->more));
          }
          else
          {
            //! Call asynchronous response handler to indicate to host
            rsi_driver_cb->nwk_cb->nwk_callbacks.ftp_file_read_call_back_handler(status, NULL, 0 , 1);
          }
          return RSI_SUCCESS;
        }
        else if(ftp_file_rsp->command_type == RSI_FTP_DIRECTORY_LIST)
        {
          if(status ==  RSI_SUCCESS)
          {
            //! Call asynchronous response handler to indicate to host
            rsi_driver_cb->nwk_cb->nwk_callbacks.ftp_directory_list_call_back_handler(status, ftp_file_rsp->data_content, ftp_file_rsp->data_length, !(ftp_file_rsp->more));
          }
          else
          {
            rsi_driver_cb->nwk_cb->nwk_callbacks.ftp_directory_list_call_back_handler(status, NULL ,0,1);

          }
          return RSI_SUCCESS;
        }

      }
      break;


    case RSI_WLAN_RSP_TCP_ACK_INDICATION:
      {
        // rsi_rsp_tcp_ack_t *tcp_ack = (rsi_rsp_tcp_ack_t *)payload;

        // //! Get the socket ID from response and get application sockID from it
        // sockID = rsi_get_application_socket_descriptor(tcp_ack->socket_id);

        // if(sockID < 0 || (sockID >= RSI_NUMBER_OF_SOCKETS))
        // {
        //   break;
        // }

        // //! Get the length from response
        // length = rsi_bytes2R_to_uint16(tcp_ack->length);

        // //! Calculate the buffers can be freed with this length
        // buffers_freed = calculate_buffers_required(rsi_socket_pool[sockID].sock_type, length);

        // //! Increase the current available buffer count
        // rsi_socket_pool[sockID].current_available_buffer_count += buffers_freed;


        // //! If current count exceeds max count
        // if(rsi_socket_pool[sockID].current_available_buffer_count > rsi_socket_pool[sockID].max_available_buffer_count)
        // {
        //   rsi_socket_pool[sockID].current_available_buffer_count = rsi_socket_pool[sockID].max_available_buffer_count;
        // }

        // //! Call registered callback
        // if(rsi_driver_cb->nwk_cb->nwk_callbacks.data_transfer_complete_handler)
        // {
        //   rsi_driver_cb->nwk_cb->nwk_callbacks.data_transfer_complete_handler(sockID, length);
        // }

      }
      break;
    case RSI_WLAN_RSP_WIRELESS_FWUP_OK:
    case RSI_WLAN_RSP_WIRELESS_FWUP_DONE:
      {
        if(rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_wireless_fw_upgrade_handler != NULL)
        {
          rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_wireless_fw_upgrade_handler(cmd_type, status);
        }
      }
      break;

    case RSI_WLAN_RSP_DHCP_USER_CLASS:
      {
	      if(rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_dhcp_usr_cls_rsp_handler != NULL)
	      {
		      rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_dhcp_usr_cls_rsp_handler(status);
	      }
      }
      break;

    case RSI_WLAN_RSP_JSON_UPDATE:
      {
        // uint8_t   filename[24] =  {'\0'};

        // if(rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_json_object_update_handler != NULL)
        // {
        //   //! Extract the filename from the received json object data payload
        //   rsi_json_extract_filename(payload, filename);

        //   rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_json_object_update_handler(filename, payload, strlen((const char *)payload), RSI_SUCCESS);
        // }
      }
      break;
    case RSI_WLAN_RSP_SOCKET_READ_DATA:
      {
        //! TODO: Need to post particular socket semaphore for each socket
        //! we get the socket id in the response payload if it is error
        //! Should be taken after implementing semophore for each socket
      }
      break;
    case RSI_WLAN_RSP_JSON_EVENT:
      {
        if(rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_json_object_event_handler != NULL)
        {
          rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_json_object_event_handler(RSI_SUCCESS, payload, strlen((const char *)payload));
        }
      }
      break;
    case RSI_WLAN_RSP_OTA_FWUP:
      {
        if(rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_ota_fw_up_response_handler != NULL)
        {
          if(status == RSI_SUCCESS)
          {
            rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_ota_fw_up_response_handler(status, 0);
          }
          else
          {
            rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_ota_fw_up_response_handler(status, *(uint16_t *)payload);
          }
        }

      }
      break;
    case RSI_WLAN_RSP_HTTP_CLIENT_PUT:
      {
        if(rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_http_client_put_response_handler != NULL)
        {
          if(status == RSI_SUCCESS)
          {
            uint8_t end_of_file = 0;
            uint8_t http_cmd_type = *payload;
            rsi_http_client_put_pkt_rsp_t *put_pkt_rsp = (rsi_http_client_put_pkt_rsp_t *)payload;

            //! Check for HTTP_CLIENT_PUT_PKT command
            if(http_cmd_type == HTTP_CLIENT_PUT_PKT)
            {
              end_of_file = put_pkt_rsp->end_of_file;
            }

            //! Call HTTP client PUT asynchronous response handler to indicate to host
            rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_http_client_put_response_handler(status, (uint8_t)*payload, end_of_file);
          }
          else
          {
            //! Call POP3 client asynchronous response handler to indicate to host
            rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_http_client_put_response_handler(status, host_desc[5], 0);
          }

          return RSI_SUCCESS;

        }
      }
      break;
	case RSI_WLAN_RSP_GET_PROFILE:
      {
        //! check the length of application buffer and copy get profile response
        if((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0))
        {
          copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length) : (rsi_wlan_cb->app_buffer_length);
          memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
          rsi_wlan_cb->app_buffer = NULL;
        }
      }
      break;
    case RSI_WLAN_RSP_POP3_CLIENT:
      {
       if(rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_pop3_client_mail_response_handler != NULL)
       {
         if(status == RSI_SUCCESS)
         {
           //! Call POP3 client asynchronous response handler to indicate to host
           rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_pop3_client_mail_response_handler(status, (uint8_t)*payload, payload);
         }
         else
         {
           //! Call POP3 client asynchronous response handler to indicate to host
           rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_pop3_client_mail_response_handler(status, host_desc[5], payload);
         }

         return RSI_SUCCESS;
       }
      }
    default:
      break;
  }
  //! check wlan_cb for any task is waiting for response
  if(rsi_wlan_cb->expected_response == cmd_type)
  {
    //! Clear expected response
    rsi_wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;

    //! signal the wlan semaphore
    rsi_semaphore_post(&rsi_wlan_cb->wlan_sem);
  }

  return RSI_SUCCESS;
}


/*==============================================*/
/**
 * @fn          rsi_wlan_radio_init()
 * @brief       To initalize wlan radio paramenters
 * @return      int32_t
 *              0  =  success
 *              <0 = failure
 * @section description
 * This function is used to initialize radio parameters.
 *
 */
int32_t rsi_wlan_radio_init()
{
  rsi_pkt_t       *pkt;
  int8_t          status = RSI_SUCCESS;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! take lock on wlan control block
  rsi_mutex_lock(&wlan_cb->wlan_mutex);

  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&wlan_cb->wlan_mutex);

    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  switch(wlan_cb->state)
  {
    case RSI_WLAN_STATE_OPERMODE_DONE:
      {
        if(wlan_cb->field_valid_bit_map & RSI_SET_MAC_BIT)
        {
          //! Memset data
          memset(&pkt->data,0,sizeof(rsi_req_mac_address_t));

          //! Memcpy data
          memcpy(&pkt->data, wlan_cb->mac_address, sizeof(rsi_req_mac_address_t));

          //! send set mac command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MAC_ADDRESS, pkt);

          if(status != RSI_SUCCESS)
          {
            //! free the packet
            rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

            //! unlock mutex
            rsi_mutex_unlock(&wlan_cb->wlan_mutex);

            //! Return the status if error in sending command occurs
            return status;
          }
        }

#if RSI_DEBUG_PRINT_ENABLE
        //! Memset data
        memset(&pkt->data,0,sizeof(rsi_req_debug_print_t));
        //! send debug print command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DEBUG_PRINTS, pkt);
        if(status != RSI_SUCCESS)
        {
          //! If DEBUG PRINT command fails

          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

          //! unlock mutex
          rsi_mutex_unlock(&wlan_cb->wlan_mutex);

          //! Return the status if error in sending command occurs
          return status;
        }
#endif

        //! Memset data
        memset(&pkt->data,0,sizeof(rsi_req_band_t));

        //! send band command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BAND, pkt);

        if(status != RSI_SUCCESS)
        {
          //! If BAND command fails

          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

          //! unlock mutex
          rsi_mutex_unlock(&wlan_cb->wlan_mutex);

          //! Return the status if error in sending command occurs
          return status;
        }
      }
    case RSI_WLAN_STATE_BAND_DONE:
      {

        //! send init command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_INIT, pkt);

        if(status != RSI_SUCCESS)
        {
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

          //! unlock mutex
          rsi_mutex_unlock(&wlan_cb->wlan_mutex);

          //! Return the status if error in sending command occurs
          return status;
        }
#if RSI_SET_REGION_AP_SUPPORT
        if(wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE)
        {
          //! Memset data
          memset(&pkt->data,0,sizeof(rsi_req_set_region_ap_t));

          //! send set region AP command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION_AP, pkt);

          if(status != RSI_SUCCESS)
          {
            //! free the packet
            rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

            //! unlock mutex
            rsi_mutex_unlock(&wlan_cb->wlan_mutex);

            //! Return the status if error in sending command occurs
            return status;
          }
        }
#endif
#if RSI_SET_REGION_SUPPORT
        if((wlan_cb->opermode == RSI_WLAN_CLIENT_MODE)||(wlan_cb->opermode == RSI_WLAN_ENTERPRISE_CLIENT_MODE))
        {
          //! Memset data
          memset(&pkt->data,0,sizeof(rsi_req_set_region_t));

          //! send set region command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION, pkt);

          if(status != RSI_SUCCESS)
          {
            //! free the packet
            rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

            //! unlock mutex
            rsi_mutex_unlock(&wlan_cb->wlan_mutex);

            //! Return the status if error in sending command occurs
            return status;
          }
        }
#endif
      }
    default:
      {

      }
  }
  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}

/*==============================================*/
/**
 *
 * @fn         uint32_t rsi_check_wlan_state(void)
 * @brief      Return wlan block status
 * @param[in]  None
 * @param[out] None
 * @return     Wlan block status
 *
 *
 * @section description
 * This function returns wlan block status
 *
 *
 */
uint32_t rsi_check_wlan_state(void)
{
  return rsi_driver_cb->wlan_cb->state;
}

/*==============================================*/
/**
 * @fn          void rsi_wlan_set_status(uint16_t status)
 * @brief       Sets the wlan status
 * @param[in]   status, status value to be set
 * @param[out]  None
 * @return      void
 *
 * @section description
 * This function sets wlan status
 *
 */
void rsi_wlan_set_status(int32_t status)
{
  rsi_driver_cb->wlan_cb->status = status;
}


/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_check_waiting_socket_cmd(void)
 * @brief       check blocking cmd
 * @param[in]   None
 * @param[out]  None
 * @return      void
 *
 * @section description
 * This function sets wlan status
 *
 */
int32_t rsi_wlan_check_waiting_socket_cmd(void)
{

  //! Get wlan cb struct pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if((wlan_cb->expected_response == RSI_WLAN_RSP_SOCKET_ACCEPT) ||
      (wlan_cb->expected_response == RSI_WLAN_RSP_SOCKET_CREATE) ||
      (wlan_cb->expected_response == RSI_WLAN_RSP_SOCKET_READ_DATA) ||
      (wlan_cb->expected_response == RSI_WLAN_RSP_SOCKET_CLOSE)  ||
      (wlan_cb->expected_response == RSI_WLAN_RSP_CONN_ESTABLISH))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_check_waiting_wlan_cmd(void)
 * @brief       check blocking cmd
 * @param[in]   None
 * @param[out]  None
 * @return      void
 *
 * @section description
 * This function sets wlan status
 *
 */
int32_t rsi_wlan_check_waiting_wlan_cmd(void)
{
  //! Get wlan cb struct pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if((wlan_cb->expected_response == RSI_WLAN_RSP_IPCONFV4) ||
      (wlan_cb->expected_response == RSI_WLAN_RSP_DISCONNECT) ||
      (wlan_cb->expected_response == RSI_WLAN_RSP_IPCONFV6) ||
      (wlan_cb->expected_response == RSI_WLAN_RSP_ROAM_PARAMS)  ||
      (wlan_cb->expected_response == RSI_WLAN_RSP_BG_SCAN)||
      (wlan_cb->expected_response == RSI_WLAN_RSP_DNS_SERVER_ADD)||
      (wlan_cb->expected_response == RSI_WLAN_RSP_SET_MULTICAST_FILTER)||
      (wlan_cb->expected_response == RSI_WLAN_RSP_DNS_QUERY))

  {
    return 1;
  }
  else
  {
    return 0;
  }

}
/*==============================================*/
/**
 * @fn         void rsi_wlan_process_raw_data(rsi_pkt_t *pkt)
 * @brief      This function is used to receive raw data packet
 * @param[in]  pkt, pointer to rx pkt
 * @param[out] None
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function is used to receive raw data packet from module
 *
 *
 */

void rsi_wlan_process_raw_data(rsi_pkt_t *pkt)
{
  uint8_t  *host_desc;
  uint8_t  *payload;
  uint16_t payload_length,status=0;

  //! Get wlan cb struct pointer
  rsi_wlan_cb_t *rsi_wlan_cb = rsi_driver_cb->wlan_cb;

  //! Get host descriptor
  host_desc = pkt->desc;

  //! Get payload pointer
  payload = pkt->data;

  //! Get payoad length
  payload_length = (rsi_bytes2R_to_uint16(host_desc) & 0xFFF);


  //! check status
  status  = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

  //! update the status in wlan_cb
  rsi_wlan_set_status(status);



  if(rsi_wlan_cb->callback_list.wlan_data_receive_handler != NULL)
  {
    //! Call asynchronous data receive handler to indicate to host
    rsi_wlan_cb->callback_list.wlan_data_receive_handler( 0, payload, payload_length);
  }

}


/*==============================================*/
/**
 * @fn          void rsi_wlan_packet_transfer_done(rsi_pkt_t *pkt)
 * @brief       Handles packet transfer completion which has an asyncronous response
 * @param[in]   pkt, pointer to packet
 * @param[out]  None
 * @return      void
 *
 * @section description
 * This function handles packet transfer completion
 *
 */
void rsi_wlan_packet_transfer_done(rsi_pkt_t *pkt)
{
  if(rsi_driver_cb->wlan_cb->expected_response == RSI_WLAN_RSP_ASYNCHRONOUS)
  {
    //! Set wlan status as success
    rsi_wlan_set_status(RSI_SUCCESS);


    //! Post the semaphore which is waiting on socket send
    rsi_semaphore_post(&rsi_driver_cb->wlan_cb->wlan_sem);

  }
}
