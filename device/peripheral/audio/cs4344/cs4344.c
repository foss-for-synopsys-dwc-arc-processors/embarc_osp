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
#include "cs4344.h"
#include "embARC_debug.h"


#define BOARD_I2S_SEND_IIS_ID			DW_I2S_0_ID
#define BOARD_I2S_RECEIVE_IIS_ID		DW_I2S_1_ID

#define I2S_0_EMP_C0 					0x0080000
#define I2S_1_SD_C0 					0x00d0000

static DEV_I2S *dev_i2s_tx_p = NULL;

/**
 * \brief   The transmiter mode's callback function of interrupt mode.
 * \detail  This function will be callback when the whole data has been send and you have to \
 			resatrt the interrupt and point the data buffer in this function.
*/
void cs4344_tx_isr_restart(DEV_BUFFER *tx_buffer)
{
	dev_i2s_tx_p->i2s_control(I2S_CMD_SET_TXINT_BUF,tx_buffer);
	/* Enable interrupt */
	dev_i2s_tx_p->i2s_control(I2S_CMD_SET_TXINT,(void *)1);
}

/**
 * \brief Error callback function.
 */
void cs4344_err_isr(void *ptr)
{
	(void)ptr;
	printf("Inter the isr in err!\n");
}

/**
 * \brief Init i2s.
 * \param dev_i2s_tx_p.Pointer pointing to the I2S device.
 * \param cs4344_init_str.Structure used to init i2s
 * \param dev.Used to init some parameters related with interrupt,and can be NULL when you do not use interrupt mode.
 * \param i2s_tx_isr.Function pointer pointing to he callback function when all data has been sent.
*/
uint32_t cs4344_init_func(const CS4344_INIT_STR *cs4344_init_str,DEV_BUFFER *dev,void (*i2s_isr)(void))
{
	uint32_t div_number;

	DW_I2S_CONFIG *i2s_config_ptr = (DW_I2S_CONFIG *)(dev_i2s_tx_p->i2s_info.i2s_config);
	if (dev_i2s_tx_p == NULL)
	{
		printf("The i2s init func error!\n");
		return -1;
	}
	dev_i2s_tx_p->i2s_info.device = cs4344_init_str->ope_device;
	dev_i2s_tx_p->i2s_info.mode = cs4344_init_str->mode;
	i2s_config_ptr->ws_length = cs4344_init_str->num_sclk;
	i2s_config_ptr->data_res[0] = cs4344_init_str->data_format;
	i2s_config_ptr->sample_rate[0] = cs4344_init_str->sample_rate;
	div_number = (MCLK_FREQUENCY_KHZ)/((cs4344_init_str->sample_rate)*(cs4344_init_str->num_sclk*16)*2);

	i2s_tx_clk_div(div_number);
	/* flush the tx fifo */
	cs4344_tx_flush_fifo();
	dev_i2s_tx_p->i2s_open(DEV_MASTER_MODE, I2S_DEVICE_TRANSMITTER);

	/* Enable device */
	dev_i2s_tx_p->i2s_control(I2S_CMD_ENA_DEV,(void *)0);
	/* Enable master clock */
	dev_i2s_tx_p->i2s_control(I2S_CMD_MST_SET_CLK,(void *)1);
	if (cs4344_init_str->pll_isr_sel == CS4344_MODE_ISR)
	{
		/* interrupt related */
		dev_i2s_tx_p->i2s_control(I2S_CMD_SET_TXCHET_BUF,(void *)I2S_0_EMP_C0);
		dev_i2s_tx_p->i2s_control(I2S_CMD_SET_TXCB,i2s_isr);
		dev_i2s_tx_p->i2s_control(I2S_CMD_SET_ERRCB,cs4344_err_isr);
		dev_i2s_tx_p->i2s_control(I2S_CMD_SET_TXINT_BUF,dev);
		/* Enable interrupt */
		dev_i2s_tx_p->i2s_control(I2S_CMD_SET_TXINT,(void *)1);
	}
	return E_OK;
}

/**
 * \brief	Init I2S with the structure of CS4344_INIT_STR
 * \param	buffer.The point which is pointed to the buffer used in interrupt mode. \
 			This parameter can be NULL when you use polling mode.
*/
int16_t cs4344_tx_init(uint32_t freq,uint32_t dfmt,uint32_t mode_sel,DEV_BUFFER *buffer,void (*i2s_isr)(void))
{
	CS4344_INIT_STR cs4344_init_str;

	cs4344_init_str.sample_rate = freq;
	cs4344_init_str.data_format = dfmt;
	cs4344_init_str.ope_device = I2S_DEVICE_TRANSMITTER;
	cs4344_init_str.mode = DEV_MASTER_MODE;
	cs4344_init_str.num_sclk = DW_I2S_WSS_32_CLK;
	cs4344_init_str.pll_isr_sel = mode_sel;

	dev_i2s_tx_p = i2s_get_dev(BOARD_I2S_SEND_IIS_ID);
	cs4344_init_func(&cs4344_init_str,buffer,i2s_isr);

	return 0;
}

/**
 * \brief Write data by i2s.
 * \param dev_i2s_tx_p.Pointer pointing to the I2S device.
 * \param data.Pointer pointed to the data ready to be sent.
 * \param len.The length of data to be sent.
 * \param channel.0.
*/
uint32_t cs4344_write_data(const void *data,uint32_t len,uint32_t channel)
{
	return dw_i2s_write(dev_i2s_tx_p, data, len, channel);
}

/**
 * \brief Read data by i2s.
 * \param dev_i2s_tx_p.Pointer pointing to the I2S device.
 * \param data.Pointer pointed to the buffer used to receive data.
 * \param len.The length of data to receive.
 * \param channel.0.
*/
uint32_t cs4344_read_data(void *data,uint32_t len,uint32_t channel)
{
	return dw_i2s_read(dev_i2s_tx_p, data, len, channel);
}

/**
 * \brief Flush transmiter fifo
 * \param dev_i2s_tx_p.Pointer pointing to the I2S device.
 */
void cs4344_tx_flush_fifo(void)
{
	dev_i2s_tx_p->i2s_control(I2S_CMD_FLUSH_TX,(void *)NULL);
}

/**
 * \brief Flush receive fifo
 * \param dev_i2s_tx_p.Pointer pointing to the I2S device.
 */
void cs4344_rx_flush_fifo(void)
{
	dev_i2s_tx_p->i2s_control(I2S_CMD_FLUSH_RX,(void *)NULL);
}
