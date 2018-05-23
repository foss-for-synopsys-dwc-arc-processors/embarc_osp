/**
 * @file        rsi_events.h
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
 *  @brief :   This file contains events and event handlers prototyoes and defines used in driver
 *
 *  @section Description  This file contains events and event handlers prototyoes and defines used in driver
 *
 *
 */
#ifndef RSI_EVENTS_H
#define RSI_EVENTS_H
/******************************************************
 * *                      Macros
 * ******************************************************/
//! Max number events used in the driver
#define RSI_MAX_NUM_EVENTS             2

//! RX event number used in the driver
#define RSI_RX_EVENT                   0

//! TX event number used in the driver
#define RSI_TX_EVENT                   1

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

//! event handler structure
typedef struct rsi_event_cb_s
{
  //! event handler pointer
  void   (*event_handler)(void);
}rsi_event_cb_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

uint16_t rsi_register_event(uint32_t event_id, void (*event_handler_ptr)(void));
uint32_t rsi_find_event(uint32_t event_map);
void rsi_set_event(uint32_t event_num);
void rsi_clear_event(uint32_t event_num);
void rsi_mask_event(uint32_t event_num);
void rsi_unmask_event(uint32_t event_num);
void rsi_tx_event_handler(void);
void rsi_rx_event_handler(void);
void rsi_events_init(void);

#endif
