/**
 * @file       rsi_scheduler.h
 * @version    0.1
 * @date       15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief :  This file contains scheduler functionality defines and function definations
 *            used in the driver
 *
 *  @section Description  This file contains scheduler functionality defines and function definations
 *                        used in the driver
 *
 *
 */
#ifndef RSI_SCHEDULER_H
#define RSI_SCHEDULER_H
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

//! scheduler control block structure
typedef struct rsi_scheduler_cb_s
{
  //! This bitmap is used to set and clear events
  volatile uint32_t event_map;

  //! This bitmap is used to mask and unmask events
  volatile uint32_t mask_map;

#ifdef RSI_WITH_OS
	//! scheduler semaphore
	rsi_semaphore_handle_t scheduler_sem;
#endif
}rsi_scheduler_cb_t;


/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
void rsi_scheduler_init(rsi_scheduler_cb_t *scheduler_cb);
void rsi_scheduler(rsi_scheduler_cb_t *scheduler_cb);
#endif
