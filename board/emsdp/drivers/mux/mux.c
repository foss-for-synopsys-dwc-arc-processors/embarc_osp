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

int32_t io_pmod_config(uint32_t pmod, uint32_t type, uint32_t config)
{
	uint32_t reg = get_pmod_mux();
	switch (pmod){
		case PMOD_A:
			reg &= ~(MUX_SEL0_MASK);
			switch (type){
				case PMOD_GPIO:
					reg |= PM_A_CFG0_GPIO; break;
				case PMOD_UART:
					reg |= PM_A_CFG0_UART1b; break;//default on 4A type pmod uart
				case PMOD_SPI:
					reg |= PM_A_CFG0_SPI; break;
				case PMOD_I2C:
					reg |= PM_A_CFG0_I2C; break;
				case PMOD_PWM_MODE1:
					reg |= PM_A_CFG0_PWM1; break;
				case PMOD_PWM_MODE2:
					reg |= PM_A_CFG0_PWM2; break;
				default:
					return E_NOSPT; break;
			}
			break;
		case PMOD_B:
			reg &= ~(MUX_SEL2_MASK);
			switch (type){
				case PMOD_GPIO:
					reg |= PM_B_CFG0_GPIO; break;
				case PMOD_UART:
					reg |= PM_B_CFG0_UART2b; break;//default on 4A type pmod uart
				case PMOD_SPI:
					reg |= PM_B_CFG0_SPI; break;
				case PMOD_I2C:
					reg |= PM_B_CFG0_I2C; break;
				case PMOD_PWM_MODE1:
					reg |= PM_B_CFG0_PWM1; break;
				case PMOD_PWM_MODE2:
					reg |= PM_B_CFG0_PWM2; break;
				default:
					return E_NOSPT; break;
			}
			break;
		case PMOD_C:
			reg &= ~(MUX_SEL4_MASK);
			switch (type){
				case PMOD_GPIO:
					reg |= PM_C_CFG0_GPIO; break;
				case PMOD_UART:
					reg |= PM_C_CFG0_UART3b; break;//default on 4A type pmod uart
				case PMOD_SPI:
					reg |= PM_C_CFG0_SPI; break;
				case PMOD_I2C:
					reg |= PM_C_CFG0_I2C; break;
				case PMOD_PWM_MODE1:
					reg |= PM_C_CFG0_PWM1; break;
				case PMOD_PWM_MODE2:
					reg |= PM_C_CFG0_PWM2; break;
				default:
					return E_NOSPT; break;
			}
			break;
		default:
			return E_NOSPT;
			break;
	}
	set_pmod_mux(reg);
	return E_OK;
}

int32_t io_mikro_config(uint32_t config)
{
	/* mikro interface can only be accessed when Arduino mux register is set to zero */
	set_arduino_mux(ARDUINO_MUX_CTRL_DEFAULT);
	return E_OK;
}

int32_t io_arduino_config(uint32_t pin_num, uint32_t type, uint32_t config)
{
	uint32_t reg = get_arduino_mux();
	switch (pin_num){
		case ARDUINO_PIN_0:
		case ARDUINO_PIN_1:
			reg &= ~MUX_SEL0_MASK;
			if(type ==	ARDUINO_GPIO){
				reg |= ARDUINO_CFG0_GPIO;
			} else if(type == ARDUINO_UART){
				reg |= ARDUINO_CFG0_UART;
			} else {
				return E_NOSPT;
			}
			break;
		case ARDUINO_PIN_2:
		case ARDUINO_PIN_3:
			reg &= ~MUX_SEL1_MASK;
			if(type ==	ARDUINO_GPIO){
				reg |= ARDUINO_CFG1_GPIO;
			} else if(type == ARDUINO_PWM){
				reg |= ARDUINO_CFG1_PWM;
			} else {
				return E_NOSPT;
			}
			break;
		case ARDUINO_PIN_4:
		case ARDUINO_PIN_5:
			reg &= ~MUX_SEL2_MASK;
			if(type ==	ARDUINO_GPIO){
				reg |= ARDUINO_CFG2_GPIO;
			} else if(type == ARDUINO_PWM){
				reg |= ARDUINO_CFG2_PWM;
			} else {
				return E_NOSPT;
			}
			break;
		case ARDUINO_PIN_6:
		case ARDUINO_PIN_7:
			reg &= ~MUX_SEL3_MASK;
			if(type ==	ARDUINO_GPIO){
				reg |= ARDUINO_CFG3_GPIO;
			} else if(type == ARDUINO_PWM){
				reg |= ARDUINO_CFG3_PWM;
			} else {
				return E_NOSPT;
			}
			break;
		case ARDUINO_PIN_8:
		case ARDUINO_PIN_9:
			reg &= ~MUX_SEL4_MASK;
			if(type ==	ARDUINO_GPIO){
				reg |= ARDUINO_CFG4_GPIO;
			} else if(type == ARDUINO_PWM){
				reg |= ARDUINO_CFG4_PWM;
			} else {
				return E_NOSPT;
			}
			break;
		case ARDUINO_PIN_10:
		case ARDUINO_PIN_11:
		case ARDUINO_PIN_12:
		case ARDUINO_PIN_13:
			reg &= ~MUX_SEL5_MASK;
			if(type ==	ARDUINO_GPIO){
				reg |= ARDUINO_CFG5_GPIO;
			} else if(type == ARDUINO_SPI){
				reg |= ARDUINO_CFG5_SPI;
			} else if(type == ARDUINO_PWM){
				reg |= ARDUINO_CFG5_PWM1;//default on PWM1 mode
			} else {
				return E_NOSPT;
			}
			break;
		case ARDUINO_PIN_AD4:
		case ARDUINO_PIN_AD5:
			reg &= ~MUX_SEL6_MASK;
			if(type ==	ARDUINO_GPIO){
				reg |= ARDUINO_CFG6_GPIO;
			} else if(type == ARDUINO_I2C){
				reg |= ARDUINO_CFG6_I2C;
			} else {
				return E_NOSPT;
			}
			break;

		default:
			return E_NOSPT;
	}
	set_arduino_mux(reg);
	return E_OK;
}

int32_t io_arduino_config_spi(uint32_t config)
{
	uint32_t reg = get_arduino_mux();
	reg &= ~MUX_SEL5_MASK;
	reg |= ARDUINO_CFG5_SPI;
	set_arduino_mux(reg);
	return E_OK;
}

int32_t io_arduino_config_uart(uint32_t config)
{
	uint32_t reg = get_arduino_mux();
	reg &= ~MUX_SEL0_MASK;
	reg |= ARDUINO_CFG0_UART;
	set_arduino_mux(reg);
	return E_OK;
}

int32_t io_arduino_config_i2c(uint32_t config)
{
	uint32_t reg = get_arduino_mux();
	reg &= ~MUX_SEL6_MASK;
	reg |= ARDUINO_CFG6_I2C;
	set_arduino_mux(reg);
	return E_OK;
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
	set_pmod_mux(PM_A_CFG0_UART1b	\
			|	PM_A_CFG1_GPIO	\
			|	PM_B_CFG0_UART2b	\
			|	PM_B_CFG1_GPIO	\
			|	PM_C_CFG0_UART3b	\
			|	PM_C_CFG1_GPIO );

	set_arduino_mux(ARDUINO_CFG0_GPIO	\
				|	ARDUINO_CFG1_GPIO	\
				|	ARDUINO_CFG2_GPIO	\
				|	ARDUINO_CFG3_GPIO	\
				|	ARDUINO_CFG4_GPIO	\
				|	ARDUINO_CFG5_GPIO	\
				|	ARDUINO_CFG6_GPIO	);

	//Default value of Generic MUX is GENERIC_CFG0_GPIO
	//set_generic_mux(GENERIC_CFG0_GPIO);
}

