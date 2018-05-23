/**
 * @file     rsi_hal.h
 * @version  0.1
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
 * @brief HAL HEADER: Function prototypes for HAL
 *
 * @section Description
 * This file contains the function definition prototypes, defines used in HAL and related functions
 *
 *
 */


#ifndef _RSIHAL_H_
#define _RSIHAL_H_


#ifndef RSI_SAMPLE_HAL
#include "hal/rsi_hal.h"
#else


/******************************************************
 * *                      Macros
 * ******************************************************/


//! User can configure this pin based on platform


//! GPIO to reset WiSeConnect Module
#define RSI_HAL_RESET_PIN                1

//! GPIO to receive packet pending interrupt
#define RSI_HAL_MODULE_INTERRUPT_PIN     2

//! GPIO to receive module wakeup from power save indication
#define RSI_HAL_WAKEUP_INDICATION_PIN    3

//! ULP GPIO to give sleep confirmation to module to goto sleep in power save
#define RSI_HAL_SLEEP_CONFIRM_PIN        4

//! GPIO to receive SPI interface busy and ready indication from module
#define RSI_HAL_INTERFACE_READY_PIN      5

//! LP GPIO to give sleep confirmation to module to goto sleep in power save
#define RSI_HAL_LP_SLEEP_CONFIRM_PIN     6

//! GPIO to power on/off WiSeConnect Module
#define RSI_HAL_MODULE_POWER_CONTROL     7

//! GPIO Pins related Macros
//! Macro to configure GPIO in output mode
#define RSI_HAL_GPIO_OUTPUT_MODE         1

//! Macro to configure GPIO in input mode
#define RSI_HAL_GPIO_INPUT_MODE          0

//! Macro to drive low value on GPIO
#define RSI_HAL_GPIO_LOW                0

//! Macro to drive high value on GPIO
#define RSI_HAL_GPIO_HIGH               1



//! Timer related macros
//! Macro to configure timer type in single shot
#define RSI_HAL_TIMER_TYPE_SINGLE_SHOT   0

//! Macro to configure timer type in periodic
#define RSI_HAL_TIMER_TYPE_PERIODIC      1

//! Macro to configure timer in micro seconds mode
#define RSI_HAL_TIMER_MODE_MICRO        0

//! Macro to configure timer in milli seconds mode
#define RSI_HAL_TIMER_MODE_MILLI        1



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


void rsi_hal_intr_config(void (* rsi_interrupt_handler)());
void rsi_hal_intr_mask(void);
void rsi_hal_intr_unmask(void);
void rsi_hal_intr_clear(void);
uint8_t rsi_hal_intr_pin_status(void);
void rsi_hal_config_gpio(uint8_t gpio_number,uint8_t mode,uint8_t value);
void rsi_hal_set_gpio(uint8_t gpio_number);
uint8_t rsi_hal_get_gpio(uint8_t gpio_number);
void rsi_hal_clear_gpio(uint8_t gpio_number);
int16_t rsi_spi_transfer(uint8_t *tx_buff,uint8_t *rx_buff, uint16_t transfer_length,uint8_t mode);
int16_t rsi_uart_send(uint8_t *ptrBuf, uint16_t bufLen);
int16_t rsi_uart_recv(uint8_t *ptrBuf, uint16_t bufLen);
uint32_t rsi_get_random_number(void);
int32_t rsi_timer_start(uint8_t timer_node, uint8_t mode,uint8_t type, uint32_t duration,void (* rsi_timer_expiry_handler)());
int32_t rsi_timer_stop(uint8_t timer_node);
uint32_t rsi_timer_read(uint8_t timer_node);
void rsi_delay_us(uint32_t delay_us);
void rsi_delay_ms(uint32_t delay_ms);

#endif
#endif
