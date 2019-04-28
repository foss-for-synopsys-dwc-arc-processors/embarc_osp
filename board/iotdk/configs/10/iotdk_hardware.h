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
--------------------------------------------- */

#ifndef _IOTDK_HARDWARE_H_
#define _IOTDK_HARDWARE_H_

#include "io_config.h"
#include "apexextensions.h"

/* System clock domains */
#define BOARD_REF_CLK		(16000000U)
#define BOARD_DFSS_CORE_CLK	(144000000U)		/* iotdk DFSS clock (including ARC EM) */
#define BOARD_DFSS_IO_CLK	(BOARD_REF_CLK)		/* iotdk DFSS IO peripherals clock */
#define BOARD_DFSS_AHB_CLK	(BOARD_DFSS_CORE_CLK)	/* iotdk top-level AHB peripherals clock as DFSS */
#define BOARD_DFSS_APB_CLK	(BOARD_DFSS_CORE_CLK)		/* iotdk top-level APB peripherals clock as DFSS */
#define BOARD_APB_CLK 		(BOARD_DFSS_APB_CLK)	/* iotdk top-level APB peripherals clock */
#define BOARD_SPI_CLK		(BOARD_DFSS_AHB_CLK)		/* iotdk SPI clock*/

/* CPU clock frequency definition */
#ifdef BOARD_DFSS_CORE_CLK
/* Get CPU clock frequency from BOARD_DFSS_CORE_CLK defined in specification */
	#define CLK_CPU		(BOARD_DFSS_CORE_CLK)
#elif defined(ARC_FEATURE_CPU_CLOCK_FREQ)
/* Get CPU clock frequency definition from TCF file */
	#define CLK_CPU		(ARC_FEATURE_CPU_CLOCK_FREQ)
#else
/* Default CPU clock frequency */
	#define CLK_CPU		(BOARD_REF_CLK)
#endif


#define BASE_ADDR_BOOT_SPI 		0x10000000
#define BASE_ADDR_FLASH 		0x40000000


#define BASE_ADDR_I2S_TX		0xF0000000
#define BASE_ADDR_I2S_RX		0xF0001000
#define BASE_ADDR_I3C			0xF0002000
#define BASE_ADDR_BOOT_SPI_CTRL		0xF0003000
#define BASE_ADDR_RTC			0xF0005000
#define BASE_ADDR_PWM			0xF0006000
#define BASE_ADDR_ADC			0xF0007000
#define BASE_ADDR_DMAC			0xF0008000
#define BASE_ADDR_FLASH_CTRL		0xF0009000
#define BASE_ADDR_SYSCONFIG		0xF000A000
#define BASE_ADDR_SDIO			0xF000B000



/* Interrupt Connection */
/* SPI */
#define INTNO_SPIMST0_SPI_MST_ERR_INTR		70
#define INTNO_SPIMST0_SPI_MST_INTR_RX_AVAIL	71
#define INTNO_SPIMST0_SPI_MST_INTR_TX_REQ	72
#define INTNO_SPIMST0_SPI_MST_INTR_IDLE		73
#define INTNO_SPIMST1_SPI_MST_ERR_INTR		74
#define INTNO_SPIMST1_SPI_MST_INTR_RX_AVAIL	75
#define INTNO_SPIMST1_SPI_MST_INTR_TX_REQ	76
#define INTNO_SPIMST1_SPI_MST_INTR_IDLE		77
#define INTNO_SPIMST2_SPI_MST_ERR_INTR		78
#define INTNO_SPIMST2_SPI_MST_INTR_RX_AVAIL	79
#define INTNO_SPIMST2_SPI_MST_INTR_TX_REQ	80
#define INTNO_SPIMST2_SPI_MST_INTR_IDLE		81
/* UART */
#define INTNO_UART0_UART_INTR			86
#define INTNO_UART1_UART_INTR			87
#define INTNO_UART2_UART_INTR			88
#define INTNO_UART3_UART_INTR			89
/* I2S */
#define INTNO_I2S_TX_EMP_0_INTR			92
#define INTNO_I2S_TX_OR_0_INTR 			93
#define INTNO_I2S_RX_DA_0_INTR			94
#define INTNO_I2S_RX_OR_0_INTR			95

#define INTNO_SDIO				91

#endif	/* _IOTDK_HARDWARE_H_ */
