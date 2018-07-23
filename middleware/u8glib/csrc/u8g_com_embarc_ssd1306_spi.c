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
#include "embARC.h"
#include "dev_gpio.h"
#include "ssd1306_app_config.h"

#ifndef SSD1306_SPI_ID
#define SSD1306_SPI_ID		0
#endif

#ifndef SSD1306_SPI_CS
#define SSD1306_SPI_CS		DEV_SPI_CS_LINE_1
#endif

#ifndef SSD1306_USE_CS_PIN
	#define SSD1306_USE_CS_PIN	1
#endif

#if SSD1306_USE_CS_PIN
# ifndef SSD1306_CS_GPIO
	#define SSD1306_CS_GPIO	(DEV_GPIO_PORT_A)
# endif

# ifndef SSD1306_CS_PIN
	#define SSD1306_CS_PIN		24
# endif
static void oled_ssd1306_chip_select(DEV_SPI* _oled_spi, bool select)
{
	static DEV_GPIO *oled_gpio_cs = NULL;
	if(!oled_gpio_cs) {
		oled_gpio_cs = gpio_get_dev(SSD1306_CS_GPIO);
		if(oled_gpio_cs) {
			oled_gpio_cs->gpio_open(1<<SSD1306_CS_PIN);
			oled_gpio_cs->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, CONV2VOID(1<<SSD1306_CS_PIN));
			oled_gpio_cs->gpio_write(1<<SSD1306_CS_PIN, 1<<SSD1306_CS_PIN);
		}
	}
	if(oled_gpio_cs) {
		if(select) {
			do {
				if(_oled_spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(SSD1306_SPI_CS)) == E_OK) {
					oled_gpio_cs->gpio_write(0, 1<<SSD1306_CS_PIN);
#if defined(OS_FREERTOS)
					taskENTER_CRITICAL();
#endif
					break;
				} else {
#if defined(OS_FREERTOS)
					vTaskDelay(0);
#else
					u8g_Delay(1);
#endif
				}
			} while(1);
		} else {
			_oled_spi->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(SSD1306_SPI_CS));
			oled_gpio_cs->gpio_write(1<<SSD1306_CS_PIN, 1<<SSD1306_CS_PIN);
#if defined(OS_FREERTOS)
			taskEXIT_CRITICAL();
#endif
		}
	}
}
#else
static void oled_ssd1306_chip_select(DEV_SPI* _oled_spi, bool select)
{
	if(select) {
		do {
			if(_oled_spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(SSD1306_SPI_CS)) == E_OK) {
#if defined(OS_FREERTOS)
					taskENTER_CRITICAL();
#endif
				break;
			} else {
#if defined(OS_FREERTOS)
				vTaskDelay(0);
#else
				u8g_Delay(1);
#endif	
			}
		} while(1);
	} else {
		_oled_spi->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(SSD1306_SPI_CS));
#if defined(OS_FREERTOS)	
		taskEXIT_CRITICAL();
#endif
	}
}
#endif

#ifndef SSD1306_SPI_FREQ
#define SSD1306_SPI_FREQ    250000ul
#endif

/* Use RESET pin or not */
#ifndef SSD1306_USE_RST_PIN
	#define SSD1306_USE_RST_PIN	1
#endif

#ifndef SSD1306_CD_GPIO
	#define SSD1306_CD_GPIO		(DEV_GPIO_PORT_A)
#endif

#ifndef SSD1306_CD_PIN
	#define SSD1306_CD_PIN		26
#endif

#if SSD1306_USE_RST_PIN
# ifndef SSD1306_RST_GPIO
	#define SSD1306_RST_GPIO	(DEV_GPIO_PORT_A)
# endif

# ifndef SSD1306_RST_PIN
	#define SSD1306_RST_PIN		25
# endif
static DEV_GPIO *oled_gpio;
static void oled_ssd1306_hw_reset(void)
{
	oled_gpio = gpio_get_dev(SSD1306_RST_GPIO);
	if (oled_gpio) {
		oled_gpio->gpio_open(1<<SSD1306_RST_PIN);
		oled_gpio->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, CONV2VOID(1<<SSD1306_RST_PIN));
		// Power off OLED
		oled_gpio->gpio_write(0, 1<<SSD1306_RST_PIN);
		u8g_Delay(100);
		// Power on OLED
		oled_gpio->gpio_write(1<<SSD1306_RST_PIN, 1<<SSD1306_RST_PIN);
		u8g_Delay(5);
	}
}

#define OLED_HW_RESET()			oled_ssd1306_hw_reset()
#else /* No hardware reset */
#define OLED_HW_RESET()
#endif

static DEV_GPIO *oled_gpio_cd = NULL;
static void oled_ssd1306_write_mode(bool isdata)
{
	if (oled_gpio_cd) {
		if(isdata)
			oled_gpio_cd->gpio_write(1<<SSD1306_CD_PIN, 1<<SSD1306_CD_PIN);
		else
			oled_gpio_cd->gpio_write(0<<SSD1306_CD_PIN, 1<<SSD1306_CD_PIN);
	}
}


static DEV_SPI *oled_ssd1306;
static uint32_t oled_ssd1306_freq = SSD1306_SPI_FREQ;



Inline int32_t oled_ssd1306_write(uint8_t *buf, uint32_t cnt)
{
	return oled_ssd1306->spi_write(buf, cnt);
}

Inline int32_t oled_ssd1306_write_cmd(uint8_t cmd)
{
	return oled_ssd1306_write(&cmd, 1);
}

Inline int32_t oled_ssd1306_write_data(uint8_t data)
{
	return oled_ssd1306_write(&data, 1);
}

Inline int32_t oled_ssd1306_write_buf(uint8_t *buf, uint8_t cnt)
{
	return oled_ssd1306_write(buf, cnt);
}

uint8_t u8g_com_embarc_ssd_spi_fn_sel(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr, int32_t devid)
{
	//uint8_t *ptr;
	int32_t ercd;

	switch(msg) {
		case U8G_COM_MSG_INIT:
			OLED_HW_RESET();
			oled_ssd1306 = spi_get_dev(devid);

			oled_gpio_cd = gpio_get_dev(SSD1306_CD_GPIO);
			if (oled_gpio_cd) {
				oled_gpio_cd->gpio_open(1<<SSD1306_CD_PIN);
				oled_gpio_cd->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, CONV2VOID(1<<SSD1306_CD_PIN));
				oled_gpio_cd->gpio_write(0, 1<<SSD1306_CD_PIN);
			}

			ercd = oled_ssd1306->spi_open(DEV_MASTER_MODE, oled_ssd1306_freq);
			if ((ercd != E_OK) && (ercd != E_OPNED)) {
				return 0;
			}
			oled_ssd1306->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(SPI_CPOL_1_CPHA_1));
			//oled_ssd1306->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(SPI_CPOL_0_CPHA_0));
			break;

		case U8G_COM_MSG_STOP:
			break;

		case U8G_COM_MSG_RESET:
#if SSD1306_USE_RST_PIN
			if(oled_gpio) {
				if(arg_val)
					oled_gpio->gpio_write(1<<SSD1306_RST_PIN, 1<<SSD1306_RST_PIN);
				else
					oled_gpio->gpio_write(0, 1<<SSD1306_RST_PIN);
			}
#endif
			break;

		case U8G_COM_MSG_CHIP_SELECT:
			oled_ssd1306_chip_select(oled_ssd1306, arg_val);
			break;
		case U8G_COM_MSG_WRITE_BYTE:
			oled_ssd1306_write_cmd(arg_val);
			break;

		case U8G_COM_MSG_WRITE_SEQ:
			oled_ssd1306_write_buf(arg_ptr, arg_val);
			break;
		case U8G_COM_MSG_WRITE_SEQ_P:
			break;
		case U8G_COM_MSG_ADDRESS:
			oled_ssd1306_write_mode(arg_val);/* define cmd (arg_val = 0) or data mode (arg_val = 1) */
			break;
		default:
			break;
	}
	return 1;
}

uint8_t u8g_com_embarc_ssd_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
	return u8g_com_embarc_ssd_spi_fn_sel(u8g, msg, arg_val, arg_ptr, SSD1306_SPI_ID);
}