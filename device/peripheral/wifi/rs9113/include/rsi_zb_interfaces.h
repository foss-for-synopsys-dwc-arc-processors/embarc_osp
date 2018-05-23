/**
 *   @file     rsi_zigb_interfaces.h
 *   @version  1.0
 *   @date     2014-Aug-23
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
 *   This file contain various structures which are ZigBee specific
 *   interface commands
 *
 *   @section Improvements
 *
 */


/**
 * Includes
 * */


#ifndef RSI_ZIGB_INTERFACES_H
#define RSI_ZIGB_INTERFACES_H

#include "rsi_zb_types.h"
#ifdef ZB_PROFILE
#include "../zigbee/profiles/include/common_includes.h"
#include "../zigbee/profiles/include/stack_common.h"
#include "../zigbee/profiles/include/mac_interface.h"
#include "../zigbee/profiles/include/nlme_interface.h"
#include "../zigbee/profiles/include/zdp_interface.h"
#include "../zigbee/profiles/include/zdo_interface.h"
#include "../zigbee/profiles/include/apsde_interface.h"
#include "../zigbee/profiles/zcl/ZLL_Utilities/inc/ZLL_NVM_Utilities.h"
#endif
#define g_EXTENDED_ADDRESS_LENGTH_c                   0x08
#define m_NWK_MAX_DEPTH_c                                   15
#define g_INVALID_INDEX_c                   0xFF

#ifndef ZB_PROFILE

 typedef struct Startup_Attribute_Set_Tag {
    uint8_t a_extended_pan_id[8];
    uint32_t channel_mask;
    uint8_t protocol_version;
    uint8_t stack_profile;
    uint8_t startup_control;
    uint8_t a_trust_center_address[8];
    uint8_t a_network_key[16];
    uint8_t use_insecure_join;
    uint8_t network_key_seq_num;
    uint8_t network_key_type;
    uint16_t network_manager_address;
    uint8_t scan_attempts;
    uint16_t time_between_scans;
    uint16_t rejoin_interval;
    uint16_t max_rejoin_interval;
    uint16_t indirect_poll_rate;
    uint8_t parent_retry_threshold;
    /*Following three attributes are no longer used.
     If we comment this, then code will not be compiled.
     For time being, this is not commented*/
    uint16_t security_timeout_period;
    uint16_t APS_ack_wait_duration;

    uint16_t a_short_address;
    uint16_t a_pan_id;
    uint8_t a_trustcenter_master_key[16];
    uint8_t a_preconfigured_link_key[16];
    /* Concentrator Parameters Attribute Set */
    uint8_t concentrator_flag;
    uint16_t concentrator_radius;
    uint16_t concentrator_discovery_time;

    uint8_t deviceAnnceReq;
    uint8_t end_device_bind_timeout;

}__attribute__((__packed__)) Startup_Attribute_Set_t;
typedef union Address_Tag {
  uint16_t short_address;
  uint8_t IEEE_address[8];
} Address;

typedef enum
{
  ZigBee_Outgoing_Direct,
  ZigBee_Via_Address_Map,
  ZigBee_Via_Binding_Table,
  ZigBee_Via_Multicast,
  ZigBee_Broadcast
}ZigBee_Outgoing_Msg_Type;

typedef struct {
  uint8_t          DestEndpoint;
  uint8_t          SrcEndpoint;
  ProfileID        ProfileId;
  ClusterID        ClusterId;
  uint8_t          AsduLength;
  uint8_t          TxOptions;
  uint8_t          Radius;
  uint8_t          aReserved[0x31];
  uint8_t          aPayload[0x33];
}ZigBeeAPSDEDataRequest_t;

typedef struct APSDE_Data_Indication_Tag {
  /* Destination address in the received message.*/
  Address           dest_address;
  uint8_t           dest_addr_mode;
  uint8_t           dest_endpoint;
  uint8_t           src_addr_mode;
  Address           src_address;
  uint8_t           src_endpoint;
  profile_id_t      profile_id;
  cluster_id_t      cluster_id;
  uint8_t           asdulength;
  uint8_t           was_broadcast;
  uint8_t           security_status;
  uint8_t           link_quality;
  uint8_t           a_asdu[128]; //! Place holder for data
}APSDE_Data_Indication_t;

typedef struct APSDE_Data_Confirmation_Tag {
  Address          dest_address;
  uint8_t          dest_addr_mode;
  uint8_t          dest_endpoint;
  uint8_t          src_endpoint;
  uint8_t          status;
}APSDE_Data_Confirmation_t;

typedef struct APSDE_InterPan_Data_Indication_Tag {
    uint8_t srcaddrmode;
    uint16_t srcpanid;
    Address srcaddress;
    uint8_t dstaddrmode;
    uint16_t dstpanid;
    Address dstAddress;
    profile_id_t  profileId;
    cluster_id_t  clusterId;
    uint8_t asdulength;
    uint8_t link_quality;
    uint8_t a_asdu[128];
} APSDE_InterPan_Data_Indication_t;


enum Tx_Options {
    g_APS_Tx_Opt_Unsecured_c = 0x00,
    g_APS_Tx_Opt_Ack_Req_c = 0x04,
    g_APS_Tx_Opt_Use_NWK_Key_c = 0x02,
    g_APS_Tx_Opt_Use_Link_c = 0x01,
    g_APS_Tx_Opt_Fragmentation_Permitted_c = 0x08,
    g_APS_Tx_Opt_Secured_Nwk_Key_Ack_c = 0x07
};

enum DEVICE_TYPE {

  g_COORDINATOR_c,
  g_ROUTER_c,
  g_END_DEVICE_c

};
typedef enum MAC_Scan_Types_Tag {

  /*! -ED Scan */
  g_MAC_ED_SCAN_TYPE_c = (uint8_t) 0x00,
  /*! -Active Scan */
  g_MAC_ACTIVE_SCAN_TYPE_c,
  /*! -Passive Scan */
  g_MAC_PASSIVE_SCAN_TYPE_c,
  /*! -Orphan Scan */
  g_MAC_ORPHAN_SCAN_TYPE_c

} MAC_Scan_Types_t;

typedef enum {
  ZigBeeCoordinator,
  ZigBeeRouter,
  ZigBeeEndDevice,
  ZigBeeActiveEndDevice,
  ZigBeeMobileEndDevice,
  ZigBeeUnknownDevice,
} ZigBeeDeviceType;

typedef struct{

  /* The 802.15.4 channel associated with the network.*/
  uint16_t          shortPanId; /* The network's PAN identifier.*/
  uint8_t           channel;
  uint8_t           extendedPanId[8]; /*The network's extended PAN identifier. */
  uint8_t           stackProfile; /* The Stack Profile associated with the network. */
  uint8_t           nwkUpdateId; /* The instance of the Network.*/
  BOOL              allowingJoining; /* Whether the network is allowing MAC associations.*/

}ZigBeeNetworkDetails; //TODO
typedef struct ZigBeeNeighborTableEntry_Tag {
  uint16_t         shortId;
  uint8_t          averageLqi;
  uint8_t          incomingCost;
  uint8_t          outgoingCost;
  uint8_t          age;
  uint8_t          aIEEEAddress[8];
}ZigBeeNeighborTableEntry_t;
typedef struct ZigBeeRoutingTableEntry_Tag {
  uint16_t         destAddr;
  uint16_t         nextHop;
  uint8_t          status;
  uint8_t          age;
  uint8_t          concentratorType;
  uint8_t          routeRecordState;
}ZigBeeRoutingTableEntry_t;
/*******************************************************************************
 Route_Record_Table_t

 This structure carries all the info required by the concentrator,
 to reach the destination with source routed frame.

 relayCount
 -Number of intermediate devices to reach the dest.

 relayList
 -List of device addresses.

 *******************************************************************************/
typedef struct Route_Record_Table_Tag {
    uint16_t dest_address;
    uint8_t relayIndex;
    uint8_t relayCount;
    uint16_t relayList[m_NWK_MAX_DEPTH_c];
}Route_Record_Table_t,rsi_GetRouteRecordforDev;


typedef enum {

  g_ZigBeeNotPartOfNWK_c,
  g_ZigBeeInTheProcessOfJoiningNWK_c,
  g_ZigBeeJoinedNWK_c,
  g_ZigBeeJoinedNWKNoParent_c,
  g_ZigBeePerformingLeaveFromNWK_c

} ZigBeeJoinStatus;
/*******************************************************************************
  node power descriptor specific to a device/node
  Field current_powermode_avail_power_sources - the first 4 bits of LSB gives the
  current sleep/ power saving mode of the node
  The MSB 4 bits gives the power sources available in this node
  Field current_powersource_currentpowersourcelevel - the first 4 bit of LSB gives
  the current power source
  The 4 bits of MSB gives the current power source level

  b3 b2 b1 b0 (bit3 bit2 bit1 bit0)

  0  0  0  0 - Receiver synchronized with the receiver on when idle
  subfield of the node descriptor
  0  0  0  1 - Receiver comes on periodically as defined by the node
  power descriptor
  0  0  1  0 - Receiver comes on when stimulated, e.g. by a user pressing
  a  button
  0  0  1  1
  to
  1  1  1  1 - Reserved
 *******************************************************************************/
typedef struct Node_Power_Descriptor_Tag {
  uint8_t          current_powermode_avail_power_sources;
  uint8_t          current_powersource_currentpowersourcelevel;
}Node_Power_Descriptor_t;


typedef struct ZDP_Bind_Request_Tag {
  uint8_t          a_src_addr[8];
  uint8_t          src_endpoint;
  uint8_t          a_cluster_id[2];
  uint8_t          dest_addr_mode;
  /*dest addr may be 2 bytes or 8 bytes depends on dst addr mode */
  uint8_t          a_dest_addr[8];
  /*dest endpoint may be 1 byte or zero bytes depends on dst addr mode */
  uint8_t          dest_endpoint;
}ZDP_Bind_Request_t;


typedef struct ZigBeeSimpleDescriptor_Tag {
  uint16_t         profileId;
  uint16_t         deviceId;
  uint8_t          deviceVersion;
  uint8_t          inputClusterCount;
  uint8_t          endpoint;
  uint8_t          outputClusterCount;
  uint16_t         *pInputClusterList;
  uint16_t         *pOutputClusterList;
}ZigBeeSimpleDescriptor_t; //TODO

typedef struct Endpoint_Description_Tag {
  ZigBeeSimpleDescriptor_t  *p_simple_desc;
  uint8_t                   endpoint_id;
}Endpoint_Description_t;

typedef enum Security_Key_Types_Tag {
    g_Trust_Center_Master_Key_c,
    g_Network_Key_c,
    g_Application_Master_Key_c,
    g_Link_Key_c,
    g_Trust_Center_Link_Key_c,
    g_Next_Network_Key_c
} Security_Key_Types;

/* Security Related Structures */
typedef enum ZigBeeKeyStructBitmask_Tag {

    /* This indicates that the key has a sequence number associated
     with it. (i.e. a Network Key).*/
    g_Key_Has_Sequence_Number_c = 0x01,

    /* This indicates that the key has an outgoing frame counter */
    g_Key_Has_Outgoing_Frame_Counter_c = 0x02,

    /* This indicates that the key has an incoming frame counter */
    g_Key_Has_Incoming_Frame_Counter_c = 0x04,

    /* This indicates that the key has an associated Partner IEEE address
     and the corresponding value within the ZigBeeKeyStructure_t has been
     populated with the data. */
    g_Key_Has_Partner_IEEE_Addr_c = 0x08,

    /* This indicates the key is authorized for use in APS data messages. If the
     key is not authorized for use in APS data messages it has not yet gone
     through a key agreement protocol, such as CBKE (i.e. ECC) */
    g_Key_Is_Authorized_c = 0x10

} ZigBeeKeyStructBitmask_t;

typedef struct ZigBeeKeyStructure_Tag {
    ZigBeeKeyStructBitmask_t bitmask;
    Security_Key_Types       type;
    uint8_t          key[16];
    uint32_t         outgoingFrameCounter;
    uint32_t         incomingFrameCounter;
    uint8_t          sequenceNumber;
    uint8_t          apartnerIEEEAddress[g_EXTENDED_ADDRESS_LENGTH_c];
}ZigBeeKeyStructure_t;

typedef enum {

  /* ZigBeeNWKIsUp  indicates of the NWK is formed or joined successfully */
  ZigBeeNWKIsUp,

  /* ZigBeeNWKIsDown indicates of the NWK formation failed or the device
   * left the network */
  ZigBeeNWKIsDown,

  /* ZigBeeJoinFailed indicates if the network join failed */
  ZigBeeJoinFailed,

  /* ZigBeeCannotJoinAsRouter indicates if the network was unable
   *  to start as Router */
  ZigBeeCannotJoinAsRouter,

  /* ZigBeeChangedNodeID indicates if the NodeID is changed after
   * resolving address conflict */
  ZigBeeChangedNodeID,

  /* ZigBeeChangedPANId indicates if the PANID is changed after resolving
   * PAN ID conflict */
  ZigBeeChangedPANId,

  /* ZigBeeChangedChannel indicates if the the channel is changed due to
   * frequency agility mechanism */
  ZigBeeChangedChannel,

  /* ZigBeeNoBeacons indicates no beacons during Discovery procedure*/
  ZigBeeNoBeacons,

  /* ZigBeeReceivedKeyInClear indicates the Network Key is received
   * in clear */
  ZigBeeReceivedKeyInClear,

  /* ZigBeeNoNWKKeyReceived indicates no Network key is received */
  ZigBeeNoNWKKeyReceived,

  /* ZigBeeNoLinkKeyReceived indicates no Link key is received */
  ZigBeeNoLinkKeyReceived,

  /* ZigBeePreconfiguredKeyRequired indicates Preconfigured
   * Link key is required */
  ZigBeePreconfiguredKeyRequired,

  /* ZigBeeChangedManagerAddress indicates nwk manager changed*/
  ZigBeeChangedManagerAddress

} ZigBeeNWKStatusInfo;


/*******************************************************************************
 Remove_Group_Request_Tag

 group_address
 - This field indicates the 16-bit group address from which an
 endpoint needs to be removed.

 endpoint
 - This field indicates the endpoint that needs to be removed from
 a group.

 *******************************************************************************/

typedef struct Remove_Group_Request_Tag {
    uint16_t group_address;
    uint8_t endpoint;
}Remove_Group_Request_t;

/*******************************************************************************
 Add_Group_Request_Tag :

 group_address
 - This field indicates the 16-bit group address to which an
 endpoint needs to be added.

 endpoint
 -This field indicates the endpoint that needs to be added to a
 group.

 *******************************************************************************/
//#pragma pack(1)
typedef struct Add_Group_Request_Tag {
    uint16_t group_address;
    uint8_t endpoint;
}Add_Group_Request_t;


typedef struct APSME_Address_Map_Table_Tag {
  uint8_t          a_IEEE_addr[8];
  uint16_t         nwk_addr;
}APSME_Address_Map_Table_t,rsi_GetAddrMapResp;

typedef struct Simple_Descriptor_Tag {
  profile_id_t     app_profile_id;
  uint16_t         app_device_id;
  uint8_t          app_device_version;
  uint8_t          incluster_count;
  cluster_id_t const *p_incluster_list;
  uint8_t          outcluster_count;
  cluster_id_t const *p_outcluster_list;
}Simple_Descriptor_t,rsi_GetSimpleDescResp;

typedef enum Addr_Mode_Tag {
  /*! -0x00 - No address mode specified */
  g_NO_ADDR_MODE_c,

  /*! -0x01 - Reserved */
  g_RESERVED_MODE_c,

  /*! -0x02 - Short address mode */
  g_SHORT_ADDR_MODE_c,

  /*! -0x03 - Long or extended address mode */
  g_EXTENDED_ADDR_MODE_c
} Addr_Mode_t;

//ZCL related defines.
typedef enum Frame_Control
{
  g_Generic_Cmd_c,
  g_Cluster_Specific_Cmd_c,
  g_Manufacture_Specific_Bit_c   = 0x04,
  g_Disable_Default_Response_c   = 0x10
}Frame_Control;


typedef struct Read_Attribute_Command_Request_tag{
  uint8_t            a_attrib_list[2];
}Read_Attribute_Command_Request_t;


typedef struct Write_Attribute_Command_Tag{
   uint8_t           a_attribute_id[2];
   uint8_t           attribute_data_type;
   uint8_t           a_attribute_data[1];
}Write_Attribute_Command_t;

typedef struct Discover_Attribute_Command_Tag
{
   uint8_t           a_start_attribute_identifier[2];
   uint8_t           maximum_count;
}Discover_Attribute_Command_t;


typedef struct Report_Attributes_Command_Tag
{
   uint8_t           a_attribute_identifier[2];
   uint8_t           attribute_data_type;
   uint8_t           attribute_data[1];
}Report_Attributes_Command_t;

typedef struct Attribute_reporting_Configuration_Record_Tag
{
	uint8_t            direction;
	uint8_t            a_attrbute_id[2]; /* in little endian order */
  uint8_t            data_type;
  uint8_t            a_minimum_reporting_interval[2];
  uint8_t            a_maximum_reporting_interval[2];
  uint8_t            a_reportable_change[2];
}Attribute_reporting_Configuration_Record;



typedef struct Configure_Report_Attributes_Command_Tag
{
  Attribute_reporting_Configuration_Record reporting_Configuration_Record[2];
}Configure_Report_Attributes_Command_t;

typedef struct Read_Reporting_Configurartion_Command_Tag
{
   uint8_t           irection;
   uint8_t           _attribute_id[2];
}Read_Reporting_Configurartion_Command_t;


typedef struct ZCL_Command_Tag
{
   uint8_t command_identifier;
   union Foundation_Commands
   {
      Read_Attribute_Command_Request_t   read_attribute_command;
      Write_Attribute_Command_t          write_attribute_command;
      Discover_Attribute_Command_t       discover_attribute_command;
      Report_Attributes_Command_t        report_attributes_command;
      Configure_Report_Attributes_Command_t configure_report_attributes_command;
      Read_Reporting_Configurartion_Command_t  a_attribute_records[1];
   }Foundation_Commands;
}ZCL_Command_t;


typedef struct ZCL_Header_And_Payload_Tag
{
   uint8_t           frame_control;
   uint8_t           a_manufacture_code[2];
   uint8_t           transaction_sequence_number;
   ZCL_Command_t ZCL_Command;
}ZCL_Header_And_Payload_t;






typedef struct App_ZCL_Request_Tag
{
   uint8_t           command_type;
   uint8_t           manufacture_specific;
   uint8_t           a_manufacturer_code[2];
   uint8_t           disable_default_response;
   ZCL_Command_t     ZCL_command_received;
}App_ZCL_Request_t;

typedef struct LinkKeyDeviceKeyPairTag{
    uint8_t deviceAddress[8];
    uint8_t linkKey[16];
}LinkKeyDeviceKeyPair_t;


typedef struct LinkKeyFrameCounterTag{
    uint8_t incomingFrameCounter[4];
    uint8_t outgoingFrameCounter[4];
}LinkKeyFrameCounter_t;


typedef struct TrustCenterLinkKeyTableTag {
   LinkKeyDeviceKeyPair_t LinkKeyDevicePair;
   LinkKeyFrameCounter_t  LinkKeyFrameCounter;
}TrustCenterLinkKeyTable_t;

//Commissioning
#define g_APSME_MAX_CLUSTERIDS_c               0x05
#define g_MAX_ENDPOINTS_IN_A_GROUP_c           0x04
typedef struct NeighborTable_Tag {
    uint8_t a_extended_address[8];
    uint16_t network_address;
    uint16_t nwk_pan_id;
    uint8_t logical_channel;
    uint8_t device_info;
    uint8_t transmit_failure;
    uint8_t LQI;
    uint8_t depth_permit_join;
    uint32_t incoming_frame_counter[2];
    uint8_t incoming_cost;
    uint8_t outgoing_cost;
    uint8_t ageing;
} __attribute__((__packed__)) NeighborTable_t;

typedef struct Route_Table_Tag {
    uint16_t dest_address;
    uint8_t status_info;
    uint8_t ageingIndex;
    uint16_t next_hop_address;
} __attribute__((__packed__)) Route_Table_t;
typedef struct Security_Material_Descriptor_Tag {
    uint8_t keysequencenumber;
    uint32_t outgoing_frame_counter;
    uint8_t key[16];
} __attribute__((__packed__)) Security_Material_Descriptor_t;
typedef struct Persistent_NIB_Tag {
    uint8_t NWK_security_level;
    uint16_t NWK_short_address;
    uint8_t NWK_active_key_seq_number;
    uint8_t NWK_all_fresh;
    uint8_t NWK_secure_all_frames;
    /* pan id is proprietary */
    uint16_t NWK_pan_id;
    uint8_t NWK_extended_panid[8];

    /* The following members are proprietary */
    uint8_t NWK_current_device_type;
    uint8_t NWK_number_of_active_children_joined;
    uint8_t NWK_number_of_routers_joined;
    uint8_t NWK_number_of_end_devices_joined;
    uint8_t NWK_current_device_depth;
    uint32_t NWK_passive_ack_neighbor_table_index;
    /*This attribute is required for ZigBee 2007*/
    uint8_t NWK_update_id;
    /*Network Manager Address is required for Zigbee 2007*/
    uint16_t NWK_manager_address;
    /*Device's parent address is stored here*/
    uint16_t NWK_parent_address;
    /*Nwk_Tx_Total is required for Zigbee 2007 Frequency Agility feature*/
    uint16_t NWK_tx_total;
    uint16_t NWK_tx_failure_count;
} __attribute__((__packed__)) Persistent_NIB_t;

typedef struct ZDO_Information_Base_Tag {
    uint8_t logical_type_complex_user_desc_avail;
    uint8_t a_server_mask[2];
    uint8_t a_extended_address[8];
    uint8_t user_descriptor[16];
    uint8_t no_ack_due_to_sync_loss;
#ifdef ZCP_15
    uint8_t field_count;
	uint8_t reserved;
    Language_Character_Set_t language_char;
    uint8_t manufacturer_name[16];
    uint8_t model_name[16];
    uint8_t serial_number[16];
    uint8_t device_url[17];
    uint8_t icon[16];
    uint8_t icon_url[17];
	uint8_t reserved2;
#endif // #ifdef ZCP_15
} __attribute__((__packed__)) ZDO_Information_Base_t;
typedef struct Source_Binding_Table_Tag {
    uint8_t src_endpoint;
    uint16_t a_cluster_id[g_APSME_MAX_CLUSTERIDS_c];
    uint16_t dest_addr;
    uint8_t dest_endpoint;
}__attribute__((__packed__)) Source_Binding_Table_t;
typedef struct Group_Table_Tag {
    uint16_t group_addr;
    uint8_t a_endpoint[g_MAX_ENDPOINTS_IN_A_GROUP_c];
}__attribute__((__packed__)) Group_Table_t;
typedef struct LinkKeyTableTag {
    uint8_t deviceaddress1[8];
    uint8_t deviceaddress2[8];
    uint8_t linkKey[16];
} __attribute__((__packed__))  LinkKeyTable_t;
typedef struct ZLL_NVM_Parameters_Tag{
#ifdef g_ZLL_LIGHT_CONTROLLER_c
    uint8_t controller_device_state;
#endif /* #ifdef g_ZLL_LIGHT_CONTROLLER_c*/
#ifdef g_ZLL_LIGHT_DEVICE_c
    uint8_t light_device_state;
#endif /* g_ZLL_LIGHT_DEVICE_c*/
    uint16_t network_address_begin;
    uint16_t network_address_end;
    uint16_t group_address_begin;
    uint16_t group_address_end;
}__attribute__((__packed__)) ZLL_NVM_Parameters_t;
typedef struct Node_Descriptor_Information_Tag {
    uint8_t aps_flags_freq_band;
    uint8_t MAC_capability_flags;
    uint8_t a_manufacturer_code[2];
    uint8_t max_buffer_size;
    uint8_t a_max_incoming_transfer_size[2];
    uint8_t a_max_outgoing_transfer_size[2];
    uint8_t descriptor_capability;
} __attribute__((__packed__)) Node_Descriptor_Information_t;
#if 1
typedef struct ZDO_Configuration_Table_T {
    uint8_t config_permit_join_duration;
    uint8_t config_NWK_secure_all_frames;
    uint8_t config_NWK_alt_protocol_version;
    uint8_t config_formation_attempts;
    uint8_t config_scan_duration;
    uint8_t config_join_attempts;
    uint8_t config_preconfigured_key;
    uint8_t config_no_of_devices_joined_before_NVM_save;
    uint16_t config_no_of_data_txns_before_NVM_save;
    uint16_t a_config_trust_center_short_address;
    uint8_t automatic_poll_allowed;
    uint8_t config_authentication_poll_rate;
    uint16_t config_switch_key_time;
    uint8_t config_security_level;
    uint8_t config_aps_ack_poll_time_out;
    Node_Descriptor_Information_t node_desc_info;
    uint8_t current_powermode_avail_power_sources;
    uint8_t current_powersource_currentpowersourcelevel;
    uint8_t a_reserved[98];
    //uint8_t a_manufacturer_code[2];
} __attribute__((__packed__)) ZDO_Configuration_Table_t;
#endif
#endif

//#ifdef ZB_PROFILE
#define m_SIZE_CONFIGURATION_c				            0x80
typedef struct nvm_storage_s {
  //1k is thr base address for SAS TABLE

  //SAS index:2 bytes
  //ZDO configuration
  uint16_t                        sas_index;// at g_NVM_SAS_TABLE_START_OFFSET_c @1K
  ZDO_Configuration_Table_t       gp_ZDO_Configuration[1];// at g_NVM_SAS_TABLE_START_OFFSET_c+2 @1k+2bytes

  //128 bytes are reserved,then follows the
  //Startup attributes based on SAS index Range:0-7
  uint8_t                         sas_data[7][m_SIZE_CONFIGURATION_c];
  //uint8_t                         sas_data[9][0x80];
  //2k is the base address for stack tables
  uint16_t                        nvm_status;
  NeighborTable_t                 Neighbor_Table[0x8];
//#if( g_NWK_MESH_ROUTING_d == RSI_ENABLE )
#if (defined(ZIGBEE_ROUTER) || (ZIGBEE_COORDINATOR))
  Route_Table_t                   Route_Table[0x6];
#endif
  Security_Material_Descriptor_t  Security_Material_Descriptor[2];
  Persistent_NIB_t                Persistent_NIB;
  ZDO_Information_Base_t          ZDO_Information_Base;
#if (defined(ZIGBEE_ROUTER) || (ZIGBEE_COORDINATOR))
  Route_Record_Table_t            route_record_table[6];
#else
  Route_Record_Table_t            route_record_table[1];
#endif
  LinkKeyFrameCounter_t           LinkKeyFrameCounterTable[10];
  LinkKeyDeviceKeyPair_t          LinkKeyDeviceKeyPairTable[10];
  Source_Binding_Table_t          src_bind_enrty[10];
#ifdef ZIGBEE_END_DEVICE
  Group_Table_t                   Group_Table;
#else
  Group_Table_t                   Group_Table[10];
#endif
  APSME_Address_Map_Table_t       Address_Map_Table[10];
//#if ( g_APS_LAYER_SEC_PROCESSING_d == 1 )
//#if (( g_TRUST_CENTRE_d == 1 ) && ( g_USE_HASH_KEY == 0 ) )
//#if ( g_APP_LINK_KEY_TABLE_NVM_STORAGE_d == RSI_ENABLE )
#ifdef ZIGBEE_COORDINATOR
  LinkKeyTable_t                  LinkKeyTable[10];
#endif
//#endif
//#endif

#if 1
  uint8_t                         cbke_info[100];
  ZLL_NVM_Parameters_t            ZLL_NVM_Parameters;
#endif
}__attribute__((__packed__)) nvm_storage_t;
//#endif

/*******************************************************************************
  APSDE_Data_Indication_Tag
  The data indication structure is received from APS layer(APS).
*******************************************************************************/











/*enumerations for txoptions
 g_APS_Tx_Opt_Unsecured_c
 - data transmission with out security.

 g_APS_Tx_Opt_Ack_Req_c
 - data transmission without security and with APS Ack.

 g_APS_Tx_Opt_Use_NWK_Key_c
 - data transmission with security and with NWK Key.

 g_APS_Tx_Opt_Fragmentation_Permitted_c
 - data transmission with Fragmentation.

 g_APS_Tx_Opt_Secured_Nwk_Key_Ack_c
 - data transmission with security using NWK Key and with APS Ack.
 */

/* End of Security Related Structures */
#define DEAFULT_RADIUS 0x5
#endif
