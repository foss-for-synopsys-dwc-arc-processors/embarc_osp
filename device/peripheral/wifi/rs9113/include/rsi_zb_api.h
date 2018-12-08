/**
 *  @file     rsi_zigb_api.h
 *   @version  1.0
 *   @date     2014-Sep-13
 *
 *   Copyright(C) Redpine Signals 2014
 *   All rights reserved by Redpine Signals.
 *
 *   @section License
 *   This program should be used on your own responsibility.
 *   Redpine Signals assumes no responsibility for any losses
 *   incurred by customers or third parties arising from the use of this file.
 *
 *   @brief API: Definitions of various data structures and variables
 *
 *   @section Description
 *   This file contain definition of different mangament, control & data commands variables.
 *   These definition are used to construct frames.
 *
 *   @section Improvements
 *   New command frames are added.
 *
 */


/**
 * Includes
 * */


#ifndef RSI_ZIGB_API_H
#define RSI_ZIGB_API_H

#include "rsi_zb_types.h"
#include "rsi_zb_interfaces.h"
#ifdef ZB_PROFILE
#include "../zigbee/profiles/include/zdo_interface.h"
#include "../zigbee/profiles/include/stack_functionality.h"
#endif
#ifdef ZB_DEBUG
#include <stdio.h>
#endif

#ifdef ZB_MAC_API
#include "rsi_mac_interface.h"
#endif
#define SERIAL_READ_BUFFER_COUNT  10
#define MAX_PACKET_SIZE           256
#define MAX_SCAN_DURATION	        0xA

//! Interface Types
#define MANAGEMENT_INTERFACE	   0x01
#define DATA_INTERFACE			     0x02
#define SECURITY_INTERFACE		   0x03
#define BINDING_INTERFACE		     0x04
#define PACKET_DATA				       0x05
#define INTERFACE_CALLBACK		   0x06
#define INTERNAL_MANAGEMENT_INTERFACE	0x07

//! Direction
#define DIR_HOST_TO_DEVICE 0x01
#define DIR_DEVICE_TO_HOST 0x02




#define MAX_PKT_LENGTH          256
#define SHORT_PANID_SIZE        2
#define EXTENDED_PANID_SIZE     8
#define EXTENDED_ADDR_SIZE      8

#define RSI_ZIGB_MAX_PAYLOAD_SIZE       256
#define RSI_ZIGB_LARGE_BUFFER_SIZE      156
#define RSI_ZIGB_ZDO_SILENT_START       13

#define RSI_ZIGB_PAYLOAD_LEN_MASK       0xFFF
#define RSI_ZIGB_QUEUE                  0x10
#define RSI_ZIGB_FRAME_DESC_LEN         16
#define RSI_ZIGB_EXTENDDED_ADDR_LEN     8
#define RSI_ZIGB_BYTES_2                2

#define RSI_ZIGB_SEQ_NUM_OFFSET         12
#define RSI_ZIGB_DIR_OFFSET             13
#define RSI_ZIGB_INTF_ID_OFFSET         14
#define CARD_READY_IND                  0xff
#define ZB_PKT                          (RSI_ZIGB_QUEUE << 4)
#define m_SIZE_CONFIGURATION_c				            0x80

#define RSI_TRUE            1
#define RSI_FALSE           0
#ifndef NULL
#define NULL                0
#endif

#ifdef LINUX_PLATFORM
#define RSI_DPRINT(lvl, fmt, args...)              if (lvl & RSI_DEBUG_LVL) printf(fmt, ##args)
#else
#define RSI_DPRINT
#endif
#define RSI_DEBUG_LVL         0x00ff

//! These bit values may be ored to all different combinations of debug printing
#define RSI_PL0                0xffff
#define RSI_PL1                0x0001
#define RSI_PL2                0x0002
#define RSI_PL3                0x0004
#define RSI_PL4                0x0008
#define RSI_PL5                0x0010
#define RSI_PL6                0x0020
#define RSI_PL7                0x0040
#define RSI_PL8                0x0080
#define RSI_PL9                0x0100
#define RSI_PL10               0x0200
#define RSI_PL11               0x0400
#define RSI_PL12               0x0800
#define RSI_PL13               0x1000
#define RSI_PL14               0x2000
#define RSI_PL15               0x4000
#define RSI_PL16               0x8000

//! Commands
//! Management APIs
typedef enum rsi_zigb_cmd_request_e
{
 ZIGBEEFORMNETWORK                      = 0x01,
 ZIGBEEJOINNETWORK                      = 0x02,
 ZIGBEEPERMITJOIN                       = 0x03,
 ZIGBEELEAVENETWORK                     = 0x04,
 ZIGBEEFINDNETWORKANDPERFORMREJOIN      = 0x05,
 ZIGBEEREJOINNETWORK                    = 0x06,
 ZIGBEENETWORKRESTORE                   = 0x07,
 ZIGBEEINITIATESCAN                     = 0x08,
 ZIGBEESTOPSCAN                         = 0x09,
 ZIGBEENETWORKSTATE                     = 0x0A,
 ZIGBEESTACKISUP                        = 0x0B,
 ZIGBEEGETSELFIEEEADDRESS               = 0x0C,
 ZIGBEEISITSELFIEEEADDRESS              = 0x0D,
 ZIGBEEGETSELFSHORTADDRESS              = 0x0E,
 ZIGBEESETMANUFACTURERCODEFORNODEDESC   = 0x0F,
 ZIGBEESETPOWERDESCRIPTOR               = 0x10,
 ZIGBEESETMAXMINCOMINGTXFRSIZE          = 0x11,
 ZIGBEESETMAXMOUTGOINGTXFRSIZE          = 0x12,
 ZIGBEESETOPERATINGCHANNEL              = 0x13,
 ZIGBEEGETDEVICETYPE                    = 0x14,
 ZIGBEEGETOPERATINGCHANNEL              = 0x15,
 ZIGBEEGETSHORTPANID                    = 0x16,
 ZIGBEEGETEXTENDEDPANID                 = 0x17,
 ZIGBEEGETENDPOINTID                    = 0x18,
 ZIGBEEGETSIMPLEDESCRIPTOR              = 0x19,
 ZIGBEEGETENDPOINTCLUSTER               = 0x1A,
 ZIGBEEGETSHORTADDRFORSPECIFIEDIEEEADDR = 0x1B,
 ZIGBEESTACKPROFILE                     = 0x1C,
 ZIGBEEGETIEEEADDRFORSPECIFIEDSHORTADDR = 0x1D,
 ZIGBEEREADNEIGHBORTABLEENTRY           = 0x1E,
 ZIGBEEGETROUTETABLEENTRY               = 0x1F,
 ZIGBEETREEDEPTH                        = 0x20,
 ZIGBEEGETNEIGHBORTABLEENTRYCOUNT       = 0x21,
 ZIGBEEGETCHILDSHORTADDRESSFORTHEINDEX  = 0x22,
 ZIGBEEGETCHILDINDEXFORSPECIFIEDSHORTADDR =  0x23,
 ZIGBEEGETCHILDDETAILS                  = 0x24,
 ZIGBEEENDDEVICEPOLLFORDATA             = 0x25,
 ZIGBEEREADCOUNTOFCHILDDEVICES          = 0x26,
 ZIGBEEREADCOUNTOFROUTERCHILDDEVICES    = 0x27,
 ZIGBEEREADMAXCHILDROUTERDEVICESCOUNT   = 0x28,
 ZIGBEEGETPARENTSHORTADDRESS            = 0x29,
 ZIGBEEGETPARENTIEEEADDRESS             = 0x2A,
 ZIGBEEINITIATEENERGYSCANREQUEST        = 0x2B,
 ZIGBEEBROADCASTNWKMANAGERREQUEST       = 0x2C,
 ZDPSENDNWKADDRREQUEST                  = 0x2D,
 ZDPSENDIEEEADDRREQUEST                 = 0x2E,
 ZDPSENDDEVICEANNOUNCEMENT              = 0x2F,
 ZDPSENDMATCHDESCRIPTORSREQUEST         = 0x30,
 ZIGBEEACTIVEENDPOINTSREQUEST           = 0x31,
 ZDPSENDPOWERDESCRIPTORREQUEST          = 0x32,
 ZDPSENDNODEDESCRIPTORREQUEST           = 0x33,
 ZIGBEESIMPLEDESCRIPTORREQUEST          = 0x34,
 ZIGBEEGETADDRESSMAPTABLEENTRY          = 0x35,
 ZIGBEESENDUNICASTDATA                  = 0x36,
 ZIGBEESENDGROUPDATA                    = 0x37,
 ZIGBEESENDBROADCASTDATA                = 0x38,
 ZIGBEEGETMAXAPSPAYLOADLENGTH           = 0x39,
 ZIGBEESETBINDINGENTRY                  = 0x3A,
 ZIGBEEDELETEBINDING                    = 0x3B,
 ZIGBEEISBINDINGENTRYACTIVE             = 0x3C,
 ZIGBEECLEARBINDINGTABLE                = 0x3D,
 ZIGBEEBINDREQUEST                      = 0x3E,
 ZIGBEEENDDEVICEBINDREQUEST             = 0x3F,
 ZIGBEEUNBINDREQUEST                    = 0x40,
 ZIGBEEGETKEY                           = 0x41,
 ZIGBEEHAVELINKKEY                      = 0x42,
 ZIGBEESWITCHNETWORKKEYHANDLER          = 0x43,
 ZIGBEEREQUESTLINKKEY                   = 0x44,
 ZIGBEEGETKEYTABLEENTRY                 = 0x45,
 ZIGBEESETKEYTABLEENTRY                 = 0x46,
 ZIGBEEADDORUPDATEKEYTABLEENTRY         = 0x47,
 ZIGBEEFINDKEYTABLEENTRY                = 0x48,
 ZIGBEEERASEKEYTABLEENTRY               = 0x49,
 ZIGBEESETSIMPLEDESCRIPTOR              = 0x4A,
 ZIGBEELASTTYPE                         = 0x4A,
 APPSCANCOMPLETEHANDLER                 = (ZIGBEELASTTYPE + 1),
 APPENERGYSCANRESULTHANDLER             = (APPSCANCOMPLETEHANDLER+1),
 APPNETWORKFOUNDHANDLER                 = (APPENERGYSCANRESULTHANDLER+1),
 APPZIGBEESTACKSTATUSHANDLER            = (APPNETWORKFOUNDHANDLER +1),
 ZIGBEECHILDJOINHANDLER                 = (APPZIGBEESTACKSTATUSHANDLER+1),
 APPCHILDJOINHANDLER                    = ZIGBEECHILDJOINHANDLER,
 APPINCOMINGMANYTOONEROUTEREQUESTHANDLER= (ZIGBEECHILDJOINHANDLER + 1),
 APPHANDLEDATAINDICATION                = (APPINCOMINGMANYTOONEROUTEREQUESTHANDLER + 1),
 APPHANDLEDATACONFIRMATION              = (APPHANDLEDATAINDICATION + 1),
 APPHANDLEINTERPANDATAINDDICATION				=	(APPHANDLEDATACONFIRMATION + 1),
 APPHANDLEINTERPANDATACONFIRMATION			=	(APPHANDLEINTERPANDATAINDDICATION + 1),
 APPHANDLENVMDATA                       = (0x5F),
 ZIGBEEGETBINDINGINDICES                = 0x60,
 ZIGBEESTACKINIT                        = 0x61,
 ZIGBEESTACKRESET                       = 0x62,
 ZIGBEEZDOUPDATESAS                     = 0x63,
 ZIGBEEREADSAS                          = 0x64,
 ZIGBEEUPDATESAS                        = 0x65,
 ZIGBEEUPDATEZDO                        = 0X66,
 ZIGBEESETPROFILE                        = 0x67,
 ZIGBEEINITPS                           =0x68,
 ZIGBEEGETROUTERECORDFORDEV               = 0x70,
 ZIGBEEAPSREMGRP                         = 0x71,
 ZIGBEESEARCHADDR                         = 0x72,
 ZIGBEEGETGRPTABLEINDX                   = 0x73,
 ZIGBEEAPSADDGRP                        = 0x74,
 ZIGBEEREMALLGRP                        = 0x75,
 ZIGBEEGETGRPID                         = 0x76,
 ZIGBEEVERFYGRPEP                       = 0x77,
 ZIGBEEGETNWKMAXDEPTH                   = 0x78,
 ZIGBEEGETSECURITYLVL                   = 0x79,
 ZIGBEEGETTCSHORTADDR                   = 0x80,
 ZIGBEEGETMAXAPSLEN                     = 0x81,
 ZIGBEEGETTCLINKKEY                     = 0x82,
 ZIGBEESETTCLINKKEY                     = 0x83,
 ZIGBEEZDOAPPZDPREQ                     = 0x84,
 ZIGBEENWKSENDREQ                       = 0x85,
 ZIGBEEZDOSILENTSTARTREQ                = 0x86,
 ZIGBEEGETSETUPDATEID                   = 0x87,
 ZIGBEESETZLLPARAMS                     = 0x88,
 ZIGBEEAESENCRYPT                       = 0x89,
 ZIGBEEGETNWKKEY                        = 0x8a,
 ZIGBEESETMACPIB                        = 0x8b,
 ZIGBEENVMUPDATE                        = 0x8c,
 ZIGBEEAPSDEREQ                         = 0x8d,
 ZIGBEEUPDATEALLZDO                     = 0X90,
 ZIGBEEUPDATEALLSAS                     = 0x91,
  ZIGBEEENQUEUEMACMLME                   = 0xA1,
 ZIGBEEENQUEUEMACMCPS                   = 0xA2,
 ZIGBEEMACPIBSET                        = 0xA3,
 ZIGBEEMACPIBGET                        = 0xA4,
 ZIGBEEGETCCASTATUS                     = 0xA6,

 ZIGBMACMLMECB                          = 0xB0,
 ZIGBMACMCPSCB                          = 0xB1,
 ZIGBEEPERMODE							            = 0xFE,
// ZIGBEE_OPER_MODE_RSP                   = 0xFE,
// PER cmd id
 ZIGBEEZDPAPPRESPONSE                   = 0xF0,
 ZIGBEEAPSASDUHANDLER	        	= 0xF1,
 ZIGBEEPROFILETESTENABLE		= 0xF2,
 ZIGBEESETSTACKEVENT			= 0xF3,
 ZIGBEECLRSTACKEVENT			= 0xF4,
 ZIGBEEGETRSSI			        = 0xF6,

 ZIGBEE_CARD_READY                      = 0xFF,
 ZIGBEEDEINITSTACK                      = 0xFF
}rsi_zigb_cmd_request_t;

typedef enum rsi_zigb_state_e {
  FSM_CARD_NOT_READY = 0,
  FSM_SCAN_COMPLETE,
  FSM_JOIN_COMPLETE,
  FSM_ZB_PS_CMD,
  FSM_ZB_MATCH_DESC_RESP,
  FSM_ZB_DATA_CONFRM,
  FSM_ZB_SEND_DATA,
  FSM_ZB_NWK_ADDR_RESP,
  FSM_ZB_SIMPLE_DESC_RESP,
  FSM_ZB_IEEE_ADDR_RESP,
  FSM_ZB_ACTIVE_RP_RESP,
  FSM_ZB_BIND_RESP,
  FSM_ZB_UNBIND_RESP,
  FSM_ZB_NODE_DESC_RESP,
  FSM_ZB_PWR_DESC_RESP
} rsi_zigb_state_t;


#define ZIGBEESETAESKEY                         0x1
#define ZIGBEESETEXTPANID                       0x2
#define ZIGBEESETTCADDR                         0x3
#define ZIGBEESETSHORTPANID                     0x4
#define ZIGBEESETSELFSHORTADDR                  0x5
#define ZIGBEESETRXONWHENIDLE                   0x6
#define ZIGBEESETPHYTRXSTATE                    0x7
#define ZIGBEESETNWKKEY                         0x8
#define ZIGBEESETEXTADDR                        0x9
#define ZIGBEESETSASINDEX                       0xA
#define ZIGBEESTARTDEVICEREQ                    0xBD
#define ZIGBEETRIGGERTOUCHLINK	 		0xFD
#define ZIGBEEZLLSETOPERCHANNEL			0xF1
#define ZIGBEEZLLGETOPERCHANNEL			0xF2

typedef void (*rsi_zigb_app_scan_complete_handler_t) ( uint32_t channel, uint8_t status );
typedef void (*rsi_zigb_app_energy_scan_result_handler_t)( uint32_t channel,uint8_t *pEnergyValue);
typedef void (*rsi_zigb_app_network_found_handler_t) (ZigBeeNetworkDetails networkInformation);
typedef void (*rsi_zigb_app_stack_status_handler_t) (ZigBeeNWKStatusInfo *statusInfo);
typedef void (*rsi_zigb_app_incoming_many_to_one_route_req_handler_t) ( uint16_t SourceAddr, uint8_t * pSrcIEEEAddr,uint8_t PathCost );
typedef void (*rsi_zigb_app_handle_data_indication_t) (APSDE_Data_Indication_t *pDataIndication);
typedef void (*rsi_zigb_app_handle_data_confirmation_t) (APSDE_Data_Confirmation_t *pDataConfirmation);
typedef void (*rsi_zigb_app_child_join_handler_t) (uint16_t short_address, char joining);
typedef void (*rsi_zigb_nvm_backup_handler_t)(void *data, uint32_t offset);
typedef void (*rsi_zigb_zdp_app_response_handler_t)(uint8_t* buffer);
#ifdef ZB_MAC_API
typedef int16_t (*rsi_zigb_mac_data_confirm_t)(MCPS_Data_Confirm_t* pMCPS_Data_Confirm);
typedef int16_t (*rsi_zigb_mac_data_indication_t)(MCPS_Data_Indication_t* pMCPS_Data_Indication);
typedef int16_t (*rsi_zigb_mac_assoc_confirm_t)(MLME_Associate_Confirm_t* pMLME_Associate_Confirm);
typedef int16_t (*rsi_zigb_mac_assoc_indication_t)(MLME_Associate_Indication_t* pMLME_Associate_Indication);
typedef int16_t (*rsi_zigb_mac_disassoc_confirm_t)(MLME_Disassociate_Confirm_t* pMLME_Disassociate_Confirm);
typedef int16_t (*rsi_zigb_mac_disassoc_indication_t)(MLME_Disassociate_Indication_t* pMLME_Disassociate_Indication);
typedef int16_t (*rsi_zigb_mac_beacon_notify_indication_t)(MLME_Beacon_Notify_Indication_t* pMLME_Beacon_Notify_Indication);
typedef int16_t (*rsi_zigb_mac_orphan_indication_t)(MLME_Orphan_Indication_t* pMLME_Orphan_Indication);
typedef int16_t (*rsi_zigb_mac_rx_enable_confirm_t)(MLME_RX_Enable_Confirm_t* pMLME_RX_Enable_Confirm);
typedef int16_t (*rsi_zigb_mac_scan_confirm_t)(MLME_Scan_Confirm_t* pMLME_Scan_Confirm);
typedef int16_t (*rsi_zigb_mac_comm_status_confirm_t)(MLME_Comm_Status_Indication_t* pMLME_Comm_Status_Indication);
typedef int16_t (*rsi_zigb_mac_start_confirm_t)(MLME_Start_Confirm_t* pMLME_Start_Confirm);
typedef int16_t (*rsi_zigb_mac_sync_loss_indication_t)(MLME_Sync_Loss_Indication_t* pMLME_Sync_Loss_Indication);
typedef int16_t (*rsi_zigb_mac_poll_confirm_t)(MLME_Poll_Confirm_t* pMLME_Poll_Confirm);

#endif

#ifdef ZB_PROFILE
typedef void (*rsi_zigb_app_interpan_data_indication_t)(uint8_t bufferid,APSDE_InterPan_Data_Indication_t *pApsdeDataInd);
typedef void (*rsi_zigb_app_interpan_data_confirmation_t)(uint8_t bufferid,APSDE_InterPan_Data_Confirmation_t *pInterPanDataConf);
#endif

typedef struct {
  uint8_t                 status;
  uint8_t                 type;
} rsi_DevTypeResp;


typedef struct {
  uint8_t                 channel;
} rsi_ChannelResp;

typedef struct {
  uint8_t                 status;
} rsi_StatusResp;

typedef struct {
  uint8_t                 status;
} rsi_SetSimpleDescResp;

typedef struct {
  uint32_t                channel;
  uint32_t                 status;
} rsi_ScanDoneResp;

typedef struct {
  uint8_t                Self_ieee[8] ;
} rsi_SelfIEEEAddrResp;

typedef struct {
  uint8_t                Addr[8] ;
} rsi_IEEEAddResp;

typedef struct {
  uint8_t                Ext_Pan_Id[8] ;
} rsi_ExtPanIdResp;

typedef struct {
  uint16_t                short_addr ;
} rsi_ShortAddrResp;

typedef struct {
  uint16_t                cluster ;
} rsi_ClusterResp;

typedef struct {
  uint16_t                pan_id ;
} rsi_PanIdResp;


typedef struct {
  uint8_t                 status;
  uint8_t                 Ieee_Add[8];
} rsi_IEEEResp;


typedef struct {
  uint8_t                 status;
  uint8_t                 Ieee_Addr[8];
  uint8_t                 device_type;
} rsi_ChildDetailsResp;

typedef struct {
  uint8_t       status;
  ZigBeeNeighborTableEntry_t neighbour_table;
}rsi_ReadNeighborTableResp;


typedef struct {
  uint8_t       status;
  ZigBeeRoutingTableEntry_t routing_table;
}rsi_ReadRouteTableResp;

typedef struct {
  uint8_t       status;
  ZigBeeKeyStructure_t key_struct;
}rsi_GetKeytable;
typedef struct {
  uint8_t     rssi_value;

}rsi_zbrssivalue;

/*=========================================================================*
 * Frame Descriptor
 *=========================================================================*/

typedef struct {
    uint8_t   uFrmDscBuf[RSI_ZIGB_FRAME_DESC_LEN];       //@ byte format for spi interface, 16 bytes
} rsi_zigb_uFrameDsc;

/*=========================================================================*
 * Response Frame Payload
 *=========================================================================*/

typedef struct {
  // !seq_no and dir are for debugging purpose
  //@ 0- For Success ,Non-Zero Value is the Error Code return
  union {
    //@ response payload
    rsi_ChannelResp         DevChannel;
    rsi_StatusResp          statusResp;
    rsi_ScanDoneResp        scandoneResp;
    rsi_DevTypeResp         GetDevResp;
    rsi_SetSimpleDescResp   SetSimpleDescResp;
    rsi_IEEEAddResp         IEEEAddrResp;
    rsi_SelfIEEEAddrResp    DevIEEEAddr;
    rsi_ShortAddrResp       DevShortAddr;
    rsi_PanIdResp           DevPanId;
    rsi_ExtPanIdResp        Extnd_PanId;
    rsi_ClusterResp         EpClusterResp;
    rsi_IEEEResp            GetIEEEResp;
    rsi_ChildDetailsResp    GetChildDetails;
    rsi_GetSimpleDescResp   GetSimpleDescResp;
    rsi_ReadNeighborTableResp ReadNeighborTableResp;
    rsi_ReadRouteTableResp    ReadRouteTableResp;
    rsi_GetAddrMapResp       GetAddrMapResp;
    rsi_GetKeytable          GetKeytableResp;
    rsi_GetRouteRecordforDev GetRouteRecordResp;
    rsi_zbrssivalue          zb_rssi_value;
    uint8_t					        uRspData[RSI_ZIGB_MAX_PAYLOAD_SIZE - RSI_ZIGB_FRAME_DESC_LEN];
  }uCmdRspPayLoad;
} rsi_zigb_uCmdRsp;

typedef struct rsi_zigb_cb_s
{
      rsi_zigb_app_scan_complete_handler_t                  zigb_app_scan_complete_handler;
      rsi_zigb_app_energy_scan_result_handler_t             zigb_app_energy_scan_result_handler;
      rsi_zigb_app_network_found_handler_t                  zigb_app_network_found_handler;
      rsi_zigb_app_stack_status_handler_t                   zigb_app_stack_status_handler;
      rsi_zigb_app_incoming_many_to_one_route_req_handler_t zigb_app_incoming_many_to_one_route_req_handler;
      rsi_zigb_app_handle_data_indication_t                 zigb_app_handle_data_indication;
      rsi_zigb_app_handle_data_confirmation_t               zigb_app_handle_data_confirmation;
      rsi_zigb_app_child_join_handler_t                     zigb_app_child_join_handler;
      rsi_zigb_nvm_backup_handler_t                         zigb_nvm_backup_handler;
      rsi_zigb_zdp_app_response_handler_t                   zigb_zdp_app_response_handler;

#ifdef ZB_PROFILE
      rsi_zigb_app_interpan_data_indication_t               zigb_app_interpan_data_indication;
      rsi_zigb_app_interpan_data_confirmation_t             zigb_app_interpan_data_confirmation;
#endif
}rsi_zigb_callback_t;

#ifdef ZB_MAC_API
void rsi_zigb_register_mac_callbacks (
    rsi_zigb_mac_data_confirm_t                zigb_mac_data_confirm,
    rsi_zigb_mac_data_indication_t             zigb_mac_data_indication,
    rsi_zigb_mac_assoc_confirm_t               zigb_mac_assoc_confirm,
    rsi_zigb_mac_assoc_indication_t            zigb_mac_assoc_indication,
    rsi_zigb_mac_disassoc_confirm_t            zigb_mac_disassoc_confirm,
    rsi_zigb_mac_disassoc_indication_t         zigb_mac_disassoc_indication,
    rsi_zigb_mac_beacon_notify_indication_t    zigb_mac_beacon_notify_indication,
    rsi_zigb_mac_orphan_indication_t           zigb_mac_orphan_indication,
    rsi_zigb_mac_rx_enable_confirm_t           zigb_mac_rx_enable_confirm,
    rsi_zigb_mac_scan_confirm_t                zigb_mac_scan_confirm,
    rsi_zigb_mac_comm_status_confirm_t         zigb_mac_comm_status_confirm,
    rsi_zigb_mac_start_confirm_t               zigb_mac_start_confirm,
    rsi_zigb_mac_poll_confirm_t                zigb_mac_poll_confirm
    );
#endif
typedef struct rsi_zigb_global_cb_s
{
	//! zb specific cb
	rsi_zigb_callback_t *zigb_specific_cb;
}rsi_zigb_global_cb_t;

#ifdef ZB_MAC_API
typedef struct rsi_zigb_mac_cb_s
{
  rsi_zigb_mac_data_confirm_t                zigb_mac_data_confirm;
  rsi_zigb_mac_data_indication_t             zigb_mac_data_indication;
  rsi_zigb_mac_assoc_confirm_t               zigb_mac_assoc_confirm;
  rsi_zigb_mac_assoc_indication_t            zigb_mac_assoc_indication;
  rsi_zigb_mac_disassoc_confirm_t            zigb_mac_disassoc_confirm;
  rsi_zigb_mac_disassoc_indication_t         zigb_mac_disassoc_indication;
  rsi_zigb_mac_beacon_notify_indication_t    zigb_mac_beacon_notify_indication;
  rsi_zigb_mac_orphan_indication_t           zigb_mac_orphan_indication;
  rsi_zigb_mac_rx_enable_confirm_t           zigb_mac_rx_enable_confirm;
  rsi_zigb_mac_scan_confirm_t                zigb_mac_scan_confirm;
  rsi_zigb_mac_comm_status_confirm_t         zigb_mac_comm_status_confirm;
  rsi_zigb_mac_start_confirm_t               zigb_mac_start_confirm;
  rsi_zigb_mac_poll_confirm_t                zigb_mac_poll_confirm;
}rsi_zigb_mac_callback_t;

typedef struct rsi_zigb_global_mac_cb_s
{
	//! zb specific cb
	rsi_zigb_mac_callback_t *zigb_mac_cb;
}rsi_zigb_global_mac_cb_t;

#endif
/* ZigBee Application control block */
typedef struct
{
  //! driver zigb control block mutex
  rsi_mutex_handle_t zigb_mutex;

  //! driver zigb control block expected command response
  rsi_zigb_cmd_request_t expected_response;

  //! driver zigb control block semaphore
  rsi_semaphore_handle_t zigb_sem;

  //! driver zigb control block tx packet pool
  rsi_pkt_pool_t  zigb_tx_pool;

  //! driver zigb control block tx buffer
  rsi_zigb_uCmdRsp      resp;

#ifdef ZB_MAC_API
  rsi_zigb_global_mac_cb_t *zigb_global_mac_cb;
#endif
  rsi_zigb_global_cb_t *zigb_global_cb;


} rsi_zigb_cb_t;


typedef struct
{
  uint32_t channel;
  uint8_t  scan_status;
} scan_done_cb_t;

typedef struct
{
  /* data confirmation wait */
  uint8_t dataConfWait;
  /* Cmd response confirmation status */
  uint8_t matchDescRspStatus;
  uint8_t powerDescRspStatus;
  uint8_t nodeDescRspStatus;
  uint8_t bindRspStatus;
  uint8_t unbindRspStatus;
  uint8_t actepRspStatus;
  uint8_t ieeeaddrRspStatus;
  uint8_t simpledescRspStatus;
  uint8_t networkaddrRspStatus;
  uint8_t scanReqSent;
} rsi_zigb_app_status_var_t;

typedef struct
{
  /* Cmd response confirmation data */
  uint8_t matchDescResponse[30];
  uint8_t matchDescResp[30];
  uint8_t powerDescResp[30];
  uint8_t nodeDescResp[30];
  uint8_t bindResp[30];
  uint8_t unbindResp[30];
  uint8_t actepResp[30];
  uint8_t ieeeaddrResp[30];
  uint8_t simpledespResp[30];
  uint8_t networkaddrResp[30];
  uint8_t asdu_pkt[RSI_ZIGB_MAX_PAYLOAD_SIZE];
} rsi_zigb_app_resp_info_t;

typedef struct
{
  //! FSM state
  rsi_zigb_state_t          state;
  /* Stack Handler status */
  ZigBeeNWKStatusInfo        stack_status;
  /* Scan Complete Handler status */
  scan_done_cb_t             scan_done_cb;
  /* ZigBee Network Coordinator Info */
  ZigBeeNetworkDetails       nwkinfo;
  /* Data Confirmation Handling */
  APSDE_Data_Confirmation_t  data_conf;
  /* ZigBee Variable Status */
  rsi_zigb_app_status_var_t  status_var;
  /* ZigBee Response Info */
  rsi_zigb_app_resp_info_t   zb_resp_info;
  /* ZigBee Simple discriptor */
  Simple_Descriptor_t        *DeviceSimpleDesc;
  /* response wait variable */
  uint8_t                    wait_for_rsp;
  /* Basic, Identify and ON_OFF switch configuration
   * clusters on Server side. */
  cluster_id_t               *a_In_Cluster_List;
  /* ON_OFF cluster on Client side */
  cluster_id_t               *a_Out_Cluster_List;
  /* Channel Mask */
  uint32_t                    channel_mask;
  /* Scan Duration */
  uint8_t                     scan_duration;
  /* Network down*/
  uint8_t                     network_down;
  /* NVM storage*/
  nvm_storage_t               nvm_storage;
} rsi_zigb_app_info_t;



#define m_ONE_KB_c					                    0x400
#define g_NULL_c                       0 //( void* ) ( 0 )
#define g_NVM_SAS_TABLE_START_OFFSET_c          m_ONE_KB_c
#define g_NVM_TABLES_START_OFFSET_c             m_ONE_KB_c * 2
#define g_MAX_NEIGHBOR_TABLE_ENTRIES_c          0x08
#define g_MAX_ROUTE_TABLE_ENTRIES_c                          0x0A
#define g_MAX_NWK_KEY_c                                     0x02
#define g_NEIGHBOR_TABLE_SIZE_c \
           ( sizeof( NeighborTable_t ) * g_MAX_NEIGHBOR_TABLE_ENTRIES_c )
#define g_NVM_SAVE_STATUS_BYTE_SIZE_c              		0x02
#define g_NEIGHBOR_TABLE_OFFSET_c               (g_NVM_TABLES_START_OFFSET_c + g_NVM_SAVE_STATUS_BYTE_SIZE_c)
#define g_ROUTE_TABLE_OFFSET_c    \
  							(g_NEIGHBOR_TABLE_OFFSET_c + g_NEIGHBOR_TABLE_SIZE_c)
//#if( g_NWK_MESH_ROUTING_d == RSI_ENABLE )
#if (defined(ZIGBEE_ROUTER) || (ZIGBEE_COORDINATOR))
#define g_ROUTE_TABLE_SIZE_c  \
            ( sizeof( Route_Table_t ) * g_MAX_ROUTE_TABLE_ENTRIES_c )
#else
    #define g_ROUTE_TABLE_SIZE_c   ( g_NULL_c)
#endif    /* g_NWK_MESH_ROUTING_d*/


#define g_SECURITY_MATERIAL_DESCRIPTOR_OFFSET_c\
                                 ( g_ROUTE_TABLE_OFFSET_c + g_ROUTE_TABLE_SIZE_c)
#define g_SECURITY_MATERIAL_DESCRIPTOR_SIZE_c \
               ( sizeof( Security_Material_Descriptor_t ) * g_MAX_NWK_KEY_c )

#define g_PERSISTENT_NIB_OFFSET_c     ( g_SECURITY_MATERIAL_DESCRIPTOR_OFFSET_c + \
                                        g_SECURITY_MATERIAL_DESCRIPTOR_SIZE_c )

#define g_PERSISTENT_NIB_SIZE_c     (sizeof( Persistent_NIB_t )  )

#define g_ZDO_INFORMATION_BASE_OFFSET_c (g_PERSISTENT_NIB_OFFSET_c + \
                                        g_PERSISTENT_NIB_SIZE_c )

#define g_ZDO_INFORMATION_BASE_SIZE_c ((sizeof( ZDO_Information_Base_t )&1)?\
                                        (sizeof( ZDO_Information_Base_t ) + 1):sizeof( ZDO_Information_Base_t ))

#define g_ROUTE_RECORD_TABLE_OFFSET_c   ( g_ZDO_INFORMATION_BASE_OFFSET_c + \
                                        g_ZDO_INFORMATION_BASE_SIZE_c )


#define g_MAX_ROUTE_RECORD_TABLE_ENTRIES_c          0x06

//#if (( g_NWK_MANY_TO_ONE_ROUTING_d == RSI_ENABLE ) && (g_NWK_SOURCE_ROUTING_d == RSI_ENABLE))
#if (defined(ZIGBEE_ROUTER) || (ZIGBEE_COORDINATOR))

#define g_ROUTE_RECORD_TABLE_SIZE_c \
                (sizeof( Route_Record_Table_t ) * g_MAX_ROUTE_RECORD_TABLE_ENTRIES_c)
#else
    /* One Default Entry would be allocated*/
#define g_ROUTE_RECORD_TABLE_SIZE_c    sizeof( Route_Record_Table_t )
#endif     /* g_NWK_MANY_TO_ONE_ROUTING_d*/

#define g_LINK_KEY_COUNTER_OFFSET_c    ( g_ROUTE_RECORD_TABLE_OFFSET_c + \
                                        g_ROUTE_RECORD_TABLE_SIZE_c )

#define g_LINK_KEY_COUNTER_SIZE_c \
	  				 (sizeof(LinkKeyFrameCounter_t) * g_MAX_LINK_KEY_ENTRIES_c)

#define g_LINK_KEY_TABLE_OFFSET_c    \
  					( g_LINK_KEY_COUNTER_OFFSET_c + g_LINK_KEY_COUNTER_SIZE_c)

#define g_MAX_LINK_KEY_ENTRIES_c               0x0A
#define g_LINK_KEY_TABLE_SIZE_c \
					 (sizeof(LinkKeyDeviceKeyPair_t) * g_MAX_LINK_KEY_ENTRIES_c)
#define g_RAM_LINK_KEY_TBL_SIZE_c   (g_LINK_KEY_TABLE_SIZE_c)
#define g_BINDING_TABLE_OFFSET_c        \
  					(g_LINK_KEY_TABLE_OFFSET_c + g_RAM_LINK_KEY_TBL_SIZE_c)
#define g_MAX_BINDING_TABLE_ENTRIES_c          0x0A
#define g_BINDING_TABLE_SIZE_c \
	  		((sizeof( Source_Binding_Table_t ) * g_MAX_BINDING_TABLE_ENTRIES_c))

#define g_RAM_BIND_TBL_SIZE_c          ( g_BINDING_TABLE_SIZE_c)

#define g_GROUP_TABLE_OFFSET_c        \
							(g_BINDING_TABLE_OFFSET_c + g_RAM_BIND_TBL_SIZE_c)

#ifdef ZIGBEE_END_DEVICE
#define  g_MAX_GROUP_TABLE_ENTRIES_c           0x00
#else
#define  g_MAX_GROUP_TABLE_ENTRIES_c           0x0A
#endif

#define g_GROUP_TABLE_SIZE_c \
            ( sizeof( Group_Table_t ) * g_MAX_GROUP_TABLE_ENTRIES_c )
#define g_RAM_GROUP_TBL_SIZE_c      (g_GROUP_TABLE_SIZE_c)
#define g_ADDRESS_MAP_TABLE_OFFSET_c  \
  							(g_GROUP_TABLE_OFFSET_c + g_RAM_GROUP_TBL_SIZE_c)

#define g_MAX_ADDR_MAP_ENTRIES_c               0x0A
#define g_ADDRESS_MAP_TABLE_SIZE_c \
             (sizeof(APSME_Address_Map_Table_t ) * g_MAX_ADDR_MAP_ENTRIES_c)
#define g_RAM_ADDRMAP_TBL_SIZE_c    (g_ADDRESS_MAP_TABLE_SIZE_c)
#define g_APP_LINK_KEY_TABLE_OFFSET_c \
					(  g_ADDRESS_MAP_TABLE_OFFSET_c + g_RAM_ADDRMAP_TBL_SIZE_c)
//! Status defines
#define TRUE                         1
#define FALSE                        0
#define RSI_ZB_SUCCESS                0x00
#define	RSI_ZB_FAIL                   0xFF
#define	RSI_ZB_INVALID                0xFE
#define RSI_ZB_CRC_FAIL					      0x02
#define RSI_ZB_RCVD_UNKNOWN_COMMAND   0x03
#define RSI_ZB_ERROR_SEQUENCE_NUMBER  0x04
#define RSI_ZB_DIRECTION_ERROR        0x05
#define RSI_ZB_INTERFACE_ERROR        0x06
#define RSI_ZB_COMMAND_ERROR          0x06
#define RSI_ZB_PAYLOAD_SIZE_ERROR     0x07


/** @brief  Defines the value for Success */
#define g_SUCCESS_c                 0x00

/** @brief  Defines the value for Failure */
#define g_FAILURE_c                 0xFF



/*******************************************************************************
 Startup_Attribute_Set_Tag
 -------------------------
 a_extended_pan_id[8]
 - This field holds the extended PAN ID of the network, in which the
 device needs to be a member.

 channel_mask
 - This field contains the set of channels, the device needs to scan
 as part of the network join or formation procedure.

 startup_control
 - This field indicates how the device needs to respond or start
 depending on the startup_control value.

 a_trust_center_address[8]
 - This field contains the 64-bit extended address of the TC.

 a_network_key[16]
 - This field indicates the key that is used for communication in
 the network.

 use_insecure_join
 - This field is a flag which indicates that a device needs to use
 insecure join during startup.

 network_manager_address[8]
 -This field indicates the 64-bit extended address of the network
 manager.

 scan_attempts
 - This field defines the number of scan attempts to be done by the
 device.

 time_between_scans
 - This field defines the time gap between two successive scans.

 rejoin_interval
 - This field indicates the time interval after which a device tries
 to rejoin the network, if disconnected.

 max_rejoin_interval
 - This field indicates the upper time limit within which multiple
 rejoins are done, that are separated by rejoin_intervals.

 indirect_poll_rate
 - This field indicates the rate at which an end device polls for
 messages, from its parent.

 parent_retry_threshold
 -This field indicates the parent retry threshold.

 a_pan_id
 -This field indicates the PAN ID of the device.

 a_master_key[16]
 -This field indicates the master key the node need to use.

 a_preconfigured_link_key[16]
 -This field indicates the link key the node need to use.

 end_device_bind_timeout
 - This field indicates the end device bind timeout.
 *******************************************************************************/
typedef struct Startup_Attribute_set_Tag {
    uint8_t a_extended_pan_id[8];
    uint32_t channel_mask;
    uint8_t startup_control;
    uint8_t use_insecure_join;
    uint8_t scan_attempts;
    uint8_t parent_retry_threshold;
    uint8_t a_trust_center_address[8];
    uint8_t a_network_key[16];
    uint16_t time_between_scans;
    uint16_t rejoin_interval;
    uint16_t max_rejoin_interval;
    uint16_t indirect_poll_rate;
    uint16_t a_pan_id;
    uint16_t network_manager_address;
    uint8_t a_trustcenter_master_key[16];
    uint8_t a_preconfigured_link_key[16];
    uint8_t end_device_bind_timeout;
} Startup_Attribute_set_t;

#if 0

/*******************************************************************************
 ZDO_Configuration_Table_Tag
 ---------------------------
 config_permit_join_duration
 - This field indicates the duration for which the device permits
 other devices to join.

 config_NWK_secure_all_frames
 - TC uses this field to determine, whether the network layer
 security is applied to all frames in the network or not.

 config_formation_attempts
 - This field indicates the number of formation retrials, after
 formation failure.

 config_scan_duration
 - The field indicates the duration of active scan while performing
 startup, join or rejoin the network.

 config_join_attempts
 - This field indicates the number of times join is retried once
 the join fails.

 config_preconfigured_key
 - This field indicates whether a preconfigured key is already
 available in the device or not.

 a_config_trust_center_short_address
 - This field holds the short address of the TC.

 automatic_poll_allowed
 - This field indicates whether an end device does an auto poll or
 not.

 config_authentication_poll_rate
 - This field indicates the time out in milliseconds for a device to
 wait for authentication after joining the network.

 config_switch_key_time
 - This field indicates the time out in seconds for switching the
 key after receiving a Switch Key Request from the TC.

 config_aps_ack_poll_time_out
 -The field indicates the time out after which an end device polls
 its parent, to retrieve an APS acknowledgement.
 *******************************************************************************/
typedef struct ZDO_Configuration_Table_Tag {
    uint8_t config_permit_join_duration;
    uint8_t config_NWK_secure_all_frames;
    uint8_t config_formation_attempts;
    uint8_t config_scan_duration;
    uint8_t config_join_attempts;
    uint8_t config_preconfigured_key;
    uint16_t a_config_trust_center_short_address;
    uint8_t automatic_poll_allowed;
    uint8_t config_authentication_poll_rate;
    uint16_t config_switch_key_time;
    uint8_t config_security_level;
    uint8_t config_aps_ack_poll_time_out;
    uint8_t a_manufacturer_code[2];
} ZDO_Configuration_Table_t;
#endif
/* API Structure information*/
typedef struct {
  uint32_t                timeout;
}sleepReqFrameSnd;

typedef struct {
  uint8_t                 RadioChannel;
  uint8_t                 power;
  uint8_t                 ExtPanId[8];
}formNetworkFrameSnd;
typedef struct {
  uint8_t                 ps_en;
  uint8_t                 deep_sleep_wkp_period;
  uint8_t                 slp_mode;
}pwrModeFrameSnd;

typedef struct {
  uint8_t                 DeviceType;
  uint8_t                 RadioChannel;
  uint8_t                 power;
  uint8_t                 ExtPanId[8];
}joinNetworkFrameSnd;

typedef struct {
  uint8_t                 PermitDuration;
}permitJoinFrameSnd;

typedef struct {
  uint32_t                ChannelMask;
  uint8_t                 Secured;
}findNWKRejoinFrameSnd;//TODO

typedef struct {
  uint8_t                 Secured;
}rejoinNWKFrameSnd;

typedef struct {
  uint32_t                ChannelMask;
  uint8_t                 ScanType;
  uint8_t                 duration;
}initScanFrameSnd;//TODO



typedef struct {
  uint8_t                 ieee_Addr[8];
}isitSelfIEEEFrameSnd;

typedef struct {
  uint16_t                ManufacturerCode;
}setManufFrameSnd;

typedef struct {
  uint8_t                 PowerSources;
  uint8_t                 CurPowerLevel;
}setPowerDescFrameSnd;

typedef struct {
  uint16_t                MaxIncomingTxfrSize;
}setMaxIncomingTxfrFrameSnd;

typedef struct {
  uint16_t                MaxOutgoingTxfrSize;
}setMaxOutTxfrFrameSnd;

typedef struct {
  uint8_t                 Channel;
}setOperChanFrameSnd;

typedef struct {
  uint8_t                 Index;
}getEndPointIdFrameSnd;

typedef struct {
  uint8_t                 EndPointId;
}getSimpleDescFrameSnd;

typedef struct {
  uint8_t                 EndPointId;
  uint8_t                 ClusterType;
  uint8_t                 ClusterIndex;
}getEPClusterFrameSnd;

typedef struct {
  uint8_t                 ieee_Addr[8];
}getShortAddrForIeeeAddrFrameSnd;

typedef struct {
  uint16_t                 ShortAddr;
}getIeeeAddrForShortAddrFrameSnd;

typedef struct {
  uint8_t                 Index;
}readNeighborTableFrameSnd;

typedef struct {
  uint8_t                 Index;
}getRouteTableFrameSnd;

typedef struct {
  uint8_t                 ChildIndex;
}getChildShortAddrFrameSnd;

typedef struct {
  uint16_t                ShortAddr;
}getChildIndexForShortAddrFrameSnd;

typedef struct {
  uint8_t                 Index;
}getChildDetailsFrameSnd;

typedef struct {
  uint32_t                ScanChannels;
  uint16_t                DestAddr;
  uint16_t                ScanRetry;
  uint8_t                 ScanDuration;
}initEnergyScanFrameSnd;//TODO

typedef struct {
  uint8_t                 ShortAddr;
  uint8_t                 ActiveChannels;
}bcastNWKManagerFrameSnd;

typedef struct {
  uint8_t                 ieee_Addr[8];
  uint8_t                 RequestType;
  uint8_t                 StartIndex;
}getZDPNWKShortAddrFrameSnd;

typedef struct {
  uint16_t                ShortAddr;
  uint8_t                 RequestType;
  uint8_t                 StartIndex;
  uint8_t                 APSAckRequired;
}getZDPIEEEAddrFrameSnd;

typedef struct {
  uint16_t                ShortAddr;
  uint8_t                 APSAckRequired;
}activeEPOfShortAddrFrameSnd;

typedef struct {
  uint16_t                ShortAddr;
  uint8_t                 APSAckRequired;
}powerDescFrameSnd;

typedef struct {
  uint16_t                ShortAddr;
  uint8_t                 APSAckRequired;
}nodeDescFrameSnd;

typedef struct {
  uint16_t                ShortAddr;
  uint8_t                 EndPointId;
}getSimpleDescOfShortAddrFrameSnd;

typedef struct {
  uint16_t                ProfileId;
  uint16_t                ClusterId;
  uint8_t                 msgType;
  uint8_t                 ieee_Addr[8];
  uint8_t                 DestEndpoint;
  uint8_t                 SrcEndpoint;
  uint8_t                 AsduLength;
  uint8_t                 TxOptions;
  uint8_t                 Radius;
  uint8_t                 Data[120]; //AsduLength Data
}unicastDataFrameSnd;//TODO

typedef struct {
  uint16_t                ProfileId;
  uint16_t                ClusterId;
  uint16_t                group_addr;
  uint8_t                 DestEndpoint;
  uint8_t                 SrcEndpoint;
  uint8_t                 AsduLength;
  uint8_t                 TxOptions;
  uint8_t                 Radius;
  uint8_t                 Data[120]; //AsduLength Data
}groupDataFrameSnd;//TODO

typedef struct {
  uint16_t                ProfileId;
  uint16_t                ClusterId;
  uint8_t                 DestEndpoint;
  uint8_t                 SrcEndpoint;
  uint8_t                 AsduLength;
  uint8_t                 TxOptions;
  uint8_t                 Radius;
  uint8_t                 Data[120]; //AsduLength Data
}bcastDataFrameSnd;//TODO

typedef struct {
  uint8_t                 SrcIEEEAddr[8];
  uint8_t                 SrcEndpoint;
  uint16_t                ClusterId;
  uint8_t                 DestAddrMode;
  uint8_t                 DestIEEEAddr[8];
  uint8_t                 DestEndpoint;
}setBindEntryFrameSnd;

typedef struct {
  uint8_t                 BindIndex;
}delBindEntryFrameSnd;

typedef struct {
  uint8_t                 BindIndex;
}isBindEntryActiveFrameSnd;

typedef struct {
  uint16_t                SrcShortAddr;
  uint16_t                ClusterId;
  uint8_t                 SrcIEEEAddr[8];
  uint8_t                 SrcEndpoint;
  uint8_t                 DestAddrMode;
  Address                 DestAddress;
  uint8_t                 DestEndpoint;
  uint8_t                 APSAckRequired;
}bindReqFrameSnd;//TODO

typedef struct {
  uint8_t                 EndPointId;
  uint8_t                 APSAckRequired;
}endDevBindFrameSnd;

typedef struct {
  uint16_t                SrcShortAddr;
  uint16_t                ClusterId;
  uint8_t                 SrcIEEEAddr[8];
  uint8_t                 SrcEndpoint;
  uint8_t                 DestAddrMode;
  Address                 DestAddress;
  uint8_t                 DestEndpoint;
  uint8_t                 APSAckRequired;
}unbindReqFrameSnd;//TODO

typedef struct {
  uint16_t                ProfileId;
  uint16_t                DevId;
  uint8_t                 EndPointId;
  uint8_t                 DevVersion;
  uint8_t                 InClusterCnt;
  uint8_t                 OutClusterCnt;
  uint8_t                 ClusterInfo[40];
}setSimpleDescFrameSnd;//TODO

typedef struct {
  uint16_t                ShortAddr;
  uint16_t                ProfileId;
  uint16_t                DestAddress;
  uint8_t                 InClusterCnt;
  uint8_t                 OutClusterCnt;
  uint8_t                 APSAckRequired;
  uint8_t                 ClusterInfo[40];
}sendMatchDescFrameSnd;//TODO

typedef struct {
  uint8_t                 Index;
}getAddrMapTableFrameSnd;

typedef struct {
  uint8_t                 KeyType;
}getKeyFrameSnd;

typedef struct {
uint8_t                   index;
TrustCenterLinkKeyTable_t LinkKeyTable;

}setTCLinkKeySnd;


typedef struct {
  uint8_t                 ieee_Addr[8];
}haveLinkKeyFrameSnd;

typedef struct {
  uint8_t                 TrustCenterIEEEAddr[8];
  uint8_t                 PartnerIEEEAddr[8];
}reqLinkKeyFrameSnd;

typedef struct {
  uint8_t                 Index;
}getKeyTableFrameSnd;

typedef struct {
  uint8_t                 Index;
  uint8_t                 ieee_Addr[8];
  uint8_t                 LinkKey;
  uint8_t                 KeyData[16];
}setKeyTableFrameSnd;

typedef struct {
  uint8_t                 ieee_Addr[8];
  uint8_t                 LinkKey;
  uint8_t                 KeyData[16];
}addKeyTableFrameSnd;

typedef struct {
  uint8_t                 ieee_Addr[8];
  uint8_t                 LinkKey;
}findKeyTableFrameSnd;

typedef struct {
  uint8_t                 Index;
}eraseKeyTableFrameSnd;

typedef struct {
  uint8_t                 TcLinkKey[16];
}setTcMasterKeyFrameSnd;

typedef struct {
  uint8_t                 PcLinkKey[16];
}preConfigLinkKeyFrameSnd;

typedef struct {
  uint8_t                 ExPanId[16];
}setExtPanIdFrameSnd;

typedef struct {
  uint8_t                 ProfileId;
}setProfileFrameSnd;

typedef struct {
  uint8_t 		  handlerId;
}sethandlerFrameSnd;

typedef struct {
  uint8_t 		  profile_test;
}setprofiletestFrameSnd;

typedef struct{
  uint8_t mode;
  Address           address;
} __attribute__((__packed__)) setSearchApsmeAddr;


typedef struct {
  uint16_t                 ShortAddr;
}getRouteRecordForDeviceFrameSnd;


typedef struct {
  uint8_t                 attr_id;
  uint8_t                 attr_index;
  uint8_t                 attr_value[8];
}__attribute__((__packed__))setMacPibFrameSnd;

typedef struct {
  uint8_t                 attr_id;
  uint8_t                 attr_index;
}__attribute__((__packed__))getMacPibFrameSnd;

/****** PER STRUCTURES *****/
#define ZIGB_PER_TRANSMIT               0x4
#define ZIGB_PER_STATS 		            0x8
#define ZIGB_ANT_SEL 		            0x9
#define ZIGB_SET_CHAN					0xA

typedef struct rsi_zigb_per_params_s
{
	uint8_t  enable;
	uint8_t  power;
	uint16_t pkt_length;
	uint8_t  mode;
	uint8_t  channel;
	uint16_t packet_count;
	uint16_t delay;

}rsi_zigb_per_params_t;

typedef struct zb_per_stats_s
{
  //! no. of pkts that pass crc
  unsigned short crc_pass;
  //! no. of pkts failing crc chk
  unsigned short crc_fail;
  //! no. of pkt aborts
  unsigned short pkt_abort;
  //! no. of tx pkts
  unsigned short tx_pkts;
  //! cca idle time
  unsigned short cca_idle;
  //! no. of times RX start got stuck
  unsigned short rx_start_stuck_stk;
  //! no of times RX cca got stuck
  unsigned short rx_cca_stk;
  //! no. of time fifo occupancy counter increments
  unsigned short fifo_occupy;
  //! no. of times phy aborts got generated
  unsigned short phy_abort;
  //! no. of times RX packet timeout inter generated
  unsigned short rx_pkt_tout;
  //! RSSI value
  signed char rssi;
}rsi_zigb_stats_t;

typedef struct per_stats_info_s
{
  //! no. of pkts that pass crc
  unsigned short crc_pass;
  //! no. of pkts failing crc chk
  unsigned short crc_fail;
  //! RSSI value
  signed char rssi;
}per_stats_info_t;

/****** Global frame desc structures *****/

extern const    uint8_t   rsi_zigb_frameInitStack[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameResetStack[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSleepReq[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameWakeupReq[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameFormNWK[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameJoinNWK[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_framePermitJoin[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameLeaveNWK[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameFindNWKnRejoin[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameRejoinNWK[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameNWKRestore[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameInitScan[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameStopScan[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameNWKState[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameStackIsUp[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetSelfIEEE[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameIsItSelfIEEE[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetSelfShortAddr[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSetManufNodeDesc[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSetPwrDesc[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSetMaxIncmgSize[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSetMaxOutSize[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSetChan[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetOperChan[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetDevType[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetShortPanId[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetExtPanId[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetEP[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetSimpleDesc[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetEPCluster[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetShortAddrForIEEE[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetStackProfile[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetIEEEForShortAddr[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameReadNeighborTable[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetRouteTable[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameTreeDepth[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetNeighborTable[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetChildShortAddr[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetChildIndexForShortAddr[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetChildDetails[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_framePollForData[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameReadChildCnt[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameReadRouterChildCnt[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetParentShortAddr[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetParentIEEEAddr[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameInitEnergyScan[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameBcastNWKManagerReq[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetZDPNWKShortAddr[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetZDPIEEEAddr[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameZDPDevAnnounce[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameActiveEPreq[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameZDPPwrDesc[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameZDPNodeDesc[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSimpleDescReq[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameUcastData[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGroupData[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameBcastData[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetMaxAPSSize[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSetSimpleDesc[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSendMatchDesc[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSetTCMasterKey[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_framePreconfigureLinkKey[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSetExtPanId[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetBindingIndices[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSetBindEntry[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameDelBindEntry[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameIsBindEntryActive[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameClearBindTable[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameBindReq[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameEndDevBind[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameUnbind[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetAddrMapTable[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetKey[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameHaveLinkKey[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameReqLinkKey[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameGetKeyTable[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSetKeyTable[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameAddKeyTable[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameFindKeyTable[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameEraseKeyTable[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameUpdateSAS[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameUpdateZDO[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameDeInitStack[RSI_ZIGB_BYTES_2];
extern const    uint8_t   rsi_zigb_frameSetProfile[RSI_ZIGB_BYTES_2];
/* Get Route Record  */
extern const    uint8_t   rsi_zigb_frameGetRouteRecord[RSI_ZIGB_BYTES_2];

extern rsi_zigb_app_info_t rsi_zigb_app_info;

/* Function Declarations */
int16_t rsi_zigb_execute_cmd(uint8_t *desc, uint8_t *payload, uint16_t length);
void rsi_zigb_app_cb_handler(uint8_t cmd_id, uint8_t *buffer);
void rsi_zigb_app_rsp_msg_handler(uint8_t cmd_id, uint8_t length, uint8_t *buffer);
rsi_zigb_uCmdRsp* rsi_zigb_app_frame_process(uint8_t *buffer);

int16_t rsi_zigb_init_stack(void);
int16_t rsi_zigb_form_network ( uint8_t  RadioChannel, uint8_t power ,uint8_t * pExtendedPanId );
int16_t rsi_zigb_join_network(uint8_t DeviceType, uint8_t RadioChannel,uint8_t power ,uint8_t * pExtendedPanId);
int16_t rsi_zigb_permit_join(uint8_t PermitDuration);
int16_t rsi_zigb_leave_network(void);
int16_t rsi_zigb_find_network_and_perform_rejoin(BOOL Secured, uint32_t channelMask);
int16_t rsi_zigb_rejoin_network(BOOL Secured);
int16_t rsi_zigb_network_restore(void);
int16_t rsi_zigb_initiate_scan(uint8_t scanType, uint32_t channelMask,uint8_t duration);
int16_t rsi_zigb_stop_scan(void);
int16_t rsi_zigb_network_state(void);
int16_t rsi_zigb_stack_is_up(void);
//int16_t rsi_zigb_get_self_ieee_address(void);
int16_t rsi_zigb_get_self_ieee_address(uint8_t* ieee_addr);
int16_t rsi_zigb_is_it_selfieee_address(uint8_t *);
int16_t rsi_zigb_get_self_short_address(void);
int16_t rsi_zigb_set_manufacturer_code_for_node_desc(uint16_t);
int16_t rsi_zigb_set_power_descriptor(Node_Power_Descriptor_t *);
int16_t rsi_zigb_set_maxm_incoming_txfr_size(uint16_t);
int16_t rsi_zigb_set_maxm_outgoing_txfr_size(uint16_t);
int16_t rsi_zigb_set_operating_channel(uint8_t );
int16_t rsi_zigb_get_device_type(uint8_t *dev_type);
int16_t rsi_zigb_get_operating_channel(void);
int16_t rsi_zigb_get_short_pAN_id(void);
int16_t rsi_zigb_get_extended_pan_id(uint8_t * );
int16_t rsi_zigb_get_endpoint_id(uint8_t);
//int16_t rsi_zigb_get_simple_descriptor(uint8_t endpointId);
int16_t rsi_zigb_get_simple_descriptor(uint8_t endpointId,Simple_Descriptor_t *simple_desc);
int16_t rsi_zigb_set_simple_descriptor(uint8_t endpointId, Simple_Descriptor_t *SimpleDesc);
int16_t rsi_zigb_get_endpoint_cluster(uint8_t , uint8_t ,uint8_t);
int16_t rsi_zigb_get_short_addr_for_specified_ieee_addr(uint8_t *);
int16_t rsi_zigb_stack_profile(void);
//int16_t rsi_zigb_get_ieee_addr_for_specified_short_addr(uint16_t );
int16_t rsi_zigb_get_ieee_addr_for_specified_short_addr(uint16_t shortAddr,uint8_t* ieee_addr);
//int16_t rsi_zigb_read_neighbor_table_entry(uint8_t);
int16_t rsi_zigb_read_neighbor_table_entry(uint8_t Index,ZigBeeNeighborTableEntry_t *neigbor_table);
//int16_t rsi_zigb_get_route_table_entry(uint8_t);
int16_t rsi_zigb_get_route_table_entry(uint8_t Index,ZigBeeRoutingTableEntry_t *routing_table);
int16_t rsi_zigb_tree_depth(void);
int16_t rsi_zigb_get_neighbor_table_entry_count(void);
int16_t rsi_zigb_get_child_short_address_for_the_index(uint8_t);
int16_t rsi_zigb_get_child_index_for_specified_short_addr(uint16_t);
//int16_t rsi_zigb_get_child_details(uint8_t);
int16_t rsi_zigb_get_child_details(uint8_t Index,uint8_t *ieee_addr,uint8_t DeviceType);
int16_t rsi_zigb_end_device_poll_for_data( void );
int16_t rsi_zigb_read_count_of_child_devices(void);
int16_t rsi_zigb_read_count_of_router_child_devices(void);
int16_t rsi_zigb_read_max_child_router_devices_count(void);
int16_t rsi_zigb_get_parent_short_address(void);
//int16_t rsi_zigb_Get_parent_ieee_address(uint8_t *);
int16_t rsi_zigb_get_parent_ieee_address(uint8_t *ieee_addr);
int16_t rsi_zigb_initiate_energy_scan_request(uint16_t ,uint32_t , uint8_t , uint16_t );
int16_t rsi_zigb_broadcast_nwk_manager_request(uint16_t , uint32_t );
int16_t rsi_zigb_send_nwk_addr_request(uint8_t * , BOOL , uint8_t );
int16_t rsi_zigb_send_ieee_addr_request(uint16_t , BOOL ,uint8_t , BOOL );
int16_t rsi_zigb_send_device_announcement(void);
int16_t rsi_zigb_send_match_descriptors_request(uint16_t , uint16_t , uint8_t *, uint8_t ,uint8_t *, uint8_t , BOOL ,uint16_t );
int16_t rsi_zigb_active_endpoints_request(uint16_t, uint8_t);
int16_t rsi_zigb_send_power_descriptor_request(uint16_t, uint8_t);
int16_t rsi_zigb_send_node_descriptor_request(uint16_t, uint8_t);
int16_t rsi_zigb_simple_descriptor_request(uint16_t, uint8_t);
//int16_t rsi_zigb_get_address_map_table_entry(uint8_t );
APSME_Address_Map_Table_t* rsi_zigb_get_address_map_table_entry(uint8_t Index);
int16_t rsi_zigb_zdp_send_node_descriptor_request(uint16_t shortAddress, uint8_t APSAckRequired);
int16_t rsi_zigb_zdp_send_power_descriptor_request(uint16_t shortAddress, uint8_t APSAckRequired);
int16_t rsi_zigb_zdp_send_ieee_addr_request(uint16_t shortAddress, BOOL RequestType,uint8_t StartIndex, BOOL APSAckRequired);
int16_t rsi_zigb_send_unicast_data(ZigBee_Outgoing_Msg_Type ,Address , ZigBeeAPSDEDataRequest_t *);
int16_t rsi_zigb_send_group_data( GroupID GroupAddress, ZigBeeAPSDEDataRequest_t * pAPSDERequest);
int16_t rsi_zigb_send_broadcast_data(ZigBeeAPSDEDataRequest_t * pAPSDERequest);
int16_t rsi_zigb_get_max_aps_payload_length(void);
int16_t rsi_zigb_set_binding_entry(ZDP_Bind_Request_t * pSetBindingEntry);
int16_t rsi_zigb_delete_binding(uint8_t bindIndex);
int16_t rsi_zigb_is_binding_entry_active(uint8_t bindIndex);
int16_t rsi_zigb_clear_binding_table(void);
int16_t rsi_zigb_bind_request(uint16_t , uint8_t *, uint8_t , uint16_t , uint8_t ,Address , uint8_t , BOOL );
int16_t rsi_zigb_end_device_bind_request(uint8_t , BOOL );
int16_t rsi_zigb_unbind_request(uint16_t , uint8_t *, uint8_t , uint16_t , uint8_t ,Address , uint8_t , BOOL );
BOOL rsi_zigb_is_it_self_ieee_address(uint8_t *pIEEEAddress);
int16_t rsi_zigb_set_maxm_out_going_txfr_size(uint16_t MaxOutgoingTxfrSize);
int16_t rsi_zigb_get_short_panid(void);
//int16_t rsi_zigb_get_extended_panid(void);
int16_t rsi_zigb_get_extended_panid(uint8_t* extended_panid);
int16_t rsi_zigb_zdp_send_device_announcement(void);
int16_t rsi_zigb_zdp_send_nwk_addr_request(uint8_t * pIEEEAddrOfInterest, BOOL RequestType, uint8_t StartIndex);

/* Securty APIs */
int16_t rsi_zigb_get_key(Security_Key_Types keytype);
int16_t rsi_zigb_have_link_key(uint8_t *pRemoteDeviceIEEEAddr);
int16_t rsi_zigb_switch_network_key_handler( uint8_t sequenceNumber );
int16_t rsi_zigb_request_link_key(uint8_t* trustCentreIEEEAddr, uint8_t* partnerIEEEAddr);
int16_t rsi_zigb_get_key_table_entry (uint8_t index, ZigBeeKeyStructure_t *pResult);
int16_t rsi_zigb_set_key_table_entry( uint8_t index, uint8_t * pIEEEAddress, BOOL linkKey, uint8_t * pKeyData );
int16_t rsi_zigb_add_or_update_key_table_entry(uint8_t *pIEEEAddress, BOOL linkKey, uint8_t *pKeyData, uint8_t *);
int16_t rsi_zigb_find_key_table_entry(uint8_t * pIEEEAddress, BOOL linkKey);
int16_t rsi_zigb_erase_key_table_entry(uint8_t index);


/** Supportive functions **/
void rsi_zigb_copy_xt_panid(uint8_t* Panid, uint8_t *buffer);
uint8_t rsi_zigb_pkt_parser(uint8_t *buffer);
void rsi_zigb_xtract_xt_panid(uint8_t* buffer, uint8_t *Panid);
uint8_t rsi_zigb_mcmp(uint8_t *buf1,uint8_t *buf2,uint8_t len);
uint8_t rsi_zigb_form_return_packet_mgmt(uint8_t seq_number, uint8_t commandType, uint8_t payloadSize, uint8_t *payload);
RSI_ZB_STATUS rsi_zigb_send_packet(uint8_t *buffer, uint8_t length);
uint8_t* rsi_zigb_get_host_pkt_buffer(void);
void rsi_zigb_reset_host_pkt_info(uint8_t *);
uint8_t* rsi_zigb_get_empty_host_buffer(void);
void rsi_zigb_uint32to_buffer(uint32_t data, uint8_t* buffer);
void rsi_zigb_mcpy(uint8_t *src, uint8_t *dst, uint8_t size);
void rsi_zigb_memset(uint8_t *src, uint8_t data, uint8_t size);
uint16_t rsi_zigb_buffer2_uint16(uint8_t* buffer);
uint32_t rsi_zigb_buffer2_uint32(uint8_t* buffer);
void rsi_zigb_uint16to_buffer(uint16_t data, uint8_t* buffer);
int32_t rsi_zigb_framework_init(void);
uint8_t* rsi_zigb_form_pkt_header_sz(uint8_t seq_number, uint8_t interface, uint8_t command, uint8_t payload_length);
int16_t rsi_zigb_addcrc(uint8_t * buf);
int16_t rsi_zigb_reset_stack(void);
int16_t rsi_zigb_sleep_request(uint32_t timeout);
int16_t rsi_zigb_wake_up_request(void);

#ifdef ZB_ONLY
int16_t rsi_zigb_oper_mode(rsi_zigb_uOperMode *);
#endif

int16_t rsi_zigb_update_sas(Startup_Attribute_set_t *);
int16_t rsi_zigb_update_all_zdo_configuration(ZDO_Configuration_Table_t *zdo_cnf);
int16_t rsi_zigb_update_all_sas(Startup_Attribute_Set_t *Startup);
int16_t rsi_zigb_update_zdo_configuration(ZDO_Configuration_Table_t *);
int16_t rsi_zigb_set_extended_pan_Id(uint8_t *);

uint8_t rsi_zigb_app_send_data( uint8_t direction, uint8_t commandType, uint8_t destEP, uint16_t dest_short_address,
		uint8_t commandId, uint16_t cluster, uint8_t dataLength,uint8_t* payloadPointer );

//! Interface functions
int16_t rsi_frame_read(uint8_t *);
uint16_t rsi_zigb_bytes2R_to_uint16(uint8_t *dBuf);
void rsi_zigb_app_scan_complete_handler(uint32_t , uint8_t );
void rsi_zigb_app_energy_scan_result_handler(uint32_t ,uint8_t *);
void rsi_zigb_app_network_found_handler(ZigBeeNetworkDetails);
void rsi_zigb_app_stack_status_handler(ZigBeeNWKStatusInfo *);
void rsi_zigb_app_incoming_many_to_one_route_req_handler(uint16_t , uint8_t *,uint8_t );
void rsi_zigb_app_handle_data_confirmation (APSDE_Data_Confirmation_t *pDataConfirmation);
void rsi_zigb_app_handle_data_indication(APSDE_Data_Indication_t *pDataIndication);
void rsi_zigb_api_test_data_indication_handler(APSDE_Data_Indication_t *pDataIndication);
void rsi_zigb_app_child_join_handler(uint16_t short_address, char joining);
#ifdef ZB_PROFILE
void rsi_zigb_app_interpan_data_indication( uint8_t bufferid,APSDE_InterPan_Data_Indication_t *pApsdeDataInd );
void rsi_zigb_app_interpan_data_confirmation( uint8_t bufferid, APSDE_InterPan_Data_Confirmation_t *pInterPanDataConf);
#endif
void rsi_zigb_nvm_backup_handler(void *data, uint32_t offset);
void rsi_zigb_zdp_app_response_handler(uint8_t *buffer);
int16_t rsi_zigb_set_profile(uint8_t profile_id);
uint8_t rsi_zigb_aps_asdu_handler(uint8_t handlerid);
uint8_t rsi_zigb_enable_profile_test(uint8_t profile_test);
uint8_t rsi_zigb_stack_set_event(void);
uint8_t rsi_zigb_stack_clear_event(void);
void rsi_zigb_register_callbacks (
      rsi_zigb_app_scan_complete_handler_t                  zigb_app_scan_complete_handler,
      rsi_zigb_app_energy_scan_result_handler_t             zigb_app_energy_scan_result_handler,
      rsi_zigb_app_network_found_handler_t                  zigb_app_network_found_handler,
      rsi_zigb_app_stack_status_handler_t                   zigb_app_stack_status_handler,
      rsi_zigb_app_incoming_many_to_one_route_req_handler_t zigb_app_incoming_many_to_one_route_req_handler,
      rsi_zigb_app_handle_data_indication_t                 zigb_app_handle_data_indication,
      rsi_zigb_app_handle_data_confirmation_t               zigb_app_handle_data_confirmation,
      rsi_zigb_app_child_join_handler_t                     zigb_app_child_join_handler,
      rsi_zigb_nvm_backup_handler_t                         zigb_nvm_backup_handler,
      rsi_zigb_zdp_app_response_handler_t                   zigb_zdp_app_response_handler
#ifdef ZB_PROFILE
      ,rsi_zigb_app_interpan_data_indication_t               zigb_app_interpan_data_indication,
       rsi_zigb_app_interpan_data_confirmation_t             zigb_app_interpan_data_confirmation
#endif
    );

int8_t rsi_zigb_cb_init(rsi_zigb_cb_t *zigb_cb);
uint16_t rsi_zigb_global_cb_init(uint8_t *buffer);
int32_t rsi_uart_init(void);
int32_t rsi_driver_process_zigb_recv_cmd(rsi_pkt_t *pkt);
uint8_t rsi_zigb_get_payload_len(uint8_t id);
/* PER DEFINATIONS HERE */
int16_t rsi_zigb_per_transmit(uint8_t power, uint16_t packet_len, uint8_t mode, uint8_t channel, uint8_t pkt_count);
int16_t rsi_zigb_per_transmit_stop(void);
int16_t rsi_zigb_setchannel( uint8_t channel_no);
int16_t rsi_zigb_perstats(per_stats_info_t *per_stats);
void rsi_zb_tx_done(rsi_pkt_t *pkt);
/** End of Supportive functions **/

/** Commissioning Related apis **/
void rsi_update_nvm_params(uint16_t offset);
uint8_t rsi_dump_nvm_to_device();
int16_t rsi_zigb_dump_nvm(uint8_t *data,uint16_t offset,uint16_t len);
#endif
