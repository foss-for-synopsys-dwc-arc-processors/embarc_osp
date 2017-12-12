/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \version 2017.03
 * \date 2014-07-31
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */
/**
 * \defgroup	BOARD_HSDK_COMMON_INIT	HSDK Common Init Module
 * \ingroup	BOARD_HSDK_COMMON
 * \brief	HSDK Board Common Init Module
 * \details
 * 		HSDK timer/gpio/interrupt init process. Device-driver installation is done while
 *	getting the device object for the first time.
 */

/**
 * \file
 * \ingroup	BOARD_HSDK_COMMON_INIT
 * \brief	common hsdk init module
 */

/**
 * \addtogroup	BOARD_HSDK_COMMON_INIT
 * @{
 */
#include "arc_builtin.h"
#include "arc.h"
#include "arc_timer.h"
#include "embARC_debug.h"

#include "board.h"
#include "hsdk_timer.h"
#include "hsdk_hardware.h"
#include "creg_hsdc.h"


/**
 * \brief	hsdk-related mux io init
 */
static void hsdk_mux_init(void)
{
	// Set GPIO 0-3 as GPIO, GPIO4-7 as RS9113 UART (Fixed Configuration)
	creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, CREG_HSDC_GPIOSEL_0, CREG_HSDC_GPIOMUX_GPIO);
	creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, CREG_HSDC_GPIOSEL_1, CREG_HSDC_GPIOMUX_UART);
	// Set GPIO 16-17 as I2C1, GPIO 18-19 as I2C2, PMOD C as I2C Interface
	creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, CREG_HSDC_GPIOSEL_4, CREG_HSDC_GPIOMUX_I2C);
	creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, CREG_HSDC_GPIOSEL_5, CREG_HSDC_GPIOMUX_I2C);
	// Set GPIO 8-11 as SPI1, GPIO 20-21 as GPIO  PMOD A as PMOD Interface Type 2A (expanded SPI)
	creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, CREG_HSDC_GPIOSEL_2, CREG_HSDC_GPIOMUX_SPI);
	creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, CREG_HSDC_GPIOSEL_6, CREG_HSDC_GPIOMUX_GPIO);
	// Set GPIO 12-15 as SPI2, GPIO 22-23 as GPIO PMOD B as PMOD Interface Type 2A (expanded SPI)
	creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, CREG_HSDC_GPIOSEL_3, CREG_HSDC_GPIOMUX_SPI);
	creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, CREG_HSDC_GPIOSEL_7, CREG_HSDC_GPIOMUX_GPIO);
}

/**
 * \brief	Board init routine MUST be called in each application
 * \note	It is better to disable interrupts when calling this function
 *	remember to enable interrupt when you want to use them
 */
void board_init(void)
{
	timer_init();
	hsdk_mux_init();
	hsdk_hardware_init();
	hsdk_timer_init();
}

/** @} end of group BOARD_HSDK_COMMON_INIT */
