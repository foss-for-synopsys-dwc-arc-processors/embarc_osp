/**
 * @file     rsi_zigb_frames.h
 * @version  0.1
 * @date     2015-Aug-31
 *
 * Copyright(C) Redpine Signals 2012
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief
 * Definitions of all ZigBee Frame Descriptors
 *
 * @section Description
 * This file contains definition of all frame descriptors used in ZigBee.
 * These definition are used to construct frames.
 *
 *
 */

/**
 * Includes
 */
#include "rsi_driver.h"

/**
 * Global Variables
 */


/* ZigBee 16 BYTE descriptor is in the format specified below
 *
 * BYTE0[0:7],BYTE1[0:3] --  Those 12 bits together represents length
 *                           of the payload for the command frame
 *
 * BYTE1[4:6]            --  Indicates queue number 1 for ZigBee pkts
 * BYTE1[7]              --  Reserved
 *
 * BYTE2 to BYTE12       --  Reserved
 *
 * BYTE13                --  Direction
 *                           1 - Host to Device
 *                           2 - Device to Host
 *
 * BYTE14                --  Belongs to category of frame
 *                           (MGMT/SECURITY/DATA/BINDING/CALLBACK ... Interface type frames)
 *
 * BYTE15                --  Command/frame type
 */

/***************************
 *** Management Commands ***
 ***************************/

/* Initiate ZigBee stack */
const uint8_t rsi_zigb_frameInitStack[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESTACKINIT};

/* Reset ZB stack */
const uint8_t   rsi_zigb_frameResetStack[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESTACKRESET};

/* Sleep request frame   */
//const uint8_t   rsi_zigb_frameSleepReq[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESLEEPRQT};

/* Wakeup request frame   */
//const uint8_t   rsi_zigb_frameWakeupReq[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEWAKEUPRQT};

/* ZigBee Form network */
const uint8_t   rsi_zigb_frameFormNWK[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEFORMNETWORK};

/* ZigBee Join network frame   */
const uint8_t   rsi_zigb_frameJoinNWK[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEJOINNETWORK};

/* Permit Join frame   */
const uint8_t   rsi_zigb_framePermitJoin[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEPERMITJOIN};

/* Leave Network frame   */
const uint8_t   rsi_zigb_frameLeaveNWK[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEELEAVENETWORK};

/* Find network and perform rejoin   */
const uint8_t   rsi_zigb_frameFindNWKnRejoin[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEFINDNETWORKANDPERFORMREJOIN};

/* Rejoin Network frame   */
const uint8_t   rsi_zigb_frameRejoinNWK[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEREJOINNETWORK};

/* Network restore frame   */
const uint8_t   rsi_zigb_frameNWKRestore[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEENETWORKRESTORE};

/* Initiate scan request frame   */
const uint8_t   rsi_zigb_frameInitScan[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEINITIATESCAN};

/* Stop scan frame   */
const uint8_t   rsi_zigb_frameStopScan[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESTOPSCAN};

/* Get current Network state   */
const uint8_t   rsi_zigb_frameNWKState[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEENETWORKSTATE};

/* Check stack status, find if stack is up */
const uint8_t   rsi_zigb_frameStackIsUp[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESTACKISUP};

/* Get SELF IEE ADDR   */
const uint8_t   rsi_zigb_frameGetSelfIEEE[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETSELFIEEEADDRESS};

/* To know if it Is self IEEE addr   */
const uint8_t   rsi_zigb_frameIsItSelfIEEE[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEISITSELFIEEEADDRESS};

/* Get self short addr    */
const uint8_t   rsi_zigb_frameGetSelfShortAddr[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETSELFSHORTADDRESS};

/* Set Manufacturing code for node desc  */
const uint8_t   rsi_zigb_frameSetManufNodeDesc[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESETMANUFACTURERCODEFORNODEDESC};

/* Set power Desc   */
const uint8_t   rsi_zigb_frameSetPwrDesc[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESETPOWERDESCRIPTOR};

/* Set Max incoming xfr frame buffer   */
const uint8_t   rsi_zigb_frameSetMaxIncmgSize[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESETMAXMINCOMINGTXFRSIZE};

/* Set Max outgoing xfr frame buffer   */
const uint8_t   rsi_zigb_frameSetMaxOutSize[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESETMAXMOUTGOINGTXFRSIZE};

/* Set operating Channel   */
const uint8_t   rsi_zigb_frameSetChan[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESETOPERATINGCHANNEL};

/* Get operating Channel   */
const uint8_t   rsi_zigb_frameGetOperChan[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETOPERATINGCHANNEL};

/* Get Device Type   */
const uint8_t   rsi_zigb_frameGetDevType[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETDEVICETYPE};

/* Get short panid   */
const uint8_t   rsi_zigb_frameGetShortPanId[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETSHORTPANID};

/* Get extended panid   */
const uint8_t   rsi_zigb_frameGetExtPanId[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETEXTENDEDPANID};

/* Get Endpoint Id  from stack  */
const uint8_t   rsi_zigb_frameGetEP[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETENDPOINTID};

/* Get simple Desc   */
const uint8_t   rsi_zigb_frameGetSimpleDesc[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETSIMPLEDESCRIPTOR};

/* Get endpoint cluster   */
const uint8_t   rsi_zigb_frameGetEPCluster[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETENDPOINTCLUSTER};

/* Get short addr for the specified IEEE Addr   */
const uint8_t   rsi_zigb_frameGetShortAddrForIEEE[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETSHORTADDRFORSPECIFIEDIEEEADDR};

/* Get stack profile Id   */
const uint8_t   rsi_zigb_frameGetStackProfile[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESTACKPROFILE};

/* Get IEEE addr for specified short addr   */
const uint8_t   rsi_zigb_frameGetIEEEForShortAddr[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETIEEEADDRFORSPECIFIEDSHORTADDR};

/* Read Neighbor table Entry   */
const uint8_t   rsi_zigb_frameReadNeighborTable[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEREADNEIGHBORTABLEENTRY};

/* Get route table entry   */
const uint8_t   rsi_zigb_frameGetRouteTable[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETROUTETABLEENTRY};

/* Get Tree depth of the network where child joined   */
const uint8_t   rsi_zigb_frameTreeDepth[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEETREEDEPTH};

/* Get neighbor table  to know active neighbors  */
const uint8_t   rsi_zigb_frameGetNeighborTable[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETNEIGHBORTABLEENTRYCOUNT};

/* Get child short addr for the index   */
const uint8_t   rsi_zigb_frameGetChildShortAddr[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETCHILDSHORTADDRESSFORTHEINDEX};

/* Get child index for specified short addr   */
const uint8_t   rsi_zigb_frameGetChildIndexForShortAddr[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETCHILDINDEXFORSPECIFIEDSHORTADDR};

/* Get child details   */
const uint8_t   rsi_zigb_frameGetChildDetails[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETCHILDDETAILS};

/* End device poll for data   */
const uint8_t   rsi_zigb_framePollForData[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEENDDEVICEPOLLFORDATA};

/* Read cound of child devices   */
const uint8_t   rsi_zigb_frameReadChildCnt[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEREADCOUNTOFCHILDDEVICES};

/* Read cound of Router child devices   */
const uint8_t   rsi_zigb_frameReadRouterChildCnt[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEREADCOUNTOFROUTERCHILDDEVICES};

/* Get parent short address   */
const uint8_t   rsi_zigb_frameGetParentShortAddr[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETPARENTSHORTADDRESS};

/* Get parent IEEE address   */
const uint8_t   rsi_zigb_frameGetParentIEEEAddr[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETPARENTIEEEADDRESS};

/* Initiate Energy Scan  */
const uint8_t   rsi_zigb_frameInitEnergyScan[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEINITIATEENERGYSCANREQUEST};

/* Broadcast NWK manager request   */
const uint8_t   rsi_zigb_frameBcastNWKManagerReq[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEBROADCASTNWKMANAGERREQUEST};

/* ZDP network addres request   */
const uint8_t   rsi_zigb_frameGetZDPNWKShortAddr[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZDPSENDNWKADDRREQUEST};

/* ZDP send Ieee Addr   */
const uint8_t   rsi_zigb_frameGetZDPIEEEAddr[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZDPSENDIEEEADDRREQUEST};

/* ZDP Send Device announcement    */
const uint8_t   rsi_zigb_frameZDPDevAnnounce[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZDPSENDDEVICEANNOUNCEMENT};

/* Get Active endpoint request   */
const uint8_t   rsi_zigb_frameActiveEPreq[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEACTIVEENDPOINTSREQUEST};

/* ZDP send power desc request   */
const uint8_t   rsi_zigb_frameZDPPwrDesc[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZDPSENDPOWERDESCRIPTORREQUEST};

/* ZDP send node desc request   */
const uint8_t   rsi_zigb_frameZDPNodeDesc[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZDPSENDNODEDESCRIPTORREQUEST};

/* Get simple desc request   */
const uint8_t   rsi_zigb_frameSimpleDescReq[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESIMPLEDESCRIPTORREQUEST};

/* Send Unicast Data Req   */
const uint8_t   rsi_zigb_frameUcastData[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESENDUNICASTDATA};

/* Send Group Data Req   */
const uint8_t   rsi_zigb_frameGroupData[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESENDGROUPDATA};

/* Send broadcast Data Req   */
const uint8_t   rsi_zigb_frameBcastData[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESENDBROADCASTDATA};

/* Get max APS payload size   */
const uint8_t   rsi_zigb_frameGetMaxAPSSize[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETMAXAPSPAYLOADLENGTH};

/* Set simple desc   */
const uint8_t   rsi_zigb_frameSetSimpleDesc[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEESETSIMPLEDESCRIPTOR};

/* Send Match desc request  */
const uint8_t   rsi_zigb_frameSendMatchDesc[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZDPSENDMATCHDESCRIPTORSREQUEST};

/* Update SAS */
const uint8_t   rsi_zigb_frameUpdateSAS[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEUPDATESAS};

/* Set Preconfigured Link Key */
const uint8_t   rsi_zigb_frameUpdateZDO[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEUPDATEZDO};

/* Set Profile */
const uint8_t   rsi_zigb_frameSetProfile[RSI_ZIGB_BYTES_2] = {SECURITY_INTERFACE, ZIGBEESETPROFILE};

/* Get Route Record  */
const uint8_t   rsi_zigb_frameGetRouteRecord[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETROUTERECORDFORDEV};

/* Get APSME_REMOVE GROUP */
const uint8_t   rsi_zigb_frameApsmeRemGrp[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEAPSREMGRP};


const uint8_t   rsi_zigb_framepermode[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEPERMODE};

const uint8_t   rsi_zigb_framenvmupdate[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEENVMUPDATE};

/* Get RSSI value */
const uint8_t rsi_zigb_frameGetRssi[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETRSSI};
const uint8_t   rsi_zigb_framePwrMode[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEINITPS};
/**********************************
 *** Binding Interface Commands ***
 **********************************/

/* Get binding indices   */
const uint8_t   rsi_zigb_frameGetBindingIndices[RSI_ZIGB_BYTES_2] = {BINDING_INTERFACE, ZIGBEEGETBINDINGINDICES};

/* Set binding entry   */
const uint8_t   rsi_zigb_frameSetBindEntry[RSI_ZIGB_BYTES_2] = {BINDING_INTERFACE, ZIGBEESETBINDINGENTRY};

/* Delete binding Entry   */
const uint8_t   rsi_zigb_frameDelBindEntry[RSI_ZIGB_BYTES_2] = {BINDING_INTERFACE, ZIGBEEDELETEBINDING};

/* Check if binding entry is active   */
const uint8_t   rsi_zigb_frameIsBindEntryActive[RSI_ZIGB_BYTES_2] = {BINDING_INTERFACE, ZIGBEEISBINDINGENTRYACTIVE};

/* Clear Binding entry   */
const uint8_t   rsi_zigb_frameClearBindTable[RSI_ZIGB_BYTES_2] = {BINDING_INTERFACE, ZIGBEECLEARBINDINGTABLE};

/* Binding request   */
const uint8_t   rsi_zigb_frameBindReq[RSI_ZIGB_BYTES_2] = {BINDING_INTERFACE, ZIGBEEBINDREQUEST};

/* Enddevice Binding request   */
const uint8_t   rsi_zigb_frameEndDevBind[RSI_ZIGB_BYTES_2] = {BINDING_INTERFACE, ZIGBEEENDDEVICEBINDREQUEST};

/*  UnBinding request   */
const uint8_t   rsi_zigb_frameUnbind[RSI_ZIGB_BYTES_2] = {BINDING_INTERFACE, ZIGBEEUNBINDREQUEST};

/* Get Address Map table */
const uint8_t   rsi_zigb_frameGetAddrMapTable[RSI_ZIGB_BYTES_2] = {BINDING_INTERFACE, ZIGBEEGETADDRESSMAPTABLEENTRY};

/**********************************
 *** Security Interface Commands ***
 **********************************/

/* Get key */
const uint8_t   rsi_zigb_frameGetKey[RSI_ZIGB_BYTES_2] = {SECURITY_INTERFACE, ZIGBEEGETKEY};

/* Have link Key */
const uint8_t   rsi_zigb_frameHaveLinkKey[RSI_ZIGB_BYTES_2] = {SECURITY_INTERFACE, ZIGBEEHAVELINKKEY};

/* request Link Key */
const uint8_t   rsi_zigb_frameReqLinkKey[RSI_ZIGB_BYTES_2] = {SECURITY_INTERFACE, ZIGBEEREQUESTLINKKEY};

/* Get key table entry */
const uint8_t   rsi_zigb_frameGetKeyTable[RSI_ZIGB_BYTES_2] = {SECURITY_INTERFACE, ZIGBEEGETKEYTABLEENTRY};

/* Set key Table entry */
const uint8_t   rsi_zigb_frameSetKeyTable[RSI_ZIGB_BYTES_2] = {SECURITY_INTERFACE, ZIGBEESETKEYTABLEENTRY};

/* Add or Update key table */
const uint8_t   rsi_zigb_frameAddKeyTable[RSI_ZIGB_BYTES_2] = {SECURITY_INTERFACE, ZIGBEEADDORUPDATEKEYTABLEENTRY};

/* Find Key Table */
const uint8_t   rsi_zigb_frameFindKeyTable[RSI_ZIGB_BYTES_2] = {SECURITY_INTERFACE, ZIGBEEFINDKEYTABLEENTRY};

/* erase key table */
const uint8_t   rsi_zigb_frameEraseKeyTable[RSI_ZIGB_BYTES_2] = {SECURITY_INTERFACE, ZIGBEEERASEKEYTABLEENTRY};

/* De-Initialize ZigBee stack */
const uint8_t rsi_zigb_frameDeInitStack[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEDEINITSTACK};

/* Update All ZDO Params*/
const uint8_t   rsi_zigb_frameUpdateALLZDO[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEUPDATEALLZDO};

/* Update All SAS */
const uint8_t   rsi_zigb_frameUpdateALLSAS[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEUPDATEALLSAS};
