/*

  u8g.h

  Universal 8bit Graphics Library

  Copyright (c) 2011, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/*
 * Modified for port to ARC embARC platform
 * by Wayne Ren wei.ren@synopsys.com, Synopsys, Inc.
 */

#include "u8g.h"

#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "dev_iic.h"

#define OLED_IIC_OBJID		1

#define I2C_SLA			0x3c
#define I2C_CMD_MODE		0x000
#define I2C_DATA_MODE		0x040
#define I2C_CONTINUE_BIT 	0x080

/* Use RESET pin or not */
#define SSD1306_USE_RESET_PIN	1

#if SSD1306_USE_RESET_PIN
#include "dev_gpio.h"

// Port D[5]
#define OLED_RST_GPIO_OBJID	3
#define OLED_RST_PIN_IDX	5
static DEV_GPIO *oled_gpio;

static void oled_ssd1306_hw_reset(void)
{
	oled_gpio = gpio_get_dev(OLED_RST_GPIO_OBJID);
	if (oled_gpio) {
		oled_gpio->gpio_open(1<<OLED_RST_PIN_IDX);
		oled_gpio->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, CONV2VOID(1<<OLED_RST_PIN_IDX));
		// Power off OLED
		oled_gpio->gpio_write(0, 1<<OLED_RST_PIN_IDX);
		u8g_Delay(100);
		// Power on OLED
		oled_gpio->gpio_write(1<<OLED_RST_PIN_IDX, 1<<OLED_RST_PIN_IDX);
		u8g_Delay(5);
	}
}

#define OLED_HW_RESET()			oled_ssd1306_hw_reset()
#else /* No hardware reset */
#define OLED_HW_RESET()
#endif


static DEV_IIC *oled_ssd1306;

static uint8_t data_flag;


Inline int32_t oled_ssd1306_write(uint8_t *buf, uint32_t cnt)
{
	int32_t ercd;

	oled_ssd1306->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));

	ercd = oled_ssd1306->iic_write(buf, cnt);

	return ercd;
}

Inline int32_t oled_ssd1306_write_cmd(uint8_t cmd)
{
	uint8_t cmd_data_pre[2];

	cmd_data_pre[0] = I2C_CMD_MODE;
	cmd_data_pre[1] = cmd;

	return oled_ssd1306_write(cmd_data_pre, 2);
}

Inline int32_t oled_ssd1306_write_data(uint8_t data)
{
	uint8_t cmd_data_pre[2];

	cmd_data_pre[0] = I2C_DATA_MODE;
	cmd_data_pre[1] = data;

	return oled_ssd1306_write(cmd_data_pre, 2);
}

Inline int32_t oled_ssd1306_write_buf(uint8_t *buf, uint8_t cnt)
{
	uint8_t cmd_data_pre[0];
	int32_t ercd;

	cmd_data_pre[0] = I2C_DATA_MODE;

	oled_ssd1306->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	ercd = oled_ssd1306->iic_write(cmd_data_pre, 1);
	oled_ssd1306->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = oled_ssd1306->iic_write(buf, cnt);

	return ercd;
}

uint8_t u8g_com_emsk_ssd_i2c_fn_sel(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr, int32_t devid)
{
	//uint8_t *ptr;
	int32_t ercd;

	switch(msg) {
		case U8G_COM_MSG_INIT:
			OLED_HW_RESET();

			oled_ssd1306 = iic_get_dev(devid);
			ercd = oled_ssd1306->iic_open(DEV_MASTER_MODE, IIC_SPEED_FAST);
			if ((ercd != E_OK) && (ercd != E_OPNED)) {
				return 0;
			}
			oled_ssd1306->iic_control(IIC_CMD_MST_SET_SPEED_MODE, CONV2VOID(IIC_SPEED_FAST));
			oled_ssd1306->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(I2C_SLA));
			break;

		case U8G_COM_MSG_STOP:
			break;

		case U8G_COM_MSG_RESET:
			break;

		case U8G_COM_MSG_CHIP_SELECT:
			break;
		case U8G_COM_MSG_WRITE_BYTE:
			oled_ssd1306_write_cmd(arg_val);
			break;

		case U8G_COM_MSG_WRITE_SEQ:
			// ptr = arg_ptr;
			// while( arg_val--) {
			// 	oled_ssd1306_write_data(*ptr);
			// 	ptr++;
			// }
			oled_ssd1306_write_buf(arg_ptr, arg_val);
			break;
		case U8G_COM_MSG_WRITE_SEQ_P:
			break;
		case U8G_COM_MSG_ADDRESS:
			data_flag = arg_val;/* define cmd (arg_val = 0) or data mode (arg_val = 1) */
			break;
	}
	return 1;
}

uint8_t u8g_com_emsk_ssd_i2c_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
	return u8g_com_emsk_ssd_i2c_fn_sel(u8g, msg, arg_val, arg_ptr, 0);
}

uint8_t u8g_com_emsk_ssd_i2c0_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
	return u8g_com_emsk_ssd_i2c_fn_sel(u8g, msg, arg_val, arg_ptr, 0);
}

uint8_t u8g_com_emsk_ssd_i2c1_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
	return u8g_com_emsk_ssd_i2c_fn_sel(u8g, msg, arg_val, arg_ptr, 1);
}
