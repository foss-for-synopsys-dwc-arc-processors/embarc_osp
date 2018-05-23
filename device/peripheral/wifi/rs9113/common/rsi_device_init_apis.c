/**
 * @file
 * @version
 * @date
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
#include "rsi_driver.h"

/*==============================================*/
/**
 * @fn      int32_t rsi_bl_module_power_on(void)
 * @brief   This API is used to turn on the module power
 * @param[in]   ,
 * @param[out]
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This  API is used to turn on the module power
 *
 *
 */
int32_t rsi_bl_module_power_on(void)
{
  int32_t status = RSI_SUCCESS;

#ifndef WISECONNECT_PLUS

  //! configure power pin in GPIO mode
  rsi_hal_config_gpio(RSI_HAL_MODULE_POWER_CONTROL,RSI_HAL_GPIO_OUTPUT_MODE,RSI_HAL_GPIO_LOW);

  //! reset power pin in GPIO mode
  rsi_hal_clear_gpio(RSI_HAL_MODULE_POWER_CONTROL);

  //! delay for 200 milli seconds
  rsi_delay_ms(200);
#ifdef RSI_SPI_INTERFACE
  //! power cycle the module
  rsi_bl_module_power_cycle();
#endif
#endif

  return status;

}

/*==============================================*/
/**
 * @fn      int32_t rsi_bl_module_power_off(void)
 * @brief   This API is used to turn off the module power
 * @param[in]   ,
 * @param[out]
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This  API is used to turn off the module power
 */
int32_t rsi_bl_module_power_off(void)
{
  int32_t status = RSI_SUCCESS;

#ifndef WISECONNECT_PLUS

  //! configure power pin in GPIO mode
  rsi_hal_config_gpio(RSI_HAL_MODULE_POWER_CONTROL,RSI_HAL_GPIO_OUTPUT_MODE,RSI_HAL_GPIO_LOW);

  //! reset/drive low value on the GPIO
  rsi_hal_set_gpio(RSI_HAL_MODULE_POWER_CONTROL);


#endif

  return status;
}


/*==============================================*/
/**
 * @fn          int32_t rsi_device_init(uint8_t select_option)
 * @brief       This API is used to set the bootup options of the device
 * @param[in]   select_option,
 * @param[out]  None
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This API is used to set the bootup options of the device
 */

int32_t rsi_device_init(uint8_t select_option)
{
  int32_t status = RSI_SUCCESS;
#ifdef RSI_SPI_INTERFACE
  uint32_t timer_instance;

#ifndef WISECONNECT_PLUS
  //! power cycle the module
  rsi_bl_module_power_cycle();

#ifdef RSI_SPI_INTERFACE
  //! SPI interface initialization
  status = rsi_spi_iface_init();

  if(status != RSI_SUCCESS)
  {
    return -1;
  }
#endif
    rsi_init_timer(&timer_instance, 300);
  do {
    status = rsi_bl_waitfor_boardready();

    if((status < 0) && (status != -3))
    {
      return -1;
    }

    if(rsi_timer_expired(&timer_instance))
    {
      return -1;
    }

  }while(status == -3);

  status = rsi_bl_select_option(select_option);

  if(status < 0)
  {

    return -1;
  }


#else
  //! Configure SLAVE pin GPIO
  rsi_hal_config_gpio(RSI_HAL_INTERFACE_READY_PIN ,0, 0);
#endif
#endif

 //! Configure interrupt
  rsi_hal_intr_config(rsi_interrupt_handler);

    //! Unmask interrupts
  rsi_hal_intr_unmask();

  return status;
}
