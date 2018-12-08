/**
 * @file      rsi_mfi.c
 * @version   0.1
 * @date      15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains IAP co processor accessing API's
 *
 *  @section Description   This file contains IAP co processor accessing API's
 *
 *
 */

#include "rsi_driver.h"

/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   ,
 * @param[out]
 * @return
 *
 *
 * @section description
 * This
 *
 *
 */

int8_t rsi_iap_init()
{

  rsi_pkt_t       *pkt;

  uint8_t status = 0;

  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  //! take lock on common control block
  rsi_mutex_lock(&rsi_common_cb->common_mutex);

  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&rsi_common_cb->common_mutex);

    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  //! send power save command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_IAP_INIT, pkt);

  //! free the packet
  rsi_pkt_free(&rsi_common_cb->common_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&rsi_common_cb->common_mutex);

  //! return status
  return status;
}



/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   ,
 * @param[out]
 * @return
 *
 *
 * @section description
 * This
 *
 *
 */

int8_t rsi_iap_generate_signature(uint16_t digest_length, int8_t *digest, uint16_t buffer_len, uint8_t *buffer)
{


  rsi_pkt_t       *pkt;
  uint8_t          status = 0;
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;
  rsi_mfi_auth_create_request_t *auth_req = NULL;


  //! take lock on common control block
  rsi_mutex_lock(&rsi_common_cb->common_mutex);

  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&rsi_common_cb->common_mutex);

    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  //! Memset the packet
  memset(pkt,0,(digest_length + 4));

  auth_req  =  (rsi_mfi_auth_create_request_t *)pkt->data;

  //! copy Digest data length
  rsi_uint32_to_4bytes(auth_req->digest_length,digest_length);

  //! Copy Digest
  memcpy(auth_req->digest, digest, digest_length);

  //! point the buffer to the app buffer
  rsi_common_cb->app_buffer = buffer;

  rsi_common_cb->app_buffer_length = buffer_len;

  //! send IAP generate signature  command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_IAP_GENERATE_SIGATURE, pkt);

  //! free the packet
  rsi_pkt_free(&rsi_common_cb->common_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&rsi_common_cb->common_mutex);

  //! return status
  return status;


}


/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   ,
 * @param[out]
 * @return
 *
 *
 * @section description
 * This
 *
 *
 */
int8_t rsi_iap_get_certificate(uint16_t buffer_len, uint8_t *buffer)
{

  rsi_pkt_t       *pkt;

  uint8_t status = 0;

  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  //! take lock on common control block
  rsi_mutex_lock(&rsi_common_cb->common_mutex);

  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&rsi_common_cb->common_mutex);

    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  //! point the buffer to the app buffer
  rsi_common_cb->app_buffer = buffer;

  rsi_common_cb->app_buffer_length = buffer_len;

  //! send IAP generate signature  command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_IAP_GET_CERTIFICATE, pkt);

  //! free the packet
  rsi_pkt_free(&rsi_common_cb->common_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&rsi_common_cb->common_mutex);

  //! return status
  return status;

}
