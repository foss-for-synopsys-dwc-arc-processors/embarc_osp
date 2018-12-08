/**
 * @file     rsi_bt_common_apis.h
 * @version  0.1
 * @date     01 Oct 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contain definitions and declarations of BT common APIs.
 *
 *  @section Description  This file contains definitions and declarations of common
 *  functions required to configure both BLE and BT Classic modules.
 *
 */

#ifndef RSI_BT_COMMON_APIS_H
#define RSI_BT_COMMON_APIS_H

#include<rsi_data_types.h>
#include<rsi_utils.h>

/******************************************************
 * *                      Macros
 * ******************************************************/
//! success return value
#define RSI_SUCCESS                                   0

//! failure return value
#define RSI_FAILURE                                  -1


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
 * *              BT Common API's Declarations
 * ******************************************************/
/*==============================================*/
/**
 * @fn         rsi_bt_set_local_name
 * @brief      sets the local BT/BLE device name
 * @param[in]  local_name, name to be set to the local BT/BLE device
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function sets the given name to the local BT/BLE device
 */
int32_t rsi_bt_set_local_name(int8_t *local_name);

/*==============================================*/
/**
 * @fn         rsi_bt_get_local_name
 * @brief      requests the local BT/BLE device name
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t
 *             0  = success
 *             !0 = failure
 * @section description
 * This function is used know the name of the local BT/BLE device
 */
int32_t rsi_bt_get_local_name(rsi_bt_resp_get_local_name_t *bt_resp_get_local_name);

/*==============================================*/
/**
 * @fn         rsi_bt_get_rssi
 * @brief      request the RSSI of the connected BT/BLE device
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used know the RSSI of the connected BT/BLE device
 */
int32_t rsi_bt_get_rssi(int8_t *remote_dev_addr, uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_get_local_device_address
 * @brief      request the local BT/BLE device address
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t
 *             0  = success
 *             !0 = failure
 * @section description
 * This function is used to know the local BT/BLE device address
 */
int32_t rsi_bt_get_local_device_address(uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_init
 * @brief      Initializes the BT/BLE device
 * @param[in]  void
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to initialise the BT/BLE device.
 * Its recommended to use this API after rsi_bt_deinit API
 */
int32_t rsi_bt_init(void);

/*==============================================*/
/**
 * @fn         rsi_bt_deinit
 * @brief      de-initializes/stops the BT/BLE device
 * @param[in]  void
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to de-initialize the BT/BLE device.
 * rsi_bt_init API shall be used after rsi_bt_deinit API
 */
int32_t rsi_bt_deinit(void);

/*==============================================*/
/**
 * @fn         rsi_bt_set_antenna
 * @brief      sets the antenna of the local BT/BLE device
 * @param[in]  antenna_value, either internal/external antenna
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to select either internal/external antenna of the
 * BT/BLE device
 */
int32_t rsi_bt_set_antenna(uint8_t antenna_value);

/*==============================================*/
/**
 * @fn         rsi_bt_set_feature_bitmap
 * @brief      set or enable the  BT or BLE features at firmware
 * @param[in]  feature_bit_map, features bit map list
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set or select the features bit map
 */
int32_t rsi_bt_set_feature_bitmap(uint32_t feature_bit_map);


/*==============================================*/
/**
 * @fn         rsi_bt_set_antenna_tx_power_level
 * @brief      sets the antenna transmit power of the local BT/BLE device
 * @param[in]  protocol_mode, BT or LE
 *             power_level,
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 */
int32_t rsi_bt_set_antenna_tx_power_level(uint8_t protocol_mode, int8_t tx_power);


#endif
