/**
 * @file       rsi_bt_common.h
 * @version    0.1
 * @date       03 Sep 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief :   This file contains the BT/BLE functionality related.
 *
 *  @section Description  This file contains all the BT and BLE common command response structures.
 *
 */
#if  (defined(RSI_BT_ENABLE)	|| defined(RSI_BLE_ENABLE))
#ifndef RSI_BT_COMMON_H
#define RSI_BT_COMMON_H

#include<rsi_data_types.h>
#include<rsi_error.h>
#include<rsi_os.h>
#include<rsi_pkt_mgmt.h>
//#include<rsi_driver.h>

/******************************************************
 * *                      Macros
 * ******************************************************/
#define  RSI_BT_DUAL_MODE                 0x8
#define  RSI_BT_BLE_MODE_BITS             (BIT(2) | BIT(3))

#define RSI_BT_STATUS_OFFSET	            0x0C
#define RSI_BT_RSP_TYPE_OFFSET            0x02
#define RSI_BT_RSP_LEN_OFFSET             0x00
#define RSI_BT_RSP_LEN_MASK             0x0FFF

#define RSI_PROTO_BT_COMMON               0x01
#define RSI_PROTO_BT_CLASSIC              0x02
#define RSI_PROTO_BLE                     0x03

#define RSI_DEV_NAME_LEN                    50
#define RSI_DEV_ADDR_LEN					6


#define RSI_BT_CLASSIC_DEVICE             0x00
#define RSI_BT_LE_DEVICE                  0x01
#define RSI_BT_UNKWON_DEVICE              0xFF

#define STACK_BT_MODE					  0x01
#define STACK_BTLE_MODE				  	  0x02

/******************************************************
 * *                    Constants
 * ******************************************************/

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

typedef struct rsi_ble_cb_s rsi_ble_cb_t;
typedef struct rsi_bt_classic_cb_s rsi_bt_classic_cb_t;

/******************************************************
 * *                   Enumerations
 * ******************************************************/

typedef enum rsi_bt_common_cmd_request_e
{
  RSI_BT_SET_LOCAL_NAME                      = 0x0001,
  RSI_BT_GET_LOCAL_NAME                      = 0x0002,
  RSI_BT_GET_RSSI                            = 0x0005,
  RSI_BT_GET_LOCAL_DEV_ADDR                  = 0x0007,
  RSI_BT_REQ_INIT			 			                 = 0x008D,
  RSI_BT_REQ_DEINIT			  			             = 0x008E,
  RSI_BT_SET_ANTENNA_SELECT                  = 0x008F,
  RSI_BT_SET_FEATURES_BITMAP                 = 0x00A6,
  RSI_BT_SET_ANTENNA_TX_POWER_LEVEL          = 0x00A7,
}rsi_bt_common_cmd_request_t;

typedef enum rsi_bt_common_event_e
{
  RSI_BT_EVENT_CARD_READY                    = 0x0505,
}rsi_bt_common_event_t;

typedef enum rsi_bt_common_cmd_resp_e
{
  RSI_BT_RSP_SET_LOCAL_NAME                      = 0x0001,
  RSI_BT_RSP_QUERY_LOCAL_NAME                    = 0x0002,
  RSI_BT_RSP_QUERY_RSSI                          = 0x0005,
  RSI_BT_RSP_QUERY_LOCAL_BD_ADDRESS              = 0x0007,
  RSI_BT_RSP_INIT				 			                   = 0x008D,
  RSI_BT_RSP_DEINIT				  			               = 0x008E,
  RSI_BT_RSP_ANTENNA_SELECT                      = 0x008F,
  RSI_BT_RSP_SET_FEATURES_BITMAP                 = 0x00A6,
  RSI_BT_RSP_ANTENNA_TX_POWER_LEVEL              = 0x00A7,
}rsi_bt_common_cmd_resp_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
//! Driver control block
struct rsi_driver_cb_s;

//! Specific BT, BLE blocks
typedef struct rsi_bt_global_cb_s
{
  //! BLE specific cb
  rsi_ble_cb_t *ble_specific_cb;
  //! BT Classic specific cb
  rsi_bt_classic_cb_t *bt_specific_cb;
}rsi_bt_global_cb_t;

//! Driver BT/BLE control block
typedef struct rsi_bt_cb_s
{
  //! driver BT control block status
  volatile int32_t status;

  //! driver BT control block state
  uint16_t state;

  //! driver BT control block mutex
  rsi_mutex_handle_t bt_mutex;

  //! driver BT control block expected command response
  void *expected_response_buffer;

  //! expected command response type
  uint16_t expected_response_type;

  //! sync command flag to identify that the command is blocking / sync type
  uint8_t sync_rsp;

  //! BT device type at disconnect.
  uint8_t dev_type;

  //! driver BT control block semaphore
  rsi_semaphore_handle_t bt_sem;

  //! driver BT control block tx packet pool
  rsi_pkt_pool_t  bt_tx_pool;

  //! buffer pointer given by application to driver
  uint8_t *app_buffer;

  //! buffer length given by application to driver
  uint32_t app_buffer_length;

  rsi_bt_global_cb_t *bt_global_cb;
}rsi_bt_cb_t;

//! Set local name command structure
typedef struct rsi_bt_req_set_local_name_s
{
  //! uint8, length of the required name to be set
  uint8_t name_len;
  //! int8[50], required name
  int8_t name[RSI_DEV_NAME_LEN];
}rsi_bt_req_set_local_name_t;

//! Get RSSI command structure
typedef struct rsi_bt_get_rssi_s
{
  //! uint8, device address of the device whose RSSI has to be known
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
}rsi_bt_get_rssi_t;

//! Response structures

//! Get local name response structures
typedef struct rsi_bt_resp_get_local_name_s
{
  //! uint8, length of the required name to be set
  uint8_t name_len;
  //! int8[50], required name
  int8_t name[RSI_DEV_NAME_LEN];
}rsi_bt_resp_get_local_name_t;

//! Set antenna structure
typedef struct rsi_ble_set_antenna_s
{
	//!uint8, antenna value - internal/external
	uint8_t value;
}rsi_ble_set_antenna_t;

//! Set feature bitmap structure
typedef struct rsi_bt_set_feature_bitmap_s
{
	//!uint8[4], features bits (bit map)
	uint32_t bit_map;
}rsi_bt_set_feature_bitmap_t;



//! Set antenna tx power level structure
typedef struct rsi_bt_set_antenna_tx_power_level_s
{
	//!uint8, protocol_mode : 1-BT, 2-LE
	uint8_t protocol_mode;
  //!int8_t, transmit power
  int8_t  tx_power;
}rsi_bt_set_antenna_tx_power_level_t;

/******************************************************
 * * BT/BLE common function declarations
 * ******************************************************/
void rsi_bt_set_status(rsi_bt_cb_t *bt_cb, int32_t status);
void rsi_bt_common_init(void);
void rsi_bt_common_tx_done(rsi_pkt_t *pkt);
int8_t rsi_bt_cb_init(rsi_bt_cb_t *bt_cb);
int32_t rsi_bt_driver_send_cmd(uint16_t cmd, void *cmd_struct, void *resp);
uint16_t rsi_bt_global_cb_init(struct rsi_driver_cb_s *driver_cb, uint8_t *buffer);
uint16_t rsi_driver_process_bt_resp_handler(rsi_pkt_t *pkt);
uint16_t rsi_bt_get_proto_type(uint16_t rsp_type, rsi_bt_cb_t **bt_cb);

#ifndef SAPIS_BT_STACK_ON_HOST
int32_t intialize_bt_stack(uint8_t mode);
#endif
#endif

#endif