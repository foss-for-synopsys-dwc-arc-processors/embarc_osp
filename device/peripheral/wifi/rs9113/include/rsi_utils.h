/**
 * @file     rsi_utils.h
 * @version  1.0
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
 * @brief HEADER UTIL: Util Header file, the things that are used in the library
 *
 * @section Description
 * This is the util.h file for the utility functions used by library.
 * Contains prototypes of utils used in rsi_lib_util.c
 *
 */

#ifndef RSI_UTILS_H
#define RSI_UTILS_H
/******************************************************
 * *                      Macros
 * ******************************************************/
#ifndef BIT
#define BIT(a) ((long int)1 << a)
#endif

#define  RSI_MIN(x,y)  ((x) > (y) ? (y) : (x))

#ifndef NULL
#define NULL 0
#endif

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
void rsi_uint32_to_4bytes(uint8_t *dBuf, uint32_t val);
void rsi_uint16_to_2bytes(uint8_t *dBuf, uint16_t val);
uint16_t rsi_bytes2R_to_uint16(uint8_t *dBuf);
uint32_t rsi_bytes4R_to_uint32(uint8_t *dBuf);
uint8_t multicast_mac_hash(uint8_t *mac);
uint8_t lmac_crc8_c(uint8_t crc8_din, uint8_t crc8_state, uint8_t end);
void rsi_ascii_dev_address_to_6bytes_rev(uint8_t *hex_addr, int8_t *ascii_mac_address);
void rsi_6byte_dev_address_to_ascii(int8_t *ascii_mac_address, uint8_t *hex_addr);
uint8_t convert_lower_case_to_upper_case(uint8_t lwrcase);
void string2array(uint8_t *dst, uint8_t *src, uint32_t length);
#endif
