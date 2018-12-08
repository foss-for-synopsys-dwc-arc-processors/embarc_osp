/**
 * @file      rsi_timer.h
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
 *  @brief :  This file contains the timer wrappers related functionality defines and function definations
 *
 *  @section Description  This file contains the timer wrappers related functionality defines and function definations
 *
 *
 */

#ifndef  RSI_TIMER_H
#define  RSI_TIMER_H
/******************************************************
 * *                      Macros
 * ******************************************************/
#define RSI_TIMER_NODE_0 0
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
typedef uint32_t rsi_timer_instance_t;
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
void rsi_timer_expiry_interrupt_handler(void);
uint32_t rsi_timer_read_counter(void);
void rsi_init_timer(rsi_timer_instance_t *rsi_timer, uint32_t duration);
int32_t rsi_timer_expired(rsi_timer_instance_t *timer);
uint32_t rsi_timer_left(rsi_timer_instance_t *timer);

#endif
