/**
 * @file     rsi_pkt_mgmt.c
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
 *  @brief : This fils contains packet management functionality
 *
 *  @section Description  This file contains packet management functionality
 *
 *
 */
/**
 * Include files
 * */
#include "rsi_driver.h"

/*==============================================*/
/**
 * @fn         int32_t rsi_pkt_pool_init(rsi_pkt_pool_t *pool_cb, uint8_t *buffer, uint32_t total_size, uint32_t pkt_size)
 * @brief      Initilalizes packet pool
 * @param[in]  pool_cb , packet pool pointer which needs to be initialized
 * @param[in]  buffer  , buffer pointer to create pool
 * @param[in]  total_size , size of pool to create
 * @param[in]  pkt_size , size of each packet in pool
 * @param[out] None
 * @return
 *             <0  - If fails
 *             =0  - If success
 *
 * @section description
 * This function initializes/creates packet pool from the provided buffer
 *
 *
 */
int32_t rsi_pkt_pool_init(rsi_pkt_pool_t *pool_cb, uint8_t *buffer, uint32_t total_size, uint32_t pkt_size)
{
  uint32_t pool_size;
  uint32_t number_of_pkts;
  uint32_t buffer_index = 1;

  //! Memset pool structure
  memset(pool_cb, 0, sizeof(rsi_pkt_pool_t));

  //! Calculate number of packets can be created with given buffer
  number_of_pkts = (total_size / pkt_size);

  //! Increase number of packet by 1 to not use zeroth index
  number_of_pkts++;

  if((total_size - (number_of_pkts * 4)) < pkt_size)
  {
    //! If atleast one packet accomadation is not possible then return error
    return -1;
  }

  //! Save pool pointer
  pool_cb->pool = (void *)buffer;

  //! Calculate pool array size
  pool_size = (number_of_pkts * 4);

  //! Increase buffer pointer by pool size
  //! Buffer contains pool array then actual buffer
  buffer += pool_size;

  //! Decrease total size by pool size
  total_size -= pool_size;

  //! While total size is available fill pool array
  while(total_size > pkt_size)
  {
    //! Fill buffer pointer to current index
    pool_cb->pool[buffer_index++] = buffer;

    //! Increase available count
    pool_cb->avail++;

    //! Increase pool size
    pool_cb->size++;

    //! Move buffer pointer to point next buffer
    buffer += pkt_size;

    //! Decrease total size by packet size
    total_size -= pkt_size;
  }

  //! Return success
  return 0;
}

/*==============================================*/
/**
 * @fn          rsi_pkt_t * rsi_pkt_alloc(rsi_pkt_pool_t *pool_cb)
 * @brief       Allocates packet from packet pool
 * @param[in]   pool_cb , packet pool pointer from which packet needs to be allocated
 * @param[out]  None
 * @return
 *              = NULL - If allocation fails
 *              = Allocated packet pointer if success
 *
 *
 * @section description
 * This function allocates packet from a given packet pool
 *
 */

rsi_pkt_t * rsi_pkt_alloc(rsi_pkt_pool_t *pool_cb)
{
  rsi_reg_flags_t flags;
  rsi_pkt_t       *pkt = NULL;

  //! Disable interrupts
  flags = rsi_critical_section_entry();

  if(pool_cb->avail == 0)
  {
    //! Retore interrupts
    rsi_critical_section_exit(flags);

    //! If available count is zero return NULL
    return NULL;
  }

  //! If packets are available then return packet pointer from pool
  pkt = (rsi_pkt_t *)(pool_cb->pool[pool_cb->avail]);

  //! Decrease available count
  pool_cb->avail--;

  //! initialize next with NULL
  pkt->next = NULL;

  //! Retore interrupts
  rsi_critical_section_exit(flags);

  //! Return pakcet pointer
  return pkt;
}

/*==============================================*/
/**
 * @fn         int32_t rsi_pkt_free(rsi_pkt_pool_t *pool_cb, rsi_pkt_t *pkt)
 * @brief      Frees the packet
 * @param[in]  pool_cb, packet pool to which packet needs to be freed
 * @param[in]  pkt    , packet pointer which needs to be freed
 * @param[out] None
 * @return
 *             <0 - If fails
 *             =0 - If success
 *
 *
 * @section description
 * This function frees the packet
 *
 */

int32_t rsi_pkt_free(rsi_pkt_pool_t *pool_cb,rsi_pkt_t *pkt)
{
  rsi_reg_flags_t flags;

  //! Disable interrupts
  flags = rsi_critical_section_entry();

  if(pool_cb->avail == pool_cb->size)
  {
    //! Retore interrupts
    rsi_critical_section_exit(flags);

    //! If available and size are equal then return an error
    return -1;
  }

  //! Increase pool available count
  pool_cb->avail++;

  //! Fill packet pinter into pool array
  pool_cb->pool[pool_cb->avail] = pkt;

  //! Retore interrupts
  rsi_critical_section_exit(flags);

  return RSI_SUCCESS;
}

/*==============================================*/
/**
 * @fn         uint32_t rsi_is_pkt_available(rsi_pkt_pool_t *pool_cb)
 * @brief      Returns number of packets available in a pool
 * @param[in]  pool_cb , pool pointer to check available pakcets count
 * @param[out] None
 * @return
 *             >=0 Number of packet in pool
 *
 * @section description
 * This function returns number of avaiable packets in pool
 *
 *
 */

uint32_t rsi_is_pkt_available(rsi_pkt_pool_t *pool_cb)
{
  rsi_reg_flags_t flags;
  uint32_t        available_count = 0;

  //! Disable interrupts
  flags = rsi_critical_section_entry();

  //! Get available packet count
  available_count = pool_cb->avail;

  //! Retore interrupts
  rsi_critical_section_exit(flags);

  //! Return available count
  return available_count;

}
