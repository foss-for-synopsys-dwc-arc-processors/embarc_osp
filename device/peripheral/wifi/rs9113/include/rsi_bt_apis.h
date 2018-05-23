/**
 * @file     rsi_bt_apis.h
 * @version  0.1
 * @date     30 Sep 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains definitions and declarations of BLE APIs.
 *
 *  @section Description  This file contains definitions and declarations required to
 *  configure BT module.
 *
 *
 */

#ifndef RSI_BT_APIS_H
#define RSI_BT_APIS_H

#include<rsi_data_types.h>
#include<rsi_utils.h>
#include<rsi_bt_common.h>
#include<rsi_bt_config.h>

/******************************************************
 * *                      Macros
 * ******************************************************/
#define RSI_LINK_KEY_LEN                     16

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

//!BT Classic GAP Command structures

typedef struct rsi_bt_req_query_services_s
{
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_query_services_t;

//!BT Classic GAP Response structures

typedef struct rsi_bt_resp_query_services_s
{
  uint8_t  num_of_services;
  uint8_t  reserved[3];
  uint32_t  uuid[32];
} rsi_bt_resp_query_services_t;

typedef struct rsi_bt_resp_search_service_s
{
  uint8_t service_status;
} rsi_bt_resp_search_service_t;

//!BT Classic GAP Event structures

typedef struct rsi_bt_event_role_change_s
{
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t role_change_status;
} rsi_bt_event_role_change_t;

typedef struct rsi_bt_event_unbond_s
{
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_unbond_t;

typedef struct rsi_bt_event_bond_response_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_bond_t;

typedef struct rsi_bt_event_inquiry_response_s
{
  uint8_t  inquiry_type;
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t  name_length;
  uint8_t  remote_device_name[RSI_DEV_NAME_LEN];
  uint8_t  cod[3];
  uint8_t  rssi;
} rsi_bt_event_inquiry_response_t;

typedef struct rsi_bt_event_remote_device_name_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t  name_length;
  uint8_t  remote_device_name[RSI_DEV_NAME_LEN];
} rsi_bt_event_remote_device_name_t;

typedef struct rsi_bt_event_remote_name_request_cancel_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_remote_name_request_cancel_t;

typedef struct rsi_bt_event_disconnect_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_disconnect_t;

typedef struct rsi_bt_event_user_confirmation_request_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t  reserved[2];
  uint8_t  confirmation_value[4];
} rsi_bt_event_user_confirmation_request_t;

typedef struct rsi_bt_event_user_passkey_display_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t  reserved[2];
  uint8_t  passkey[4];
} rsi_bt_event_user_passkey_display_t;

typedef struct rsi_bt_event_user_passkey_notify_s {
    uint8_t  dev_addr[6];
    uint8_t  reserved[2];
    uint8_t  passkey[4];
} rsi_bt_event_user_passkey_notify_t;

typedef struct rsi_bt_event_user_pincode_request_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_user_pincode_request_t;

typedef struct rsi_bt_event_user_passkey_request_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_user_passkey_request_t;

typedef struct rsi_bt_event_auth_complete_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_auth_complete_t;

typedef struct rsi_bt_event_user_linkkey_request_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_user_linkkey_request_t;

typedef struct rsi_bt_event_user_linkkey_save_s {
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t  linkKey[RSI_LINK_KEY_LEN];
} rsi_bt_event_user_linkkey_save_t;

typedef struct rsi_bt_event_spp_receive_s
{
  uint16_t data_len;
  uint8_t  data[RSI_BT_MAX_PAYLOAD_SIZE];
} rsi_bt_event_spp_receive_t;

typedef struct rsi_bt_event_spp_connect_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_spp_connect_t;

typedef struct rsi_bt_event_spp_disconnect_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_spp_disconnect_t;

typedef struct rsi_bt_event_mode_change_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	uint8_t  current_mode;
  uint8_t  reserved;
	uint16_t  mode_interval;
} rsi_bt_event_mode_change_t;

typedef struct rsi_bt_event_sniff_subrating_s
{
  uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
	uint16_t  max_tx_latency;
	uint16_t  min_remote_timeout;
	uint16_t  min_local_timeout;
} rsi_bt_event_sniff_subrating_t;

typedef struct rsi_bt_event_ssp_complete_s
{
  uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
	uint8_t   status;
} rsi_bt_event_ssp_complete_t;


/******************************************************
 * *       BT GAP Response Callbacks Declarations
 * ******************************************************/

/**
 * @callback   rsi_bt_on_role_change_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  role_change_status, contains the role_change_status information
 * @return      void
 * @section description
 * This callback function will be called if the role change status cmd response is received from the module.
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_role_change_t) (uint16_t resp_status, rsi_bt_event_role_change_t *role_change_status);

/**
 * @callback   rsi_bt_on_connect_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  bond_response, contains the connect/bond response information
 * @return      void
 * @section description
 * This callback function will be called if new connection indication is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_connect_t) (uint16_t resp_status, rsi_bt_event_bond_t *bond_response);


/**
 * @callback   rsi_bt_on_unbond_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  unbond_status, contains the disconnection information
 * @return      void
 * @section description
 * This callback function will be called if disconnection indication is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_unbond_t) (uint16_t resp_status, rsi_bt_event_unbond_t *unbond_status);

/**
 * @callback   rsi_bt_on_disconnect_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  bt_disconnect, contains the disconnection information
 * @return      void
 * @section description
 * This callback function will be called if disconnection indication is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_disconnect_t) (uint16_t resp_status, rsi_bt_event_disconnect_t *bt_disconnect);

/**
 * @callback   rsi_bt_on_scan_resp_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  rsi_ble_resp_profiles, contains the inquiry response information
 * @return      void
 * @section description
 * This callback function will be called if the single scan response is received from the module in response to inuiry command.
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_scan_resp_t) (uint16_t resp_status, rsi_bt_event_inquiry_response_t *single_scan_resp);

/**
 * @callback   rsi_bt_on_remote_name_resp_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  name_resp, contains the remote device name information
 * @return      void
 * @section description
 * This callback function will be called if the remote name request cmd response is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_remote_name_resp_t) (uint16_t resp_status, rsi_bt_event_remote_device_name_t *name_resp);

/**
 * @callback   rsi_bt_on_passkey_display_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  bt_event_user_passkey_display, contains the passkey information
 * @return      void
 * @section description
 * This callback function will be called if passkey display request is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_passkey_display_t) (uint16_t resp_status, rsi_bt_event_user_passkey_display_t *bt_event_user_passkey_display);

/**
 * @callback   rsi_bt_on_remote_name_request_cancel_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  remote_name_request_cancel, contains the response to remote name request cancel cmd.
 * @return      void
 * @section description
 * This callback function will be called if the remote name request canacel cmd response is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_remote_name_request_cancel_t) (uint16_t resp_status, rsi_bt_event_remote_name_request_cancel_t *remote_name_request_cancel);

/**
 * @callback   rsi_bt_on_confirm_request_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  user_confirmation_request, contains the confirmation request information
 * @return      void
 * @section description
 * This callback function will be called if user confirmation request is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_confirm_request_t) (uint16_t resp_status, rsi_bt_event_user_confirmation_request_t *user_confirmation_request);

/**
 * @callback   rsi_bt_on_pincode_request_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  user_pincode_request, contains the pincode request information
 * @return      void
 * @section description
 * This callback function will be called if pincode request is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_pincode_request_t) (uint16_t resp_status, rsi_bt_event_user_pincode_request_t *user_pincode_request);

/**
 * @callback   rsi_bt_on_passkey_request_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  user_passkey_request, contains the passkey request information
 * @return      void
 * @section description
 * This callback function will be called if passkey request is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_passkey_request_t) (uint16_t resp_status, rsi_bt_event_user_passkey_request_t *user_passkey_request);

/**
 * @callback   rsi_bt_on_inquiry_complete_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @return      void
 * @section description
 * This callback function will be called if inquiry complete status is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_inquiry_complete_t) (uint16_t resp_status);

/**
 * @callback   rsi_bt_on_auth_complete_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  auth_complete, contains the authentication complete information
 * @return      void
 * @section description
 * This callback function will be called if authentication complete indication is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_auth_complete_t) (uint16_t resp_status, rsi_bt_event_auth_complete_t *auth_complete);

/**
 * @callback   rsi_bt_on_linkkey_request_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  user_linkkey_request, contains the linkkey request information
 * @return      void
 * @section description
 * This callback function will be called if linkkey request is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_linkkey_request_t) (uint16_t resp_status, rsi_bt_event_user_linkkey_request_t *user_linkkey_request);

/**
 * @callback   rsi_bt_on_ssp_complete_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  status, contains the ssp complete status
 * @return      void
 * @section description
 * This callback function will be called if ssp complete status is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_ssp_complete_t) (uint16_t resp_status, rsi_bt_event_ssp_complete_t *ssp_complete);

/**
 * @callback   rsi_bt_on_linkkey_save_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  user_linkkey_save, contains the linkkey save information
 * @return      void
 * @section description
 * This callback function will be called if linkkey save request is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_linkkey_save_t) (uint16_t resp_status, rsi_bt_event_user_linkkey_save_t *user_linkkey_save);

/**
 * @callback   rsi_bt_on_get_services_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  service_list, contains the response information to get services cmd.
 * @return      void
 * @section description
 * This callback function will be called if the get services command response is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_get_services_t) (uint16_t resp_status, rsi_bt_resp_query_services_t  *service_list);

/**
 * @callback   rsi_bt_on_search_service_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  status, contains the status response to search service cmd.
 * @return      void
 * @section description
 * This callback function will be called if the search services command response is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_search_service_t) (uint16_t resp_status, uint8_t *remote_dev_addr, uint8_t status);

/**
 * @callback   rsi_bt_on_mode_chnage_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  status, contains the status response to search service cmd.
 * @return      void
 * @section description
 * This callback function will be called if the search services command response is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_mode_chnage_t) (uint16_t resp_status, rsi_bt_event_mode_change_t  *mode_change);

/**
 * @callback   rsi_bt_on_sniff_subrating_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  status, contains the status response to search service cmd.
 * @return      void
 * @section description
 * This callback function will be called if the search services command response is received from the module
 * This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_sniff_subrating_t) (uint16_t resp_status, rsi_bt_event_sniff_subrating_t  *mode_change);


/******************************************************
 * *       BT SPP Response Callbacks Declarations
 * ******************************************************/

/**
 * @callback   rsi_bt_on_spp_connect_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  spp_connect, contains the spp connect information
 * @return      void
 * @section description
 * This callback function will be called if spp connection happens.
 * This callback has to be registered using rsi_bt_spp_register_callbacks API
 */
typedef void (*rsi_bt_on_spp_connect_t) (uint16_t resp_status, rsi_bt_event_spp_connect_t *spp_connect);

/**
 * @callback   rsi_bt_on_spp_disconnect_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  spp_disconnect, contains the spp disconnect information
 * @return      void
 * @section description
 * This callback function will be called if spp disconnection happens.
 * This callback has to be registered using rsi_bt_spp_register_callbacks API
 */
typedef void (*rsi_bt_on_spp_disconnect_t) (uint16_t resp_status, rsi_bt_event_spp_disconnect_t *spp_disconnect);

/**
 * @callback   rsi_bt_on_spp_rx_data_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  bt_event_spp_receive, contains the spp rcvd data information.
 * @return      void
 * @section description
 * This callback function will be called if spp data is received.
 * This callback has to be registered using rsi_bt_spp_register_callbacks API
 */
typedef void (*rsi_bt_on_spp_rx_data_t) (uint16_t resp_status, rsi_bt_event_spp_receive_t *bt_event_spp_receive);

/******************************************************
 * * BT GAP Callbacks register function Declarations
 * ******************************************************/

/*==============================================*/
/**
 * @fn          rsi_bt_register_gap_callbacks
 * @brief       This function registers the GAP callbacks
 * @param[in]   rsi_bt_on_connect_t                   bt_on_conn_status_event                         : Connection status callback
 * @param[in]   rsi_bt_on_unbond_t                    bt_on_unbond_status                         : Connection status callback
 * @param[in]   rsi_bt_on_disconnect_t                bt_on_disconnect_event                          : Disconnection status callback
 * @param[in]   rsi_bt_on_scan_resp_t                 bt_on_scan_resp_event                           : Scan report callback
 * @param[in]   rsi_bt_on_remote_name_resp_t          bt_on_remote_name_resp_event                    : Remote name report callback
 * @param[in]   rsi_bt_on_passkey_display_t           bt_on_passkey_display_event                     : Passkey display report callback
 * @param[in]   rsi_bt_on_remote_name_request_cancel_t          bt_on_remote_name_request_cancel_event: Remote name request cancel status callback
 * @param[in]   rsi_bt_on_confirm_request_t           bt_on_confirm_request_event                     : Authentication status callback
 * @param[in]   rsi_bt_on_pincode_request_t           bt_on_pincode_request_event                     : Pincode request status callback
 * @param[in]   rsi_bt_on_passkey_request_t           bt_on_passkey_request_event                     : Passkey request status callback
 * @param[in]   rsi_bt_on_inquiry_complete_t          bt_on_inquiry_complete_event                    : Inquiry report callback
 * @param[in]   rsi_bt_on_auth_complete_t             bt_on_auth_complete_event                       : Authentication status callback
 * @param[in]   rsi_bt_on_linkkey_request_t           bt_on_linkkey_request_event                     : Linkkey request report callback
 * @param[in]   rsi_bt_on_ssp_complete_t              bt_on_ssp_complete_event                        : SSP status callback
 * @param[in]   rsi_bt_on_linkkey_save_t              bt_on_linkkey_save_event                        : Linkkey save status callback
 * @param[in]   rsi_bt_on_get_services_t              bt_on_get_services_event                        : Get services report callback
 * @param[in]   rsi_bt_on_search_service_t            bt_on_search_service_event                      : Search service status callback
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function registers the BT Classic GAP callbacks
 *
 */
void rsi_bt_gap_register_callbacks (
    rsi_bt_on_role_change_t               bt_on_role_change_status_event,
    rsi_bt_on_connect_t                   bt_on_conn_status_event,
    rsi_bt_on_unbond_t                    bt_on_unbond_status ,
    rsi_bt_on_disconnect_t                bt_on_disconnect_event,
    rsi_bt_on_scan_resp_t                 bt_on_scan_resp_event,
    rsi_bt_on_remote_name_resp_t          bt_on_remote_name_resp_event,
    rsi_bt_on_passkey_display_t           bt_on_passkey_display_event,
    rsi_bt_on_remote_name_request_cancel_t          bt_on_remote_name_request_cancel_event,
    rsi_bt_on_confirm_request_t           bt_on_confirm_request_event,
    rsi_bt_on_pincode_request_t           bt_on_pincode_request_event,
    rsi_bt_on_passkey_request_t           bt_on_passkey_request_event,
    rsi_bt_on_inquiry_complete_t          bt_on_inquiry_complete_event,
    rsi_bt_on_auth_complete_t             bt_on_auth_complete_event,
    rsi_bt_on_linkkey_request_t           bt_on_linkkey_request_event,
    rsi_bt_on_ssp_complete_t              bt_on_ssp_complete_event,
    rsi_bt_on_linkkey_save_t              bt_on_linkkey_save_event,
    rsi_bt_on_get_services_t              bt_on_get_services_event,
    rsi_bt_on_search_service_t            bt_on_search_service_event,
		rsi_bt_on_mode_chnage_t               bt_on_mode_change_event,
		rsi_bt_on_sniff_subrating_t           bt_on_sniff_subrating_event
    );

/******************************************************
 * * BT SPP Callbacks register function Declarations
 * ******************************************************/

/*==============================================*/

/**
 * @fn          rsi_bt_spp_register_callbacks
 * @brief       This function registers the SPP callbacks
 * @param[in]   rsi_bt_on_spp_connect_t         bt_on_spp_connect_event              : spp connection status callback
 * @param[in]   rsi_bt_on_spp_disconnect_t      bt_on_spp_disconnect_event           : spp disconnection status callback
 * @param[in]   rsi_bt_on_spp_rx_data_t         bt_on_spp_rx_data_event              : spp data transfer status callback
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function registers the spp callbacks
 *
 */
void rsi_bt_spp_register_callbacks (
    rsi_bt_on_spp_connect_t         bt_on_spp_connect_event,
    rsi_bt_on_spp_disconnect_t      bt_on_spp_disconnect_event,
    rsi_bt_on_spp_rx_data_t         bt_on_spp_rx_data_event
    );


/*==============================================*/
/**
 * @fn         rsi_bt_set_local_class_of_device
 * @brief      sets the local device COD
 * @param[in]  class_of_device, name to be set to the local device COD
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function sets the given name to the local device COD
 */
int32_t rsi_bt_set_local_class_of_device(uint32_t class_of_device);

/*==============================================*/
/**
 * @fn         rsi_bt_get_local_class_of_device
 * @brief      request the local COD name
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used know the local device COD.
 */
int32_t rsi_bt_get_local_class_of_device(uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_start_discoverable
 * @brief      request the local device to enter discovery mode
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set the BT Module in discovery mode.
 */
int32_t rsi_bt_start_discoverable(void);

/*==============================================*/
/**
 * @fn         rsi_bt_start_limited_discoverable
 * @brief      request the local device to enter limited discovery mode
 * @param[in]  time_out_ms, Limited discovery mode time_out in ms.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set the BT Module in limited discovery mode.
 */
int32_t rsi_bt_start_limited_discoverable(int32_t time_out_ms);

/*==============================================*/
/**
 * @fn         rsi_bt_stop_discoverable
 * @brief      request the local device to exit discovery mode
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to exit the BT Module from discovery mode.
 */
int32_t rsi_bt_stop_discoverable(void);

/*==============================================*/
/**
 * @fn         rsi_bt_get_discoverable_status
 * @brief      request the local device discovery mode status
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to get BT Module discovery mode status.
 */
int32_t rsi_bt_get_discoverable_status(uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_set_connectable
 * @brief      request the local device to set connectablity mode
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set the BT Module in connectable mode.
 */
int32_t rsi_bt_set_connectable(void);

/*==============================================*/
/**
 * @fn         rsi_bt_set_non_connectable
 * @brief      request the local device to set connectablity mode
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set the BT Module in non-connectable mode.
 */
int32_t rsi_bt_set_non_connectable(void);

/*==============================================*/
/**
 * @fn         rsi_bt_get_connectable_status
 * @brief      request the local device connectablity status
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to get BT Module connectablity status.
 */
int32_t rsi_bt_get_connectable_status(uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_enable_authentication
 * @brief      request the local device to enable authentication
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to enable authentication.
 */
int32_t rsi_bt_enable_authentication(void);

/*==============================================*/
/**
 * @fn         rsi_bt_disable_authentication
 * @brief      request the local device to disable authentication
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to disable authentication.
 */
int32_t rsi_bt_disable_authentication(void);

/*==============================================*/
/**
 * @fn         rsi_bt_get_authentication
 * @brief      request authentication
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to initiate authentication.
 */
int32_t rsi_bt_get_authentication(void);

/*==============================================*/
/**
 * @fn         rsi_bt_remote_name_request_async
 * @brief      request the remote device name
 * @param[in]  remote_address, remote device address
 * @param[out] bt_event_remote_device_name, response buffer to hold the name of remote device
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used know the remote device name.
 */
int32_t rsi_bt_remote_name_request_async(int8_t *remote_dev_addr, rsi_bt_event_remote_device_name_t *bt_event_remote_device_name);

/*==============================================*/
/**
 * @fn         rsi_bt_remote_name_request_cancel
 * @brief      cancels the remote device name request
 * @param[in]  remote_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used cancel the remote device name request.
 */
int32_t rsi_bt_remote_name_request_cancel(int8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_bt_inquiry
 * @brief      request the local device to start inquiry
 * @param[in]  inquiry_type, inquiry type.
 * @param[in]  inquiry_duration, duration of inquiry.
 * @param[in]  max_devices, maximum devices.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start inquiry.
 */
int32_t rsi_bt_inquiry(uint8_t inquiry_type, uint32_t inquiry_duration, uint8_t max_devices);

/*==============================================*/
/**
 * @fn         rsi_bt_cancel_inquiry
 * @brief      request the local device to cancel inquiry
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to cancel inquiry.
 */
int32_t rsi_bt_cancel_inquiry(void);

/*==============================================*/
/**
 * @fn         rsi_bt_connect
 * @brief      request to initiate the connection with the remote device
 * @param[in]  remote_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to initiate the connection request
 */
int32_t rsi_bt_connect(int8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_bt_cancel_connect
 * @brief      request the connection cancel with the remote device
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to cancel the connection request
 * */
int32_t rsi_bt_cancel_connect(int8_t *remote_dev_address);

/*==============================================*/
/**
 * @fn         rsi_bt_disconnect
 * @brief      request the disconnect with the remote device
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to disconnect the physical connection
 * */
int32_t rsi_bt_disconnect(int8_t *remote_dev_address);

/*==============================================*/
/**
 * @fn         rsi_bt_accept_ssp_confirm
 * @brief      accepts the user confirmation
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used to give the confirmation for the values sent by remote BT device at the time of bonding.
 * */
int32_t rsi_bt_accept_ssp_confirm(int8_t *remote_dev_address);

/*==============================================*/
/**
 * @fn         rsi_bt_reject_ssp_confirm
 * @brief      rejects the User Confirmation
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used to reject the confirmation for the values sent by remote BT device at the time of bonding.
 * */
int32_t rsi_bt_reject_ssp_confirm(int8_t *remote_dev_address);

/*==============================================*/
/**
 * @fn         rsi_bt_passkey
 * @brief      requests the passkey
 * @param[in]  remote_dev_address, remote device address
 * @param[in] passkey, response buffer to hold the response of this API
 * @param[in] reply_type, reply type - positive or negative
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used to request the passkey.
 * */
int32_t rsi_bt_passkey(int8_t  *remote_dev_addr, uint32_t passkey, uint8_t reply_type);

/*==============================================*/
/**
 * @fn         rsi_bt_pincode_request_reply
 * @brief      accepts pincode
 * @param[in]  remote_dev_address, remote device address
 * @param[in]  pin_code, pin code input
 * @param[in]  reply_type, reply type - positive or negative
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used to accept the pincode.
 * */
int32_t rsi_bt_pincode_request_reply(int8_t  *remote_dev_addr, uint8_t *pin_code, uint8_t reply_type);

/*==============================================*/
/**
 * @fn         rsi_bt_linkkey_request_reply
 * @brief      accepts pincode
 * @param[in]  remote_dev_address, remote device address
 * @param[in]  pin_code, pin code input
 * @param[in]  reply_type, reply type - positive or negative
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used to accept the pincode.
 * */
int32_t rsi_bt_linkkey_request_reply (int8_t  *remote_dev_addr, uint8_t *linkkey, uint8_t reply_type);

/*==============================================*/
/**
 * @fn         rsi_bt_get_local_device_role
 * @brief      requests local device role
 * @param[in]  remote_dev_address, remote device address
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used to request the role of local device.
 * */
int32_t rsi_bt_get_local_device_role(int8_t  *remote_dev_addr, uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_get_services_async
 * @brief      queries remote device services
 * @param[in]  remote_dev_address, remote device address
 * @param[in]  num_of_responses, number of services expected
 * @param[out] bt_resp_query_services, response struct to hold the response of this API
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used to query the services of local device.
 * */
int32_t rsi_bt_get_services_async(int8_t *remote_dev_addr, rsi_bt_resp_query_services_t *bt_resp_query_services);

/*==============================================*/
/**
 * @fn         rsi_bt_search_service_async
 * @brief      queries local device services
 * @param[in]  remote_dev_address, remote device address
 * @param[in]  service_uuid, uuid of the service for search
 * @param[out] bt_resp_search_service, response struct to hold the response of this API
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used to search service of the given uuid.
 * */
int32_t rsi_bt_search_service_async(int8_t *remote_dev_addr, uint32_t service_uuid);

/*==============================================*/
/**
 * @fn         rsi_bt_sniff_mode
 * @brief      request the local device enter into sniff mode
 * @param[in]  remote_dev_addr, remote device address
 * @param[in]  sniff_max_intv,  Sniff maximum Interval
 * @param[in]  sniff_min_intv, Sniff Minimum Interval
 * @param[in]  sniff_attempt, Sniff Attempt
 * @param[in]  sniff_tout, Sniff timeout
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start inquiry.
 */
int32_t rsi_bt_sniff_mode(uint8_t *remote_dev_addr, uint16_t sniff_max_intv, uint16_t sniff_min_intv, uint16_t sniff_attempt, uint16_t sniff_tout);

/*==============================================*/
/**
 * @fn         rsi_bt_sniff_exit_mode
 * @brief      request the local device enter into sniff mode
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start inquiry.
 */
int32_t rsi_bt_sniff_exit_mode(uint8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_bt_sniff_subrating_mode
 * @brief      request the local device enter into sniff mode
 * @param[in]  remote_dev_addr, remote device address
 * @param[in]  max_latency, Maximum Latency
 * @param[in]  min_remote_tout, Minimum remote timeout
 * @param[in]  min_local_tout, Minimum local timeout
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start inquiry.
 */
int32_t rsi_bt_sniff_subrating_mode(uint8_t *remote_dev_addr, uint16_t max_latency, uint16_t min_remote_tout, uint16_t min_local_tout);

/*==============================================*/
/**
 * @fn         rsi_bt_set_ssp_mode
 * @brief      request the local device to enable/disable SSP mode
 * @param[in]  pair_mode, enable/disable the SSP mode
 * @param[in]  IOcapability, io capability of local device
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start inquiry.
 */
int32_t rsi_bt_set_ssp_mode(uint8_t pair_mode, uint8_t IOcapability);
#endif
