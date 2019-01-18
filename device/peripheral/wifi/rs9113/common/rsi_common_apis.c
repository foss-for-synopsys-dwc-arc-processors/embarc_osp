/**
 * @file rsi_common_apis.c
 * @version 0.1
 * @date    15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief :This file contains common API's which needs to be called from application
 *
 *  @section Description  This file contains functions that process common protocol commands
 *  to be sent or recieved from the device
 *
 *
 */
#include "rsi_driver.h"
#include "rsi_bt_config.h"
#include "rsi_timer.h"
#ifdef RSI_BT_ENABLE
#include "rsi_bt.h"
#endif
#ifdef RSI_BLE_ENABLE
#include "rsi_ble.h"
#endif
/*
 * Global Variables
 * */
rsi_driver_cb_t *rsi_driver_cb = NULL;

uint16_t rsi_zigb_global_mac_cb_init(uint8_t *buffer);

/*==============================================*/
/**
 * @fn          int32_t rsi_common_get_status()
 * @brief       Return common block status
 * @param[in]   None,
 * @param[out]  None
 * @return      Common block status
 *
 *
 * @section description
 * This funcion returns common block status
 *
 *
 */

int32_t rsi_common_get_status()
{
  return rsi_driver_cb->common_cb->status;
}

/*==============================================*/
/**
 * @fn          void rsi_common_set_status()
 * @brief       Return common block status
 * @param[in]   None,
 * @param[out]  None
 * @return      Common block status
 *
 *
 * @section description
 * This funcion returns common block status
 *
 *
 */

void rsi_common_set_status(int32_t status)
{
  rsi_driver_cb->common_cb->status = status;
}

/*==============================================*/
/**
 *
 * @fn         rsi_driver_cb_t *rsi_driver_init(uint8_t *buffer, uint32_t length)
 * @brief      Initicalizes the driver components
 * @param[in]  buffer, application buffer pointer to initialize driver components
 * @param[in]  length, length of the memory provided by application
 * @param[out] None
 * @return
 *             >0 - If fails, returns memory requiored by driver
 *              0 - If success
 *             -1 - If SPI/UART intialization fails
 *             -2 - If no of sockets exceeded
 *
 * @section description
 * This fucntion intializes driver components
 *
 */
int32_t rsi_driver_init(uint8_t *buffer, uint32_t length)
{
  uint32_t        actual_length=0;
  //int32_t         status = RSI_SUCCESS;

  //! Memset user buffer
  memset(buffer, 0, length);

  //! check buffer is enough or not for driver components
  actual_length = sizeof(rsi_driver_cb_t) + RSI_DRIVER_RX_POOL_SIZE +
    sizeof(rsi_common_cb_t) + RSI_COMMON_POOL_SIZE +
    sizeof(rsi_wlan_cb_t) +  RSI_WLAN_POOL_SIZE    +
    sizeof(rsi_nwk_cb_t)
#ifdef RSI_ZB_ENABLE
    + sizeof(rsi_zigb_cb_t) + RSI_ZIGB_POOL_SIZE + sizeof(rsi_zigb_global_cb_t)
#endif
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE)
    + sizeof(rsi_bt_cb_t) + RSI_BT_COMMON_POOL_SIZE
    + sizeof(rsi_bt_global_cb_t)
#endif
#ifdef RSI_BT_ENABLE
    + sizeof(rsi_bt_cb_t) + RSI_BT_CLASSIC_POOL_SIZE + sizeof(rsi_bt_classic_cb_t)
#endif
#ifdef RSI_BLE_ENABLE
    + sizeof(rsi_bt_cb_t) + RSI_BLE_POOL_SIZE + sizeof(rsi_ble_cb_t)
#endif
    ;

  //! If length is not sufficient
  if(length < actual_length)
  {
    return actual_length;
  }

  //! Allocate memory for driver cb
  rsi_driver_cb = (rsi_driver_cb_t *)buffer;
  buffer += sizeof(rsi_driver_cb_t);

  //! Allocate memory for rx_pool
  rsi_pkt_pool_init(&rsi_driver_cb->rx_pool, buffer, RSI_DRIVER_RX_POOL_SIZE, RSI_DRIVER_RX_PKT_LEN);
  buffer += RSI_DRIVER_RX_POOL_SIZE;

  //! Allocate memory for common_cb
  rsi_driver_cb->common_cb =(rsi_common_cb_t *)buffer;
  buffer += sizeof(rsi_common_cb_t);

  //! Initialize common cb
  rsi_common_cb_init(rsi_driver_cb->common_cb);

  //! Allocate pool for common block
  rsi_pkt_pool_init(&rsi_driver_cb->common_cb->common_tx_pool, buffer, RSI_COMMON_POOL_SIZE, RSI_COMMON_CMD_LEN);
  buffer += RSI_COMMON_POOL_SIZE;

  //! Allocate memory for wlan block
  rsi_driver_cb->wlan_cb =(rsi_wlan_cb_t *)buffer;
  buffer += sizeof(rsi_wlan_cb_t);

  //! Allocate memory for network block
  rsi_driver_cb->nwk_cb =(rsi_nwk_cb_t *)buffer;
  buffer += sizeof(rsi_nwk_cb_t);

#ifdef RSI_WLAN_ENABLE
  //! Initialize wlan cb
  rsi_wlan_cb_init(rsi_driver_cb->wlan_cb);
#endif

  //! allocate pool for wlan_cb
  rsi_pkt_pool_init(&rsi_driver_cb->wlan_cb->wlan_tx_pool, buffer, RSI_WLAN_POOL_SIZE, RSI_WLAN_CMD_LEN);
  buffer += RSI_WLAN_POOL_SIZE;

  //! Initialize scheduler
  rsi_scheduler_init(&rsi_driver_cb->scheduler_cb);

  //! Initialize events
  rsi_events_init();

  rsi_queues_init(&rsi_driver_cb->wlan_tx_q);

  rsi_queues_init(&rsi_driver_cb->common_tx_q);

#ifdef RSI_ZB_ENABLE
  rsi_driver_cb->zigb_cb =(rsi_zigb_cb_t *)buffer;
  buffer += sizeof(rsi_zigb_cb_t);

  //! Initialize zigb cb
  rsi_zigb_cb_init(rsi_driver_cb->zigb_cb);

  //! allocate pool for zigb_cb
  rsi_pkt_pool_init(&rsi_driver_cb->zigb_cb->zigb_tx_pool, buffer, RSI_ZIGB_POOL_SIZE, RSI_ZIGB_CMD_LEN);
  buffer += RSI_ZIGB_POOL_SIZE;

#ifdef ZB_MAC_API
  rsi_driver_cb->zigb_cb->zigb_global_mac_cb = (rsi_zigb_global_mac_cb_t*)buffer;
  buffer += sizeof(rsi_zigb_global_mac_cb_t);
  //! Fill in zigb_global_cb
  buffer += rsi_zigb_global_mac_cb_init(buffer);
#else
  rsi_driver_cb->zigb_cb->zigb_global_cb = (rsi_zigb_global_cb_t*)buffer;
  buffer += sizeof(rsi_zigb_global_cb_t);

  //! Fill in zigb_global_cb
  buffer += rsi_zigb_global_cb_init(buffer);
#endif
  rsi_queues_init(&rsi_driver_cb->zigb_tx_q);
#ifdef ZB_DEBUG
  printf("\n ZIGB POOL INIT \n");
#endif
#endif

#if defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE)
  //! Allocate memory for bt_common_cb
  rsi_driver_cb->bt_common_cb = (rsi_bt_cb_t *)buffer;
  buffer += sizeof(rsi_bt_cb_t);

  //! Create bt mutex
  rsi_mutex_create(&rsi_driver_cb->bt_single_mutex);

  //! Initialize bt_common_cb
  rsi_bt_cb_init(rsi_driver_cb->bt_common_cb);

  //! Allocate pool for bt_common_cb
  rsi_pkt_pool_init(&rsi_driver_cb->bt_common_cb->bt_tx_pool, buffer, RSI_BT_COMMON_POOL_SIZE, RSI_BT_COMMON_CMD_LEN);
  buffer += RSI_BT_COMMON_POOL_SIZE;

  rsi_queues_init(&rsi_driver_cb->bt_common_tx_q);
#endif

#ifdef RSI_BT_ENABLE
  //! Allocate memory for bt_classic_cb
  rsi_driver_cb->bt_classic_cb = (rsi_bt_cb_t *)buffer;
  buffer += sizeof(rsi_bt_cb_t);

  //! Initialize bt_classic_cb
  rsi_bt_cb_init(rsi_driver_cb->bt_classic_cb);

  //! Allocate pool for bt_classic_cb
  rsi_pkt_pool_init(&rsi_driver_cb->bt_classic_cb->bt_tx_pool, buffer, RSI_BT_CLASSIC_POOL_SIZE, RSI_BT_CLASSIC_CMD_LEN);
  buffer += RSI_BT_CLASSIC_POOL_SIZE;

  rsi_queues_init(&rsi_driver_cb->bt_classic_tx_q);

#endif

#ifdef RSI_BLE_ENABLE
  //! Allocate memory for ble_cb
  rsi_driver_cb->ble_cb = (rsi_bt_cb_t *)buffer;
  buffer += sizeof(rsi_bt_cb_t);

  //! Initialize ble_cb
  rsi_bt_cb_init(rsi_driver_cb->ble_cb);

  //! Allocate pool for ble_cb
  rsi_pkt_pool_init(&rsi_driver_cb->ble_cb->bt_tx_pool, buffer, RSI_BLE_POOL_SIZE, RSI_BLE_CMD_LEN);
  buffer += RSI_BLE_POOL_SIZE;

  rsi_queues_init(&rsi_driver_cb->ble_tx_q);
#endif

#ifdef SAPIS_BT_STACK_ON_HOST
  //! Allocate memory for bt_classic_cb
  rsi_driver_cb->bt_ble_stack_cb = (rsi_bt_cb_t *)buffer;
  buffer += sizeof(rsi_bt_cb_t);

  //! Initialize bt_classic_cb
  rsi_bt_cb_init(rsi_driver_cb->bt_ble_stack_cb);

  //! Allocate pool for bt_classic_cb
  rsi_pkt_pool_init(&rsi_driver_cb->bt_ble_stack_cb->bt_tx_pool, buffer, RSI_BT_CLASSIC_POOL_SIZE, RSI_BT_CLASSIC_CMD_LEN);
  buffer += RSI_BT_CLASSIC_POOL_SIZE;

  rsi_queues_init(&rsi_driver_cb->bt_ble_stack_tx_q);

#endif

#if defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE)
  //! Allocate memory for bt_common_cb
  rsi_driver_cb->bt_global_cb = (rsi_bt_global_cb_t*)buffer;
  buffer += sizeof(rsi_bt_global_cb_t);

  //! Fill in bt_global_cb
  buffer += rsi_bt_global_cb_init(rsi_driver_cb, buffer);
#endif

// #ifdef RSI_WLAN_ENABLE
//   //! Memory for sockets
//   rsi_socket_pool = (rsi_socket_info_t *)buffer;
//   buffer += RSI_SOCKET_INFO_POOL_SIZE;

//   //! Check for max no of sockets
//   if(RSI_NUMBER_OF_SOCKETS > 10)
//   {
// 	status = -2;
// 	return status;
//   }
// #endif
#ifdef RSI_SPI_INTERFACE

  rsi_timer_start(RSI_TIMER_NODE_0, RSI_HAL_TIMER_TYPE_PERIODIC, RSI_HAL_TIMER_MODE_MILLI,1,rsi_timer_expiry_interrupt_handler);


  //! Configure power save GPIOs
  rsi_powersave_gpio_init();

#endif
#ifdef RSI_UART_INTERFACE
  //! UART initialisation
  status = rsi_uart_init();
  if(status != RSI_SUCCESS)
  {
    return -1;
  }
#endif

  return actual_length;
}

/*==============================================*/

/**
 * @fn         rsi_wireless_init(uint16_t opermode, uint16_t coex_mode)
 * @brief      Initialize WiSeConnect software
 * @param[in]  opermode , WLAN operating mode
 * @param[in]  coex_mode, co-ex mode
 * @return     int32_t
 *             0  = success
 *             <0 = failure
 *
 * @section description
 * This function is used to initialize WiSeConnect software module.
 * Application should call this function
 *
 */
int32_t rsi_wireless_init(uint16_t opermode,uint16_t coex_mode)
{
  rsi_pkt_t      *pkt;
  rsi_opermode_t *rsi_opermode;
  int32_t        status = RSI_SUCCESS;

  //! Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  common_cb->ps_coex_mode = coex_mode;
  common_cb->ps_coex_mode &= ~BIT(0);

  //! if state is not in card ready received state
  while(common_cb->state == RSI_COMMON_STATE_NONE)
  {
#ifndef RSI_WITH_OS
    //! Wait untill receives card ready
    rsi_scheduler(&rsi_driver_cb->scheduler_cb);
#endif
  }

  if(common_cb->state != RSI_COMMON_CARDREADY)
  {
    //! Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  else if(wlan_cb->auto_config_state == RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON)
  {
    while(1)
    {
      //! Run scheduler
      rsi_scheduler(&rsi_driver_cb->scheduler_cb);

      //! Check auto config state

      if((wlan_cb->auto_config_state == RSI_WLAN_STATE_AUTO_CONFIG_DONE) || (wlan_cb->auto_config_state == RSI_WLAN_STATE_AUTO_CONFIG_FAILED))
      {
        wlan_cb->auto_config_state = 0;
        break;
      }
#ifdef RSI_WLAN_ENABLE
      if((status = rsi_wlan_get_status()))
      {
        //! return error
        return status;
      }
#endif
    }

    return 0;
  }

  //! lock the mutex
  rsi_mutex_lock(&common_cb->common_mutex);

  //! allocate command buffer
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&common_cb->common_mutex);

    //! return packet allocation failure
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  //! memset the packet data
  memset(&pkt->data, 0, sizeof(rsi_opermode_t));

  //! Take the user provided data and fill it in opermode mode structure
  rsi_opermode = (rsi_opermode_t *)pkt->data;

  //! Save opermode command
  wlan_cb->opermode = opermode;

  //! Fill coex and opermode parameters
  rsi_uint32_to_4bytes(rsi_opermode->opermode, (coex_mode <<16 | opermode));

  //! send opermode command to driver
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_OPERMODE, pkt);

  //! command response status if success
  if(!status)
  {
    //! update the status
    status = rsi_common_get_status();
  }

  //! free the packet
  rsi_pkt_free(&common_cb->common_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&common_cb->common_mutex);
#if defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE)
  if(!status)
  {
    if ( (coex_mode == RSI_OPERMODE_WLAN_BLE) ||
        (coex_mode ==  RSI_OPERMODE_WLAN_BT_CLASSIC))
    {
      //WC waiting for BT Classic/ZB/BLE card ready
      rsi_bt_common_init();
    }
  }
#endif
  //! Return the status
  return status;
}

/*==============================================*/
/**
 * @fn           int32_t rsi_wireless_deinit();
 * @brief        Resets the module
 * @param[in]    none
 * @return       int32_t
 *               0  =  success
 *               <0 = failure
 *
 * @section description
 * This function resets the module
 */

int32_t rsi_wireless_deinit()
{
  rsi_pkt_t *pkt;
  int32_t   status = RSI_SUCCESS;

  //! Get common cb structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  //! take lock on common control block
  rsi_mutex_lock(&common_cb->common_mutex);

  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&common_cb->common_mutex);

    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  //! Magic number to set asyncronous reswponse
  common_cb->expected_response = RSI_COMMON_RSP_ASYNCHRONOUS ;

  //! send softreset command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_SOFT_RESET, pkt);

  //! free the packet
  rsi_pkt_free(&common_cb->common_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&common_cb->common_mutex);

  if(status != RSI_SUCCESS)
  {
  //! Return the status
   return status;
  }

//! Mask interrupts
  rsi_hal_intr_mask();

#ifdef RSI_SPI_INTERFACE
//! poll for interrupt status
  while(!rsi_hal_intr_pin_status());

  //! SPI interface initialization
  status = rsi_spi_iface_init();
  if(status != RSI_SUCCESS)
    {
    //! Return the status
     return status;
    }
#endif

  //! Unmask interrupts
  rsi_hal_intr_unmask();

#ifdef RSI_UART_INTERFACE
      rsi_uart_deinit();
#endif

  //! Wait on common semaphore
  rsi_semaphore_wait(&rsi_driver_cb->common_cb->common_sem, RSI_WAIT_FOREVER);

  //! get common status
  status = rsi_common_get_status();

  //! Return the status
  return status;

}

/*==============================================*/
/**
 * @fn             int32_t rsi_wireless_antenna(uint8_t type,uint8_t gain_2g, uint8_t gain_5g, uint8_t antenna_path, uint8_t antenna_type)
 * @brief          This function is used to select antenna on the device
 * @param[in]   ,  type : RSI_TYPE_RF_OUT_2, RSI_TYPE_RF_OUT_1
 * @param[in]   ,  gain_2g : 0 to 10
 * @param[in]   ,  gain_5g : 0 to 10
 * @param[in]   ,  antenna_path : RSI_ANTENNA_PATH_INTERNAL, RSI_ANTENNA_PATH_EXTERNAL
 * @param[in]   ,  antenna_type : RSI_ANTENNA_TYPE_REDPINE, RSI_ANTENNA_TYPE_FRACTUS, RSI_ANTENNA_TYPE_MOLEX
 * @param[out]     None
 * @return       int32_t
 *               0  = success
 *               <0 = failure
 *
 * @section description
 * This Function is used to select antenna type on the device
 *
 */
int32_t rsi_wireless_antenna(uint8_t type,uint8_t gain_2g, uint8_t gain_5g, uint8_t antenna_path, uint8_t antenna_type)
{

  rsi_pkt_t       *pkt;
  rsi_antenna_select_t *rsi_antenna;
  int8_t          status = RSI_SUCCESS;

  //! Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  //! lock the mutex
  rsi_mutex_lock(&common_cb->common_mutex);

  //! allocate command buffer
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&common_cb->common_mutex);

    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }


  //! Take the user provided data and fill it in antenna select structure
  rsi_antenna = (rsi_antenna_select_t *)pkt->data;

  //! antenna type
  rsi_antenna->antenna_value = type;

  //! antena gain in 2.4GHz
  rsi_antenna->gain_2g       = gain_2g;

  //! antenna gain in 5GHz
  rsi_antenna->gain_5g       = gain_5g;

  //! antenna path
  rsi_antenna->antenna_path  = antenna_path;

  //! antenna type
  rsi_antenna->antenna_type  = antenna_type;

  //! send  antenna select command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ANTENNA_SELECT, pkt);

  //! free the packet
  rsi_pkt_free(&common_cb->common_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&common_cb->common_mutex);

  //! Return the status
  return status;

}



/*==============================================*/
/**
 * @fn          void rsi_wireless_driver_task(void)
 * @brief       This function handles driver's events
 * @param[in]   None
 * @param[out]  None
 * @return      void
 *
 *
 * @section description
 * This API is used to handle driver's events for non-OS platforms
 *
 *
 */

void rsi_wireless_driver_task(void)
{
#ifdef RSI_WITH_OS
  while(1)
#endif
  {
    rsi_scheduler(&rsi_driver_cb->scheduler_cb);
  }
}


void rsi_wireless_driver_task_scheduler(void)
{
    rsi_scheduler(&rsi_driver_cb->scheduler_cb);
}
