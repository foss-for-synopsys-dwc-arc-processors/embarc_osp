/**
 * @file     rsi_wlan_apis.c
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
 *  @brief : This file contains wlan API's which needs to be called from application
 *
 *  @section Description  This file contains wlan API called from application
 *
 *
 */
#include "rsi_driver.h"


/*==============================================*/
/**
 * @fn         int32_t rsi_wlan_scan(int8_t *ssid, uint8_t chno, rsi_rsp_scan_t *result, uint32_t length)
 * @brief      Scans for the access points
 * @param[in]  ssid , ssid of the access point to scan
 * @param[in]  chno , channel number of the access point
 * @param[in]  result , buffer address provided by the application to fill the scan response
 * @param[in]  length , Length of the buffer provided by the application
 * @return     int32_t
 *             0  =  success
 *             <0 = failure
 * @section description
 * This function is used to scan the access points available and posts the scan response to
 * application. Application should call this api to get the scan results.
 *
 */
int32_t rsi_wlan_scan(int8_t *ssid, uint8_t chno, rsi_rsp_scan_t *result,uint32_t length)
{
  int8_t          status = RSI_SUCCESS;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! take lock on wlan control block
  rsi_mutex_lock(&wlan_cb->wlan_mutex);

  //! attach the buffer given by user
  wlan_cb->app_buffer = (uint8_t *)result;

  //! length of the buffer provided by user
  wlan_cb->app_buffer_length = length;

  //! call async scan to perform actual scan
  status = rsi_wlan_scan_async(ssid, chno, NULL);

  if(status != RSI_SUCCESS)
  {
    //! unlock mutex
    rsi_mutex_unlock(&wlan_cb->wlan_mutex);

    //! Return the status if error in sending command occurs
    return status;
  }

  //! Wait on wlan semaphore
  rsi_semaphore_wait(&rsi_driver_cb->wlan_cb->wlan_sem, RSI_WAIT_FOREVER);

  //! get wlan/network command response status
  status =  rsi_wlan_get_status();

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);


  //! Return status
  return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_wlan_scan_async(int8_t *ssid, uint8_t chno, void (*callback)(uint16_t status, const uint8_t *buffer, const uint16_t length))
 * @brief      Scans for the access points
 * @param[in]  ssid , ssid of the access point to scan
 * @param[in]  chno , channel number of the access point
 * @return     int32_t
 *             0  =  success
 *             <0 = failure
 * @section description
 * This function is used to scan the access points available and posts the scan response to
 * application. Application should call this api to get the scan results.
 *
 */

int32_t rsi_wlan_scan_async(int8_t *ssid, uint8_t chno, void (*scan_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length))

{
  rsi_pkt_t       *pkt;
  rsi_req_scan_t  *scan;
  int8_t          status = RSI_SUCCESS;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! check to aquire lock or
  if(scan_response_handler != NULL)
  {
    //! take lock on wlan control block
    rsi_mutex_lock(&wlan_cb->wlan_mutex);
  }

  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
  if(scan_response_handler != NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&wlan_cb->wlan_mutex);
  }

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

            if(scan_response_handler != NULL)
            {
              //! unlock mutex
              rsi_mutex_unlock(&wlan_cb->wlan_mutex);
            }

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
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

          if(scan_response_handler != NULL)
          {
            //! unlock mutex
            rsi_mutex_unlock(&wlan_cb->wlan_mutex);
          }

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

            if(scan_response_handler != NULL)
            {
              //! unlock mutex
              rsi_mutex_unlock(&wlan_cb->wlan_mutex);
            }

          //! Return the status if error in sending command occurs
          return status;
        }
      }
    case RSI_WLAN_STATE_BAND_DONE:
      {
#if RSI_WLAN_CONFIG_ENABLE
        //! send Configuration request command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CONFIG, pkt);

        if(status != RSI_SUCCESS)
        {
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

            if(scan_response_handler != NULL)
            {
              //! unlock mutex
              rsi_mutex_unlock(&wlan_cb->wlan_mutex);
            }

	      //! Return the status if error in sending command occurs
          return status;
    	}
#endif
    	//! send init command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_INIT, pkt);

        if(status != RSI_SUCCESS)
        {
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

            if(scan_response_handler != NULL)
            {
              //! unlock mutex
              rsi_mutex_unlock(&wlan_cb->wlan_mutex);
            }

          //! Return the status if error in sending command occurs
          return status;
        }
#if RSI_SET_REGION_SUPPORT

        //! Memset data
        memset(&pkt->data,0,sizeof(rsi_req_set_region_t));

        //! send set region command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION, pkt);

        if(status != RSI_SUCCESS)
        {
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

            if(scan_response_handler != NULL)
            {
              //! unlock mutex
              rsi_mutex_unlock(&wlan_cb->wlan_mutex);
            }

          //! Return the status if error in sending command occurs
          return status;
        }
#endif

#if RSI_TIMEOUT_SUPPORT
		//! Memset data
		memset(&pkt->data,0,sizeof(rsi_req_timeout_t));

		//! send set region command
		status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TIMEOUT, pkt);

		if(status != RSI_SUCCESS)
		{
			//! free the packet
			rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

			if(scan_response_handler != NULL)
			{
				//! unlock mutex
				rsi_mutex_unlock(&wlan_cb->wlan_mutex);
			}

			//! Return the status if error in sending command occurs
			return status;
		}
#endif
      }
    case RSI_WLAN_STATE_SCAN_DONE:
    case RSI_WLAN_STATE_INIT_DONE:
    case RSI_WLAN_STATE_CONNECTED:
      {
        //! FIll SCAN parameters
        scan = (rsi_req_scan_t *)pkt->data;

        //! memset the pkt
        memset(&pkt->data,0,sizeof(rsi_req_scan_t));

        //! copy the channel number
        rsi_uint32_to_4bytes(scan->channel, chno);

        //! copy SSID given by the application
        if(ssid != NULL)
        {
          rsi_strcpy(scan->ssid, ssid);
        }

        //! Magic number to set asyncronous reswponse
        wlan_cb->expected_response = RSI_WLAN_RSP_ASYNCHRONOUS ;

        if(scan_response_handler != NULL )
        {
          //! Register scan response handler
          wlan_cb->callback_list.wlan_scan_response_handler = scan_response_handler;
        }

        //! send scan command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SCAN, pkt);
      }
      break;
    default:
      {
        //! Return the status if command given in driver invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
      }
      break;
  }

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  if(scan_response_handler != NULL)
  {
    //! release lock
    rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  }

  return status;
}

/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_connect(int8_t *ssid, rsi_security_mode_t sec_type, void *secret_key)
 * @brief       Connects to the AP
 * @param[in]   ssid , SSID of the access point to connect
 * @param[in]   sec_type , security type of the access point
 * @param[in]   secret_key  , PSK key of the access point
 * @return      int32_t
 *              0  =  success
 *              <0 = failure
 * @section description
 * This function is used to connect to the access point.
 *
 */

int32_t rsi_wlan_connect(int8_t *ssid, rsi_security_mode_t sec_type, void *secret_key)
{

  int8_t          status = RSI_SUCCESS;

  //! take lock on wlan control block
  rsi_mutex_lock(&rsi_driver_cb->wlan_cb->wlan_mutex);

  //! call async connect to call actual connect
  status = rsi_wlan_connect_async(ssid, sec_type, secret_key, NULL);

  if(status != RSI_SUCCESS)
  {
    //! unlock mutex
    rsi_mutex_unlock(&rsi_driver_cb->wlan_cb->wlan_mutex);

    //! Return the status if error in sending command occurs
    return status;
  }
  //! Wait on wlan semaphore
  rsi_semaphore_wait(&rsi_driver_cb->wlan_cb->wlan_sem, RSI_WAIT_FOREVER);

  //! get wlan/network command response status
  status =  rsi_wlan_get_status();

  if(status != RSI_SUCCESS)
  {
    //! unlock mutex
    rsi_mutex_unlock(&rsi_driver_cb->wlan_cb->wlan_mutex);

    //! Return the status if error in sending command occurs
    return status;
  }

  //! unlock mutex
  rsi_mutex_unlock(&rsi_driver_cb->wlan_cb->wlan_mutex);

  //! execute post connect commands
  status = rsi_wlan_execute_post_connect_cmds();

  return status;

}
/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_connect_async(int8_t *ssid, rsi_security_mode_t sec_type, void *secret_key,  void (*join_response_handler)(uint16_t status,const uint8_t *buffer, const uint16_t length))
 * @brief       Connects to the AP
 * @param[in]   ssid , SSID of the access point to connect
 * @param[in]   sec_type , security type of the access point
 * @param[in]   secret_key  , PSK key of the access point
 * @return      int32_t
 *              0  =  success
 *              <0 = failure
 * @section description
 * This function is used to connect to the access point.
 *
 */

int32_t rsi_wlan_connect_async(int8_t *ssid, rsi_security_mode_t sec_type, void *secret_key, void (*join_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length))
{
  rsi_eap_credentials_t *credentials;
  rsi_req_wps_method_t *wps_method;
  rsi_req_psk_t   *psk;
  rsi_wep_keys_t  *wep_keys;
  rsi_pkt_t       *pkt;
  rsi_req_scan_t  *scan;
  rsi_req_join_t  *join;
  rsi_req_eap_config_t *eap_config;
  int8_t      status = RSI_SUCCESS;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! Check whether PSK is provided or not for security modes
  if((sec_type != 0) && (secret_key == NULL) && (sec_type != RSI_WPS_PUSH_BUTTON) && (sec_type != RSI_USE_GENERATED_WPSPIN))
  {
    //! Throw error in case of invalid parameters
    return RSI_ERROR_INVALID_PARAM;
  }

  //! check whether module is in valid state range or not
  if(wlan_cb->state >= RSI_WLAN_STATE_CONNECTED)
  {
    //! Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  if((wlan_cb->opermode == RSI_WLAN_ENTERPRISE_CLIENT_MODE) && !((sec_type == RSI_WPA_EAP) ||(sec_type == RSI_WPA2_EAP)))
  {
    //! Throw error in case of invalid parameters
    return RSI_ERROR_INVALID_PARAM;
  }

  if(join_response_handler != NULL)
  {
    //! take lock on wlan control block
    rsi_mutex_lock(&wlan_cb->wlan_mutex);
  }

  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    if(join_response_handler != NULL)
    {
      //! unlock mutex
      rsi_mutex_unlock(&wlan_cb->wlan_mutex);
    }

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

            if(join_response_handler != NULL)
            {
              //! unlock mutex
              rsi_mutex_unlock(&wlan_cb->wlan_mutex);
            }

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
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

          if(join_response_handler != NULL)
          {
            //! unlock mutex
            rsi_mutex_unlock(&wlan_cb->wlan_mutex);
          }

          //! Return the status if error in sending command occurs
          return status;
        }
#endif

        //! send band command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BAND, pkt);
        if(status != RSI_SUCCESS)
        {
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

          if(join_response_handler != NULL)
          {
            //! unlock mutex
            rsi_mutex_unlock(&wlan_cb->wlan_mutex);
          }

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

          if(join_response_handler != NULL)
          {
            //! unlock mutex
            rsi_mutex_unlock(&wlan_cb->wlan_mutex);
          }

          //! Return the status if error in sending command occurs
          return status;
        }
#if RSI_SET_REGION_SUPPORT

        //! Memset data
        memset(&pkt->data,0,sizeof(rsi_req_set_region_t));

        //! send set region command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION, pkt);

        if(status != RSI_SUCCESS)
        {
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

          if(join_response_handler != NULL)
          {
            //! unlock mutex
            rsi_mutex_unlock(&wlan_cb->wlan_mutex);
          }

          //! Return the status if error in sending command occurs
          return status;
        }
#endif

#if RSI_TIMEOUT_SUPPORT
		//! Memset data
		memset(&pkt->data,0,sizeof(rsi_req_timeout_t));

		//! send set region command
		status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TIMEOUT, pkt);

		if(status != RSI_SUCCESS)
		{
			//! free the packet
			rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

			if(join_response_handler != NULL)
			{
				//! unlock mutex
				rsi_mutex_unlock(&wlan_cb->wlan_mutex);
			}

			//! Return the status if error in sending command occurs
			return status;
		}
#endif
      }
    case RSI_WLAN_STATE_INIT_DONE:
      {
#if RSI_WMM_PS_ENABLE
        //! memset the packet data
        memset(&pkt->data, 0, sizeof(rsi_wmm_ps_parms_t));

        //! send wmm parameters
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WMM_PS, pkt);

		        if(status != RSI_SUCCESS)
        {
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

          if(join_response_handler != NULL)
          {
            //! unlock mutex
            rsi_mutex_unlock(&wlan_cb->wlan_mutex);
          }

          //! Return the status if error in sending command occurs
          return status;
        }

#endif
        //! Send EAP command for EAP security
        if(wlan_cb->opermode == RSI_WLAN_ENTERPRISE_CLIENT_MODE)
        {

          credentials = (rsi_eap_credentials_t *)secret_key;

          //! memset the packet data
          memset(&pkt->data, 0, sizeof(rsi_req_eap_config_t));

          eap_config = (rsi_req_eap_config_t *)pkt->data;

          //! Copy user name
          memcpy(eap_config->user_identity, credentials->username, RSI_EAP_USER_NAME_LENGTH);

          //! Copy password
          memcpy(eap_config->password, credentials->password, RSI_EAP_PASSWORD_LENGTH);

          //! send eap config command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EAP_CONFIG, pkt);

          if(status != RSI_SUCCESS)
          {
            //! free the packet
            rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

            if(join_response_handler != NULL)
            {
              //! unlock mutex
              rsi_mutex_unlock(&wlan_cb->wlan_mutex);
            }

            //! Return the status if error in sending command occurs
            return status;
          }
        }

        //! give buffer as NULL, need not give scan results to host
        wlan_cb->app_buffer = NULL;

        //! length of the buffer is zero
        wlan_cb->app_buffer_length = 0;

        //! Send SCAN command
        scan = (rsi_req_scan_t *)pkt->data;

        //! memset the packet data
        memset(&pkt->data,0,sizeof(rsi_req_scan_t));

        //! copy ssid given by the application
        if(ssid != NULL)
        {
          rsi_strcpy(scan->ssid, ssid);
        }

        //! send scan command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SCAN, pkt);

        if(status != RSI_SUCCESS)
        {
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

          if(join_response_handler != NULL)
          {
            //! unlock mutex
            rsi_mutex_unlock(&wlan_cb->wlan_mutex);
          }

          //! Return the status if error in sending command occurs
          return status;
        }
      }
    case RSI_WLAN_STATE_SCAN_DONE:
      {
        if((sec_type == RSI_WPS_PIN) && (wlan_cb->opermode != 2))
        {

          wps_method = (rsi_req_wps_method_t *)pkt->data;

          //! Memset buffer
          memset(&pkt->data, 0, sizeof(rsi_req_wps_method_t));

          //!set configuration to validate wps pin
          rsi_uint16_to_2bytes(wps_method->wps_method, 1);

          //!set configuration to validate wps pin
          rsi_uint16_to_2bytes(wps_method->generate_pin, 0);

          //! Copy WPS Pin
          memcpy(wps_method->wps_pin, secret_key, RSI_WPS_PIN_LEN);

          //! send wps method command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WPS_METHOD, pkt);

          if(status != RSI_SUCCESS)
          {
            //! free the packet
            rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

            if(join_response_handler != NULL)
            {
              //! unlock mutex
              rsi_mutex_unlock(&wlan_cb->wlan_mutex);
            }

            //! Return the status if error in sending command occurs
            return status;
          }
        }
        else if(sec_type == RSI_WEP)
        {

          //! Memset buffer
          memset(&pkt->data, 0, sizeof(rsi_wep_keys_t));

          wep_keys = (rsi_wep_keys_t *)pkt->data;

          //! Copy wep keys
          memcpy(wep_keys, secret_key, sizeof(rsi_wep_keys_t));

          //! send psk command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_WEP_KEYS, pkt);

          if(status != RSI_SUCCESS)
          {
            //! free the packet
            rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

            if(join_response_handler != NULL)
            {
              //! unlock mutex
              rsi_mutex_unlock(&wlan_cb->wlan_mutex);
            }

            //! Return the status if error in sending command occurs
            return status;
          }

        }
        //! Send PSK command for PSK security
        else if(((sec_type == RSI_WPA)||(sec_type == RSI_WPA2)||(sec_type == RSI_WPA_WPA2_MIXED)||(sec_type == RSI_WPA_PMK)||(sec_type == RSI_WPA2_PMK)) && (wlan_cb->opermode != 2))
        {
          psk = (rsi_req_psk_t *)pkt->data;

          //! Memset buffer
          memset(&pkt->data, 0, sizeof(rsi_req_psk_t));

          if((sec_type == RSI_WPA2_PMK)||(sec_type == RSI_WPA_PMK))
          {
            //! Set type as PMK
            psk->type = 2;

            //! Copy PMK
            memcpy(psk->psk_or_pmk, secret_key, RSI_PMK_LEN);

          }
          else
          {
            //! Set type as PSK
            psk->type = 1;

            //! Copy PSK
            memcpy(psk->psk_or_pmk, secret_key,RSI_PSK_LEN);
          }

          //! send psk command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HOST_PSK, pkt);

          if(status != RSI_SUCCESS)
          {
            //! free the packet
            rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

            if(join_response_handler != NULL)
            {
              //! unlock mutex
              rsi_mutex_unlock(&wlan_cb->wlan_mutex);
            }

            //! Return the status if error in sending command occurs
            return status;
          }
        }

#if RSI_REJOIN_PARAMS_SUPPORT

        //! Memset data
        memset(&pkt->data,0,sizeof(rsi_req_rejoin_params_t));

        //! send rejoin params command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_REJOIN_PARAMS, pkt);

        if(status != RSI_SUCCESS)
        {
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

          if(join_response_handler != NULL)
          {
            //! unlock mutex
            rsi_mutex_unlock(&wlan_cb->wlan_mutex);
          }

          //! Return the status if error in sending command occurs
          return status;
        }
#endif
        //! memset the pkt
        memset(&pkt->data,0,sizeof(rsi_req_join_t));

        join = (rsi_req_join_t *)pkt->data;

        //! Fill security type
        if(sec_type == RSI_WPA2_PMK)
        {
          join->security_type = RSI_WPA2;
        }
        else if(sec_type == RSI_WPA_PMK)
        {
          join->security_type = RSI_WPA;
        }
        else if((sec_type == RSI_WPS_PIN)||
            (sec_type == RSI_USE_GENERATED_WPSPIN)||
            (sec_type == RSI_WPS_PUSH_BUTTON))
        {
          join->security_type = RSI_OPEN;
        }
        else
        {
          join->security_type = sec_type;
        }

        if(ssid != NULL)
        {
          //! Copy ssid and ssid len
          join->ssid_len = rsi_strlen(ssid);

          //! Copy Join SSID
          rsi_strcpy(join->ssid, ssid);
        }

        //! Magic number to set asyncronous reswponse
        wlan_cb->expected_response = RSI_WLAN_RSP_ASYNCHRONOUS ;

        if(join_response_handler != NULL )
        {
          //! Register scan response handler
          wlan_cb->callback_list.wlan_join_response_handler = join_response_handler;
        }

        //! send join command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JOIN, pkt);


      }
      break;
    default:
      {
        //! Return the status if command given in driver invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;

      }

  }
  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  if(join_response_handler != NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&wlan_cb->wlan_mutex);
  }

  //! Return the status
  return status;
}

/*==============================================*/
/**
 * @fn        int32_t rsi_wlan_bgscan_profile(uint8_t cmd)
 * @brief     This API is used to get bg scan results or stop bgscan
 * @param[in]  None
 * @param[out] None
 * @return      int32_t
 *              0  =  success
 *              <0 = failure
 * @section description
 * This API is used to  to get bg scan results or stop bgscan
 */

int32_t rsi_wlan_bgscan_profile(uint8_t cmd,rsi_rsp_scan_t *result,uint32_t length )
{
  rsi_pkt_t       *pkt;
  int8_t      status = RSI_SUCCESS;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! check whether module is in valid state range or not
  if(wlan_cb->state < RSI_WLAN_STATE_CONNECTED)
  {
    //! Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  //! take lock on wlan control block
  rsi_mutex_lock(&wlan_cb->wlan_mutex);

  //! attach the buffer given by user
  wlan_cb->app_buffer = (uint8_t *)result;

  //! length of the buffer provided by user
  wlan_cb->app_buffer_length = length;

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

  //! Memset data
  memset(&pkt->data,0,sizeof(rsi_req_bg_scan_t));

  //! Magic word
  pkt->data[0] = 0xAB;

  //! cmd
  pkt->data[1] = cmd;

  //! send bg scan command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BG_SCAN, pkt);

  if(status != RSI_SUCCESS)
  {
    //! free the packet
    rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

    //! unlock mutex
    rsi_mutex_unlock(&wlan_cb->wlan_mutex);

    //! Return the status if error in sending command occurs
    return status;
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
 * @fn        int32_t rsi_wlan_execute_post_connect_cmds(void)
 * @brief     This API is used to send the commands that are required after connection
 * @param[in]  None
 * @param[out] None
 * @return      int32_t
 *              0  =  success
 *              <0 = failure
 * @section description
 * This API is used to send the commands that are required after connection
 *
 */

//! execute post connect commands
int32_t rsi_wlan_execute_post_connect_cmds(void)
{
  rsi_pkt_t       *pkt;
  int8_t      status = RSI_SUCCESS;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! check whether module is in valid state range or not
  if(wlan_cb->state < RSI_WLAN_STATE_CONNECTED)
  {
    //! Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

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
#if RSI_BG_SCAN_SUPPORT

  //! Memset data
  memset(&pkt->data,0,sizeof(rsi_req_bg_scan_t));

  //! send bg scan command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BG_SCAN, pkt);

  if(status != RSI_SUCCESS)
  {
    //! free the packet
    rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

    //! unlock mutex
    rsi_mutex_unlock(&wlan_cb->wlan_mutex);

    //! Return the status if error in sending command occurs
    return status;
  }
#endif
#if RSI_ROAMING_SUPPORT

  //! Memset data
  memset(&pkt->data,0,sizeof(rsi_req_roam_params_t));

  //! send roaming parameters command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_ROAM_PARAMS, pkt);

#endif
  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}


/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_wps_push_button_event(int8_t *ssid)
 * @brief       start wps for two minutes from AP
 * @return      int32_t
 *              0  =  success
 *              <0 = failure
 * @section description
 * This function is used to start wps push button method from the access point.
 *
 */

int32_t rsi_wlan_wps_push_button_event(int8_t *ssid)
{
  rsi_pkt_t       *pkt;
  rsi_req_join_t  *join;
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

  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_req_join_t));

  join = (rsi_req_join_t *)pkt->data;

  if(ssid != NULL)
  {
    //! Copy ssid and ssid len
    join->ssid_len = rsi_strlen(ssid);

    //! Copy Join SSID
    rsi_strcpy(join->ssid, ssid);
  }

  //! Deregister join response handler
  wlan_cb->callback_list.wlan_join_response_handler = NULL;

  //! send join command to start wps
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JOIN, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}

/*==============================================*/
/**
 * @fn                int32_t rsi_wlan_wps_generate_pin(uint8_t *response, uint16_t length )
 * @brief             Generates valid wps pin
 * @param[in]         application buffer to get the command response payload
 * @param[in]         length of the application buffer to copy command response
 * @param[out]        8 bytes wps pin
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 *
 * @section description
 * This function is used to generate wps pin and posts the response to
 * application. Application should call this api to get the generated pin.
 *
 *
 */

int32_t rsi_wlan_wps_generate_pin(uint8_t *wps_pin, uint16_t length)
{
  rsi_pkt_t       *pkt;
  rsi_req_wps_method_t *wps_method;
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

  wps_method = (rsi_req_wps_method_t *)pkt->data;

  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_req_wps_method_t));

  //!set configuration to generate wps pin
  rsi_uint16_to_2bytes(wps_method->wps_method, 1);

  //!set configuration to generate wps pin
  rsi_uint16_to_2bytes(wps_method->generate_pin, 1);

  //! attach the buffer given by user
  wlan_cb->app_buffer = (uint8_t *)wps_pin;

  //! length of the buffer provided by user
  wlan_cb->app_buffer_length = length;

  //! send  wps request command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WPS_METHOD, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}

/*==============================================*/
/**
 * @fn                int32_t rsi_wlan_wps_enter_pin(int8_t *wps_pin)
 * @brief             validates wps pin entered
 * @param[in]         wps_pin , 8 byte valid wps pin
 * @param[out]        same wps pin if command success
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 *
 * @section description
 * This function is used to validate wps pin entered.
 *
 */


int32_t rsi_wlan_wps_enter_pin(int8_t *wps_pin)
{
  rsi_pkt_t       *pkt;
  rsi_req_wps_method_t *wps_method;
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

  wps_method = (rsi_req_wps_method_t *)pkt->data;

  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_req_wps_method_t));

  //!set configuration to validate wps pin
  rsi_uint16_to_2bytes(wps_method->wps_method, 1);

  //!set configuration to validate wps pin
  rsi_uint16_to_2bytes(wps_method->generate_pin, 0);

  //! Copy WPS Pin
  memcpy(wps_method->wps_pin, wps_pin, RSI_WPS_PIN_LEN);

  //! send wps request command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WPS_METHOD, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;

}

/*==============================================*/
/**
 * @fn          int32_t rsi_get_random_bytes(void)
 * @brief       Disconnects from AP
 * @return      int32_t
 *              0  =  success
 *              <0 = failure
 * @section description
 * This function is used to get random bytes from the device.
 *
 */

int32_t rsi_get_random_bytes(uint8_t *result,uint32_t length)
{
  rsi_pkt_t       *pkt;
  int8_t          status = RSI_SUCCESS;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! attach the buffer given by user
  wlan_cb->app_buffer = (uint8_t *)result;

  //! length of the buffer provided by user
  wlan_cb->app_buffer_length = length;

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

  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_req_disassoc_t));

  //! send get random number command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GET_RANDOM, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}


/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_disconnect(void)
 * @brief       Disconnects from AP
 * @return      int32_t
 *              0  =  success
 *              <0 = failure
 * @section description
 * This function is used to disconnect from the access point.
 *
 */

int32_t rsi_wlan_disconnect(void)
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

  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_req_disassoc_t));

  //! send disconnect command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DISCONNECT, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}

/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_disconnect_stations(uint8_t *mac_address)
 * @brief       Disconnects the connected station
 * @return      int32_t
 *              0  =  success
 *              <0 = failure
 * @section description
 * This function is used to disconnect connected stations by the access point.
 *
 */

int32_t rsi_wlan_disconnect_stations(uint8_t *mac_address)
{
  rsi_pkt_t       *pkt;
  rsi_req_disassoc_t *disassoc_info;
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

  disassoc_info= (rsi_req_disassoc_t *)pkt->data;

  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_req_disassoc_t));

  //!set mode flag to represent ap mode
  rsi_uint16_to_2bytes(disassoc_info->mode_flag, 1);

  //!copy mac address of station to be disconnected
  memcpy(disassoc_info->client_mac_address, mac_address, 6);

  //! send disconnect command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DISCONNECT, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}

/*==============================================*/
/**
 * @fn           int32_t rsi_config_ipaddress(rsi_ip_version_t version, uint8_t mode, uint8_t *ip_addr, uint8_t *mask, uint8_t *gw, uint8_t *ipconfig_rsp, uint16_t length);
 * @brief        This function is used to assign the IP address to the module
 * @param[in]    version , ip version to be used
 * @param[in]    mode    , 1 - dhcp mode ; 0 - static mode
 * @param[in]    ip_addr , IP address of module if in static mode
 * @param[in]    mask    , Netmask used if in static ip mode
 * @param[in]    gw      , gate way ip address if in static ipmode
 * @param[in]    ipconfig_rsp, application buffer to get the IP config response
 * @param[in]    length  , length of the application buffer to copy IP config response
 *
 * @return       int32_t
 *               0  =  success
 *               <0 = failure
 *
 * @section description
 * This function assigns the ip address after connection establishment
 */
int32_t  rsi_config_ipaddress(rsi_ip_version_t version, uint8_t mode, uint8_t *ip_addr, uint8_t *mask, uint8_t *gw, uint8_t *ipconfig_rsp, uint16_t length, uint8_t vap_id)
{
  rsi_pkt_t             *pkt;
  rsi_req_ipv4_parmas_t *ip_params;
  rsi_req_ipv6_parmas_t *ipv6_params;
  int32_t               status = RSI_SUCCESS;

  //! Get WLAN cb struct pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! Wlan radio init
  status = rsi_wlan_radio_init();

  if(status != RSI_SUCCESS)
  {
    //! Return the status if error in sending command occurs
    return status;
  }
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

  //! attach the buffer given by user
  wlan_cb->app_buffer = (uint8_t *)ipconfig_rsp;

  //! length of the buffer provided by user
  wlan_cb->app_buffer_length = length;

  //! If ipversion is IPv4
  if(version == RSI_IP_VERSION_4)
  {
    //! fill all the ipv4 parameters
    ip_params = (rsi_req_ipv4_parmas_t *)pkt->data;

    //! Set DHCP mode
    ip_params->dhcp_mode = mode;

    //! fill ip address only if static mode is selected
    if(mode == RSI_STATIC)
    {
      //! Fill IP address
      memcpy(ip_params->ipaddress, ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      //! Fill network mask
      memcpy(ip_params->netmask, mask, RSI_IPV4_ADDRESS_LENGTH);

      //! Fill gateway
      memcpy(ip_params->gateway, gw, RSI_IPV4_ADDRESS_LENGTH);
    }

    //! Check for DHCP HOSTNAME feature
    if(mode & RSI_DHCP_HOSTNAME)
    {
      //! Copy DHCP client hostname
      rsi_strcpy(ip_params->hostname, RSI_DHCP_HOST_NAME);
    }

		//! Set vap_id
		ip_params->vap_id = vap_id;

		//! send ip config command
		status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_IPCONFV4, pkt);

  }
  else if(version == RSI_IP_VERSION_6)
  {
    //! fill all the ipv6 params
    ipv6_params = (rsi_req_ipv6_parmas_t *)pkt->data;


    //! Fill prefix length
    rsi_uint16_to_2bytes(ipv6_params->prefixLength,64);

    //! Fill DHCP mode
    rsi_uint16_to_2bytes(ipv6_params->mode,mode);

    //! fill ip address only if static mode is selected
    if(mode == RSI_STATIC)
    {
      //! Fill IPv6 address
      memcpy(ipv6_params->ipaddr6, ip_addr, RSI_IPV6_ADDRESS_LENGTH);

      //! Fill gateway
      memcpy(ipv6_params->gateway6, gw, RSI_IPV6_ADDRESS_LENGTH);
    }

    //! send ip config command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_IPCONFV6, pkt);
  }
  else
  {

    //! deattach buffer
    wlan_cb->app_buffer = NULL;

    //!make length of the buffer to 0
    wlan_cb->app_buffer_length = 0;

    //! update Throw error in case of invalid parameters
    status =  RSI_ERROR_INVALID_PARAM;

  }

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status
  return status;

}


/*==============================================*/
/**
 * @fn         int32_t  rsi_wlan_set_certificate(uint8_t certificate_type, uint8_t *buffer, uint32_t certificate_length)
 * @brief      This function is used to write or erase certificate into module
 * @param[in]  certificate_type, type of certificate
 * @param[in]  buffer, certificate content
 * @param[in]  certificate_length, total length of the certificate
 * @param[out] None
 * @return
 *
 *
 * @section description
 * This function is used to write or erase certificate into module
 */
int32_t rsi_wlan_set_certificate(uint8_t certificate_type, uint8_t *buffer, uint32_t certificate_length)
{
  static uint32_t rem_len;
  uint16_t        chunk_size = 0;
  static uint32_t offset;
  rsi_pkt_t       *pkt;
  int32_t         status = RSI_SUCCESS;
  rsi_req_set_certificate_t *rsi_chunk_ptr;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! check if command is in correct state or not
  if((wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE ))
  {
    //! Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  if((buffer == NULL) && (certificate_length))
  {
    //! Throw error in case of invalid parameters
    return RSI_ERROR_INVALID_PARAM;
  }

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


  rsi_chunk_ptr = (rsi_req_set_certificate_t *)pkt->data;

  //!Get the certificate chunk size
  chunk_size = (RSI_MAX_CERT_SEND_SIZE - sizeof(struct rsi_cert_info_s));

  //! get certificate length
  rem_len = certificate_length;

  //! if certificate length is 0,then erase that certifate
  if(rem_len == 0)
  {
    //! memset the pkt
    memset(&pkt->data, 0,RSI_MAX_CERT_SEND_SIZE);

    //!Set the total length of certificate
    rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.total_len, certificate_length);

    //! Set the certificate type
    rsi_chunk_ptr->cert_info.certificate_type = certificate_type;

    //! more chunks to send
    rsi_chunk_ptr->cert_info.more_chunks = 0;

    //! Set the length of the certificate chunk
    rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.certificate_length, 0);

    //! send the driver command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_CERTIFICATE, pkt);


  }
  while(rem_len)
  {

    //! memset the pkt
    memset(&pkt->data, 0, RSI_MAX_CERT_SEND_SIZE);

    if(rem_len >= chunk_size)
    {
      //! Copy the certificate chunk
      memcpy(rsi_chunk_ptr->certificate, buffer + offset, chunk_size);

      //! Move the offset by chunk size
      offset += chunk_size;

      //! Subtract the rem_len by the chunk size
      rem_len -= chunk_size;

      //!Set the total length of certificate
      rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.total_len, certificate_length);

      //! Set the certificate type
      rsi_chunk_ptr->cert_info.certificate_type = certificate_type;

      //! more chunks to send
      rsi_chunk_ptr->cert_info.more_chunks = (rem_len == 0) ? 0 : 1;

      //! Set the length of the certificate chunk
      rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.certificate_length, chunk_size);

      //! send the driver command
      status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_CERTIFICATE, pkt);

      if(status != RSI_SUCCESS)
      {
        break;
      }
    }
    else
    {

      //! Copy the certificate chunk
      memcpy(rsi_chunk_ptr->certificate, buffer + offset, rem_len);

      //! Move the offset by chunk size
      offset += rem_len;

      //!Set the total length of certificate
      rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.total_len, certificate_length);

      //! Set the certificate type
      rsi_chunk_ptr->cert_info.certificate_type = certificate_type;

      //!This is the last chunks to send
      rsi_chunk_ptr->cert_info.more_chunks = 0;

      //! Set the length of the certificate chunk
      rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.certificate_length, rem_len);

      //! send the driver command
      status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_CERTIFICATE, pkt);

      if(status != RSI_SUCCESS)
      {
        break;
      }

      //! Reset rem_len and offset
      rem_len = 0;
      offset = 0;
    }
  }

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status
  return status;

}

/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_get_status(void)
 * @brief       Returns wlan status
 * @param[in]   None
 * @param[out]  None
 * @return      wlan status
 *
 * @section description
 * This function returns wlan status
 *
 */
int32_t rsi_wlan_get_status(void)
{
  return rsi_driver_cb->wlan_cb->status;
}

/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_get(rsi_query_cmd_t cmd_type, uint8_t *response, uint16_t length)
 * @brief       Returns enquired data
 * @param[in]   command type.The options are : \n
 *              - RSI_RSSI
 *              - RSI_FW_VERSION
 *              - RSI_MAC_ADDRESS
 *              - RSI_CONNECTION_STATUS
 *              - RSI_WLAN_INFO
 *              - RSI_STATION_INFO
 *              - RSI_SOCKETS_INFO
 *
 * @param[in]   application buffer to get the command response payload
 * @param[in]   length of the application buffer to copy command response
 * @param[out]  none
 * @return      command response status
 *
 * @section description
 * The functionality of this function depends on the ion
 *
 * This function returns wlan status
 *
 */
int32_t rsi_wlan_get(rsi_wlan_query_cmd_t cmd_type, uint8_t *response, uint16_t length)
{
  int32_t    status = RSI_SUCCESS;
  rsi_pkt_t             *pkt;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! Get common control block structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  //! if state is not in card ready received state
  if(common_cb->state == RSI_COMMON_STATE_NONE)
  {
    //! Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

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

  //! copy query cmd type in driver control block
  wlan_cb->query_cmd = cmd_type;

  if(response != NULL)
  {
    //! attach the buffer given by user
    wlan_cb->app_buffer = response;

    //! length of the buffer provided by user
    wlan_cb->app_buffer_length = length;
  }
  else
  {
    //! Assign NULL to the app_buffer to avoid junk
    wlan_cb->app_buffer = NULL;

    //! length of the buffer to 0
    wlan_cb->app_buffer_length = 0;

  }

  switch(cmd_type)
  {
    case RSI_RSSI:
      {
        //! copy query cmd type in driver control block

        //! Send RSSI query request
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_RSSI, pkt);

      }
      break;
    case RSI_FW_VERSION:
      {

        //! Send firmware version query request
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FW_VERSION, pkt);

      }
      break;
    case RSI_MAC_ADDRESS:
      {

        //! Send MAC address query request
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_MAC_ADDRESS, pkt);

      }
      break;
    case RSI_CONNECTION_STATUS:
      {

        //! Send MAC address query request
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CONNECTION_STATUS, pkt);

      }
      break;
    case RSI_WLAN_INFO:
      {
				if(length < sizeof(rsi_rsp_wireless_info_t))
				{
					//! free the packet
					rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

					//! unlock mutex
					rsi_mutex_unlock(&wlan_cb->wlan_mutex);

					return  RSI_ERROR_INSUFFICIENT_BUFFER;
				}
        if(wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE)
        {
          //! Send cmd for wlan info in AP mode
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_QUERY_GO_PARAMS, pkt);
        }
        else if((wlan_cb->opermode == RSI_WLAN_CLIENT_MODE) || (wlan_cb->opermode == RSI_WLAN_ENTERPRISE_CLIENT_MODE))
        {
          //! Send cmd for wlan info in client mode
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_QUERY_NETWORK_PARAMS, pkt);
        }
        else if(wlan_cb->opermode == RSI_WLAN_WIFI_DIRECT_MODE)
        {

        }
      }
      break;
    case RSI_STATIONS_INFO:
      {
				if(length < sizeof(rsi_rsp_stations_info_t))
				{
					//! free the packet
					rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

					//! unlock mutex
					rsi_mutex_unlock(&wlan_cb->wlan_mutex);

					return  RSI_ERROR_INSUFFICIENT_BUFFER;
				}

        //! Send MAC address query request
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_QUERY_GO_PARAMS, pkt);

      }
      break;
    case RSI_SOCKETS_INFO:
      {
        if(length < sizeof(rsi_rsp_sockets_info_t))
        {
          //! free the packet
					rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

					//! unlock mutex
					rsi_mutex_unlock(&wlan_cb->wlan_mutex);

					return  RSI_ERROR_INSUFFICIENT_BUFFER;
        }

        //! Send MAC address query request
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_QUERY_NETWORK_PARAMS, pkt);

      }
      break;
    default:
      break;

  }

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status
  return status;

}



/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_set(rsi_wlan_cmd_request_t cmd_type, uint8_t *request, uint16_t length)
 * @brief       Sets the configuration requested by the user
 * @param[in]   command type.The options are : \n
 *              - RSI_WLAN_REQ_SET_MAC_ADDRESS
 *              - RSI_WLAN_REQ_SET_MULTICAST_FILTER
 * @param[in]   application buffer to set the configuration payload
 * @param[in]   length of the application buffer to copy in command request
 * @param[out]         none
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 *
 * @section description
 * This function sets the required  configuration and returns status
 *
 */
int32_t rsi_wlan_set(rsi_wlan_set_cmd_t cmd_type, uint8_t *request, uint16_t length)
{
  int32_t    status = RSI_SUCCESS;
  rsi_pkt_t             *pkt;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! Get common control block structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  //! if state is not in card ready received state
  if(common_cb->state == RSI_COMMON_STATE_NONE)
  {
    //! Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  if(request == NULL)
  {
    //! Command given in wrong state
    return RSI_ERROR_INVALID_PARAM;
  }

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


  switch(cmd_type)
  {
    case RSI_SET_MAC_ADDRESS:
      {
        //! enable valid bit to ensure setting mac id before every initilization
        wlan_cb->field_valid_bit_map |= RSI_SET_MAC_BIT ;

        //! copy mac address in to wlan control block
        memcpy(wlan_cb->mac_address, request, 6);

      }
      break;

    case RSI_MULTICAST_FILTER:
      {

        //! Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_multicast_filter_info_t));

        //! copy mac address
        memcpy((uint8_t *)&pkt->data,request, sizeof(rsi_req_multicast_filter_info_t));

        //! Set  Multi cast filter configuration
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MULTICAST_FILTER, pkt);

      }
      break;

    default:
      break;

  }
  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status
  return status;

}



/*==============================================*/
/**
 * @fn                 int32_t rsi_wlan_ap_start(int8_t *ssid, uint8_t channel, rsi_security_mode_t security_type,rsi_encryption_mode_t encryption_mode, uint8_t *password, uint16_t beacon_interval, uint8_t dtim_period)
 * @brief              Configure the device in AP mode
 * @param[in]          ssid, set AP ssid
 * @param[in]          channel, set AP channel
 * @param[in]          security_type, security type - WPA/WPA2
 * @param[in]          encryption_mode, encryption mode - TKIP/AES
 * @param[in]          password, psk
 * @param[in]          beacon_interval, beacon interval
 * @param[in]          dtim_period, dtim period
 *
 * @param[out]         none
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 *
 * @section description
 * This function fills required parameters and configure the device as AP
 *
 *
 */
int32_t rsi_wlan_ap_start(int8_t *ssid, uint8_t channel, rsi_security_mode_t security_type,
    rsi_encryption_mode_t encryption_mode, uint8_t *password, uint16_t beacon_interval, uint8_t dtim_period)
{

  rsi_pkt_t       *pkt;
  rsi_req_join_t  *join;
  int8_t          status = RSI_SUCCESS;

  rsi_req_ap_config_t *ap_config;


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
#endif

      }
    case RSI_WLAN_STATE_IP_CONFIG_DONE:
    case RSI_WLAN_STATE_IPV6_CONFIG_DONE:
    case RSI_WLAN_STATE_INIT_DONE:
      {
        //! Memset packet
        memset(&pkt->data,0,sizeof(rsi_req_ap_config_t));

        ap_config = (rsi_req_ap_config_t *)pkt->data;

        //! copy channel by uint16 to 2 bytes conversion
        rsi_uint16_to_2bytes(ap_config->channel, channel);

        if(ssid != NULL)
        {
          //! Copy Join SSID
          rsi_strcpy(ap_config->ssid, ssid);
        }

        //! security type
        ap_config->security_type = security_type;

        //! encryption mode
        ap_config->encryption_mode = encryption_mode;

        //! If security is enabled
        if(security_type)
        {
          //! copy psk
          rsi_strcpy(ap_config->psk, password);
        }

        //! copy beacon interval by uint16 to 2 bytes conversion
        rsi_uint16_to_2bytes(ap_config->beacon_interval, beacon_interval);

        //! copy dtim period by uint16 to 2 bytes conversion
        rsi_uint16_to_2bytes(ap_config->dtim_period, dtim_period);

        //! send AP config command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_AP_CONFIGURATION, pkt);

        //! check status for AP config command
        if(status != RSI_SUCCESS)
        {
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

          //! unlock mutex
          rsi_mutex_unlock(&wlan_cb->wlan_mutex);

          //! Return the status if error in sending command occurs
          return status;
        }

#if RSI_MODE_11N_ENABLE

        //! Memset packet
        memset(&pkt->data,0,sizeof(rsi_req_ap_ht_caps_t));

        //! send HT capabilities command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HT_CAPABILITIES, pkt);

        if(status != RSI_SUCCESS)
        {
          //! free the packet
          rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

          //! unlock mutex
          rsi_mutex_unlock(&wlan_cb->wlan_mutex);

          //! Return the status if error in sending command occurs
          return status;
        }
#endif
        //! memset the pkt
        memset(&pkt->data,0,sizeof(rsi_req_join_t));

        join = (rsi_req_join_t *)pkt->data;

        if(ssid != NULL)
        {
          //! Copy ssid and ssid len
          join->ssid_len = rsi_strlen(ssid);

          //! Copy Join SSID
          rsi_strcpy(join->ssid, ssid);
        }

		//! vap id of the current mode 0 - station mode, 1 - AP1 mode
		//  Applicable in cocurrent mode only
#if CONCURRENT_MODE
		join->vap_id = 1;
#else
		join->vap_id = 0;
#endif

    //! Deregister join response handler
    wlan_cb->callback_list.wlan_join_response_handler = NULL;

    //! send join command
		status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JOIN, pkt);

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
      break;
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
 * @fn          int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type)
 * @brief       Sets power save profile
 * @param[in]   psp_mode.The options are : \n
 *              0 - Active
 *              1 - Sleep without SoC turn off when connected else disconnected deep sleep
 *              2 - Sleep with SoC turn off (connected/disconnected)
 *              8 - Deep sleep
 * @param[in]   psp_type. The options are : \n
 *              0 - Max power save
 *              1 - Fast PSP
 *              2 - UAPSD
 * @param[out]  none
 * @return      command response status
 *
 * @section description
 * This function sets the different power save profile
 *
 */

int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type)
{
  int32_t         status = RSI_SUCCESS;

  //!Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  //!keep backup of power save profile mode and type
  rsi_common_cb->power_save.wlan_psp_type = psp_type;

  //!keep backup of power save profile mode and type
  rsi_common_cb->power_save.wlan_psp_mode = psp_mode;

  status = rsi_sleep_mode_decision(rsi_common_cb);

  return status;
}

/*==============================================*/
/**
 * @fn                int32_t rsi_transmit_test_start(uint16_t power, uint16_t rate, uint16_t length, uint16_t mode, uint16_t channel)
 * @brief             Starts transmit test in tx mode
 * @param[in]         power, rate, length, mode , channel
 * @param[out]        none
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 *
 * @section description
 * This function is used to start transmit test in tx mode
 *
 */

int32_t rsi_transmit_test_start(uint16_t power, uint32_t rate, uint16_t length, uint16_t mode, uint16_t channel)
{
  rsi_pkt_t       *pkt;
  rsi_req_tx_test_info_t *tx_test_info;
  int8_t          status = RSI_SUCCESS;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! Check whether transmit test mode is selescted
  if(wlan_cb->opermode != RSI_WLAN_TRANSMIT_TEST_MODE)
  {
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  //! Wlan radio init
  status = rsi_wlan_radio_init();

  if(status != RSI_SUCCESS)
  {
    //! Return the status if error in sending command occurs
    return status;
  }

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

  //! Fill tx test info parameters
  tx_test_info = (rsi_req_tx_test_info_t *)pkt->data;

  //! memset the pkt
  memset(&pkt->data,0,sizeof(rsi_req_tx_test_info_t));

  //! enable transmit test
  rsi_uint16_to_2bytes(tx_test_info->enable, 1);

  //! configure transmit power of tx test
  rsi_uint16_to_2bytes(tx_test_info->power, power);

  //! configure transmit rate of tx test
  rsi_uint32_to_4bytes(tx_test_info->rate, rate);

  //! configure packet length of tx test
  rsi_uint16_to_2bytes(tx_test_info->length, length);

  //! configure transmit mode of tx test
  rsi_uint16_to_2bytes(tx_test_info->mode, mode);

  //! configure channel of tx test
  rsi_uint16_to_2bytes(tx_test_info->channel, channel);

  //! send  command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TX_TEST_MODE, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}

/*==============================================*/
/**
 * @fn                rsi_transmit_test_stop(void)
 * @brief             Stops triggered transmit test in tx mode
 * @param[in]         none
 * @param[out]        none
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 *
 * @section description
 * This function is used to stop transmit test running in tx mode
 *
 */

int32_t rsi_transmit_test_stop(void)
{
  rsi_pkt_t       *pkt;
  rsi_req_tx_test_info_t *tx_test_info;
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

  //! Fill tx test info parameters
  tx_test_info = (rsi_req_tx_test_info_t *)pkt->data;

  //! memset the pkt
  memset(&pkt->data, 0, sizeof(rsi_req_tx_test_info_t));

  //! configure to stop tx test
  rsi_uint16_to_2bytes(tx_test_info->enable, 0);

  //! send  command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TX_TEST_MODE, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}

/*==============================================*/
/**
 * @fn                int32_t rsi_wlan_receive_stats_start(uint16_t channel)
 * @brief             Starts rsi wlan receive test, indicates rx stats to host periodically
 * @param[in]         channel, channel in which receive stats to be noticed
 * @param[out]
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 *
 * @section description
 * This function is used to start recieve stats and indicate them to host periodically
 *
 */


int32_t rsi_wlan_receive_stats_start(uint16_t channel)
{
  rsi_pkt_t       *pkt;
  rsi_req_rx_stats_t *rx_stats;
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

  //! Fill rx stats  parameters
  rx_stats = (rsi_req_rx_stats_t *)pkt->data;

  //! memset the pkt
  memset(&pkt->data,0,sizeof(rsi_req_rx_stats_t));

  //! configure to start rx stats
  rsi_uint16_to_2bytes(rx_stats->start, 0);

  //! copy the channel number
  rsi_uint16_to_2bytes(rx_stats->channel, channel);

  //! Magic number to set asyncronous reswponse
  wlan_cb->expected_response = RSI_WLAN_RSP_ASYNCHRONOUS ;

  //! send  command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_RX_STATS, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}

/*==============================================*/
/**
 * @fn                int32_t rsi_wlan_receive_stats_stop(void)
 * @brief             Stops module receiving stats.
 * @param[in]         none
 * @param[out]        none
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 *
 * @section description
 * This function is used to stop wlan recieve stats.
 *
 */


int32_t rsi_wlan_receive_stats_stop(void)
{
  rsi_pkt_t       *pkt;
  rsi_req_rx_stats_t *rx_stats;
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

  //! Fill rx stats  parameters
  rx_stats = (rsi_req_rx_stats_t *)pkt->data;

  //! memset the pkt
  memset(&pkt->data,0,sizeof(rsi_req_rx_stats_t));

  //! configure to stop rx stats
  rsi_uint16_to_2bytes(rx_stats->start, 1);

  //! send  command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_RX_STATS, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}


/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_wfd_start_discovery(uint16_t go_intent, int8_t *device_name,
 uint16_t channel, int8_t *ssid_post_fix, uint8_t *psk,
 void(*wlan_wfd_discovery_notify_handler)(uint16_t status,const uint8_t *buffer, const uint16_t length),
 void(*wlan_wfd_connection_request_notify_handler)(uint16_t status,const uint8_t *buffer, const uint16_t length))

 * @param[in]        go_intent, group owner intent ,integer value 0 <= go_intent <= 16
 * @param[in]        device_name, device name during discovery phase for other wfd devices
 * @param[in]        channel, operating channel number
 * @param[in]        ssid_post_fix, post fix SSID of the device when turned up as GO
 * @param[in]        psk, Preshared key
 * @param[in]        void(*wlan_wfd_discovery_notify_handler)(uint16_t status,const uint8_t *buffer, const uint16_t length),
 *                   callback to notify discoverd wfd devices
 * @param[in]        void(*wlan_wfd_connection_request_notify_handler)(uint16_t status,const uint8_t *buffer, const uint16_t length))
 callback to notify connection request received from remote devices
 * @param[out]       <none>
 * @brief       Starts wifi direct discovery mode
 * @return      int32_t
 *              0  =  success
 *              <0 = failure
 * @section description
 * This function is used to start discovery in wifi direct mode.
 *
 */

int32_t rsi_wlan_wfd_start_discovery(uint16_t go_intent, int8_t *device_name,
    uint16_t channel, int8_t *ssid_post_fix, uint8_t *psk,
    void (*wlan_wfd_discovery_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length),
    void (*wlan_wfd_connection_request_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length))
{
  rsi_pkt_t       *pkt;
  int8_t          status = RSI_SUCCESS;
  rsi_req_configure_p2p_t  *config_p2p;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! Check whether WiFi-Direct mode is selescted
  if(wlan_cb->opermode != RSI_WLAN_WIFI_DIRECT_MODE)
  {
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  //! Wlan radio init
  status = rsi_wlan_radio_init();

  if(status != RSI_SUCCESS)
  {
    //! Return the status if error in sending command occurs
    return status;
  }

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
  config_p2p = (rsi_req_configure_p2p_t *)pkt->data;

  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_req_configure_p2p_t));

  //! configure go intent
  rsi_uint16_to_2bytes(config_p2p->go_intent, go_intent);

  if(device_name != NULL)
  {
    //! Copy device name
    rsi_strcpy(config_p2p->device_name, device_name);
  }

  //! configure operating channel
  rsi_uint16_to_2bytes(config_p2p->operating_channel, channel);

  if(ssid_post_fix != NULL)
  {
    //! Copy ssid post fix
    rsi_strcpy(config_p2p->ssid_post_fix, ssid_post_fix);
  }

  if(psk != NULL)
  {
    //! Copy psk
    rsi_strcpy(config_p2p->psk, psk);
  }

  if(wlan_wfd_discovery_notify_handler != NULL)
  {
    rsi_driver_cb->wlan_cb->callback_list.wlan_wfd_discovery_notify_handler =  wlan_wfd_discovery_notify_handler;
  }

  if(wlan_wfd_connection_request_notify_handler != NULL)
  {
    rsi_driver_cb->wlan_cb->callback_list.wlan_wfd_connection_request_notify_handler =  wlan_wfd_connection_request_notify_handler;
  }
  //! send wifi direct discovery command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CONFIGURE_P2P, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}


/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_wfd_connect(int8_t *device_name,  void (*join_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length))
 * @brief       connect to wfd device with given device name
 * @param[in]   device_name,remote device name if GO intent in configuration is < 16,
 *              rsi module device_name to create as auto GO.
 * @param[in]   join_response_handler, asynchronous response to the connect api
 * @param[out]  <node type>
 *              'G' = GO
 *              'C' = client
 * @return      int32_t
 *              0  =  success
 *              <0 = failure
 * @section description
 * This function is used to start wps push button method from the access point.
 *
 */

int32_t rsi_wlan_wfd_connect(int8_t *device_name,  void (*join_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length))
{
  rsi_pkt_t       *pkt;
  rsi_req_join_t  *join;
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

  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_req_join_t));

  join = (rsi_req_join_t *)pkt->data;

  if(device_name != NULL)
  {
    //! Copy ssid and ssid len
    join->ssid_len = rsi_strlen(device_name);

    //! Copy Join SSID
    rsi_strcpy(join->ssid, device_name);
  }

  //! Magic number to set asyncronous reswponse
  wlan_cb->expected_response = RSI_WLAN_RSP_ASYNCHRONOUS ;

  if(join_response_handler != NULL )
  {
    //! Register scan response handler
    wlan_cb->callback_list.wlan_join_response_handler = join_response_handler;
  }

  //! send join command to start wps
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JOIN, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}

/*==============================================*/
/**
 * @fn         int32_t rsi_wlan_send_data(uint8_t* buffer, uint32_t length)
 * @brief      This function is used to send raw data packet
 * @param[in]  buffer, pointer to data
 * @param[in]  length, data length
 * @param[out] None
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function is used to send raw data packet to module
 *
 *
 */

int32_t rsi_wlan_send_data(uint8_t* buffer, uint32_t length)
{
  int32_t  status = RSI_SUCCESS;
  uint8_t  *host_desc;
  rsi_pkt_t *pkt;

  //! If buffer is not valid
  if((buffer == NULL) ||(length == 0))
  {
    //! return packet allocation failure error
    return RSI_ERROR_INVALID_PARAM;
  }

  //! Allocate packet to send data
  pkt = rsi_pkt_alloc(&rsi_driver_cb->wlan_cb->wlan_tx_pool);

  if(pkt == NULL)
  {
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  //! Get host descriptor pointer
  host_desc = pkt->desc;

  //! Memset host descriptor
  memset(host_desc, 0, RSI_HOST_DESC_LENGTH);

  //! Fill host descriptor
  rsi_uint16_to_2bytes(host_desc, (length & 0xFFF));

  //! Fill packet type
  host_desc[1] |= (RSI_WLAN_DATA_Q << 4);

  //! Copy data to be sent
  memcpy(pkt->data, buffer, length);

  //! Enqueue packet to WLAN TX queue
  rsi_enqueue_pkt(&rsi_driver_cb->wlan_tx_q, pkt);


  //! Magic number to set asyncronous response
  rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_ASYNCHRONOUS;

  //! Set TX packet pending event
  rsi_set_event(RSI_TX_EVENT);

  //! Wait on wlan semaphore
  rsi_semaphore_wait(&rsi_driver_cb->wlan_cb->wlan_sem, RSI_WAIT_FOREVER);

  //! Clear expected response
  rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;

  //! Get wlan status
  status = rsi_wlan_get_status();

  //! Free packet
  rsi_pkt_free(&rsi_driver_cb->wlan_cb->wlan_tx_pool, pkt);

  //! Return status
  return status;

}

/*==============================================*/
/**
 * @fn         int32_t rsi_wlan_ping_async(uint8_t flags, uint8_t* ip_address, uint32_t size, void (*wlan_ping_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length))
 * @brief      This function is used to ping the network
 * @param[in]  flags, RSI_IPV6: To select IPv6 network, by default IPv4 is selected
 * @param[in]  ip_address, remote ip address to ping
 * @param[in]  size, ping packet size
 * @param[in]   wlan_ping_response_handler, Call back for ping response
 * @param[out] None
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function is used to send raw data packet to module
 *
 *
 */

int32_t rsi_wlan_ping_async(uint8_t flags, uint8_t *ip_address, uint16_t size, void (*wlan_ping_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length))
{
  int32_t  status = RSI_SUCCESS;
  rsi_pkt_t *pkt;
  rsi_req_ping_t  *ping;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! If ipaddress is not valid
  if((ip_address == NULL) ||(size == 0))
  {
    //! return packet allocation failure error
    return RSI_ERROR_INVALID_PARAM;
  }

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

  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_req_ping_t));

  ping = (rsi_req_ping_t *)pkt->data;

  if(flags & RSI_IPV6)
  {
    //! copy ip version
    rsi_uint16_to_2bytes(ping->ip_version,(uint16_t)RSI_IP_VERSION_6);

    //! copy ipv6 address
    memcpy(ping->ping_address.ipv6_address, ip_address,RSI_IPV6_ADDRESS_LENGTH);
  }
  else
  {
    //! copy ip version
    rsi_uint16_to_2bytes(ping->ip_version,(uint16_t)RSI_IP_VERSION_4);

    //! copy ipv4 address
    memcpy(ping->ping_address.ipv4_address, ip_address, RSI_IPV4_ADDRESS_LENGTH);

  }

  //! copy ping size
  rsi_uint16_to_2bytes(ping->ping_size, size);

  ping->timeout = RSI_PING_REQ_TIMEOUT;

  //! register ping response handler
  if(wlan_ping_response_handler != NULL)
  {
    wlan_cb->callback_list.wlan_ping_response_handler = wlan_ping_response_handler;
  }

  //! Magic number to set asyncronous response
  wlan_cb->expected_response = RSI_WLAN_RSP_ASYNCHRONOUS ;

  //! send ping command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_PING_PACKET, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}

/*==============================================*/
/**
 * @fn         void rsi_register_auto_config_rsp_handler(void (*rsi_auto_config_rsp_handler )(uint16_t status, uint8_t state))
 * @brief      This function is used to register card ready handler
 * @param[in]  rsi_auto_config_rsp_handler, pointer to rsi_auto_config_rsp_handler
 * @param[out] None
 * @return     None
 *
 *
 * @section description
 * This function is used to register Auto configuration response handler
 *
 *
 */

void rsi_register_auto_config_rsp_handler(void (*rsi_auto_config_rsp_handler )(uint16_t status, uint8_t state))
{
  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! Register callback handler
  wlan_cb->callback_list.auto_config_rsp_handler = rsi_auto_config_rsp_handler;

}


/*==============================================*/
/**
 * @fn         int32_t rsi_wlan_add_profile(uint8_t type, uint8_t *profile)
 * @brief      This function is used to add profile for auto configuration
 * @param[in]  type, Profile type
 * @param[in]  profile, pointer to config profile
 * @param[out] None
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function is used to add profile for auto configuration
 *
 *
 */


int32_t rsi_wlan_add_profile(uint32_t type, uint8_t *profile)
{
  int32_t  status = RSI_SUCCESS;
  rsi_pkt_t *pkt;
  uint16_t send_size = 0;
  uint8_t               *host_desc = NULL;
  rsi_config_profile_t  *config_profile;

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

  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_config_profile_t));

  config_profile = (rsi_config_profile_t *)pkt->data;


  //! Fill profile type
  rsi_uint32_to_4bytes(config_profile->profile_type, type);


  //! Fill wlan profile
  if(type == RSI_WLAN_PROFILE_AP)
  {
    memcpy(&config_profile->wlan_profile_struct.ap_profile, (uint8_t *)profile, sizeof(ap_profile_t));
  }
  else if(type == RSI_WLAN_PROFILE_CLIENT)
  {
    memcpy(&config_profile->wlan_profile_struct.client_profile, (uint8_t *)profile, sizeof(client_profile_t));
  }
  else if(type == RSI_WLAN_PROFILE_EAP)
  {
    memcpy(&config_profile->wlan_profile_struct.eap_client_profile, (uint8_t *)profile, sizeof(eap_client_profile_t));
  }
  else if(type == RSI_WLAN_PROFILE_P2P)
  {
    memcpy(&config_profile->wlan_profile_struct.p2p_profile, (uint8_t *)profile, sizeof(p2p_profile_t));
  }
  else
  {

  }

  //! Using host descriptor to set payload length
  send_size = sizeof(rsi_config_profile_t);

  //! get the host descriptor
  host_desc = (pkt->desc);

  //! Fill data length in the packet host descriptor
  rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

  //! send set profile command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_PROFILE, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}


/*==============================================*/
/**
 * @fn         uint8_t rsi_wlan_get_state(void)
 * @brief      This function is used to get the current wlan state
 * @param[in]  None
 * @param[out] None
 * @return     state
 *
 *
 * @section description
 * This function is used to get the wlan state
 *
 *
 */

uint8_t rsi_wlan_get_state(void)
{
  //! return wlan state
  return rsi_driver_cb->wlan_cb->state;
}


/*==============================================*/
/**
 * @fn         int32_t rsi_wlan_get_profile(uint32_t type, rsi_config_profile_t *profile_rsp, uint16_t length)
 * @brief      This function is used to get the stored config profile
 * @param[in]  type, config profile type
 * @param[in]  profile_rsp, config profile response
 * @param[in]  length, Length of the config profile response
 * @param[out] None
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function is used to get the stored config profile
 *
 *
 */


int32_t rsi_wlan_get_profile(uint32_t type, rsi_config_profile_t *profile_rsp, uint16_t length)
{
  int32_t   status                = RSI_SUCCESS;
  rsi_pkt_t *pkt                  = NULL;
  rsi_profile_req_t  *profile_req = NULL;

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


  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_profile_req_t));

  profile_req = (rsi_profile_req_t *)pkt->data;

  //! Fill profile type
  rsi_uint32_to_4bytes(profile_req->profile_type, type);

  //! attach the buffer given by user
  wlan_cb->app_buffer = (uint8_t *)profile_rsp;

  //! length of the buffer provided by user
  wlan_cb->app_buffer_length = length;

  //! send get profile command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GET_PROFILE, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}


/*==============================================*/
/**
 * @fn         uint8_t* rsi_fill_config_profile(uint32_t type, uint8_t *profile_buffer)
 * @brief      This function is used to fill the config profile based on the profile type
 * @param[in]  Type, Profile type
 * @param[in]  Profile_buffer, pointer to profile buffer
 * @param[out] None
 * @return
 *              profile_buffer
 *
 * @section description
 * This function is used to send raw data packet to module
 *
 *
 */


uint8_t* rsi_fill_config_profile(uint32_t type, uint8_t *profile_buffer)
{
  int i,j =0;

  uint32_t ip_addr = 0;
  uint32_t mask    = 0;
  uint32_t gw      = 0;
  eap_client_profile_t *eap_client_profile_ptr = NULL;
  network_profile_t *nwk_profile_ptr = NULL;
	client_profile_t *client_profile_ptr = NULL;
	ap_profile_t *ap_profile_ptr = NULL;
	p2p_profile_t *p2p_profile_ptr = NULL;

  if(type == RSI_WLAN_PROFILE_AP)
  {
    ip_addr = RSI_CONFIG_AP_IP_ADDRESS;
    mask    = RSI_CONFIG_AP_SN_MASK_ADDRESS;
    gw      = RSI_CONFIG_AP_GATEWAY_ADDRESS;

		ap_profile_ptr = (ap_profile_t *)profile_buffer;

    nwk_profile_ptr = &ap_profile_ptr->network_profile;

    memset(ap_profile_ptr, 0, sizeof(ap_profile_t));

    //! Fill AP wlan feature bitmap
    rsi_uint32_to_4bytes(ap_profile_ptr->wlan_feature_bit_map, RSI_CONFIG_AP_WLAN_FEAT_BIT_MAP);

    //! Fill AP TCP_IP feature bitmap
    rsi_uint32_to_4bytes(ap_profile_ptr->tcp_ip_feature_bit_map, RSI_CONFIG_AP_TCP_IP_FEAT_BIT_MAP);

    //! Fill AP custom feature bitmap
    rsi_uint32_to_4bytes(ap_profile_ptr->custom_feature_bit_map, RSI_CONFIG_AP_CUSTOM_FEAT_BIT_MAP);

    //! Fill data rate
    ap_profile_ptr->data_rate = RSI_CONFIG_AP_DATA_RATE;

    //! Fill tx power
    ap_profile_ptr->tx_power =  RSI_CONFIG_AP_TX_POWER;

    //! Fill AP band
    ap_profile_ptr->band    =  RSI_CONFIG_AP_BAND;

    //! Fill AP channel number
    rsi_uint16_to_2bytes(ap_profile_ptr->channel, RSI_CONFIG_AP_CHANNEL);

    //! Fill AP SSID
    rsi_strcpy(ap_profile_ptr->ssid, RSI_CONFIG_AP_SSID);

    //! Fill AP security type
    ap_profile_ptr->security_type = RSI_CONFIG_AP_SECURITY_TYPE;

    //! Fill AP encryption type
    ap_profile_ptr->encryption_type = RSI_CONFIG_AP_ENCRYPTION_TYPE;

    //! Fill AP PSK
    if(RSI_CONFIG_AP_PSK)
      rsi_strcpy(ap_profile_ptr->psk, RSI_CONFIG_AP_PSK);

    //! Fill beacon interval
    rsi_uint16_to_2bytes(ap_profile_ptr->beacon_interval, RSI_CONFIG_AP_BEACON_INTERVAL);

    //! Fill DTIM period
    rsi_uint16_to_2bytes(ap_profile_ptr->dtim_period, RSI_CONFIG_AP_DTIM);

    //! Fill AP keep alive time
    ap_profile_ptr->keep_alive_type = RSI_CONFIG_AP_KEEP_ALIVE_TYPE;

    //! Fill AP keep alive counter
    ap_profile_ptr->keep_alive_counter = RSI_CONFIG_AP_KEEP_ALIVE_COUNTER;

    //! Fill AP max number of stations
    rsi_uint16_to_2bytes(ap_profile_ptr->max_no_sta, RSI_CONFIG_AP_MAX_STATIONS_COUNT);


    nwk_profile_ptr->tcp_stack_used = RSI_CONFIG_AP_TCP_STACK_USED;

    if(nwk_profile_ptr->tcp_stack_used == 1)
    {
      nwk_profile_ptr->dhcp_enable = 0;

      //! Fill IP address
      memcpy(nwk_profile_ptr->ip_address, (uint8_t *)&ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      //! Fill network mask
      memcpy(nwk_profile_ptr->sn_mask, (uint8_t *)&mask, RSI_IPV4_ADDRESS_LENGTH);

      //! Fill gateway
      memcpy(nwk_profile_ptr->default_gw, (uint8_t *)&gw, RSI_IPV4_ADDRESS_LENGTH);
    }
    else
    {
      //! Not supported
    }

  }
  else if(type == RSI_WLAN_PROFILE_CLIENT)
  {
    ip_addr = RSI_CONFIG_CLIENT_IP_ADDRESS;
    mask    = RSI_CONFIG_CLIENT_SN_MASK_ADDRESS;
    gw      = RSI_CONFIG_CLIENT_GATEWAY_ADDRESS;

    client_profile_ptr = (client_profile_t *)profile_buffer;

    nwk_profile_ptr = &client_profile_ptr->network_profile;

    memset(client_profile_ptr, 0, sizeof(client_profile_t));

    //! Fill wlan feature bitmap
    rsi_uint32_to_4bytes(client_profile_ptr->wlan_feature_bit_map, RSI_CONFIG_CLIENT_WLAN_FEAT_BIT_MAP);

    //! Fill TCP_IP feature bitmap
    rsi_uint32_to_4bytes(client_profile_ptr->tcp_ip_feature_bit_map, RSI_CONFIG_CLIENT_TCP_IP_FEAT_BIT_MAP);

    //! Fill custom feature bitmap
    rsi_uint32_to_4bytes(client_profile_ptr->custom_feature_bit_map, RSI_CONFIG_CLIENT_CUSTOM_FEAT_BIT_MAP);

    //! Fill listen interval
    rsi_uint32_to_4bytes(client_profile_ptr->listen_interval, RSI_CONFIG_CLIENT_LISTEN_INTERVAL);

    //! Fill data rate
    client_profile_ptr->data_rate = RSI_CONFIG_CLIENT_DATA_RATE;

    //! Fill tx power
    client_profile_ptr->tx_power =  RSI_CONFIG_CLIENT_TX_POWER;

    //! Fill AP band
    client_profile_ptr->band    =  RSI_CONFIG_CLIENT_BAND;

    //! Fill Client SSID
    rsi_strcpy(client_profile_ptr->ssid, RSI_CONFIG_CLIENT_SSID);

    //! Fill AP SSID length
    client_profile_ptr->ssid_len = strlen(RSI_CONFIG_CLIENT_SSID);

    //! Fill scan feature bitmap
    client_profile_ptr->scan_feature_bitmap = RSI_CONFIG_CLIENT_SCAN_FEAT_BITMAP;

    //! Fill Client channel number
    rsi_uint16_to_2bytes(client_profile_ptr->channel, RSI_CONFIG_CLIENT_CHANNEL);

    //! Scan channel bitmap magic code
    rsi_uint16_to_2bytes(client_profile_ptr->scan_chan_bitmap_magic_code, RSI_CONFIG_CLIENT_MAGIC_CODE);

    //! Scan channel bitmap for 2_4 Ghz
    rsi_uint32_to_4bytes(client_profile_ptr->scan_chan_bitmap_2_4_ghz, RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_2_4_GHZ);

    //! Scan channel bitmap for 5_0 Ghz
    rsi_uint32_to_4bytes(client_profile_ptr->scan_chan_bitmap_5_0_ghz, RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_5_0_GHZ);

    //! Fill Client security type
    client_profile_ptr->security_type = RSI_CONFIG_CLIENT_SECURITY_TYPE;

    //! Fill AP encryption type
    client_profile_ptr->encryption_type = RSI_CONFIG_CLIENT_ENCRYPTION_TYPE;

    //! Fill Client PSK
    if(RSI_CONFIG_CLIENT_PSK)
      rsi_strcpy(client_profile_ptr->psk, RSI_CONFIG_CLIENT_PSK);

    //! Fill Client PMK
    if(strlen(RSI_CONFIG_CLIENT_PMK))
    {
      for(i=0 ,j=0 ; i < 64 && j < 32; i+=2,j++)
      {
        if(RSI_CONFIG_CLIENT_PMK[i] && RSI_CONFIG_CLIENT_PMK[i + 1])
        {
          client_profile_ptr->pmk[j]  = ((uint16_t )convert_lower_case_to_upper_case(RSI_CONFIG_CLIENT_PMK[i])) * 16;
          client_profile_ptr->pmk[j] += convert_lower_case_to_upper_case(RSI_CONFIG_CLIENT_PMK[i + 1]);
        }
        else
        {
          client_profile_ptr->pmk[j]=0;
        }
      }
    }

    nwk_profile_ptr->tcp_stack_used = RSI_CONFIG_CLIENT_TCP_STACK_USED;

    if(nwk_profile_ptr->tcp_stack_used == 1)
    {
      nwk_profile_ptr->dhcp_enable = RSI_CONFIG_CLIENT_DHCP_MODE;

      //! Fill IP address
      memcpy(nwk_profile_ptr->ip_address, (uint8_t *)&ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      //! Fill network mask
      memcpy(nwk_profile_ptr->sn_mask, (uint8_t *)&mask, RSI_IPV4_ADDRESS_LENGTH);

      //! Fill gateway
      memcpy(nwk_profile_ptr->default_gw, (uint8_t *)&gw, RSI_IPV4_ADDRESS_LENGTH);
    }
    else
    {
      //! Not supported
    }

  }
  else if(type == RSI_WLAN_PROFILE_EAP)
  {
    ip_addr = RSI_CONFIG_EAP_IP_ADDRESS;
    mask    = RSI_CONFIG_EAP_SN_MASK_ADDRESS;
    gw      = RSI_CONFIG_EAP_GATEWAY_ADDRESS;

    eap_client_profile_ptr = (eap_client_profile_t *)profile_buffer;

    nwk_profile_ptr = &eap_client_profile_ptr->network_profile;

    memset(eap_client_profile_ptr, 0, sizeof(eap_client_profile_t));

    //! Fill wlan feature bitmap
    rsi_uint32_to_4bytes(eap_client_profile_ptr->wlan_feature_bit_map, RSI_CONFIG_EAP_WLAN_FEAT_BIT_MAP);

    //! Fill TCP_IP feature bitmap
    rsi_uint32_to_4bytes(eap_client_profile_ptr->tcp_ip_feature_bit_map, RSI_CONFIG_EAP_TCP_IP_FEAT_BIT_MAP);

    //! Fill custom feature bitmap
    rsi_uint32_to_4bytes(eap_client_profile_ptr->custom_feature_bit_map, RSI_CONFIG_EAP_CUSTOM_FEAT_BIT_MAP);

    //! Fill data rate
    eap_client_profile_ptr->data_rate = RSI_CONFIG_EAP_DATA_RATE;

    //! Fill tx power
    eap_client_profile_ptr->tx_power =  RSI_CONFIG_EAP_TX_POWER;

    //! Fill band
    eap_client_profile_ptr->band    =  RSI_CONFIG_EAP_BAND;

    //! Fill Client SSID
    rsi_strcpy(eap_client_profile_ptr->ssid, RSI_CONFIG_EAP_SSID);

    //! Fill AP SSID length
    eap_client_profile_ptr->ssid_len = strlen(RSI_CONFIG_EAP_SSID);

    //! Fill scan feature bitmap
    eap_client_profile_ptr->scan_feature_bitmap = RSI_CONFIG_EAP_SCAN_FEAT_BITMAP;

    //! Fill security type
    eap_client_profile_ptr->security_type = RSI_CONFIG_EAP_SECURITY_TYPE;

    //! Fill Client channel number
    rsi_uint16_to_2bytes(eap_client_profile_ptr->channel, RSI_CONFIG_EAP_CHANNEL);

    //! Scan channel bitmap magic code
    rsi_uint16_to_2bytes(eap_client_profile_ptr->scan_chan_bitmap_magic_code, RSI_CONFIG_EAP_CHAN_MAGIC_CODE);

    //! Scan channel bitmap for 2_4 Ghz
    rsi_uint32_to_4bytes(eap_client_profile_ptr->scan_chan_bitmap_2_4_ghz, RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_2_4_GHZ);

    //! Scan channel bitmap for 5_0 Ghz
    rsi_uint32_to_4bytes(eap_client_profile_ptr->scan_chan_bitmap_5_0_ghz, RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_5_0_GHZ);

    //! Fill EAP method
    rsi_strcpy(eap_client_profile_ptr->eap_method, RSI_CONFIG_EAP_METHOD);

    //! Fill Inner method
    rsi_strcpy(eap_client_profile_ptr->inner_method, RSI_CONFIG_EAP_INNER_METHOD);

    //! Fill User identity
    rsi_strcpy(eap_client_profile_ptr->user_identity, RSI_CONFIG_EAP_USER_IDENTITY);

    //! Fill Password
    rsi_strcpy(eap_client_profile_ptr->passwd, RSI_CONFIG_EAP_PASSWORD);


    nwk_profile_ptr->tcp_stack_used = RSI_CONFIG_EAP_TCP_STACK_USED;

    if(nwk_profile_ptr->tcp_stack_used == 1)
    {
      nwk_profile_ptr->dhcp_enable = RSI_CONFIG_EAP_DHCP_MODE;

      //! Fill IP address
      memcpy(nwk_profile_ptr->ip_address, (uint8_t *)&ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      //! Fill network mask
      memcpy(nwk_profile_ptr->sn_mask, (uint8_t *)&mask, RSI_IPV4_ADDRESS_LENGTH);

      //! Fill gateway
      memcpy(nwk_profile_ptr->default_gw, (uint8_t *)&gw, RSI_IPV4_ADDRESS_LENGTH);
    }
    else
    {
      //! Not supported
    }
  }
  else if(type == RSI_WLAN_PROFILE_P2P)
  {
    ip_addr = RSI_CONFIG_P2P_IP_ADDRESS;
    mask    = RSI_CONFIG_P2P_SN_MASK_ADDRESS;
    gw      = RSI_CONFIG_P2P_GATEWAY_ADDRESS;

    p2p_profile_ptr = (p2p_profile_t *)profile_buffer;

    nwk_profile_ptr = &p2p_profile_ptr->network_profile;

    memset(p2p_profile_ptr, 0, sizeof(p2p_profile_t));

    //! Fill wlan feature bitmap
    rsi_uint32_to_4bytes(p2p_profile_ptr->wlan_feature_bit_map, RSI_CONFIG_P2P_WLAN_FEAT_BIT_MAP);

    //! Fill TCP_IP feature bitmap
    rsi_uint32_to_4bytes(p2p_profile_ptr->tcp_ip_feature_bit_map, RSI_CONFIG_P2P_TCP_IP_FEAT_BIT_MAP);

    //! Fill custom feature bitmap
    rsi_uint32_to_4bytes(p2p_profile_ptr->custom_feature_bit_map, RSI_CONFIG_P2P_CUSTOM_FEAT_BIT_MAP);

    //! Fill Join SSID
    rsi_strcpy(p2p_profile_ptr->join_ssid, RSI_CONFIG_P2P_JOIN_SSID);

    //! Fill data rate
    p2p_profile_ptr->data_rate = RSI_CONFIG_CLIENT_DATA_RATE;

    //! Fill tx power
    p2p_profile_ptr->tx_power =  RSI_CONFIG_CLIENT_TX_POWER;

    //! Fill AP band
    p2p_profile_ptr->band    =  RSI_CONFIG_CLIENT_BAND;

    //! Fill GO Intnent
    rsi_uint16_to_2bytes(p2p_profile_ptr->go_intent, RSI_CONFIG_P2P_GO_INTNET);

    //! Fill Device name
    rsi_strcpy(p2p_profile_ptr->device_name,RSI_CONFIG_P2P_DEVICE_NAME);

    //! Fill operating channel
    rsi_uint16_to_2bytes(p2p_profile_ptr->operating_channel, RSI_CONFIG_P2P_OPERATING_CHANNEL);

    //! Fill SSID postfix
    rsi_strcpy(p2p_profile_ptr->ssid_postfix, RSI_CONFIG_P2P_SSID_POSTFIX);

    //! Fill PSK key
    rsi_strcpy(p2p_profile_ptr->psk_key, RSI_CONFIG_P2P_PSK_KEY);

    nwk_profile_ptr->tcp_stack_used = RSI_CONFIG_P2P_TCP_STACK_USED;

    if(nwk_profile_ptr->tcp_stack_used == 1)
    {
      nwk_profile_ptr->dhcp_enable = RSI_CONFIG_P2P_DHCP_MODE;

      //! Fill IP address
      memcpy(nwk_profile_ptr->ip_address, (uint8_t *)&ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      //! Fill network mask
      memcpy(nwk_profile_ptr->sn_mask, (uint8_t *)&mask, RSI_IPV4_ADDRESS_LENGTH);

      //! Fill gateway
      memcpy(nwk_profile_ptr->default_gw, (uint8_t *)&gw, RSI_IPV4_ADDRESS_LENGTH);
    }
    else
    {
      //! Not supported
    }
  }
  else
  {
    profile_buffer = NULL;
  }

  return profile_buffer;
}

/*==============================================*/
/**
 * @fn         int32_t rsi_wlan_delete_profile(uint32_t type)
 * @brief      This function is used to send raw data packet
 * @param[in]  type, profile type
 * @param[out] None
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function is used to send raw data packet to module
 *
 *
 */

int32_t rsi_wlan_delete_profile(uint32_t type)
{
  int32_t   status                = RSI_SUCCESS;
  rsi_pkt_t *pkt                  = NULL;
  rsi_profile_req_t  *profile_req = NULL;

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

  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_profile_req_t));

  profile_req = (rsi_profile_req_t *)pkt->data;

  //! Fill profile type
  rsi_uint32_to_4bytes(profile_req->profile_type, type);

  //! send delete profile command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DELETE_PROFILE, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}


/*==============================================*/
/**
 * @fn         rsi_wlan_enable_auto_config(uint8_t enable, uint32_t profile_type)
 * @brief      This function is used to send raw data packet
 * @param[in]  enable, to enable/disable the profile configuration
 * @param[in]  type, profile type
 * @param[out] None
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function is used to send raw data packet to module
 *
 *
 */

int32_t rsi_wlan_enable_auto_config(uint8_t enable, uint32_t type)
{
  int32_t   status                               = RSI_SUCCESS;
  rsi_pkt_t *pkt                                 = NULL;
  rsi_auto_config_enable_t  *config_enable_req   = NULL;

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

  //! Memset buffer
  memset(&pkt->data, 0, sizeof(rsi_auto_config_enable_t));

  config_enable_req = (rsi_auto_config_enable_t *)pkt->data;

  //! Fill config profile enable/disable
  config_enable_req->config_enable = enable;

  //! Fill profile type
  rsi_uint32_to_4bytes(config_enable_req->profile_type, type);

  //! send delete profile command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_AUTO_CONFIG_ENABLE, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}




/*====================================================*/
/**
 * @fn          uint16_t rsi_wlan_register_callbacks(uint32_t callback_id, void (*callback_handler_ptr)((uint16_t status, const uint8_t *buffer, const uint16_t length))
 * @brief       Registers the call backs
 * @param[in]   uint32_t callback_id, call back number which needs to be registered
 * @param[in]   void (*callback_handler_ptr)(void), call back handler which needs to be registered for a given call back
 * @return
 *                1 -  error
 *                0 - Success
 * @section description
 * This function is used to register the call back
 */
uint16_t rsi_wlan_register_callbacks(uint32_t callback_id, void (*callback_handler_ptr)(uint16_t status, uint8_t *buffer, const uint32_t length))
{
  if(callback_id > RSI_MAX_NUM_CALLBACKS)
  {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    return 1;
  }
  if(callback_id == RSI_JOIN_FAIL_CB)  //! check for NULL or not
  {
    //! Register join fail call back handler
    rsi_driver_cb->wlan_cb->callback_list.join_fail_handler = callback_handler_ptr;
  }
  else if(callback_id == RSI_IP_FAIL_CB)
  {
    //! Register ip renewal fail call back handler
    rsi_driver_cb->wlan_cb->callback_list.ip_renewal_fail_handler = callback_handler_ptr;
  }
  else if(callback_id == RSI_REMOTE_SOCKET_TERMINATE_CB)
  {
    //! Register remote socket terminate call back handler
    rsi_driver_cb->wlan_cb->callback_list.remote_socket_terminate_handler = callback_handler_ptr;
  }
  else if(callback_id == RSI_IP_CHANGE_NOTIFY_CB)
  {
    //! Register ip change notify call back handler
    rsi_driver_cb->wlan_cb->callback_list.ip_change_notify_handler = callback_handler_ptr;
  }
  else if(callback_id == RSI_STATIONS_CONNECT_NOTIFY_CB)
  {
    //! Register station connect notify call back handler
    rsi_driver_cb->wlan_cb->callback_list.stations_connect_notify_handler = callback_handler_ptr;
  }
  else if(callback_id == RSI_STATIONS_DISCONNECT_NOTIFY_CB)
  {
    //! Register station disconnect notify call back handler
    rsi_driver_cb->wlan_cb->callback_list.stations_disconnect_notify_handler = callback_handler_ptr;
  }
  else if(callback_id == RSI_WLAN_DATA_RECEIVE_NOTIFY_CB)
  {
    //! Register wlan data packet receive notify call back handler
    rsi_driver_cb->wlan_cb->callback_list.wlan_data_receive_handler = callback_handler_ptr;
  }
  else if(callback_id == RSI_WLAN_RECEIVE_STATS_RESPONSE_CB)
  {
    //! Register wlan receive stats notify call back handler
    rsi_driver_cb->wlan_cb->callback_list.wlan_receive_stats_response_handler = callback_handler_ptr;
  }
  else if(callback_id == RSI_WLAN_WFD_DISCOVERY_NOTIFY_CB)
  {
    //! Register wifi direct device discovery notify call back handler
    rsi_driver_cb->wlan_cb->callback_list.wlan_wfd_discovery_notify_handler = callback_handler_ptr;
  }
  else if(callback_id == RSI_WLAN_WFD_CONNECTION_REQUEST_NOTIFY_CB)
  {
    //! Register wifi direct connection request notify call back handler
    rsi_driver_cb->wlan_cb->callback_list.wlan_wfd_connection_request_notify_handler = callback_handler_ptr;
  }
  else if(callback_id == RSI_WLAN_RAW_DATA_RECEIVE_HANDLER)
  {
    //! Register raw data receive notify call back handler
    rsi_driver_cb->wlan_cb->callback_list.raw_data_receive_handler = callback_handler_ptr;

  }
  return 0;
}




