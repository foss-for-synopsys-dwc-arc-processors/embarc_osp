/**
 * @file       rsi_hal_mcu_interrupt.c
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
 * @brief HAL INTERRUPT: Functions related to HAL Interrupts
 *
 * @section Description
 * This file contains the list of functions for configuring the microcontroller interrupts.
 * Following are list of API's which need to be defined in this file.
 *
 */


/**
 * Includes
 */
#include "arc_exception.h"
#include "board.h"
#include "rsi_driver.h"


/*===================================================*/
/**
 * @fn           void rsi_hal_intr_config(void (* rsi_interrupt_handler)())
 * @brief        Starts and enables the SPI interrupt
 * @param[in]    rsi_interrupt_handler() ,call back function to handle interrupt
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to initialize the register/pins
 *               related to interrupts and enable the interrupts.
 */
void rsi_hal_intr_config(void (* rsi_interrupt_handler)())
{


	//! Configure interrupt pin/register in input mode and register the interrupt handler
	int_handler_install(EMSDP_REDPINE_HOST_WAKEUP, rsi_interrupt_handler);
	return;

}


/*===================================================*/
/**
 * @fn           void rsi_hal_intr_mask(void)
 * @brief        Disables the SPI Interrupt
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to mask/disable interrupts.
 */
void rsi_hal_intr_mask(void)
{

	//! Mask/Disable the interrupt
	int_disable(EMSDP_REDPINE_HOST_WAKEUP);
	return;

}


/*===================================================*/
/**
 * @fn           void rsi_hal_intr_unmask(void)
 * @brief        Enables the SPI interrupt
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to enable interrupts.
 */
void rsi_hal_intr_unmask(void)
{

	//! Unmask/Enable the interrupt
	int_enable(EMSDP_REDPINE_HOST_WAKEUP);
	return;

}



/*===================================================*/
/**
 * @fn           void rsi_hal_intr_clear(void)
 * @brief        Clears the pending interrupt
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to clear the handled interrupts.
 */
void rsi_hal_intr_clear(void)
{
	//! Clear the interrupt
//TODO: clear SPI_MASTER interrupt
	return;


}


/*===================================================*/
/**
 * @fn          void rsi_hal_intr_pin_status(void)
 * @brief       Checks the SPI interrupt at pin level
 * @param[in]   none
 * @param[out]  uint8_t, interrupt status
 * @return      none
 * @description This API is used to check interrupt pin status(pin level whether it is high/low).
 */
uint8_t rsi_hal_intr_pin_status(void)
{

	// volatile uint8_t status = 0;
	return int_probe(EMSDP_REDPINE_HOST_WAKEUP) ? 1 : 0;
	//! Return interrupt pin  status(high(1) /low (0))

	// return status;
}

