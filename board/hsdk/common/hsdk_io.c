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

#include "dw_gpio.h"
#include "hsdk_io.h"
#include "cy8c95xx.h"
#include "hsdk/hsdk.h"

#define HSDK_LED_GPORT_OFFSET			0

static CY8C95XX_DEF cy8c95xx_obj;

static void cy8c95xx_delay_us(uint32_t us)
{
	uint64_t start_us, us_delayed;

	us_delayed = (uint64_t)us;
	start_us = board_get_cur_us();
	while ((board_get_cur_us() - start_us) < us_delayed);
}

/** hsdk on-board led init, led default off */
void hsdk_led_init(void)
{
	int32_t ercd = E_OK;
	int32_t rv = 0;
	uint8_t buffer_tx[1];
	uint8_t val = 0;

	cy8c95xx_obj.i2c_id = HSDK_I2C_ID_CY8C95XX;
	cy8c95xx_obj.slvaddr_io = HSDK_I2C_ADDR_CY8C95XX_IO;
	cy8c95xx_obj.slvaddr_eep = HSDK_I2C_ADDR_CY8C95XX_EEP;

	DEV_IIC_PTR iic_obj = iic_get_dev(cy8c95xx_obj.i2c_id);
	ercd = iic_obj->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD);

	if ((ercd == E_OK) || (ercd == E_OPNED)) {
		iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(cy8c95xx_obj.slvaddr_io));
	}


	/* Select port */
	buffer_tx[0] = CY8C95XX_PORT_1;
	rv = cy8c95xx_reg_write(&cy8c95xx_obj, CY8C95XX_DEV_IO, CY8C95XX_PORT_SELECT_REG, buffer_tx, 1);
	cy8c95xx_delay_us(100);

	do {
		cy8c95xx_reg_read(&cy8c95xx_obj, CY8C95XX_DEV_IO, CY8C95XX_PORT_SELECT_REG, &val, 1);
	} while (val != buffer_tx[0]);
	/* Set direction: all pins output */
	buffer_tx[0] = 0x00;
	rv = cy8c95xx_reg_write(&cy8c95xx_obj, CY8C95XX_DEV_IO, CY8C95XX_PIN_DIRECTION_REG, buffer_tx, 1);
	cy8c95xx_delay_us(100);
	do {
		cy8c95xx_reg_read(&cy8c95xx_obj, CY8C95XX_DEV_IO, CY8C95XX_PIN_DIRECTION_REG, &val, 1);
	} while (val != buffer_tx[0]);
	cy8c95xx_delay_us(100);
	/* Set driver mode: Pull Up */
	cy8c95xx_reg_read(&cy8c95xx_obj, CY8C95XX_DEV_IO, CY8C95XX_DRIVE_MODE_PU_REG, &val, 1);
	buffer_tx[0] = 0xFF;
	rv = cy8c95xx_reg_write(&cy8c95xx_obj, CY8C95XX_DEV_IO, CY8C95XX_DRIVE_MODE_PU_REG, buffer_tx, 1);
	cy8c95xx_delay_us(100);

error_exit:
	return;
}

/** write 1 to light on led bit, else light off led */
void led_write(uint32_t led_val, uint32_t mask)
{
	led_val = (~led_val) & mask;
	cy8c95xx_writeport(&cy8c95xx_obj, CY8C95XX_PORT_1, (uint8_t)(led_val << HSDK_LED_GPORT_OFFSET));

error_exit:
	return;
}

/** read led value, on for 1, off for 0 */
uint32_t led_read(uint32_t mask)
{
	uint32_t value;

	cy8c95xx_readport(&cy8c95xx_obj, CY8C95XX_PORT_1, (uint8_t *)&value);
	value &= mask;

	return value;

error_exit:
	return 0;
}

/** hsdk on board io init */
void hsdk_io_init(void)
{
	hsdk_led_init();
}
