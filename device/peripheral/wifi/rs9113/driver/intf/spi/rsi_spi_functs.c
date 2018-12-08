/**
 * @file     rsi_spi_functs.c
 * @version  0.1
 * @date     15 Aug 2015
 *
 * Copyright(C) Redpine Signals 2015
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief Common SPI Functions
 *
 * @section Description
 * SPI bus Layer function used to transfer spi protocol level commands to module.
 * For more detail refer PRM for spi commands
 *
 */


/*
 * Includes
 */
#include "rsi_driver.h"

#ifdef RSI_SPI_INTERFACE

/**
 * Global Variables
 */


/*==================================================*/
/**
 * @fn          int16_t rsi_send_c1c2(uint8_t c1, uint8_t c2)
 * @brief       Sends the C1 & C2 commands bytes, should check response for C1 command.Incase of busy should retry.
 * @param[in]   uint8_t c1 SPI c1 command.
 * @param[in]   uint8_t c2 SPI c2 command
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 */
int16_t rsi_send_c1c2(uint8_t c1, uint8_t c2)
{
  int16_t                   retval;
  uint32_t                  timeout;
  uint8_t                   txCmd[2];
  uint8_t                   localBuf[8];

  timeout = 1 * RSI_TICKS_PER_SECOND;

  //! reset the timeout timer to 0
  RSI_RESET_TIMER1;

  while (1)
  {
    txCmd[0] = c1;
    txCmd[1] = c2;
    if (RSI_INC_TIMER_1 > timeout)
    {
      retval = RSI_ERROR_SPI_BUSY;
      break;
    }

    //! Send C1 & C2
    retval = rsi_spi_transfer(&txCmd[0],localBuf, 2,RSI_MODE_8BIT);
    if((localBuf[1] == RSI_SPI_SUCCESS) ||(localBuf[1] == 0x00))
    {
      //! success, so return now
      retval = RSI_SUCCESS;
      break;
    }
    else if (localBuf[1] == RSI_SPI_FAIL)
    {
      retval = RSI_ERROR_SPI_FAIL;
      break;
    }
    else if (localBuf[1] == RSI_SPI_BUSY)
    {
      //! Busy, retry once again
      retval = RSI_ERROR_SPI_BUSY;
    }
  }

  return retval;
}



/*==================================================*/
/**
 * @fn          int16_t rsi_send_c3c4(uint8_t c3, uint8_t c4)
 * @brief       Sends the C3/C4 bytes
 * @param[in]   uint8_t c3 SPI c3 command bytes to be sent
 * @param[in]   uint8_t c4 SPI c4 command bytes to be sent
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 * @prerequisite rsi_send_c1c2 should successful before this function.
 */
int16_t rsi_send_c3c4(uint8_t c3, uint8_t c4)
{
  int16_t                   retval;
  //! command buffer
  uint8_t                   txCmd[2];

  txCmd[0] = c3;
  txCmd[1] = c4;

  //!command should send only 8 bit mode
  retval = rsi_spi_transfer(txCmd, NULL, 2, RSI_MODE_8BIT);

  return retval;
}



/*==================================================*/
/**
 * @fn            int16_t rsi_spi_wait_start_token(uint32_t timeout,uint8_t mode)
 * @brief         Loops reading the SPI until a start token, 0x55, is received
 * @param[in]     uint32_t  timeout   Timeout for start token.
 * @param[in]     uint8_t   mode      To indicate 8bit/32bit mode.
 * @param[out]    none
 * @return        errCode
 *                 0 = success
 *                -1 = busy/timeout failure
 *                -2 = spi failure
 * @section prerequisite
 * should issue read commands before this function.
 */
int16_t rsi_spi_wait_start_token(uint32_t timeout,uint8_t mode)
{
  int16_t                   retval;
#ifdef RSI_BIT_32_SUPPORT
  //! char to send/receive data in
  uint32_t                  txChar;
#else
  uint8_t                   txChar;
#endif
  //! Look for start token
  //! Send a character, could be any character, and check the response for a start token
  //! If we don't find it within the timeout time, error out
  //! Timeout value needs to be passed since context is important
  //! reset the timeout timer to 0;
  RSI_RESET_TIMER1;

  while(1)
  {
    if (RSI_INC_TIMER_1 > timeout)
    {
      retval = RSI_ERROR_SPI_BUSY;
      //! timeout
      break;
    }

    txChar = 0x00;
#ifdef RSI_BIT_32_SUPPORT
    if( mode == RSI_MODE_8BIT)
#endif
      retval = rsi_spi_transfer(NULL, &txChar, 1, mode);
#ifdef RSI_BIT_32_SUPPORT
    else
      retval = rsi_spi_transfer(NULL,(uint8_t *)&txChar, 4, mode);
#endif
    if (txChar == RSI_SPI_START_TOKEN)
    {
      //! found the start token
      retval = RSI_SUCCESS;
      break;
    }
  }
  return retval;
}


/*==================================================*/
/**
 * @fn          int16_t rsi_set_intr_mask(uint8_t interruptMask)
 * @brief       Sets the INTERRUPT MASK REGISTER of the Wi-Fi module
 * @param[in]   uint8_t interruptMask, the value to set the mask register to
 * @param[out]  none
 * @return      errorcode
 *               0 = Success
 *              -1 = busy/timeout failure
 *              -2 = spi failure

*/
int16_t rsi_set_intr_mask(uint8_t interruptMask)
{
  int16_t                     retval;
  int16_t                     i;
#ifdef RSI_BIT_32_SUPPORT
  uint8_t                   dBuf[4]= {0,0,0,0};
#else
  uint8_t                   dBuf[2]= {0,0};
#endif


  //! Create the data to write to the interrupt mask register
  for (i = 0; i < sizeof(dBuf); i++)
  {
    dBuf[i] = 0;
  }

  dBuf[0] = interruptMask;

  //!memory write the mask value
  retval = rsi_mem_wr(RSI_INT_MASK_REG_ADDR, sizeof(dBuf), dBuf);

  return retval;
}




/*==================================================*/
/**
 * @fn          int16_t rsi_clear_interrupt(uint8_t interruptClear)
 * @brief       Clears the interrupt register
 * @param[in]   uint8_t interrutClear, the value to set the interrupt clear register to
 * @param[out]  none
 * @return      errorcode
 *              -1 = busy/timeout failure
 *              -2 = spi failure
 *               0 = Success
 */
int16_t rsi_clear_interrupt(uint8_t interruptClear)
{
  int16_t                      retval;
#ifdef RSI_BIT_32_SUPPORT
  uint8_t                   dBuf[4]= {0,0,0,0};
#else
  uint8_t                   dBuf[2]= {0,0};
#endif
  //! read in the register so we can set bit 5 and write back out
  //!retval = rsi_memRd(RSI_INT_CLR_REG_ADDR, sizeof(dBuf), dBuf);
  //! 0x74, 0x00, 0x22000010
  //! set bit 5, interrupt clear
  dBuf[0] |= interruptClear;

  //! memory write the mask value
  retval = rsi_mem_wr(RSI_INT_CLR_REG_ADDR, sizeof(dBuf), dBuf);

  return retval;
}



/*==================================================*/
/**
 * @fn          int16_t rsi_device_interrupt_status(uint8_t *int_status)
 * @brief       Returns the value of the Interrupt register
 * @param[in]   none
 * @param[out]  none
 * @return      errorcode
 *               0 = Success
 *              -1 = busy/timeout failure
 *              -2 = spi failure
 */
int16_t rsi_device_interrupt_status(uint8_t *int_status)
{
#ifndef WISECONNECT_PLUS
	int16_t                   retval;
	uint32_t                  timeout;

  timeout = 1 * RSI_TICKS_PER_SECOND;

  RSI_RESET_TIMER2;
  while(1)
  {
    if (RSI_INC_TIMER_2 > timeout)
    {
      //! timeout
      retval = RSI_ERROR_SPI_BUSY;
      break;
    }

    //! read the interrupt register
    retval = rsi_reg_rd(RSI_SPI_INT_REG_ADDR, int_status);

    if(retval != RSI_ERROR_SPI_BUSY)
    {
      break;
    }
  }

  return retval;
#else
	rsi_wait_for_spi_slave_idle();

	rsi_send_read_interrupt_status();

	rsi_wait_for_spi_slave_ready();

	rsi_send_dummy_bytes(int_status);

	return  RSI_SUCCESS;
#endif


}




/*==================================================================*/
/**
 * @fn         int16_t rsi_spi_high_speed_enable(void)
 * @param[in]  None
 * @param[out] None
 * @return     0, 0=success
 * @description
 * This API is program Redpine module SPI in high speed mode
 */

int16_t rsi_spi_high_speed_enable()
{
  uint8_t data=1;
  uint8_t return_val;
  return_val = rsi_reg_wr(0x08,&data);
  return return_val;
}





#ifdef WISECONNECT_PLUS
/*==============================================*/
/**
 * @fn           int16_t rsi_send_read_interrupt_status
 * @brief        send read interrupt status
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @section description
 * This API is used to send read interrupt status command
 *
 */

int16_t rsi_send_read_interrupt_status(void)
{
	int16_t                   retval;
	uint8_t                   txCmd[4];

	//@ command buffer
	txCmd[0] = RSI_READ_INTERRUPT_STATUS;
	txCmd[1] = 0;
	txCmd[2] = 0;
	txCmd[3] = 0;

	//!command should send only 8 bit mode
	retval = rsi_spi_transfer(txCmd, NULL, 4, RSI_MODE_8BIT);

	return retval;
}





/*==============================================*/
/**
 * @fn           int16_t rsi_send_dummy_bytes
 * @brief        send read interrupt status
 * @param[in]    uint8_t *response, buffer pointer to copy response
 * @param[out]   none
 * @return       none
 * @section description
 * This API is used to send dummy bytes
 *
 */
int16_t rsi_send_dummy_bytes(uint8_t *response)
{
	 int16_t                 retval;
	 uint8_t                 txCmd[5];  //@ command buffer
	 uint8_t                 rxcmd[5];

	 //!command should send only 8 bit mode
	 retval = rsi_spi_transfer(NULL, rxcmd, 5, RSI_MODE_8BIT);
	 *response = rxcmd[2];
	 return retval;
}



/*==============================================*/
/**
 * @fn           int16_t rsi_send_dummy_bytes
 * @brief        send read interrupt status
 * @param[in]    uint8_t *response, buffer pointer to copy response
 * @param[out]   none
 * @return       none
 * @section description
 * This API is used to send dummy bytes
 *
 */
int16_t rsi_send_read_message()
{
	int16_t                   retval;
	uint8_t                   txCmd[4];

	//@ command buffer
	txCmd[0] = RSI_READ_MESSAGE;
	txCmd[1] = 0;
	txCmd[2] = 0;
	txCmd[3] = 0;

	//!command should send only 8 bit mode
	retval = rsi_spi_transfer(txCmd, NULL, 4, RSI_MODE_8BIT);

	return retval;
}


/*==============================================*/
/**
 * @fn           int16_t rsi_send_dummy_bytes
 * @brief        send read interrupt status
 * @param[in]    uint8_t *response, buffer pointer to copy response
 * @param[out]   none
 * @return       none
 * @section description
 * This API is used to send dummy bytes
 *
 */
int16_t rsi_send_write_message(uint16_t length)
{
	int16_t                   retval;
	uint8_t                   txCmd[4];

	//@ command buffer
	txCmd[0] = RSI_WRITE_MESSAGE;
	txCmd[1] = (length & 0x00FF);
	txCmd[2] = (0 << 4) | ((length & 0x0FFF) >> 8);
	txCmd[3] = 0;

	//!command should send only 8 bit mode
	retval = rsi_spi_transfer(txCmd, NULL, 4, RSI_MODE_8BIT);

	return retval;
}



/*==============================================*/
/**
 * @fn           int16_t rsi_send_dummy_bytes
 * @brief        send read interrupt status
 * @param[in]    uint8_t *response, buffer pointer to copy response
 * @param[out]   none
 * @return       none
 * @section description
 * This API is used to send dummy bytes
 *
 */
void rsi_wait_for_spi_slave_ready()
{
	while(!rsi_hal_get_gpio(RSI_HAL_INTERFACE_READY_PIN));
}


/*==============================================*/
/**
 * @fn           int16_t rsi_send_dummy_bytes
 * @brief        send read interrupt status
 * @param[in]    uint8_t *response, buffer pointer to copy response
 * @param[out]   none
 * @return       none
 * @section description
 * This API is used to send dummy bytes
 *
 */
void rsi_wait_for_spi_slave_idle()
{
	while(rsi_hal_get_gpio(RSI_HAL_INTERFACE_READY_PIN));
}



#endif
#endif
