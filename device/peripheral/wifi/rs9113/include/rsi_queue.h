/**
 * @file      rsi_queue.h
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
 *  @brief : This file contains queue structure prototype and packet management
 *           related function prototypes
 *
 *  @section Description: This file contains queue structure prototype and packet management
 *           related function prototypes
 *
 *
 */
#ifndef RSI_QUEUE_H
#define RSI_QUEUE_H
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
/******************************************************
 * *                    Structures
 * ******************************************************/

//! queue structure
typedef struct rsi_queue_cb_s
{
  //! queue head
  rsi_pkt_t *head;

  //! queue tail
  rsi_pkt_t *tail;

  //! number of packets pending in queue
  volatile uint16_t pending_pkt_count;

  //! mask status of the queue
  uint16_t queue_mask;

  //! Mutex handle to queues
  rsi_mutex_handle_t queue_mutex;

}rsi_queue_cb_t;

/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
extern void rsi_queues_init(rsi_queue_cb_t *queue);
extern void rsi_enqueue_pkt(rsi_queue_cb_t *queue,rsi_pkt_t *pkt);
extern rsi_pkt_t *rsi_dequeue_pkt(rsi_queue_cb_t *queue);
extern uint32_t rsi_check_queue_status(rsi_queue_cb_t *queue);
extern void rsi_block_queue(rsi_queue_cb_t *queue);
extern void rsi_unblock_queue(rsi_queue_cb_t *queue);
#endif
