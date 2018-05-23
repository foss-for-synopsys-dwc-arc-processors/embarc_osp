/**
 * @file         rsi_error.h
 * @version      0.1
 * @date         15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains the errors used in the driver
 *
 *  @section Description   This file contains the errors used in the driver
 *
 *
 */
#ifndef RSI_ERROR_H
#define RSI_ERROR_H
/******************************************************
 * *                      Macros
 * ******************************************************/
#define RSI_ERROR_REMOTE_TERNIMATE 0xFF62
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
//! enumeration for error codes used in driver
typedef enum rsi_error_e
{
  RSI_ERROR_NONE =0,
  RSI_ERROR_TIMEOUT = -1,
  RSI_ERROR_INVALID_PARAM = -2,
  RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE = -3,
  RSI_ERROR_PKT_ALLOCATION_FAILURE = -4,
  RSI_ERROR_COMMAND_NOT_SUPPORTED = -5,
  RSI_ERROR_INSUFFICIENT_BUFFER   = -6,
  RSI_ERROR_IN_OS_OPERATION = -7
}rsi_error_t;



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



#endif
