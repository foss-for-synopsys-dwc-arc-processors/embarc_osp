/**
 * @file        rsi_common.h
 * @version     0.1
 * @date        15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief :  This file contains scheduler functionality defines and function declarations
 *            of common control block used in the driver
 *
 *  @section Description  This file contains scheduler functionality defines and function declarations
 *            of common control block used in the driver
 *
 */

#ifndef RSI_IAP_H
#define RSI_IAP_H
/******************************************************
 * *                      Macros
 * ******************************************************/
#define RSI_MAX_DIGEST_LENGTH   100
/******************************************************/

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

/******************************************************
 * *                 Type Definitions
 * ******************************************************/


//! Structure of MFI Authentication create request
typedef struct rsi_mfi_auth_create_request_s
{
  //! Input Digest length from the host
  uint8_t digest_length[4];

  //! Digest to give to the IAP chip
  uint8_t digest[RSI_MAX_DIGEST_LENGTH];

}rsi_mfi_auth_create_request_t;


/******************************************************
 * *                    Structures
 * ******************************************************/


/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
#endif
