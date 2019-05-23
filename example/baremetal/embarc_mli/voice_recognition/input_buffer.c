/* ------------------------------------------
 * Copyright (c) 2019, Synopsys, Inc. All rights reserved.

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

#include "embARC.h"
#include "embARC_debug.h"

#include "input_buffer.h"
#include "settings.h"

#include "dw_i2s.h"

#define I2S_RX_DIV             (24576000 / (INPUT_SAMPLERATE_HZ * INPUT_SAMPLE_SIZE * INPUT_CHANNELS_NUM))

/* 
 * Input buffer declaration and settings 
 */
volatile uint32_t read_done;

static volatile uint32_t rx_done_cnt = 0;
static volatile uint32_t err_cnt = 0;
static DEV_I2S_PTR i2s_in_dev;
static DEV_BUFFER dev_buffer;

static void i2s_rx_calllback(void *ptr)
{
    rx_done_cnt++;
    read_done = 1;
}


static void i2s_err_calllback(void *ptr)
{
    err_cnt++;
}


/*
 * Input data management API
 */
void input_buffer_init(void)
{
    uint32_t param;

    // Setting I2S RX reference clock for 16KHz/16bit/stereo input
    i2s_rx_clk_div(I2S_RX_DIV);

	board_delay_ms(200, 0);

    i2s_in_dev = i2s_get_dev(DW_I2S_1_ID);
    dw_i2s_open(i2s_in_dev, DEV_MASTER_MODE, I2S_DEVICE_RECEIVER);

    param = DW_I2S_WSS_16_CLK; // 16 bit data
    dw_i2s_control(i2s_in_dev, I2S_CMD_MST_SET_WSS, (void*)param);
    param = DW_I2S_SCLKG_NO;   // no gating
    dw_i2s_control(i2s_in_dev, I2S_CMD_MST_SET_SCLKG, (void*)param);

    // setting callbacks
    dw_i2s_control(i2s_in_dev, I2S_CMD_SET_RXCB, (void*)i2s_rx_calllback);
    dw_i2s_control(i2s_in_dev, I2S_CMD_SET_ERRCB, (void*)i2s_err_calllback);

    // pre-configure buffer descriptor
    dev_buffer.len = INPUT_FRAME_SIZE * INPUT_CHANNELS_NUM;
    dev_buffer.ofs = 0;
}

static uint32_t started = 0;

void input_buffer_next_read(int16_t* buff)
{
    uint32_t param = DW_I2S_CHANNEL0;

    read_done = 0;

    /* Setting buffer address */
    dev_buffer.buf = buff;
    dw_i2s_control(i2s_in_dev, I2S_CMD_SET_RXINT_BUF, (void*)&dev_buffer);

    if(!started)
    {
        /* Enabling interrupts and starting clock */
        param = 1; 
        dw_i2s_control(i2s_in_dev, I2S_CMD_SET_RXINT, (void*)param);
        dw_i2s_control(i2s_in_dev, I2S_CMD_MST_SET_CLK, (void*)param);
        dw_i2s_control(i2s_in_dev, I2S_CMD_ENA_DEV, (void*)param);
    }
}

