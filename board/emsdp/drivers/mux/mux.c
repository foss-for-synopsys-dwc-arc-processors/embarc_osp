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
#include "dev_pinmux.h"
#include "mux_hal.h"
#include "mux.h"
#include "embARC_error.h"
#include "arc.h"
#include "arc_builtin.h"
#include "emsdp/emsdp.h"

static MUX_REG *mux_regs;

/** set PMOD muxer scheme */
void set_pmod_mux(uint32_t val)
{
	mux_regs[PMOD_MUX_CTRL] = val;
}

/** get PMOD muxer scheme */
uint32_t get_pmod_mux(void)
{
	return (uint32_t) mux_regs[PMOD_MUX_CTRL];
}

/** set arduino connection scheme */
void set_arduino_mux(uint32_t val)
{
	mux_regs[ARDUINO_MUX_CTRL] = val;
}

/** get arduino connection scheme */
uint32_t get_arduino_mux(void)
{
	return (uint32_t) mux_regs[ARDUINO_MUX_CTRL];
}

/** set generic connection scheme */
void set_generic_mux(uint32_t val)
{
	mux_regs[GENERIC_MUX_CTRL] = val;
}

/** get generic connection scheme */
uint32_t get_generic_mux(void)
{
	return (uint32_t) mux_regs[GENERIC_MUX_CTRL];
}

void io_mux_init(void)
{
	mux_regs = (MUX_REG *)(EMSDP_CREG_BASE | EMSDP_CREG_PMOD_MUX_OFFSET);
	// Initialize Mux controller registers by default values
	set_pmod_mux(PMOD_MUX_CTRL_DEFAULT);
	set_arduino_mux(ARDUINO_MUX_CTRL_DEFAULT);
	set_generic_mux(GENERIC_MUX_CTRL_DEFAULT);

	/**
	 * + Please refer to corresponding EMSDP User Guide for detailed information
	 * + Set up pin-multiplexer of all PMOD connections
	 *   - PM_A: GPIO, I2C, SPI, Type3 Uart1, Type4 Uart1, PWM_1, PWM_2
	 *   - PM_B: GPIO, I2C, SPI, Type3 Uart2, Type4 Uart2, PWM_1, PWM_2
	 *   - PM_C: GPIO, I2C, SPI, Type3 Uart3, Type4 Uart3, PWM_1, PWM_2
	 */
	set_pmod_mux(PM_A_CFG0_SPI	\
			|	PM_A_CFG1_GPIO	\
			|	PM_B_CFG0_I2C	\
			|	PM_B_CFG1_GPIO	\
			|	PM_C_CFG0_GPIO	\
			|	PM_C_CFG1_GPIO );

	set_arduino_mux(ARDUINO_CFG0_GPIO	\
				|	ARDUINO_CFG1_GPIO	\
				|	ARDUINO_CFG2_GPIO	\
				|	ARDUINO_CFG3_GPIO	\
				|	ARDUINO_CFG4_GPIO	\
				|	ARDUINO_CFG5_SPI	\
				|	ARDUINO_CFG6_I2C	);
}

