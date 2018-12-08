/**
 * @file        rsi_pkt_mgmt.h
 * @version     0.1
 * @date        15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains packet structure prototype and packet management
 *           related function prototypes
 *
 *  @section Description  This file contains packet structure prototype and packet management
 *           related function prototypes
 *
 *
 */

#ifndef RSI_PKT_MGMT_H
#define RSI_PKT_MGMT_H
/******************************************************
 * *                      Macros
 * ******************************************************/
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
//! driver TX/RX packet structure
typedef struct rsi_pkt_s
{
  //! next packet pointer
  struct rsi_pkt_s *next;

  //! host descriptor
  uint8_t desc[16];

  //! payload
  uint8_t data[1];
}rsi_pkt_t;

//! packet pool structure
typedef struct rsi_pkt_pool_s
{
  //! Pool total packets count
  uint16_t size;

  //! Pool avaialble packets count
  uint16_t avail;

  //! Pool pointer
  void     **pool;

}rsi_pkt_pool_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_pkt_pool_init(rsi_pkt_pool_t *pool_cb, uint8_t *buffer, uint32_t total_size, uint32_t pkt_size);
rsi_pkt_t * rsi_pkt_alloc(rsi_pkt_pool_t *pool_cb);
int32_t rsi_pkt_free(rsi_pkt_pool_t *pool_cb,rsi_pkt_t *pkt);
uint32_t rsi_is_pkt_available(rsi_pkt_pool_t *pool_cb);
#endif
