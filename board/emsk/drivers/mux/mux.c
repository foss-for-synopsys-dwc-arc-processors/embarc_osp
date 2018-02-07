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
/**
 * \defgroup	BOARD_EMSK_DRV_MUX	EMSK Mux Driver
 * \ingroup	BOARD_EMSK_DRIVER
 * \brief	EMSK Mux Controller Driver
 * \details
 *		Mux controller is the hardware external PMOD port pin connection
 *	controller, it can distribute the external port pins into different
 *	functions like general input/output, spi, iic, uart and so on.
 */

/**
 * \file
 * \ingroup	BOARD_EMSK_DRV_MUX
 * \brief	emsk mux controller driver
 */

/**
 * \addtogroup	BOARD_EMSK_DRV_MUX
 * @{
 */
#include "dev_pinmux.h"
#include "mux_hal.h"
#include "mux.h"
#include "embARC_error.h"
#include "arc.h"
#include "arc_builtin.h"
#include "../../emsk.h"

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

/** set SPI connection scheme */
void set_spi_map(uint32_t val)
{
	mux_regs[SPI_MAP_CTRL] = val;
}

/** set UART connection scheme */
void set_uart_map(uint32_t val)
{
	mux_regs[UART_MAP_CTRL] = val;
}

int32_t io_mikro_config(uint32_t config)
{
	/* no mikro interface */
	return E_NOSPT;
}

int32_t io_arduino_config_spi(uint32_t config)
{
	/* no arduino interface */
	return E_NOSPT;
}

int32_t io_arduino_config_uart(uint32_t config)
{
	/* no arduino interface */
	return E_NOSPT;
}

int32_t io_arduino_config_i2c(uint32_t config)
{
	/* no arduino interface */
	return E_NOSPT;
}

int32_t io_arduino_config(uint32_t pin_num, uint32_t type, uint32_t config)
{
	/* no arduino interface */
	return E_NOSPT;
}


int32_t io_pmod_config(uint32_t pmod, uint32_t type, uint32_t config)
{
	uint32_t val = get_pmod_mux();
	switch (pmod) {
	case PMOD_1: /* PMOD GPIO, PMOD_UART */
		if (type == PMOD_GPIO) {
			val &= ~(PM1_UR_UART_0 | PM1_LR_SPI_S);
		} else if (type == PMOD_UART) {
			val |= PM1_UR_UART_0;
		} else {
			return E_NOSPT;
		}
		break;
	case PMOD_2: /* PMOD GPIO, PMOD_I2C */
	case PMOD_3:
	case PMOD_4:
		pmod--;
		pmod <<= 2; /* (pmode - 1) * 4*/
		if (type == PMOD_GPIO) {
			val &= ~(1 << pmod);
		} else if (type == PMOD_I2C) {
			val |= (1 << pmod);
		} else {
			return E_NOSPT;
		}
		break;
	case PMOD_5: /* PMOD GPIO, PMOD_SPI */
		if (type == PMOD_GPIO) {
			val &= ~(PM5_UR_SPI_M1);
		} else if (type == PMOD_SPI) {
			val |= PM5_UR_SPI_M1;
		} else {
			return E_NOSPT;
		}
		break;
	case PMOD_6: /* PMOD GPIO, PMOD_SPI */
		if (type == PMOD_GPIO) {
			val &= ~(PM6_UR_SPI_M0);
		} else if (type == PMOD_SPI) {
			val |= PM6_UR_SPI_M0;
		} else {
			return E_NOSPT;
		}
		break;
	default:
		return E_PAR;
	}

	set_pmod_mux(val);

	return E_OK;
}

void io_mux_init(void)
{
	mux_regs = (MUX_REG *)(PERIPHERAL_BASE | REL_REGBASE_PINMUX);
	// Initialize Mux controller registers by default values
	mux_regs[PMOD_MUX_CTRL] = PMOD_MUX_CTRL_DEFAULT;
	mux_regs[SPI_MAP_CTRL] =  SPI_MAP_CTRL_DEFAULT;
	mux_regs[UART_MAP_CTRL] = UART_MAP_CTRL_DEFAULT;

	/**
	 * + Please refer to corresponding EMSK User Guide for detailed information
	 *   -> Appendix: A  Hardware Functional Description
	 *      -> Pmods Configuration summary
	 * + Set up pin-multiplexer of all PMOD connections
	 *   - PM1 J1: Upper row as UART 0, lower row as SPI Slave
	 *   - PM2 J2: IIC 0 and run/halt signals
	 *   - PM3 J3: GPIO Port A and Port C
	 *   - PM4 J4: IIC 1 and Port D
	 *   - PM5 J5: Upper row as SPI Master, lower row as Port A
	 *   - PM6 J6: Upper row as SPI Master, lower row as Port A
	 */
	set_pmod_mux(PM1_UR_UART_0 | PM1_LR_SPI_S	\
				| PM2_I2C_HRI		\
				| PM3_GPIO_AC		\
				| PM4_I2C_GPIO_D	\
				| PM5_UR_SPI_M1 | PM5_LR_GPIO_A	\
				| PM6_UR_SPI_M0 | PM6_LR_GPIO_A );

	/**
	 * PM1 upper row as UART
	 * UM4:RXD, UM3:TXD
	 * UM2:RTS_N, UM1:CTS_N
	 */
	set_uart_map(0xe4);
}

/** @} end of group BOARD_EMSK_DRV_MUX */
