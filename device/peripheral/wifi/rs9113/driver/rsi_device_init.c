/**
 * @file       rsi_device_init.c
 * @version    2.7
 * @date       2015-Sep-1
 *
 * Copyright(C) Redpine Signals 2015
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief UTIL: Generic function utils such as bytes2Touint16 which are not tied to anything.
 *
 * @section Description
 * This file implements rsi bootloader interaction/functions.
 * @functions Defined:
 * rsi_waitfor_boardready
 * rsi_select_option
 * rsi_upgrade_fw
 * rsi_module_power_cycle
 */



/**
 * Include files
 */
#include "rsi_driver.h"
#ifndef RSI_UART_INTERFACE
/*===========================================================*/
/**
 * @fn          int16 rsi_bl_module_power_cycle(void)
 * @brief       MODULE POWER ON, Power cycles the module
 * @param[in]   none
 * @param[out]  none
 * @return      errCode
 *              -1 = Error
 *              0  = OK
 * @description This API is used to power cycle the module.
 * This API is valid only if there is a power gate, external to the module,
 * which is controlling the power to the module using a GPIO signal of the MCU.
 */
int16_t rsi_bl_module_power_cycle(void)
{

  //! configure Reset pin in GPIO mode
  rsi_hal_config_gpio(RSI_HAL_RESET_PIN,RSI_HAL_GPIO_OUTPUT_MODE,RSI_HAL_GPIO_LOW);

  //! reset/drive low value on the GPIO
  rsi_hal_clear_gpio(RSI_HAL_RESET_PIN);


  //! delay for 100 milli seconds
  rsi_delay_ms(100);

  //! Set/drive high value on the GPIO
  rsi_hal_set_gpio(RSI_HAL_RESET_PIN);
#ifdef WISECONNECT_PLUS

  //! Work around for reset issue for spansion
  //! wait to firmware boot up
  rsi_delay_ms(100);


  //! configure Reset pin in GPIO mode
  rsi_hal_config_gpio(RSI_HAL_RESET_PIN,RSI_HAL_GPIO_INPUT_MODE,RSI_HAL_GPIO_LOW);
#endif

  return 0;
}


/*==============================================*/
/**
 * @fn          int16_t rsi_waitfor_boardready(void)
 * @brief       Waits to receive board ready from WiFi module
 * @param[in]   none
 * @param[out]  none
 * @return      errCode
 *              0  = SUCCESS
 *              < 0 = Failure
 *              -3 = Board ready not received
 *              -4 = Bootup options last configuration not saved
 *              -5 = Bootup options checksum failed
 *              -6 = Bootloader version mismatch
 * @section description
 * This API is used to check board ready from WiFi module.
 */

int16_t rsi_bl_waitfor_boardready(void)
{
  int16_t retval = 0;
  uint16_t read_value = 0;

  retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);

  if(retval < 0)
  {
    return retval;
  }
  if ((read_value & 0xFF00) == (RSI_HOST_INTERACT_REG_VALID & 0xFF00))
  {
    if((read_value & 0xFF) == RSI_BOOTUP_OPTIONS_LAST_CONFIG_NOT_SAVED)
    {
      return -4;
    }
    else if((read_value & 0xFF) == RSI_BOOTUP_OPTIONS_CHECKSUM_FAIL)
    {
      return -5;
    }
#if RSI_BOOTLOADER_VERSION_CHECK
    else if((read_value & 0xFF) == RSI_BOOTLOADER_VERSION)
    {
    }
    else
    {

      return -6;
    }
#endif

    return 0;
  }

  return -3;
}

/*==============================================*/
/**
 * @fn          int16_t rsi_bl_select_option(uint8_t cmd)
 * @brief       Sends cmd to select option to load or update configuration
 * @param[in]   uint8_t cmd, type of configuration to be saved
 * @param[out]  none
 * @return      errCode
 < 0 = Command issue failed
 *              0  = SUCCESS
 * @section description
 * This API is used to send firmware load request to WiFi module or update default configurations.
 */
int16_t rsi_bl_select_option(uint8_t cmd)
{
  uint16_t   boot_cmd = 0;
  int16_t    retval = 0;
  uint16_t   read_value = 0;
  uint32_t timer_instance;

  boot_cmd = RSI_HOST_INTERACT_REG_VALID | cmd;

  retval   = rsi_bootloader_instructions(RSI_REG_WRITE, &boot_cmd);
  if(retval < 0)
  {
    return retval;
  }

  rsi_init_timer(&timer_instance, 300);

  while((cmd != RSI_LOAD_IMAGE_I_FW) && (cmd != RSI_LOAD_IMAGE_I_ACTIVE_LOW_FW))
  {
    retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);
    if(retval < 0)
    {
      return retval;
    }
    if (read_value == (RSI_HOST_INTERACT_REG_VALID | cmd))
    {
      break;
    }
    if(rsi_timer_expired(&timer_instance))
    {
   	  return -1;
    }
  }
  return retval;
}

/*==============================================*/
/**
 * @fn          int16_t rsi_bl_upgrade_firmware(uint8_t *firmware_image , uint32_t fw_image_size, uint8_t flags)
 * @brief       Upgrades firmware to WiFi module
 * @param[in]   uint8_t image_type, type of firmware image to upgrade
 * @param[in]   uint8_t *firmware_image, pointer to firmware
 * @param[in]   uint32_t fw_image_size, size of firmware image
 * @param[out]  none
 * @return      errCode
 *              <0 = Command issue failed
 *              0  = SUCCESS
 * @section description
 * This API is used to upgrade firmware to WiFi module.
 */
int16_t rsi_bl_upgrade_firmware(uint8_t *firmware_image , uint32_t fw_image_size, uint8_t flags)
{
  static uint16_t boot_cmd;
  uint16_t read_value = 0;
  uint32_t offset = 0;
  int16_t  retval = 0;
  uint32_t boot_insn = 0,poll_resp = 0;
  uint32_t timer_instance;

  //! If it is a start of file set the boot cmd to pong valid
  if(flags & RSI_FW_START_OF_FILE)
  {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID;
  }

  //! check for invalid packet
  if((fw_image_size%(RSI_MIN_CHUNK_SIZE) != 0) && (!(flags & RSI_FW_END_OF_FILE)))
  {
    return -1;
  }

  //! loop to execute multiple of 4K chunks
  while(offset < fw_image_size)
  {
    switch (boot_cmd)
    {
      case (RSI_HOST_INTERACT_REG_VALID | RSI_PING_VALID):
        boot_insn = RSI_PONG_WRITE;
        poll_resp = RSI_PING_AVAIL;
        boot_cmd = RSI_HOST_INTERACT_REG_VALID |RSI_PONG_VALID;
        break;

      case (RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID):
        boot_insn = RSI_PING_WRITE;
        poll_resp = RSI_PONG_AVAIL;
        boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_PING_VALID;
        break;
    }

    retval = rsi_bootloader_instructions(boot_insn, (uint16_t *)((uint8_t *)firmware_image + offset));
    if(retval < 0)
    {
      return retval;
    }

    rsi_init_timer(&timer_instance, 300);

    while(1)
    {
      retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);
      if(retval < 0)
      {
        return retval;
      }
      if (read_value == (RSI_HOST_INTERACT_REG_VALID | poll_resp))
      {
        break;
      }
      if(rsi_timer_expired(&timer_instance))
       {
       	return -1;
       }
    }
    offset += RSI_MIN_CHUNK_SIZE;
  }

  //! For last chunk set boot cmd as End of file reached
  if(flags & RSI_FW_END_OF_FILE)
  {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_EOF_REACHED;

    retval = rsi_bootloader_instructions(RSI_REG_WRITE, &boot_cmd);
    if(retval < 0)
    {
      return retval;
    }
    rsi_init_timer(&timer_instance, 300);
    //! check for successful firmware upgrade
    do
    {
      retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);
      if(retval < 0)
      {
        return retval;
      }
      if(rsi_timer_expired(&timer_instance))
      {
      	return -1;
      }

    }while(read_value != (RSI_HOST_INTERACT_REG_VALID | RSI_FWUP_SUCCESSFUL));
  }
  return retval;
}



/*==============================================*/
/**
 * @fn          int16_t rsi_bootloader_instructions(uint8_t type, uint16_t *data)
 * @brief       Sends boot instructions to WiFi module
 * @param[in]   uint8_t type, type of the insruction to perform
 * @param[in]   uint32_t *data, pointer to data which is to be read/write
 * @param[out]  none
 * @return      errCode
 *              < 0  = Command issued failure/Invalid command
 *                0  = SUCCESS
 *              > 0  = Read value
 * @section description
 * This API is used to send boot instructions to WiFi module.
 */

int16_t rsi_bootloader_instructions(uint8_t type, uint16_t *data)
{
  int16_t   retval = 0;
  uint32_t  cmd = 0;
  uint32_t  j = 0;
  uint16_t  local = 0;
  uint16_t  read_data = 0;
  uint32_t timer_instance;

  switch(type)
  {
    case RSI_REG_READ:
      retval = rsi_mem_rd(RSI_HOST_INTF_REG_OUT,2,(uint8_t *)&read_data);
      *data = read_data;
      break;

    case RSI_REG_WRITE:
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN,2, (uint8_t *)data);
      break;
    case RSI_PING_WRITE:
      for (j = 0; j<2048; j++)
      {
        retval = rsi_mem_wr(0x19000 + (j*2), 2, (uint8_t *)((uint32_t)data + (j*2)));
        if(retval < 0)
        {
          return retval;
        }
      }

      local = (RSI_PING_AVAIL | RSI_HOST_INTERACT_REG_VALID);
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&local);
      break;
    case RSI_PONG_WRITE:

      for (j = 0; j<2048; j++){
        retval = rsi_mem_wr(0x1a000 + (j*2), 2 ,(uint8_t *)((uint32_t)data + (j*2)));
        if(retval < 0)
        {
          return retval;
        }
      }
      //! Perform the write operation
      local = (RSI_PONG_AVAIL | RSI_HOST_INTERACT_REG_VALID);

      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&local);
      break;
    case RSI_UPGRADE_IMAGE_I_FW:

      cmd = RSI_UPGRADE_IMAGE_I_FW | RSI_HOST_INTERACT_REG_VALID;

      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      if(retval < 0)
      {
        return retval;
      }

      rsi_init_timer(&timer_instance, 300);

      do
      {
        retval = rsi_mem_rd(RSI_HOST_INTF_REG_OUT, 2, (uint8_t *)&read_data);
        if(retval < 0)
        {
          return retval;
        }
        if(rsi_timer_expired(&timer_instance))
        {
          return -1;
        }
      }while(read_data != (RSI_SEND_RPS_FILE | RSI_HOST_INTERACT_REG_VALID));
      break;
    case RSI_LOAD_IMAGE_I_FW:
      cmd = RSI_LOAD_IMAGE_I_FW | RSI_HOST_INTERACT_REG_VALID;
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      break;
    case RSI_LOAD_IMAGE_I_ACTIVE_LOW_FW:
      cmd = RSI_LOAD_IMAGE_I_ACTIVE_LOW_FW | RSI_HOST_INTERACT_REG_VALID;
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      break;
    default:
      retval = -2;
      break;
  }
  return retval;
}
#endif
