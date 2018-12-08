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

#ifndef RSI_BOOTUP_CONFIG_H
#define RSI_BOOTUP_CONFIG_H
/******************************************************
 * *                      Macros
 * ******************************************************/




//!Select Default image
#define RSI_SELECT_IMAGE_I_BY_DEFAULT        '5'

//! Check CRC
#define RSI_CHECK_CRC                     'K'

#define RSI_HOST_INTF_REG_OUT                      0x4105003C
#define RSI_HOST_INTF_REG_IN                       0x41050034
#define RSI_BOARD_READY                            0xABCD
#define RSI_REG_READ                               0xD1
#define RSI_REG_WRITE                              0xD2
#define RSI_SEND_RPS_FILE                          '2'
#define RSI_FWUP_SUCCESSFUL                        'S'
#define RSI_EOF_REACHED                            'E'
#define RSI_PONG_VALID                             'O'
#define RSI_PING_VALID                             'I'
#define RSI_PONG_WRITE                             0xD4
#define RSI_PING_WRITE                             0xD5
#define RSI_PONG_AVAIL                             'O'
#define RSI_PING_AVAIL                             'I'

#define RSI_MIN_CHUNK_SIZE                        4096
#define RSI_FW_START_OF_FILE                       BIT(0)
#define RSI_FW_END_OF_FILE                         BIT(1)

#define RSI_HOST_INTERACT_REG_VALID                (0xAB << 8)

#define RSI_LOADING_INITIATED                      '1'

#define RSI_EOF_REACHED                            'E'
#define RSI_BOOTUP_OPTIONS_LAST_CONFIG_NOT_SAVED   0xF1
#define RSI_BOOTUP_OPTIONS_CHECKSUM_FAIL           0xF2
#define RSI_INVALID_OPTION                         0xF3
#define RSI_CHECKSUM_SUCCESS                       0xAA
#define RSI_CHECKSUM_FAILURE                       0xCC
#define RSI_CHECKSUM_INVALID_ADDRESS               0x4C


//!SPI Internal Register Offset
#define RSI_SPI_INT_REG_ADDR               0x00      //@ register access method
#define RSI_SPI_MODE_REG_ADDR              0x08      //@ register access method


int16_t rsi_bl_upgrade_firmware(uint8_t *firmware_image , uint32_t fw_image_size, uint8_t flags);
int32_t rsi_device_init(uint8_t select_option);
int16_t rsi_bootloader_instructions(uint8_t type, uint16_t *data);
int16_t rsi_bl_waitfor_boardready(void);
int16_t rsi_bl_select_option(uint8_t cmd);
int32_t rsi_bl_module_power_off(void);
int32_t rsi_bl_module_power_on(void);
int16_t rsi_bl_module_power_cycle(void);

#endif


