/**
 * @file     rsi_uart.c
 * @version  0.1
 * @date     15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains UART HAL porting functionality
 *
 *  @section Description  This file contains UART HAL porting functionality
 *
 *
 */
#ifdef RSI_UART_INTERFACE
#ifdef WINDOWS
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include "rsi_data_types.h"
#include "rsi_error.h"
#include "rsi_utils.h"
#include "rsi_pkt_mgmt.h"
#include "rsi_os.h"
#include "rsi_queue.h"
#include "rsi_uart.h"
#include "rsi_events.h"

rsi_linux_app_cb_t rsi_linux_app_cb;

int fd;

#ifdef WINDOWS
CRITICAL_SECTION	crit_sec;
#endif

#ifdef WINDOWS
void system_error(char *name) {
  char *ptr = NULL;
  FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM,
      0,
      GetLastError(),
      0,
      (char *)&ptr,
      1024,
      NULL);
  fprintf(stderr, "\nError %s: %s\n", name, ptr);
  LocalFree(ptr);
}
#endif

/*==============================================*/
/**
 * @fn          void rsi_serial_init(void)
 * @brief       Serial device initialization
 * @param[out]  int16
 * @return      errCode
 *              0  = SUCCESS
 *              else failure
 * @section description
 * This is to initialize serial device for communicating
 * over UART/USB-serial.
 *
 *
 */

int16_t rsi_serial_init(void)
{
#ifndef WINDOWS
  struct termios newtio;
#endif
#ifdef WINDOWS
  COMMTIMEOUTS timeouts;
  DCB port;
  DWORD mode;
  char port_name[128] = RSI_UART_DEVICE;
  //! open the comm port.
  fd = CreateFile(port_name,
      GENERIC_READ | GENERIC_WRITE,
      0,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL);

  if ( INVALID_HANDLE_VALUE == fd) {
    system_error("opening file\n");
    return 1;
  }
  //! get the current DCB, and adjust a few bits to our liking.
  memset(&port, 0, sizeof(port));
  port.DCBlength = sizeof(port);
  if ( !GetCommState(fd, &port))
    system_error("getting comm state");
  if (!BuildCommDCB("baud=115200 parity=n data=8 stop=1", &port))
    system_error("building comm DCB");
  if (!SetCommState(fd, &port))
    system_error("adjusting port settings");

  timeouts.ReadIntervalTimeout = 50;
  timeouts.ReadTotalTimeoutMultiplier = 50;
  timeouts.ReadTotalTimeoutConstant = 50;
  timeouts.WriteTotalTimeoutMultiplier = 50;
  timeouts.WriteTotalTimeoutConstant = 50;
  if (!SetCommTimeouts(fd, &timeouts))
    system_error("setting port time-outs.");
  rsi_linux_app_cb.ttyfd = fd;
  return 0;
#else
  //! Open modem device for reading and writing and not as controlling tty
  //! because we don't want to get killed if linenoise sends CTRL-C.
  fd = open(RSI_UART_DEVICE, O_RDWR/* | O_NOCTTY */);

  if (fd <0) {
    perror(RSI_UART_DEVICE);
    exit(-1);
  }

  //! clear struct for new port settings
  bzero(&newtio, sizeof(newtio));

  //! BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
  //! CRTSCTS : output hardware flow control (only used if the cable has
  //!  all necessary lines. See sect. 7 of Serial-HOWTO)
  //!  CS8     : 8n1 (8bit,no parity,1 stopbit)
  //!  CLOCAL  : local connection, no modem contol
  //!  CREAD   : enable receiving characters
#if UART_HW_FLOW_CONTROL
  newtio.c_cflag = BAUDRATE  | CS8 | CLOCAL | CREAD| CRTSCTS;
#else
  newtio.c_cflag = BAUDRATE  | CS8 | CLOCAL | CREAD;
#endif

  //! IGNPAR  : ignore bytes with parity errors
  //! ICRNL   : map CR to NL (otherwise a CR input on the other computer
  //! will not terminate input)
  //! otherwise make device raw (no other input processing)

  newtio.c_iflag = IGNPAR;// | ICRNL;

  //! Raw output
  newtio.c_oflag = 0;

  //! ICANON  : enable canonical input
  //! disable all echo functionality, and don't send signals to calling program

  newtio.c_lflag = 0;

  //! initialize all control characters
  //! default values can be found in /usr/include/termios.h, and are given
  //! in the comments, but we don't need them here
  newtio.c_cc[VINTR]    = 0;    //! Ctrl-c
  newtio.c_cc[VQUIT]    = 0;    //! Ctrl-
  newtio.c_cc[VERASE]   = 0;    //! del
  newtio.c_cc[VKILL]    = 0;    //! @
  newtio.c_cc[VEOF]     = 4;    //! Ctrl-d
  newtio.c_cc[VTIME]    = 0;    //! inter-character timer unused
  newtio.c_cc[VMIN]     = 1;    //! blocking read until 1 character arrives
  newtio.c_cc[VSWTC]    = 0;    //! '\0'
  newtio.c_cc[VSTART]   = 0;    //! Ctrl-q
  newtio.c_cc[VSTOP]    = 0;    //! Ctrl-s
  newtio.c_cc[VSUSP]    = 0;    //! Ctrl-z
  newtio.c_cc[VEOL]     = 0;    //! '\0'
  newtio.c_cc[VREPRINT] = 0;    //! Ctrl-r
  newtio.c_cc[VDISCARD] = 0;    //! Ctrl-u
  newtio.c_cc[VWERASE]  = 0;    //! Ctrl-w
  newtio.c_cc[VLNEXT]   = 0;    //! Ctrl-v
  newtio.c_cc[VEOL2]    = 0;    //! '\0'

  //! now clean the modem line and activate the settings for the port
  tcflush(fd, TCIFLUSH);

  //! set the current attributes
  tcsetattr(fd,TCSANOW,&newtio);

  //! assign the file descriptor to global variable
  rsi_linux_app_cb.ttyfd = fd;
  return 0;

#endif
}



uint8_t rsi_serial_fifo_status()
{
  //! In Case of MCU  port return 1 if serial fifo is not empty
  //! return 0 in case of serial fifo is empty
  //! In Case of OS based code
  return 1;

}

/*==================================================================*/
/**
 * @fn         uint8_t rsi_uart_byte_read()
 * @param[in]  none
 * @param[out] None
 * @return     read character
 * @description
 * This API is used to read the byte data from module through the UART interface.
 *
 *
 */


uint8_t rsi_uart_byte_read()
{
  uint8_t ch = 0;
#ifdef WINDOWS
  DWORD read, Err;
  COMSTAT CST;
#endif
#ifdef WINDOWS
 while(1)
    {
      ClearCommError(rsi_linux_app_cb.ttyfd, (LPDWORD)&Err, &CST);
      if(CST.cbInQue != 0)break;
      Sleep(10);
    }
    ReadFile(rsi_linux_app_cb.ttyfd, &ch, 1, &read, NULL);
#else
  //! read each character
  read(rsi_linux_app_cb.ttyfd, &ch, 1);
#endif
  //! return the read character
  return ch;
}



/*==================================================================*/
/**
 * @fn         int16 rsi_uart_send(UINT8 *ptrBuf,uint16 bufLen,uint8_t *valBuf,uint8_t mode)
 * @param[in]  uint8_t *ptrBuf, pointer to the buffer with the data to be sent/received
 * @param[in]  uint16 bufLen, number of bytes to send
 * @param[out] None
 * @return     0, 0=success
 * @section description
 * This API is used to send data to the Wi-Fi module through the UART interface.
 *
 */


int16_t rsi_uart_send(uint8_t *ptrBuf, uint16_t bufLen)
{
  int16_t retval = 0;

#ifdef RSI_ENABLE_DEBUG_PRINT
  uint16_t ii = 0;

  printf("\n **TX PACKET SENT** \n");
  for(ii = 0; ii < bufLen; ii++)
  {
    if(ii && ((ii % 16 ) == 0) )
    {
      printf("\n");
    }
    printf(" 0x%.2x ",ptrBuf[ii]);
  }
  printf("\n ");
#endif
#ifdef WINDOWS
  DWORD written;
  retval = WriteFile(rsi_linux_app_cb.ttyfd, ptrBuf, bufLen, &written, NULL);
#else
  //! write function call to write on the interface
  retval = write(rsi_linux_app_cb.ttyfd, ptrBuf, bufLen);
  if (retval == bufLen)
  {
    retval = 0;
  }
#endif
  return retval;
}




/*==============================================*/
/**
 * @fn          void rsi_recv_pkt_serial(void)
 * @brief       Recieve the packet from MCU
 * @param[in]   none
 * @param[out]  none
 * @return      none
 * @section description
 * This function will read the number of bytes return by the
 * module
 *
 *
 */


void rsi_recv_pkt_serial()
{
  uint16_t dummy_byte_len=0;

#ifdef RSI_ENABLE_DEBUG_PRINT
  uint16_t payload_length = 0;
  uint16_t ii = 0;
#endif

#ifdef RSI_ENABLE_DEBUG_PRINT
  printf("\n Thread started \n");
#endif
  //! Check the UART FIFO status
  while (rsi_serial_fifo_status())
  {
    //! put the RX state in IDLE
    if (rsi_linux_app_cb.rx_is_in_progress ==  RSI_UART_IDLE_STATE) {

      //! Malloc the packet to receive RX packet
      rsi_linux_app_cb.rcvPktPtr = (rsi_pkt_t *) malloc( 1600 + sizeof(rsi_pkt_t));


      //! If packet is not allocated,return
      if(rsi_linux_app_cb.rcvPktPtr == NULL)
      {
#ifdef RSI_ENABLE_DEBUG_PRINT
        printf("\n ERROR : Packet Allocation failed \n");
#endif
        return;
      }

      //! Change the state to pre descriptor receive state
      rsi_linux_app_cb.rx_is_in_progress = RSI_UART_LEN_DESC_RECV_STATE;

      //! set the byte count to 0
      rsi_linux_app_cb.byte_count = 0;
    }


    //! Reading first four bytes to calculate packet size and offset
    if (rsi_linux_app_cb.rx_is_in_progress == RSI_UART_LEN_DESC_RECV_STATE)
    {
      //! get each byte of predescriptor
      rsi_linux_app_cb.pre_desc_buf[rsi_linux_app_cb.byte_count++] = rsi_uart_byte_read();

      //! If all the 4 bytes of pre descriptor is received, then change the state to host descriptor receive state
      if (rsi_linux_app_cb.byte_count == RSI_PRE_DESC_LEN)
      {
        rsi_linux_app_cb.rx_is_in_progress = RSI_UART_WAIT_FOR_HOST_DESC_STATE;

        //! calculate the dummy length using pre descriptor
        dummy_byte_len = rsi_bytes2R_to_uint16(&rsi_linux_app_cb.pre_desc_buf[2]) - RSI_PRE_DESC_LEN;
      }
      continue;
    }

    //! Reading dummy Bytes if any
    if (rsi_linux_app_cb.rx_is_in_progress == RSI_UART_WAIT_FOR_HOST_DESC_STATE)
    {
      if (dummy_byte_len)
      {
        //! Read dummy bytes
        rsi_uart_byte_read();
        dummy_byte_len--;
        continue;
      }

      //! handle zero lenght packets
      if (!dummy_byte_len)
      {
        //! Change the status to payload receive state
        rsi_linux_app_cb.rx_is_in_progress = RSI_UART_PAYLOAD_RECV_STATE;

        //! Calculate the payload length
        rsi_linux_app_cb.payload_size = rsi_bytes2R_to_uint16(&rsi_linux_app_cb.pre_desc_buf[0]) - rsi_bytes2R_to_uint16(&rsi_linux_app_cb.pre_desc_buf[2]);

#ifdef RSI_ENABLE_DEBUG_PRINT
        //! get the payload size from pre descriptor
        payload_length = rsi_linux_app_cb.payload_size;
#endif

        //! assign the byte count
        rsi_linux_app_cb.byte_count = 0;

        //! Handle zero lenght packets
        if (!rsi_linux_app_cb.payload_size)
        {
          //! If payload length received  is zero then don't collect the packet
          //! and free the buffer and revert the state machine to IDLE
          rsi_linux_app_cb.rx_is_in_progress = RSI_UART_IDLE_STATE;

          //! If zero length packet is received, then free the packet
          free(rsi_linux_app_cb.rcvPktPtr);

          //! Make the pointer zero
          rsi_linux_app_cb.rcvPktPtr = NULL;
          return;
        }
      }
    }

    //! If the packet is in payload receive state
    if (rsi_linux_app_cb.rx_is_in_progress == RSI_UART_PAYLOAD_RECV_STATE) {

      //! If payload is present
      if (rsi_linux_app_cb.payload_size) {

        //! Read the payload bytes
        rsi_linux_app_cb.rcvPktPtr->desc[rsi_linux_app_cb.byte_count++] = rsi_uart_byte_read();

        //! decrement the paylaod size by 1 after reading each byte
        rsi_linux_app_cb.payload_size--;

      }

      //! After reading payload
      if (rsi_linux_app_cb.payload_size == 0)
      {
        //! Make the state to IDLE
        rsi_linux_app_cb.rx_is_in_progress = RSI_UART_IDLE_STATE;


#ifdef RSI_ENABLE_DEBUG_PRINT
        printf("\n **RX PACKET RECEIVED** \n");

        for(ii = 0; ii < RSI_FRAME_DESC_LEN; ii++)
        {
          printf(" 0x%.2x ",rsi_linux_app_cb.rcvPktPtr->desc[ii]);

        }
        printf("\n");

        for(ii = 0; ii < payload_length - RSI_FRAME_DESC_LEN ; ii++)
        {
          if(ii && ((ii % 16 ) == 0))
          {
            printf("\n");
          }
          printf(" 0x%.2x ",rsi_linux_app_cb.rcvPktPtr->data[ii]);

        }
#endif
        //! enqueue packet
        rsi_enqueue_pkt(&rsi_linux_app_cb.rcv_queue, rsi_linux_app_cb.rcvPktPtr);

        //! Set the RECEIVE EVENT here. In case of microcontrolle
        //! set this event in ISR
        rsi_set_event(RSI_RX_EVENT);

        //! Make the packet NULL
        rsi_linux_app_cb.rcvPktPtr = NULL;
      }
    }
  }
}








/*==============================================*/
/**
 * @fn          void recvThread(void *args)
 * @brief       Recieve the packet from MCU
 * @param[in]   args
 * @return      none
 * @section description
 * This function is the thread which is created to read the
 * data coming from the module on UART interface
 *
 *
 */

void *recvThread(void* args)
{
  uint8_t card_ready_frame[16] = { 0x0, 0x40, 0x89, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

  //! If skip card ready is enabled
  if(RSI_SKIP_CARD_READY)
  {
    //! Allocate the packet for sending card ready frame
    rsi_linux_app_cb.rcvPktPtr = (rsi_pkt_t *)malloc(sizeof(rsi_pkt_t));

    //! copy card ready frame
    memcpy(rsi_linux_app_cb.rcvPktPtr->desc, card_ready_frame, RSI_FRAME_DESC_LEN);

    //! enqueue packet
    rsi_enqueue_pkt(&rsi_linux_app_cb.rcv_queue, rsi_linux_app_cb.rcvPktPtr);

    //! Set the RECEIVE EVENT here. In case of microcontrolle
    //! set this event in ISR
    rsi_set_event(RSI_RX_EVENT);

  }
  //! Receive the data coming from UART interface
  rsi_recv_pkt_serial();

  return 0;
}



/*==============================================*/
/**
 * @fn          void rsi_platform_based_init(void)
 * @brief       initialises the mutex for uart packet handlig and
 *              creates thread to receive the packets from module
 * @param[in]   none
 * @param[out]  none
 * @return      none
 * @section description
 * initialises the mutex for uart packet handlig and
 * creates thread to receive the packets from module
 *
 */
void rsi_platform_based_init(void)
{
  int err;
#ifdef WINDOWS
  DWORD thread1;
#else
  pthread_t     thread1;
#endif

#ifdef WINDOWS
#ifdef RSI_ENABLE_DEBUG_PRINT
    printf("\n Waiting for Card Ready...\n");
#endif
  InitializeCriticalSection(&crit_sec);
#else
  //! Mutex initialisation
  if(pthread_mutex_init(&rsi_linux_app_cb.mutex1, NULL) != 0)
  {
#ifdef RSI_ENABLE_DEBUG_PRINT
    printf("\n ERROR occured in Mutex initialisation \n");
#endif
  }
#endif
#ifdef WINDOWS
  err = CreateThread(NULL, // default security attributes
      0, // use default stack size
      (LPTHREAD_START_ROUTINE)recvThread, // thread function
      NULL, // argument to thread function
      0, // use default creation flags
      &thread1); // returns the thread identifier
  if(err == NULL) {
#else
  //! create the thread for reading data coming on UART interface
  if ((err = pthread_create(&thread1, NULL, &recvThread, NULL)))   {
#endif
#ifdef RSI_ENABLE_DEBUG_PRINT
    printf("\n ERROR occured in thread creation\n");
#endif
  }
}



/*==============================================*/
/**
 * @fn          int16_t rsi_frame_read(uint8_t *pkt_buffer)
 * @brief       HAL API to read the packet coming on UART API's
 * @param[in]   pkt_buffer
 * @return      Success
 * @section description This function gets the packet coming from the module and
 * copies to the buffer pointed
 *
 *
 */


int16_t rsi_frame_read(uint8_t *pkt_buffer)
{

  rsi_pkt_t *rx_pkt = NULL;

  uint16_t length;

  //! dequeue the packet from application control block
  rx_pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_linux_app_cb.rcv_queue);

  if(rx_pkt)
  {
    //! calculate the length from the packet
    length = (rx_pkt->desc[0] | ((rx_pkt->desc[1] & 0xF)<<8)) + RSI_FRAME_DESC_LEN;

    //! copy the data
    memcpy(pkt_buffer,&rx_pkt->desc,length);

    //! Free the packet allocated
    free(rx_pkt);


    return 0;
  }

  return -1;

}

/*==============================================*/
/**
 * @fn          void rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame,uint8_t *payloadparam,uint16_t size_param)
 * @brief       This API is used to write the packet on to the UART interface
 * @param[in]   uFrameDscFrame , frame descriptor of the packet
 * @param[in]   payloadparam , payload of the packet
 * @param[in]   size_param , length of the packet
 * @return      0 in case of success ,
 *              - 1 in case of failure
 * @section description
 *  This API is used to write the packet on to the UART interface
 *
 *
 */
int16_t rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame,uint8_t *payloadparam,uint16_t size_param)
{
  int16_t retval = 0;

  //! API to write packet to UART interface
  retval = rsi_uart_send((uint8_t *)uFrameDscFrame, (size_param + RSI_FRAME_DESC_LEN));

  return retval;
}


/*==============================================*/
/**
 * @fn          int32_t rsi_uart_init(void)
 * @brief       UART initialisation
 * @return      0 if success and -1 if failure
 * @section description
 * This API initialises the UART interface
 * module.
 *
 *
 */
int32_t rsi_uart_init(void)
{
  //! UART initialisations
  if (rsi_serial_init())
  {
#ifdef RSI_ENABLE_DEBUG_PRINT
    printf("\n ERROR occured in UART initialisation\n");
#endif
    return -1;
  }
  //! initialise the UART recv queue
  rsi_queues_init(&rsi_linux_app_cb.rcv_queue);

  //! platform based initalisations
  rsi_platform_based_init();

  return 0;

}

/*==============================================*/
/**
 * @fn          int32_t rsi_uart_deinit(void)
 * @brief       UART De initialisation
 * @return      0 if success and -1 if failure
 * @section description
 * This API initialises the UART interface
 * module.
 *
 *
 */

int32_t rsi_uart_deinit(void)
{
#ifdef WINDOWS
	CloseHandle(fd);
    if(RSI_USB_CDC_DEVICE)
	  Sleep(5000);

#else
  close(fd);
  if(RSI_USB_CDC_DEVICE)
    sleep(5000);
#endif

  //! UART initialisations
  if (rsi_serial_init())
  {
#ifdef RSI_ENABLE_DEBUG_PRINT
    printf("\n ERROR occured in UART initialisation\n");
#endif
    return -1;
  }

  return 0;
}


#endif
