/**
 * @file       rsi_hal_mcu_random.c
 * @version    0.1
 * @date       18 sept 2015
 *
 * Copyright(C) Redpine Signals 2015
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief: HAL timers APIs
 *
 * @Description:
 * This file Contains all the API's related to HAL
 *
 */


/**
 * Includes
 */
#include "rsi_driver.h"
#include <stdlib.h>


/**
 * Global Variables
 */
//static bool seed_is_set = 0;

/*==================================================================*/
/**
 * @fn          uint32_t rsi_get_random_number(void)
 * @param[in]   None
 * @return      Random number
 * @description This API is used to return random number.
 */
uint32_t rsi_get_random_number()
{

	//! Call the API which generates random number
	// if (seed_is_set == 0){
	// 	srand(time(NULL));
	// 	seed_is_set = 1;
	// }
	return rand();
}

