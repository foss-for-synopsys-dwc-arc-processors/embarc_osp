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
#include "emsdp/emsdp.h"

DEV_SPI_PTR spi_get_dev(int32_t spi_id)
{
	switch(spi_id){
#if (USE_DW_SPI_0)
		case DW_SPI_0_ID:
#endif
#if (USE_DW_SPI_1)
		case DW_SPI_1_ID:
#endif
#if (USE_DW_SPI_2)
		case DW_SPI_2_ID:
#endif
#if (USE_DW_SPI_3)
		case DW_SPI_3_ID:
#endif
			return dw_spi_get_dev(spi_id);
			break;

#if (USE_DFSS_SPI_0)
		case DFSS_SPI_0_ID:
#endif
#if (USE_DFSS_SPI_1)
		case DFSS_SPI_1_ID:
#endif
#if (USE_DFSS_SPI_2)
		case DFSS_SPI_2_ID:
#endif
#if (USE_DFSS_SPI_3)
		case DFSS_SPI_3_ID:
#endif
			return dfss_spi_get_dev(spi_id);
			break;
		default:
			break;
	}
	return NULL;
}

DEV_GPIO_PTR gpio_get_dev(int32_t gpio_id)
{

	switch(gpio_id){
#if (USE_DW_GPIO_PORT_A)
		case DW_GPIO_PORT_A:
#endif
#if (USE_DW_GPIO_PORT_B)
		case DW_GPIO_PORT_B:
#endif
#if (USE_DW_GPIO_PORT_C)
		case DW_GPIO_PORT_C:
#endif
#if (USE_DW_GPIO_PORT_D)
		case DW_GPIO_PORT_D:
#endif
			return dw_gpio_get_dev(gpio_id);
			break;

#if (USE_DFSS_GPIO_0)
		case DFSS_GPIO_0_ID:
#endif
#if (USE_DFSS_GPIO_1)
		case DFSS_GPIO_1_ID:
#endif
#if (USE_DFSS_GPIO_2)
		case DFSS_GPIO_2_ID:
#endif
#if (USE_DFSS_GPIO_3)
		case DFSS_GPIO_3_ID:
#endif
#if (USE_DFSS_GPIO_8B0)
		case DFSS_GPIO_8B0_ID:
#endif
#if (USE_DFSS_GPIO_8B1)
		case DFSS_GPIO_8B1_ID:
#endif
#if (USE_DFSS_GPIO_8B2)
		case DFSS_GPIO_8B2_ID:
#endif
#if (USE_DFSS_GPIO_8B3)
		case DFSS_GPIO_8B3_ID:
#endif
#if (USE_DFSS_GPIO_4B0)
		case DFSS_GPIO_4B0_ID:
#endif
#if (USE_DFSS_GPIO_4B1)
		case DFSS_GPIO_4B1_ID:
#endif
#if (USE_DFSS_GPIO_4B2)
		case DFSS_GPIO_4B2_ID:
#endif
#if (USE_DFSS_GPIO_4B3)
		case DFSS_GPIO_4B3_ID:
#endif
			return dfss_gpio_get_dev(gpio_id);
			break;
		default:
			break;
	}
	return NULL;
}

DEV_UART_PTR uart_get_dev(int32_t uart_id){

	switch(uart_id){
#if (USE_DW_UART_0)
		case DW_UART_0_ID:
#endif
#if (USE_DW_UART_1)
		case DW_UART_1_ID:
#endif
			return dw_uart_get_dev(uart_id);
			break;

#if (USE_DFSS_UART_0)
		case DFSS_UART_0_ID:
#endif
#if (USE_DFSS_UART_1)
		case DFSS_UART_1_ID:
#endif
#if (USE_DFSS_UART_2)
		case DFSS_UART_2_ID:
#endif
#if (USE_DFSS_UART_3)
		case DFSS_UART_3_ID:
#endif
			return dfss_uart_get_dev(uart_id);
			break;
		default:
			break;
	}
	return NULL;
}


DEV_IIC_PTR iic_get_dev(int32_t iic_id)
{
	switch (iic_id) {
#if (USE_DFSS_IIC_0)
		case DFSS_IIC_0_ID:
#endif
#if (USE_DFSS_IIC_1)
		case DFSS_IIC_1_ID:
#endif
#if (USE_DFSS_IIC_2)
		case DFSS_IIC_2_ID:
#endif
			return dfss_iic_get_dev(iic_id);
			break;
		default:
			break;
	}
	return NULL;
}

DEV_I2S_PTR i2s_get_dev(int32_t i2s_id)
{
	switch (i2s_id) {
#if (USE_DFSS_I2S_0)
		case DFSS_I2S_0_ID:
#endif
#if (USE_DFSS_I2S_1)
		case DFSS_I2S_1_ID:
#endif
		case DFSS_NO_I2S:
			return dfss_i2s_get_dev(i2s_id);
			break;
		default:
			break;
	}
	return NULL;
}
