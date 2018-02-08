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
 * \defgroup	EMBARC_APP_BAREMETAL_AUDIO	embARC Audio Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_BAREMETAL
 * \brief	embARC example for audio
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *
 * ### Design Concept
 *
 * ### Usage Manual
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_BAREMETAL_AUDIO
 * \brief	main source file for Pmod audio example
 */

/**
 * \addtogroup	EMBARC_APP_BAREMETAL_AUDIO
 * @{
 */
/* embARC HAL */
#include "embARC.h"
#include "embARC_debug.h"

#include "dw_i2s.h"
#include "dev_common.h"

#define I2S_SAMPLE_FREQ_32 32

static DEV_I2S *dev_i2s_tx_p = NULL,*dev_i2s_rx_p = NULL;
__attribute__((aligned(16))) DEV_BUFFER tx_buffer,rx_buffer;

// A full cycle, 16-bit, 2's complement Sine wave lookup table
static uint16_t sine_table[256] = {
	0x0000, 0x0324, 0x0647, 0x096a, 0x0c8b, 0x0fab, 0x12c8, 0x15e2,
	0x18f8, 0x1c0b, 0x1f19, 0x2223, 0x2528, 0x2826, 0x2b1f, 0x2e11,
	0x30fb, 0x33de, 0x36ba, 0x398c, 0x3c56, 0x3f17, 0x41ce, 0x447a,
	0x471c, 0x49b4, 0x4c3f, 0x4ebf, 0x5133, 0x539b, 0x55f5, 0x5842,
	0x5a82, 0x5cb4, 0x5ed7, 0x60ec, 0x62f2, 0x64e8, 0x66cf, 0x68a6,
	0x6a6d, 0x6c24, 0x6dca, 0x6f5f, 0x70e2, 0x7255, 0x73b5, 0x7504,
	0x7641, 0x776c, 0x7884, 0x798a, 0x7a7d, 0x7b5d, 0x7c29, 0x7ce3,
	0x7d8a, 0x7e1d, 0x7e9d, 0x7f09, 0x7f62, 0x7fa7, 0x7fd8, 0x7ff6,
	0x7fff, 0x7ff6, 0x7fd8, 0x7fa7, 0x7f62, 0x7f09, 0x7e9d, 0x7e1d,
	0x7d8a, 0x7ce3, 0x7c29, 0x7b5d, 0x7a7d, 0x798a, 0x7884, 0x776c,
	0x7641, 0x7504, 0x73b5, 0x7255, 0x70e2, 0x6f5f, 0x6dca, 0x6c24,
	0x6a6d, 0x68a6, 0x66cf, 0x64e8, 0x62f2, 0x60ec, 0x5ed7, 0x5cb4,
	0x5a82, 0x5842, 0x55f5, 0x539b, 0x5133, 0x4ebf, 0x4c3f, 0x49b4,
	0x471c, 0x447a, 0x41ce, 0x3f17, 0x3c56, 0x398c, 0x36ba, 0x33de,
	0x30fb, 0x2e11, 0x2b1f, 0x2826, 0x2528, 0x2223, 0x1f19, 0x1c0b,
	0x18f8, 0x15e2, 0x12c8, 0x0fab, 0x0c8b, 0x096a, 0x0647, 0x0324,
	0x0000, 0xfcdc, 0xf9b9, 0xf696, 0xf375, 0xf055, 0xed38, 0xea1e,
	0xe708, 0xe3f5, 0xe0e7, 0xdddd, 0xdad8, 0xd7da, 0xd4e1, 0xd1ef,
	0xcf05, 0xcc22, 0xc946, 0xc674, 0xc3aa, 0xc0e9, 0xbe32, 0xbb86,
	0xb8e4, 0xb64c, 0xb3c1, 0xb141, 0xaecd, 0xac65, 0xaa0b, 0xa7be,
	0xa57e, 0xa34c, 0xa129, 0x9f14, 0x9d0e, 0x9b18, 0x9931, 0x975a,
	0x9593, 0x93dc, 0x9236, 0x90a1, 0x8f1e, 0x8dab, 0x8c4b, 0x8afc,
	0x89bf, 0x8894, 0x877c, 0x8676, 0x8583, 0x84a3, 0x83d7, 0x831d,
	0x8276, 0x81e3, 0x8163, 0x80f7, 0x809e, 0x8059, 0x8028, 0x800a,
	0x8000, 0x800a, 0x8028, 0x8059, 0x809e, 0x80f7, 0x8163, 0x81e3,
	0x8276, 0x831d, 0x83d7, 0x84a3, 0x8583, 0x8676, 0x877c, 0x8894,
	0x89bf, 0x8afc, 0x8c4b, 0x8dab, 0x8f1e, 0x90a1, 0x9236, 0x93dc,
	0x9593, 0x975a, 0x9931, 0x9b18, 0x9d0e, 0x9f14, 0xa129, 0xa34c,
	0xa57e, 0xa7be, 0xaa0b, 0xac65, 0xaecd, 0xb141, 0xb3c1, 0xb64c,
	0xb8e4, 0xbb86, 0xbe32, 0xc0e9, 0xc3aa, 0xc674, 0xc946, 0xcc22,
	0xcf05, 0xd1ef, 0xd4e1, 0xd7da, 0xdad8, 0xdddd, 0xe0e7, 0xe3f5,
	0xe708, 0xea1e, 0xed38, 0xf055, 0xf375, 0xf696, 0xf9b9, 0xfcdc,
};

/**
 * \brief   The transmiter mode's callback function of interrupt mode.
 * \detail  This function will be callback when the whole data has been send and you have to \
 			resatrt the interrupt and data buffer in this function.
*/
static void i2s_tx_isr()
{
	dev_i2s_tx_p->i2s_control(I2S_CMD_SET_TXINT_BUF,&tx_buffer);
	/* Enable interrupt */
	dev_i2s_tx_p->i2s_control(I2S_CMD_SET_TXINT,(void *)1);
}

/**
 * \brief   The receiver mode's callback function of interrupt mode.
 * \detail  This function will be callback when the whole data has been send and you have to \
         resatrt the interrupt and data buffer in this function.
*/
static void i2s_rx_isr()
{
	printf("Enter rx interrupt!\n");
}

/**
 * \brief	Init I2S with the structure of CS_I2S_INIT
 * \param	buffer.The point which is pointed to the buffer used in interrupt mode. \
 			This parameter can be NULL when you use polling mode.
*/
static int16_t i2s_tx_init(DEV_BUFFER *buffer)
{
	CS_I2S_INIT i2s_init_str;

	i2s_init_str.sample_rate = I2S_SAMPLE_FREQ_32;
	i2s_init_str.data_format = I2S_DATA_FORMAT_16;
	i2s_init_str.ope_device = I2S_DEVICE_TRANSMITTER;
	i2s_init_str.mode = DEV_MASTER_MODE;
	i2s_init_str.num_sclk = DW_I2S_WSS_32_CLK;
	i2s_init_str.pll_isr_sel = I2S_MODE_ISR;
	dev_i2s_tx_p = i2s_get_dev(DW_I2S_0_ID);

	i2s_init_func(dev_i2s_tx_p,&i2s_init_str,buffer,i2s_tx_isr);

	return 0;
}

/**
 * \brief   Init I2S with the structure of CS_I2S_INIT
 * \param   buffer.The point which is pointed to the buffer used in interrupt mode. \
         This parameter can be NULL when you use polling mode.
*/
static int16_t i2s_rx_init(DEV_BUFFER *buffer)
{
	CS_I2S_INIT i2s_init_str;

	i2s_init_str.sample_rate = I2S_SAMPLE_FREQ_32;
	i2s_init_str.data_format = I2S_DATA_FORMAT_16;
	i2s_init_str.ope_device = I2S_DEVICE_RECEIVER;
	i2s_init_str.mode = DEV_MASTER_MODE;
	i2s_init_str.num_sclk = DW_I2S_WSS_32_CLK;
	i2s_init_str.pll_isr_sel = I2S_MODE_ISR;
	dev_i2s_rx_p = i2s_get_dev(DW_I2S_1_ID);

	i2s_init_func(dev_i2s_rx_p,&i2s_init_str,buffer,i2s_rx_isr);

	return 0;
}

int main(void)
{
	tx_buffer.buf=(uint16_t *)sine_table;
	tx_buffer.len=256;
	tx_buffer.ofs=0;
	i2s_tx_init(&tx_buffer);
	while(1)
	{

	}
	return E_SYS;

}

/** @} */
