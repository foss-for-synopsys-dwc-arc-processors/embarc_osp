/*
 *  Copyright (c) 2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes the platform-specific initializers.
 *
 */

#ifndef PLATFORM_ARC_H_
#define PLATFORM_ARC_H_

#include <stdint.h>
#include <stdbool.h>
#include "openthread/types.h"

#include "board.h"

#ifdef BOARD_EMSK
/* PMOD RF port and pin setting */
#define MRF24J40_SPI_ID             DW_SPI_0_ID
#define MRF24J40_SPI_CS             EMSK_SPI_LINE_0
#define MRF24J40_GPIO_PORT_WAKE     DEV_GPIO_PORT_0
#define MRF24J40_GPIO_PORT_RESET    DEV_GPIO_PORT_0
#define MRF24J40_GPIO_PORT_INTR     DEV_GPIO_PORT_0
#define MRF24J40_GPIO_PIN_WAKE      DEV_GPIO_PIN_30
#define MRF24J40_GPIO_PIN_RESET     DEV_GPIO_PIN_29
#define MRF24J40_GPIO_PIN_INTR      DEV_GPIO_PIN_28
/** PMOD RF SPI FREQ & CLK mode settting */
#define MRF24J40_SPIFREQ            BOARD_SPI_FREQ
#define MRF24J40_SPICLKMODE         SPI_CLK_MODE_0
#endif /* BOARD_EMSK */

#ifdef BOARD_HSDK
/* PMOD RF port and pin setting */
#define MRF24J40_SPI_ID             DW_SPI_1_ID
#define MRF24J40_SPI_CS             HSDC_SPI_CS_0
/* Wake n.c. */
#define MRF24J40_GPIO_PORT_WAKE     CPU_DW_GPIO_PORT_A
#define MRF24J40_GPIO_PORT_RESET    CPU_DW_GPIO_PORT_A
#define MRF24J40_GPIO_PORT_INTR     CPU_DW_GPIO_PORT_A
#define MRF24J40_GPIO_PIN_WAKE      DEV_GPIO_PIN_16
#define MRF24J40_GPIO_PIN_RESET     DEV_GPIO_PIN_21
#define MRF24J40_GPIO_PIN_INTR      DEV_GPIO_PIN_20
/** PMOD RF SPI FREQ & CLK mode settting */
#define MRF24J40_SPIFREQ            BOARD_SPI_FREQ
#define MRF24J40_SPICLKMODE         SPI_CLK_MODE_0
#endif /* BOARD_HSDK */

// Global OpenThread instance structure
extern otInstance *sInstance;

/**
 * This function initializes the alarm service used by OpenThread.
 *
 */
void arcAlarmInit(void);

/**
 * This function retrieves the time remaining until the alarm fires.
 *
 * @param[out]  aTimeval  A pointer to timer value.
 *
 */
void arcAlarmUpdateTimeout(int32_t *aTimeout);

/**
 * This function performs alarm driver processing.
 *
 */
void arcAlarmProcess(otInstance *aInstance);

/**
 * This function initializes the radio service used by OpenThread.
 *
 */
void arcRadioInit(void);

/**
 * This function performs radio driver processing.
 *
 */
void arcRadioProcess(otInstance *aInstance);

/**
 * This function initializes the random number service used by OpenThread.
 *
 */
void arcRandomInit(int num);

/**
 * This function performs UART driver processing.
 *
 */
void arcUartProcess(void);

#endif  // PLATFORM_ARC_H_
