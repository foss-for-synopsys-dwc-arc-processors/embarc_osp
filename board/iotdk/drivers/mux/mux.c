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

#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "iotdk/iotdk.h"

#define MIKRO_MUX_CONFIG  0x13

int32_t io_mikro_config(uint32_t config)
{
	if (config == IO_PINMUX_ENABLE) {
		sysconf_reg_ptr->ARDUINO_MUX |= MIKRO_MUX_CONFIG;
	} else if (config == IO_PINMUX_DISABLE) {
		sysconf_reg_ptr->ARDUINO_MUX &= ~MIKRO_MUX_CONFIG;
	} else {
		return E_NOSPT;
	}

	return E_OK;
}

static int32_t io_arduino_config_pwm(uint32_t num, uint32_t config)
{
	uint32_t val = 0;

	switch (num) {
	case ARDUINO_PIN_3:
		val = ARDUINO_MUX_PWM0;
		break;
	case ARDUINO_PIN_5:
		val = ARDUINO_MUX_PWM1;
		break;
	case ARDUINO_PIN_6:
		val = ARDUINO_MUX_PWM2;
		break;
	case ARDUINO_PIN_9:
		val = ARDUINO_MUX_PWM3;
		break;
	case ARDUINO_PIN_10:
		sysconf_reg_ptr->ARDUINO_MUX &= ~ARDUINO_MUX_SPI;
		val = ARDUINO_MUX_PWM4;
		break;
	case ARDUINO_PIN_11:
		sysconf_reg_ptr->ARDUINO_MUX &= ~ARDUINO_MUX_SPI;
		val = ARDUINO_MUX_PWM5;
		break;
	default:
		return E_PAR;
		break;
	}

	if (config == IO_PINMUX_ENABLE) {
		sysconf_reg_ptr->ARDUINO_MUX |= val;
	} else if (config == IO_PINMUX_DISABLE) {
		sysconf_reg_ptr->ARDUINO_MUX &= ~val;
	} else {
		return E_NOSPT;
	}

	return E_OK;
}

static int32_t io_arduino_config_gpio(uint32_t num, uint32_t config)
{
	uint32_t val = 0;

	if (config != IO_PINMUX_DISABLE && config != IO_PINMUX_ENABLE) {
		return E_NOSPT;
	}

	switch (num) {
	case ARDUINO_PIN_0:
	case ARDUINO_PIN_1:
		val = ARDUINO_MUX_UART;
		break;
	case ARDUINO_PIN_2:
	case ARDUINO_PIN_4:
	case ARDUINO_PIN_8:
	case ARDUINO_PIN_7:
		val = 0;
		break;
	case ARDUINO_PIN_3:
		val = ARDUINO_MUX_PWM0;
		break;
	case ARDUINO_PIN_5:
		val = ARDUINO_MUX_PWM1;
		break;
	case ARDUINO_PIN_6:
		val = ARDUINO_MUX_PWM2;
		break;
	case ARDUINO_PIN_9:
		val = ARDUINO_MUX_PWM3;
		break;
	case ARDUINO_PIN_10:
		val = ARDUINO_MUX_PWM4 | ARDUINO_MUX_SPI;
		break;
	case ARDUINO_PIN_11:
		val = ARDUINO_MUX_PWM5 | ARDUINO_MUX_SPI;
		break;
	case ARDUINO_PIN_12:
	case ARDUINO_PIN_13:
		val = ARDUINO_MUX_SPI;
		break;
	case ARDUINO_PIN_AD0:
		val = ARDUINO_MUX_ADC0;
		break;
	case ARDUINO_PIN_AD1:
		val = ARDUINO_MUX_ADC1;
		break;
	case ARDUINO_PIN_AD2:
		val = ARDUINO_MUX_ADC2;
		break;
	case ARDUINO_PIN_AD3:
		val = ARDUINO_MUX_ADC3;
		break;
	case ARDUINO_PIN_AD4:
	case ARDUINO_PIN_AD5:
		val = ARDUINO_MUX_I2C;
		break;
	default :
		return E_PAR;
	}

	sysconf_reg_ptr->ARDUINO_MUX &= ~val;

	return E_OK;
}

static int32_t io_arduino_config_adc(uint32_t pin_num, uint32_t config)
{
	uint32_t val = 0;

	switch (pin_num) {

	case ARDUINO_PIN_AD0:
		val = ARDUINO_MUX_ADC0;
		break;
	case ARDUINO_PIN_AD1:
		val = ARDUINO_MUX_ADC1;
		break;
	case ARDUINO_PIN_AD2:
		val = ARDUINO_MUX_ADC2;
		break;
	case ARDUINO_PIN_AD3:
		val = ARDUINO_MUX_ADC3;
		break;
	case ARDUINO_PIN_AD4:
		sysconf_reg_ptr->ARDUINO_MUX &= ~ARDUINO_MUX_I2C;
		val = ARDUINO_MUX_ADC4;
		break;
	case ARDUINO_PIN_AD5:
		sysconf_reg_ptr->ARDUINO_MUX &= ~ARDUINO_MUX_I2C;
		val = ARDUINO_MUX_ADC5;
		break;
	default :
		return E_PAR;
	}

	if (config == IO_PINMUX_ENABLE) {
		sysconf_reg_ptr->ARDUINO_MUX |= val;
	} else if (config == IO_PINMUX_DISABLE) {
		sysconf_reg_ptr->ARDUINO_MUX &= ~val;
	} else {
		return E_NOSPT;
	}

	return E_OK;
}

int32_t io_arduino_config(uint32_t pin_num, uint32_t type, uint32_t config)
{
	int32_t ret = E_PAR;

	switch (type) {
	case ARDUINO_GPIO:
		ret = io_arduino_config_gpio(pin_num, config);
		break;
	case ARDUINO_PWM:
		ret = io_arduino_config_pwm(pin_num, config);
		break;
	case ARDUINO_SPI:
		if (pin_num == ARDUINO_PIN_10 ||
				pin_num == ARDUINO_PIN_11 ||
				pin_num == ARDUINO_PIN_12 ||
				pin_num == ARDUINO_PIN_13) {
			ret = io_arduino_config_spi(config);
		}
		break;
	case ARDUINO_I2C:
		if (pin_num == ARDUINO_PIN_AD5 ||
				pin_num == ARDUINO_PIN_AD4) {
			ret = io_arduino_config_i2c(config);
		}
		break;
	case ARDUINO_UART:
		if (pin_num == ARDUINO_PIN_0 ||
				pin_num == ARDUINO_PIN_1) {
			ret = io_arduino_config_uart(config);
		}
		break;
	case ARDUINO_ADC:
		if (pin_num >= ARDUINO_PIN_AD0 && pin_num <= ARDUINO_PIN_AD5) {
			ret = io_arduino_config_adc(pin_num, config);
		}
		break;
	default:
		return E_NOSPT;
	}

	return ret;
}

int32_t io_arduino_config_spi(uint32_t config)
{
	if (config == IO_PINMUX_ENABLE) {
		sysconf_reg_ptr->ARDUINO_MUX |= ARDUINO_MUX_SPI;
	} else if (config == IO_PINMUX_DISABLE) {
		sysconf_reg_ptr->ARDUINO_MUX &= ~ARDUINO_MUX_SPI;
	} else {
		return E_NOSPT;
	}

	return E_OK;
}

int32_t io_arduino_config_uart(uint32_t config)
{
	if (config == IO_PINMUX_ENABLE) {
		sysconf_reg_ptr->ARDUINO_MUX |= ARDUINO_MUX_UART;
	} else if (config == IO_PINMUX_DISABLE) {
		sysconf_reg_ptr->ARDUINO_MUX &= ~ARDUINO_MUX_UART;
	} else {
		return E_NOSPT;
	}

	return E_OK;
}

int32_t io_arduino_config_i2c(uint32_t config)
{
	if (config == IO_PINMUX_ENABLE) {
		sysconf_reg_ptr->ARDUINO_MUX |= ARDUINO_MUX_I2C;
	} else if (config == IO_PINMUX_DISABLE) {
		sysconf_reg_ptr->ARDUINO_MUX &= ~ARDUINO_MUX_I2C;
	} else {
		return E_NOSPT;
	}

	return E_OK;
}

int32_t io_pmod_config(uint32_t pmod, uint32_t type, uint32_t config)
{
	if (config != IO_PINMUX_DISABLE && config != IO_PINMUX_ENABLE) {
		return E_NOSPT;
	}

	switch (pmod) {
	case PMOD_A:
		return E_PAR; /* no pmod A on iotdk */
	case PMOD_B:
		if (type == PMOD_SPI && config == IO_PINMUX_ENABLE) {
			sysconf_reg_ptr->PMOD_MUX &= ~PMOD_MUX_PMB;
		} else if (type == PMOD_GPIO || config == IO_PINMUX_DISABLE) {
			sysconf_reg_ptr->PMOD_MUX |= PMOD_MUX_PMB;
		} else {
			return E_PAR;
		}
		break;
	case PMOD_C:
		if (type == PMOD_UART && config == IO_PINMUX_ENABLE) {
			sysconf_reg_ptr->PMOD_MUX &= ~PMOD_MUX_PMC;
		} else if (type == PMOD_GPIO || config == IO_PINMUX_DISABLE) {
			sysconf_reg_ptr->PMOD_MUX |= PMOD_MUX_PMC;
		} else {
			return E_PAR;
		}
	default:
		return E_PAR;
	}

	return E_OK;
}

void io_mux_init(void)
{
	sysconf_reg_ptr->PMOD_MUX = 0;
	sysconf_reg_ptr->ARDUINO_MUX = 0;
}
