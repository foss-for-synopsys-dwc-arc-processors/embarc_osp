/**
 * @file     rsi_nwk.c
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
 *  @brief : This file contians networking functionality
 *
 *  @section Description  This file contains networking functionality
 *
 *
 */
/**
 * Include files
 * */
#include "rsi_driver.h"

/*==============================================*/
/**
 * @fn         int32_t rsi_driver_send_data(uint32_t sockID, uint8_t* buffer, uint32_t length, struct sockaddr *destAddr)
 * @brief      This function is used to send data packet
 * @param[in]  sockID, socket descriptor on which data needs to be send
 * @param[in]  buffer, pointer to data
 * @param[in]  length, data length
 * @param[in]  destAddr, pointer to destination socket details
 * @param[out] None
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This
 *
 *
 */

int32_t rsi_driver_send_data(uint32_t sockID, uint8_t* buffer, uint32_t length, struct rsi_sockaddr *destAddr)
{
  int32_t  status = RSI_SUCCESS;
  uint8_t  *host_desc;
  uint32_t total_send_size = 0;
  uint16_t headerLen;
  rsi_req_socket_send_t *send;

  //! Allocate packet to send data
  rsi_pkt_t *pkt = rsi_pkt_alloc(&rsi_driver_cb->wlan_cb->wlan_tx_pool);

  if(pkt == NULL)
  {
    return RSI_SOCK_ERROR;
  }

  //! Get data pointer
  send = (rsi_req_socket_send_t *)pkt->data;

  //! Get host descriptor pointer
  host_desc = pkt->desc;

  //! Memset host descriptor
  memset(host_desc, 0, RSI_HOST_DESC_LENGTH);

  //! Memset before filling
  memset(send , 0, (sizeof(rsi_req_socket_send_t) - sizeof(send->send_buffer)));

  if(((rsi_socket_pool[sockID].sock_type >> 4) & 0xF) == AF_INET)
  {
    //! Fill IP version as 4
    rsi_uint16_to_2bytes(send->ip_version, 4);

    //! Fill destination IPv4 address
    memcpy((uint8_t *)send->dest_ip_addr.ipv4_address,(uint8_t *) ntohl(&(((struct rsi_sockaddr_in *)destAddr)->sin_addr.s_addr)), RSI_IPV4_ADDRESS_LENGTH);

    //! Fill destination port
    rsi_uint16_to_2bytes(send->dest_port, ntohs(((struct rsi_sockaddr_in *)destAddr)->sin_port));


    //! Calculate header length
    if((rsi_socket_pool[sockID].sock_type & 0xF) == SOCK_STREAM)
    {
      //! TCP IPv4 header
      headerLen = RSI_TCP_FRAME_HEADER_LEN;
    }
    else
    {
      //! UDP IPv4 header
      headerLen = RSI_UDP_FRAME_HEADER_LEN;
    }
  }
  else
  {
    //! Fill IP version as 6
    rsi_uint16_to_2bytes(send->ip_version, 6);

    //! Fill destination IPv6 address

    //! Fill destination port
    rsi_uint16_to_2bytes(send->dest_port, ntohs(((struct rsi_sockaddr_in6 *)destAddr)->sin6_port));

    //! Calculate header length
    if((rsi_socket_pool[sockID].sock_type & 0xF) == SOCK_STREAM)
    {
      //! TCP IPv6 header length
      headerLen = RSI_TCP_V6_FRAME_HEADER_LEN;
    }
    else
    {
      //! UDP IPv6 header length
      headerLen = RSI_UDP_V6_FRAME_HEADER_LEN;
    }
  }

  //! if it is a web socket, setting web socket data header length
  if(rsi_socket_pool[sockID].sock_bitmap & RSI_SOCKET_FEAT_WEBS_SUPPORT)
  {
    //! If websocket is enabled
    headerLen += ((length < 126) ? 6 : 8);
  }

  //! Fill socket desriptor
  send->socket_id[0] = rsi_socket_pool[sockID].sock_id;

  //! Fill opcode used in websocket
  send->socket_id[1] = rsi_socket_pool[sockID].opcode;

  //! Fill send buffer length
  rsi_uint16_to_2bytes(send->length, length);

  //! Fill data offset
  rsi_uint16_to_2bytes(send->data_offset, headerLen);

  //! Total send length includes header length and payload length
  total_send_size = (headerLen + length);

  headerLen -= (sizeof(rsi_req_socket_send_t) - sizeof(send->send_buffer));

  //! Copy data to be sent
  memcpy(&send->send_buffer[headerLen], buffer, length);

  //! Fill host descriptor
  rsi_uint16_to_2bytes(host_desc, (total_send_size & 0xFFF));

  //! Fill packet type
  host_desc[1] |= (RSI_WLAN_DATA_Q << 4);

  //! Enqueue packet to WLAN TX queue
  rsi_enqueue_pkt(&rsi_driver_cb->wlan_tx_q, pkt);

  //! Set TX packet pending event
  rsi_set_event(RSI_TX_EVENT);

  //! Wait on wlan semaphore
  rsi_semaphore_wait(&rsi_driver_cb->wlan_cb->wlan_sem, RSI_WAIT_FOREVER);

  //! Reset expected response type
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
 * @fn         int32_t rsi_driver_process_recv_data(rsi_pkt_t *pkt)
 * @brief      Processes data receive packet
 * @param[in]  pkt, pointer to data packet
 * @param[out] None
 * @return
 *             Non zero - If fails
 *             0 - If success
 *
 * @section description
 * This function processes data receive packet
 *
 */

int32_t rsi_driver_process_recv_data(rsi_pkt_t *pkt)
{
  uint32_t data_length, data_offset;
  int8_t  sockID;
  uint8_t websocket_info = 0;
  rsi_socket_info_t *sock_info;
  rsi_rsp_socket_recv_t *recv;
  uint8_t  *host_desc;
  uint16_t payload_length,status = 0;
  int32_t frame_type=0;
  //! Get payload pointer
  recv = (rsi_rsp_socket_recv_t *)pkt->data;

  //! Get host descriptor
  host_desc = pkt->desc;

  frame_type = host_desc[2];

  //! Get payoad length
  payload_length = (rsi_bytes2R_to_uint16(host_desc) & 0xFFF);

  //! check status
  status  = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

  //! update the status in wlan_cb
  rsi_wlan_set_status(status);

  if(frame_type == 0x01)
  {
	 rsi_driver_cb->wlan_cb->callback_list.raw_data_receive_handler(0,pkt->data,payload_length);
  }
  else
  {
  //! Get socket descriptor
  sockID = rsi_get_application_socket_descriptor(recv->socket_id[0]);

  //! Get web socket information
  websocket_info = recv->socket_id[1];

  if((sockID < 0) || (sockID >= RSI_NUMBER_OF_SOCKETS))
  {
    return -1;
  }

  //! Get socket info
  sock_info = &rsi_socket_pool[sockID];

  //! Get received data length
  data_length = rsi_bytes2R_to_uint16(recv->length);

  //! Get actual data offset
  data_offset = rsi_bytes2R_to_uint16(recv->offset);

  if(!(sock_info->sock_bitmap & RSI_SOCKET_FEAT_SYNCHRONOUS))
  {

    if(sock_info->sock_receive_callback != NULL)
    {
      if((sock_info->sock_bitmap & RSI_SOCKET_FEAT_WEBS_SUPPORT))
      {
	    *(uint8_t *)(recv + data_offset - 1) = websocket_info;

        //! Call registered callback if asynchronous callback
        sock_info->sock_receive_callback(sockID, ((uint8_t *)recv + data_offset - 1), data_length + 1);
      }
      else
      {
        //! Call registered callback if asynchronous callback
        sock_info->sock_receive_callback(sockID, ((uint8_t *)recv + data_offset), data_length);
      }
    }
    return RSI_SUCCESS;
  }

#ifdef RX_BUFFER_MEM_COPY
  //! If socket receive available length is still present
  if(sock_info->sock_recv_available_length != 0)
  {
    return -1;
  }

  //! Memcopy to socket receive buffer
  memcpy(sock_info->sock_recv_buffer, ((uint8_t *)recv + data_offset), data_length);

  //! Increase socket recieve available length
  sock_info->sock_recv_available_length = data_length;
#else
  if(sock_info->recv_buffer)
  {
    //! Memcopy to socket receive buffer
    memcpy(sock_info->recv_buffer, ((uint8_t *)recv + data_offset), data_length);

    sock_info->recv_buffer_length = data_length;
  }
  else
  {
    sock_info->recv_buffer_length = 0;
  }
#endif

  if((sock_info->sock_type & 0xF) == SOCK_DGRAM)
  {
    //! Save destination port number
    sock_info->destination_port = rsi_bytes2R_to_uint16(recv->dest_port);

    //! Save destination IP address
    if((sock_info->sock_type >> 4) == AF_INET)
    {
      memcpy(sock_info->destination_ip_addr.ipv4, recv->dest_ip_addr.ipv4_address, RSI_IPV4_ADDRESS_LENGTH);
    }
    else
    {
      memcpy(sock_info->destination_ip_addr.ipv6, recv->dest_ip_addr.ipv6_address, RSI_IPV6_ADDRESS_LENGTH);
    }
  }

  //! Post semaphore for data read request
  if(rsi_driver_cb->wlan_cb->expected_response  == RSI_WLAN_RSP_SOCKET_READ_DATA)
  {
    //! Clear expected response
    rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;

    //! signal the wlan semaphore
    rsi_semaphore_post(&rsi_driver_cb->wlan_cb->wlan_sem);
  }
}
  return RSI_SUCCESS;

}


/*====================================================*/
/**
 * @fn          uint16_t rsi_wlan_register_callbacks(rsi_nwk_callback_id_t callback_id, void (*callback_handler_ptr)(void))
 * @brief       Registers the call backs
 * @param[in]   uint32_t callback_id, call back number which needs to be registered
 * @param[in]   void (*callback_handler_ptr)(void), call back handler which needs to be registered for a given call back
 * @return
 *                1 -  error
 *                0 - Success
 * @section description
 * This function is used to register the call back
 */

uint16_t rsi_nwk_register_callbacks(rsi_nwk_callback_id_t callback_id, void (*callback_handler_ptr)(uint8_t command_type , uint32_t status, const uint8_t *buffer, const uint32_t length))
{

  if(callback_id > RSI_MAX_NUM_CALLBACKS)
  {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    return 1;
  }
  if(callback_id == RSI_NWK_ERROR_CB)  //! check for NULL or not
  {
    //! Register join fail call back handler
    rsi_driver_cb->nwk_cb->nwk_callbacks.nwk_error_call_back_handler = callback_handler_ptr;
  }

  return 0;
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


uint16_t rsi_wlan_nwk_register_json_update_cb(uint32_t callback_id, void (*callback_handler_ptr)(uint8_t *filename, uint8_t *json_object, uint32_t length, uint32_t status))
{
  if(callback_id > RSI_MAX_NUM_CALLBACKS)
  {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    return 1;
  }

  //! Register json update handler
  rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_json_object_update_handler = callback_handler_ptr;

  return 0;
}



uint16_t rsi_wlan_nwk_register_json_event_cb(uint32_t callback_id, void (*callback_handler_ptr)(uint32_t status, uint8_t *json_object, uint32_t length))
{
  if(callback_id > RSI_MAX_NUM_CALLBACKS)
  {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    return 1;
  }

  //! Register json update handler
  rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_json_object_event_handler = callback_handler_ptr;

  return 0;
}

uint16_t rsi_wlan_nwk_register_webpage_req_cb(uint32_t callback_id, void (*callback_handler_ptr)(uint8_t type, uint8_t *url_name, uint8_t *post_content_buffer, uint32_t post_content_length, uint32_t status))
{
  if(callback_id > RSI_MAX_NUM_CALLBACKS)
  {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    return 1;
  }

  //! Register webpage request handler
  rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_webpage_request_handler = callback_handler_ptr;

  return 0;
}

uint16_t rsi_wlan_nwk_register_wireless_fw_upgrade_cb(uint32_t callback_id, void (*callback_handler_ptr)(uint8_t type, uint32_t status))
{
  if(callback_id > RSI_MAX_NUM_CALLBACKS)
  {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    return 1;
  }

  //! Register wireless firmware upgrade call back handler
  rsi_driver_cb->nwk_cb->nwk_callbacks.rsi_wireless_fw_upgrade_handler = callback_handler_ptr;

  return 0;
}



