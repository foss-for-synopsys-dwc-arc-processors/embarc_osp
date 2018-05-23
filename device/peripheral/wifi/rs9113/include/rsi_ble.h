/**
 * @file       rsi_ble.h
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
 *  @brief :   This file contains the BLE functionality related.
 *
 *  @section Description  This file contains all the BLE structures.
 *
 */

#ifndef RSI_BLE_H
#define RSI_BLE_H

#include<rsi_data_types.h>
#include<rsi_bt_common.h>
#include <rsi_ble_apis.h>

/******************************************************
 * *                      Macros
 * ******************************************************/


#define RSI_BLE_STATE_NONE		                      0x00
#define RSI_BLE_STATE_CONNECTION	                  0x01
#define RSI_BLE_STATE_DSICONNECT                      0x02
#define RSI_BLE_MAX_REQ_LIST                          0x05


/******************************************************
 * *                    Constants
 * ******************************************************/


/******************************************************
 * *                   Enumerations
 * ******************************************************/

//! enumeration for BLE command request codes
typedef enum rsi_ble_cmd_request_e
{
	RSI_BLE_REQ_ADV                     = 0x0075,
	RSI_BLE_REQ_SCAN                    = 0x0076,
	RSI_BLE_REQ_CONN                    = 0x0077,
	RSI_BLE_REQ_DISCONNECT              = 0x0078,
	RSI_BLE_GET_DEV_STATE               = 0x0079,
  RSI_BLE_REQ_SMP_PAIR                = 0x007C,
  RSI_BLE_SMP_PAIR_RESPONSE           = 0x007D,
  RSI_BLE_SMP_PASSKEY                 = 0x007E,
	RSI_BLE_REQ_PROFILES                = 0x007F,
	RSI_BLE_REQ_PROFILE                 = 0x0080,
	RSI_BLE_REQ_CHAR_SERVICES           = 0x0081,
	RSI_BLE_REQ_INC_SERVICES            = 0x0082,
	RSI_BLE_REQ_READ_BY_UUID            = 0x0083,
	RSI_BLE_REQ_DESC                    = 0x0084,
	RSI_BLE_REQ_READ_VAL                = 0x0085,
	RSI_BLE_REQ_MULTIPLE_READ           = 0x0086,
	RSI_BLE_REQ_LONG_READ               = 0x0087,
	RSI_BLE_REQ_WRITE                   = 0x0088,
	RSI_BLE_REQ_WRITE_NO_ACK            = 0x0089,
  RSI_BLE_REQ_LONG_WRITE              = 0x008A,
  RSI_BLE_REQ_PREPARE_WRITE           = 0x008B,
  RSI_BLE_REQ_EXECUTE_WRITE           = 0x008C,
  RSI_BLE_ADD_SERVICE                 = 0x0092,
  RSI_BLE_ADD_ATTRIBUTE               = 0x0093,
  RSI_BLE_SET_LOCAL_ATT_VALUE         = 0x0094,
  RSI_BLE_GET_LOCAL_ATT_VALUE         = 0x0095,
  RSI_BLE_CMD_NOTIFY	                = 0x0096,
  RSI_BLE_SET_ADVERTISE_DATA          = 0x009C,
  RSI_BLE_GET_LE_PING                 = 0x00A1,
  RSI_BLE_SET_LE_PING                 = 0x00A2,
  RSI_BLE_SET_RANDOM_ADDRESS          = 0x00A3,
  RSI_BLE_ENCRYPT                     = 0x00A4,
  RSI_BLE_CMD_READ_RESP	              = 0x00A5,
  RSI_BLE_SET_SCAN_RESPONSE_DATA		  = 0x00A8,
  RSI_BLE_LE_WHITE_LIST                 = 0x00AA,
  RSI_BLE_CMD_REMOVE_SERVICE            = 0x00AB,
  RSI_BLE_CMD_REMOVE_ATTRIBUTE          = 0x00AC,
	RSI_BLE_PROCESS_RESOLV_LIST           = 0x00AD,
	RSI_BLE_GET_RESOLVING_LIST_SIZE       = 0x00AE,
	RSI_BLE_SET_ADDRESS_RESOLUTION_ENABLE = 0x00AF,
 	RSI_BLE_REQ_READ_PHY                  = 0x00B0,
	RSI_BLE_REQ_SET_PHY                   = 0x00B1,
  RSI_BLE_SET_DATA_LEN                  = 0x00B2,
  RSI_BLE_READ_MAX_DATA_LEN             = 0X00B3,
	RSI_BLE_SET_PRIVACY_MODE              = 0x00B4,
  RSI_BLE_LE_LTK_REQ_REPLY              = 0X00BA,
}rsi_ble_cmd_request_t;

//! enumeration for BLE command response codes
typedef enum rsi_ble_cmd_resp_e
{
	RSI_BLE_RSP_ADVERTISE               = 0x0075,
	RSI_BLE_RSP_SCAN                    = 0x0076,
	RSI_BLE_RSP_CONNECT                 = 0x0077,
	RSI_BLE_RSP_DISCONNECT              = 0x0078,
	RSI_BLE_RSP_QUERY_DEVICE_STATE      = 0x0079,
  RSI_BLE_RSP_SMP_PAIR                = 0x007C,
  RSI_BLE_RSP_SMP_PAIR_RESPONSE       = 0x007D,
  RSI_BLE_RSP_SMP_PASSKEY             = 0x007E,
	RSI_BLE_RSP_PROFILES                = 0x007F,
	RSI_BLE_RSP_PROFILE                 = 0x0080,
	RSI_BLE_RSP_CHAR_SERVICES           = 0x0081,
	RSI_BLE_RSP_INC_SERVICES            = 0x0082,
	RSI_BLE_RSP_READ_BY_UUID            = 0x0083,
	RSI_BLE_RSP_DESC                    = 0x0084,
	RSI_BLE_RSP_READ_VAL                = 0x0085,
	RSI_BLE_RSP_MULTIPLE_READ           = 0x0086,
	RSI_BLE_RSP_LONG_READ               = 0x0087,
	RSI_BLE_RSP_WRITE                   = 0x0088,
	RSI_BLE_RSP_WRITE_NO_ACK            = 0x0089,
	RSI_BLE_RSP_LONG_WRITE              = 0x008A,
	RSI_BLE_RSP_PREPARE_WRITE           = 0x008B,
	RSI_BLE_RSP_EXECUTE_WRITE           = 0x008C,
	RSI_BLE_RSP_INIT                    = 0x008D,
	RSI_BLE_RSP_DEINIT                  = 0x008E,
	RSI_BLE_RSP_SET_ANTENNA             = 0x008F,
	RSI_BLE_RSP_ADD_SERVICE             = 0x0092,
	RSI_BLE_RSP_ADD_ATTRIBUTE           = 0x0093,
	RSI_BLE_RSP_SET_LOCAL_ATT_VALUE     = 0x0094,
    RSI_BLE_RSP_GET_LOCAL_ATT_VALUE     = 0x0095,
	RSI_BLE_RSP_NOTIFY	                = 0x0096,
    RSI_BLE_RSP_GET_LE_PING             = 0x00A1,
    RSI_BLE_RSP_SET_LE_PING             = 0x00A2,
    RSI_BLE_RSP_SET_RANDOM_ADDRESS      = 0x00A3,
    RSI_BLE_RSP_ENCRYPT                 = 0x00A4,
    RSI_BLE_RSP_READ_RESP	            = 0x00A5,
    RSI_BLE_RSP_LE_WHITE_LIST           = 0x00AA,
    RSI_BLE_RSP_REMOVE_SERVICE          = 0x00AB,
    RSI_BLE_RSP_REMOVE_ATTRIBUTE        = 0x00AC,
    RSI_BLE_RSP_PROCESS_RESOLV_LIST           = 0x00AD,
    RSI_BLE_RSP_GET_RESOLVING_LIST_SIZE       = 0x00AE,
    RSI_BLE_RSP_SET_ADDRESS_RESOLUTION_ENABLE = 0x00AF,
 	RSI_BLE_RSP_READ_PHY                      = 0x00B0,
	RSI_BLE_RSP_SET_PHY                       = 0x00B1,
    RSI_BLE_RSP_SET_DATA_LEN                  = 0x00B2,
    RSI_BLE_RSP_READ_MAX_DATA_LEN             = 0X00B3,
	RSI_BLE_RSP_PRIVACY_MODE                  = 0x00B4,
	RSI_BLE_RSP_LE_LTK_REQ_REPLY            = 0X00BA,
}rsi_ble_cmd_resp_t;


//! enumeration for BLE command response codes
typedef enum rsi_ble_event_e
{
	RSI_BLE_EVENT_DISCONNECT           = 0x1006,
	RSI_BLE_EVENT_ADV_REPORT           = 0x150E,
	RSI_BLE_EVENT_CONN_STATUS          = 0x150F,
    RSI_BLE_EVENT_SMP_REQUEST          = 0x1510,
    RSI_BLE_EVENT_SMP_RESPONSE         = 0x1511,
    RSI_BLE_EVENT_SMP_PASSKEY          = 0x1512,
    RSI_BLE_EVENT_SMP_FAILED           = 0x1513,
	RSI_BLE_EVENT_GATT_NOTIFICATION    = 0x1514,
	RSI_BLE_EVENT_GATT_INDICATION      = 0x1515,
    RSI_BLE_EVENT_ENCRYPT_STARTED      = 0x1516,
	RSI_BLE_EVENT_GATT_WRITE           = 0x1517,
	RSI_BLE_EVENT_LE_PING_TIME_EXPIRED = 0x1518,
	RSI_BLE_EVENT_PREPARE_WRITE	       = 0x1519,
    RSI_BLE_EVENT_EXECUTE_WRITE	       = 0x151A,
	RSI_BLE_EVENT_READ_REQ 		       = 0x151B,
	RSI_BLE_EVENT_MTU 		           = 0x151C,
	RSI_BLE_EVENT_SMP_PASSKEY_DISPLAY_EVENT    = 0x151D,
	RSI_BLE_EVENT_PHY_UPDATE_COMPLETE          = 0x151E,
	RSI_BLE_EVENT_DATA_LENGTH_UPDATE_COMPLETE  = 0x151F,
	RSI_BLE_EVENT_SC_PASSKEY                   = 0x1520,
	RSI_BLE_EVENT_ENHANCE_CONN_STATUS          = 0x1521,
	RSI_BLE_EVENT_DIRECTED_ADV_REPORT          = 0x1522,
	RSI_BLE_EVENT_LE_LTK_REQUEST 		           = 0x152A,

}rsi_ble_event_t;

/********************************************************
 * *                 Structure Definitions
 * ******************************************************/

//! GAP command structures
typedef struct rsi_ble_req_rand_s
{
	//!uint8, random address of the device to be set
	uint8_t  rand_addr[RSI_DEV_ADDR_LEN];
}rsi_ble_req_rand_t;




//! Advertising command structure
typedef struct rsi_ble_req_adv_s
{
	//!uint8, advertising status - disable/enable
	uint8_t  status;
	//!uint8, advertising type used during advertising
	uint8_t  adv_type;
	//!uint8, advertising filter type
	uint8_t  filter_type;
	//!uint8, address type of the device to which directed advertising has to be done
	uint8_t  direct_addr_type;
	//!uint8, address of the device to which directed advertising has to be done
	uint8_t  direct_addr[RSI_DEV_ADDR_LEN];

	//!uint8, advertising interval min
	uint16_t  adv_int_min;

	//!uint8, advertising interval max
	uint16_t  adv_int_max;

	//!uint8, address of the local device
	uint8_t  own_add_type;

  //!uint8, advertising channel map
	uint8_t  adv_channel_map;
}rsi_ble_req_adv_t;

//! Advertising data command structure
typedef struct rsi_ble_req_adv_data_s
{
	//!uint8, advertising data length
	uint8_t  data_len;
	//!uint8, advertising data
	uint8_t  adv_data[31];
}rsi_ble_req_adv_data_t;


//!Scan response data command structure
typedef struct rsi_ble_req_scanrsp_data_s
{
	//!uint8, scanresponse data length
	uint8_t  data_len;
	//!uint8, scanresponse data
	uint8_t  scanrsp_data[31];
}rsi_ble_req_scanrsp_data_t;

//!Scan command structure
typedef struct rsi_ble_req_scan_s
{
	//!uint8, scanning status - disable/enable
	uint8_t status;
	//!uint8, scan type - passive/active
	uint8_t scan_type;
	//!uint8, to filter incoming advertising reports
	uint8_t filter_type;

  //!uint8, address of the local device
	uint8_t  own_add_type;
  //!uint8, scan interval
	uint16_t  scan_int;

  //!uint8, scan window
	uint16_t  scan_win;

}rsi_ble_req_scan_t;

//ENCRYPT COMMAND STRUCTURE

typedef struct rsi_ble_encrypt_s
{

	uint8_t key[16];
	uint8_t data[16];

}rsi_ble_encrypt_t;

//White list structure
typedef struct rsi_ble_white_list_s
{
  //This bit is used to add or delet the address form/to whit list
	uint8_t addordeltowhitlist;
  //Address of the device
	uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //Address type
	uint8_t bdaddressType;

}rsi_ble_white_list_t;

//!Connect command structure
typedef struct rsi_ble_req_conn_s
{
	//!uint8, address type of the device to connect
	uint8_t dev_addr_type;
	//!uint8[6], address of the device to connect
	uint8_t dev_addr[RSI_DEV_ADDR_LEN];
	//!uint16, scan interval
  uint16_t le_scan_interval;
	//!uint16, scan window
  uint16_t le_scan_window;
	//!uint16, minimum connection interval
  uint16_t conn_interval_min;
	//!uint16, maximum connection interval
  uint16_t conn_interval_max;
	//!uint16, connection latency
  uint16_t conn_latency;
	//!uint16, supervision timeout
  uint16_t supervision_tout;
}rsi_ble_req_conn_t;

//!Disconnect command structure
typedef struct rsi_ble_req_disconnect_s
{
	//!uint8[6], address of the device to disconnect
	uint8_t dev_addr[RSI_DEV_ADDR_LEN];
}rsi_ble_req_disconnect_t;

//!SMP protocol structures

//!SMP Pair Request command structure = 0x007C
typedef struct rsi_ble_req_smp_pair_s {
    uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
    uint8_t  io_capability;
} rsi_ble_req_smp_pair_t;

//SMP Response command structure = 0x007D
typedef struct rsi_ble_smp_response_s {
    uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
    uint8_t  io_capability;
} rsi_ble_smp_response_t;


//SMP Passkey command structure, cmd_ix - 0x007E
typedef struct rsi_ble_smp_passkey_s {
    uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
    uint8_t   reserved[2];
    uint32_t  passkey;
} rsi_ble_smp_passkey_t;

//LE ping get auth payload timeout command structure, cmd_ix - 0x00A1
typedef struct rsi_ble_get_le_ping_timeout_s {
    uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
} rsi_ble_get_le_ping_timeout_t;

//LE ping get auth payload timeout command structure, cmd_ix - 0x00A2
typedef struct rsi_ble_rsp_get_le_ping_timeout_s {
    uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
    uint16_t  time_out;
} rsi_ble_rsp_get_le_ping_timeout_t;

//LE ping get auth payload timeout command structure, cmd_ix - 0x00A2
typedef struct rsi_ble_set_le_ping_timeout_s {
    uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
    uint16_t  time_out;
} rsi_ble_set_le_ping_timeout_t;

//LE Add Device to resolvlist command structure, cmd_ix - 0x00AD
typedef struct rsi_ble_resolvlist_s {
    uint8_t     process_type;
    uint8_t     remote_dev_addr_type;
    uint8_t     remote_dev_addr[RSI_DEV_ADDR_LEN];
    uint8_t     peer_irk[16];
    uint8_t     local_irk[16];
} rsi_ble_resolvlist_t;

//LE Get resolvlist size command structure, cmd_ix - 0x00AE
typedef struct rsi_ble_get_resolving_list_size_s {

    uint8_t     size;
} rsi_ble_get_resolving_list_size_t;

//LE Set address resolution enable command structure, cmd_ix - 0x00AF
typedef struct rsi_ble_set_addr_resolution_enable_s {

    uint8_t     enable;
    uint8_t     reserved;
    uint16_t    tout;
} rsi_ble_set_addr_resolution_enable_t;

typedef struct rsi_ble_set_privacy_mode_s {
    uint8_t     remote_dev_addr_type;
    uint8_t     remote_dev_addr[RSI_DEV_ADDR_LEN];
    uint8_t     privacy_mode;
} rsi_ble_set_privacy_mode_t;
//LE read phy request command structure, cmd_ix - 0x00B0
typedef struct rsi_ble_req_read_phy_s {
	uint8_t dev_addr[RSI_DEV_ADDR_LEN];
}rsi_ble_req_read_phy_t;

//LE read phy request command reasponse structure, cmd_ix - 0x00B0
typedef struct rsi_ble_resp_read_phy_s {
   uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
   uint8_t   tx_phy;
   uint8_t   rx_phy;
}rsi_ble_resp_read_phy_t;

//LE set phy command reasponse structure, cmd_ix - 0x00B1
typedef struct rsi_ble_set_phy_s {
    uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
    uint8_t   all_phy;
    uint8_t   tx_phy;
    uint8_t   rx_phy;
    uint8_t   reserved;
    uint16_t  phy_options;
}rsi_ble_set_phy_t;

//LE set data length command reasponse structure, cmd_ix - 0x00B2
typedef struct rsi_ble_setdatalength_s{
    uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
    uint16_t  txoctets;
    uint16_t  txtime;
}rsi_ble_setdatalength_t;

//LE read max data length command reasponse structure, cmd_ix - 0x00B3
typedef struct rsi_ble_resp_read_max_data_length_s{
    uint16_t  maxtxoctets;
    uint16_t  maxtxtime;
    uint16_t  maxrxoctets;
    uint16_t  maxrxtime;
}rsi_ble_read_max_data_length_t;

typedef struct rsi_ble_set_le_ltkreqreply_s{
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
	uint8_t	replytype;
	uint8_t	localltk[16];
}rsi_ble_set_le_ltkreqreply_t;

//! GATT structures

//! GATT Profiles list request structure
typedef struct rsi_ble_req_profiles_list_s
{
	//!uint8[6], remote device address.
	uint8_t    dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], profiles search will be start from start_handle.
	uint16_t   start_handle;
	//!uint8[2], profile search will stop with end_handle.
	uint16_t   end_handle;
} rsi_ble_req_profiles_list_t;

//! GATT Profile request structure
typedef struct rsi_ble_req_profile_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], reserved.
	uint8_t  reserved[2];
	//!uint8[20], profile UUID.
	uuid_t   profile_uuid;
} rsi_ble_req_profile_t;

//! GATT Profile response  structure
//profile_descriptors_t;

//! GATT multiple characteristic services request structure
typedef struct rsi_ble_req_char_services_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], search will be start from start_handle.
	uint8_t  start_handle[2];
	//!uint8[2], search will be end with end_handle.
	uint8_t  end_handle[2];
} rsi_ble_req_char_services_t;

//! GATT include service query request structure
typedef struct rsi_ble_req_inc_services_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], search will be start from start_handle.
	uint8_t  start_handle[2];
	//!uint8[2], search will be end with end_handle.
	uint8_t  end_handle[2];
} rsi_ble_req_inc_services_t;

//! GATT read value by UUID request structure
typedef struct rsi_ble_req_char_val_by_uuid_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], search will be start from start_handle.
	uint8_t  start_handle[2];
	//!uint8[2], search will be end with end_handle.
	uint8_t  end_handle[2];
	//!uint8[2], reserved.
	uint8_t  reserved[2];
	//!uint8[20], search UUID value.
	uuid_t   char_uuid;
} rsi_ble_req_char_val_by_uuid_t;

//! GATT read value by UUID response structure
//rsi_ble_resp_att_value_t

//! GATT multiple attribute descriptors request structure
typedef struct rsi_ble_req_att_descs_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], search will be start from start_handle.
	uint8_t  start_handle[2];
	//!uint8[2], search will be end with end_handle.
	uint8_t  end_handle[2];
} rsi_ble_req_att_descs_t;

//! GATT attribute value request structure
typedef struct rsi_ble_req_att_value_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], attribute handle.
	uint8_t  handle[2];
} rsi_ble_req_att_value_t;

//! GATT multiple attribute values request structure
typedef struct rsi_ble_req_multiple_att_val_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8, number of attributes handles.
	uint8_t   num_of_handles;
	//!uint8, reserved
	uint8_t   reserved;
	//!(uint16 * 5), list of attributes handles.
	uint16_t  handles[RSI_BLE_MAX_RESP_LIST];

} rsi_ble_req_multi_att_values_t;

//! GATT multiple attribute values response structure
//rsi_ble_resp_att_value_t

//! GATT long attribute value request structure
typedef struct rsi_ble_req_long_att_value_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], attribute handle.
	uint16_t  handle;
	//!uint16, attribute value offset .
	uint16_t  offset;
} rsi_ble_req_long_att_value_t;

//! GATT long attribute value response structure
//rsi_ble_resp_att_value_t

//! GATT write attribute value request structure
typedef struct rsi_ble_set_att_val_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], attribute handle.
	uint8_t  handle[2];
	//!uint8, length of attribute value.
	uint8_t  length;
	//!uint8[25], attribute value.
	uint8_t  att_value[25];
} rsi_ble_set_att_value_t;

//! GATT write attribute value without ack request structure
typedef struct rsi_ble_set_att_cmd_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], attribute handle.
	uint8_t  handle[2];
	//!uint8, length of attribute value.
	uint8_t  length;
	//!uint8[25], attribute value.
	uint8_t  att_value[100];
} rsi_ble_set_att_cmd_t;

//! GATT write long attribute value request structure
typedef struct rsi_ble_set_long_att_val_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], attribute handle.
	uint8_t  handle[2];
	//!uint8[2], attribute value offset.
	uint8_t  offset[2];
	//!uint8, length of attribute value.
	uint8_t  length;
	//!uint8[40], attribute value.
	uint8_t  att_value[40];
} rsi_ble_set_long_att_value_t;

//! GATT prepare write value request structure
typedef struct rsi_ble_req_prepare_write_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], attribute handle.
	uint8_t  handle[2];
	//!uint8[2], attribute value offset.
	uint8_t  offset[2];
	//!uint8, length of attribute value.
	uint8_t  length;
	//!uint8[40], attribute value.
	uint8_t  att_value[40];
} rsi_ble_req_prepare_write_t;

//! GATT execute write request structure
typedef struct rsi_ble_req_execute_write_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8, execute flag(write/don't write).
	uint8_t  flag;
} rsi_ble_req_execute_write_t;

//!GATT Events

//!GATT Service
//! add new GATT service request structure
typedef struct  rsi_ble_req_add_serv_s
{
	//!uint8[20], service_uuid.
	uuid_t    service_uuid;
	//!uint8, number of attribute in the service need to use.
	uint8_t   num_of_attributes;
	//!uint8, total attributes value(data) size.
	uint8_t   total_att_datasize;
} rsi_ble_req_add_serv_t;

//! write or change local attribute value request structure
typedef struct  rsi_ble_set_local_att_value_s
{
	//!uint8[2], attribute handle.
	uint16_t    handle;
	//!uint8[2], attribute value length.
	uint16_t    data_len;
	//!uint8[31], attribute value (data).
	uint8_t     data[31];
} rsi_ble_set_local_att_value_t;

//! write or change local attribute value request structure
typedef struct  rsi_ble_notify_att_value_s
{
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], attribute handle.
	uint16_t    handle;
	//!uint8[2], attribute value length.
	uint16_t    data_len;
	//!uint8[31], attribute value (data).
	uint8_t     data[100];
} rsi_ble_notify_att_value_t;

//! read local attribute value request structure
typedef struct rsi_ble_get_local_att_value_s
{
	//!uint8[2], attribute handle.
	uint16_t      handle;
} rsi_ble_get_local_att_value_t;

typedef struct rsi_ble_gatt_read_response_s
{
	//!uint8[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8[2], attribute handle.
//	uint16_t      handle;
//	uint16_t      offset;
	uint8_t       type;
	uint8_t       reserved;
	//!uint8[2], attribute value length.
	uint16_t    data_len;
	//!uint8[31], attribute value (data).
	uint8_t     data[100];
} rsi_ble_gatt_read_response_t;

//white list(cmd), cmd_ix = 0x00AB
typedef struct  rsi_ble_gatt_remove_serv_s {
    uint32_t serv_hndler;
} rsi_ble_gatt_remove_serv_t;

//white list(cmd), cmd_ix = 0x00AC
typedef struct  rsi_ble_gatt_remove_att_s {
    uint32_t    serv_hndler;
    uint16_t    att_hndl;
} rsi_ble_gatt_remove_att_t;

//! Driver BLE control block
struct rsi_ble_cb_s
{
	//! GAP Callbacks
	rsi_ble_on_adv_report_event_t    ble_on_adv_report_event;
	rsi_ble_on_connect_t             ble_on_conn_status_event;
	rsi_ble_on_disconnect_t          ble_on_disconnect_event;
    rsi_ble_on_le_ping_payload_timeout_t  ble_on_le_ping_time_expired_event;
    rsi_ble_on_le_ltk_req_event_t  ble_on_le_ltk_req_event;

    //!SMP Callbackes
    rsi_ble_on_smp_request_t         ble_on_smp_request_event;
    rsi_ble_on_smp_response_t        ble_on_smp_response_event;
    rsi_ble_on_smp_passkey_t         ble_on_smp_passkey_event;
    rsi_ble_on_smp_failed_t          ble_on_smp_fail_event;
    rsi_ble_on_encrypt_started_t     ble_on_smp_encrypt_started;
    rsi_ble_on_smp_passkey_display_t ble_on_smp_passkey_display;

	//! GATT Callbacks
	rsi_ble_on_profiles_list_resp_t  ble_on_profiles_list_resp;
	rsi_ble_on_profile_resp_t        ble_on_profile_resp;
	rsi_ble_on_char_services_resp_t  ble_on_char_services_resp;
	rsi_ble_on_inc_services_resp_t   ble_on_inc_services_resp;
	rsi_ble_on_att_desc_resp_t       ble_on_att_desc_resp;
	rsi_ble_on_read_resp_t           ble_on_read_resp;
	rsi_ble_on_write_resp_t          ble_on_write_resp;
	rsi_ble_on_gatt_write_event_t    ble_on_gatt_events;
    rsi_ble_on_gatt_prepare_write_event_t  ble_on_prepare_write_event;
	rsi_ble_on_execute_write_event_t       ble_on_execute_write_event;
	rsi_ble_on_read_req_event_t            ble_on_read_req_event;
	rsi_ble_on_mtu_event_t                 ble_on_mtu_event;

    rsi_ble_on_phy_update_complete_t       ble_on_phy_update_complete_event;
    rsi_ble_on_data_length_update_t        rsi_ble_on_data_length_update_event;
    rsi_ble_on_sc_passkey_t                ble_on_sc_passkey;
	rsi_ble_on_enhance_connect_t           ble_on_enhance_conn_status_event;
    rsi_ble_on_directed_adv_report_event_t ble_on_directed_adv_report_event;
};

/******************************************************
 * * BLE internal function declarations
 * ******************************************************/

void rsi_ble_callbacks_handler(rsi_bt_cb_t *ble_cb, uint16_t rsp_type, uint8_t *payload, uint16_t payload_length);
int32_t rsi_ble_driver_send_cmd(uint16_t cmd, void *cmd_struct, void *resp);

#endif


