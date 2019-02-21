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

#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "dev_pinmux.h"
#include "hsdk/hsdk.h"

#define GPIO_MUX_CONFIG_NUM 8

typedef struct gpio_mux {
	uint8_t val;
	uint8_t type;
} GPIO_MUX;


static GPIO_MUX gpio_mux_config[GPIO_MUX_CONFIG_NUM] = {
	{0, PINMUX_TYPE_DEFAULT},
	{1, PINMUX_TYPE_DEFAULT},
	{0, PINMUX_TYPE_DEFAULT},
	{0, PINMUX_TYPE_DEFAULT},
	{0, PINMUX_TYPE_DEFAULT},
	{0, PINMUX_TYPE_DEFAULT},
	{0, PINMUX_TYPE_DEFAULT},
	{0, PINMUX_TYPE_DEFAULT},
};


int32_t io_mikro_config(uint32_t config)
{
	if (config == IO_PINMUX_ENABLE) {
		if (gpio_mux_config[1].type == PINMUX_TYPE_MIKRO &&
		gpio_mux_config[3].type == PINMUX_TYPE_MIKRO &&
		 gpio_mux_config[4].type == PINMUX_TYPE_MIKRO &&
		 gpio_mux_config[5].type == PINMUX_TYPE_MIKRO &&
		 gpio_mux_config[7].type == PINMUX_TYPE_MIKRO) {
			return E_OK; /*mikrobus already enabled */
		}

		if (gpio_mux_config[1].type == PINMUX_TYPE_DEFAULT &&
		 gpio_mux_config[3].type == PINMUX_TYPE_DEFAULT &&
		 gpio_mux_config[4].type == PINMUX_TYPE_DEFAULT &&
		 gpio_mux_config[5].type == PINMUX_TYPE_DEFAULT &&
		 gpio_mux_config[7].type == PINMUX_TYPE_DEFAULT) {
			gpio_mux_config[1].val = 0;
			gpio_mux_config[3].val = 2;
			gpio_mux_config[4].val = 4;
			gpio_mux_config[5].val = 3;
			gpio_mux_config[7].val = 1;
			gpio_mux_config[1].type = PINMUX_TYPE_MIKRO;
			gpio_mux_config[3].type = PINMUX_TYPE_MIKRO;
			gpio_mux_config[4].type = PINMUX_TYPE_MIKRO;
			gpio_mux_config[5].type = PINMUX_TYPE_MIKRO;
			gpio_mux_config[7].type = PINMUX_TYPE_MIKRO;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 1,
			0);
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 3,
			2);
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 4,
			4);
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 5,
			3);
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 7,
			1);
		} else {
			return E_OPNED; /* alread used by other type */
		}
	} else if (config == IO_PINMUX_DISABLE) {
		if (gpio_mux_config[1].type == PINMUX_TYPE_MIKRO &&
		gpio_mux_config[3].type == PINMUX_TYPE_MIKRO &&
		 gpio_mux_config[4].type == PINMUX_TYPE_MIKRO &&
		 gpio_mux_config[5].type == PINMUX_TYPE_MIKRO &&
		 gpio_mux_config[7].type == PINMUX_TYPE_MIKRO) {
			gpio_mux_config[1].val = 1;
			gpio_mux_config[3].val = 0;
			gpio_mux_config[4].val = 0;
			gpio_mux_config[5].val = 0;
			gpio_mux_config[7].val = 0;
			gpio_mux_config[1].type = PINMUX_TYPE_DEFAULT;
			gpio_mux_config[3].type = PINMUX_TYPE_DEFAULT;
			gpio_mux_config[4].type = PINMUX_TYPE_DEFAULT;
			gpio_mux_config[5].type = PINMUX_TYPE_DEFAULT;
			gpio_mux_config[7].type = PINMUX_TYPE_DEFAULT;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 1,
			1);
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 3,
			0);
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 4,
			0);
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 5,
			0);
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 7,
			0);
		} else {
			return E_OPNED; /* already used by other type, or not config */
		}
	} else {
		return E_NOSPT;
	}

	return E_OK;
}

static int32_t io_arduino_config_pwm(uint32_t num, uint32_t config)
{
	uint32_t index = 0;

	switch (num) {
	case ARDUINO_PIN_3:
		index = 4;
		break;
	case ARDUINO_PIN_10:
	case ARDUINO_PIN_11:
		index = 3;
		break;
	case ARDUINO_PIN_6:
		index = 6;
		break;
	case ARDUINO_PIN_5:
	case ARDUINO_PIN_9:
		index = 2;
		break;
	default:
		return E_PAR;
		break;
	}

	if (config == IO_PINMUX_ENABLE) {
		if (gpio_mux_config[index].type == PINMUX_TYPE_DEFAULT ||
		gpio_mux_config[index].type == PINMUX_TYPE_ARDUINO) {
			if (gpio_mux_config[index].val != 0 &&
				gpio_mux_config[index].val != 6) {
				return E_OPNED;
			}
			gpio_mux_config[index].type = PINMUX_TYPE_ARDUINO;
			gpio_mux_config[index].val = 6;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, index,
			6);
		} else {
			return E_OPNED;
		}

	} else if (config == IO_PINMUX_DISABLE) {
		if (gpio_mux_config[index].type == PINMUX_TYPE_ARDUINO) {
			gpio_mux_config[index].type = PINMUX_TYPE_DEFAULT;
			gpio_mux_config[index].val = 0;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, index,
			0);
		} else {
			return E_OPNED;
		}
	} else {
		return E_NOSPT;
	}

	return E_OK;
}

static int32_t io_arduino_config_gpio(uint32_t num, uint32_t config)
{
	uint32_t index = 0;

	switch (num) {
		case ARDUINO_PIN_0:
		case ARDUINO_PIN_1:
			index = 7;
			break;
		case ARDUINO_PIN_2:
		case ARDUINO_PIN_3:
			index = 4;
			break;
		case ARDUINO_PIN_4:
		case ARDUINO_PIN_5:
		case ARDUINO_PIN_8:
		case ARDUINO_PIN_9:
			index = 2;
			break;
		case ARDUINO_PIN_6:
		case ARDUINO_PIN_7:
			index = 6;
			break;
		case ARDUINO_PIN_10:
		case ARDUINO_PIN_11:
		case ARDUINO_PIN_12:
		case ARDUINO_PIN_13:
			index = 3;
			break;
		case ARDUINO_PIN_AD4:
		case ARDUINO_PIN_AD5:
			index = 5;
			break;
		default :
			return E_PAR;
	}

	if (config == IO_PINMUX_ENABLE) {
		if (gpio_mux_config[index].type == PINMUX_TYPE_DEFAULT ||
		gpio_mux_config[index].type == PINMUX_TYPE_ARDUINO) {
			if (gpio_mux_config[index].val != 0) {
				return E_OPNED;
			}
			gpio_mux_config[index].type = PINMUX_TYPE_ARDUINO;
			gpio_mux_config[index].val = 0;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, index,
			0);
		} else {
			return E_OPNED;
		}

	} else if (config == IO_PINMUX_DISABLE) {
		if (gpio_mux_config[index].type == PINMUX_TYPE_ARDUINO) {
			gpio_mux_config[index].type = PINMUX_TYPE_DEFAULT;
			gpio_mux_config[index].val = 0;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, index,
			0);
		} else {
			return E_OPNED;
		}
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
	default:
		return E_NOSPT;
	}

	return ret;
}

int32_t io_arduino_config_spi(uint32_t config)
{
	if (config == IO_PINMUX_ENABLE) {
		if (gpio_mux_config[3].type == PINMUX_TYPE_DEFAULT ||
		gpio_mux_config[3].type == PINMUX_TYPE_ARDUINO) {
			if (gpio_mux_config[3].val != 0 &&
				gpio_mux_config[3].val != 2) {
				return E_OPNED;
			}
			gpio_mux_config[3].type = PINMUX_TYPE_ARDUINO;
			gpio_mux_config[3].val = 2;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 3,
			2);
		} else {
			return E_OPNED;
		}

	} else if (config == IO_PINMUX_DISABLE) {
		if (gpio_mux_config[3].type == PINMUX_TYPE_ARDUINO) {
			gpio_mux_config[3].type = PINMUX_TYPE_DEFAULT;
			gpio_mux_config[3].val = 0;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 3,
			0);
		} else {
			return E_OPNED;
		}
	} else {
		return E_NOSPT;
	}

	return E_OK;
}

int32_t io_arduino_config_uart(uint32_t config)
{
	if (config == IO_PINMUX_ENABLE) {
		if (gpio_mux_config[7].type == PINMUX_TYPE_DEFAULT ||
		gpio_mux_config[7].type == PINMUX_TYPE_ARDUINO) {
			if (gpio_mux_config[7].val != 0 &&
				gpio_mux_config[7].val != 1) {
				return E_OPNED;
			}
			gpio_mux_config[7].type = PINMUX_TYPE_ARDUINO;
			gpio_mux_config[7].val = 1;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 7,
			1);
		} else {
			return E_OPNED;
		}

	} else if (config == IO_PINMUX_DISABLE) {
		if (gpio_mux_config[7].type == PINMUX_TYPE_ARDUINO) {
			gpio_mux_config[7].type = PINMUX_TYPE_DEFAULT;
			gpio_mux_config[7].val = 0;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 7,
			0);
		} else {
			return E_OPNED;
		}
	} else {
		return E_NOSPT;
	}

	return E_OK;
}

int32_t io_arduino_config_i2c(uint32_t config)
{
	if (config == IO_PINMUX_ENABLE) {
		if (gpio_mux_config[5].type == PINMUX_TYPE_DEFAULT ||
		gpio_mux_config[5].type == PINMUX_TYPE_ARDUINO) {
			if (gpio_mux_config[5].val != 0 &&
				gpio_mux_config[5].val != 3) {
				return E_OPNED;
			}
			gpio_mux_config[5].type = PINMUX_TYPE_ARDUINO;
			gpio_mux_config[5].val = 3;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 5,
			3);
		} else {
			return E_OPNED;
		}

	} else if (config == IO_PINMUX_DISABLE) {
		if (gpio_mux_config[5].type == PINMUX_TYPE_ARDUINO) {
			gpio_mux_config[5].type = PINMUX_TYPE_DEFAULT;
			gpio_mux_config[5].val = 0;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, 5,
			0);
		} else {
			return E_OPNED;
		}
	} else {
		return E_NOSPT;
	}

	return E_OK;
}

int32_t io_pmod_config(uint32_t pmod, uint32_t type, uint32_t config)
{
	uint32_t index1 = 0;
	uint32_t index2 = 0;

	switch (pmod) {
	case PMOD_A:
		index1 = 2;
		index2 = 6;
		break;
	case PMOD_B:
		index1 = 3;
		index2 = 7;
		break;
	case PMOD_C:
		if (type > PMOD_I2C) {
			return E_PAR; /* PMOD_C does not support PWM */
		}
		index1 = 4;
		index2 = 5;
	default:
		return E_PAR;
	}

	if (config == IO_PINMUX_ENABLE) {
		if ((gpio_mux_config[index1].type == PINMUX_TYPE_DEFAULT &&
			gpio_mux_config[index2].type == PINMUX_TYPE_DEFAULT) ||
			(gpio_mux_config[index1].type == PINMUX_TYPE_PMOD &&
			 gpio_mux_config[index2].type == PINMUX_TYPE_PMOD)) {
				gpio_mux_config[index1].type = PINMUX_TYPE_PMOD;
				gpio_mux_config[index1].val = type;
				gpio_mux_config[index2].type = PINMUX_TYPE_PMOD;
				gpio_mux_config[index2].val = 0;
				creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, index1,
				type);
				creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, index2,
				0);
			} else {
				return E_OPNED;
			}

	} else if (config == IO_PINMUX_DISABLE) {
		if (gpio_mux_config[index1].type == PINMUX_TYPE_ARDUINO &&
		   gpio_mux_config[index2].type == PINMUX_TYPE_ARDUINO) {
			gpio_mux_config[index1].type = PINMUX_TYPE_DEFAULT;
			gpio_mux_config[index1].val = 0;
			gpio_mux_config[index2].type = PINMUX_TYPE_DEFAULT;
			gpio_mux_config[index2].val = 0;
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, index1,
			0);
			creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, index2,
			0);
		} else {
			return E_OPNED;
		}
	} else {
		return E_NOSPT;
	}

	return E_OK;
}

void io_mux_init(void)
{
	int32_t i;

	for (i = 0; i < GPIO_MUX_CONFIG_NUM; i++) {
		creg_hsdc_set_gpio_mux((CREG_HSDC_STRUCT *)HSDC_CREG_REGBASE, i,
			gpio_mux_config[i].val);
	}
}
