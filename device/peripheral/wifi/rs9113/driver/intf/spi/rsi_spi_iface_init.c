/**
 * @file     rsi_spi_iface_init.c
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
 * @brief SPI INIT: Functions which initiaize the SPI hardware interface in the module
 *
 * @section Description
 * This file contains the SPI Initialization function.
 * This function enables the SPI interface on the Wi-Fi chip.
 * This function is only run once during startup, after power-on, or reset
 *
 *
 */


/**
 * Includes
 */
#include "rsi_driver.h"
#ifdef RSI_SPI_INTERFACE

/**
 * Global Variables
 */
uint32_t                 rsi_spiTimer1;
uint32_t                 rsi_spiTimer2;
uint32_t                 rsi_spiTimer3;


/*=============================================*/
/**
 * @fn                  int16_t rsi_spi_iface_init(void)
 * @brief               Start the SPI interface
 * @param[in]           none
 * @param[out]          none
 * @return              errCode
 *                      -1 = SPI busy / Timeout
 *                      -2 = SPI Failure
 *                      0  = SUCCESS
 * @section description
 * This API initializes the Wi-Fi module’s Slave SPI interface.
 */
int16_t rsi_spi_iface_init(void)
{
  uint8_t                   txCmd[2];
  int16_t                   retval = 0;
  uint16_t                  timeout;
  uint8_t                   localBuf[2] = {0};


  //! 10ms timeout on command, nothing magic, just a reasonable number
  timeout   = 10;

  //! init the timer counter
  RSI_RESET_TIMER1;
  while(1)
  {
    if (RSI_INC_TIMER_1 > timeout)
    {
      retval = RSI_ERROR_SPI_BUSY;
      break;
    }

    txCmd[0] = RSI_C1_INIT_CMD;
    txCmd[1] = 0x00;

    retval = rsi_spi_transfer(txCmd, localBuf, 2, RSI_MODE_8BIT);
    //retval = rsi_spi_send(txCmd, 2, localBuf, RSI_MODE_8BIT);
    if (localBuf[1] == RSI_SPI_SUCCESS)
    {
      retval = RSI_SUCCESS;
      break;
    }
    else
    {
      retval = RSI_ERROR_SPI_BUSY;
    }
  }

  return retval;
}

/*=============================================*/
/**
 * @fn                  int16_t rsi_ulp_wakeup_init(void)
 * @brief               Start the SPI interface
 * @param[in]           none
 * @param[out]          none
 * @return              none
 * @section description
 * This API initializes the Wi-Fi module’s Slave SPI interface on ulp wakeup.
 */
void rsi_ulp_wakeup_init(void)
{
  uint8_t                   txCmd[2];

  txCmd[0] = RSI_C1_INIT_CMD;
  txCmd[1] = 0x00;

  rsi_spi_transfer(txCmd, NULL, 2, RSI_MODE_8BIT);
}
#endif
