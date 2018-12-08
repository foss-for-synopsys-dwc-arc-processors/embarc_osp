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
#ifndef _MUX_H_
#define _MUX_H_


#include "embARC_toolchain.h"
#include "dev_pinmux.h"

#define MUX_SEL0_OFFSET		(0)
#define MUX_SEL1_OFFSET		(4)
#define MUX_SEL2_OFFSET		(8)
#define MUX_SEL3_OFFSET		(12)
#define MUX_SEL4_OFFSET		(16)
#define MUX_SEL5_OFFSET		(20)
#define MUX_SEL6_OFFSET		(24)
#define MUX_SEL7_OFFSET		(28)

#define MUX_SEL0_MASK		(0xf << MUX_SEL0_OFFSET)
#define MUX_SEL1_MASK		(0xf << MUX_SEL1_OFFSET)
#define MUX_SEL2_MASK		(0xf << MUX_SEL2_OFFSET)
#define MUX_SEL3_MASK		(0xf << MUX_SEL3_OFFSET)
#define MUX_SEL4_MASK		(0xf << MUX_SEL4_OFFSET)
#define MUX_SEL5_MASK		(0xf << MUX_SEL5_OFFSET)
#define MUX_SEL6_MASK		(0xf << MUX_SEL6_OFFSET)
#define MUX_SEL7_MASK		(0xf << MUX_SEL7_OFFSET)

/**
 * Default pin muxer settings
 */
#define PMOD_MUX_CTRL_DEFAULT		(0)		/*!< all pins are configured as GPIO inputs as default */
#define ARDUINO_MUX_CTRL_DEFAULT	(0)		/*!< Default Arduino mode */
#define GENERIC_MUX_CTRL_DEFAULT	(0)		/*!< Default generic mux mode */

/**
 * PMOD A Multiplexor
 */
#define PM_A_CFG0_GPIO		((0)<< MUX_SEL0_OFFSET)
#define PM_A_CFG0_I2C		((1)<< MUX_SEL0_OFFSET)		//io_i2c_mst2
#define PM_A_CFG0_SPI		((2)<< MUX_SEL0_OFFSET)		//io_spi_mst1, cs_0
#define PM_A_CFG0_UART1a	((3)<< MUX_SEL0_OFFSET)		//io_uart1
#define PM_A_CFG0_UART1b	((4)<< MUX_SEL0_OFFSET)		//io_uart1
#define PM_A_CFG0_PWM1		((5)<< MUX_SEL0_OFFSET)
#define PM_A_CFG0_PWM2		((6)<< MUX_SEL0_OFFSET)

#define PM_A_CFG1_GPIO		((0)<< MUX_SEL1_OFFSET)


/**
 * PMOD B Multiplexor
 */
#define PM_B_CFG0_GPIO		((0)<< MUX_SEL2_OFFSET)
#define PM_B_CFG0_I2C		((1)<< MUX_SEL2_OFFSET)		//io_i2c_mst2
#define PM_B_CFG0_SPI		((2)<< MUX_SEL2_OFFSET)		//io_spi_mst1, cs_1
#define PM_B_CFG0_UART2a	((3)<< MUX_SEL2_OFFSET)		//io_uart2
#define PM_B_CFG0_UART2b	((4)<< MUX_SEL2_OFFSET)		//io_uart2
#define PM_B_CFG0_PWM1		((5)<< MUX_SEL2_OFFSET)
#define PM_B_CFG0_PWM2		((6)<< MUX_SEL2_OFFSET)

#define PM_B_CFG1_GPIO		((0)<< MUX_SEL3_OFFSET)


/**
 * PMOD C Multiplexor
 */
#define PM_C_CFG0_GPIO		((0)<< MUX_SEL4_OFFSET)
#define PM_C_CFG0_I2C		((1)<< MUX_SEL4_OFFSET)		//io_i2c_mst2
#define PM_C_CFG0_SPI		((2)<< MUX_SEL4_OFFSET)		//io_spi_mst1, cs_2
#define PM_C_CFG0_UART3a	((3)<< MUX_SEL4_OFFSET)		//io_uart3
#define PM_C_CFG0_UART3b	((4)<< MUX_SEL4_OFFSET)		//io_uart3
#define PM_C_CFG0_PWM1		((5)<< MUX_SEL4_OFFSET)
#define PM_C_CFG0_PWM2		((6)<< MUX_SEL4_OFFSET)

#define PM_C_CFG1_GPIO		((0)<< MUX_SEL5_OFFSET)

/**
 * Arduino Multiplexor
 */
#define ARDUINO_CFG0_GPIO		((0)<< MUX_SEL0_OFFSET)
#define ARDUINO_CFG0_UART		((1)<< MUX_SEL0_OFFSET)		//io_uart0

#define ARDUINO_CFG1_GPIO		((0)<< MUX_SEL1_OFFSET)
#define ARDUINO_CFG1_PWM		((1)<< MUX_SEL1_OFFSET)

#define ARDUINO_CFG2_GPIO		((0)<< MUX_SEL2_OFFSET)
#define ARDUINO_CFG2_PWM		((1)<< MUX_SEL2_OFFSET)

#define ARDUINO_CFG3_GPIO		((0)<< MUX_SEL3_OFFSET)
#define ARDUINO_CFG3_PWM		((1)<< MUX_SEL3_OFFSET)

#define ARDUINO_CFG4_GPIO		((0)<< MUX_SEL4_OFFSET)
#define ARDUINO_CFG4_PWM		((1)<< MUX_SEL4_OFFSET)

#define ARDUINO_CFG5_GPIO		((0)<< MUX_SEL5_OFFSET)
#define ARDUINO_CFG5_SPI		((1)<< MUX_SEL5_OFFSET)		//io_spi_mst0, cs_0
#define ARDUINO_CFG5_PWM1		((2)<< MUX_SEL5_OFFSET)
#define ARDUINO_CFG5_PWM2		((3)<< MUX_SEL5_OFFSET)
#define ARDUINO_CFG5_PWM3		((4)<< MUX_SEL5_OFFSET)

#define ARDUINO_CFG6_GPIO		((0)<< MUX_SEL6_OFFSET)
#define ARDUINO_CFG6_I2C		((1)<< MUX_SEL6_OFFSET)		//io_i2c_mst1

/**
 * Generic  Multiplexor
 * GPIO | EBI | Host-IF | DBG
 */
#define GENERIC_CFG0_GPIO		((0)<< MUX_SEL0_OFFSET)
#define GENERIC_CFG0_EBI		((1)<< MUX_SEL0_OFFSET)
#define GENERIC_CFG0_HOSTIF		((2)<< MUX_SEL0_OFFSET)
#define GENERIC_CFG0_DBG		((3)<< MUX_SEL0_OFFSET)

typedef volatile uint32_t MUX_REG;

#ifdef __cplusplus
extern "C" {
#endif

extern void set_pmod_mux(uint32_t val);
extern uint32_t get_pmod_mux(void);
extern void set_arduino_mux(uint32_t val);
extern uint32_t get_arduino_mux(void);
extern void set_generic_mux(uint32_t val);
extern uint32_t get_generic_mux(void);

#ifdef __cplusplus
}
#endif

#endif /* _MUX_H_ */
