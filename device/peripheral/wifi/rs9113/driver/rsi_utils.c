/**
 * @file       rsi_utils.c
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
 * @brief UTIL: Generic function utilities
 *
 * @section Description
 * This file implements misc utilities/functions.
 *
 */


/**
 * Include files
 */
#include "rsi_driver.h"


/**
 * Global defines
 */


/*=============================================================================*/
/**
 * @fn              void rsi_uint16_to_2bytes(uint8_t *dBuf, uint16_t val)
 * @brief           Convert uint16 to two byte array
 * @param[in]       uint8_t *dBuf,pointer to buffer to put the data in
 * @param[in]       uint16_t data to convert
 * @param[out]      none
 * @return          none
 */
void rsi_uint16_to_2bytes(uint8_t *dBuf, uint16_t val)
{
#ifdef RSI_LITTLE_ENDIAN
  dBuf[0] = val & 0x00ff;
  dBuf[1] = (val >> 8) & 0x00ff;
#else
  dBuf[1] = val & 0x00ff;
  dBuf[0] = (val >> 8) & 0x00ff;
#endif
}

/*=============================================================================*/
/**
 * @fn              void rsi_uint32_to_4bytes(uint8_t *dBuf, uint32_t val)
 * @brief           Convert uint32 to four byte array
 * @param[in]       uint8_t *dBuf,pointer to the buffer to put the data in
 * @param[in]       uint32_t data to convert
 * @param[out]      none
 * @return          none
 */
void rsi_uint32_to_4bytes(uint8_t *dBuf, uint32_t val)
{
#ifdef RSI_LITTLE_ENDIAN
  dBuf[0] = val & 0x000000ff;
  dBuf[1] = (val >> 8) & 0x000000ff;
  dBuf[2] = (val >> 16) & 0x000000ff;
  dBuf[3] = (val >> 24) & 0x000000ff;
#else
  dBuf[3] = val & 0x000000ff;
  dBuf[2] = (val >> 8) & 0x000000ff;
  dBuf[1] = (val >> 16) & 0x000000ff;
  dBuf[0] = (val >> 24) & 0x000000ff;
#endif
}

/*=============================================================================*/
/**
 * @fn              uint16_t rsi_bytes2R_to_uint16(uint8_t *dBuf)
 * @brief           Convert a 2 byte array to uint16, first byte in array is LSB
 * @param[in]       uint8_t *dBuf,pointer to a buffer to get the data from
 * @param[out]      none
 * @return          uint16_t, converted data
 */
uint16_t rsi_bytes2R_to_uint16(uint8_t *dBuf)
{
  uint16_t        val;
#ifdef RSI_LITTLE_ENDIAN
  val = dBuf[1];
  val <<= 8;
  val |= dBuf[0] & 0x000000ff;
#else
  val = dBuf[0];
  val <<= 8;
  val |= dBuf[1] & 0x000000ff;
#endif
  return val;
}

/*=============================================================================*/
/**
 * @fn           uint32_t rsi_bytes4R_to_uint32(uint8_t *dBuf)
 * @brief        Convert a 4 byte array to uint32, first byte in array is LSB
 * @param[in]    uint8_t *dBuf,pointer to buffer to get the data from
 * @param[out]   none
 * @return       uint32, converted data
 */
uint32_t rsi_bytes4R_to_uint32(uint8_t *dBuf)
{
  //! the 32-bit value to return
  uint32_t            val;

#ifdef RSI_LITTLE_ENDIAN
  val = dBuf[3];
  val <<= 8;
  val |= dBuf[2] & 0x000000ff;
  val <<= 8;
  val |= dBuf[1] & 0x000000ff;
  val <<= 8;
  val |= dBuf[0] & 0x000000ff;
#else
  val = dBuf[0];
  val <<= 8;
  val |= dBuf[1] & 0x000000ff;
  val <<= 8;
  val |= dBuf[2] & 0x000000ff;
  val <<= 8;
  val |= dBuf[3] & 0x000000ff;
#endif

  return val;
}

/*==============================================*/
/**
 * @fn         rsi_ascii_hex2num
 * @brief      ascii to hex convertion
 * @param[in]  ascii_hex_in
 * @return     int32_t, hex number
 * @section description
 * */

int8_t rsi_ascii_hex2num(int8_t ascii_hex_in)
{
	if((ascii_hex_in >= '0') && (ascii_hex_in <= '9'))
		return (ascii_hex_in - '0');
	if((ascii_hex_in >= 'A') && (ascii_hex_in <= 'F'))
		return (ascii_hex_in - 'A' + 10);
	if((ascii_hex_in >= 'a') && (ascii_hex_in <= 'f'))
		return (ascii_hex_in - 'a' + 10);

	return 0;
}
/*=============================================================================*/
/**
 * @fn		  int8 rsi_char_hex2dec
 * @brief	  Convert given ASCII hex notation to descimal notation (used for mac address)
 * @param[in] int8 *cBuf, ASCII hex notation string
 * @return	  value in integer
 */
int8_t rsi_char_hex2dec (int8_t *cBuf)
{
  int8_t k=0;
  uint8_t i;
  for(i=0;i<strlen((char*)cBuf);i++)
  {
    k = ((k*16) + rsi_ascii_hex2num(cBuf[i]));
  }
  return k;
}

/*=============================================================================*/
/**
 * @fn			void rsi_ascii_dev_address_to_6bytes_rev
 * @brief		Convert an ASCII : notation MAC address to a 6-byte hex address
 * @param[in	int8 *asciiMacFormatAddress, source address to convert, must be a null terminated string
 * @param[out]	uint8 *hex_addr, converted address is returned here
 * @return		none
 */
void rsi_ascii_dev_address_to_6bytes_rev(uint8_t *hex_addr, int8_t *ascii_mac_address)
{
  uint8_t  i;					    // loop counter
  uint8_t  cBufPos;					// which char in the ASCII representation
  uint8_t  byteNum;					// which byte in the 32Bithex_address
  int8_t   cBuf[6]; 				// temporary buffer

  byteNum = 5;
  cBufPos = 0;
  for (i = 0; i < strlen((char *)ascii_mac_address); i++) {
    // this will take care of the first 5 octets
    if (ascii_mac_address[i] == ':') {					// we are at the end of the address octet
      cBuf[cBufPos] = 0;						// terminate the string
      cBufPos = 0;							// reset for the next char
      hex_addr[byteNum--] = (uint8_t)rsi_char_hex2dec((int8_t *)cBuf);	// convert the strint to an integer
    }
    else {
      cBuf[cBufPos++] = ascii_mac_address[i];
    }
  }
  // handle the last octet						// we are at the end of the string with no .
  cBuf[cBufPos] = 0x00;							// terminate the string
  hex_addr[byteNum] = (uint8_t)rsi_char_hex2dec((int8_t *)cBuf);		// convert the strint to an integer
}

int8_t hex_to_ascii (uint8_t  hex_num)
{
	uint8_t  ascii = 0;

	switch (hex_num & 0x0F) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		ascii = (hex_num & 0x0F) + '0';
		return ascii;


	case 0xa:
	case 0xb:
	case 0xc:
	case 0xd:
	case 0xe:
	case 0xf:
		ascii = (hex_num & 0x0F) - 10 + 'A';
		return ascii;
	}

	return ascii;
}

/*=============================================================================*/
/**
 * @fn			void rsi_ascii_dev_address_to_6bytes_rev(uint8 *hex_addr, int8 *ascii_mac_address)
 * @brief		Convert an ASCII : notation MAC address to a 6-byte hex address
 * @param[in		int8 *asciiMacFormatAddress, source address to convert, must be a null terminated string
 * @param[out]		uint8 *hex_addr, converted address is returned here
 * @return		none
 */
void rsi_6byte_dev_address_to_ascii(int8_t *ascii_mac_address, uint8_t *hex_addr)
{
  int8_t  i;					    // loop counter
  uint8_t  cBufPos;					// which char in the ASCII representation


  //byteNum = 5;
  cBufPos = 0;
  for (i = 5; i >= 0; i--) {
	  ascii_mac_address[cBufPos++] = hex_to_ascii (hex_addr[i] >> 4);
	  ascii_mac_address[cBufPos++] = hex_to_ascii (hex_addr[i]);
	  if (i != 0) {
		  ascii_mac_address[cBufPos++] = ':';
	  }
  }
}


/*==============================================*/
/**
 * @fn          lmac_crc8_c
 * @brief       This function is a crc function
 *              6 bit value
 * @param[in]   uint8_t  crc byte input
 * @param[in]   uint8_t  accumulated crc
 * @param[in]   uint8_t  last byte crc
 * @param[out]  uint8_t  6bit crc
 *
 * @section description
 * This API is used to calcuate crc for given byte and accumulate crc.
 */
uint8_t lmac_crc8_c(uint8_t crc8_din, uint8_t crc8_state, uint8_t end)
{
  uint8_t din     [8] ;
  uint8_t state   [8] ;
  uint8_t state_c [8] ;
  uint8_t crc8_out    ;

  din[0] = ((crc8_din & BIT(7)) >> 7) ;
  din[1] = ((crc8_din & BIT(6)) >> 6) ;
  din[2] = ((crc8_din & BIT(5)) >> 5) ;
  din[3] = ((crc8_din & BIT(4)) >> 4) ;
  din[4] = ((crc8_din & BIT(3)) >> 3) ;
  din[5] = ((crc8_din & BIT(2)) >> 2) ;
  din[6] = ((crc8_din & BIT(1)) >> 1) ;
  din[7] = ((crc8_din & BIT(0)) >> 0) ;

  state[0] = ((crc8_state & BIT(0)) >> 0) ;
  state[1] = ((crc8_state & BIT(1)) >> 1) ;
  state[2] = ((crc8_state & BIT(2)) >> 2) ;
  state[3] = ((crc8_state & BIT(3)) >> 3) ;
  state[4] = ((crc8_state & BIT(4)) >> 4) ;
  state[5] = ((crc8_state & BIT(5)) >> 5) ;
  state[6] = ((crc8_state & BIT(6)) >> 6) ;
  state[7] = ((crc8_state & BIT(7)) >> 7) ;

  state_c[7] = (state[7]^din[7])^(state[6]^din[6])^
    (state[5]^din[5]);

  state_c[6] = (state[6]^din[6])^(state[5]^din[5])^
    (state[4]^din[4]);

  state_c[5] = (state[5]^din[5])^(state[4]^din[4])^
    (state[3]^din[3]);

  state_c[4] = (state[4]^din[4])^(state[3]^din[3])^
    (state[2]^din[2]);

  state_c[3] = (state[1]^din[1])^(state[2]^din[2])^
    (state[3]^din[3])^(state[7]^din[7]);

  state_c[2] = (state[0]^din[0])^(state[1]^din[1])^
    (state[2]^din[2])^(state[6]^din[6]);

  state_c[1] = (state[0]^din[0])^(state[1]^din[1])^
    (state[6]^din[6]);

  state_c[0] = (state[0]^din[0])^(state[7]^din[7])^
    (state[6]^din[6]);
  if(!end){
    crc8_out =
      ((state_c[0] & BIT(0)) << 0)
      | ((state_c[1] & BIT(0)) << 1)
      | ((state_c[2] & BIT(0)) << 2)
      | ((state_c[3] & BIT(0)) << 3)
      | ((state_c[4] & BIT(0)) << 4)
      | ((state_c[5] & BIT(0)) << 5)
      | ((state_c[6] & BIT(0)) << 6)
      | ((state_c[7] & BIT(0)) << 7)
      ;
  }
  else{
    crc8_out =
      ((state_c[7] & BIT(0)) << 0)
      | ((state_c[6] & BIT(0)) << 1)
      | ((state_c[5] & BIT(0)) << 2)
      | ((state_c[4] & BIT(0)) << 3)
      | ((state_c[3] & BIT(0)) << 4)
      | ((state_c[2] & BIT(0)) << 5);

    crc8_out = ~crc8_out;
    crc8_out &= 0x3f;
  }
  return(crc8_out);
}

/*==============================================*/
/**
 * @fn          multicast_mac_hash
 * @brief       This function is to calculate 6bit hash value for given mac address
 * @param[in]   uint8_t   pointer to mac address
 * @param[out]  uint8_t   6bit hash value
 *
 * @section description
 * This API is used to calculate 6bit hash value from given mac address.
 */

uint8_t multicast_mac_hash(uint8_t *mac)
{
  uint8_t i, crc = 0xff;
  for(i=0; i<6 ; i++){
    crc = lmac_crc8_c(mac[i],crc, ((i == 5)?1:0));
  }
  return (crc);
}


/*=========================================================================*/
/**
 * @fn          uint8 convert_lower_case_to_upper_case(uint8 lwrcase)
 * @brief       Converts the lower case character to upper case
 * @param[in]   character to convert
 * @return      converted character
 * @section description
 * This function is used to convert the given lower case character to upper case
 * destination string
 */
uint8_t convert_lower_case_to_upper_case(uint8_t lwrcase)
{
  uint8_t digit = (lwrcase >= 'a' && lwrcase <= 'f') ? (lwrcase-0x20) : lwrcase;
  return (digit >= 'A' && digit <= 'F') ? digit - 0x37 : digit - '0';
}

/*=========================================================================*/
/**
 * @fn          void string2array(uint8_t *dst, uint8_t *src, uint32_t length)
 * @brief       Converts string to array
 * @param[in]   Pointer to destination array
 * @return      Pointer to source string
 * @section description
 * This function is used to convert the given string to
 * destination array
 */
void string2array(uint8_t *dst, uint8_t *src, uint32_t length)
{
  int i = 0,j = 0;
  for(i=0 ,j=0 ; i < (length * 2) && j < length; i+=2,j++)
  {
    if(src[i] && src[i + 1])
    {
      dst[j]  = ((uint16_t )convert_lower_case_to_upper_case(src[i])) * 16;
      dst[j] += convert_lower_case_to_upper_case(src[i + 1]);
    }
    else
    {
      dst[j]  = 0;
    }
  }

}



/*=========================================================================*/
/**
 * @fn          uint8* rsi_itoa( uint32 val, uint8 *str)
 * @brief       Converts string to array
 * @param[in]   Pointer to destination array
 * @return      Pointer to source string
 * @section description
 * This function is used to convert the given string to
 * destination array
 */
uint8_t* rsi_itoa(uint32_t val, uint8_t *str)
{
  int16_t  ii = 0, jj = 0;
  uint8_t  tmp[10];
  if(val ==0)
  {
    //! if value is zero then handling
    str[jj] = '0';
    jj++;
    str[jj]='\0';
    return str;
  }

  while( val)
  {
    tmp[ii] = '0' + (val % 10);
    val /= 10;
    ii++;
  }

  for( jj = 0, ii--; ii >= 0; ii--, jj++)
  {
    str[jj] = tmp[ii];
  }
  str[jj]='\0';

  return str;
}
