/**
 *   @file     rsi_zigb_utility.c
 *   @version  1.0
 *   @date     2014-Aug-23
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


#ifdef RSI_ZB_ENABLE
#include "rsi_driver.h"
//#include "rsi_zigb_types.h"

/*===========================================================================
 *
 * @fn          void rsi_zigb_copy_xt_panid(uint8_t* Panid, uint8_t *buffer)
 * @brief       Copies the received panid to buffer
 * @param[in]   panid- panid data
 * @param[out]  buffer
 * @return      none
 * @section description
 * This API is used to copy panid to buffer pointer
 *
 * ===========================================================================*/

void rsi_zigb_copy_xt_panid(uint8_t* Panid, uint8_t *buffer)
{
  uint8_t i = 0;
  for(i = 0; i < 8; i++) {
    buffer[i] = Panid[i];
  }
}

/*===========================================================================
 *
 * @fn          void rsi_zigb_xtract_xt_panid(uint8_t* buffer, uint8_t *Panid)
 * @brief       Extracts the received panid from buffer
 * @param[in]   buffer containing panid
 * @param[out]  panid- panid data
 * @return      none
 * @section description
 * This API is used to extract panid from buffer pointer
 *
 * ===========================================================================*/

void rsi_zigb_xtract_xt_panid(uint8_t* buffer, uint8_t *Panid)
{
  uint8_t i = 0;
  for(i = 0; i < 8; i++) {
    Panid[i] =  buffer[i];
  }
}

/*===========================================================================
 *
 * @fn          uint16_t rsi_zigb_buffer2_uint16(uint8_t* buffer)
 * @brief       Extract uint16 value
 * @param[in]   buffer
 * @param[out]  none
 * @return      uint16 (2byte) value
 * @section description
 * This API is used to extract 2 bytes data from buffer pointer
 *
 * ===========================================================================*/

uint16_t rsi_zigb_buffer2_uint16(uint8_t* buffer)
{
  uint16_t *pData;
  pData = (uint16_t*)buffer;
  return *pData;
}

/*===========================================================================
 *
 * @fn          uint32_t rsi_zigb_buffer2_uint32(uint8_t* buffer)
 * @brief       Extract uint32 value
 * @param[in]   buffer
 * @param[out]  none
 * @return      uint32 (4byte) value
 * @section description
 * This API is used to extract 4 bytes data from buffer pointer
 *
 * ===========================================================================*/

uint32_t rsi_zigb_buffer2_uint32(uint8_t* buffer)
{
  uint32_t *pData;
  pData = (uint32_t*)buffer;
  return *pData;
}

/*===========================================================================
 *
 * @fn          void rsi_zigb_uint16to_buffer(uint16_t data, uint8_t* buffer)
 * @brief       Copy 2 bytes data to buffer
 * @param[in]   uint16 data
 * @param[out]  buffer
 * @return      void
 * @section description
 * This API is used to copy 2 bytes data to buffer pointer
 *
 * ===========================================================================*/

void rsi_zigb_uint16to_buffer(uint16_t data, uint8_t* buffer)
{
#if 0
  uint16_t *pBuf = (uint16_t*)buffer;
  *pBuf = data;
#else
  buffer[0] = data;
  buffer[1] = data >> 8;
#endif
}

/*===========================================================================
 *
 * @fn          void rsi_zigb_uint32to_buffer(uint32_t data, uint8_t* buffer)
 * @brief       Copy 4 bytes data to buffer
 * @param[in]   uint32 data
 * @param[out]  buffer
 * @return      void
 * @section description
 * This API is used to copy 4 bytes data to buffer pointer
 *
 * ===========================================================================*/

void rsi_zigb_uint32to_buffer(uint32_t data, uint8_t* buffer)
{
  uint32_t *pBuf = (uint32_t*)buffer;
  *pBuf = data;
}

/*===========================================================================
 *
 * @fn          void rsi_zigb_mcpy(uint8_t *src, uint8_t *dst, uint8_t size)
 * @brief       Copy memory from src to dst
 * @param[in]   source buffer
 * @param[out]  destination buffer
 * @param[in]   Size of buffer
 * @return      none
 * @section description
 * This API is used to copy given size memory from src to dst
 *
 * ===========================================================================*/

void rsi_zigb_mcpy(uint8_t *src, uint8_t *dst, uint8_t size)
{
  uint8_t  i = 0;
  for(i =0 ;i < size; i++) {
    dst[i] = src[i];
  }
}

/*===========================================================================
 *
 * @fn          void rsi_zigb_memset(uint8_t *src, uint8_t data, uint8_t size)
 * @brief       Memset the buffer with given data
 * @param[out]  source buffer
 * @param[in]   data
 * @param[in]   size of buffer
 * @return      none
 * @section description
 * This API is used to reset the given buffer with the provided data
 *
 * ===========================================================================*/

void rsi_zigb_memset(uint8_t *src, uint8_t data, uint8_t size)
{
  uint8_t  i = 0;
  for(i =0 ;i < size; i++) {
    src[i] = data;
  }
}

/*===========================================================================
 *
 * @fn          uint8_t rsi_zigb_mcmp(uint8_t *buf1,uint8_t *buf2,uint8_t len)
 * @brief       Compares two buffers of certain length
 * @param[in]   pointer to buffer 1
 * @param[in]   pointer to buffer 2
 * @param[in]   Length of bufres to be compared
 * @return      1: FAIL,0 : SUCCESS
 * @section description
 * This API is used to compare two buffers of length 'len'
 *
 * ===========================================================================*/

uint8_t rsi_zigb_mcmp(uint8_t *buf1,uint8_t *buf2,uint8_t len)
{
  int i;
  for ( i =0 ; i< len ; i++)
  {
    if(*(buf1 + i)!= *(buf2 + i))
      return 1;
  }
  return 0;
}

/*=============================================================================*/
/**
 * @fn              uint16 rsi_zigb_bytes2R_to_uint16(uint8 *dBuf)
 * @brief           Convert a 2 byte array to uint16, first byte in array is LSB
 * @param[in]       uint8 *dBuf,pointer to a buffer to get the data from
 * @param[out]      none
 * @return          uint16, converted data
 */
uint16_t rsi_zigb_bytes2R_to_uint16(uint8_t *dBuf)
{
  uint16_t val;
  val = dBuf[1];
  val <<= 8;
  val |= dBuf[0] & 0x000000ff;
  return val;
}
#endif
