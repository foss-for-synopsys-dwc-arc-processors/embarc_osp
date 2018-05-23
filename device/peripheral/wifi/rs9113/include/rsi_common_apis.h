/**
 * @file         rsi_common_apis.h
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
 *  @brief :
 *
 *  @section Description  This file contains
 *
 *
 */

#ifndef RSI_COMMON_APIS_H
#define RSI_COMMON_APIS_H

/******************************************************
 * *                      Macros
 * ******************************************************/

//! To select antenna type , internal antenna for 301 modules RFOUT2 for 03 modules
# define RSI_INTERNAL_OR_RFOUT2      0

//! To select antenna type , ufl connector for 301 modules RFOUT1 for 03 modules
# define RSI_UFL_OR_RFOUT1           1

//! success return value
#define RSI_SUCCESS                        0

//! failure return value
#define RSI_FAILURE                       -1

//!Load Image types
#define RSI_LOAD_IMAGE_I_FW                  '1'
#define RSI_LOAD_IMAGE_I_ACTIVE_LOW_FW       0x71

//! Upgrade images
#define RSI_UPGRADE_IMAGE_I_FW               '2'      /* For upgrading Firmware*/
#define RSI_BL_UPGRADE                       '#'      /* For upgrading Bootloader*/


/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

//! enumerations for power save profile modes
typedef enum rsi_power_save_profile_mode_e
{
  RSI_ACTIVE                    = 0,
  RSI_SLEEP_MODE_1              = 1,
  RSI_SLEEP_MODE_2              = 2,
  RSI_SLEEP_MODE_8              = 8
}rsi_power_save_profile_mode_t;

//! enumerations for power save profile types
typedef enum rsi_power_save_profile_type_e
{
  RSI_MAX_PSP                   = 0,
  RSI_FAST_PSP                  = 1,
  RSI_UAPSD                     = 2
}rsi_power_save_profile_type_t;

//! enumerations for differnt power mode exists for RS9113
typedef enum rsi_power_save_mode_e
{
  RSI_POWER_MODE_DISABLE           = 0,
  RSI_CONNECTED_SLEEP_PS           = 1,
  RSI_CONNECTED_GPIO_BASED_PS      = 2,
  RSI_CONNECTED_MSG_BASED_PS       = 3,
  RSI_GPIO_BASED_DEEP_SLEEP        = 8,
  RSI_MSG_BASED_DEEP_SLEEP         = 9
}rsi_power_save_mode_t;


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

extern int32_t rsi_driver_init(uint8_t *buffer,uint32_t length);
extern int32_t rsi_wireless_init(uint16_t opermode,uint16_t coex_mode);
extern int32_t rsi_wireless_antenna(uint8_t type,uint8_t gain_2g, uint8_t gain_5g, uint8_t antenna_path, uint8_t antenna_type);
extern int32_t rsi_wireless_deinit(void);
extern int32_t rsi_common_get_status(void);
extern void rsi_wireless_driver_task(void);
extern void rsi_wireless_driver_task_scheduler(void);
#endif
