/**
 * @file         rsi_user.h
 * @version      0.1
 * @date         15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains user configurable details to configure the driver blocks
 *
 *  @section Description This file contains user configurable details to configure the driver blocks
 *
 *
 */

#ifndef RSI_USER_H
#define RSI_USER_H
/******************************************************
 * *                      Macros
 * ******************************************************/

//! Default number of sockets supported,max 10 sockets are supported
#define RSI_NUMBER_OF_SOCKETS 2

//! Max packets in wlan control tx pool
#define RSI_WLAN_TX_POOL_PKT_COUNT 1

//! Max packets in bt common control tx pool
#define RSI_BT_COMMON_TX_POOL_PKT_COUNT 1

//! Max packets in bt classic control tx pool
#define RSI_BT_CLASSIC_TX_POOL_PKT_COUNT 1

//! Max packets in ble control tx pool
#define RSI_BLE_TX_POOL_PKT_COUNT 1

//! Max packets in wlan control tx pool
#define RSI_ZIGB_TX_POOL_PKT_COUNT 1

//! Max packets in common control tx pool
#define RSI_COMMON_TX_POOL_PKT_COUNT 1

//! Max packets
#define RSI_DRIVER_RX_POOL_PKT_COUNT 1

//! Little Endian is enabled
#define RSI_LITTLE_ENDIAN      1

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
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
#endif
