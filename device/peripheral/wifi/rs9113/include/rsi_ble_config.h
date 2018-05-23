/**
 * @file     rsi_ble_config.h
 * @version  0.1
 * @date     03 Sep 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contain definitions and declarations of BLE APIs.
 *
 *  @section Description  This file contains definitions and declarations required to
 *  configure BLE module.
 *
 *
 */

#ifndef RSI_BLE_CONFIG_H
#define RSI_BLE_CONFIG_H

#include<rsi_data_types.h>

/******************************************************
 * *                      Macros
 * ******************************************************/


#define RSI_BLE_SET_RAND_ADDR                       "00:23:A7:12:34:56"//   "00:15:83:6A:64:17"

#define CLEAR_WHITELIST                              0x00
#define ADD_DEVICE_TO_WHITELIST                      0x01
#define DELETE_DEVICE_FROM_WHITELIST                 0x02

//! WLAN + BLE coex mode
#ifdef SAPIS_BT_STACK_ON_HOST
#define RSI_OPERMODE_WLAN_BLE          				(13 + 16) //BT Stack bypass included
#else
#define RSI_OPERMODE_WLAN_BLE                       13
#endif

#ifdef RSI_HOMEKIT_APP
#define RSI_BLE_HOMEKIT_ENABLE                        1
#else
#define RSI_BLE_HOMEKIT_ENABLE                        0
#endif
#define RSI_BLE_MAX_NBR_ATT_REC                       20
#define RSI_BLE_MAX_NBR_ATT_SERV                      5

#define RSI_BLE_MAX_NBR_SLAVES                        1
/*=======================================================================*/
//! Advertising command parameters
/*=======================================================================*/

#define RSI_BLE_ADV_TYPE                              UNDIR_CONN
#define RSI_BLE_ADV_FILTER_TYPE                       ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY
#define RSI_BLE_ADV_DIR_ADDR_TYPE                     LE_PUBLIC_ADDRESS
#define RSI_BLE_ADV_DIR_ADDR                          "00:15:83:6A:64:17"
#ifdef RSI_HOMEKIT_APP
#define  RSI_BLE_ADV_INT_MIN                           0x80
#define  RSI_BLE_ADV_INT_MAX                           0x90
#define  RSI_BLE_ADV_CHANNEL_MAP                       0x01
#else
#define  RSI_BLE_ADV_INT_MIN                           0x800
#define  RSI_BLE_ADV_INT_MAX                           0x900
#define  RSI_BLE_ADV_CHANNEL_MAP                       0x07
#endif

//!Advertise status
//! Start the advertising process
#define RSI_BLE_START_ADV                             0x01
//! Stop the advertising process
#define RSI_BLE_STOP_ADV                              0x00

//!Advertise types

/* Advertising will be visible(discoverable) to all the devices.
 * Scanning/Connection is also accepted from all devices
 * */
#define UNDIR_CONN                                    0x80

/* Advertising will be visible(discoverable) to the particular device
 * mentioned in RSI_BLE_ADV_DIR_ADDR only.
 * Scanning and Connection will be accepted from that device only.
 * */
#define DIR_CONN                                      0x81

/* Advertising will be visible(discoverable) to all the devices.
 * Scanning will be accepted from all the devices.
 * Connection will be not be accepted from any device.
 * */
#define UNDIR_SCAN                                    0x82

/* Advertising will be visible(discoverable) to all the devices.
 * Scanning and Connection will not be accepted from any device
 * */
#define UNDIR_NON_CONN                                0x83

/* Advertising will be visible(discoverable) to the particular device
 * mentioned in RSI_BLE_ADV_DIR_ADDR only.
 * Scanning and Connection will be accepted from that device only.
 * */
#define DIR_CONN_LOW_DUTY_CYCLE                       0x84

//!Advertising flags
#define LE_LIMITED_DISCOVERABLE                       0x01
#define LE_GENERAL_DISCOVERABLE                       0x02
#define LE_BR_EDR_NOT_SUPPORTED                       0x04

//!Advertise filters
#define ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY               0x00
#define ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_ANY        0x01
#define ALLOW_SCAN_REQ_ANY_CONN_REQ_WHITE_LIST        0x02
#define ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_WHITE_LIST 0x03

//! Address types
#define LE_RANDOM_ADDRESS                             0x01
#define LE_PUBLIC_ADDRESS                             0x00

/*=======================================================================*/

/*=======================================================================*/
//! Connection parameters
/*=======================================================================*/
#define LE_SCAN_INTERVAL                             0x0100
#define LE_SCAN_WINDOW                               0x0050
#define CONNECTION_INTERVAL_MIN                      0x00A0
#define CONNECTION_INTERVAL_MAX                      0x00A0
#define CONNECTION_LATENCY                           0x0000
#define SUPERVISION_TIMEOUT                          0x07D0 //2000

/*=======================================================================*/

/*=======================================================================*/
//! Scan command parameters
/*=======================================================================*/

#define RSI_BLE_SCAN_TYPE                             SCAN_TYPE_ACTIVE
#define RSI_BLE_SCAN_FILTER_TYPE                      SCAN_FILTER_TYPE_ALL

//!Scan status
#define RSI_BLE_START_SCAN                            0x01
#define RSI_BLE_STOP_SCAN                             0x00

//!Scan types
#define SCAN_TYPE_ACTIVE                              0x01
#define SCAN_TYPE_PASSIVE                             0x00

//!Scan filters
#define SCAN_FILTER_TYPE_ALL                          0x00
#define SCAN_FILTER_TYPE_ONLY_WHITE_LIST              0x01

#define RSI_SEL_INTERNAL_ANTENNA                      0x00
#define RSI_SEL_EXTERNAL_ANTENNA                      0x01

#endif
