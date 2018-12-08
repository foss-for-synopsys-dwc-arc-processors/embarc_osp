/**
 * @file       rsi_hal_mcu_ioports.c
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
 * @brief Functions to control IO pins of the microcontroller
 *
 * @section Description
 * This file contains API to control different pins of the microcontroller
 * which interface with the module and other components related to the module.
 *
 */


/**
 * Includes
 */
#include "dev_gpio.h"
#include "embARC_error.h"
#include "rsi_driver.h"

/**
 * Global Variales
 */

DEV_GPIO_PTR gpio_obj;

void gpio_number_to_port_and_offset(uint8_t gpio_number, uint32_t *port, uint32_t *offset){
	// *port = 0;
	// *offset = 0;
}

/*===========================================================*/
/**
 * @fn            void rsi_hal_config_gpio(uint8_t gpio_number,uint8_t mode,uint8_t value)
 * @brief         Configures gpio pin in output mode,with a value
 * @param[in]     uint8_t gpio_number, gpio pin number to be configured
 * @param[in]     uint8_t mode , input/output mode of the gpio pin to configure
 *                0 - input mode
 *                1 - output mode
 * @param[in]     uint8_t value, default value to be driven if gpio is configured in output mode
 *                0 - low
 *                1 - high
 * @param[out]    none
 * @return        none
 * @description This API is used to configure host gpio pin in output mode.
 */
void rsi_hal_config_gpio(uint8_t gpio_number,uint8_t mode,uint8_t value)
{
	//! Initialise the gpio pins in input/output mode
	// uint32_t port, offset;
	// gpio_number_to_port_and_offset(gpio_number, &port, &offset);
	// uint32_t mask = 1 << offset;
	// DEV_GPIO_PTR gpio_obj = gpio_get_dev(port);
	// if(gpio_obj -> gpio_open(mode << offset) == E_OPNED){// for embARC gpio, 1 is output
	// 	gpio_obj -> gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, (void *)mask);
	// 	gpio_obj -> gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)mask);
	// }

	// //! Drive a default value on gpio if gpio is configured in output mode
	// if(mode == 1){
	// 	gpio_obj -> gpio_write (value << offset, mask);
	// }


	return;


}



/*===========================================================*/
/**
 * @fn            void rsi_hal_set_gpio(uint8_t gpio_number)
 * @brief         Makes/drives the gpio  value high
 * @param[in]     uint8_t gpio_number, gpio pin number
 * @param[out]    none
 * @return        none
 * @description   This API is used to drives or makes the host gpio value high.
 */
void rsi_hal_set_gpio(uint8_t gpio_number)
{

	//! drives a high value on GPIO
	// uint32_t port, offset;
	// gpio_number_to_port_and_offset(gpio_number, &port, &offset);
	// uint32_t mask = 1 << offset;
	// DEV_GPIO_PTR gpio_obj = gpio_get_dev(port);
	// gpio_obj -> gpio_write(1, mask);


	return;
}




/*===========================================================*/
/**
 * @fn          uint8_t rsi_hal_get_gpio(void)
 * @brief       get the gpio pin value
 * @param[in]   uint8_t gpio_number, gpio pin number
 * @param[out]  none
 * @return      gpio pin value
 * @description This API is used to configure get the gpio pin value.
 */
uint8_t rsi_hal_get_gpio(uint8_t gpio_number)
{
	uint32_t gpio_value = 0;

	//! Get the gpio value
	// uint32_t port, offset;
	// gpio_number_to_port_and_offset(gpio_number, &port, &offset);
	// uint32_t mask = 1 << offset;
	// DEV_GPIO_PTR gpio_obj = gpio_get_dev(port);
	// gpio_obj -> gpio_read(&gpio_value, mask);


	return gpio_value;



}




/*===========================================================*/
/**
 * @fn            void rsi_hal_set_gpio(uint8_t gpio_number)
 * @brief         Makes/drives the gpio value to low
 * @param[in]     uint8_t gpio_number, gpio pin number
 * @param[out]    none
 * @return        none
 * @description   This API is used to drives or makes the host gpio value low.
 */
void rsi_hal_clear_gpio(uint8_t gpio_number)
{

	//! drives a low value on GPIO
	// uint32_t port, offset;
	// gpio_number_to_port_and_offset(gpio_number, &port, &offset);
	// uint32_t mask = 1 << offset;
	// DEV_GPIO_PTR gpio_obj = gpio_get_dev(port);
	// gpio_obj -> gpio_write(0, mask);


	return;
}


