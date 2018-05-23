/**
 * @file     rsi_ble_apis.h
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

#ifndef RSI_BLE_APIS_H
#define RSI_BLE_APIS_H

#include<rsi_data_types.h>
#include<rsi_utils.h>
#include<rsi_bt_common.h>

/******************************************************
 * *                      Macros
 * ******************************************************/

#define RSI_BLE_MAX_RESP_LIST                         0x05
#define RSI_MAX_ADV_REPORT_SIZE                       31

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
******************************************************/

//! GAP Event structures

//!Advertise report event structure
typedef struct rsi_ble_event_adv_report_s
{
	//!uint8_t, address type of the advertising device
	uint8_t dev_addr_type;
	//!uint8_t[6], address of the advertising device
	uint8_t dev_addr[RSI_DEV_ADDR_LEN];
	uint8_t adv_data_len;
	uint8_t adv_data[RSI_MAX_ADV_REPORT_SIZE];
	int8_t rssi;
}rsi_ble_event_adv_report_t;

//!Connection status event structure
typedef struct rsi_ble_event_conn_status_s
{
	//!uint8_t, address type of the connected device
	uint8_t dev_addr_type;
	//!uint8_t[6], address of the connected device
	uint8_t dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8_t, status of the connection - success/failure
	uint8_t status;

}rsi_ble_event_conn_status_t;

//!Enhance Connection status event structure
typedef struct rsi_ble_event_enhnace_conn_status_s
{
	//!uint8_t, address type of the connected device
	uint8_t dev_addr_type;
	//!uint8_t[6], address of the connected device
	uint8_t dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8_t[6], resolvlable address of the connected device
	uint8_t peer_resolvlable_addr[RSI_DEV_ADDR_LEN];
	//!uint8_t[6], resolvlable address of the local device
	uint8_t local_resolvlable_addr[RSI_DEV_ADDR_LEN];
	//!uint8_t, status of the connection - success/failure
	uint8_t status;
}rsi_ble_event_enhance_conn_status_t;

//!Disconnect event structure
typedef struct rsi_ble_event_disconnect_s
{
	//!uint8_t, address of the disconnected device
	uint8_t dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8_t, device type (Classic/LE)
	uint8_t dev_type;

}rsi_ble_event_disconnect_t;

//!le ping timeout expired event structure
typedef struct rsi_ble_event_le_ping_time_expired_s
{
	//!uint8_t, address of the disconnected device
	uint8_t dev_addr[RSI_DEV_ADDR_LEN];

}rsi_ble_event_le_ping_time_expired_t;

//le ltk request event Structure
typedef struct rsi_bt_event_le_ltk_request_s {
  //!uint8_t, address of the remote device encrypted
  uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t  localediv;
  uint8_t   localrand[8];
} rsi_bt_event_le_ltk_request_t;

//encryption enabled structure
typedef struct rsi_bt_event_encryption_enabled_s {
  //!uint8_t, address of the remote device encrypted
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t  enabled;
  uint8_t  reserved;
  uint16_t localediv;
  uint8_t  localrand[8];
  uint8_t  localltk[16];
} rsi_bt_event_encryption_enabled_t;

//!SMP protocol structure
//SMP Request event structure
typedef struct rsi_bt_event_smp_req_s {
	//!uint8_t, address of remote device
    uint8_t  dev_addr[6];
} rsi_bt_event_smp_req_t;

//SMP Response event structure
typedef struct rsi_bt_event_smp_resp_s {
	//!uint8_t, address of remote device
    uint8_t  dev_addr[6];
} rsi_bt_event_smp_resp_t;

//SMP passkey event structure
typedef struct rsi_bt_event_smp_passkey_s {
	//!uint8_t, address of remote device
    uint8_t  dev_addr[6];
} rsi_bt_event_smp_passkey_t;

//SMP passkey display event structure
typedef struct rsi_bt_event_smp_passkey_display_s {
	//!uint8_t, address of remote device
    uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
    uint8_t  passkey[6];
} rsi_bt_event_smp_passkey_display_t;

//SMP passkey display event structure
typedef struct rsi_bt_event_sc_passkey_s {
	//!uint8_t, address of remote device
    uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
    uint8_t   reserved[2];
    uint32_t  passkey;
} rsi_bt_event_sc_passkey_t;

//SMP failed event structure
typedef struct rsi_bt_event_smp_failed_s {
	//!uint8_t, address of remote device
    uint8_t  dev_addr[6];
} rsi_bt_event_smp_failed_t;

//! phy update complete event
typedef struct rsi_ble_event_phy_update_s {
    uint8_t  dev_addr[6];
    uint8_t  Status;
    uint8_t  TxPhy;
    uint8_t  RxPhy;
} rsi_ble_event_phy_update_t;

//! phy update complete event
typedef struct rsi_ble_event_data_length_update_s
{
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	uint16_t  MaxTxOctets;
  uint16_t  MaxTxTime;
  uint16_t  MaxRxOctets;
  uint16_t  MaxRxTime;
} rsi_ble_event_data_length_update_t;

//! Basic GATT structures

//! 128 bit UUID format structure
typedef struct uuid128_s {
	uint8_t      data1[4];
	uint8_t      data2[2];
	uint8_t      data3[2];
	uint8_t      data4[8];
} uuid128_t;

//! 16 bit UUID format structure
typedef  uint16_t      uuid16_t;

//! 32 bit UUID format structure
typedef  uint32_t      uuid32_t;

//! UUID format structure
typedef struct uuid_s {
	//!uint8_t, size of UUID
	uint8_t      size;
	//!uint8_t[3], reserved
	uint8_t      reserved[3];
	union uuid_t {
		//!uint8_t[16], 128 bit(16 byte) UUID value
		uuid128_t  val128;
		//!uint8_t[4], 32 bit(4 bytes) UUID value
		uuid32_t   val32;
		//!uint8_t[2], 16 bit(2 bytes) UUID value
		uuid16_t   val16;
	} val;
} uuid_t;

//! profile descriptor/primary service structure
typedef struct profile_descriptor_s
{
	//!uint8_t[2], profile start handle
	uint8_t  start_handle[2];
	//!uint8_t[2], profile end handle
	uint8_t  end_handle[2];
	//!uint8_t[20], profile UUID size, value
	uuid_t   profile_uuid;
} profile_descriptors_t ;

//! GATT attribute descriptor structure
typedef struct att_desc_s
{
	//!uint8_t[2], attribute handle
	uint8_t  handle[2];
	//!uint8_t[2], reserved.
	uint8_t  reserved[2];
	//!uint8_t[20], attribute type UUID
	uuid_t   att_type_uuid;
} att_desc_t;

//!characteristic service attribute value structure
typedef struct char_serv_data_s
{
	//!uint8_t, characteristic value property.
	uint8_t   char_property;
	//!uint8_t, reserved.
	uint8_t   reserved;
	//!uint8_t[2], characteristic value handle
	uint16_t  char_handle;
	//!uint8_t[20], characteristic value UUID
	uuid_t    char_uuid;
} char_serv_data_t;

//!characteristic service attribute structure
typedef struct char_serv_s
{
	//!uint8_t[2], characteristic service attribute handle
	uint16_t  handle;
	//!uint8_t[2], reserved.
	uint8_t reserved[2];
	//!uint8_t[24], characteristic service attribute value
	char_serv_data_t char_data;
} char_serv_t;

//!include service attribute value structure
typedef struct inc_serv_data_s
{
	//!uint8_t[2], include service start handle
	uint16_t  start_handle;
	//!uint8_t[2], include service end handle
	uint16_t  end_handle;
	//!uint8_t[20], include service UUID
	uuid_t    uuid;
} inc_serv_data_t;

//! include service attribute structure
typedef struct inc_serv_s
{
	//!uint8_t[2], include service attribute handle
	uint16_t  handle;
	//!uint8_t[2], reserved.
	uint8_t reserved[2];
	//!uint8_t[24], include service attribute value
	inc_serv_data_t   inc_serv;
} inc_serv_t;

//! GATT Request structures

//! add new attributes to service request structure
typedef struct  rsi_ble_req_add_att_s
{
	//!uint8_t[4], service handler obtained from its rsi_ble_resp_add_serv_t
	void       *serv_handler;
	//!uint8_t[2], new attribute handle.
	uint16_t    handle;
	//!uint8_t[2], reserved.
	uint16_t    reserved;
	//!uint8_t[20], attribute type UUID.
	uuid_t      att_uuid;
	//!uint8_t, attribute property.
	uint8_t     property;
	//!uint8_t[31], attribute data.
	uint8_t     data[31];
	//!uint8_t[2], attribute data len.
	uint16_t    data_len;
} rsi_ble_req_add_att_t;

//! GATT Response structures

//! GATT Profiles list response structure
typedef struct rsi_ble_resp_profiles_list_s
{
	//!uint8_t, number of profiles found.
	uint8_t                number_of_profiles;
	//!uint8_t[3], reserved.
	uint8_t                reserved[3];
	//!(uint8_t[24] * 5), list of found profiles.
	profile_descriptors_t  profile_desc[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_resp_profiles_list_t;

//! GATT characteristic query service response structure
typedef struct rsi_ble_resp_char_serv_s
{
	//!uint8_t, number of characteristic services found.
	uint8_t      num_of_services;
	//!uint8_t[3], reserved.
	uint8_t      reserved[3];
	//!(uint8_t[28] * 5), characteristic service list.
	char_serv_t  char_services[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_resp_char_services_t;

//! GATT include service response structure
typedef struct rsi_ble_resp_inc_serv
{
	//!uint8_t, number of include service found.
	uint8_t     num_of_services;
	//!uint8_t[3], reserved.
	uint8_t     reserved[3];
	//!(uint8_t[28] * 5), include service list.
	inc_serv_t  services[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_resp_inc_services_t;

//! GATT attribute value response structure
typedef struct rsi_ble_resp_att_value_s
{
	//!uint8_t, length of attribute value.
	uint8_t  len;
	//!uint8_t[30], attribute values.
	uint8_t  att_value[100];
} rsi_ble_resp_att_value_t;

//! GATT attribute descriptors response structure
typedef struct rsi_ble_resp_att_descs_s
{
	//!uint8_t, number of descriptors found.
	uint8_t     num_of_att;
	//!uint8_t[3], reserved.
	uint8_t     reserved[3];
	//!(uint8_t[24] * 5), attribute descriptors list.
	att_desc_t  att_desc[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_resp_att_descs_t;

//! add new service response structure
typedef struct rsi_ble_resp_add_serv_s
{
	//!uint8_t[4], new service handler.
	void      *serv_handler;
	//!uint8_t[2], new service start handle.
	uint16_t   start_handle;
} rsi_ble_resp_add_serv_t;

//! read local attribute value response structure
typedef struct  rsi_ble_resp_local_att_value_s
{
	//!uint8_t[2], attribute handle.
	uint16_t    handle;
	//!uint8_t[2], attribute value length.
	uint16_t    data_len;
	//!uint8_t[31], attribute value (data).
	uint8_t     data[20];
} rsi_ble_resp_local_att_value_t;

//! GATT Event structures

//! GATT Write event structure
typedef struct rsi_ble_event_write_s
{
	//!uint8_t[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8_t[2], attribute handle.
	uint8_t  handle[2];
	//!uint8_t, length of attribute value.
	uint8_t  length;
	//!uint8_t[50], attribute value.
	uint8_t  att_value[100];
} rsi_ble_event_write_t;

//! GATT prepare Write event structure
typedef struct rsi_ble_event_prepare_write_s
{
	//!uint8_t[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8_t[2], attribute handle.
	uint8_t  handle[2];
	//!uint8_t[2], attribute value offset.
  uint8_t  offset[2];
	//!uint8_t, length of attribute value.
	uint16_t  length;
	//!uint8_t[50], attribute value.
	uint8_t  att_value[100];
} rsi_ble_event_prepare_write_t;

//! GATT execuite Write event structure
typedef struct rsi_ble_execute_write_s {
	//!uint8_t[6], remote device address.
    uint8_t dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8_t, execute write flag.
    uint8_t exeflag;
} rsi_ble_execute_write_t;

//! GATT execuite Write event structure
typedef struct rsi_ble_read_req_s {
	//!uint8_t[6], remote device address.
    uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8_t[2], attribute handle.
    uint16_t  handle;
	//!uint8_t, type of operation(read/longread)
    uint8_t   type;
    //!uint8_t, reserved byte
    uint8_t   reserved;
    //!uint8_t, read offset
    uint16_t  offset;
} rsi_ble_read_req_t;

//! GATT execuite Write event structure
typedef struct rsi_ble_event_mtu_s {
	//!uint8_t[6], remote device address.
    uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8_t[2], attribute handle.
    uint16_t  mtu_size;
} rsi_ble_event_mtu_t;


//! GATT Notification event structure
typedef struct rsi_ble_event_notify_s
{
	//!uint8_t[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8_t[2], attribute handle.
	uint8_t  handle[2];
	//!uint8_t, length of attribute value.
	uint8_t  length;
	//!uint8_t[50], attribute value.
	uint8_t  att_value[50];
} rsi_ble_event_notify_t;

//! GATT Indication event structure
typedef struct rsi_ble_event_indication_s
{
	//!uint8_t[6], remote device address.
	uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	//!uint8_t[2], attribute handle.
	uint8_t  handle[2];
	//!uint8_t, length of attribute value.
	uint8_t  length;
	//!uint8_t[50], attribute value.
	uint8_t  att_value[50];
} rsi_ble_event_indication_t;

typedef struct rsi_ble_event_directedadv_report_s
{
	//!uint8_t, address type of the advertising device
	uint16_t event_type;
	uint8_t dev_addr_type;
	uint8_t dev_addr[RSI_DEV_ADDR_LEN];
	uint8_t directed_addr_type;
	uint8_t directed_dev_addr[RSI_DEV_ADDR_LEN];
	int8_t rssi;
}rsi_ble_event_directedadv_report_t;
/******************************************************
 * *                 Global Variables
 * ******************************************************/

/******************************************************
 * *              GAP API's Declarations
 * ******************************************************/

//*==============================================*/
/**
 * @fn         rsi_ble_start_advertising
 * @brief      request the local device to start advertising
 * @param[in]  none
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to start advertising
 * */
int32_t rsi_ble_start_advertising(void);

/*==============================================*/
/**
 * @fn         rsi_ble_encrypt
 * @brief      request the local device to encrypt
 * @param[in]  none
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to encrypt
 * */
int32_t rsi_ble_encrypt(uint8_t*,uint8_t*,uint8_t*);

/*==============================================*/
/**
 * @fn         rsi_ble_stop_advertising
 * @brief      request the local device to stop advertising
 * @param[in]  none
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to stop advertising
 * */
int32_t rsi_ble_stop_advertising(void);

/*==============================================*/
/**
 * @fn         rsi_ble_set_advertise_data
 * @brief      request the local device to set the advertising data
 * @param[in]  none
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to set the advertising data
 * */
int32_t rsi_ble_set_advertise_data(uint8_t *data, uint16_t data_len);

/*==============================================*/
/**
 * @fn         rsi_ble_start_scanning
 * @brief      request the local device to start scanning
 * @param[in]  none
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to start scanning
 * */
int32_t rsi_ble_start_scanning(void);

/*==============================================*/
/**
 * @fn         rsi_ble_stop_scanning
 * @brief      request the local device to stop scanning
 * @param[in]  none
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to stop scanning
 * */
int32_t rsi_ble_stop_scanning(void);

/*==============================================*/
/**
 * @fn         rsi_ble_connect
 * @brief      request to initiate the connection with the remote device
 * @param[in]  addr_type, address type of the remote device
 * @param[in]  remote_address, address of the remote device
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to initiate the connection request
 * */
int32_t rsi_ble_connect(uint8_t remote_dev_addr_type, int8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_ble_connect_cancel
 * @brief      request the connection cancel with the remote device
 * @param[in]  remote_dev_address, address of the remote device
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to cancel the connection request
 * */
int32_t rsi_ble_connect_cancel(int8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_ble_disconnect
 * @brief      request the disconnect with the remote device
 * @param[in]  remote_dev_address, address of the remote device
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to disconnect the physical connection
 * */
int32_t rsi_ble_disconnect(int8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_ble_get_device_state
 * @brief      request the local device state
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to know the local device state
 */
int32_t rsi_ble_get_device_state(uint8_t *resp);

/******************************************************
 * *        GATT Client API's Declarations
 * ******************************************************/
/**
 * @fn         rsi_ble_get_profiles_async
 * @brief      Get the supported profiles/services of the connected / remote device
 * @param[in]  dev_addr, remote device address
 * @param[in]  start_handle, Start handle (index) of the remote device's service records
 * @param[in]  end_handle, End handle (index) of the remote device's service records
 * @param[out] p_profile_list, profiles/services information will be filled in this
 *                             after retrieving from the remote device
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used get the remote device supported profiles list.
 *
 * rsi_ble_on_profiles_list_resp_t callback function will be called after the profiles list response is received
 */
int32_t rsi_ble_get_profiles_async(uint8_t *dev_addr,
		uint16_t start_handle,
		uint16_t end_handle,
		rsi_ble_resp_profiles_list_t *p_prof_list);

/**
 * @fn         rsi_ble_get_profile_async
 * @brief      Get the supported profile/service of the connected / remote device
 * @param[in]  dev_addr, remote device address
 * @param[in]  profile_uuid, Services/profiles are searched using profile_uuid
 * @param[out] p_profile, profile/service information will be filled in this structure
 *                        after retrieving from the remote device
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used get the remote device supported profile
 *
 * rsi_ble_on_profile_resp_t callback function will be called after the profile response is received
 */
int32_t rsi_ble_get_profile_async(uint8_t *dev_addr,
		uuid_t profile_uuid,
		profile_descriptors_t *p_profile);

/**
 * @fn         rsi_ble_get_char_services_async
 * @brief      Get service characteristics of the connected / remote device
 * @param[in]  dev_addr, remote device address
 * @param[in]  start_handle, Start handle (index) of the remote device's service records
 * @param[in]  end_handle, End handle (index) of the remote device's service records
 * @param[out] p_char_services_list, service characteristics details are filled in this structure.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to get the service characteristics of the remote GATT server
 *
 * rsi_ble_on_char_services_resp_t callback function will be called after the  service characteristics response is received
 */
int32_t rsi_ble_get_char_services_async(uint8_t *dev_addr,
		uint16_t start_handle,
		uint16_t end_handle,
		rsi_ble_resp_char_services_t *p_char_serv_list);

/**
 * @fn         rsi_ble_get_inc_services_async
 * @brief      Get the supported include services of the connected / remote device
 * @param[in]  dev_addr, remote device address
 * @param[in]  start_handle, Start handle (index) of the remote device's service records
 * @param[in]  end_handle, End handle (index) of the remote device's service records
 * @param[out] p_inc_serv_list, include service details are filled in this structure.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to get the include service of the remote GATT server
 *
 * rsi_ble_on_inc_services_resp_t callback function will be called after the  include service response is received
 */
int32_t rsi_ble_get_inc_services_async(uint8_t *dev_addr,
		uint16_t start_handle,
		uint16_t end_handle,
		rsi_ble_resp_inc_services_t *p_inc_serv_list);

/**
 * @fn         rsi_ble_get_att_descriptors_async
 * @brief      Get the characteristic descriptors list from remote device
 * @param[in]  dev_addr, remote device address
 * @param[in]  start_handle, Start handle (index) of the remote device's service records
 * @param[in]  end_handle, End handle (index) of the remote device's service records
 * @param[out] p_att_desc, characteristic descriptors are filled in this structure.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to get the characteristic descriptors details of the remote device
 *
 * rsi_ble_on_att_desc_resp_t callback function will be called after the attribute descriptors response is received
 */
int32_t rsi_ble_get_att_descriptors_async(uint8_t *dev_addr,
		uint16_t start_handle,
		uint16_t end_handle,
		rsi_ble_resp_att_descs_t *p_att_desc);


/**
 * @fn         rsi_ble_get_char_value_by_uuid_async
 * @brief      Get characteristic value by UUID (char_uuid)
 * @param[in]  dev_addr, remote device address
 * @param[in]  start_handle, Start handle (index) of the remote device's service records
 * @param[in]  end_handle, End handle (index) of the remote device's service records
 * @param[in]  char_uuid, UUID of the characteristic
 * @param[out] p_char_value, characteristic value is filled in this structure.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to get the characteristic value using UUID
 *
 * rsi_ble_on_read_resp_t callback function will be called upon receiving the attribute value
 */
int32_t rsi_ble_get_char_value_by_uuid_async(uint8_t *dev_addr,
		uint16_t start_handle,
		uint16_t end_handle,
		uuid_t   char_uuid,
		rsi_ble_resp_att_value_t *p_char_val);


/**
 * @fn         rsi_ble_get_att_value_async
 * @brief      Get the attribute by handle
 * @param[in]  dev_addr, remote device address
 * @param[in]  handle, handle of attribute
 * @param[out] p_att_val, attribute value is filled in this structure.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to get the attribute value using handle
 *
 * rsi_ble_on_read_resp_t callback function will be called upon receiving the attribute value
 */
int32_t rsi_ble_get_att_value_async(uint8_t *dev_addr,
		uint16_t handle,
		rsi_ble_resp_att_value_t *p_att_val);

/**
 * @fn         rsi_ble_get_multiple_att_values_async
 * @brief      Get multiple attribute values by using multiple handles
 * @param[in]  dev_addr, remote device address
 * @param[in]  num_of_handles, number of handles in the list
 * @param[in]  handles, list of attribute handles
 * @param[out] p_att_vals, Attribute values are filled in this structure.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to get the multiple attribute values by using multiple handles
 *
 * rsi_ble_on_read_resp_t callback function will be called upon receiving the attribute value
 */
int32_t rsi_ble_get_multiple_att_values_async(uint8_t *dev_addr,
		uint8_t  num_handles,
		uint16_t *handles,
		rsi_ble_resp_att_value_t *p_att_vals);


/**
 * @fn         rsi_ble_get_long_att_value_async
 * @brief      Get the long attribute value by using handle and offset
 * @param[in]  dev_addr, remote device address
 * @param[in]  handle, attribute handle
 * @param[in]  offset, offset with in the attribute value
 * @param[out] p_att_vals, attribute value is filled in this structure
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to get the attribute value by using handle and offset
 *
 * rsi_ble_on_read_resp_t callback function will be called upon receiving the attribute value
 */
int32_t rsi_ble_get_long_att_value_async(uint8_t *dev_addr,
		uint16_t handle,
		uint16_t offset,
		rsi_ble_resp_att_value_t *p_att_vals);

/**
 * @fn         rsi_ble_set_att_value_async
 * @brief      Set attribute value
 * @param[in]  dev_addr, remote device address
 * @param[in]  handle, attribute handle
 * @param[in]  data_len, attribute value length
 * @param[in]  p_data, attribute value .
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set the attribute value (data_len and p_data) represented by handle
 *
 * rsi_ble_on_write_resp_t callback function will be called if the attribute set action is completed
 */
int32_t rsi_ble_set_att_value_async(uint8_t *dev_addr,
		uint16_t handle,
		uint8_t  data_len,
		uint8_t  *p_data);

/**
 * @fn         rsi_ble_set_att_cmd
 * @brief      Set attribute value with out waiting for any ack from remote device
 * @param[in]  dev_addr, remote device address
 * @param[in]  handle, attribute handle
 * @param[in]  data_len, attribute value length
 * @param[in]  p_data, attribute value .
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set the attribute value (data_len and p_data) represented by handle.
 * It does not wait for ack from remote device
 *
 * rsi_ble_on_write_resp_t callback function will be called if the attribute set action is completed
 */
int32_t rsi_ble_set_att_cmd(uint8_t *dev_addr,
		uint16_t handle,
		uint8_t  data_len,
		uint8_t  *p_data);

/**
 * @fn         rsi_ble_req_set_long_att_value_async
 * @brief      Set long attribute value
 * @param[in]  dev_addr, remote device address
 * @param[in]  handle, attribute handle
 * @param[in]  offset, attribute value offset
 * @param[in]  data_len, attribute value length
 * @param[in]  p_data, attribute value .
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set the long attribute value
 *
 * rsi_ble_on_write_resp_t callback function will be called if the attribute set action is completed
 */
int32_t rsi_ble_set_long_att_value_async(uint8_t *dev_addr,
		uint16_t handle,
		uint16_t offset,
		uint8_t  data_len,
		uint8_t  *p_data);

/**
 * @fn         rsi_ble_prepare_write_async
 * @brief      Prepare attribute value
 * @param[in]  dev_addr, remote device address
 * @param[in]  handle, attribute handle
 * @param[in]  offset, attribute value offset
 * @param[in]  data_len, attribute value length
 * @param[in]  p_data, attribute value .
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to prepare the attribute value at remote device
 *
 * rsi_ble_on_write_resp_t callback function will be called if the prepare attribute write action is completed
 */
int32_t rsi_ble_prepare_write_async(uint8_t *dev_addr,
		uint16_t handle,
		uint16_t offset,
		uint8_t  data_len,
		uint8_t  *p_data);

/**
 * @fn         rsi_ble_execute_write_asnyc
 * @brief      Execute the prepared attribute values
 * @param[in]  dev_addr, remote device address
 * @param[in]  exe_flag, execute flag to write the values or not
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to execute the prepared the attribute values
 *
 * rsi_ble_on_write_resp_t callback function will be called if the execute attribute write action is completed
 */
int32_t rsi_ble_execute_write_async(uint8_t *dev_addr,
		uint8_t exe_flag);

/******************************************************
 * *        GATT Server API's Declarations
 * ******************************************************/
/**
 * @fn         rsi_ble_add_service
 * @brief      Add new service to local GATT Server
 * @param[in]  service_uuid, new service uuid value
 * @param[in]  num_of_attributes, number of attributes required in the service.
 * @param[in]  total_data_size, total data size required in the service
 * @param[out] p_resp_serv, new service handler is filled in this structure.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to add new service to local GATT Server
 */
int32_t rsi_ble_add_service(uuid_t  serv_uuid,
		uint8_t num_attributes,
		uint8_t total_data_size,
		rsi_ble_resp_add_serv_t *p_resp_serv);

/**
 * @fn         rsi_ble_add_attribute
 * @brief      Add new attribute to a specific service
 * @param[in]  p_attribute, new attribute to be added to service
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to add new attribute to a service
 */
int32_t rsi_ble_add_attribute(rsi_ble_req_add_att_t *p_attribute);

/**
 * @fn         rsi_ble_set_local_att_value
 * @brief      change the local attribute value.
 * @param[in]  handle, local attribute handle
 * @param[in]  data_len, attribute value length
 * @param[in]  p_data, attribute value
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to change the local attribute value.
 */
int32_t rsi_ble_set_local_att_value(uint16_t  handle,
		uint16_t  data_len,
		uint8_t   *p_data);

/**
 * @fn         rsi_ble_get_local_att_value
 * @brief      Get the local attribute value
 * @param[in]  handle, local attribute handle
 * @param[out] p_resp_local_att_val, local attribute value is filled in this structure
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to get the local attribute value
 */
int32_t rsi_ble_get_local_att_value(uint16_t handle,
		rsi_ble_resp_local_att_value_t *p_resp_local_att_val);

/**
 * @fn         rsi_ble_gatt_read_response
 * @brief      sending Read resonse data to remote device
 * @param[in]  remote_dev_address,  remote bd address in string format
 * @param[in]  read_type, reasponse type (read/long read)
 * @param[in]  handle, handle of the attribute record
 * @param[in]  offset, offset of the read resonse data
 * @param[in]  length, length of the read resonse data
 * @param[in]  p_data, data pointer of the read resonse data
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to send the attribute record data.
 */
int32_t  rsi_ble_gatt_read_response(uint8_t *dev_addr,uint8_t read_type,uint16_t handle,uint16_t offset,uint16_t length,uint8_t  *p_data);

/**
 * @fn         rsi_ble_smp_pair_request
 * @brief      request the smp pairing process with remote device
 * @param[in]  remote_dev_address,  remote bd address in string format
 * @param[in]  io_capability, device input and output capability
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start the SMP pairing process
 */
int32_t rsi_ble_smp_pair_request (uint8_t *remote_dev_address, uint8_t io_capability);

/**
 * @fn         rsi_ble_smp_pair_response
 * @brief      request to send smp pairing response in pairing process with remote device
 * @param[in]  remote_dev_address,  remote bd address in string format
 * @param[in]  io_capability, device input and output capability
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to respond the SMP pairing request
 */
int32_t rsi_ble_smp_pair_response (uint8_t *remote_dev_address, uint8_t io_capability);

 /**
 * @fn         rsi_ble_smp_passkey
 * @brief      request to send smp passkey in SMP pairing process with remote device
 * @param[in]  remote_dev_address,  remote bd address in string format
 * @param[in]  io_capability, device input and output capability
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to respond the SMP passkey request
 */
int32_t rsi_ble_smp_passkey (uint8_t *remote_dev_address, uint32_t passkey);



/******************************************************
 * *       BLE GAP Callbacks Declarations
 * ******************************************************/

/**
 * @callback   rsi_ble_on_adv_report_event_t
 * @brief      Callback function to be called if advertise report event is received
 * @param[in]  rsi_ble_event_adv, contains the advertise report information
 * @return      void
 * @section description
 * This callback function will be called if the advertise event report is received from the module
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_adv_report_event_t) (rsi_ble_event_adv_report_t *rsi_ble_event_adv);


/**
 * @callback   rsi_ble_on_connect_t
 * @brief      Callback function to be called if BLE connection status is received
 * @param[in]  rsi_ble_event_conn, contains the BLE connection status
 * @return      void
 * @section description
 * This callback function will be called if the BLE connection status is received from the module
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_connect_t) (rsi_ble_event_conn_status_t *rsi_ble_event_conn);


/**
 * @callback   rsi_ble_on_connect_t
 * @brief      Callback function to be called if BLE connection status is received
 * @param[in]  rsi_ble_event_conn, contains the BLE connection status
 * @return      void
 * @section description
 * This callback function will be called if the BLE connection status is received from the module
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_enhance_connect_t) (rsi_ble_event_enhance_conn_status_t *rsi_ble_event_enhance_conn);

/**
 * @callback   rsi_ble_on_disconnect_t
 * @brief      Callback function to be called if disconnect event is received
 * @param[in]  rsi_ble_event_disconnect, contains the disconnect status
 * @return      void
 * @section description
 * This callback function will be called if the disconnect status event is received from the module
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_disconnect_t) (rsi_ble_event_disconnect_t *rsi_ble_event_disconnect, uint16_t reason);

/**
 * @callback   rsi_ble_on_le_ping_payload_timeout_t
 * @brief      Callback function to be called if le ping payload timeout expired event is received
 * @param[in]  rsi_ble_event_disconnect, contains the disconnect status
 * @return      void
 * @section description
 * This callback function will be called if the le ping time expired event is received from the module
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_le_ping_payload_timeout_t) (rsi_ble_event_le_ping_time_expired_t *rsi_ble_event_timeout_expired);

/**
 * @callback   rsi_ble_on_le_ltk_req_event_t
 * @brief      Callback function to be called if le ping payload timeout expired event is received
 * @param[in]  rsi_ble_event_disconnect, contains the disconnect status
 * @return      void
 * @section description
 * This callback function will be called if the le ping time expired event is received from the module
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_le_ltk_req_event_t) (rsi_bt_event_le_ltk_request_t *rsi_ble_event_le_ltk_request);




/******************************************************
 * *       BLE SMP EVENT Callbacks Declarations
 * ******************************************************/

/**
 * @callback   rsi_ble_on_smp_request_t
 * @brief      Callback function to be called if smp request is received in Master mode
 * @param[out] resp_status, contains the response status (Success or Error code)
 * @param[out] remote_dev_address, contains the smp requested device address
 * @return     void
 * @section description
 * This callback function will be called if the  smp request is received from the remote device
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_smp_request_t) (rsi_bt_event_smp_req_t  *remote_dev_address);

/*==============================================*/
/**
 * @callback   rsi_ble_on_smp_response_t
 * @brief      Callback function to be called if smp request is received in slave mode
 * @param[out] resp_status, contains the response status (Success or Error code)
 * @param[out] remote_dev_address, contains the smp requested device address
 * @return     void
 * @section description
 * This callback function will be called if the  smp request is received from the remote device
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_smp_response_t) (rsi_bt_event_smp_resp_t  *remote_dev_address);

/*==============================================*/
/**
 * @callback   rsi_ble_on_smp_passkey_t
 * @brief      Callback function to be called if smp passkey event is received from module
 * @param[out] resp_status, contains the response status (Success or Error code)
 * @param[out] remote_dev_address, contains the smp requested device address
 * @return     void
 * @section description
 * This callback function will be called if the  smp passkey is received from the module
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_smp_passkey_t) (rsi_bt_event_smp_passkey_t  *remote_dev_address);

/*==============================================*/
/**
 * @callback   rsi_ble_on_smp_passkey_display_t
 * @brief      Callback function to be called if smp passkey event is received from module
 * @param[out] resp_status, contains the response status (Success or Error code)
 * @param[out] remote_dev_address, contains the smp passkey information
 * @return     void
 * @section description
 * This callback function will be called if the  smp passkey is received from the module
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_smp_passkey_display_t) (rsi_bt_event_smp_passkey_display_t *smp_passkey_display);

/*==============================================*/
/**
 * @callback   rsi_ble_on_smp_failed_t
 * @brief      Callback function to be called if smp failed event is received from module
 * @param[out] resp_status, contains the response status (Success or Error code)
 * @param[out] remote_dev_address, contains the smp requested device address
 * @return     void
 * @section description
 * This callback function will be called if the  smp process is failed with remote device
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_smp_failed_t) (uint16_t resp_status, rsi_bt_event_smp_failed_t  *remote_dev_address);

/*==============================================*/
/**
 * @callback   rsi_ble_on_smp_encrypted_t
 * @brief      Callback function to be called if encrypted event is received from module
 * @param[out] resp_status, contains the response status (Success or Error code)
 * @return     void
 * @section description
 * This callback function will be called if the  encryption process is started with remote device
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_encrypt_started_t) (uint16_t resp_status, rsi_bt_event_encryption_enabled_t *enc_enabled);

/*==============================================*/
/**
 * @callback   rsi_ble_on_smp_passkey_display_t
 * @brief      Callback function to be called if encrypted event is received from module
 * @param[out] resp_status, contains the response status (Success or Error code)
 * @return     void
 * @section description
 * This callback function will be called if the  encryption process is started with remote device
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_smp_passkey_display_t ) (rsi_bt_event_smp_passkey_display_t *smp_passkey_display);

/*==============================================*/
/**
 * @callback   rsi_ble_on_sc_passkey_t
 * @brief      Callback function to be called if encrypted event is received from module
 * @param[out] resp_status, contains the response status (Success or Error code)
 * @return     void
 * @section description
 * This callback function will be called if the  encryption process is started with remote device
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_sc_passkey_t ) (rsi_bt_event_sc_passkey_t *sc_passkey);


/**
 * @callback   rsi_ble_on_phy_update_complete_t
 * @brief      Callback function to indicate MTU size
 * @param[out] rsi_ble_event_phy_update_complete,  contains the controller support phy information
 * @return      void
 * @section description
 * This callback function will be called when phy update complete event is received
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_phy_update_complete_t) (rsi_ble_event_phy_update_t *rsi_ble_event_phy_update_complete);


/*==============================================*/
/**
 * @callback   rsi_ble_on_data_length_update_t
 * @brief      This event is raised when data length is update in controller
 * @param[out] rsi_ble_data_length_update_t,  contains the controller support tx and rx length information
 * @return      void
 * @section description
 * This callback function will be called when data length update complete event is received
 *
 */
typedef void (*rsi_ble_on_data_length_update_t) (rsi_ble_event_data_length_update_t  *remote_dev_address);


/**
 * @callback   rsi_ble_on_directed_adv_report_event_t
 * @brief      Callback function to be called if directed advertise report event is received
 * @param[in]  rsi_ble_event_directed, contains the advertise report information
 * @return      void
 * @section description
 * This callback function will be called if the advertise event report is received from the module
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_directed_adv_report_event_t) (rsi_ble_event_directedadv_report_t *rsi_ble_event_directed);

/*==============================================*/
/**
 * @fn         rsi_ble_gap_register_callbacks
 * @brief      registers GAP callback functions
 * @param[in]  ble_on_adv_report_event, callback function for Advertise events
 * @param[in]  ble_on_connect, callback function for Connect events
 * @param[in]  ble_on_disconnect, callback function for Disconnect events
 *  * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function registers the function pointers for asynchronous GAP events
 */
void rsi_ble_gap_register_callbacks (
		rsi_ble_on_adv_report_event_t   ble_on_adv_report_event,
		rsi_ble_on_connect_t   ble_on_conn_status_event,
		rsi_ble_on_disconnect_t   ble_on_disconnect_event,
        rsi_ble_on_le_ping_payload_timeout_t   timeout_expired_event,
		rsi_ble_on_phy_update_complete_t      ble_on_phy_update_complete_event,
		rsi_ble_on_data_length_update_t       ble_on_data_length_update_complete_event,
		rsi_ble_on_enhance_connect_t   ble_on_enhance_conn_status_event,
		rsi_ble_on_directed_adv_report_event_t  ble_on_directed_adv_report_event);

void rsi_ble_smp_register_callbacks (
    rsi_ble_on_smp_request_t   ble_on_smp_request_event,
    rsi_ble_on_smp_response_t   ble_on_smp_response_event,
    rsi_ble_on_smp_passkey_t   ble_on_smp_passkey_event,
    rsi_ble_on_smp_failed_t   ble_on_smp_fail_event,
    rsi_ble_on_encrypt_started_t  rsi_ble_on_encrypt_started_event,
    rsi_ble_on_smp_passkey_display_t  ble_on_smp_passkey_display_event,
    rsi_ble_on_sc_passkey_t         ble_sc_passkey_event,
    rsi_ble_on_le_ltk_req_event_t  ble_on_le_ltk_req_event);

/******************************************************
 * *       BLE GATT Response Callbacks Declarations
 * ******************************************************/

/**
 * @callback   rsi_ble_on_profiles_list_resp_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  rsi_ble_resp_profiles, contains the profiles list response information
 * @return      void
 * @section description
 * This callback function will be called if the  profiles list response is received from the module
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_profiles_list_resp_t) (uint16_t resp_status, rsi_ble_resp_profiles_list_t  *rsi_ble_resp_profiles);

/**
 * @callback   rsi_ble_on_profile_resp_t
 * @brief      Callback function to be called if profile response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  rsi_ble_resp_profile, contains the profile response information
 * @return      void
 * @section description
 * This callback function will be called if the  profile response is received from the module
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_profile_resp_t) (uint16_t resp_status, profile_descriptors_t *rsi_ble_resp_profile);

/**
 * @callback   rsi_ble_on_char_services_resp_t
 * @brief      Callback function to be called if service characteristics response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  rsi_ble_resp_char_serv, contains the service characteristics response information
 * @return      void
 * @section description
 * This callback function will be called if the  service characteristics response is received from the module
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_char_services_resp_t) (uint16_t resp_status, rsi_ble_resp_char_services_t *rsi_ble_resp_char_serv);

/**
 * @callback   rsi_ble_on_inc_services_resp_t
 * @brief      Callback function to be called if include services response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  rsi_ble_resp_inc_serv, contains the include services response information
 * @return      void
 * @section description
 * This callback function will be called if the  include service response is received from the module
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_inc_services_resp_t) (uint16_t resp_status, rsi_ble_resp_inc_services_t *rsi_ble_resp_inc_serv);

/**
 * @callback   rsi_ble_on_att_desc_resp_t
 * @brief      Callback function to be called if attribute descriptors response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  rsi_ble_resp_att_desc, contains the attribute descriptors response information
 * @return      void
 * @section description
 * This callback function will be called if the attribute descriptors  response is received from the module
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_att_desc_resp_t) (uint16_t resp_status, rsi_ble_resp_att_descs_t *rsi_ble_resp_att_desc);

/**
 * @callback   rsi_ble_on_read_resp_t
 * @brief      Callback function to be called upon receiving the attribute value
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  resp_id, contains the resoponse id because of which, this callback is called
 *                      response ids: (RSI_BLE_RSP_READ_VAL, RSI_BLE_RSP_READ_BY_UUID, RSI_BLE_RSP_LONG_READ, RSI_BLE_RSP_MULTIPLE_READ)
 * @param[in]  rsi_ble_resp_att_val, contains the attribute value
 * @return      void
 * @section description
 * This callback function will be called upon receiving the attribute value
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_read_resp_t) (uint16_t resp_status, uint16_t resp_id, rsi_ble_resp_att_value_t *rsi_ble_resp_att_val);

/**
 * @callback   rsi_ble_on_write_resp_t
 * @brief      Callback function to be called if the attribute set/prepare/execute action is completed
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  resp_id, contains the response id because of which, this callback is called
 *                      response ids: (RSI_BLE_RSP_WRITE, RSI_BLE_RSP_WRITE_NO_ACK, RSI_BLE_RSP_LONG_WRITE, RSI_BLE_RSP_EXECUTE_WRITE)
 * @return      void
 * @section description
 * This callback function will be called if the attribute set/prepare/execute action is completed
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_write_resp_t) (uint16_t resp_status, uint16_t resp_id);

/******************************************************
 * *       BLE GATT Event Callbacks Declarations
 * ******************************************************/
/**
 * @callback   rsi_ble_on_gatt_write_event_t
 * @brief      Callback function to be called if the GATT write/notify/indicate events are received
 * @param[in]  event_id, contains the event id (RSI_BLE_EVENT_GATT_NOTIFICATION,  RSI_BLE_EVENT_GATT_INDICATION, RSI_BLE_EVENT_GATT_WRITE)
 * @param[in]  rsi_ble_write,  contains the GATT event information
 * @return      void
 * @section description
 * This callback function will be called if the GATT write/notify/indicate events are received
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_gatt_write_event_t) (uint16_t event_id, rsi_ble_event_write_t *rsi_ble_write);

/**
 * @callback   rsi_ble_on_gatt_write_event_t
 * @brief      Callback function to be called if the GATT write/notify/indicate events are received
 * @param[in]  event_id, contains the event id (RSI_BLE_EVENT_GATT_NOTIFICATION,  RSI_BLE_EVENT_GATT_INDICATION, RSI_BLE_EVENT_GATT_WRITE)
 * @param[in]  rsi_ble_write,  contains the GATT event information
 * @return      void
 * @section description
 * This callback function will be called if the GATT write/notify/indicate events are received
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_gatt_prepare_write_event_t) (uint16_t event_id, rsi_ble_event_prepare_write_t *rsi_ble_write);

/**
 * @callback   rsi_ble_on_execute_write_event_t
 * @brief      Callback function to be called if the GATT write/notify/indicate events are received
 * @param[in]  event_id, contains the event id (RSI_BLE_EVENT_GATT_NOTIFICATION,  RSI_BLE_EVENT_GATT_INDICATION, RSI_BLE_EVENT_GATT_WRITE)
 * @param[in]  rsi_ble_write,  contains the GATT event information
 * @return      void
 * @section description
 * This callback function will be called if the GATT write/notify/indicate events are received
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_execute_write_event_t) (uint16_t event_id, rsi_ble_execute_write_t *rsi_ble_execute_write);

/**
 * @callback   rsi_ble_on_read_req_event_t
 * @brief      Callback function to be called if the GATT write/notify/indicate events are received
 * @param[in]  event_id, contains the event id (RSI_BLE_EVENT_GATT_NOTIFICATION,  RSI_BLE_EVENT_GATT_INDICATION, RSI_BLE_EVENT_GATT_WRITE)
 * @param[in]  rsi_ble_write,  contains the GATT event information
 * @return      void
 * @section description
 * This callback function will be called if the GATT write/notify/indicate events are received
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_read_req_event_t) (uint16_t event_id, rsi_ble_read_req_t *rsi_ble_read_req);

/**
 * @callback   rsi_ble_on_mtu_event_t
 * @brief      Callback function to indicate MTU size
 * @param[in]
 * @param[in]  rsi_ble_event_mtu,  contains the MTU size information
 * @return      void
 * @section description
 * This callback function will be called when connected to indicate MTU size
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */

typedef void (*rsi_ble_on_mtu_event_t) (rsi_ble_event_mtu_t *rsi_ble_event_mtu);

/******************************************************
 * * BLE GATT Callbacks register function Declarations
 * ******************************************************/

/*==============================================*/
/**
 * @fn         rsi_ble_gatt_register_callbacks
 * @brief      registers GATT callback functions
 * @param[in]  ble_on_profiles_list_resp, callback function for profiles list response
 * @param[in]  ble_on_profile_resp, callback function for profile response
 * @param[in]  ble_on_char_services_resp, callback function for characteristics of the service response
 * @param[in]  ble_on_inc_services_resp, callback function for include service response
 * @param[in]  ble_on_att_desc_resp, callback function for characteristic descriptor response
 * @param[in]  ble_on_read_resp, callback function for characteristic value read response
 * @param[in]  ble_on_write_resp, callback function for characteristic value write response
 * @param[in]  ble_on_gatt_event, callback function for GATT events(Write/Indication/Notification)
 *  * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function registers the function pointers for GATT responses
 */
void rsi_ble_gatt_register_callbacks (
     rsi_ble_on_profiles_list_resp_t  ble_on_profiles_list_resp,
     rsi_ble_on_profile_resp_t        ble_on_profile_resp,
     rsi_ble_on_char_services_resp_t     ble_on_char_services_resp,
     rsi_ble_on_inc_services_resp_t      ble_on_inc_services_resp,
     rsi_ble_on_att_desc_resp_t       ble_on_att_desc_resp,
     rsi_ble_on_read_resp_t           ble_on_read_resp,
     rsi_ble_on_write_resp_t          ble_on_write_resp,
     rsi_ble_on_gatt_write_event_t    ble_on_gatt_event,
     rsi_ble_on_gatt_prepare_write_event_t  ble_on_gatt_prepare_write_event,
     rsi_ble_on_execute_write_event_t  ble_on_execute_write_event,
     rsi_ble_on_read_req_event_t       ble_on_read_req_event,
    rsi_ble_on_mtu_event_t            ble_on_mtu_event);

#endif
