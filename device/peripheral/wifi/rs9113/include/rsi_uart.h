/**
 *  @file        rsi_uart.h
 *  @version     0.1
 *  @date        15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief API: Definitions of Various UART related parameters
 *
 *  @section Description
 *  This file contain the defines and UART api's .
 *
 */


#ifndef _RSI_UART_H
#define _RSI_UART_H
#include <rsi_data_types.h>
#include <rsi_wlan_config.h>
#include <rsi_pkt_mgmt.h>
#include <rsi_queue.h>
#ifndef WINDOWS
#include <pthread.h>
#endif
#include <string.h>

/******************************************************
 * *                      Macros
 * ******************************************************/
#ifndef WINDOWS
//! UART device port
#define RSI_UART_DEVICE "/dev/ttyUSB0"
#else
#define RSI_UART_DEVICE                    "\\\\.\\COM97"
#endif

//! UART Baud rate to be set
#define BAUDRATE  B115200

//! Pre descriptor length
#define RSI_PRE_DESC_LEN         4

//! Enable UART hardware flow control 0 - disable, 1- Enable
#define UART_HW_FLOW_CONTROL     0

//! Frame decriptor length
#define RSI_FRAME_DESC_LEN       16

//! Skip card ready if in UART mode
#define RSI_SKIP_CARD_READY      0

//! UART device or USB-CDC device
#define RSI_USB_CDC_DEVICE       1

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

//! RX Progrss States
typedef enum rsi_uart_rx_state_e{
  //! 0 - IDLE  state
  RSI_UART_IDLE_STATE = 0,

  //! Pre descriptor receive state
  RSI_UART_LEN_DESC_RECV_STATE,

  //! Host descriptor receive state
  RSI_UART_WAIT_FOR_HOST_DESC_STATE,

  //! Payload receive state
  RSI_UART_PAYLOAD_RECV_STATE
} rsi_uart_rx_state_t;



/******************************************************
 * *                 Type Definitions
 * ******************************************************/

//! host descriptor structure
typedef struct rsi_frame_desc_s {
  //! Data frame body length. Bits 14:12=queue, 000 for data, Bits 11:0 are the length
  uint8_t   frame_len_queue_no[2];
  //! Frame type
  uint8_t   frame_type;
  //! Unused , set to 0x00
  uint8_t   reserved[9];
  //! Management frame descriptor response status, 0x00=success, else error
  uint8_t   status;
  uint8_t   reserved1[3];
} rsi_frame_desc_t;

typedef struct rsi_linux_app_cb_s {

  //! RX receive state
  rsi_uart_rx_state_t   rx_is_in_progress;

  //! RX packet allocated
  rsi_pkt_t             *rcvPktPtr;

  //! Uart file descriptor
  int32_t               ttyfd;

  //! Pre descriptor read
  uint8_t               pre_desc_buf[RSI_PRE_DESC_LEN];

  //! byte count of received bytes
  uint16_t              byte_count;

  //! payload size of the packet
  uint16_t              payload_size;

  //! recv queue
  rsi_queue_cb_t        rcv_queue;
#ifndef WINDOWS
  //! mutex
  pthread_mutex_t      mutex1;
#endif
}rsi_linux_app_cb_t;



/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
extern rsi_linux_app_cb_t rsi_linux_app_cb;
extern int16_t rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame,uint8_t *payloadparam,uint16_t size_param);
extern int16_t rsi_frame_read(uint8_t *pkt_buffer);
extern int16_t rsi_uart_send(uint8_t *ptrBuf, uint16_t bufLen);
extern int16_t rsi_uart_recv(uint8_t *ptrBuf, uint16_t bufLen);
extern int16_t rsi_serial_init(void);
extern uint8_t rsi_uart_byte_read(void);
extern uint8_t rsi_serial_fifo_status(void);
extern int32_t rsi_uart_init(void);
extern void rsi_enter_critical_sec(void);
extern void rsi_exit_critical_sec(void);
extern void rsi_platform_based_init(void);
extern int32_t rsi_uart_deinit(void);
#endif

