/**
 * @file         rsi_driver.h
 * @version      0.1
 * @date         15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains core driver components details
 *
 *  @section Description   This file contains core driver components details
 *
 *
 */

#ifndef RSI_DRIVER_H
#define RSI_DRIVER_H

#include <rsi_data_types.h>
#include <rsi_error.h>
#include <rsi_wlan_defines.h>
#include <rsi_wlan_config.h>
#include <rsi_user.h>
#include <rsi_utils.h>
#include <rsi_os.h>
#include <rsi_events.h>
#include <rsi_scheduler.h>
#include <rsi_pkt_mgmt.h>
#include <rsi_queue.h>
#include <rsi_common_apis.h>
#include <rsi_common.h>
#include <rsi_wlan_apis.h>
#include <rsi_wlan.h>
#include <rsi_socket.h>
#include <rsi_timer.h>
#ifdef RSI_SPI_INTERFACE
#include <rsi_spi_intf.h>
#include <rsi_spi_cmd.h>
#endif
#ifdef RSI_UART_INTERFACE
#include <rsi_uart.h>
#endif
#include <rsi_hal.h>
#include <rsi_nwk.h>
#include <rsi_setregion_countryinfo.h>
#include <rsi_bootup_config.h>
#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE))
#include <rsi_bt_common.h>
#include <rsi_bt_apis.h>
#include <rsi_bt_config.h>
#include <rsi_ble_config.h>
#endif
#ifdef RSI_ZB_ENABLE
#include <rsi_zb_api.h>
#endif
#include <string.h>
#ifdef RSI_WAC_MFI_ENABLE
#include "rsi_iap.h"
#endif
//#include <stdlib.h>
/******************************************************
 * *                      Macros
 * ******************************************************/

//! string functions
#define rsi_strcpy(x, y)     strcpy((char *)x, (const char *)y)
#define rsi_strcat(x, y)     strcat((char *)x, (const char *)y)
#define rsi_strlen(x)        strlen((const char *)x)


//! Max packet length of WLAN tx packet
#define RSI_WLAN_CMD_LEN 1600


//! Max packet length of ZigBee tx packet
#define RSI_ZIGB_CMD_LEN 256

//! pool size of WLAN tx packets
#define RSI_WLAN_POOL_SIZE ((sizeof(rsi_pkt_pool_t)) + ((RSI_WLAN_CMD_LEN + sizeof(void *)) * RSI_WLAN_TX_POOL_PKT_COUNT))

//! pool size of WLAN tx packets
#define RSI_ZIGB_POOL_SIZE ((sizeof(rsi_pkt_pool_t)) + ((RSI_ZIGB_CMD_LEN + sizeof(void *)) * RSI_ZIGB_TX_POOL_PKT_COUNT))

//! Max packet length of common command responses
#ifdef RSI_PUF_ENABLE
#define RSI_COMMON_CMD_LEN 1600 //Changed for PUF
#else
#define RSI_COMMON_CMD_LEN 100
#endif

//! pool size of common command packets
#define RSI_COMMON_POOL_SIZE ((sizeof(rsi_pkt_pool_t)) + ((RSI_COMMON_CMD_LEN + sizeof(void *)) * RSI_COMMON_TX_POOL_PKT_COUNT))

//! Max packet length of rx packet
#define RSI_DRIVER_RX_PKT_LEN 1600

//! pool size of driver rx packets
#define RSI_DRIVER_RX_POOL_SIZE ((sizeof(rsi_pkt_pool_t)) + ((RSI_DRIVER_RX_PKT_LEN + sizeof(void *)) * RSI_DRIVER_RX_POOL_PKT_COUNT))

//! pool size of socket information
#define RSI_SOCKET_INFO_POOL_SIZE (sizeof(rsi_socket_info_t) * RSI_NUMBER_OF_SOCKETS )


//! Max packet length of BT COMMON tx packet
#define RSI_BT_COMMON_CMD_LEN  300 //TODO Fix it
#define RSI_BT_CLASSIC_CMD_LEN 300 //TODO Fix it
#define RSI_BLE_CMD_LEN        300 //TODO Fix it

#ifdef SAPIS_BT_STACK_ON_HOST
#define RSI_OPERMODE_WLAN_BTBLE_DUALMODE_STACK_BYPASS  (16 + 8 + 4 + 1)
#define RSI_OPERMODE_WLAN_BTBLE_DUALROLE_STACK_BYPASS  (234)
#endif
//! pool size of BT COMMON tx packets
#define RSI_BT_COMMON_POOL_SIZE ((sizeof(rsi_pkt_pool_t)) + ((RSI_BT_COMMON_CMD_LEN + sizeof(void *)) * RSI_BT_COMMON_TX_POOL_PKT_COUNT))
//! pool size of BT Classic tx packets
#define RSI_BT_CLASSIC_POOL_SIZE ((sizeof(rsi_pkt_pool_t)) + ((RSI_BT_CLASSIC_CMD_LEN + sizeof(void *)) * RSI_BT_CLASSIC_TX_POOL_PKT_COUNT))
//! pool size of BLE tx packets
#define RSI_BLE_POOL_SIZE ((sizeof(rsi_pkt_pool_t)) + ((RSI_BLE_CMD_LEN + sizeof(void *)) * RSI_BLE_TX_POOL_PKT_COUNT))



//! ZB protocol type
#define RSI_ZB_Q                           1

//! BT protocol type
#define RSI_BT_Q                           2
#ifdef SAPIS_BT_STACK_ON_HOST
#define RSI_BT_INT_MGMT_Q                  6
#define RSI_BT_HCI_Q                       7
#endif
//! WLAN mgmt queue type
#define RSI_WLAN_MGMT_Q                    4

//! WLAN data queue type
#define RSI_WLAN_DATA_Q                    5

//! frame descriptor length
#define RSI_FRAME_DESC_LEN                 16

//! status offset in frame descriptor
#define RSI_STATUS_OFFSET                  12

//! response type offset in frame descriptor
#define RSI_RESP_OFFSET                    2

#define RSI_TRUE                           1
#define RSI_FALSE                          0

#define RSI_WAIT_FOREVER                   0
#define RSI_WAITING_FOR_OK                 0
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

//! driver control block structure
typedef struct rsi_driver_cb_s
{
	//! rx packet pool
	rsi_pkt_pool_t  rx_pool;

	//! driver common control block structure pointer
	rsi_common_cb_t *common_cb;

	//! driver common control block TX queue
	rsi_queue_cb_t  common_tx_q;

	//! driver WLAN  control block structure pointer
	rsi_wlan_cb_t   *wlan_cb;

	//! driver Network control block structure pointer
	rsi_nwk_cb_t   *nwk_cb;

  //! driver WLAN control block TX queue
	rsi_queue_cb_t  wlan_tx_q;
#ifdef RSI_ZB_ENABLE
	//! driver BT/ZB control block structure pointer
	rsi_zigb_cb_t  *zigb_cb;

	//! driver BT/ZB control block TX queue
	rsi_queue_cb_t  zigb_tx_q;
#endif

#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE))
	rsi_bt_global_cb_t *bt_global_cb;
	rsi_bt_cb_t *bt_common_cb;
    //! Single Mutex for all BT common/Classic/LE commands
    rsi_mutex_handle_t bt_single_mutex;

    //! Single Tx queue for all BT common/Classic/LE commands
    rsi_queue_cb_t *bt_single_tx_q;
	//! driver BT common control block structure pointer
	rsi_queue_cb_t  bt_common_tx_q;
#endif
#ifdef RSI_BT_ENABLE
	rsi_bt_cb_t  *bt_classic_cb;
	//! driver BT Classic control block TX queue
	rsi_queue_cb_t  bt_classic_tx_q;
#endif
#ifdef RSI_BLE_ENABLE
	rsi_bt_cb_t  *ble_cb;
	//! driver BLE control block TX queue
	rsi_queue_cb_t  ble_tx_q;
#endif
#ifdef SAPIS_BT_STACK_ON_HOST
	rsi_bt_cb_t  *bt_ble_stack_cb;
	//! driver BT/BLE stack control block TX queue
	rsi_queue_cb_t  bt_ble_stack_tx_q;
#endif
	//! driver scheduler control block
	rsi_scheduler_cb_t scheduler_cb;

	//! driver event list structure
	rsi_event_cb_t     event_list[RSI_MAX_NUM_EVENTS];

	//! tx packet state in case of UART interface
#ifdef RSI_UART_INTERFACE
	uint8_t tx_pkt_state;
#endif
}rsi_driver_cb_t;

extern rsi_driver_cb_t *rsi_driver_cb;
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_send_ps_mode_to_module(uint8_t selected_ps_mode, uint8_t selected_ps_type);
int32_t rsi_driver_process_bt_zb_recv_cmd(rsi_pkt_t *pkt);
void rsi_clear_sockets(uint8_t sockID);
void rsi_interrupt_handler(void);

int32_t rsi_driver_process_recv_data(rsi_pkt_t *pkt);
int32_t rsi_driver_send_data(uint32_t sock_no, uint8_t* buffer, uint32_t length, struct rsi_sockaddr *destAddr);


#endif
