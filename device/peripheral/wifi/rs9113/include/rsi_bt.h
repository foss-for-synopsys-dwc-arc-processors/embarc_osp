/**
 * @file       rsi_bt.h
 * @version    0.1
 * @date       23 Sep 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief :   This file contains the Bt functionality related.
 *
 *  @section Description  This file contains all the BT structures.
 *
 */

#ifndef RSI_BT_H1
#define RSI_BT_H1

#include<rsi_data_types.h>
#include<rsi_driver.h>
#include<rsi_bt_common.h>
#include<rsi_bt_apis.h>

/******************************************************
 * *                      Macros
 * ******************************************************/
#define RSI_MAX_PINCODE_REPLY_SIZE                  17

/******************************************************
 * *                    Constants
 * ******************************************************/

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

/******************************************************
 * *                   Enumerations
 * ******************************************************/

//! enumeration for BT  command request codes
typedef enum rsi_bt_cmd_request_e
{
  RSI_BT_REQ_SET_LOCAL_COD                       = 0x0003,
  RSI_BT_REQ_QUERY_LOCAL_COD                     = 0x0004,
  RSI_BT_REQ_LINKKEY_REPLY                       = 0x0091,
  RSI_BT_REQ_SET_PROFILE_MODE                    = 0x0008,
  RSI_BT_REQ_SET_DISCV_MODE                      = 0x0009,
  RSI_BT_REQ_QUERY_DISCV_MODE                    = 0x000A,
  RSI_BT_REQ_SET_CONNECTABILITY_MODE             = 0x000B,
  RSI_BT_REQ_QUERY_CONNECTABILITY_MODE           = 0x000C,
  RSI_BT_REQ_SET_PAIR_MODE                       = 0x000D,
  RSI_BT_REQ_QUERY_PAIR_MODE                     = 0x000E,
  RSI_BT_REQ_REMOTE_NAME_REQUEST                 = 0x000F,
  RSI_BT_REQ_REMOTE_NAME_REQUEST_CANCEL          = 0x0010,
  RSI_BT_REQ_INQUIRY                             = 0x0011,
  RSI_BT_REQ_INQUIRY_CANCEL                      = 0x0012,
  RSI_BT_REQ_BOND                                = 0x0013,
  RSI_BT_REQ_BOND_CANCEL                         = 0x0014,
  RSI_BT_REQ_UNBOND                              = 0x0015,
  RSI_BT_REQ_USER_CONFIRMATION                   = 0x0018,
  RSI_BT_REQ_PASSKEY_REPLY                       = 0x0019,
  RSI_BT_REQ_PINCODE_REPLY                       = 0x001A,
  RSI_BT_REQ_QUERY_ROLE                          = 0x001B,
  RSI_BT_REQ_QUERY_SERVICES                      = 0x001D,
  RSI_BT_REQ_SEARCH_SERVICE                      = 0x001E,
  RSI_BT_REQ_SPP_CONNECT                         = 0x001F,
  RSI_BT_REQ_SPP_DISCONNECT                      = 0x0020,
  RSI_BT_REQ_SPP_TRANSFER                        = 0x0021,
  RSI_BT_REQ_PER_TX                              = 0x0098,
  RSI_BT_REQ_PER_RX                              = 0x0099,
  RSI_BT_REQ_PER_STATS                           = 0x009A,
  RSI_BT_REQ_CW_MODE                             = 0x009B,
  RSI_BT_REQ_SNIFF_MODE                          = 0x009D,
  RSI_BT_REQ_SNIFF_EXIT_MODE                     = 0x009E,
  RSI_BT_REQ_SNIFF_SUBRATING_MODE                = 0x009F,
  RSI_BT_REQ_SET_SSP_MODE                        = 0x00A0,
  RSI_BT_REQ_SET_EIR							 = 0X00A9,
}rsi_bt_cmd_request_t;

typedef enum rsi_bt_cmd_resp_e
{
  RSI_BT_RSP_CARD_READY                          = 0x0505,
  RSI_BT_RSP_SET_LOCAL_COD                       = 0x0003,
  RSI_BT_RSP_QUERY_LOCAL_COD                     = 0x0004,
  RSI_BT_RSP_LINKKEY_REPLY                       = 0x0091,
  RSI_BT_RSP_SET_PROFILE_MODE                    = 0x0008,
  RSI_BT_RSP_SET_DISCV_MODE                      = 0x0009,
  RSI_BT_RSP_QUERY_DISCV_MODE                    = 0x000A,
  RSI_BT_RSP_SET_CONNECTABILITY_MODE             = 0x000B,
  RSI_BT_RSP_QUERY_CONNECTABILITY_MODE           = 0x000C,
  RSI_BT_RSP_SET_PAIR_MODE                       = 0x000D,
  RSI_BT_RSP_QUERY_PAIR_MODE                     = 0x000E,
  RSI_BT_RSP_REMOTE_NAME_REQUEST                 = 0x000F,
  RSI_BT_RSP_REMOTE_NAME_REQUEST_CANCEL          = 0x0010,
  RSI_BT_RSP_INQUIRY                             = 0x0011,
  RSI_BT_RSP_INQUIRY_CANCEL                      = 0x0012,
  RSI_BT_RSP_BOND                                = 0x0013,
  RSI_BT_RSP_BOND_CANCEL                         = 0x0014,
  RSI_BT_RSP_UNBOND                              = 0x0015,
  RSI_BT_RSP_USER_CONFIRMATION                   = 0x0018,
  RSI_BT_RSP_PASSKEY_REPLY                       = 0x0019,
  RSI_BT_RSP_PINCODE_REPLY                       = 0x001A,
  RSI_BT_RSP_QUERY_ROLE                          = 0x001B,
  RSI_BT_RSP_QUERY_SERVICES                      = 0x001D,
  RSI_BT_RSP_SEARCH_SERVICE                      = 0x001E,
  RSI_BT_RSP_SPP_CONNECT                         = 0x001F,
  RSI_BT_RSP_SPP_DISCONNECT                      = 0x0020,
  RSI_BT_RSP_SPP_TRANSFER                        = 0x0021,
  RSI_BT_RSP_PER_TX                              = 0x0098,
  RSI_BT_RSP_PER_RX                              = 0x0099,
  RSI_BT_RSP_PER_STATS                           = 0x009A,
  RSI_BT_RSP_CW_MODE                             = 0x009B,

	/* sniff responses */
  RSI_BT_RSP_SNIFF_MODE                          = 0x009D,
  RSI_BT_RSP_SNIFF_EXIT_MODE                     = 0x009E,
  RSI_BT_RSP_SNIFF_SUBRATING_MODE                = 0x009F,
  RSI_BT_RSP_SET_SSP_MODE                        = 0x00A0,
  RSI_BT_RSP_SET_EIR							 = 0X00A9,

}rsi_bt_cmd_resp_t;

//! Event Frame Codes
typedef enum rsi_bt_event_e
{
  RSI_BT_EVT_ROLE_CHANGE_STATUS                  = 0x1000,
  RSI_BT_EVT_UNBOND_STATUS                       = 0x1001,
  RSI_BT_EVT_BOND_RESPONSE                       = 0x1002,
  RSI_BT_EVT_INQUIRY_RESPONSE                    = 0x1003,
  RSI_BT_EVT_REMOTE_DEVICE_NAME                  = 0x1004,
  RSI_BT_EVT_REMOTE_NAME_REQUEST_CANCEL          = 0x1005,
  RSI_BT_EVT_DISCONNECTED                        = 0x1006,
  RSI_BT_EVT_USER_CONFIRMATION_REQUEST           = 0x1007,
  RSI_BT_EVT_USER_PASKEY_DISPLAY                 = 0x1008,
  RSI_BT_EVT_USER_PINCODE_REQUEST                = 0x1009,
  RSI_BT_EVT_USER_PASSKEY_REQUEST                = 0x100A,
  RSI_BT_EVT_INQ_COMPLETE                        = 0x100B,
  RSI_BT_EVT_AUTH_COMPLETE                       = 0x100C,
  RSI_BT_EVT_USER_LINKKEY_REQUEST                = 0x100D,
  RSI_BT_EVT_USER_LINKKEY_SAVE                   = 0x100E,
  RSI_BT_EVT_SSP_COMPLETE 	                     = 0x100F,

  RSI_BT_EVT_MODE_CHANGED                        = 0x1010,
  RSI_BT_EVT_SNIFF_SUBRATING_CHANGED             = 0x1011,

  RSI_BT_EVT_USER_PASSKEY_NOTIFY                 = 0x1012,

  RSI_BT_EVT_SPP_RECEIVE                         = 0x1100,
  RSI_BT_EVT_SPP_CONNECTED                       = 0x1101,
  RSI_BT_EVT_SPP_DISCONNECTED                    = 0x1102,
}rsi_bt_event_t;

/******************************************************
 * *                    Structures
 * ******************************************************/

//!Set local COD name
typedef struct rsi_bt_req_set_local_cod_s
{
  uint32_t class_of_device;
}rsi_bt_req_set_local_cod_t;

//! Set Profile mode
typedef struct rsi_bt_req_profile_mode_s
{
  uint8_t  profile_mode;
}rsi_bt_req_profile_mode_t;

//! Set Discovery mode
typedef struct rsi_bt_req_set_discv_mode_s
{
  uint8_t  mode;
  uint8_t  reserved[3];
  int32_t  time_out;
} rsi_bt_req_set_discv_mode_t;

//! Query Discovery mode response
typedef struct rsi_bt_resp_query_discv_mode_s
{
  uint8_t  discovery_mode;
} rsi_bt_resp_query_discv_mode_t;

//! Set Connectability mode
typedef struct rsi_bt_req_set_connectability_mode
{
  uint8_t  connectability_mode;
}rsi_bt_req_set_connectability_mode_t;

//! Query Connectability mode response
typedef struct rsi_bt_resp_query_connectability_mode_s
{
  uint8_t  connectability_mode;
} rsi_bt_resp_query_connectability_mode_t;

//! Set pair mode
typedef struct rsi_bt_pair_mode_s
{
  uint8_t  pair_mode;
}rsi_bt_req_set_pair_mode_t;

//! Query pair mode response
typedef struct rsi_bt_resp_query_pair_mode_s
{
  uint8_t  pair_mode;
} rsi_bt_resp_query_pair_mode_t;

//! Remote Name Request
typedef struct rsi_bt_req_remote_name_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
}rsi_bt_req_remote_name_t;

//! Remote Name Request Cancel
typedef struct rsi_bt_req_remote_name_cancel_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
}rsi_bt_req_remote_name_cancel_t;

//! Inquiry command structure
typedef struct rsi_bt_req_inquiry_s
{
  uint8_t  inquiry_type;
  uint8_t  reserved[3];
  uint32_t  duration;
  uint8_t  maximum_devices_to_find;
  uint8_t  reserved1[3];
} rsi_bt_req_inquiry_t;

//! Connect command
typedef struct rsi_bt_req_connect_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
}rsi_bt_req_connect_t;

//! Connect cancel command
typedef struct rsi_bt_req_connect_cancel_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
}rsi_bt_req_connect_cancel_t;

//! Un-bond command
typedef struct rsi_bt_req_disconnect_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
}rsi_bt_req_disconnect_t;

//! User confirmation command structure
typedef struct rsi_bt_req_user_confirmation_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
	uint8_t  reserved[2];
  uint8_t  confirmation;
} rsi_bt_req_user_confirmation_t;

//! Passkey request reply command structure
typedef struct rsi_bt_req_passkey_reply_s
{
  uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t   reply_type;
  uint8_t   reserved;
  uint32_t   pass_key;
} rsi_bt_req_passkey_reply_t;

//! Pincode request reply command structure
typedef struct rsi_bt_req_pincode_reply_s
{
  uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t   reply_type;
  uint8_t   reserved;
  uint8_t   pincode[RSI_MAX_PINCODE_REPLY_SIZE];
} rsi_bt_req_pincode_reply_t;

//! Query Master-Slave role command
typedef struct rsi_bt_req_query_role_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_query_role_t;

//! Master-Slave role response
typedef struct rsi_bt_resp_query_role_s
{
  uint8_t  role;
} rsi_bt_resp_query_role_t;

//! Search Service command structure
typedef struct rsi_bt_req_search_service_s
{
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t reserved[2];
  uint32_t   service_uuid;
} rsi_bt_req_search_service_t;

//! SPP Connect command
typedef struct rsi_bt_req_spp_connect_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_spp_connect_t;

//! SPP disconnect command
typedef struct rsi_bt_req_spp_disconnect_s
{
  uint8_t  dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_spp_disconnect_t;

//! SPP transfer command stucture
typedef struct rsi_bt_req_spp_transfer_s
{
  uint8_t  data_length[2];
  uint8_t  data[RSI_BT_MAX_PAYLOAD_SIZE];
} rsi_bt_req_spp_transfer_t;

//! Linkkey request reply command structure
typedef struct rsi_bt_req_linkkey_reply_s
{
  uint8_t   dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t   reply_type;
  uint8_t   reserved;
  uint8_t   linkkey[RSI_LINK_KEY_LEN];
} rsi_bt_req_linkkey_reply_t;


//!Sniff Mode
typedef struct rsi_bt_req_sniff_mode_s
{
  uint8_t   dev_addr[6];
  uint16_t  sniff_max_intv;
  uint16_t  sniff_min_intv;
  uint16_t  sniff_attempt;
  uint16_t  sniff_tout;
} rsi_bt_req_sniff_mode_t;


//!Sniff Exit Mode
typedef struct rsi_bt_req_sniff_exit_mode_s
{
  uint8_t   dev_addr[6];
} rsi_bt_req_sniff_exit_mode_t;

//!Sniff Subrating Mode
typedef struct rsi_bt_req_sniff_subrating_mode_s
{
  uint8_t   dev_addr[6];
  uint16_t  max_latency;
  uint16_t  min_remote_tout;
  uint16_t  min_local_tout;
} rsi_bt_req_sniff_subrating_mode_t;

//!Set SSP Mode
typedef struct rsi_bt_req_set_ssp_mode_s
{
  uint8_t   pair_mode;
  uint8_t   IO_capability;
} rsi_bt_req_set_ssp_mode_t;

//!Set EIR data
typedef struct rsi_bt_set_eir_data_s
{
    uint8_t  fec_required;
	uint8_t  data_len;
	uint8_t  eir_data[200];
}rsi_bt_set_eir_data_t;

//! Driver BT Classic control block
struct rsi_bt_classic_cb_s
{
  //! GAP Callbacks
  rsi_bt_on_role_change_t               bt_on_role_change_status_event;
  rsi_bt_on_connect_t                   bt_on_conn_status_event;
  rsi_bt_on_unbond_t                    bt_on_unbond_status;
  rsi_bt_on_disconnect_t                bt_on_disconnect_event;
  rsi_bt_on_scan_resp_t                 bt_on_scan_resp_event;
  rsi_bt_on_remote_name_resp_t          bt_on_remote_name_resp_event;
  rsi_bt_on_passkey_display_t           bt_on_passkey_display_event;
  rsi_bt_on_remote_name_request_cancel_t          bt_on_remote_name_request_cancel_event;
  rsi_bt_on_confirm_request_t           bt_on_confirm_request_event;
  rsi_bt_on_pincode_request_t           bt_on_pincode_request_event;
  rsi_bt_on_passkey_request_t           bt_on_passkey_request_event;
  rsi_bt_on_inquiry_complete_t          bt_on_inquiry_complete_event;
  rsi_bt_on_auth_complete_t             bt_on_auth_complete_event;
  rsi_bt_on_linkkey_request_t           bt_on_linkkey_request_event;
  rsi_bt_on_ssp_complete_t              bt_on_ssp_complete_event;
  rsi_bt_on_linkkey_save_t              bt_on_linkkey_save_event;
  rsi_bt_on_get_services_t              bt_on_get_services_event;
  rsi_bt_on_search_service_t            bt_on_search_service_event;
  rsi_bt_on_mode_chnage_t               bt_on_mode_change_event;
  rsi_bt_on_sniff_subrating_t	        bt_on_sniff_subrating_event;

  //! SPP Callbacks
  rsi_bt_on_spp_connect_t         bt_on_spp_connect_event;
  rsi_bt_on_spp_disconnect_t      bt_on_spp_disconnect_event;
  rsi_bt_on_spp_rx_data_t         bt_on_spp_rx_data_event;

};

/******************************************************
 * * BT Classic internal function declarations
 * ******************************************************/
void rsi_bt_callbacks_handler(rsi_bt_cb_t *bt_cb, uint16_t rsp_type, uint8_t *payload, uint16_t payload_length);

#endif
