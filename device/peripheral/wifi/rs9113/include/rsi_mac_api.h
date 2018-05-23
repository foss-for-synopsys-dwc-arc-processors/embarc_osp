/**
 *  @file      rsi_mac_api.h
 *   @version  1.0
 *   @date     2014-Dec-07
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

#ifndef MAC_API_H
#define MAC_API_H

#include "rsi_zb_types.h"



/** @brief  Defines the length of the 16 - bit Short Address */
#define g_SHORT_ADDRESS_LENGTH_c            0x02

/** @brief  Defines the length of the 16 - bit Short PAN ID */
#define g_SHORT_PAN_ID_LENGTH_c             0x02
/**********************************
 *** MAC Interface Commands ***
 **********************************/
/* ZigBee Enqueue to MAC MLME Q frame   */
const uint8_t   rsi_zigb_frameEnqueueMacMlme[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEENQUEUEMACMLME};

/* ZigBee Enqueue to MAC MCPS Q frame   */
const uint8_t   rsi_zigb_frameEnqueueMacMcps[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEENQUEUEMACMCPS};

/* ZigBee Set MAC PIB frame   */
const uint8_t   rsi_zigb_frameSetMACPib[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEMACPIBSET};

/* ZigBee Get MAC PIB frame   */
const uint8_t   rsi_zigb_frameGetMACPib[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEMACPIBGET};

/* ZigBee Get CCA status */
const uint8_t   rsi_zigb_frameGetCcaStatus[RSI_ZIGB_BYTES_2] = {MANAGEMENT_INTERFACE, ZIGBEEGETCCASTATUS};

/* MAC PIB Lookup Table*/
const uint8_t ga_PIB_Look_Up_Table_c[25][2] =
{
  {
    g_MAC_ASSOCIATION_PERMIT_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_AUTO_REQUEST_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_BEACON_PAYLOAD_LENGTH_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_BEACON_ORDER_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_BSN_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_COORD_EXTENDED_ADDDRESS_c,
    g_EXTENDED_ADDRESS_LENGTH_c
  },
  {
    g_MAC_COORD_SHORT_ADDRESS_c,
    g_SHORT_ADDRESS_LENGTH_c
  },
  {
    g_MAC_DSN_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_SUPERFRAME_ORDER_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_TRANSACTION_PERSISTENCE_TIME_c,
    sizeof(uint16_t)
  },
  {
    g_MAC_ASSOC_PAN_COORDINATOR_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_MAX_FRAME_TOTAL_WAIT_TIME_c,
    sizeof(uint16_t)
  },
  {
    g_MAC_RESPONSE_WAIT_TIME_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_MAX_CSMA_BACKOFFS_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_MIN_BE_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_SHORT_ADDRESS_c,
    g_SHORT_ADDRESS_LENGTH_c
  },
  {
    g_MAC_PAN_ID_c,
    g_SHORT_PAN_ID_LENGTH_c
  },
  {
    g_MAC_ACK_WAIT_DURATION_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_RX_ON_WHEN_IDLE_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_PAN_COORDINATOR_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_MAX_BE_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_MAX_FRAME_RETRIES_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_LIFS_PERIOD_c,
    sizeof(uint8_t)
  },
  {
    g_MAC_SIFS_PERIOD_c,
    sizeof(uint8_t)
  }
};


/**************************** Function Prototypes *********************/
int16_t rsi_zigb_send_to_mac_mlme(uint8_t messageIndex);
int16_t rsi_zigb_send_to_mac_mcps(uint8_t messageIndex);
uint8_t rsi_zigb_get_pib_len(uint8_t pib_id);
void memUtils_memCopy ( uint8_t* pDestination, uint8_t* pSource, uint8_t length );
int32_t rsi_zigb_send_cmd(uint8_t *cmd, rsi_pkt_t *pkt);
#endif
