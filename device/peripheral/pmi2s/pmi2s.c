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
 * \version 2017.12
 * \date 2017-11-30
 * \author Shuai Wang(Shuai.Wang1@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	BOARD_PERIPHER_DRV_PMODI2S	Peripheral Driver Pmod I2S CS4344
 * \ingroup	BOARD_PERIPHER_DRIVER
 * \brief	Pmod I2S CS4344 peripheral driver
 * \details
 *		Implement driver for Pmod I2S CS4344 using DesignWare I2S driver.
 */

/**
 * \file
 * \ingroup	BOARD_PERIPHER_DRV_PMODI2S
 * \brief	Pmod I2S CS4344 peripheral driver
 */

/**
 * \addtogroup	BOARD_PERIPHER_DRV_PMODI2S
 * @{
 */
#include "pmi2s.h"
#include "embARC_debug.h"

#define I2S_0_EMP_C0 0x0030000
#define I2S_0_SD_C0 0x00d0000
/**
 * \brief Error callback function.
 */
static void i2s_err_isr(void *ptr)
{
	(void)ptr;
	printf("Inter the isr in err!\n");
}

/**
 * \brief Init i2s.
 * \param pdev_i2s.Pointer pointing to the I2S device.
 * \param cs_i2s_init.Structure used to init i2s
 * \param dev.Used to init some parameters related with interrupt,and can be NULL when you do not use interrupt mode.
 * \param i2s_tx_isr.Function pointer pointing to he callback function when all data has been sent.
*/
uint32_t i2s_init_func(DEV_I2S *pdev_i2s,const CS_I2S_INIT *cs_i2s_init,DEV_BUFFER *dev,void (*i2s_isr)(void))
{
	uint32_t div_number;

	DW_I2S_CONFIG *i2s_config_ptr = (DW_I2S_CONFIG *)(pdev_i2s->i2s_info.i2s_config);

	if (pdev_i2s == NULL)
	{
		printf("The i2s init func error!\n");
		return -1;
	}

	pdev_i2s->i2s_info.device = cs_i2s_init->ope_device;
	pdev_i2s->i2s_info.mode = cs_i2s_init->mode;
	i2s_config_ptr->ws_length = cs_i2s_init->num_sclk;
	i2s_config_ptr->data_res[0] = cs_i2s_init->data_format;
	i2s_config_ptr->sample_rate[0] = cs_i2s_init->sample_rate;
	div_number = (MCLK_FREQUENCY_KHZ)/((cs_i2s_init->sample_rate)*(cs_i2s_init->num_sclk*16)*2);
	if(cs_i2s_init->ope_device == I2S_DEVICE_TRANSMITTER)
	{
		i2s_tx_clk_div(div_number);
		/* flush the tx fifo */
		i2s_tx_flush_fifo(pdev_i2s);
		pdev_i2s->i2s_open(DEV_MASTER_MODE, I2S_DEVICE_TRANSMITTER);
	} else {
		i2s_rx_clk_div(div_number);
		/* flush the rx fifo */
		i2s_rx_flush_fifo(pdev_i2s);
		pdev_i2s->i2s_open(DEV_MASTER_MODE, I2S_DEVICE_RECEIVER);
	}
	/* Enable device */
	pdev_i2s->i2s_control(I2S_CMD_ENA_DEV,(void *)0);
	/* Enable master clock */
	pdev_i2s->i2s_control(I2S_CMD_MST_SET_CLK,(void *)1);
	if (cs_i2s_init->pll_isr_sel == I2S_MODE_ISR)
	{
		if (cs_i2s_init->ope_device == I2S_DEVICE_TRANSMITTER)
		{
			/* interrupt related */
			pdev_i2s->i2s_control(I2S_CMD_SET_TXCHET_BUF,(void *)I2S_0_EMP_C0);
			pdev_i2s->i2s_control(I2S_CMD_SET_TXCB,i2s_isr);
			pdev_i2s->i2s_control(I2S_CMD_SET_ERRCB,i2s_err_isr);
			pdev_i2s->i2s_control(I2S_CMD_SET_TXINT_BUF,dev);
			/* Enable interrupt */
			pdev_i2s->i2s_control(I2S_CMD_SET_TXINT,(void *)1);
		} else {
			/* interrupt related */
			pdev_i2s->i2s_control(I2S_CMD_SET_RXCHDT_BUF,(void *)I2S_0_SD_C0);
			pdev_i2s->i2s_control(I2S_CMD_SET_RXCB,i2s_isr);
			pdev_i2s->i2s_control(I2S_CMD_SET_ERRCB,i2s_err_isr);
			pdev_i2s->i2s_control(I2S_CMD_SET_RXINT_BUF,dev);
			/* Enable interrupt */
			pdev_i2s->i2s_control(I2S_CMD_SET_RXINT,(void *)1);
		}
	}
	return E_OK;
}

/**
 * \brief Write data by i2s.
 * \param pdev_i2s.Pointer pointing to the I2S device.
 * \param data.Pointer pointed to the data ready to be sent.
 * \param len.The length of data to be sent.
 * \param channel.0.
*/
uint32_t i2s_write_data(DEV_I2S *pdev_i2s,const void *data,uint32_t len,uint32_t channel)
{
	return dw_i2s_write(pdev_i2s, data, len, channel);
}

/**
 * \brief Read data by i2s.
 * \param pdev_i2s.Pointer pointing to the I2S device.
 * \param data.Pointer pointed to the buffer used to receive data.
 * \param len.The length of data to receive.
 * \param channel.0.
*/
uint32_t i2s_read_data(DEV_I2S *pdev_i2s,void *data,uint32_t len,uint32_t channel)
{
	return dw_i2s_read(pdev_i2s, data, len, channel);
}

/**
 * \brief Flush transmiter fifo
 * \param pdev_i2s.Pointer pointing to the I2S device.
 */
void i2s_tx_flush_fifo(DEV_I2S *pdev_i2s)
{
	pdev_i2s->i2s_control(I2S_CMD_FLUSH_TX,(void *)NULL);
}

/**
 * \brief Flush receive fifo
 * \param pdev_i2s.Pointer pointing to the I2S device.
 */
void i2s_rx_flush_fifo(DEV_I2S *pdev_i2s)
{
	pdev_i2s->i2s_control(I2S_CMD_FLUSH_RX,(void *)NULL);
}
/** @} end of group BOARD_PERIPHER_DRV_PMODI2S */