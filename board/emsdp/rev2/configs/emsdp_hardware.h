/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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
--------------------------------------------- */
#ifndef __EMSDP_HARDWARE_H__
#define __EMSDP_HARDWARE_H__

#include "arc_feature_config.h"
#include "apexextensions.h"
#include "board_emsdp.h"

#undef DBG_init
#undef DBG_print
#undef DGB_getChar
#undef DBG_finish
#undef DBG_finish_with_error
#undef BDG_install
/**
 * Clock Definitions
 */
#ifndef EMSDP_PWM_TIMER_CLOCK
#define EMSDP_PWM_TIMER_CLOCK		(EMSDP_REF_CLOCK)
#endif
#ifndef EMSDP_SPI_CLOCK
#define EMSDP_SPI_REF_CLOCK 		(EMSDP_REF_CLOCK)
#endif

#ifndef EMSDP_I2C_CLOCK
#define EMSDP_I2C_CLOCK			(EMSDP_REF_CLOCK)
#endif

/** CPU Clock Frequency definition */
#if defined(BOARD_CPU_FREQ)
	/*!< Get cpu clock frequency definition from build system */
	#define CLK_CPU					(BOARD_CPU_FREQ)
#elif defined(ARC_FEATURE_CPU_CLOCK_FREQ)
	/*!< Get cpu clock frequency definition from tcf file */
	#define CLK_CPU					(40000000U)//(ARC_FEATURE_CPU_CLOCK_FREQ)
#else
	/*!< Default cpu clock frequency */
	#define CLK_CPU					(50000000U)
#endif

/** Peripheral Bus Reference Clock definition */
#ifdef BOARD_DEV_FREQ
	/*!< Get peripheral bus reference clock defintion from build system */
	#define CLK_BUS_APB				(BOARD_DEV_FREQ)
#else
	/*!< Default peripheral bus reference clock defintion */
	#define CLK_BUS_APB				(100000000U)
#endif


//CREG Control Register Map
#define EMSDP_CREG_BOOT_OFFSET			(0x0000)
#define EMSDP_CREG_START_OFFSET			(0x0010)
#define EMSDP_CREG_PMOD_MUX_OFFSET		(0x0030)
#define EMSDP_CREG_ARDUINO_MUX_OFFSET	(0x0034)
#define EMSDP_CREG_GENERIC_MUX_OFFSET	(0x0038)
#define EMSDP_CREG_IP_SW_RESET_OFFSET	(0x1FF0)
#define EMSDP_CREG_IP_VERSION_OFFSET	(0x1FF8)
#define EMSDP_CREG_IP_TYPE_OFFSET		(0x1FFC)

#endif //__EMSDP_HARDWARE_H__
