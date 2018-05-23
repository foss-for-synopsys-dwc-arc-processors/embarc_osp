/**
 * @file       rsi_spi_mem_rd_wr.c
 * @version    0.1
 * @date       15 Aug 2015
 *
 * Copyright(C) Redpine Signals 2015
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief SPI MEMRDWR: SPI Memory Read/Write functions, r/w memory on the module via the SPI interface
 *
 * @section Description
 * This file contains the SPI Memory Read/Write functions to the module
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


/*===========================================================================*/
/**
 * @fn          int16_t rsi_mem_wr(uint32_t addr, uint16_t len, uint8_t *dBuf)
 * @brief       Performs a memory write to the Wi-Fi module
 * @param[in]   uint32_t addr, address to write to
 * @param[in]   uint16_t, len, number of bytes to write
 * @param[in]   uint8_t *dBuf, pointer to the buffer of data to write
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 *
 * ABH Master Write (Internal Legacy Name)
 */
int16_t rsi_mem_wr(uint32_t addr, uint16_t len, uint8_t *dBuf)
{
  uint8_t         txCmd[4];
  int16_t         retval;
  uint8_t         c1;
  uint8_t         c2;
  uint8_t         c3;
  uint8_t         c4;
  uint8_t     localBuf[8];

  c1     = RSI_C1MEMWR16BIT4BYTE;
#ifdef RSI_BIT_32_SUPPORT
  c2     = RSI_C2SPIADDR4BYTE;
#else
  c2     = RSI_C2SPIADDR1BYTE;
#endif
  //! C3, LSB of length
  c3     = (int8_t)(len & 0x00ff);

  //! C4, MSB of length
  c4     = (int8_t)((len >> 8) & 0x00ff);

  rsi_uint32_to_4bytes(txCmd, addr);

  //! Send C1/C2
  retval = rsi_send_c1c2(c1, c2);

  //! Check for SPI Busy/Error
  if (retval != 0)
  {
    return retval;
  }

  //! Send C3/C4
  retval = rsi_send_c3c4(c3, c4);

  //! Send the 4 address bytes
  retval = rsi_spi_transfer(txCmd,localBuf,sizeof(txCmd),RSI_MODE_8BIT);

  //! Send the Data
#ifdef RSI_BIT_32_SUPPORT
  retval = rsi_spi_transfer(dBuf, localBuf, len, RSI_MODE_32BIT);
#else
  retval = rsi_spi_transfer(dBuf, localBuf, len, RSI_MODE_8BIT);
#endif
  return retval;
}


/*===========================================================================*/
/**
 * @fn          int16_t rsi_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf)
 * @brief       Performs a memory read from the Wi-Fi module
 * @param[in]   uint32_t, address to read from
 * @param[in]   uint16_t, len, number of bytes to read
 * @param[in]   uint8_t *dBuf, pointer to the buffer to receive the data into
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 *
 * ABH Master Read (Internal Legacy Name)
 */
int16_t rsi_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf)
{
  uint8_t                            txCmd[4];
  int16_t                            retval;
  uint8_t                            c1;
  uint8_t                            c2;
  uint8_t                            c3;
  uint8_t                            c4;
  uint8_t                            localBuf[8];

  c1            = RSI_C1MEMRD16BIT4BYTE;
#ifdef RSI_BIT_32_SUPPORT
  c2            = RSI_C2SPIADDR4BYTE;
#else
  c2            = RSI_C2MEMRDWRNOCARE;
#endif
  c3            = len & 0x00ff;                      //! C3, LSB of length
  c4            = (len >> 8) & 0x00ff;               //! C4, MSB of length
  //! put the address bytes into the buffer to send
  txCmd[0]      = addr & 0x000000ff;                 //! A0, Byte 0 of address (LSB)
  txCmd[1]      = (addr >> 8) & 0x000000ff;          //! A1, Byte 1 of address
  txCmd[2]      = (addr >> 16) & 0x000000ff;         //! A2, Byte 2 of address
  txCmd[3]      = (addr >> 24) & 0x000000ff;         //! A3, Byte 3 of address (MSB)
  //! Send C1/C2
  retval = rsi_send_c1c2(c1, c2);

  //! Check for SPI busy
  if (retval != 0)
  {
    return retval;
  }

  //! Send C3/C4
  retval = rsi_send_c3c4(c3, c4);

  //! Send the 4 address bytes
  retval = rsi_spi_transfer(txCmd, localBuf ,sizeof(txCmd) ,RSI_MODE_8BIT);

  //! Wait for the start token
  retval = rsi_spi_wait_start_token(RSI_START_TOKEN_TIMEOUT,RSI_MODE_8BIT);

  //! Read in the memory data
  retval = rsi_spi_transfer( NULL, dBuf, len, RSI_MODE_8BIT);

  return retval;
}

#endif
