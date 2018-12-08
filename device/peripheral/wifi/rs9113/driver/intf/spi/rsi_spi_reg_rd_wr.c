/**
 * @file    rsi_spi_reg_rd_wr.c
 * @version	0.1
 * @date 		15 Aug 2015
 *
 * Copyright(C) Redpine Signals 2015
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief SPI REGRDWR: SPI Register Read/Write functions
 *
 * @section Description
 * This file contains the SPI based Register R/W functionss
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


/**===========================================================================
 * @fn          int16_t rsi_reg_rd_wr(uint8_t regAddr, uint8_t *dBuf)
 * @brief       Reads a register in the Wi-Fi module
 * @param[in]   uint8_t regAddr, address of spi register to read, addr is 6-bits, upper 2 bits must be cleared
 * @param[in]   uint8_t *dBuf, pointer to the buffer of data to write, assumed to be at least 2 bytes long
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 *
 * @section description
 * Reads a spi register with an address specified.
 *
 */
int16_t rsi_reg_rd(uint8_t regAddr, uint8_t *dBuf)
{
  int16_t                   retval;
  uint8_t                   c1;
  uint8_t                   c2;

  //! set the C1/C2 values
  c1 = RSI_C1INTREAD1BYTES;
  c2 = 0x00;
  // and/or the addrss into C2, addr is 6-bits long, upper two bits must be cleared for byte mode
  c2 |= regAddr;

  //! Send C1C2
  retval = rsi_send_c1c2(c1, c2);


  //! Check for SPI Busy/Error
  if (retval != 0)
  {
    //! exit with error if we timed out waiting for the SPI to get ready
    return retval;
  }

  //! Wait for start token
  retval = rsi_spi_wait_start_token(RSI_START_TOKEN_TIMEOUT,RSI_MODE_8BIT);

  if (retval != 0)
  {
    //! exit with error if we timed out waiting for start token
    return retval;
  }

  //! Start token found now read the two bytes of data
  retval = rsi_spi_transfer(NULL, dBuf, 1,RSI_MODE_8BIT);

  return retval;
}

/**===========================================================================
 * @fn          int16_t rsi_reg_wr(uint8_t regAddr, uint8_t *dBuf)
 * @brief       writes to a register in the Wi-Fi module
 * @param[in]   uint8_t regAddr, address of spi register to be written
 * @param[in]   uint8_t *dBuf, pointer to the buffer of data to write, assumed to be at least 2 bytes long
 * @param[out]  none
 * @return      errCode
 * @return 	-1 = SPI busy / Timeout
 * @return      -2 = SPI Failure
 * @return       0  = SUCCESS
 *
 * @section description
 * Writes to a spi register with an address specified.
 *
 */

int16_t rsi_reg_wr(uint8_t regAddr, uint8_t *dBuf)
{
  int16_t                   retval;
  uint8_t                   c1;
  uint8_t                   c2;
  uint8_t                   localBuf[8];

  //! set the C1/C2 values
  c1 = RSI_C1INTWRITE2BYTES;
  c2 = 0x00;
  //! and/or the addrss into C2, addr is 6-bits long, upper two bits must be cleared for byte mode
  c2 |= regAddr;

  //! Send C1C2
  retval = rsi_send_c1c2(c1, c2);

  //! Check for SPI Busy/Error
  if (retval != 0)
  {
    //! exit with error if we timed out waiting for the SPI to get ready
    return retval;
  }

  //! SPI send
  retval = rsi_spi_transfer(dBuf, localBuf, 2,RSI_MODE_8BIT);

  if (retval != 0)
  {
    //! exit with error if we timed out waiting for start token
    return retval;
  }
  return retval;
}

#endif
