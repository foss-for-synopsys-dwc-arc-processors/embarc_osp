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
#ifndef _AXS_HARDWARE_H_
#define _AXS_HARDWARE_H_

#include "arc_feature_config.h"

/** CPU Clock Frequency definition */
#if defined(BOARD_CPU_FREQ)
	/*!< Get cpu clock frequency definition from build system */
	#define CLK_CPU			(BOARD_CPU_FREQ)
#elif defined(ARC_FEATURE_CPU_CLOCK_FREQ)
	/*!< Get cpu clock frequency definition from tcf file */
	#define CLK_CPU			(ARC_FEATURE_CPU_CLOCK_FREQ)
#else
	/*!< Default cpu clock frequency */
	#define CLK_CPU			(100000000)
#endif

/** Peripheral Bus Reference Clock definition */
#ifdef BOARD_DEV_FREQ
	/*!< Get peripheral bus reference clock defintion from build system */
	#define CLK_BUS_APB		(BOARD_DEV_FREQ)
#else
	/*!< Default peripheral bus reference clock defintion */
	#define CLK_BUS_APB		(25000000U)
#endif

#define CLK_I2C				(100000000U)
#define CLK_UART			(33333333U)
#define CLK_SPI_FLASH			(100000000U)
#define CLK_PGU				(25000000U)

/* each peripheral covers  1 MB space */
#define REGBASE_SPI_FLASH		(0xE0000000U)
#define REGBASE_CGU			(0xE0010000U)	/*!< clock generating unit */
#define REGBASE_CREG_MB			(0xE0011000U)
#define REGBASE_ICTL			(0xE0012000U)	/*!< Interrupt controller based on GPIO */
#define REGBASE_GPIO0			(0xE0013000U)
#define REGBASE_GPIO1			(0xE0014000U)
#define REGBASE_SDIO			(0xE0015000U)
#define REGBASE_NAND			(0xE0016000U)
#define REGBASE_PGU			(0xE0017000U)	/*!< pixel graphic unit */
#define REGBASE_GMAC			(0xE0018000U)
#define REGBASE_SPI0			(0xE001A000U)
#define REGBASE_SPI1			(0xE001B000U)
#define REGBASE_SPI2			(0xE001C000U)
#define REGBASE_I2C0			(0xE001D000U)
#define REGBASE_I2C1			(0xE001E000U)
#define REGBASE_I2S0			(0xE001E000U)	/*!< i2s covers i2c1 in axs 103 */
#define REGBASE_I2C2			(0xE001F000U)
#define REGBASE_UART0			(0xE0020000U)
#define REGBASE_UART1			(0xE0021000U)
#define REGBASE_UART2			(0xE0022000U)
#define REGBASE_AXI_MASTER		(0xE0023000U)
#define REGBASE_USB_EHCI		(0xE0040000U)
#define REGBASE_USB_OHCI		(0xE0060000U)
#define REGBASE_DMAC			(0xE0080000U)

#define REGBASE_CGU_CPU			(0xF0000000U)
#define REGBASE_CREG_CPU		(0xF0001000U)
#define REGBASE_ICTL_CPU		(0xF0002000U)
#define REGBASE_GPIO_CPU		(0xF0003000U)
#define REGBASE_UART_CPU		(0xF0005000U)


/* Interrupt Connection */
#define INTNO_ICTL_MB			24		/*!< mainboard interrupt controller */
#define INTNO_ICTL_CPU			25		/*!< CPU card interrupt controller */
#define INTNO_CREG			26		/*!< ctrl register */

/* Mainboard ICTL interrupt connection */
#define ICTL_INTNO_CGU_PLL_LOCKED		((INTNO_ICTL_MB << 8) | 0)
#define ICTL_INTNO_CGU_PLL_UNLOCKED		((INTNO_ICTL_MB << 8) | 1)
#define ICTL_INTNO_CGU_PLL_LOCK_ERROR		((INTNO_ICTL_MB << 8) | 2)
#define ICTL_INTNO_CREG				((INTNO_ICTL_MB << 8) | 3)
#define ICTL_INTNO_GMAC				((INTNO_ICTL_MB << 8) | 4)
#define ICTL_INTNO_PGU				((INTNO_ICTL_MB << 8) | 5)
#define ICTL_INTNO_NAND				((INTNO_ICTL_MB << 8) | 6)
#define ICTL_INTNO_SDIO				((INTNO_ICTL_MB << 8) | 7)
#define ICTL_INTNO_USB				((INTNO_ICTL_MB << 8) | 8)
#define ICTL_INTNO_DMAC				((INTNO_ICTL_MB << 8) | 9)
#define ICTL_INTNO_SPI_FLASH			((INTNO_ICTL_MB << 8) | 10)
#define ICTL_INTNO_SPI0				((INTNO_ICTL_MB << 8) | 11)
#define ICTL_INTNO_SPI1				((INTNO_ICTL_MB << 8) | 12)
#define ICTL_INTNO_SPI2				((INTNO_ICTL_MB << 8) | 13)
#define ICTL_INTNO_I2C0				((INTNO_ICTL_MB << 8) | 14)
#define ICTL_INTNO_I2C1				((INTNO_ICTL_MB << 8) | 15)
#define ICTL_INTNO_I2S0				((INTNO_ICTL_MB << 8) | 16)
#define ICTL_INTNO_I2C2				((INTNO_ICTL_MB << 8) | 17)
#define ICTL_INTNO_UART0			((INTNO_ICTL_MB << 8) | 18)
#define ICTL_INTNO_UART1			((INTNO_ICTL_MB << 8) | 19)
#define ICTL_INTNO_UART2			((INTNO_ICTL_MB << 8) | 20)
#define ICTL_INTNO_GPIO0			((INTNO_ICTL_MB << 8) | 21)
#define ICTL_INTNO_GPIO1			((INTNO_ICTL_MB << 8) | 22)
#define ICTL_INTNO_ETH_PHY			((INTNO_ICTL_MB << 8) | 23)
#define ICTL_INTNO_HDMI				((INTNO_ICTL_MB << 8) | 24)
#define ICTL_INTNO_EXT0				((INTNO_ICTL_MB << 8) | 25)
#define ICTL_INTNO_EXT1				((INTNO_ICTL_MB << 8) | 26)
#define ICTL_INTNO_EXT2				((INTNO_ICTL_MB << 8) | 27)
#define ICTL_INTNO_AXI_MASTER			((INTNO_ICTL_MB << 8) | 28)

#define ICTL_INTNO_NUM				28

/* CPU card interrupt connection */
#define CPU_ICTL_INTNO_ADDR_DECODER_UPDATED	((INTNO_ICTL_CPU << 8) | 1)
#define CPU_ICTL_INTNO_UART			((INTNO_ICTL_CPU << 8) | 2)
#define CPU_ICTL_INTNO_CGU_PLL_LOCKED		((INTNO_ICTL_CPU << 8) | 3)
#define CPU_ICTL_INTNO_CGU_PLL_UNLOCKED		((INTNO_ICTL_CPU << 8) | 4)
#define CPU_ICTL_INTNO_CGU_PLL_LOCK_ERROR	((INTNO_ICTL_CPU << 8) | 5)

#define CPU_ICTL_INTNO_NUM			5

#define NUM_INT_ALL	(CPU_ICTL_INTNO_NUM + ICTL_INTNO_NUM)

#define ICTL_GPIOA_LINE				12
#define ICTL_GPIOA_LINE_MASK			(1 << ICTL_GPIOA_LINE)


#define I2C_ADDR_RTC			(0xD0 >> 1)
#define I2C_ADDR_EEPROM_BANK1		(0xA8 >> 1)
#define I2C_ADDR_EEPROM_BANK2		(0xAA >> 1)
#define I2C_ADDR_UDA1380_1		(0x30 >> 1)
#define I2C_ADDR_UDA1380_2		(0x34 >> 1)
#define I2C_ADDR_CS42428		(0x9C >> 1)

#define I2C_ADDR_ADV7511_1		(0x70 >> 1)
#define I2C_ADDR_ADV7511_2		(0x72 >> 1)
#define I2C_ADDR_ADV7511_3		(0x78 >> 1)
#define I2C_ADDR_ADV7511_4		(0x7C >> 1)
#define I2C_ADDR_ADV7511_5		(0x7E >> 1)

#include "axc003_gpio.h"
#include "axs_103_interrupt.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void axs_hardware_init(void);

#ifdef __cplusplus
}
#endif

#endif	/* _AXS_HARDWARE_H_ */
